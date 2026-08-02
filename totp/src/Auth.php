<?php
declare(strict_types=1);

require_once __DIR__ . '/Database.php';
require_once __DIR__ . '/Totp.php';

/**
 * JXTOTP second-factor (MFA) module for Janox/ERP.
 *
 * Unlike the original standalone prototype, this class does not manage
 * users, passwords, password reset, or account recovery — that remains
 * Janox's responsibility entirely. startMfaChallenge() is invoked by
 * jxtotp.php only after Janox has already verified username/password;
 * this class has no access to ERP credential tables and never checks
 * them.
 *
 * STATE_AUTHENTICATED here means only "the second factor for this
 * session has been satisfied" — not that the user is logged into the
 * ERP app. That last step remains entirely Janox's own mechanism
 * (jxtotp.php).
 *
 * Session keys are namespaced (SESSION_USER/SESSION_STATE/SESSION_CSRF
 * below).
 *
 * States: NONE -> PENDING_SETUP (first-time setup) or PENDING_TOTP
 * (already configured) -> AUTHENTICATED via confirmTotpSetup(),
 * verifyTotp(), or checkTrustedDevice(); logout() returns to NONE.
 */
class Auth
{
    // Configuration constants

    /**
     * Max consecutive failed OTP codes for one account before lockout.
     * Applies only to verifyTotp() — confirmTotpSetup() has no lockout
     * since the account isn't yet enabled for the second factor.
     */
    const MAX_ATTEMPTS   = 5;

    /** Account lockout duration in seconds (900 = 15 min). */
    const LOCKOUT_SEC    = 900;

    /**
     * Max distinct failed accounts per IP before IP-level lockout (not
     * raw attempt count — see recordOtpFailedAttempt() for the dedup
     * logic). Sized for NAT/corporate-IP traffic where many ERP users
     * share one public IP.
     */
    const IP_MAX_ATTEMPTS = 100;

    /** IP lockout duration in seconds (900 = 15 min). */
    const IP_LOCKOUT_SEC  = 900;

    /**
     * Rolling window for counting per-IP failed attempts (3600 = 1h);
     * the counter resets once the window expires.
     */
    const IP_WINDOW_SEC   = 3600;

    /** Trusted-device session duration in seconds (10 days). */
    const TRUSTED_DEVICE_TTL = 864000;

    /** Cookie name for the trusted-device token. */
    const TRUSTED_DEVICE_COOKIE = 'td_token';

    // $_SESSION keys for this module. Prefixed with 'auth_' in case this
    // session is ever shared with other code using $_SESSION directly.
    const SESSION_USER  = 'auth_user_id';
    const SESSION_STATE = 'auth_state';
    const SESSION_CSRF  = 'auth_csrf';

    // Session state machine constants.
    const STATE_NONE          = 'none';
    const STATE_PENDING_SETUP = 'pending_setup'; // TOTP not yet configured
    const STATE_PENDING_TOTP  = 'pending_totp';  // TOTP configured, awaiting code
    const STATE_AUTHENTICATED = 'authenticated'; // second factor satisfied

    private $db; // untyped: PHP 7.3 compatibility (typed properties require 7.4+)

    public function __construct(Database $db)
    {
        $this->db = $db;
    }

    // SECTION 1: MFA challenge start (called from jxtotp.php)

    /**
     * Starts the second-factor flow for a user whose password Janox has
     * ALREADY verified.
     *
     * Single entry point from jxtotp.php: called once, on first hit,
     * once Janox has determined mfa='T' and the password is correct.
     *
     * Decides internally whether the user needs TOTP setup or code
     * entry and sets session state accordingly — jxtotp.php never makes
     * or forces this decision.
     *
     * @param string $username Janox username (lowercase) — string, never cast to int
     */
    public function startMfaChallenge(string $username): void
    {
        $this->ensureTotpUser($username);

        $_SESSION[self::SESSION_USER]  = $username;
        $_SESSION[self::SESSION_STATE] = $this->isTotpConfigured($username)
            ? self::STATE_PENDING_TOTP
            : self::STATE_PENDING_SETUP;
    }

