<?php
declare(strict_types=1);

/**
 * QR code generator: versions 1-9, error correction level M, pure PHP + GD,
 * no external dependencies. Implements ISO/IEC 18004 encoding, Reed-Solomon
 * ECC, and mask pattern selection.
 *
 * Byte mode only, EC level M, versions 1-9 (max 180 data bytes). Pure
 * PHP + GD is a deliberate constraint (portability, no Composer dependency).
 * Self-contained: no dependency on the rest of the JXTOTP/Janox integration.
 *
 * buildMatrix() pipeline: finder patterns -> timing patterns -> alignment
 * patterns -> dark module -> reserve format/version info areas -> encode
 * data + Reed-Solomon ECC -> place data (zigzag) -> try all 8 masks, pick
 * lowest penalty -> apply mask -> write format info -> write version info
 * (v7+ only).
 */
class QrCode
{
    /**
     * Data capacity in bytes per QR version, EC level M.
     */
    private static $CAPACITY_M = [
        1 => 14,  2 => 26,  3 => 42,  4 => 62,  5 => 84,
        6 => 106, 7 => 122, 8 => 152, 9 => 180,
    ];

    /**
     * EC block structure per version, EC level M.
     * Format: [ecPerBlock, g1Count, g1Data, g2Count, g2Data].
     */
    private static $EC_BLOCKS_M = [
        1 => [10, 1, 16, 0,  0],
        2 => [16, 1, 28, 0,  0],
        3 => [26, 1, 44, 0,  0],
        4 => [18, 2, 32, 0,  0],
        5 => [24, 2, 43, 0,  0],
        6 => [16, 4, 27, 0,  0],
        7 => [18, 4, 31, 0,  0],
        8 => [22, 2, 38, 2, 39],
        9 => [22, 3, 36, 2, 37],
    ];

    /**
     * Alignment pattern center coordinates per version (ISO/IEC 18004 table).
     * Centers are the cartesian product of these coordinates; version 1 has none.
     */
    private static $ALIGN_POS = [
        1 => [],
        2 => [6, 18],
        3 => [6, 22],
        4 => [6, 26],
        5 => [6, 30],
        6 => [6, 34],
        7 => [6, 22, 38],
        8 => [6, 24, 42],
        9 => [6, 26, 46],
    ];

    /**
     * Pre-computed 18-bit version info (version number + BCH(18,6) ECC), v7-9 only.
     */
    private static $VERSION_INFO = [7 => 0x07C94, 8 => 0x085BC, 9 => 0x09A99];

    /**
     * Pre-computed 15-bit format info per mask (0-7), EC level M.
     * BCH(15,5), already XORed with the ISO 18004 format mask 101010000010010.
     */
    private static $FORMAT_INFO = [
        0 => 0x5412, 1 => 0x5125, 2 => 0x5E7C, 3 => 0x5B4B,
        4 => 0x45F9, 5 => 0x40CE, 6 => 0x4F97, 7 => 0x4AA0,
    ];

    // GF(256) exp/log tables for Reed-Solomon arithmetic; lazily built once by initGF().
    private static $EXP      = [];
    private static $LOG      = [];
    private static $GF_INIT  = false;

    // ── Public API ─────────────────────────────────────────────────────────────

    /**
     * Generates a QR code image and returns the raw PNG bytes.
     *
     * @param  string $data    Data to encode (typically an otpauth:// URI)
     * @param  int    $scale   Pixels per module (default 6)
     * @param  int    $margin  Quiet-zone width in modules (default 4, per spec minimum)
     * @return string          PNG bytes, ready for base64_encode() or echo
     * @throws \RuntimeException if $data exceeds capacity (180 bytes, EC-M, v9)
     */
    public static function generate(string $data, int $scale = 6, int $margin = 4): string
    {
        self::initGF();

        $version = self::selectVersion($data);
        if ($version === 0) {
            throw new \RuntimeException('Dati troppo lunghi per QR code (max 180 byte con EC Level M).');
        }

        $matrix = self::buildMatrix($data, $version);
        return self::renderPng($matrix, $scale, $margin);
    }

