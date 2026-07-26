<?php
declare(strict_types=1);
require_once __DIR__ . '/_bootstrap.php';

/**
 * Verifica del secondo fattore TOTP — pagina EMBEDDED.
 *
 * Non è mai raggiungibile via URL diretta: questa cartella non è in nessuna
 * document root di alcun web server. Viene raggiunta SOLO tramite
 * include_once da jxtotp.php, dopo che Auth::startMfaChallenge() ha già
 * determinato lo stato STATE_PENDING_TOTP per l'utente in sessione (oppure
 * quando la guardia in jxtotp.php riconosce che questa richiesta prosegue un
 * flusso già iniziato).
 *
 * FEATURE: contratto di ritorno verso jxtotp.php.
 *   Questo file non stampa mai una pagina "autenticato" e non decide da solo
 *   come completare il login verso l'applicazione ERP: quella parte resta
 *   interamente a Janox (tramite il proprio meccanismo di OTP interno). Il
 *   contratto è semplice:
 *
 *     - Secondo fattore NON ancora soddisfatto (nessun codice inviato, o
 *       codice errato): stampa il form/errore e termina con die(). La
 *       risposta HTTP finisce qui — jxtotp.php non riprende il controllo.
 *
 *     - Secondo fattore SODDISFATTO (codice corretto, oppure dispositivo già
 *       "attendibile"): NON stampa nulla e termina con `return true;`. Quel
 *       valore diventa il valore restituito dall'include_once in jxtotp.php,
 *       che a quel punto — e solo a quel punto — decide se e come procedere
 *       con il proprio meccanismo di login (OTP interno di Janox).
 *
 *   Questo file non chiama MAI app_login()/app_generate_otp() direttamente:
 *   lo fa sempre e solo jxtotp.php, dopo aver controllato il valore restituito
 *   qui sotto. Manteniamo così il nostro TOTP e l'OTP interno di Janox come
 *   due concetti concettualmente separati, senza commistioni.
 */

// ── FEATURE: dispositivo attendibile ("ricordami") ──────────────────────────
// Primo controllo, prima di mostrare qualunque form: se questo browser ha
// già un cookie di dispositivo attendibile valido per l'utente in sessione,
// checkTrustedDevice() porta direttamente lo stato a STATE_AUTHENTICATED e
// l'utente non deve inserire nessun codice OTP in questo accesso.
if ($auth->checkTrustedDevice()) {
    return true;
}

$error = '';

// ── POST: l'utente ha inserito un codice OTP ────────────────────────────────
if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    // Verifica CSRF prima di qualunque elaborazione: previene che un sito
    // terzo induca il browser a inviare qui una richiesta non voluta.
    if (!$auth->verifyCsrf($_POST['csrf_token'] ?? '')) {
        $error = 'Richiesta non valida.';
    } else {
        $code     = trim($_POST['otp_code'] ?? '');
        $clientIp = getClientIp();

        // verifyTotp() include internamente sia il lockout per-account sia il
        // rate limiting per-IP (vedi Auth::verifyTotp()): un attaccante non
        // può tentare in modo illimitato di indovinare il codice a 6 cifre.
        if ($auth->verifyTotp($code, $clientIp)) {

            // ── FEATURE: imposta dispositivo attendibile se richiesto ────────
            // Se l'utente ha spuntato "ricordami", memorizziamo questo browser
            // come attendibile: ai prossimi accessi (fino a 10 giorni) il
            // codice OTP non verrà richiesto di nuovo (vedi il controllo in
            // testa a questo file).
            if (!empty($_POST['remember_device'])) {
                $auth->setTrustedDevice();
            }

            // Secondo fattore soddisfatto: nessun output, nessun die().
            // L'esecuzione ritorna a jxtotp.php, che decide come proseguire.
            return true;
        }

        $error = 'Codice OTP non valido, scaduto, o già usato.';
    }
}

// ── Nessun secondo fattore soddisfatto: mostra il form OTP ──────────────────
// Sia al primissimo hit (nessun POST ancora) sia dopo un codice errato.
$csrf = $auth->getCsrfToken();