    /**
     * Creates the totp_users row for this user if it doesn't already exist.
     *
     * There is no createUser() in this module: users are created by
     * Janox. This only ensures a place exists to store a TOTP secret for
     * a Janox user with mfa='T', called on that user's first access —
     * not at ERP account creation time.
     *
     * No-op if the row already exists: never regenerates an existing
     * secret (that would invalidate it and force the user through setup
     * again).
     */
    public function ensureTotpUser(string $username): void
    {
        $existing = $this->db->query(
            'SELECT o2user FROM totp_users WHERE o2user = ?',
            [$username]
        )->fetch();

        if ($existing) {
            return;
        }

        // Unique 160-bit secret, shown as a QR code once during setup and never again.
        $secret = Totp::generateSecret();

        $this->db->query(
            'INSERT INTO totp_users (o2user, totp_secret, created_at) VALUES (?, ?, ?)',
            [$username, $secret, time()]
        );
    }

    /**
     * Whether the user has completed second-factor setup.
     *
     * False both when the row doesn't exist and when it exists but
     * totp_confirmed!='1' (setup started but not yet confirmed with a
     * first OTP code).
     */
    public function isTotpConfigured(string $username): bool
    {
        $row = $this->db->query(
            'SELECT totp_confirmed FROM totp_users WHERE o2user = ?',
            [$username]
        )->fetch();

        return (bool) ($row && ($row['totp_confirmed'] === '1'));
    }

    /**
     * Returns the username with an MFA challenge in progress (or just
     * completed), regardless of state (PENDING_SETUP/PENDING_TOTP/
     * AUTHENTICATED).
     *
     * Used by the jxtotp.php guard to detect whether an incoming request
     * continues an MFA flow already started for the SAME user, rather
     * than resuming one abandoned by a previous login for a different
     * username.
     */
    public function getChallengeUser(): ?string
    {
        return isset($_SESSION[self::SESSION_USER]) ? (string) $_SESSION[self::SESSION_USER] : null;
    }

    // SECTION 2: second-factor setup (first-time configuration)

    /**
     * Fetches the data needed to render the QR code during TOTP setup.
     *
     * Returns null if:
     *   - the user doesn't yet exist in totp_users (shouldn't happen:
     *     startMfaChallenge() always calls ensureTotpUser() first);
     *   - setup is already complete (totp_confirmed = '1').
     *
     * The secret is deliberately withheld once setup is done — this
     * module has no account-recovery flow that would need to re-display it.
     *
     * @param string $userId Janox username — string, never cast to int
     * @return array{username: string, secret: string}|null
     */
    public function getTotpSetupData(string $userId): ?array
    {
        $user = $this->db->query(
            'SELECT o2user, totp_secret, totp_confirmed FROM totp_users WHERE o2user = ?',
            [$userId]
        )->fetch();

        if (!$user || ($user['totp_confirmed'] === '1')) {
            return null;
        }

        return [
            'username' => $user['o2user'],
            'secret'   => $user['totp_secret'],
        ];
    }

