<?php
declare(strict_types=1);

/**
 * Generatore QR code — puro PHP + estensione GD.
 *
 * Supporta: byte mode, EC level M, versioni 1-9 (fino a 180 byte di dati).
 * File isolato: sostituibile senza toccare nulla al di fuori di questa classe.
 *
 * NOTA (modulo JXTOTP): file invariato rispetto al prototipo standalone —
 * la generazione del QR code non ha nulla di specifico per l'integrazione
 * Janox/ERP.
 *
 * ═══════════════════════════════════════════════════════════
 * COME È FATTO UN QR CODE — PANORAMICA PER JUNIOR DEVELOPER
 * ═══════════════════════════════════════════════════════════
 *
 * Un QR code è una griglia quadrata di moduli (quadratini) neri e bianchi.
 * La dimensione della griglia dipende dalla "versione": versione 1 = 21×21 moduli,
 * versione 2 = 25×25, ..., ogni versione aggiunge 4 moduli per lato.
 *
 * La griglia è divisa in zone "funzionali" (riservate al decodificatore) e
 * zone "dati" (dove i bit dei dati vengono scritti):
 *
 *   ┌──────────────────────────────────────────┐
 *   │ [FINDER] · · · · · · · · · [FINDER]      │  ← 3 quadrati agli angoli
 *   │                                          │
 *   │ · · TIMING STRIPS · · · · · ·            │  ← due righe/colonne a scacchi
 *   │         [ALIGN]                          │  ← piccoli quadrati interni (v2+)
 *   │                                          │
 *   │   D A T I   D A T I   D A T I           │  ← bit scritti in zigzag
 *   │                                          │
 *   │ [FINDER]                                 │
 *   └──────────────────────────────────────────┘
 *
 * Pipeline di costruzione (buildMatrix):
 *   1. Disegna i 3 finder pattern (angoli)
 *   2. Disegna le timing strip (righe/colonne 6)
 *   3. Disegna gli alignment pattern (versione 2+)
 *   4. Disegna il dark module
 *   5. Riserva le aree per format info e version info
 *   6. Codifica i dati + Reed-Solomon ECC → array di codeword
 *   7. Scrivi le codeword nella griglia (pattern zigzag)
 *   8. Prova tutte le 8 maschere, scegli quella con penalità minima
 *   9. Applica la maschera migliore
 *  10. Scrivi il format info (EC level + numero maschera) nelle aree riservate
 *  11. Scrivi il version info (solo per versioni 7+)
 */
class QrCode
{
    /**
     * Capacità in byte per versione QR, EC level M.
     * Esempio: versione 6 può contenere fino a 106 byte di dati.
     * La versione minima che contiene i dati viene scelta automaticamente in selectVersion().
     */
    private static $CAPACITY_M = [
        1 => 14,  2 => 26,  3 => 42,  4 => 62,  5 => 84,
        6 => 106, 7 => 122, 8 => 152, 9 => 180,
    ];

    /**
     * Struttura dei blocchi di correzione errori per EC level M.
     *
     * Formato: [ec_per_block, g1_count, g1_data, g2_count, g2_data]
     *
     * I dati vengono divisi in blocchi, ognuno con il proprio set di codeword EC
     * (Error Correction). Versioni più grandi richiedono più blocchi per distribuire
     * il carico di ECC.
     *
     * Esempio versione 7: [18, 4, 31, 0, 0]
     *   → 4 blocchi (gruppo 1), ognuno con 31 codeword dati e 18 codeword EC
     *   → 0 blocchi gruppo 2
     *   → totale dati: 4×31 = 124 codeword (ma EC level M → 122 byte netti)
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
     * Coordinate dei centri degli alignment pattern per ogni versione.
     *
     * Gli alignment pattern sono piccoli quadrati 5×5 aggiuntivi che aiutano il
     * decodificatore a correggere la distorsione prospettica dell'immagine.
     * Versione 1 non li ha. Da versione 2 in poi vengono posizionati alle
     * intersezioni delle coordinate elencate qui.
     *
     * Esempio versione 7: coordinate [6, 22, 38] → 3×3 = 9 posizioni possibili,
     * ma 3 vengono saltate perché coinciderebbero con i finder pattern negli angoli.
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
     * Version information per versioni 7-9 (pre-calcolato).
     *
     * Dalle versione 7 in poi, la griglia include 18 bit che codificano il numero
     * di versione con correzione errori BCH(18,6). Questi 18 bit sono pre-calcolati
     * secondo lo standard ISO 18004 e scritti in due blocchi 6×3 simmetrici:
     * uno nell'angolo in alto a destra, uno in basso a sinistra.
     */
    private static $VERSION_INFO = [7 => 0x07C94, 8 => 0x085BC, 9 => 0x09A99];