    // ── Version selection ──────────────────────────────────────────────────────

    /**
     * Picks the smallest QR version that fits $data.
     *
     * @return int Version number, or 0 if $data exceeds the largest supported version
     */
    private static function selectVersion(string $data): int
    {
        $len = strlen($data);
        foreach (self::$CAPACITY_M as $v => $cap) {
            if ($len <= $cap) {
                return $v;
            }
        }
        return 0;
    }

    // ── Matrix construction ────────────────────────────────────────────────────

    /**
     * Builds the full QR matrix (2D array).
     *
     * Cell values during construction: -1 = unassigned, 0 = light, 1 = dark.
     * $reserved marks function-pattern cells (finder/timing/alignment/format/
     * version info) that must not be overwritten by data placement.
     */
    private static function buildMatrix(string $data, int $version): array
    {
        $size = $version * 4 + 17; // v1 = 21, +4 per version

        $matrix  = array_fill(0, $size, array_fill(0, $size, -1));
        $reserved = array_fill(0, $size, array_fill(0, $size, false));

        self::placeFinders($matrix, $reserved, $size);
        self::placeTimings($matrix, $reserved, $size);
        self::placeAlignments($matrix, $reserved, $version, $size);
        self::placeDarkModule($matrix, $reserved, $version);
        self::reserveFormat($reserved, $size);
        self::reserveVersionInfo($reserved, $version, $size);

        $codewords = self::encodeData($data, $version);
        self::placeData($matrix, $reserved, $codewords, $size);

        // Mask must be applied before format info is written, since format
        // info encodes the chosen mask number.
        $mask = self::selectMask($matrix, $reserved, $size);
        self::applyMask($matrix, $reserved, $mask, $size);

        self::placeFormatInfo($matrix, $mask, $size);
        self::placeVersionInfo($matrix, $version, $size);

        return $matrix;
    }

    // ── Finder pattern ─────────────────────────────────────────────────────────

    /**
     * Draws the 3 finder patterns at top-left, top-right, and bottom-left
     * (bottom-right corner is left free for data), plus their 1-module quiet
     * zone separator.
     */
    private static function placeFinders(array &$m, array &$r, int $size): void
    {
        $positions = [[0, 0], [0, $size - 7], [$size - 7, 0]];
        foreach ($positions as [$row, $col]) {
            for ($i = 0; $i < 7; $i++) {
                for ($j = 0; $j < 7; $j++) {
                    // Dark ring (i/j = 0 or 6) or dark 3x3 core (i,j in 2..4); light band between.
                    $v = ($i === 0 || $i === 6 || $j === 0 || $j === 6 ||
                         ($i >= 2 && $i <= 4 && $j >= 2 && $j <= 4)) ? 1 : 0;
                    $m[$row + $i][$col + $j] = $v;
                    $r[$row + $i][$col + $j] = true;
                }
            }
            // 9x9 scan around the finder: only fill still-unassigned (-1) cells light,
            // so the separator never overwrites an adjacent finder/timing cell.
            for ($i = -1; $i <= 7; $i++) {
                for ($j = -1; $j <= 7; $j++) {
                    $rr = $row + $i;
                    $cc = $col + $j;
                    if ($rr >= 0 && $rr < $size && $cc >= 0 && $cc < $size && $m[$rr][$cc] === -1) {
                        $m[$rr][$cc] = 0;
                        $r[$rr][$cc] = true;
                    }
                }
            }
        }
    }

    // ── Timing pattern ─────────────────────────────────────────────────────────

    /**
     * Draws the alternating-module timing strips at row 6 and column 6,
     * spanning between the finder patterns.
     */
    private static function placeTimings(array &$m, array &$r, int $size): void
    {
        for ($i = 8; $i < $size - 8; $i++) {
            $v = ($i % 2 === 0) ? 1 : 0; // even index -> dark
            if ($m[6][$i] === -1) { $m[6][$i] = $v; $r[6][$i] = true; }
            if ($m[$i][6] === -1) { $m[$i][6] = $v; $r[$i][6] = true; }
        }
    }