    /**
     * Confirms TOTP setup by verifying the user's first OTP code.
     *
     * Final setup step: totp_confirmed is set to '1' only on success.
     * Success also satisfies the second factor for this session
     * (STATE_AUTHENTICATED) — a valid first code is itself proof of
     * authenticator possession, so there's no point asking for a second
     * one immediately after (see register.php).
     *
     * Includes single-use replay protection (see verifyTotp()). No
     * lockout here: the account isn't yet TOTP-enabled, so there's no
     * bruteforce risk with real consequences.
     *
     * @param string $userId Janox username — string, never cast to int
     */
    public function confirmTotpSetup(string $userId, string $code): bool
    {
        $user = $this->db->query(
            'SELECT totp_secret, totp_confirmed, last_totp_code, last_totp_window FROM totp_users WHERE o2user = ?',
            [$userId]
        )->fetch();

        // Don't allow confirming the same setup twice.
        if (!$user || ($user['totp_confirmed'] === '1')) {
            return false;
        }

        // Anti-replay: same mechanism as verifyTotp() below — ±1 window
        // to match Totp::verify()'s own clock-skew tolerance.
        $currentWindow = (int) (time() / self::TOTP_PERIOD);
        if ($user['last_totp_code'] !== null
            && $user['last_totp_code'] === $code
            && abs($currentWindow - (int) $user['last_totp_window']) <= 1) {
            return false; // Code already used in this time window
        }

        if (!Totp::verify($user['totp_secret'], $code)) {
            return false;
        }

        // Setup complete: mark confirmed, record the code used (anti-replay)
        // and the first successful-login timestamp.
        $this->db->query(
            'UPDATE totp_users SET totp_confirmed = \'1\', last_totp_code = ?, last_totp_window = ?, last_totp_login = ? WHERE o2user = ?',
            [$code, $currentWindow, time(), $userId]
        );

        // Regenerate the session ID whenever state becomes "satisfied" —
        // same pattern as verifyTotp()/checkTrustedDevice() below.
        session_regenerate_id(true);
        $_SESSION[self::SESSION_STATE] = self::STATE_AUTHENTICATED;

        return true;
    }

    // SECTION 3: second-factor verification (already configured)

    /**
     * Verifies the OTP code entered by the user.
     *
     * Only valid when session state is PENDING_TOTP; returns false
     * otherwise, which prevents bypassing the flow.
     *
     * Two-tier rate limiting:
     *   - per-account (failed_otp_attempts/otp_locked_until in totp_users);
     *   - per-IP (otp_rate_limits, via checkOtpRateLimit()), covering an
     *     attacker who tries codes across different accounts from one IP.
     *
     * Also enforces single-use replay protection: the same code cannot
     * be reused within the same 30s window.
     *
     * @param string $code     6-digit OTP code entered by the user
     * @param string $clientIp Client IP for per-IP rate limiting; pass ''
     *                         to disable that check (useful in automated tests)
     */
    public function verifyTotp(string $code, string $clientIp = ''): bool
    {
        // Guards against reaching this state without a prior password
        // check and a startMfaChallenge() call that set PENDING_TOTP.
        if ($this->getState() !== self::STATE_PENDING_TOTP) {
            return false;
        }

        // IP rate limit, checked before touching the user table so an
        // already-blocked IP fails fast.
        if ($clientIp && !$this->checkOtpRateLimit($clientIp)) {
            return false;
        }

        $userId = $_SESSION[self::SESSION_USER] ?? null;
        if (!$userId) {
            return false;
        }

        $user = $this->db->query(
            'SELECT totp_secret, last_totp_code, last_totp_window, failed_otp_attempts, otp_locked_until
             FROM totp_users WHERE o2user = ?',
            [$userId]
        )->fetch();

        if (!$user) {
            return false;
        }

        // Account already locked: reject before even checking the code.
        if ((int) $user['otp_locked_until'] > time()) {
            if ($clientIp) {
                $this->recordOtpFailedAttempt($clientIp, $userId);
            }
            return false;
        }

        // Anti-replay: ±1 window, matching Totp::verify()'s own ±30s
        // clock-skew tolerance — a code valid for window W is accepted in
        // W-1..W+1, so the replay guard must cover the same range.
        $currentWindow = (int) (time() / self::TOTP_PERIOD);
        if ($user['last_totp_code'] !== null
            && $user['last_totp_code'] === $code
            && abs($currentWindow - (int) $user['last_totp_window']) <= 1) {
            return false; // Already used — possible replay attempt
        }

        if (!Totp::verify($user['totp_secret'], $code)) {
            // Increment this account's failed-attempt counter.
            $attempts  = (int) $user['failed_otp_attempts'] + 1;
            $lockUntil = $attempts >= self::MAX_ATTEMPTS ? time() + self::LOCKOUT_SEC : 0;
            $this->db->query(
                'UPDATE totp_users SET failed_otp_attempts = ?, otp_locked_until = ? WHERE o2user = ?',
                [$attempts, $lockUntil, $userId]
            );
            if ($clientIp) {
                $this->recordOtpFailedAttempt($clientIp, $userId);
            }
            return false;
        }

        // Correct code: reset lockout counters, record the code used
        // (anti-replay) and the successful-login timestamp.
        $this->db->query(
            'UPDATE totp_users SET failed_otp_attempts = 0, otp_locked_until = 0,
             last_totp_code = ?, last_totp_window = ?, last_totp_login = ? WHERE o2user = ?',
            [$code, $currentWindow, time(), $userId]
        );

        // Regenerate the session ID on reaching "satisfied" state; the
        // true argument also removes the old session file from disk, not
        // just its ID (session fixation).
        session_regenerate_id(true);
        $_SESSION[self::SESSION_STATE] = self::STATE_AUTHENTICATED;

        return true;
    }

