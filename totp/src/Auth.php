<?php
declare(strict_types=1);

require_once __DIR__ . '/Database.php';
require_once __DIR__ . '/Totp.php';

/**
 * Classe principale del modulo JXTOTP — secondo fattore MFA per Janox/ERP.
 *
 * DIFFERENZA FONDAMENTALE rispetto al prototipo standalone originale:
 *   Questa classe NON gestisce più utenti, password, reset password né
 *   recupero via email — tutto questo resta interamente responsabilità di
 *   Janox. startMfaChallenge() viene chiamato da jxtotp.php SOLO DOPO che
 *   Janox ha già verificato username e password con il proprio meccanismo:
 *   non è compito di questa classe (né possibile, dato che non abbiamo
 *   accesso alle tabelle ERP) verificare le credenziali.
 *
 *   Di conseguenza, "STATE_AUTHENTICATED" in questa classe significa
 *   soltanto "il secondo fattore per questa sessione è stato soddisfatto" —
 *   NON che l'utente sia autenticato in un'applicazione. Quell'ultimo passo
 *   (l'accesso vero e proprio all'app ERP) resta sempre a Janox, tramite il
 *   proprio meccanismo di OTP interno (vedi il file jxtotp.php del framework).
 *
 * Gestisce una sessione PHP interamente AUTONOMA (vedi _bootstrap.php):
 *   jxtotp.php è la pagina di login stessa e gira PRIMA che qualunque
 *   sessione applicativa Janox esista — non c'è quindi nessuna sessione
 *   esterna a cui appoggiarsi. Questo modulo apre e gestisce la propria
 *   sessione esattamente come farebbe un'applicazione standalone.
 *
 * Flusso completo:
 *
 *   Setup del secondo fattore (prima volta per questo utente):
 *     startMfaChallenge() → STATE_PENDING_SETUP → [mostra QR] → confirmTotpSetup()
 *     → STATE_AUTHENTICATED
 *
 *   Verifica del secondo fattore (già configurato in precedenza):
 *     startMfaChallenge() → STATE_PENDING_TOTP → verifyTotp() → STATE_AUTHENTICATED
 *     [oppure, se il browser è già "attendibile"]
 *     startMfaChallenge() → STATE_PENDING_TOTP → checkTrustedDevice() → STATE_AUTHENTICATED
 *     (il codice OTP non viene nemmeno richiesto)
 *
 * Diagramma stati sessione:
 *
 *   NONE ──[startMfaChallenge() + TOTP non configurato]──→ PENDING_SETUP
 *   NONE ──[startMfaChallenge() + TOTP già configurato]───→ PENDING_TOTP
 *   PENDING_SETUP ──[confirmTotpSetup() OK]────────────────→ AUTHENTICATED
 *   PENDING_TOTP  ──[verifyTotp() OK]───────────────────────→ AUTHENTICATED
 *   PENDING_TOTP  ──[checkTrustedDevice() OK]───────────────→ AUTHENTICATED
 *   Qualsiasi ──[logout()]───────────────────────────────────→ NONE
 */
class Auth
{
    // ── Costanti di configurazione ─────────────────────────────────────────────

    /**
     * Numero massimo di codici OTP errati consecutivi per lo stesso account
     * prima del blocco temporaneo. Si applica solo in fase di verifica
     * (verifyTotp()): in fase di setup (confirmTotpSetup()) non c'è lockout,
     * perché l'account non è ancora abilitato al secondo fattore.
     */
    const MAX_ATTEMPTS   = 5;

    /** Durata del blocco account in secondi (900 = 15 minuti). */
    const LOCKOUT_SEC    = 900;

    /**
     * Numero massimo di tentativi OTP falliti dallo stesso IP prima del
     * blocco IP. Più alto del limite per-account (5) perché un IP può avere
     * molti utenti legittimi dietro un NAT o un proxy aziendale.
     */
    const IP_MAX_ATTEMPTS = 20;