    // ── Alignment pattern ──────────────────────────────────────────────────────

    /**
     * Draws the 5x5 alignment patterns (version 2+) at the cartesian product
     * of ALIGN_POS coordinates, skipping centers that overlap a finder pattern.
     *
     * Centers that fall on a timing strip (row/col 6) are intentionally NOT
     * skipped — the alignment pattern takes priority there, per ISO 18004.
     */
    private static function placeAlignments(array &$m, array &$r, int $version, int $size): void
    {
        $pos = self::$ALIGN_POS[$version];
        if (empty($pos)) {
            return; // version 1 has no alignment patterns
        }

        foreach ($pos as $row) {
            foreach ($pos as $col) {
                // Only finder overlap is excluded; timing-strip overlap is allowed (see above).
                $inTopLeft    = ($row <= 8 && $col <= 8);
                $inTopRight   = ($row <= 8 && $col >= $size - 8);
                $inBottomLeft = ($row >= $size - 8 && $col <= 8);
                if ($inTopLeft || $inTopRight || $inBottomLeft) {
                    continue;
                }

                // Dark ring (|i|=2 or |j|=2), light band, dark center — centered on (row, col).
                for ($i = -2; $i <= 2; $i++) {
                    for ($j = -2; $j <= 2; $j++) {
                        $v = ($i === -2 || $i === 2 || $j === -2 || $j === 2 || ($i === 0 && $j === 0)) ? 1 : 0;
                        $m[$row + $i][$col + $j] = $v;
                        $r[$row + $i][$col + $j] = true;
                    }
                }
            }
        }
    }

    // ── Dark module ────────────────────────────────────────────────────────────

    /**
     * Places the single fixed dark module at (version*4+9, 8), per ISO 18004.
     */
    private static function placeDarkModule(array &$m, array &$r, int $version): void
    {
        $row = $version * 4 + 9;
        $m[$row][8] = 1;
        $r[$row][8] = true;
    }

    // ── Version info area reservation (v7+) ────────────────────────────────────

    /**
     * Reserves the two 6x3 version-info blocks (top-right and bottom-left,
     * v7+ only). Filled in later by placeVersionInfo(), after masking.
     */
    private static function reserveVersionInfo(array &$r, int $version, int $size): void
    {
        if ($version < 7) {
            return;
        }
        // Top-right block: rows 0-5, cols size-11..size-9
        for ($i = 0; $i < 6; $i++) {
            for ($j = $size - 11; $j <= $size - 9; $j++) {
                $r[$i][$j] = true;
            }
        }
        // Bottom-left block (transposed): rows size-11..size-9, cols 0-5
        for ($i = $size - 11; $i <= $size - 9; $i++) {
            for ($j = 0; $j < 6; $j++) {
                $r[$i][$j] = true;
            }
        }
    }

    // ── Version info placement (v7+, post-mask) ────────────────────────────────

    /**
     * Writes the 18-bit version info into the reserved zones (v7+ only).
     */
    private static function placeVersionInfo(array &$m, int $version, int $size): void
    {
        if ($version < 7 || !isset(self::$VERSION_INFO[$version])) {
            return;
        }
        $val = self::$VERSION_INFO[$version];
        for ($i = 0; $i < 18; $i++) {
            $bit = ($val >> $i) & 1;
            $row = (int) floor($i / 3); // bit i -> row i/3, col i%3 within the 6x3 block
            $col = $i % 3;
            $m[$row][$size - 11 + $col] = $bit; // top-right copy
            $m[$size - 11 + $col][$row] = $bit; // bottom-left copy (transposed)
        }
    }

    // ── Format info area reservation ────────────────────────────────────────────

    /**
     * Reserves the format-info cells (two copies, marked here and valued
     * later by placeFormatInfo() once the mask number is known).
     */
    private static function reserveFormat(array &$r, int $size): void
    {
        // Primary copy: L-shape around the top-left finder.
        for ($j = 0; $j <= 8; $j++) { $r[8][$j] = true; }  // row 8, cols 0-8
        for ($i = 0; $i <= 8; $i++) { $r[$i][8] = true; }  // col 8, rows 0-8

        // Secondary copy: strip next to the bottom-left finder
        for ($i = $size - 8; $i < $size; $i++) { $r[$i][8] = true; }

        // Secondary copy: strip next to the top-right finder
        for ($j = $size - 8; $j < $size; $j++) { $r[8][$j] = true; }
    }