    /**
     * Whether an IP can still attempt an OTP code.
     * True if the IP isn't locked (or has no attempts recorded yet).
     */
    private function checkOtpRateLimit(string $ip): bool
    {
        $now = time();

        $row = $this->db->query(
            'SELECT attempts, window_start, locked_until FROM otp_rate_limits WHERE ip = ?',
            [$ip]
        )->fetch();

        if (!$row) {
            return true; // No attempts recorded for this IP
        }

        if ((int) $row['locked_until'] > $now) {
            return false; // IP currently locked
        }

        if (($now - (int) $row['window_start']) >= self::IP_WINDOW_SEC) {
            return true; // Window expired: counter resets
        }

        return (int) $row['attempts'] < self::IP_MAX_ATTEMPTS;
    }

    /**
     * Records a failed OTP attempt for an IP, deduplicated per account.
     *
     * Dedup rationale: many ERP users can sit behind one shared public
     * IP (NAT/corporate intranet). Without dedup, a single user
     * repeatedly mistyping their code would burn through the IP-wide
     * budget and lock out the whole intranet on legitimate noise, not an
     * attack. otp_rate_limits.attempts therefore counts DISTINCT failed
     * accounts per IP per window, not raw attempts — only the first
     * failure of a given (ip, username) pair in a window increments it
     * (see otp_ip_account_attempts below). Per-account lockout in
     * totp_users still applies independently of this mechanism.
     *
     * Uses separate INSERT/UPDATE rather than UPSERT/MERGE for
     * SQLite/PostgreSQL/SQL Server portability.
     */
    private function recordOtpFailedAttempt(string $ip, string $username): void
    {
        $now = time();

        // Has this (ip, username) pair already failed in this window? If
        // so, today's failure must not double-count against the IP's
        // shared budget — see rationale above.
        $pair = $this->db->query(
            'SELECT window_start FROM otp_ip_account_attempts WHERE ip = ? AND username = ?',
            [$ip, $username]
        )->fetch();

        $isNewOffenderThisWindow = true;

        if ($pair && ($now - (int) $pair['window_start']) < self::IP_WINDOW_SEC) {
            // Same pair, same window: already counted.
            $isNewOffenderThisWindow = false;
        } elseif ($pair) {
            // Known pair but window expired: start a new window, so this
            // counts as a new offender again.
            $this->db->query(
                'UPDATE otp_ip_account_attempts SET window_start = ? WHERE ip = ? AND username = ?',
                [$now, $ip, $username]
            );
        } else {
            $this->db->query(
                'INSERT INTO otp_ip_account_attempts (ip, username, window_start) VALUES (?, ?, ?)',
                [$ip, $username, $now]
            );
        }

        if (!$isNewOffenderThisWindow) {
            return; // Already counted this window: leave otp_rate_limits untouched.
        }

        // Step 2: increment the global per-IP counter.
        $row = $this->db->query(
            'SELECT attempts, window_start FROM otp_rate_limits WHERE ip = ?',
            [$ip]
        )->fetch();

        if (!$row) {
            $this->db->query(
                'INSERT INTO otp_rate_limits (ip, attempts, window_start, locked_until) VALUES (?, 1, ?, 0)',
                [$ip, $now]
            );
            return;
        }

        if (($now - (int) $row['window_start']) >= self::IP_WINDOW_SEC) {
            $this->db->query(
                'UPDATE otp_rate_limits SET attempts = 1, window_start = ?, locked_until = 0 WHERE ip = ?',
                [$now, $ip]
            );
            return;
        }

        $attempts  = (int) $row['attempts'] + 1;
        $lockUntil = $attempts >= self::IP_MAX_ATTEMPTS ? $now + self::IP_LOCKOUT_SEC : 0;

        $this->db->query(
            'UPDATE otp_rate_limits SET attempts = ?, locked_until = ? WHERE ip = ?',
            [$attempts, $lockUntil, $ip]
        );
    }

