<?php
declare(strict_types=1);

/**
 * TOTP (Time-based One-Time Password) implementation — RFC 6238.
 *
 * Stateless: all methods are static. No external dependencies — relies only
 * on PHP's native hash_hmac().
 */
class Totp
{
    /**
     * Base32 alphabet per RFC 4648: 26 uppercase letters + digits 2-7 (32 symbols).
     * Excludes 0, 1, 8, 9 to avoid visual confusion with O, I, B, g.
     */
    const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567';

    /** OTP code length. RFC 6238 default is 6. */
    const DIGITS   = 6;

    /** Time-step duration in seconds. RFC 6238 default is 30. */
    const PERIOD   = 30;

    /**
     * Generates a new random TOTP secret using a CSPRNG (random_bytes()).
     *
     * Must never be regenerated after initial confirmation — rotating it
     * invalidates every device already enrolled.
     *
     * @param int $bytes Entropy in bytes (default 20 = 160 bits -> 32 Base32 chars).
     * @return string Base32-encoded secret.
     */
    public static function generateSecret(int $bytes = 20): string
    {
        return self::base32Encode(random_bytes($bytes));
    }

    /**
     * Builds the otpauth:// URI (Google Authenticator Key URI Format) for the QR code.
     *
     * algorithm/digits/period are intentionally omitted since they match the
     * RFC 6238 defaults — shorter URI, lower QR version, easier to scan.
     *
     * @param string $secret   Base32 secret.
     * @param string $username Account label.
     * @param string $issuer   Issuer name; included in both path and query
     *                         string for compatibility with older authenticator apps.
     * @return string otpauth:// URI.
     */
    public static function getUri(string $secret, string $username, string $issuer): string
    {
        return sprintf(
            'otpauth://totp/%s:%s?secret=%s&issuer=%s',
            rawurlencode($issuer),
            rawurlencode($username),
            $secret,
            rawurlencode($issuer)
        );
    }

    /**
     * Verifies a user-supplied OTP code.
     *
     * @param string $secret Base32 secret.
     * @param string $code   6-digit code to check.
     * @param int    $window Tolerance in time-steps on each side of the current
     *                       one (default ±1 step = ±30s) to absorb clock drift
     *                       between client and server.
     * @return bool
     */
    public static function verify(string $secret, string $code, int $window = 1): bool
    {
        // Reject anything that isn't exactly 6 digits before doing any HMAC work.
        if (!preg_match('/^\d{6}$/', $code)) {
            return false;
        }

        // Current Unix time-step counter.
        $t = (int) floor(time() / self::PERIOD);

        // Check the current step plus $window steps before/after (drift tolerance).
        for ($i = -$window; $i <= $window; $i++) {
            $expected = self::hotp(self::base32Decode($secret), $t + $i);
            if (hash_equals($expected, $code)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns the OTP code for the current time-step (test/debug helper).
     *
     * @param string $secret Base32 secret.
     * @return string
     */
    public static function getCurrentCode(string $secret): string
    {
        $t = (int) floor(time() / self::PERIOD);
        return self::hotp(self::base32Decode($secret), $t);
    }

    // ── Internal methods ────────────────────────────────────────────────────────

    /**
     * HOTP per RFC 4226 §5.3: HMAC-SHA1 -> dynamic truncation -> mod 10^6.
     *
     * @param string $keyBytes Raw (decoded) secret bytes.
     * @param int    $counter  Time-step counter (TOTP) or event counter (HOTP).
     * @return string Zero-padded 6-digit code.
     */
    private static function hotp(string $keyBytes, int $counter): string
    {
        // 8-byte big-endian counter ('J' = unsigned 64-bit BE); raw=true returns
        // the 20-byte digest instead of a hex string.
        $msg  = pack('J', $counter);
        $hmac = hash_hmac('sha1', $msg, $keyBytes, true);

        // Dynamic truncation: low 4 bits of the last digest byte select the offset.
        $offset = ord($hmac[19]) & 0x0F;

        // Assemble 4 bytes into an integer; clear the sign bit of the first byte
        // to avoid signed/unsigned ambiguity.
        $code   = (
            ((ord($hmac[$offset])     & 0x7F) << 24) |
            ((ord($hmac[$offset + 1]) & 0xFF) << 16) |
            ((ord($hmac[$offset + 2]) & 0xFF) << 8)  |
            ((ord($hmac[$offset + 3]) & 0xFF))
        ) % 1000000;

        return str_pad((string) $code, self::DIGITS, '0', STR_PAD_LEFT);
    }

    /**
     * Encodes binary data as Base32 (RFC 4648, unpadded).
     *
     * @param string $data Raw bytes.
     * @return string Base32 string.
     */
    public static function base32Encode(string $data): string
    {
        $out      = '';
        $len      = strlen($data);
        $buffer   = 0;   // bit buffer
        $bitsLeft = 0;   // valid bits currently in the buffer

        for ($i = 0; $i < $len; $i++) {
            // Append this byte's 8 bits to the buffer.
            $buffer   = ($buffer << 8) | ord($data[$i]);
            $bitsLeft += 8;

            // Emit a character for every 5-bit group available.
            while ($bitsLeft >= 5) {
                $bitsLeft -= 5;
                // Shift the target 5 bits into the low position, mask with 0x1F.
                $out .= self::ALPHABET[($buffer >> $bitsLeft) & 0x1F];
            }
        }

        // Pad a leftover partial group (< 5 bits) with trailing zero bits.
        if ($bitsLeft > 0) {
            $out .= self::ALPHABET[($buffer << (5 - $bitsLeft)) & 0x1F];
        }

        // RFC 4648 '=' padding intentionally omitted — authenticator apps accept
        // unpadded Base32.
        return $out;
    }

    /**
     * Decodes a Base32 string to binary (inverse of base32Encode()).
     *
     * Tolerant of lowercase input, '=' padding, and characters outside the
     * alphabet (silently skipped).
     *
     * @param string $data Base32 string.
     * @return string Raw bytes.
     */
    public static function base32Decode(string $data): string
    {
        // Strip padding, normalize to uppercase.
        $data = strtoupper(rtrim($data, '='));

        // Char -> 5-bit value lookup.
        $map      = array_flip(str_split(self::ALPHABET));
        $out      = '';
        $buffer   = 0;
        $bitsLeft = 0;

        for ($i = 0, $len = strlen($data); $i < $len; $i++) {
            $ch = $data[$i];
            // Skip characters outside the alphabet.
            if (!isset($map[$ch])) {
                continue;
            }

            // Append 5 bits to the buffer.
            $buffer    = ($buffer << 5) | $map[$ch];
            $bitsLeft += 5;

            // Emit a byte once at least 8 bits are buffered.
            if ($bitsLeft >= 8) {
                $bitsLeft -= 8;
                $out .= chr(($buffer >> $bitsLeft) & 0xFF);
            }
        }

        return $out;
    }
}