// Campi da riproporre nel form: 'user' e 'jxapp' vengono letti di nuovo da
// jxtotp.php al prossimo hit e devono restare intatti fino al login finale
// verso l'app ERP (app_login() li rinvia come campi hidden). La password NON
// viene mai richiesta di nuovo: è già stata verificata da Janox al primo hit,
// e non è più presente in $_REQUEST da questo punto in avanti.
$preserveUser = h((string) ($_REQUEST['user']  ?? ''));
$preserveApp  = h((string) ($_REQUEST['jxapp'] ?? ''));
?>
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Verifica MFA</title>
<style>
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
body{font-family:"Segoe UI",system-ui,sans-serif;background:#F0F4FA;color:#12213A;min-height:100vh;display:flex;align-items:flex-start;justify-content:center;padding:clamp(24px,5vh,60px) 20px 40px}
.page-wrap{width:100%;max-width:440px}
.card{background:#fff;border-radius:12px;box-shadow:0 4px 24px rgba(0,0,0,.10);overflow:hidden}
.card-header{background:#0D1B2A;color:#E8EDF5;padding:28px 32px 20px;text-align:center}
.card-icon{font-size:2rem;margin-bottom:10px}
.card-header h1{font-size:1.4rem;font-weight:700;letter-spacing:-.02em}
.card-subtitle{font-size:.85rem;color:#8EA8C0;margin-top:8px;line-height:1.5}
.card-body{padding:28px 32px 32px}
.form-group{margin-bottom:18px}
label{display:block;font-size:.82rem;font-weight:600;color:#3A4F6A;margin-bottom:6px;letter-spacing:.02em}
.input{width:100%;padding:11px 14px;border:1.5px solid #D0D8EC;border-radius:7px;font-size:.95rem;color:#12213A;outline:none;transition:border-color .2s}
.input:focus{border-color:#3D7EAA;box-shadow:0 0 0 3px rgba(61,126,170,.12)}
.input-otp{font-size:1.6rem;letter-spacing:.3em;text-align:center;font-family:"Courier New",monospace;font-weight:700}
.btn{display:inline-flex;align-items:center;justify-content:center;padding:12px 24px;border-radius:7px;font-size:.95rem;font-weight:600;cursor:pointer;border:none;text-decoration:none;transition:background .18s}
.btn-primary{background:#E84855;color:#fff}
.btn-primary:hover{background:#C93340}
.btn-full{width:100%;margin-top:4px}
.alert{padding:11px 14px;border-radius:7px;font-size:.87rem;margin-bottom:18px;line-height:1.5}
.alert-error{background:#FEE7E9;color:#9B1C28;border:1px solid #FBC8CD}
</style>
</head>
<body>
<div class="page-wrap">
  <div class="card">
    <div class="card-header">
      <div class="card-icon">&#128241;</div>
      <h1>Verifica MFA</h1>
      <p class="card-subtitle">Apri la tua app authenticator e inserisci il codice a 6 cifre.</p>
    </div>
    <div class="card-body">
      <?php if ($error): ?>
        <div class="alert alert-error"><?php echo h($error); ?></div>
      <?php endif; ?>
      <form method="post" action="" autocomplete="off">
        <input type="hidden" name="csrf_token" value="<?php echo h($csrf); ?>">
        <!-- 'user' e 'jxapp' vengono rimandati intatti: servono a jxtotp.php
             sia per riconoscere che questo POST prosegue un flusso MFA già
             iniziato (vedi la guardia in app_check_user()), sia per il login
             finale verso l'app ERP (app_login() li rinvia come campi hidden). -->
        <input type="hidden" name="user" value="<?php echo $preserveUser; ?>">
        <input type="hidden" name="jxapp" value="<?php echo $preserveApp; ?>">
        <div class="form-group">
          <label for="otp_code">Codice OTP</label>
          <input type="text" id="otp_code" name="otp_code"
                 class="input input-otp"
                 inputmode="numeric" pattern="\d{6}" maxlength="6"
                 placeholder="000000" required autofocus>
        </div>
        <div class="form-group" style="display:flex;align-items:center;gap:8px;margin-top:4px">
          <input type="checkbox" id="remember_device" name="remember_device"
                 value="1" style="width:16px;height:16px;cursor:pointer">
          <label for="remember_device" style="margin:0;font-size:.86rem;font-weight:400;cursor:pointer;color:#3A4F6A">
            Considera questo dispositivo attendibile (non richiedere OTP per 10 giorni)
          </label>
        </div>
        <button type="submit" class="btn btn-primary btn-full">Verifica</button>
      </form>
    </div>
  </div>
</div>
</body>
</html>
<?php
// Risposta HTML già inviata: la richiesta termina qui, jxtotp.php non
// riprende il controllo (vedi il PHPDoc in testa a questo file).
die();