    // SECTION 4: trusted devices ("remember me")

    /**
     * Checks whether the current device is trusted for a user in
     * PENDING_TOTP; if so, skips the OTP prompt entirely.
     *
     * A random 32-byte token is issued on a "remember me" success: the
     * raw token goes in the browser cookie, its SHA-256 hash in the DB —
     * a DB leak alone doesn't expose usable cookie tokens. On each
     * visit, after startMfaChallenge(), this looks up the td_token
     * cookie, hashes it, and checks it against the DB for the session's
     * user.
     *
     * @return bool true if the device is trusted and the second factor
     *              was satisfied without requesting a code
     */
    public function checkTrustedDevice(): bool
    {
        // Only meaningful in PENDING_TOTP: no trusted device can exist
        // before TOTP is configured (PENDING_SETUP).
        if ($this->getState() !== self::STATE_PENDING_TOTP) {
            return false;
        }

        $token = $_COOKIE[self::TRUSTED_DEVICE_COOKIE] ?? '';
        if (!$token) {
            return false; // No trusted-device cookie present
        }

        $userId = $_SESSION[self::SESSION_USER] ?? null;
        if (!$userId) {
            return false;
        }

        // hash() is deterministic: the same token always produces the same hash.
        $hash = hash('sha256', $token);
        $now  = time();

        $device = $this->db->query(
            'SELECT id FROM trusted_devices WHERE user_id = ? AND token_hash = ? AND expires_at > ?',
            [$userId, $hash, $now]
        )->fetch();

        if (!$device) {
            // Cookie present but token not found (expired, revoked, or
            // forged) — fall through to the normal OTP form.
            return false;
        }

        // Valid trusted device: record the login and satisfy the second
        // factor without ever requesting a code.
        $this->db->query(
            'UPDATE totp_users SET last_totp_login = ? WHERE o2user = ?',
            [$now, (string) $userId]
        );

        session_regenerate_id(true);
        $_SESSION[self::SESSION_STATE] = self::STATE_AUTHENTICATED;

        return true;
    }

    /**
     * Marks the current device as trusted for the user who just
     * completed the second factor.
     *
     * Call only after verifyTotp() — never after confirmTotpSetup(): a
     * user doing first-time setup hasn't yet chosen whether to trust
     * this browser long-term (see register.php). Pass the "remember me"
     * checkbox value from the OTP form.
     */
    public function setTrustedDevice(): void
    {
        $userId = $this->getCurrentUserId();
        if ($userId === null) {
            return; // Only callable once the second factor is satisfied
        }

        // Cryptographically secure random token (32 bytes = 256 bits).
        $token     = bin2hex(random_bytes(32));
        $tokenHash = hash('sha256', $token);
        $now       = time();
        $expiresAt = $now + self::TRUSTED_DEVICE_TTL;

        // Truncate user agent to 255 chars (audit only, not used in logic).
        $userAgent = substr($_SERVER['HTTP_USER_AGENT'] ?? '', 0, 255);

        $this->db->query(
            'INSERT INTO trusted_devices (user_id, token_hash, created_at, expires_at, user_agent) VALUES (?, ?, ?, ?, ?)',
            [$userId, $tokenHash, $now, $expiresAt, $userAgent]
        );

        // Same cookie security attributes as the session cookie (see _bootstrap.php).
        $secure = !empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off';
        setcookie(
            self::TRUSTED_DEVICE_COOKIE,
            $token,
            [
                'expires'  => $expiresAt,
                'path'     => '/',
                'secure'   => $secure,
                'httponly' => true,
                'samesite' => 'Strict',
            ]
        );
    }