    // ── Data encoding + ECC ─────────────────────────────────────────────────────

    /**
     * Encodes $data into the final interleaved codeword array: builds the
     * byte-mode bitstream, splits into blocks, computes Reed-Solomon ECC per
     * block, then interleaves data and ECC codewords (data byte 0 of every
     * block, then byte 1, etc.) so burst errors spread across blocks instead
     * of concentrating in one.
     *
     * @return int[] Codewords (0-255)
     */
    private static function encodeData(string $data, int $version): array
    {
        list($ecPerBlock, $g1Count, $g1Data, $g2Count, $g2Data) = self::$EC_BLOCKS_M[$version];
        $totalData = $g1Count * $g1Data + $g2Count * $g2Data;

        // ── Step 1: build the bitstream ─────────────────────────────────────────
        $bits = '';
        $bits .= '0100';                                 // mode indicator: byte mode
        $bits .= self::intToBits(strlen($data), 8);     // character count indicator (8 bit for v1-9)
        for ($i = 0; $i < strlen($data); $i++) {
            $bits .= self::intToBits(ord($data[$i]), 8);
        }
        $bits .= '0000';                                 // terminator

        while (strlen($bits) % 8 !== 0) {
            $bits .= '0';
        }

        // Pad with the two fixed spec bytes (0xEC, 0x11), alternating, up to capacity.
        $padBytes = ['11101100', '00010001'];
        $pi = 0;
        while (strlen($bits) < $totalData * 8) {
            $bits .= $padBytes[$pi % 2];
            $pi++;
        }

        $dataWords = [];
        for ($i = 0; $i < $totalData; $i++) {
            $dataWords[] = bindec(substr($bits, $i * 8, 8));
        }

        // ── Step 2: split into blocks, compute Reed-Solomon ECC per block ───────
        $blocks   = [];
        $ecBlocks = [];
        $idx = 0;
        for ($b = 0; $b < $g1Count; $b++) {
            $block = array_slice($dataWords, $idx, $g1Data);
            $idx  += $g1Data;
            $blocks[]   = $block;
            $ecBlocks[] = self::rsBlock($block, $ecPerBlock);
        }
        for ($b = 0; $b < $g2Count; $b++) {
            $block = array_slice($dataWords, $idx, $g2Data);
            $idx  += $g2Data;
            $blocks[]   = $block;
            $ecBlocks[] = self::rsBlock($block, $ecPerBlock);
        }

        // ── Step 3: interleave data, then interleave ECC ─────────────────────────
        $out     = [];
        $maxData = max($g1Data, $g2Data);
        for ($col = 0; $col < $maxData; $col++) {
            foreach ($blocks as $block) {
                if (isset($block[$col])) {
                    $out[] = $block[$col];
                }
            }
        }
        for ($col = 0; $col < $ecPerBlock; $col++) {
            foreach ($ecBlocks as $ec) {
                $out[] = $ec[$col];
            }
        }

        return $out;
    }

    // ── Data placement ──────────────────────────────────────────────────────────

    /**
     * Writes codeword bits into the matrix along the standard zigzag path:
     * starting bottom-right, moving up in 2-column pairs, reversing direction
     * each pair. Reserved and already-assigned cells are skipped automatically.
     */
    private static function placeData(array &$m, array $r, array $codewords, int $size): void
    {
        $bits = '';
        foreach ($codewords as $cw) {
            $bits .= str_pad(decbin($cw), 8, '0', STR_PAD_LEFT);
        }

        $bitIdx = 0;
        $upward = true;       // current scan direction for this column pair
        $col    = $size - 1;

        while ($col > 0) {
            if ($col === 6) {
                $col--; // column 6 is the vertical timing strip — skip it entirely
            }

            for ($delta = 0; $delta < $size; $delta++) {
                $row = $upward ? ($size - 1 - $delta) : $delta;
                for ($c = 0; $c < 2; $c++) {
                    $cc = $col - $c;
                    if (!$r[$row][$cc] && $m[$row][$cc] === -1) {
                        $m[$row][$cc] = ($bitIdx < strlen($bits) && $bits[$bitIdx] === '1') ? 1 : 0;
                        $bitIdx++;
                    }
                }
            }

            $upward = !$upward;
            $col   -= 2;
        }
    }