    /** Durata del blocco IP in secondi (900 = 15 minuti). */
    const IP_LOCKOUT_SEC  = 900;

    /**
     * Finestra temporale per il conteggio dei tentativi falliti per IP
     * (3600 = 1 ora). I tentativi più vecchi di questa finestra vengono
     * dimenticati: il contatore riparte da 0, non si accumula all'infinito.
     */
    const IP_WINDOW_SEC   = 3600;

    /** Durata della sessione "dispositivo attendibile" in secondi (10 giorni). */
    const TRUSTED_DEVICE_TTL = 864000;

    /** Nome del cookie HTTP usato per il token del dispositivo attendibile. */
    const TRUSTED_DEVICE_COOKIE = 'td_token';

    // Chiavi usate nella superglobale $_SESSION della nostra sessione autonoma.
    // Prefissare con 'auth_' evita collisioni nell'improbabile caso in cui,
    // in futuro, questa sessione dovesse condividere il processo PHP con
    // altro codice che usa $_SESSION per scopi propri.
    const SESSION_USER  = 'auth_user_id';
    const SESSION_STATE = 'auth_state';
    const SESSION_CSRF  = 'auth_csrf';

    // Costanti per gli stati della macchina a stati della sessione.
    const STATE_NONE          = 'none';
    const STATE_PENDING_SETUP = 'pending_setup'; // TOTP non ancora configurato
    const STATE_PENDING_TOTP  = 'pending_totp';  // TOTP configurato, in attesa del codice
    const STATE_AUTHENTICATED = 'authenticated'; // secondo fattore soddisfatto

    private $db; // typed property omessa: compatibilità PHP 7.3 (typed properties → PHP 7.4+)

    public function __construct(Database $db)
    {
        $this->db = $db;
    }

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 1: Avvio della sfida MFA (chiamato da jxtotp.php)
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Avvia il flusso del secondo fattore per un utente la cui password è
     * GIÀ STATA VERIFICATA da Janox.
     *
     * Questo è l'unico punto di ingresso al modulo dal lato di jxtotp.php:
     * viene chiamato una sola volta, al primo hit, quando Janox ha appena
     * determinato che l'utente ha mfa='T' e la password è corretta.
     *
     * Decide autonomamente (senza che jxtotp.php debba saperlo o deciderlo)
     * se l'utente deve completare il setup TOTP (mai configurato prima) o
     * inserire un codice OTP (già configurato in precedenza), e imposta lo
     * stato di sessione di conseguenza. Questa decisione NON viene mai presa
     * o forzata da jxtotp.php: vive interamente qui.
     *
     * @param string $username Username Janox (già in minuscolo, stringa — mai intero)
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
     * Crea la riga in totp_users per questo utente, se non esiste già.
     *
     * Non esiste un metodo "createUser()" in questo modulo: gli utenti sono
     * creati da Janox, non da noi. Questo metodo si limita a garantire che
     * esista un posto dove conservare il secret TOTP di un utente Janox che
     * ha mfa='T' — viene chiamato al primo accesso di quell'utente, non alla
     * sua creazione nell'ERP (di cui questo modulo non è mai a conoscenza).
     *
     * Se la riga esiste già, non fa nulla: non rigenera mai un secret già
     * esistente (lo invaliderebbe, obbligando l'utente a rifare il setup).
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

        // Genera un secret TOTP unico per questo utente (160 bit casuali).
        // Verrà mostrato come QR code al primo setup — e mai più dopo.
        $secret = Totp::generateSecret();

        $this->db->query(
            'INSERT INTO totp_users (o2user, totp_secret, created_at) VALUES (?, ?, ?)',
            [$username, $secret, time()]
        );
    }