    /**
     * Format information per EC level M, maschere 0-7 (pre-calcolati).
     *
     * 15 bit che codificano: EC level (2 bit) + numero maschera (3 bit) + BCH ECC (10 bit).
     * Già applicato XOR con la maschera di formato 101010000010010 (standard ISO 18004).
     * Vengono scritti in due copie nella griglia per ridondanza.
     *
     * Questi valori sono stati calcolati con BCH(15,5) generatore polinomiale 10100110111.
     * EC level M = 00 in binario.
     */
    private static $FORMAT_INFO = [
        0 => 0x5412, 1 => 0x5125, 2 => 0x5E7C, 3 => 0x5B4B,
        4 => 0x45F9, 5 => 0x40CE, 6 => 0x4F97, 7 => 0x4AA0,
    ];

    // Tabelle per l'aritmetica in GF(256) (Galois Field), necessaria per Reed-Solomon ECC.
    // Vengono inizializzate una sola volta da initGF().
    private static $EXP      = [];
    private static $LOG      = [];
    private static $GF_INIT  = false;

    // ── API pubblica ───────────────────────────────────────────────────────────

    /**
     * Genera l'immagine QR code e restituisce i byte PNG.
     *
     * @param  string $data    Stringa da codificare (tipicamente un URI otpauth://)
     * @param  int    $scale   Pixel per modulo QR (default 6 → ogni modulo è 6×6 pixel)
     * @param  int    $margin  Moduli di bordo bianco attorno al QR (default 4, minimo consigliato)
     * @return string          Byte dell'immagine PNG, pronti per base64_encode() o echo
     * @throws \RuntimeException se i dati superano la capacità massima (180 byte, EC-M, v9)
     */
    public static function generate(string $data, int $scale = 6, int $margin = 4): string
    {
        // Le tabelle GF(256) sono costose da calcolare: vengono create solo una volta
        // e riutilizzate per tutte le chiamate successive (flag GF_INIT).
        self::initGF();

        $version = self::selectVersion($data);
        if ($version === 0) {
            throw new \RuntimeException('Dati troppo lunghi per QR code (max 180 byte con EC Level M).');
        }

        $matrix = self::buildMatrix($data, $version);
        return self::renderPng($matrix, $scale, $margin);
    }

    // ── Selezione versione ─────────────────────────────────────────────────────

    /**
     * Sceglie la versione QR più piccola in grado di contenere i dati.
     *
     * Versioni più basse = griglia più piccola = QR code più facile da scansionare.
     * Restituisce 0 se nessuna versione supportata è sufficiente.
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

    // ── Costruzione matrice ────────────────────────────────────────────────────

    /**
     * Costruisce la matrice QR completa (array 2D di 0 e 1).
     *
     * La matrice usa tre valori interni durante la costruzione:
     *   -1 = modulo non ancora assegnato
     *    0 = bianco
     *    1 = nero
     *
     * La matrice $reserved tiene traccia di quali celle sono "funzionali"
     * (riservate a finder, timing, alignment, format, version info) e quindi
     * non devono essere sovrascritte dai dati.
     */
    private static function buildMatrix(string $data, int $version): array
    {
        // La dimensione della griglia: versione 1 = 21, versione 2 = 25, ecc.
        $size = $version * 4 + 17;

        // Inizializza la matrice a -1 (nessun modulo assegnato) e la mappa dei moduli riservati.
        $matrix  = array_fill(0, $size, array_fill(0, $size, -1));
        $reserved = array_fill(0, $size, array_fill(0, $size, false));

        // Step 1-5: posizionamento dei pattern funzionali e riserva delle aree speciali.
        self::placeFinders($matrix, $reserved, $size);
        self::placeTimings($matrix, $reserved, $size);
        self::placeAlignments($matrix, $reserved, $version, $size);
        self::placeDarkModule($matrix, $reserved, $version);
        self::reserveFormat($reserved, $size);
        self::reserveVersionInfo($reserved, $version, $size);

        // Step 6-7: codifica i dati (con ECC) e scrivili nella matrice.
        $codewords = self::encodeData($data, $version);
        self::placeData($matrix, $reserved, $codewords, $size);

        // Step 8-9: scegli la maschera migliore e applicala.
        // La maschera deve essere applicata PRIMA di scrivere il format info
        // perché il format info include il numero della maschera scelta.
        $mask = self::selectMask($matrix, $reserved, $size);
        self::applyMask($matrix, $reserved, $mask, $size);

        // Step 10-11: scrivi le informazioni di formato e versione (nelle celle riservate).
        self::placeFormatInfo($matrix, $mask, $size);
        self::placeVersionInfo($matrix, $version, $size);

        return $matrix;
    }