    // ── Mask selection and application ─────────────────────────────────────────

    /**
     * Tries all 8 mask patterns and returns the one with the lowest penalty
     * score (calcPenalty(), per ISO 18004).
     */
    private static function selectMask(array $matrix, array $reserved, int $size): int
    {
        $best      = PHP_INT_MAX;
        $bestMask  = 0;

        for ($mask = 0; $mask < 8; $mask++) {
            $m = $matrix; // copy: applyMask() mutates in place
            self::applyMask($m, $reserved, $mask, $size);
            $penalty = self::calcPenalty($m, $size);
            if ($penalty < $best) {
                $best     = $penalty;
                $bestMask = $mask;
            }
        }

        return $bestMask;
    }

    /**
     * Applies $mask to the matrix: XORs every non-reserved data cell where
     * maskCondition() is true.
     */
    private static function applyMask(array &$m, array $r, int $mask, int $size): void
    {
        for ($i = 0; $i < $size; $i++) {
            for ($j = 0; $j < $size; $j++) {
                if ($r[$i][$j]) {
                    continue; // never mask function-pattern cells
                }
                if (self::maskCondition($mask, $i, $j)) {
                    $m[$i][$j] ^= 1;
                }
            }
        }
    }

    /**
     * The 8 mask condition functions (ISO 18004, table 10).
     *
     * @return bool True if the module at ($i, $j) should be inverted
     */
    private static function maskCondition(int $mask, int $i, int $j): bool
    {
        switch ($mask) {
            case 0: return ($i + $j) % 2 === 0;
            case 1: return $i % 2 === 0;
            case 2: return $j % 3 === 0;
            case 3: return ($i + $j) % 3 === 0;
            case 4: return ((int)($i / 2) + (int)($j / 3)) % 2 === 0;
            case 5: return ($i * $j) % 2 + ($i * $j) % 3 === 0;
            case 6: return (($i * $j) % 2 + ($i * $j) % 3) % 2 === 0;
            case 7: return (($i + $j) % 2 + ($i * $j) % 3) % 2 === 0;
        }
        return false;
    }

    // ── Penalty scoring ─────────────────────────────────────────────────────────