    /**
     * Indica se l'utente ha già completato il setup del secondo fattore.
     *
     * Restituisce false sia se la riga non esiste (l'utente non ha ancora
     * generato un secret) sia se esiste ma totp_confirmed=0 (setup avviato
     * ma non ancora confermato con il primo codice OTP).
     */
    public function isTotpConfigured(string $username): bool
    {
        $row = $this->db->query(
            'SELECT totp_confirmed FROM totp_users WHERE o2user = ?',
            [$username]
        )->fetch();

        return (bool) ($row && $row['totp_confirmed']);
    }

    /**
     * Restituisce lo username per cui è in corso (o è appena terminata) una
     * sfida MFA, leggendolo dalla nostra sessione autonoma — indipendentemente
     * dallo stato attuale (PENDING_SETUP, PENDING_TOTP o AUTHENTICATED).
     *
     * Usato dalla guardia in jxtotp.php per riconoscere se una richiesta in
     * arrivo sta continuando un flusso MFA già iniziato per LO STESSO utente
     * (e non, per errore, riprendendo un flusso abbandonato da un login
     * precedente con un altro username).
     */
    public function getChallengeUser(): ?string
    {
        return isset($_SESSION[self::SESSION_USER]) ? (string) $_SESSION[self::SESSION_USER] : null;
    }

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 2: Setup del secondo fattore (prima configurazione)
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Recupera i dati necessari per mostrare il QR code durante il setup TOTP.
     *
     * Restituisce null se:
     *   - L'utente non esiste ancora in totp_users (non dovrebbe succedere:
     *     startMfaChallenge() chiama sempre ensureTotpUser() prima).
     *   - Il setup è già stato completato (totp_confirmed = 1).
     *
     * Nel secondo caso il secret non viene restituito di proposito: dopo il
     * setup il QR non deve più essere visualizzato (nessun flusso di recupero
     * autenticatore in questo modulo — fuori ambito per questa fase).
     *
     * @return array{username: string, secret: string}|null
     */
    public function getTotpSetupData(string $userId): ?array
    {
        $user = $this->db->query(
            'SELECT o2user, totp_secret, totp_confirmed FROM totp_users WHERE o2user = ?',
            [$userId]
        )->fetch();

        if (!$user || $user['totp_confirmed']) {
            return null;
        }

        return [
            'username' => $user['o2user'],
            'secret'   => $user['totp_secret'],
        ];
    }

    /**
     * Conferma il setup TOTP verificando il primo codice OTP inserito dall'utente.
     *
     * Passo finale della configurazione: solo dopo questa verifica
     * totp_confirmed viene impostato a 1. In caso di successo, il secondo
     * fattore per questa sessione è considerato soddisfatto (STATE_AUTHENTICATED):
     * la conferma del primo codice valido È GIÀ, essa stessa, la prova che
     * l'utente possiede il dispositivo authenticator — non ha senso chiedere
     * un secondo codice immediatamente dopo (vedi register.php).
     *
     * Include la prevenzione del riutilizzo del codice OTP (Feature: single-use
     * token): se lo stesso codice viene inviato due volte nello stesso
     * intervallo di 30 secondi, il secondo invio viene rifiutato.
     *
     * Nessun lockout in questa fase: l'account non è ancora abilitato al TOTP,
     * quindi non c'è rischio di bruteforce con conseguenze reali — l'utente
     * può riprovare senza limiti se sbaglia a digitare il codice.
     */
    public function confirmTotpSetup(string $userId, string $code): bool
    {
        $user = $this->db->query(
            'SELECT totp_secret, totp_confirmed, last_totp_code, last_totp_window FROM totp_users WHERE o2user = ?',
            [$userId]
        )->fetch();

        // Non permettere di confermare due volte lo stesso setup.
        if (!$user || $user['totp_confirmed']) {
            return false;
        }

        // ── FEATURE: Single-use token ──────────────────────────────────────────
        // Stesso meccanismo di verifyTotp() più sotto: vedi quel PHPDoc per la
        // spiegazione completa del perché la finestra di confronto è ±1.
        $currentWindow = (int) (time() / self::TOTP_PERIOD);
        if ($user['last_totp_code'] !== null
            && $user['last_totp_code'] === $code
            && abs($currentWindow - (int) $user['last_totp_window']) <= 1) {
            return false; // Codice già usato in questa finestra temporale
        }

        if (!Totp::verify($user['totp_secret'], $code)) {
            return false;
        }

        // Setup completato: segna totp_confirmed=1, registra il codice usato
        // (anti-replay) e l'orario del primo accesso riuscito.
        $this->db->query(
            'UPDATE totp_users SET totp_confirmed = 1, last_totp_code = ?, last_totp_window = ?, last_totp_login = ? WHERE o2user = ?',
            [$code, $currentWindow, time(), $userId]
        );

        // PREVENZIONE SESSION FIXATION: rigenera l'ID di sessione ogni volta
        // che lo stato passa a "soddisfatto", esattamente come al successo di
        // verifyTotp()/checkTrustedDevice() più sotto.
        session_regenerate_id(true);
        $_SESSION[self::SESSION_STATE] = self::STATE_AUTHENTICATED;

        return true;
    }

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 3: Verifica del secondo fattore (già configurato)
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Verifica il codice TOTP inserito dall'utente.
     *
     * Può essere chiamato solo quando la sessione è in stato PENDING_TOTP,
     * cioè dopo che startMfaChallenge() ha determinato che il TOTP è già
     * configurato per questo utente. Se chiamato in un altro stato,
     * restituisce false: questo previene bypass del flusso.
     *
     * FEATURE: rate limiting a due livelli, invariato nello spirito dal
     * prototipo standalone (dove proteggeva i tentativi di password),
     * applicato qui ai tentativi di codice OTP:
     *   - per-ACCOUNT (failed_otp_attempts/otp_locked_until in totp_users):
     *     blocca l'account specifico dopo troppi codici errati consecutivi.
     *   - per-IP (tabella otp_rate_limits, via checkOtpRateLimit()): blocca
     *     l'indirizzo IP dopo troppi tentativi falliti, anche se distribuiti
     *     su più account — copre il caso di un attaccante che provi codici
     *     su utenti diversi dallo stesso IP.
     *
     * FEATURE: Single-use token (anti-replay) — lo stesso codice non può
     * essere usato due volte nello stesso intervallo di 30 secondi. Protegge
     * da un attaccante che intercettasse un codice valido e lo riusasse
     * immediatamente dopo l'utente legittimo.
     *
     * @param string $code     Codice OTP a 6 cifre inserito dall'utente
     * @param string $clientIp IP del client per il rate limiting per-IP.
     *                         Stringa vuota '' per disabilitare quel controllo
     *                         (utile in test automatici).
     */
    public function verifyTotp(string $code, string $clientIp = ''): bool
    {
        // Verifica che siamo nel giusto stato della macchina: impossibile
        // arrivare qui senza che Janox abbia già verificato la password e
        // senza che startMfaChallenge() abbia già determinato PENDING_TOTP.
        if ($this->getState() !== self::STATE_PENDING_TOTP) {
            return false;
        }

        // ── FEATURE: IP Rate Limiting ──────────────────────────────────────────
        // Controllato PRIMA di leggere il database utenti, per intercettare
        // il più presto possibile un IP già bloccato.
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

        // Blocco per-account attivo: non procedere nemmeno a verificare il codice.
        if ((int) $user['otp_locked_until'] > time()) {
            if ($clientIp) {
                $this->recordOtpFailedAttempt($clientIp);
            }
            return false;
        }

        // ── FEATURE: Single-use token ──────────────────────────────────────────
        // Perché ±1 finestra e non solo la finestra corrente?
        //   Totp::verify() accetta codici con uno scarto di ±30 secondi per
        //   gestire piccole differenze di orologio tra telefono e server. Se
        //   un codice era valido per la finestra W, è accettato anche nelle
        //   finestre W-1 e W+1: il blocco anti-replay deve coprire lo stesso
        //   intervallo, altrimenti resterebbe un buco nella protezione.
        $currentWindow = (int) (time() / self::TOTP_PERIOD);
        if ($user['last_totp_code'] !== null
            && $user['last_totp_code'] === $code
            && abs($currentWindow - (int) $user['last_totp_window']) <= 1) {
            return false; // Codice già usato: possibile tentativo di replay
        }

        if (!Totp::verify($user['totp_secret'], $code)) {
            // Incrementa il contatore dei tentativi falliti per questo account.
            $attempts  = (int) $user['failed_otp_attempts'] + 1;
            $lockUntil = $attempts >= self::MAX_ATTEMPTS ? time() + self::LOCKOUT_SEC : 0;
            $this->db->query(
                'UPDATE totp_users SET failed_otp_attempts = ?, otp_locked_until = ? WHERE o2user = ?',
                [$attempts, $lockUntil, $userId]
            );
            if ($clientIp) {
                $this->recordOtpFailedAttempt($clientIp);
            }
            return false;
        }

        // Codice corretto: azzera i contatori di lockout, registra il codice
        // usato (anti-replay) e l'orario dell'accesso riuscito.
        $this->db->query(
            'UPDATE totp_users SET failed_otp_attempts = 0, otp_locked_until = 0,
             last_totp_code = ?, last_totp_window = ?, last_totp_login = ? WHERE o2user = ?',
            [$code, $currentWindow, time(), $userId]
        );

        // PREVENZIONE SESSION FIXATION: rigenera l'ID di sessione ogni volta
        // che lo stato passa a "soddisfatto". Il parametro true elimina anche
        // il vecchio file di sessione dal disco, non solo ne cambia l'ID.
        session_regenerate_id(true);
        $_SESSION[self::SESSION_STATE] = self::STATE_AUTHENTICATED;

        return true;
    }

    /**
     * Controlla se un indirizzo IP può ancora tentare un codice OTP.
     * Restituisce true se l'IP non è bloccato (o non ha ancora tentativi registrati).
     */
    private function checkOtpRateLimit(string $ip): bool
    {
        $now = time();

        $row = $this->db->query(
            'SELECT attempts, window_start, locked_until FROM otp_rate_limits WHERE ip = ?',
            [$ip]
        )->fetch();

        if (!$row) {
            return true; // Nessun tentativo registrato per questo IP
        }

        if ((int) $row['locked_until'] > $now) {
            return false; // IP attualmente bloccato
        }

        if (($now - (int) $row['window_start']) >= self::IP_WINDOW_SEC) {
            return true; // Finestra scaduta: contatore "fresco"
        }

        return (int) $row['attempts'] < self::IP_MAX_ATTEMPTS;
    }

    /**
     * Registra un tentativo di codice OTP fallito per un indirizzo IP.
     *
     * Usa INSERT + UPDATE separati (non UPSERT/MERGE) per compatibilità con
     * SQLite, PostgreSQL e SqlServer senza dover cambiare la sintassi SQL in
     * caso di futura migrazione del database di questo modulo.
     */
    private function recordOtpFailedAttempt(string $ip): void
    {
        $now = time();

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

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 4: Dispositivi attendibili ("ricordami")
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Controlla se il dispositivo corrente è "attendibile" per l'utente in
     * PENDING_TOTP: se sì, salta interamente la richiesta del codice OTP.
     *
     * FEATURE: Trusted device — dopo un secondo fattore completato con
     * "ricordami" attivo, ai login successivi da quel browser il codice OTP
     * non viene più richiesto (per Auth::TRUSTED_DEVICE_TTL, 10 giorni).
     *
     * Come funziona tecnicamente:
     *   1. Al successo con "ricordami", generiamo un token casuale (32 byte).
     *      Il token grezzo va nel cookie del browser; l'hash SHA-256 va nel DB.
     *      (Se il DB venisse compromesso, gli hash da soli non basterebbero
     *      per ricostruire i token originali dei cookie.)
     *   2. A ogni accesso, dopo startMfaChallenge(), questo metodo cerca un
     *      cookie td_token, ne calcola l'hash, e lo cerca nel DB per l'utente
     *      in sessione.
     *   3. Se trovato e non scaduto, autentica direttamente senza richiedere OTP.
     *
     * @return bool true se il dispositivo è attendibile e il secondo fattore
     *              è stato soddisfatto senza richiedere alcun codice
     */
    public function checkTrustedDevice(): bool
    {
        // Ha senso solo in PENDING_TOTP: se il TOTP non è ancora configurato
        // (PENDING_SETUP) non può ancora esistere un dispositivo attendibile.
        if ($this->getState() !== self::STATE_PENDING_TOTP) {
            return false;
        }

        $token = $_COOKIE[self::TRUSTED_DEVICE_COOKIE] ?? '';
        if (!$token) {
            return false; // Nessun cookie di dispositivo attendibile presente
        }

        $userId = $_SESSION[self::SESSION_USER] ?? null;
        if (!$userId) {
            return false;
        }

        // hash('sha256', ...) è deterministico: lo stesso token produce sempre lo stesso hash.
        $hash = hash('sha256', $token);
        $now  = time();

        $device = $this->db->query(
            'SELECT id FROM trusted_devices WHERE user_id = ? AND token_hash = ? AND expires_at > ?',
            [$userId, $hash, $now]
        )->fetch();

        if (!$device) {
            // Cookie presente ma token non trovato (scaduto, revocato, o
            // contraffatto). Non fare nulla — l'utente vedrà normalmente il form OTP.
            return false;
        }

        // Dispositivo attendibile valido: registra l'accesso e completa il
        // secondo fattore senza aver mai richiesto un codice.
        $this->db->query(
            'UPDATE totp_users SET last_totp_login = ? WHERE o2user = ?',
            [$now, (string) $userId]
        );

        session_regenerate_id(true);
        $_SESSION[self::SESSION_STATE] = self::STATE_AUTHENTICATED;

        return true;
    }

    /**
     * Registra il dispositivo corrente come "attendibile" per l'utente che ha
     * appena completato con successo il secondo fattore.
     *
     * Va chiamato subito DOPO verifyTotp() (mai dopo confirmTotpSetup(): un
     * utente che sta configurando il TOTP per la prima volta non ha ancora
     * scelto se fidarsi di questo browser a lungo termine — vedi register.php),
     * passando la preferenza dell'utente dal checkbox "ricordami" del form OTP.
     *
     * Il token casuale generato qui viene messo nel cookie del browser e il
     * suo hash SHA-256 viene salvato nel database — vedi checkTrustedDevice()
     * per la spiegazione completa del perché si salva l'hash e non il token.
     */
    public function setTrustedDevice(): void
    {
        $userId = $this->getCurrentUserId();
        if ($userId === null) {
            return; // Chiamabile solo quando il secondo fattore è soddisfatto
        }

        // Genera un token casuale crittograficamente sicuro (32 byte = 256 bit).
        $token     = bin2hex(random_bytes(32));
        $tokenHash = hash('sha256', $token);
        $now       = time();
        $expiresAt = $now + self::TRUSTED_DEVICE_TTL;

        // Tronca l'user agent a 255 caratteri (solo per audit, non usato in logica).
        $userAgent = substr($_SERVER['HTTP_USER_AGENT'] ?? '', 0, 255);

        $this->db->query(
            'INSERT INTO trusted_devices (user_id, token_hash, created_at, expires_at, user_agent) VALUES (?, ?, ?, ?, ?)',
            [$userId, $tokenHash, $now, $expiresAt, $userAgent]
        );

        // Imposta il cookie con gli stessi attributi di sicurezza del cookie
        // di sessione (vedi _bootstrap.php).
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
     * Revoca tutti i dispositivi attendibili per l'utente che ha completato
     * il secondo fattore. Elimina i record dal DB e cancella il cookie locale.
     *
     * Non è ancora richiamato da nessuna pagina di questo modulo (non esiste
     * più un'area "post-login" nel nostro modulo: quel ruolo appartiene ora
     * all'app ERP). Mantenuto disponibile per un futuro strumento di gestione
     * o per un'eventuale pagina amministrativa lato ERP.
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

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 5: Lettura stato sessione
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Restituisce lo stato corrente della sessione.
     * Se la chiave non esiste (nessuna sfida MFA in corso), restituisce STATE_NONE.
     */
    public function getState(): string
    {
        return $_SESSION[self::SESSION_STATE] ?? self::STATE_NONE;
    }

    /**
     * Shortcut per verificare se il secondo fattore è stato soddisfatto per
     * questa sessione.
     */
    public function isAuthenticated(): bool
    {
        return $this->getState() === self::STATE_AUTHENTICATED;
    }

    /**
     * Restituisce lo username per cui il secondo fattore è stato soddisfatto,
     * o null se non lo è (a differenza di getChallengeUser(), che restituisce
     * lo username indipendentemente dallo stato).
     */
    public function getCurrentUserId(): ?string
    {
        if (!$this->isAuthenticated()) {
            return null;
        }
        return isset($_SESSION[self::SESSION_USER]) ? (string) $_SESSION[self::SESSION_USER] : null;
    }

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 6: Logout
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Invalida completamente la sessione autonoma di questo modulo.
     *
     * INVARIATO rispetto al prototipo standalone originale: qui è corretto
     * distruggere l'intera sessione (session_destroy() + cancellazione del
     * cookie), perché è la NOSTRA sessione autonoma — non è mai condivisa con
     * Janox (vedi _bootstrap.php). Non c'è quindi il rischio, che ci sarebbe
     * stato se avessimo condiviso la sessione applicativa di Janox, di
     * distruggere dati di sessione che non ci appartengono.
     *
     * Nota: il cookie td_token (dispositivo attendibile) NON viene eliminato
     * al logout — la revoca esplicita dei dispositivi fidati è un'azione
     * separata (revokeAllTrustedDevices()).
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

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 7: CSRF
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Restituisce il token CSRF per la sessione corrente (ne genera uno se non esiste).
     *
     * CSRF (Cross-Site Request Forgery): un sito malevolo potrebbe indurre il
     * browser dell'utente a inviare una richiesta autenticata al nostro
     * endpoint (es. tramite un form nascosto). Il browser include
     * automaticamente i cookie di sessione, quindi il server non può
     * distinguere la richiesta legittima da quella forgiata senza un token
     * segreto noto solo al server e al client legittimo.
     */
    public function getCsrfToken(): string
    {
        if (empty($_SESSION[self::SESSION_CSRF])) {
            // 32 byte casuali → 64 caratteri hex → token praticamente impossibile da indovinare.
            $_SESSION[self::SESSION_CSRF] = bin2hex(random_bytes(32));
        }
        return $_SESSION[self::SESSION_CSRF];
    }

    /**
     * Verifica che il token CSRF inviato dal form corrisponda a quello in sessione.
     *
     * hash_equals() fa un confronto timing-safe: impiega sempre lo stesso
     * tempo indipendentemente da quanti caratteri iniziali coincidono. Questo
     * previene attacchi che tentano di indovinare il token misurando il
     * tempo di risposta.
     */
    public function verifyCsrf(string $token): bool
    {
        $stored = $_SESSION[self::SESSION_CSRF] ?? '';
        return hash_equals($stored, $token);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // SEZIONE 8: Costanti interne (non configurabili dall'esterno)
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * Durata di un intervallo temporale TOTP in secondi.
     * Usata internamente per calcolare il numero di finestra corrente.
     * Deve corrispondere a Totp::PERIOD.
     */
    private const TOTP_PERIOD = 30;
}
