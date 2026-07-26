<?php
declare(strict_types=1);

/**
 * Wrapper leggero su PDO SQLite — modulo JXTOTP (ambito solo secondo fattore MFA).
 *
 * Responsabilità:
 *   1. Aprire (o creare) il file di database SQLite al percorso indicato.
 *   2. Creare lo schema (tutte le tabelle) se non esiste ancora.
 *   3. Esporre un metodo query() che usa *sempre* prepared statements, prevenendo SQL injection.
 *
 * FEATURE: nessuna logica di migrazione.
 *   A differenza del prototipo standalone originale (che gestiva un array
 *   $migrations con ALTER TABLE per evolvere uno schema esistente senza
 *   perdere dati), questo modulo riparte con un database completamente
 *   vuoto: non contiene più alcun dato dell'ERP (utenti, password, email,
 *   scadenze account — tutto questo resta interamente in Janox). Eventuali
 *   evoluzioni future dello schema di QUESTO modulo saranno gestite con uno
 *   strumento esterno dedicato, non qui: per questo motivo NON va reintrodotto
 *   il meccanismo ALTER TABLE del prototipo.
 *
 * Perché SQLite?
 *   Nessun server separato da installare o configurare: il database è un
 *   singolo file su disco, satellite rispetto al database applicativo di
 *   Janox. Questo modulo non scrive né legge MAI le tabelle ERP (incluso
 *   o2admin_users): comunica con Janox esclusivamente tramite lo username
 *   (una semplice stringa) passato dal punto di aggancio in jxtotp.php.
 */
class Database
{
    private $pdo; // typed property omessa: compatibilità PHP 7.3 (typed properties → PHP 7.4+)

    public function __construct(string $dbPath)
    {
        // Crea la directory data/ se non esiste ancora (al primissimo avvio).
        $dir = dirname($dbPath);
        if (!is_dir($dir)) {
            mkdir($dir, 0755, true);
        }

        // PDO apre il file SQLite e lo crea automaticamente se non esiste.
        //
        //   ERRMODE_EXCEPTION → ogni errore SQL lancia un'eccezione invece di
        //     fallire silenziosamente: un bug non passa inosservato.
        //   FETCH_ASSOC → i risultati arrivano come array associativo
        //     (es. $row['o2user'] invece di $row[0]), più leggibile e robusto
        //     se in futuro cambiasse l'ordine delle colonne.
        $this->pdo = new PDO('sqlite:' . $dbPath, null, null, [
            PDO::ATTR_ERRMODE            => PDO::ERRMODE_EXCEPTION,
            PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
        ]);

        // WAL (Write-Ahead Logging): letture e scritture concorrenti non si
        // bloccano a vicenda. Utile perché più richieste HTTP verso jxtotp.php
        // (una per ogni utente che sta completando il secondo fattore) possono
        // arrivare quasi in contemporanea.
        $this->pdo->exec('PRAGMA journal_mode=WAL');

        // Abilita l'integrità referenziale (vincoli FOREIGN KEY), disattivata
        // di default da SQLite per compatibilità con versioni molto vecchie.
        // Serve per il FOREIGN KEY di trusted_devices verso totp_users.
        $this->pdo->exec('PRAGMA foreign_keys=ON');

        $this->initSchema();
    }

    /**
     * Crea le tabelle se non esistono ancora.
     *
     * Idempotente: CREATE TABLE IF NOT EXISTS non ha alcun effetto se le
     * tabelle esistono già — può essere chiamata a ogni avvio senza rischi.
     * Nessuna migrazione: vedi la nota FEATURE nel PHPDoc della classe.
     */
    private function initSchema(): void
    {
        // ── Tabella totp_users ──────────────────────────────────────────────
        //
        // Una riga per ogni utente Janox che ha (o starà per avere) il secondo
        // fattore TOTP configurato. La riga viene creata al primo accesso in
        // cui Janox segnala mfa='T' per quell'utente (vedi Auth::ensureTotpUser()),
        // NON quando l'utente viene creato nell'ERP: questo modulo non sa —
        // e non deve sapere — quando un utente ERP viene creato.
        //
        // o2user è la chiave primaria e contiene lo stesso valore o2user della
        // tabella ERP o2admin_users, ma qui NON esiste alcun legame fisico
        // (niente FOREIGN KEY verso un altro database): è semplicemente la
        // stessa stringa, usata come chiave di corrispondenza tra i due mondi.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS totp_users (
                -- Username Janox. Sempre stringa (tipo Janox 'jxuser', L50
                -- caratteri): non applicare MAI un cast (int) a questo valore.
                o2user               TEXT    NOT NULL,

                -- Secret TOTP in Base32 (160 bit), generato alla creazione
                -- della riga da ensureTotpUser(). Salvato in chiaro: il file
                -- auth.db va protetto a livello di permessi del filesystem.
                totp_secret          TEXT    NOT NULL DEFAULT '',

                -- 0 = l'utente non ha ancora scansionato il QR e confermato il
                --     primo codice OTP (setup incompleto).
                -- 1 = setup completato, il secondo fattore è attivo.
                totp_confirmed       INTEGER NOT NULL DEFAULT 0,

                -- Anti-replay: ultimo codice OTP accettato (in setup o in
                -- verifica) e la finestra temporale (floor(unix_time/30)) in
                -- cui è stato accettato. Impedisce che lo stesso codice,
                -- eventualmente intercettato, venga riutilizzato.
                last_totp_code       TEXT,
                last_totp_window     INTEGER,

                -- Lockout per-account: tentativi OTP falliti consecutivi e
                -- timestamp Unix di sblocco (0 = nessun blocco attivo).
                -- Si azzerano entrambi a ogni verifica riuscita.
                failed_otp_attempts  INTEGER NOT NULL DEFAULT 0,
                otp_locked_until     INTEGER NOT NULL DEFAULT 0,

                -- Unix timestamp di creazione della riga (= primo accesso
                -- con mfa='T' per questo utente).
                created_at           INTEGER NOT NULL DEFAULT 0,

                -- Unix timestamp dell'ultimo secondo fattore completato con
                -- successo. Solo per audit locale a questo modulo: Janox
                -- gestisce autonomamente il proprio last_date/last_time.
                last_totp_login      INTEGER,

                PRIMARY KEY (o2user)
            )
        ");