    /**
     * Computes the ISO 18004 penalty score for a masked matrix (lower is better).
     *
     * Rule 1: runs of 5+ identical modules (+3, +1 per extra module).
     * Rule 2: 2x2 blocks of identical modules (+3 each).
     * Rule 3: finder-like patterns [1,0,1,1,1,0,1,0,0,0,0] or its mirror (+40 each).
     * Rule 4: dark-module ratio deviation from 50% (+10 per 5% step).
     */
    private static function calcPenalty(array $m, int $size): int
    {
        $score = 0;

        // Rule 1: runs of 5+ identical modules, rows and columns
        for ($i = 0; $i < $size; $i++) {
            foreach ([true, false] as $isRow) {
                $run = 1;
                for ($j = 1; $j < $size; $j++) {
                    $cur  = ($isRow ? $m[$i][$j]     : $m[$j][$i])     < 1 ? 0 : 1;
                    $prev = ($isRow ? $m[$i][$j - 1] : $m[$j - 1][$i]) < 1 ? 0 : 1;
                    if ($cur === $prev) {
                        $run++;
                    } else {
                        if ($run >= 5) { $score += 3 + ($run - 5); }
                        $run = 1;
                    }
                }
                if ($run >= 5) { $score += 3 + ($run - 5); }
            }
        }

        // Rule 2: 2x2 blocks of identical modules
        for ($i = 0; $i < $size - 1; $i++) {
            for ($j = 0; $j < $size - 1; $j++) {
                $v = $m[$i][$j]     < 1 ? 0 : 1;
                $a = $m[$i][$j + 1] < 1 ? 0 : 1;
                $b = $m[$i + 1][$j] < 1 ? 0 : 1;
                $c = $m[$i + 1][$j + 1] < 1 ? 0 : 1;
                if ($v === $a && $v === $b && $v === $c) {
                    $score += 3;
                }
            }
        }

        // Rule 3: finder-like border patterns
        $p1 = [1,0,1,1,1,0,1,0,0,0,0]; // 10111010000
        $p2 = [0,0,0,0,1,0,1,1,1,0,1]; // 00001011101 (mirror of p1)
        for ($i = 0; $i < $size; $i++) {
            for ($j = 0; $j <= $size - 11; $j++) {
                $matchR1 = $matchR2 = $matchC1 = $matchC2 = true;
                for ($k = 0; $k < 11; $k++) {
                    $mr = $m[$i][$j + $k]     < 1 ? 0 : 1;
                    $mc = $m[$j + $k][$i]     < 1 ? 0 : 1;
                    if ($mr !== $p1[$k]) { $matchR1 = false; }
                    if ($mr !== $p2[$k]) { $matchR2 = false; }
                    if ($mc !== $p1[$k]) { $matchC1 = false; }
                    if ($mc !== $p2[$k]) { $matchC2 = false; }
                }
                if ($matchR1) { $score += 40; }
                if ($matchR2) { $score += 40; }
                if ($matchC1) { $score += 40; }
                if ($matchC2) { $score += 40; }
            }
        }

        // Rule 4: dark module ratio
        $dark  = 0;
        $total = $size * $size;
        for ($i = 0; $i < $size; $i++) {
            for ($j = 0; $j < $size; $j++) {
                if ($m[$i][$j] === 1) { $dark++; }
            }
        }
        $pct   = (int) round($dark / $total * 100);
        $score += 10 * (int) floor(abs($pct - 50) / 5);

        return $score;
    }

    // ── Format information ──────────────────────────────────────────────────────

    /**
     * Writes the 15-bit format info into its two reserved copies (redundant,
     * so the code remains readable if one corner is damaged).
     */
    private static function placeFormatInfo(array &$m, int $mask, int $size): void
    {
        $fmt  = self::$FORMAT_INFO[$mask];
        $bits = [];
        for ($i = 14; $i >= 0; $i--) {
            $bits[] = ($fmt >> $i) & 1; // MSB (bit 14) first
        }

        // Primary copy: fixed L-shaped path around the top-left finder (ISO 18004)
        $seqRow = [8,8,8,8,8,8,8,8,7,5,4,3,2,1,0];
        $seqCol = [0,1,2,3,4,5,7,8,8,8,8,8,8,8,8];
        for ($i = 0; $i < 15; $i++) {
            $m[$seqRow[$i]][$seqCol[$i]] = $bits[$i];
        }

        // Secondary copy: split across the bottom-left and top-right finders
        for ($i = 0; $i < 7; $i++) {
            $m[$size - 1 - $i][8] = $bits[$i];
        }
        for ($i = 7; $i < 15; $i++) {
            $m[8][$size - 15 + $i] = $bits[$i];
        }
    }

    // ── Reed-Solomon ECC ─────────────────────────────────────────────────────────

    /**
     * Builds the GF(256) exp/log tables (primitive element alpha=2, primitive
     * polynomial x^8+x^4+x^3+x^2+1 = 0x11D), idempotent via GF_INIT.
     */
    private static function initGF(): void
    {
        if (self::$GF_INIT) {
            return;
        }
        self::$EXP = array_fill(0, 512, 0);
        self::$LOG = array_fill(0, 256, 0);
        $x = 1;
        for ($i = 0; $i < 255; $i++) {
            self::$EXP[$i] = $x;
            self::$LOG[$x] = $i;
            $x <<= 1;
            if ($x & 0x100) {
                $x ^= 0x11D; // reduce modulo the primitive polynomial
            }
        }
        // Duplicate 0-254 into 255-509 so gfMul() can index EXP[(logA+logB) % 255]
        // without a separate modulo branch.
        for ($i = 255; $i < 512; $i++) {
            self::$EXP[$i] = self::$EXP[$i - 255];
        }
        self::$GF_INIT = true;
    }

