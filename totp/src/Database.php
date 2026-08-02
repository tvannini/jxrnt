<?php
declare(strict_types=1);

/**
 * Lightweight PDO SQLite wrapper for the JXTOTP module (second-factor MFA only).
 *
 * - Opens (or creates) the SQLite file at the given path.
 * - Creates the schema on first run.
 * - Exposes query() as the sole entry point, always via prepared statements.
 */
class Database
{
    private $pdo; // no type: PHP 7.3 compat (typed properties require 7.4+)

    public function __construct(string $dbPath)
    {
        // First-run bootstrap: create data/ if missing.
        $dir = dirname($dbPath);
        if (!is_dir($dir)) {
            mkdir($dir, 0755, true);
        }

        // PDO creates the SQLite file automatically if it doesn't exist.
        // ERRMODE_EXCEPTION: fail loudly on SQL errors instead of silently.
        // FETCH_ASSOC: rows as associative arrays, column-order independent.
        $this->pdo = new PDO('sqlite:' . $dbPath, null, null, [
            PDO::ATTR_ERRMODE            => PDO::ERRMODE_EXCEPTION,
            PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
        ]);

        // WAL: concurrent reads/writes don't block each other — relevant
        // since multiple jxtotp.php requests can land at nearly the same time.
        $this->pdo->exec('PRAGMA journal_mode=WAL');

        // FOREIGN KEY enforcement is off by default in SQLite; needed for
        // the trusted_devices -> totp_users cascade below.
        $this->pdo->exec('PRAGMA foreign_keys=ON');

        $this->initSchema();
    }

    /**
     * Creates all tables if they don't already exist.
     *
     * Idempotent (CREATE TABLE IF NOT EXISTS), safe to call on every boot.
     * No migrations — see class-level PHPDoc.
     */
    private function initSchema(): void
    {
        // totp_users: one row per Janox user with TOTP configured (or
        // pending setup). Row is created on first login where Janox reports
        // mfa='T' for that user (see Auth::ensureTotpUser()), not at ERP
        // user-creation time — this module has no visibility into that.
        //
        // o2user mirrors o2admin_users.o2user but with no physical FK
        // (different database) — it's just a shared string key.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS totp_users (
                -- Janox username (string, L50)
                o2user               VARCHAR(50) NOT NULL,

                -- Base32 TOTP secret (160 bit), plaintext — protect jxtotp.db
                -- at the filesystem level.
                totp_secret          VARCHAR(240) NOT NULL DEFAULT '',

                -- 0 = setup pending (QR not yet confirmed), 1 = active.
                totp_confirmed       CHAR(1)      NOT NULL DEFAULT '0',

                -- Anti-replay: last accepted OTP code + its time window
                -- (floor(unix_time/30)).
                last_totp_code       VARCHAR(50)  NOT NULL DEFAULT '',
                last_totp_window     INTEGER      NOT NULL DEFAULT 0,

                -- Per-account lockout: consecutive failed attempts and
                -- unlock timestamp (0 = not locked). Reset on success.
                failed_otp_attempts  INTEGER      NOT NULL DEFAULT 0,
                otp_locked_until     INTEGER      NOT NULL DEFAULT 0,

                -- Row creation timestamp.
                created_at           INTEGER      NOT NULL DEFAULT 0,

                -- Last successful 2FA timestamp; local audit only, does not
                -- feed Janox's own last_date/last_time.
                last_totp_login      INTEGER      NOT NULL DEFAULT 0,

                PRIMARY KEY (o2user)
            )
        ");

        // trusted_devices: "remember this device" — skips OTP re-entry for
        // Auth::TRUSTED_DEVICE_TTL after a successful 2FA.
        //
        // Only the SHA-256 hash of the token is stored; the raw token lives
        // solely in the user's browser cookie, so a DB leak alone can't
        // reconstruct valid cookies. user_id is TEXT (references o2user,
        // string PK) — never cast to (int). ON DELETE CASCADE keeps this
        // table clean when the parent totp_users row is removed.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS trusted_devices (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id     TEXT    NOT NULL,
                token_hash  TEXT    NOT NULL UNIQUE,
                created_at  INTEGER NOT NULL,
                -- Unix timestamp (Auth::TRUSTED_DEVICE_TTL = 10 days).
                expires_at  INTEGER NOT NULL,
                -- Audit only, never used in auth logic.
                user_agent  TEXT    NOT NULL DEFAULT '',
                FOREIGN KEY (user_id) REFERENCES totp_users(o2user) ON DELETE CASCADE
            )
        ");

        // otp_rate_limits: per-IP OTP brute-force throttling. Password auth
        // already happened upstream in Janox, but this still guards the
        // 6-digit OTP itself against guessing — blocks an IP regardless of
        // which account it's targeting.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS otp_rate_limits (
                ip           TEXT    PRIMARY KEY,
                attempts     INTEGER NOT NULL DEFAULT 0,
                -- Start of the current counting window (Unix timestamp).
                window_start INTEGER NOT NULL DEFAULT 0,
                -- Unix timestamp until which the IP is locked; 0 = not locked.
                locked_until INTEGER NOT NULL DEFAULT 0
            )
        ");

        // otp_ip_account_attempts: dedup layer for otp_rate_limits. Without
        // it, one user retrying from a shared IP (NAT/intranet) would burn
        // through the shared per-IP budget for everyone else behind it.
        // Only the first failure of a given (ip, username) pair per window
        // increments the global otp_rate_limits counter; per-account lockout
        // in totp_users still applies independently of this table.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS otp_ip_account_attempts (
                ip           TEXT    NOT NULL,
                username     TEXT    NOT NULL,
                -- Start of the current counting window for this (ip, username) pair.
                window_start INTEGER NOT NULL DEFAULT 0,
                PRIMARY KEY (ip, username)
            )
        ");
    }

    /** Exposes the raw PDO instance (rarely needed — prefer query()). */
    public function getPdo(): PDO
    {
        return $this->pdo;
    }

    /**
     * Runs a SQL statement via prepared statement (parameterized, never
     * string-concatenated — this is what prevents SQL injection here).
     *
     *   Safe:   $db->query('SELECT * FROM totp_users WHERE o2user = ?', [$user]);
     *   Unsafe: $db->query("SELECT * FROM totp_users WHERE o2user = '$user'");
     *
     * @throws PDOException on SQL error (e.g. UNIQUE constraint violation).
     */
    public function query(string $sql, array $params = []): PDOStatement
    {
        $stmt = $this->pdo->prepare($sql);
        $stmt->execute($params);
        return $stmt;
    }
}