        // ── Tabella trusted_devices ─────────────────────────────────────────
        //
        // FEATURE: dispositivo attendibile ("ricordami").
        // Dopo un secondo fattore completato con successo, l'utente può
        // scegliere di non dover reinserire il codice OTP per un certo
        // periodo (Auth::TRUSTED_DEVICE_TTL) quando accede dallo stesso browser.
        //
        // Sicurezza:
        //   - Si salva solo l'hash SHA-256 del token: il token grezzo vive
        //     esclusivamente nel cookie del browser dell'utente. Se questo
        //     database venisse compromesso, gli hash da soli non permettono
        //     di ricostruire cookie validi.
        //   - user_id è TEXT (non INTEGER): referenzia o2user, chiave stringa.
        //     Nessun cast (int) va mai applicato a questo campo.
        //   - ON DELETE CASCADE: se la riga corrispondente in totp_users viene
        //     eliminata, tutti i suoi dispositivi attendibili la seguono
        //     automaticamente — nessun dato orfano.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS trusted_devices (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id     TEXT    NOT NULL,
                token_hash  TEXT    NOT NULL UNIQUE,
                created_at  INTEGER NOT NULL,
                -- Scadenza Unix timestamp (Auth::TRUSTED_DEVICE_TTL = 10 giorni).
                expires_at  INTEGER NOT NULL,
                -- User-Agent al momento della creazione: solo per audit,
                -- non viene mai usato in nessuna logica di autenticazione.
                user_agent  TEXT    NOT NULL DEFAULT '',
                FOREIGN KEY (user_id) REFERENCES totp_users(o2user) ON DELETE CASCADE
            )
        ");

        // ── Tabella otp_rate_limits ─────────────────────────────────────────
        //
        // FEATURE: rate limiting per IP sui tentativi di codice OTP.
        // La password è già stata verificata da Janox prima che questo modulo
        // entri in gioco — ma un attaccante che fosse comunque riuscito a
        // superare quel controllo (o che stia semplicemente tentando la
        // fortuna) potrebbe ancora provare a indovinare il codice OTP a 6
        // cifre. Questa tabella blocca un IP dopo troppi tentativi falliti,
        // indipendentemente dall'account preso di mira — copre il caso in cui
        // lo stesso IP tenti codici diversi su utenti diversi.
        $this->pdo->exec("
            CREATE TABLE IF NOT EXISTS otp_rate_limits (
                ip           TEXT    PRIMARY KEY,
                attempts     INTEGER NOT NULL DEFAULT 0,
                -- Unix timestamp di inizio della finestra di conteggio corrente.
                window_start INTEGER NOT NULL DEFAULT 0,
                -- Unix timestamp oltre il quale l'IP è bloccato. 0 = nessun blocco.
                locked_until INTEGER NOT NULL DEFAULT 0
            )
        ");
    }

    /** Espone l'oggetto PDO grezzo (usato raramente — preferire query()). */
    public function getPdo(): PDO
    {
        return $this->pdo;
    }

    /**
     * Esegue una query SQL con parametri sicuri (prepared statement).
     *
     * I parametri sono passati come array separato e mai concatenati alla
     * stringa SQL: questo è il modo corretto per prevenire SQL injection, il
     * driver PDO tratta i parametri come puri dati, non come codice SQL,
     * indipendentemente dal loro contenuto.
     *
     *   SICURO:   $db->query('SELECT * FROM totp_users WHERE o2user = ?', [$user]);
     *   INSICURO: $db->query("SELECT * FROM totp_users WHERE o2user = '$user'");
     *               ↑ se $user fosse "' OR '1'='1", l'intera tabella verrebbe esposta.
     *
     * Lancia PDOException in caso di errore SQL (es. constraint UNIQUE violato).
     */
    public function query(string $sql, array $params = []): PDOStatement
    {
        $stmt = $this->pdo->prepare($sql);
        $stmt->execute($params);
        return $stmt;
    }
}