    /**
     * GF(256) multiplication: a * b = alpha^(log(a) + log(b)); 0 if either operand is 0.
     */
    private static function gfMul(int $a, int $b): int
    {
        if ($a === 0 || $b === 0) {
            return 0;
        }
        return self::$EXP[(self::$LOG[$a] + self::$LOG[$b]) % 255];
    }

    /**
     * Computes the Reed-Solomon generator polynomial of the given degree:
     * (x - alpha^0)(x - alpha^1)...(x - alpha^(degree-1)) in GF(256).
     *
     * @return int[] Coefficients, highest degree first
     */
    private static function rsGeneratorPoly(int $degree): array
    {
        $poly = [1];
        for ($i = 0; $i < $degree; $i++) {
            $alpha = self::$EXP[$i];
            $new   = array_fill(0, count($poly) + 1, 0);
            foreach ($poly as $j => $coef) {
                $new[$j]     ^= $coef;                        // GF addition is XOR
                $new[$j + 1] ^= self::gfMul($coef, $alpha);
            }
            $poly = $new;
        }
        return $poly;
    }

    /**
     * Computes Reed-Solomon ECC codewords for a data block via synthetic
     * polynomial division: the message (shifted left by $ecCount, i.e.
     * padded with zeros) is repeatedly XORed against the generator
     * polynomial scaled by the current leading coefficient; the remainder
     * is the ECC.
     *
     * @param  int[] $data    Data codewords of the block (0-255)
     * @param  int   $ecCount Number of ECC codewords to generate
     * @return int[]          ECC codewords (0-255)
     */
    private static function rsBlock(array $data, int $ecCount): array
    {
        $gen = self::rsGeneratorPoly($ecCount);
        $msg = array_merge($data, array_fill(0, $ecCount, 0));

        for ($i = 0; $i < count($data); $i++) {
            $coef = $msg[$i];
            if ($coef !== 0) {
                for ($j = 1; $j <= $ecCount; $j++) {
                    $msg[$i + $j] ^= self::gfMul($gen[$j], $coef);
                }
            }
        }

        return array_slice($msg, count($data)); // remainder = ECC codewords
    }

    // ── Helper ───────────────────────────────────────────────────────────────────

    /**
     * Converts an integer to a zero-padded binary string of $length bits.
     * Example: intToBits(5, 4) -> "0101"
     */
    private static function intToBits(int $value, int $length): string
    {
        return str_pad(decbin($value), $length, '0', STR_PAD_LEFT);
    }

    // ── PNG rendering via GD ───────────────────────────────────────────────────

    /**
     * Rasterizes the QR matrix to a PNG image via GD.
     *
     * Each dark module is drawn as a filled $scale x $scale rectangle; the
     * quiet zone ($margin modules) surrounds the grid.
     */
    private static function renderPng(array $matrix, int $scale, int $margin): string
    {
        $size    = count($matrix);
        $imgSize = ($size + 2 * $margin) * $scale;
        $img     = imagecreatetruecolor($imgSize, $imgSize);
        $white   = imagecolorallocate($img, 255, 255, 255);
        $black   = imagecolorallocate($img, 0, 0, 0);

        imagefill($img, 0, 0, $white);

        for ($row = 0; $row < $size; $row++) {
            for ($col = 0; $col < $size; $col++) {
                if ($matrix[$row][$col] === 1) {
                    $x1 = ($col + $margin) * $scale;
                    $y1 = ($row + $margin) * $scale;
                    imagefilledrectangle($img, $x1, $y1, $x1 + $scale - 1, $y1 + $scale - 1, $black);
                }
            }
        }

        // imagepng() writes to stdout, not a string — capture it via the output buffer.
        ob_start();
        imagepng($img);
        $png = ob_get_clean();
        imagedestroy($img);

        return $png;
    }
}
