<?php
declare(strict_types=1);

/**
 * JXTOTP module bootstrap.
 *
 * Included by jxtotp.php (the Janox framework login entry point) and by this folder's
 * embedded pages (index.php, register.php).
 *
 * Responsibilities:
 *   1. Load module classes (Database, Totp, QrCode, Auth).
 *   2. Instantiate the shared $db and $auth used by all including pages.
 *   3. Define helpers h(), getClientIp().
 *
 * Flat folder layout:
 *   totp/
 *   ├── _bootstrap.php       ← this file
 *   ├── index.php            ← OTP verification (embedded)
 *   ├── register.php         ← TOTP setup (embedded)
 *   ├── <app>/data/jxtotp.db ← SQLite, auto-created on first run
 *   └── src/                 ← Auth, Database, Totp, QrCode classes
 */

// ________________________________________________ Onclude needed classes definitions ___
require_once __DIR__.'/src/Database.php';
require_once __DIR__.'/src/Totp.php';
require_once __DIR__.'/src/QrCode.php';
require_once __DIR__.'/src/Auth.php';

// ________________________________________ SQLite DB path, in application data folder ___
if (isset($_SESSION['o2_app'])) {
    define('DB_PATH', $_SESSION['o2_app']->dir_data.'jxtotp.db');
    }
else {
    error_send('Undefined application');
    }

// __________________________________________________________ Shared objects instances ___
$db   = new Database(DB_PATH);
$auth = new Auth($db);


/**
 * Escapes a string for safe HTML output.
 *
 * @param  string $s   Raw message string
 * @return string HTML-safe string.
 */
function h(string $s): string {

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
function getClientIp(): string {

    if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
        // ________ "client, proxy1, proxy2": first entry is the originating client IP ___
        $parts = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR']);
        $ip    = trim($parts[0]);
        if (filter_var($ip, FILTER_VALIDATE_IP)) {
            return $ip;
            }
        }
    return $_SERVER['REMOTE_ADDR'] ?? '0.0.0.0';

    }