    /**
     * Revokes all trusted devices for the user who completed the second
     * factor: deletes the DB records and clears the local cookie.
     *
     * Not currently called anywhere in this module — there's no
     * post-login area here any more, that's the ERP app's job now. Kept
     * available for a future management tool or ERP-side admin page.
     */
    public function revokeAllTrustedDevices(): void
    {
        $userId = $this->getCurrentUserId();
        if ($userId === null) {
            return;
        }

        $this->db->query(
            'DELETE FROM trusted_devices WHERE user_id = ?',
            [$userId]
        );

        $secure = !empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off';
        setcookie(
            self::TRUSTED_DEVICE_COOKIE,
            '',
            [
                'expires'  => time() - 3600,
                'path'     => '/',
                'secure'   => $secure,
                'httponly' => true,
                'samesite' => 'Strict',
            ]
        );
    }

    // SECTION 5: session state reads

    /**
     * Current session state, or STATE_NONE if no MFA challenge is in progress.
     */
    public function getState(): string
    {
        return $_SESSION[self::SESSION_STATE] ?? self::STATE_NONE;
    }

    /**
     * Whether the second factor has been satisfied for this session.
     */
    public function isAuthenticated(): bool
    {
        return $this->getState() === self::STATE_AUTHENTICATED;
    }

    /**
     * Username for which the second factor has been satisfied, or null
     * otherwise (unlike getChallengeUser(), which ignores state).
     */
    public function getCurrentUserId(): ?string
    {
        if (!$this->isAuthenticated()) {
            return null;
        }
        return isset($_SESSION[self::SESSION_USER]) ? (string) $_SESSION[self::SESSION_USER] : null;
    }

    // SECTION 6: logout

    /**
     * Fully invalidates this module's session.
     *
     * Known residual risk: in this deployment the PHP session is shared
     * with jxtotp.php's $_SESSION['o2_app'] (see class docblock) — if
     * this method is ever called from a path reached via jxtotp.php, it
     * would destroy Janox's application session data too, not just
     * ours. No such call path exists today (totp/logout.php, which used
     * to expose this, was removed as unreferenced). Review this before
     * wiring logout() to any route reachable from jxtotp.php.
     *
     * Note: the td_token (trusted device) cookie is NOT cleared here —
     * revoking trusted devices is a separate action
     * (revokeAllTrustedDevices()).
     */
    public function logout(): void
    {
        $_SESSION = [];

        if (ini_get('session.use_cookies')) {
            $p = session_get_cookie_params();
            setcookie(
                session_name(), '', time() - 42000,
                $p['path'], $p['domain'], $p['secure'], $p['httponly']
            );
        }

        session_destroy();
    }

    // SECTION 7: CSRF

    /**
     * Returns the CSRF token for the current session, generating one if
     * absent.
     */
    public function getCsrfToken(): string
    {
        if (empty($_SESSION[self::SESSION_CSRF])) {
            // 32 random bytes -> 64 hex chars, effectively unguessable.
            $_SESSION[self::SESSION_CSRF] = bin2hex(random_bytes(32));
        }
        return $_SESSION[self::SESSION_CSRF];
    }

    /**
     * Verifies the submitted CSRF token against the one stored in session.
     *
     * hash_equals() is timing-safe. The empty-stored-token case is
     * checked explicitly before it (see guard below), since
     * hash_equals('', '') === true in PHP — without that guard, a
     * request with no token in session and none in POST would
     * incorrectly pass verification.
     */
    public function verifyCsrf(string $token): bool
    {
        $stored = $_SESSION[self::SESSION_CSRF] ?? '';
        if ($stored === '') {
            // No token issued yet this session: hash_equals('', $token)
            // would return true if $token is also '' (e.g. a form that
            // omits the csrf_token field entirely). Reject explicitly
            // instead of letting that coincidence pass as valid.
            return false;
        }
        return hash_equals($stored, $token);
    }

    // SECTION 8: internal constants

    /**
     * TOTP time-step length in seconds; must match Totp::PERIOD.
     */
    private const TOTP_PERIOD = 30;
}