    // ── Finder pattern ─────────────────────────────────────────────────────────

    /**
     * Disegna i 3 finder pattern (i quadrati caratteristici agli angoli del QR code).
     *
     * Ogni finder pattern è un quadrato 7×7 con struttura concentrica: bordo esterno nero,
     * fascia bianca, quadrato centrale 3×3 nero. Il decodificatore li cerca per determinare
     * la posizione, dimensione e orientamento del QR code nell'immagine fotografata.
     *
     * I tre pattern si trovano in: angolo in alto a sinistra (0,0), in alto a destra (0, size-7),
     * in basso a sinistra (size-7, 0). L'angolo in basso a destra è lasciato libero per i dati.
     *
     * Attorno a ogni finder viene aggiunto un separatore di 1 modulo bianco che isola
     * visivamente il pattern dalla zona dati.
     */
    private static function placeFinders(array &$m, array &$r, int $size): void
    {
        $positions = [[0, 0], [0, $size - 7], [$size - 7, 0]];
        foreach ($positions as [$row, $col]) {
            for ($i = 0; $i < 7; $i++) {
                for ($j = 0; $j < 7; $j++) {
                    // 1 (nero) se siamo sul bordo esterno (i o j = 0 o 6)
                    // oppure nel quadrato centrale (i e j tra 2 e 4).
                    // 0 (bianco) nella fascia intermedia.
                    $v = ($i === 0 || $i === 6 || $j === 0 || $j === 6 ||
                         ($i >= 2 && $i <= 4 && $j >= 2 && $j <= 4)) ? 1 : 0;
                    $m[$row + $i][$col + $j] = $v;
                    $r[$row + $i][$col + $j] = true;
                }
            }
            // Separatore: bordo bianco 1 modulo attorno al finder.
            // Iteriamo su un'area 9×9 e settiamo a bianco solo le celle -1 (non ancora assegnate).
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
     * Disegna le due "timing strip": una riga orizzontale (riga 6) e una colonna
     * verticale (colonna 6) che alternano moduli neri e bianchi.
     *
     * Le timing strip permettono al decodificatore di calcolare la dimensione esatta
     * di un modulo e di correggere eventuali distorsioni nell'immagine.
     * Partono dalla fine dell'area del finder (modulo 8) e arrivano all'inizio
     * del finder sull'altro lato (modulo size-8).
     */
    private static function placeTimings(array &$m, array &$r, int $size): void
    {
        for ($i = 8; $i < $size - 8; $i++) {
            $v = ($i % 2 === 0) ? 1 : 0; // alterno: 8=nero, 9=bianco, 10=nero, ...
            if ($m[6][$i] === -1) { $m[6][$i] = $v; $r[6][$i] = true; }
            if ($m[$i][6] === -1) { $m[$i][6] = $v; $r[$i][6] = true; }
        }
    }

    // ── Alignment pattern ──────────────────────────────────────────────────────

    /**
     * Disegna gli alignment pattern (quadrati 5×5 aggiuntivi per versioni 2+).
     *
     * Gli alignment pattern hanno struttura simile ai finder ma in scala ridotta:
     * bordo esterno nero, fascia bianca, modulo centrale nero.
     * Aiutano il decodificatore a correggere la curvatura o la distorsione prospettica.
     *
     * Le posizioni vengono calcolate come prodotto cartesiano delle coordinate in ALIGN_POS:
     * per versione 7 con coordinate [6, 22, 38] ci sarebbero 9 combinazioni,
     * ma 3 vengono saltate perché il centro cadrebbe nell'area occupata dai finder pattern
     * (angoli in alto a sinistra, in alto a destra, in basso a sinistra).
     *
     * NOTA: NON saltare i pattern che cadono sulle timing strip (riga/colonna 6).
     * In quel caso il pattern di allineamento ha priorità e sovrascrive la timing strip.
     * Questo è un comportamento previsto dallo standard ISO 18004.
     */
    private static function placeAlignments(array &$m, array &$r, int $version, int $size): void
    {
        $pos = self::$ALIGN_POS[$version];
        if (empty($pos)) {
            return; // versione 1: nessun alignment pattern
        }

        foreach ($pos as $row) {
            foreach ($pos as $col) {
                // Salta solo se il centro cade nell'area dei 3 finder pattern.
                // Un centro che cade su una timing strip viene invece scritto normalmente.
                $inTopLeft    = ($row <= 8 && $col <= 8);
                $inTopRight   = ($row <= 8 && $col >= $size - 8);
                $inBottomLeft = ($row >= $size - 8 && $col <= 8);
                if ($inTopLeft || $inTopRight || $inBottomLeft) {
                    continue;
                }

                // Disegna il pattern 5×5 centrato su ($row, $col).
                // Bordo esterno (|i|=2 o |j|=2) = nero, fascia interna = bianco, centro = nero.
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
     * Posiziona il "dark module": un singolo modulo nero la cui posizione è fissa
     * per ogni versione (riga version*4+9, colonna 8).
     *
     * È sempre nero, sempre presente, sempre riservato. La sua funzione principale
     * è garantire che il format info abbia un numero dispari di moduli neri nelle
     * sue posizioni di copia, per facilitare il rilevamento dell'orientamento.
     */
    private static function placeDarkModule(array &$m, array &$r, int $version): void
    {
        $row = $version * 4 + 9;
        $m[$row][8] = 1;
        $r[$row][8] = true;
    }

    // ── Riserva area version info (v7+) ────────────────────────────────────────

    /**
     * Riserva le due zone 6×3 dove verrà scritto il version information (solo per v7+).
     *
     * Nelle versioni 7 e superiori, la griglia include due copie speculari di un blocco
     * 18-bit che codifica il numero di versione: una zona 6×3 nell'angolo in alto a destra
     * (adiacente al finder), e una zona 3×6 simmetrica in basso a sinistra.
     * Vengono marcate come riservate ora (fase di costruzione) e riempite dopo il masking.
     */
    private static function reserveVersionInfo(array &$r, int $version, int $size): void
    {
        if ($version < 7) {
            return;
        }
        // Blocco in alto a destra: righe 0-5, colonne size-11 a size-9
        for ($i = 0; $i < 6; $i++) {
            for ($j = $size - 11; $j <= $size - 9; $j++) {
                $r[$i][$j] = true;
            }
        }
        // Blocco in basso a sinistra: righe size-11 a size-9, colonne 0-5
        for ($i = $size - 11; $i <= $size - 9; $i++) {
            for ($j = 0; $j < 6; $j++) {
                $r[$i][$j] = true;
            }
        }
    }

    // ── Posizionamento version info (v7+, dopo masking) ────────────────────────

    /**
     * Scrive i 18 bit del version information nelle zone riservate (solo v7+).
     *
     * Il valore intero in $VERSION_INFO contiene i 18 bit pre-calcolati (numero di versione
     * + 12 bit BCH di correzione errori). I bit vengono scritti dal bit 0 (LSB) al bit 17:
     *
     * Bit i → riga = i/3 (intero), colonna = i%3 nel blocco 6×3.
     * Stesso bit scritto simmetricamente in entrambe le copie (trasposizione della matrice).
     */
    private static function placeVersionInfo(array &$m, int $version, int $size): void
    {
        if ($version < 7 || !isset(self::$VERSION_INFO[$version])) {
            return;
        }
        $val = self::$VERSION_INFO[$version];
        for ($i = 0; $i < 18; $i++) {
            $bit = ($val >> $i) & 1;
            $row = (int) floor($i / 3); // 0-5: riga nel blocco 6×3
            $col = $i % 3;              // 0-2: colonna nel blocco 6×3
            $m[$row][$size - 11 + $col] = $bit; // copia in alto a destra
            $m[$size - 11 + $col][$row] = $bit; // copia in basso a sinistra (trasposta)
        }
    }

    // ── Riserva area formato ────────────────────────────────────────────────────

    /**
     * Riserva le celle del format information (due copie attorno al finder in alto a sinistra
     * e copie secondarie vicino agli altri due finder).
     *
     * Le celle vengono marcate come riservate ma non valorizzate ancora:
     * il format info contiene il numero di maschera che verrà determinato dopo (selectMask),
     * quindi può essere scritto solo al termine del processo.
     */
    private static function reserveFormat(array &$r, int $size): void
    {
        // Prima copia: L-shape attorno al finder in alto a sinistra.
        for ($j = 0; $j <= 8; $j++) { $r[8][$j] = true; }  // riga 8, colonne 0-8
        for ($i = 0; $i <= 8; $i++) { $r[$i][8] = true; }  // colonna 8, righe 0-8

        // Seconda copia: strip verticale adiacente al finder in basso a sinistra
        for ($i = $size - 8; $i < $size; $i++) { $r[$i][8] = true; }

        // Seconda copia: strip orizzontale adiacente al finder in alto a destra
        for ($j = $size - 8; $j < $size; $j++) { $r[8][$j] = true; }
    }

    // ── Codifica dati + ECC ────────────────────────────────────────────────────

    /**
     * Codifica la stringa dati in un array di codeword (byte) pronti per la matrice.
     *
     * Il processo:
     *
     *   1. BITSTREAM: costruisce una sequenza di bit che inizia con l'indicatore di modo
     *      (0100 = byte mode), poi la lunghezza dei dati (8 bit per versioni 1-9),
     *      poi i byte dei dati stessi, poi un terminatore (0000), infine padding
     *      per riempire esattamente totalData codeword × 8 bit.
     *      I byte di padding alternano 0xEC (11101100) e 0x11 (00010001) — valori fissi
     *      definiti dallo standard per essere riconoscibili e avere buona dispersione di bit.
     *
     *   2. BLOCCHI ECC: i dati vengono divisi in blocchi (g1Count blocchi da g1Data byte
     *      e g2Count blocchi da g2Data byte). Per ogni blocco si calcolano ecPerBlock
     *      codeword di Reed-Solomon (codice di correzione errori).
     *
     *   3. INTERLEAVING: i dati dei blocchi vengono interlacciati: prima il byte 0 di
     *      ogni blocco, poi il byte 1 di ogni blocco, ecc. Questo distribuisce gli errori
     *      burst (es. un graffio sul QR code) su blocchi diversi, migliorando la recuperabilità.
     *      Subito dopo, vengono interlacciati anche i codeword ECC.
     *
     * @return int[]  Array di interi 0-255 (codeword)
     */
    private static function encodeData(string $data, int $version): array
    {
        list($ecPerBlock, $g1Count, $g1Data, $g2Count, $g2Data) = self::$EC_BLOCKS_M[$version];
        $totalData = $g1Count * $g1Data + $g2Count * $g2Data;

        // ── Step 1: costruisci il bitstream ─────────────────────────────────────
        $bits = '';
        $bits .= '0100';                                 // mode indicator: 0100 = byte mode
        $bits .= self::intToBits(strlen($data), 8);     // character count indicator (8 bit per v1-9)
        for ($i = 0; $i < strlen($data); $i++) {
            $bits .= self::intToBits(ord($data[$i]), 8); // ogni carattere come 8 bit
        }
        $bits .= '0000';                                 // terminatore

        // Allinea a multiplo di 8 bit aggiungendo zeri
        while (strlen($bits) % 8 !== 0) {
            $bits .= '0';
        }

        // Riempie con byte di padding alternati finché non raggiungiamo la capacità totale
        $padBytes = ['11101100', '00010001']; // = 0xEC, 0x11
        $pi = 0;
        while (strlen($bits) < $totalData * 8) {
            $bits .= $padBytes[$pi % 2];
            $pi++;
        }

        // Converte il bitstream in array di interi (codeword)
        $dataWords = [];
        for ($i = 0; $i < $totalData; $i++) {
            $dataWords[] = bindec(substr($bits, $i * 8, 8));
        }

        // ── Step 2: calcola i blocchi e i codeword ECC ──────────────────────────
        $blocks   = [];
        $ecBlocks = [];
        $idx = 0;
        for ($b = 0; $b < $g1Count; $b++) {
            $block = array_slice($dataWords, $idx, $g1Data);
            $idx  += $g1Data;
            $blocks[]   = $block;
            $ecBlocks[] = self::rsBlock($block, $ecPerBlock); // calcola ECC con Reed-Solomon
        }
        for ($b = 0; $b < $g2Count; $b++) {
            $block = array_slice($dataWords, $idx, $g2Data);
            $idx  += $g2Data;
            $blocks[]   = $block;
            $ecBlocks[] = self::rsBlock($block, $ecPerBlock);
        }

        // ── Step 3: interleave dati poi interleave ECC ───────────────────────────
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

    // ── Posizionamento dati nella matrice ──────────────────────────────────────

    /**
     * Scrive i bit delle codeword nelle celle libere della matrice.
     *
     * Lo standard QR code definisce un percorso specifico per il posizionamento dei dati:
     * si parte dall'angolo in basso a destra e si procede a "zigzag" verso l'alto in
     * colonne da 2 moduli, poi si scende nella coppia di colonne successiva, ecc.
     *
     * Il percorso salta automaticamente le celle riservate ($reserved = true)
     * e le celle già scritte da pattern funzionali ($matrix != -1).
     *
     * La colonna 6 (timing strip verticale) viene saltata interamente: il zigzag
     * passa direttamente dalla colonna 7 alla colonna 5 senza perdere bit.
     */
    private static function placeData(array &$m, array $r, array $codewords, int $size): void
    {
        // Converte l'array di codeword in una stringa di bit
        $bits = '';
        foreach ($codewords as $cw) {
            $bits .= str_pad(decbin($cw), 8, '0', STR_PAD_LEFT);
        }

        $bitIdx = 0;
        $upward = true;       // direzione di scorrimento: true=verso l'alto, false=verso il basso
        $col    = $size - 1;  // partiamo dall'ultima colonna

        while ($col > 0) {
            if ($col === 6) {
                $col--; // salta la timing strip verticale (colonna 6)
            }

            // Percorri la colonna corrente nella direzione indicata da $upward.
            // Per ogni riga, controlliamo le due colonne della coppia ($col e $col-1).
            for ($delta = 0; $delta < $size; $delta++) {
                $row = $upward ? ($size - 1 - $delta) : $delta;
                for ($c = 0; $c < 2; $c++) {
                    $cc = $col - $c;
                    // Scrivi solo nelle celle non riservate e non ancora assegnate.
                    if (!$r[$row][$cc] && $m[$row][$cc] === -1) {
                        $m[$row][$cc] = ($bitIdx < strlen($bits) && $bits[$bitIdx] === '1') ? 1 : 0;
                        $bitIdx++;
                    }
                }
            }

            $upward = !$upward; // inverti la direzione per la coppia di colonne successiva
            $col   -= 2;        // passa alla coppia di colonne precedente
        }
    }

    // ── Selezione e applicazione della maschera ────────────────────────────────

    /**
     * Prova tutte le 8 maschere e restituisce quella con penalità minima.
     *
     * Cos'è una maschera QR?
     * Dopo aver scritto i dati, alcuni pattern locali nella matrice possono
     * assomigliare ai finder pattern o creare lunghe file di moduli identici,
     * ingannando il decodificatore. La maschera "rompe" questi pattern applicando
     * XOR ai moduli dati con una funzione matematica.
     *
     * Esistono 8 funzioni di maschera (maskCondition). Lo standard impone di
     * provarle tutte e scegliere quella con il punteggio di penalità più basso,
     * calcolato da calcPenalty() secondo 4 regole definite in ISO 18004.
     */
    private static function selectMask(array $matrix, array $reserved, int $size): int
    {
        $best      = PHP_INT_MAX;
        $bestMask  = 0;

        for ($mask = 0; $mask < 8; $mask++) {
            $m = $matrix; // copia della matrice: applyMask è destructive
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
     * Applica la maschera $mask alla matrice: per ogni cella dati (non riservata)
     * dove la funzione di maschera è vera, inverte il valore del modulo (XOR con 1).
     */
    private static function applyMask(array &$m, array $r, int $mask, int $size): void
    {
        for ($i = 0; $i < $size; $i++) {
            for ($j = 0; $j < $size; $j++) {
                if ($r[$i][$j]) {
                    continue; // non toccare le aree funzionali
                }
                if (self::maskCondition($mask, $i, $j)) {
                    $m[$i][$j] ^= 1; // inverti il modulo
                }
            }
        }
    }

    /**
     * Le 8 funzioni di condizione per le maschere QR (ISO 18004, tabella 10).
     * $i = riga, $j = colonna. Restituisce true se il modulo deve essere invertito.
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

    // ── Calcolo della penalità ─────────────────────────────────────────────────

    /**
     * Calcola il punteggio di penalità per la matrice (con una certa maschera applicata).
     *
     * Quattro regole ISO 18004 che penalizzano pattern problematici per i decodificatori:
     *
     *   Regola 1: sequenze di 5+ moduli uguali consecutivi (+3 + ogni modulo oltre 5).
     *             Le lunghe file monocromatiche interferiscono con la lettura.
     *
     *   Regola 2: blocchi 2×2 di moduli uguali (+3 per ogni blocco).
     *             Le aree uniformi rendono difficile stabilire i confini dei moduli.
     *
     *   Regola 3: pattern che assomigliano al bordo del finder pattern (+40 per occorrenza).
     *             Pattern [1,0,1,1,1,0,1,0,0,0,0] o il suo speculare [0,0,0,0,1,0,1,1,1,0,1].
     *
     *   Regola 4: proporzione di moduli scuri lontana dal 50% (+10 per ogni 5% di scostamento).
     *             Una buona distribuzione nero/bianco facilita la lettura automatica.
     */
    private static function calcPenalty(array $m, int $size): int
    {
        $score = 0;

        // Regola 1: run di 5+ moduli identici in righe e colonne
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

        // Regola 2: blocchi 2×2 di moduli identici
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

        // Regola 3: pattern simili al bordo del finder
        $p1 = [1,0,1,1,1,0,1,0,0,0,0]; // 10111010000
        $p2 = [0,0,0,0,1,0,1,1,1,0,1]; // 00001011101 (speculare di p1)
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

        // Regola 4: proporzione di moduli scuri
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

    // ── Format information ─────────────────────────────────────────────────────

    /**
     * Scrive i 15 bit del format information nelle due copie riservate.
     *
     * Il format information codifica: EC level (2 bit) + numero maschera (3 bit)
     * + 10 bit di correzione errori BCH. Il tutto è XOR con una maschera fissa
     * (101010000010010) per garantire che il pattern non sia mai tutto zeri.
     *
     * La prima copia si trova in un percorso L-shaped attorno al finder in alto a sinistra.
     * La seconda copia è divisa tra il finder in basso a sinistra (7 bit) e quello
     * in alto a destra (8 bit), per ridondanza nel caso un angolo del QR code sia danneggiato.
     */
    private static function placeFormatInfo(array &$m, int $mask, int $size): void
    {
        $fmt  = self::$FORMAT_INFO[$mask];
        // Estrae i 15 bit dal MSB (bit 14) al LSB (bit 0)
        $bits = [];
        for ($i = 14; $i >= 0; $i--) {
            $bits[] = ($fmt >> $i) & 1;
        }

        // Prima copia (percorso specifico definito dallo standard, in senso orario)
        $seqRow = [8,8,8,8,8,8,8,8,7,5,4,3,2,1,0];
        $seqCol = [0,1,2,3,4,5,7,8,8,8,8,8,8,8,8];
        for ($i = 0; $i < 15; $i++) {
            $m[$seqRow[$i]][$seqCol[$i]] = $bits[$i];
        }

        // Seconda copia (finder in basso a sinistra + in alto a destra)
        for ($i = 0; $i < 7; $i++) {
            $m[$size - 1 - $i][8] = $bits[$i];
        }
        for ($i = 7; $i < 15; $i++) {
            $m[8][$size - 15 + $i] = $bits[$i];
        }
    }

    // ── Reed-Solomon ECC ───────────────────────────────────────────────────────

    /**
     * Inizializza le tabelle di esponenti e logaritmi per GF(256).
     *
     * GF(256) è il "Galois Field" (campo finito) di 256 elementi, usato dalla
     * codifica Reed-Solomon del QR code. In GF(256) l'addizione è XOR e la
     * moltiplicazione si fa tramite logaritmi (come i numeri reali, ma modulari).
     *
     * Per costruire le tabelle si parte da alpha = 2 (elemento primitivo) e si
     * calcola 2^0, 2^1, ..., 2^254 modulo il polinomio primitivo x^8+x^4+x^3+x^2+1
     * (= 0x11D in esadecimale). Quando il valore supera 255, si applica XOR con 0x11D
     * che è l'equivalente del "modulo" in questo campo.
     *
     * Perché tabelle EXP e LOG duplicate fino a 511? Per poter calcolare
     * EXP[(LOG[a] + LOG[b]) % 255] senza dover gestire il modulo in gfMul().
     */
    private static function initGF(): void
    {
        if (self::$GF_INIT) {
            return; // già inizializzate in una chiamata precedente
        }
        self::$EXP = array_fill(0, 512, 0);
        self::$LOG = array_fill(0, 256, 0);
        $x = 1;
        for ($i = 0; $i < 255; $i++) {
            self::$EXP[$i] = $x;
            self::$LOG[$x] = $i;
            $x <<= 1;
            if ($x & 0x100) {
                $x ^= 0x11D; // x^8+x^4+x^3+x^2+1: riduci modulo il polinomio primitivo
            }
        }
        // Duplica i valori 0-254 in 255-509 per semplificare l'addizione in gfMul
        for ($i = 255; $i < 512; $i++) {
            self::$EXP[$i] = self::$EXP[$i - 255];
        }
        self::$GF_INIT = true;
    }

    /**
     * Moltiplicazione in GF(256): a * b = alpha^(log(a) + log(b)).
     * Se uno degli operandi è 0, il prodotto è 0 (per definizione).
     */
    private static function gfMul(int $a, int $b): int
    {
        if ($a === 0 || $b === 0) {
            return 0;
        }
        return self::$EXP[(self::$LOG[$a] + self::$LOG[$b]) % 255];
    }

    /**
     * Calcola il polinomio generatore di Reed-Solomon di grado $degree.
     *
     * Il polinomio generatore g(x) è il prodotto (x - alpha^0)(x - alpha^1)...(x - alpha^(n-1))
     * calcolato in GF(256). Viene usato da rsBlock() per la divisione polinomiale.
     *
     * Il risultato è un array di coefficienti, dove $poly[0] è il termine di grado massimo.
     */
    private static function rsGeneratorPoly(int $degree): array
    {
        $poly = [1]; // polinomio iniziale: 1
        for ($i = 0; $i < $degree; $i++) {
            $alpha = self::$EXP[$i]; // alpha^i
            $new   = array_fill(0, count($poly) + 1, 0);
            foreach ($poly as $j => $coef) {
                $new[$j]     ^= $coef;                        // addizione in GF = XOR
                $new[$j + 1] ^= self::gfMul($coef, $alpha);  // moltiplicazione in GF
            }
            $poly = $new;
        }
        return $poly;
    }

    /**
     * Calcola i codeword di correzione errori Reed-Solomon per un blocco dati.
     *
     * Reed-Solomon funziona come la divisione polinomiale: il messaggio dati viene
     * trattato come un polinomio M(x), moltiplicato per x^ecCount (= shift a sinistra),
     * e diviso per il polinomio generatore g(x). Il resto di questa divisione
     * (i $ecCount codeword di ECC) viene allegato ai dati.
     *
     * In pratica è una divisione sintetica: si itera sui coeﬃcienti del messaggio
     * e ad ogni passo si sottrae (XOR in GF(256)) il polinomio generatore scalato
     * per il coeﬃciente corrente.
     *
     * @param  int[] $data    Codeword dati del blocco (interi 0-255)
     * @param  int   $ecCount Numero di codeword ECC da generare
     * @return int[]          Codeword ECC (interi 0-255)
     */
    private static function rsBlock(array $data, int $ecCount): array
    {
        $gen = self::rsGeneratorPoly($ecCount);
        // Messaggio iniziale: dati seguiti da $ecCount zeri (spazio per il resto)
        $msg = array_merge($data, array_fill(0, $ecCount, 0));

        for ($i = 0; $i < count($data); $i++) {
            $coef = $msg[$i]; // coefficiente di testa della divisione
            if ($coef !== 0) {
                for ($j = 1; $j <= $ecCount; $j++) {
                    $msg[$i + $j] ^= self::gfMul($gen[$j], $coef);
                }
            }
        }

        // Restituisce solo il resto (ultimi $ecCount elementi)
        return array_slice($msg, count($data));
    }

    // ── Helper ─────────────────────────────────────────────────────────────────

    /**
     * Converte un intero in stringa binaria di $length bit con zero-padding iniziale.
     * Es: intToBits(5, 4) → "0101"
     */
    private static function intToBits(int $value, int $length): string
    {
        return str_pad(decbin($value), $length, '0', STR_PAD_LEFT);
    }

    // ── Rendering PNG con GD ───────────────────────────────────────────────────

    /**
     * Converte la matrice QR in un'immagine PNG usando l'estensione GD.
     *
     * Ogni modulo della matrice (valore 1 = nero) viene disegnato come un rettangolo
     * di $scale × $scale pixel. Il margine bianco ($margin moduli) viene aggiunto
     * attorno alla griglia per migliorare la leggibilità — lo standard raccomanda
     * almeno 4 moduli di bordo.
     *
     * Il PNG viene catturato dall'output buffer (ob_start/ob_get_clean) perché
     * imagepng() scrive direttamente sullo stdout, non restituisce una stringa.
     */
    private static function renderPng(array $matrix, int $scale, int $margin): string
    {
        $size    = count($matrix);
        $imgSize = ($size + 2 * $margin) * $scale;
        $img     = imagecreatetruecolor($imgSize, $imgSize);
        $white   = imagecolorallocate($img, 255, 255, 255);
        $black   = imagecolorallocate($img, 0, 0, 0);

        imagefill($img, 0, 0, $white); // sfondo bianco

        for ($row = 0; $row < $size; $row++) {
            for ($col = 0; $col < $size; $col++) {
                if ($matrix[$row][$col] === 1) {
                    // Calcola le coordinate pixel del modulo (tenendo conto del margine)
                    $x1 = ($col + $margin) * $scale;
                    $y1 = ($row + $margin) * $scale;
                    imagefilledrectangle($img, $x1, $y1, $x1 + $scale - 1, $y1 + $scale - 1, $black);
                }
            }
        }

        ob_start();
        imagepng($img);
        $png = ob_get_clean();
        imagedestroy($img); // libera la memoria GD

        return $png;
    }
}
