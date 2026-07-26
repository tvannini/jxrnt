<?php
declare(strict_types=1);

/**
 * Bootstrap del modulo JXTOTP.
 *
 * Incluso da jxtotp.php (il punto di aggancio nel framework Janox) e da
 * tutte le pagine embedded di questa cartella (index.php, register.php,
 * logout.php, qrcode.php).
 *
 * FEATURE: sessione PHP autonoma.
 *   Questo modulo gestisce una PROPRIA sessione, del tutto indipendente da
 *   qualsiasi sessione applicativa Janox. Il motivo: jxtotp.php È la pagina
 *   di login stessa e gira PRIMA che qualunque sessione Janox esista — non
 *   c'è quindi nulla a cui "agganciarsi". La sessione va creata e gestita
 *   qui esattamente come farebbe un'applicazione standalone qualsiasi.
 *
 * Si occupa di:
 *   1. Caricare le classi del modulo (Database, Totp, QrCode, Auth).
 *   2. Configurare i parametri di sicurezza della sessione PRIMA di avviarla.
 *   3. Avviare la sessione, se non è già attiva in questa richiesta.
 *   4. Istanziare $db e $auth, condivisi da tutte le pagine che includono questo file.
 *   5. Definire le funzioni helper h(), redirect() e getClientIp().
 *
 * Struttura di questa cartella — layout piatto, nessun livello htdocs/auth:
 * nessuno di questi file è raggiungibile via URL diretta, solo jxtotp.php lo è
 * (questa cartella non è in nessuna document root):
 *   totp/
 *   ├── _bootstrap.php   ← questo file
 *   ├── index.php        ← verifica codice OTP (embedded)
 *   ├── register.php     ← setup TOTP (embedded)
 *   ├── logout.php        ← invalida la sessione MFA (utility)
 *   ├── qrcode.php         ← utility di generazione QR (non esposta via URL)
 *   ├── data/auth.db        ← SQLite, creato automaticamente al primo avvio
 *   └── src/                ← classi PHP (Auth, Database, Totp, QrCode)
 */

// Percorso del database SQLite: direttamente sotto questa cartella, nessun
// calcolo di percorso relativo a più livelli (il layout è piatto).
define('DB_PATH', __DIR__ . '/data/auth.db');

require_once __DIR__ . '/src/Database.php';
require_once __DIR__ . '/src/Totp.php';
require_once __DIR__ . '/src/QrCode.php';
require_once __DIR__ . '/src/Auth.php';

// ── Configurazione sessione sicura ────────────────────────────────────────────
// Queste impostazioni DEVONO essere applicate prima di session_start().

// HTTPOnly: il cookie di sessione non è accessibile a JavaScript.
// Previene il furto del cookie tramite XSS (Cross-Site Scripting).
ini_set('session.cookie_httponly', '1');

// SameSite=Strict: il browser non invia il cookie in richieste cross-site.
// Prima linea di difesa contro CSRF (Cross-Site Request Forgery).
// "Strict" è più restrittivo di "Lax": il cookie non viene inviato nemmeno
// seguendo un link esterno al sito. Va bene per un modulo di autenticazione.
ini_set('session.cookie_samesite', 'Strict');

// use_strict_mode: il server rifiuta session ID che non ha creato lui stesso.
// Previene session fixation: un attaccante non può forzare un ID di sessione
// specifico inviando alla vittima un URL con un session id predisposto.
ini_set('session.use_strict_mode', '1');

// Secure: il cookie viene trasmesso solo su HTTPS.
// Attivato automaticamente solo quando il server è raggiunto tramite HTTPS
// (in sviluppo locale su HTTP questo flag resta disattivato: altrimenti il
// cookie non verrebbe mai inviato dal browser).
if (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') {
    ini_set('session.cookie_secure', '1');
}

// Avvia la sessione solo se non è già attiva in questa richiesta.
//
// Questo file viene incluso incondizionatamente in cima alla funzione
// app_check_user() di jxtotp.php su OGNI richiesta (sia la prima che le
// successive di uno stesso flusso MFA): la prima volta questa riga crea la
// sessione e il relativo cookie; le volte successive la riprende grazie al
// cookie che il browser rimanda automaticamente — è così che lo stato
// PENDING_SETUP/PENDING_TOTP sopravvive tra una richiesta HTTP e la successiva,
// nonostante ogni richiesta esegua questo script PHP da capo.
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

// ── Istanze condivise ─────────────────────────────────────────────────────────
$db   = new Database(DB_PATH);
$auth = new Auth($db);

/**
 * Escape HTML sicuro per l'output a schermo.
 *
 * DA USARE SEMPRE per qualsiasi stringa proveniente da input utente, database,
 * o qualsiasi fonte esterna prima di includerla nell'HTML.
 * Converte caratteri speciali HTML (<, >, ", ', &) nelle loro entità equivalenti,
 * prevenendo XSS (Cross-Site Scripting).
 *
 * Esempio: h('<script>alert(1)</script>') → '&lt;script&gt;alert(1)&lt;/script&gt;'
 */
function h(string $s): string
{
    return htmlspecialchars($s, ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
}

/**
 * Redirect HTTP con terminazione immediata dello script.
 *
 * Usa sempre redirect() invece di header() diretto: garantisce che exit sia
 * sempre chiamato, evitando che il codice successivo al redirect venga
 * eseguito (un errore comune che può portare a vulnerabilità di sicurezza).
 */
function redirect(string $url): void
{
    header('Location: ' . $url);
    exit;
}

/**
 * Restituisce l'indirizzo IP del client, usato per il rate limiting sui
 * tentativi di codice OTP (vedi Auth::verifyTotp()).
 *
 * In ambienti con proxy o load balancer (Nginx, Apache, CDN), l'IP reale del
 * client non è in REMOTE_ADDR ma nell'header HTTP_X_FORWARDED_FOR.
 *
 * ATTENZIONE: HTTP_X_FORWARDED_FOR può essere falsificato da un client
 * malevolo se il server non è dietro un proxy fidato. Va abilitato il
 * controllo su X-Forwarded-For solo se il server è effettivamente dietro un
 * reverse proxy di cui ci si fida (un client che si connette direttamente
 * potrebbe altrimenti mettere un IP falso in quell'header).
 *
 * Formato di X-Forwarded-For: "client, proxy1, proxy2" (più IP separati da
 * virgola). Prendiamo il primo (l'IP del client originale aggiunto dal primo proxy).
 */
function getClientIp(): string
{
    if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
        $parts = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR']);
        $ip    = trim($parts[0]);
        if (filter_var($ip, FILTER_VALIDATE_IP)) {
            return $ip;
        }
    }
    return $_SERVER['REMOTE_ADDR'] ?? '0.0.0.0';
}
