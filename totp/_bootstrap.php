<?php
declare(strict_types=1);

/**
 * JXTOTP module bootstrap.
 *
 * Included by jxtotp.php (the Janox framework login entry point) and by
 * this folder's embedded pages (index.php, register.php).
 *
 * Responsibilities:
 *   1. Load module classes (Database, Totp, QrCode, Auth).
 *   2. Instantiate the shared $db and $auth used by all including pages.
 *   3. Define helpers h(), getClientIp().
 *
 * Flat folder layout:
 *   totp/
 *   ├── _bootstrap.php   ← this file
 *   ├── index.php        ← OTP verification (embedded)
 *   ├── register.php     ← TOTP setup (embedded)
 *   ├── data/auth.db     ← SQLite, auto-created on first run
 *   └── src/             ← Auth, Database, Totp, QrCode classes
 */

// SQLite DB path, directly under this folder (flat layout).
define('DB_PATH', __DIR__ . '/data/auth.db');

require_once __DIR__ . '/src/Database.php';
require_once __DIR__ . '/src/Totp.php';
require_once __DIR__ . '/src/QrCode.php';
require_once __DIR__ . '/src/Auth.php';

// ── Shared instances ───────────────────────────────────────────────────────
$db   = new Database(DB_PATH);
$auth = new Auth($db);

/**
 * Escapes a string for safe HTML output.
 *
 * @param string $s Raw string (user input, DB value, or other external source).
 * @return string HTML-safe string.
 */
function h(string $s): string
{
    return htmlspecialchars($s, ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
}

/**
 * Returns the client IP, used for OTP attempt rate limiting (see
 * Auth::verifyTotp()).
 *
 * Caveat: trusts X-Forwarded-For without a trusted-proxy allowlist, so it's
 * spoofable if the server isn't actually behind a trusted reverse proxy.
 *
 * @return string
 */
function getClientIp(): string
{
    if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
        // "client, proxy1, proxy2" — first entry is the originating client IP.
        $parts = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR']);
        $ip    = trim($parts[0]);
        if (filter_var($ip, FILTER_VALIDATE_IP)) {
            return $ip;
        }
    }
    return $_SERVER['REMOTE_ADDR'] ?? '0.0.0.0';
}
