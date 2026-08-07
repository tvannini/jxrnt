<?php
declare(strict_types=1);
require_once __DIR__ . '/_bootstrap.php';

/**
 * TOTP second-factor setup (embedded page). Same return contract as
 * index.php toward jxtotp.php: success returns true with no output;
 * otherwise renders the form/error and calls die(). See index.php PHPDoc
 * for the full contract.
 *
 * By the time this file is included, jxtotp.php has already called
 * Auth::startMfaChallenge(), which in turn called ensureTotpUser(), so the
 * totp_users row and secret already exist.
 */

// Always expects STATE_PENDING_SETUP; jxtotp.php decides when to include
// this file instead of index.php.
$userId = $auth->getChallengeUser();
if ($userId === null) {
    // Reached without a valid MFA session (e.g. cookie lost mid-flow) — fail closed.
    http_response_code(400);
    die('Sessione di setup non valida. Ripeti il login.');
}

$error = '';

// POST: user scanned the QR and is submitting the first OTP code.
// jxtotp.php includes this file within the same POST as the Janox login
// form, so checking REQUEST_METHOD alone would misfire on first hit (no
// otp_code yet). isset($_POST['otp_code']) reliably distinguishes our own
// form submission from that inherited request (no collision with Janox's
// user/password/jxapp/jxotp fields).
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['otp_code'])) {
    if (!$auth->verifyCsrf($_POST['csrf_token'] ?? '')) {
        $error = 'Richiesta non valida.';
    } else {
        $code = trim($_POST['otp_code'] ?? '');
        if ($auth->confirmTotpSetup($userId, $code)) {

            // Confirming the first valid OTP is itself proof of device
            // possession, so no immediate re-challenge. No "remember me"
            // here — the user hasn't had a chance to opt in yet; that's
            // offered starting from the next login via index.php.
            return true;
        }

        // No attempt limit during setup: TOTP isn't enabled yet, so there's
        // no lockout risk to manage here (see Auth::confirmTotpSetup()).
        $error = 'Codice OTP non valido. Riprova.';
    }
}

// QR generated in memory as PNG bytes, embedded as a base64 data URI — no
// external calls, the TOTP secret never leaves the server.
$setupData = $auth->getTotpSetupData($userId);
if ($setupData === null) {
    // Setup already completed elsewhere (e.g. two open tabs on the same
    // login) — nothing left to show; jxtotp.php will see the updated state
    // on the next request.
    http_response_code(409);
    die('Setup già completato.');
}

if (isset($_SESSION['o2_app'])) {
    $label = h($_SESSION['o2_app']->title ?? 'Janox TOTP');
    }
else {
    $label = 'Janox TOTP';
    }

$totpUri   = Totp::getUri($setupData['secret'], $setupData['username'], $label);
$qrPng     = QrCode::generate($totpUri, 6, 4);
$qrDataUri = 'data:image/png;base64,' . base64_encode($qrPng);

$csrf         = $auth->getCsrfToken();
$preserveUser = h((string) ($_REQUEST['user']  ?? ''));
$preserveApp  = h((string) ($_REQUEST['jxapp'] ?? ''));
?>
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Configura MFA</title>
<style>
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
body{font-family:"Segoe UI",system-ui,sans-serif;background:#F0F4FA;color:#12213A;min-height:100vh;display:flex;align-items:flex-start;justify-content:center;padding:clamp(24px,5vh,60px) 20px 40px}
.page-wrap{width:100%;max-width:440px}
.card{background:#fff;border-radius:12px;box-shadow:0 4px 24px rgba(0,0,0,.10);overflow:hidden}
.card-header{background:#0D1B2A;color:#E8EDF5;padding:28px 32px 20px;text-align:center}
.card-icon{font-size:1.8rem;margin-bottom:8px}
.card-header h1{font-size:1.35rem;font-weight:700}
.card-subtitle{font-size:.84rem;color:#8EA8C0;margin-top:8px;line-height:1.5}
.card-body{padding:26px 32px 30px}
.form-group{margin-bottom:16px}
label{display:block;font-size:.82rem;font-weight:600;color:#3A4F6A;margin-bottom:5px}
.input{width:100%;padding:11px 14px;border:1.5px solid #D0D8EC;border-radius:7px;font-size:.93rem;color:#12213A;outline:none;transition:border-color .2s}
.input:focus{border-color:#3D7EAA;box-shadow:0 0 0 3px rgba(61,126,170,.12)}
.input-otp{font-size:1.5rem;letter-spacing:.3em;text-align:center;font-family:"Courier New",monospace;font-weight:700}
.btn{display:inline-flex;align-items:center;justify-content:center;padding:11px 24px;border-radius:7px;font-size:.93rem;font-weight:600;cursor:pointer;border:none;text-decoration:none;transition:background .18s}
.btn-primary{background:#E84855;color:#fff}
.btn-primary:hover{background:#C93340}
.btn-full{width:100%;margin-top:6px}
.alert{padding:11px 14px;border-radius:7px;font-size:.87rem;margin-bottom:16px;line-height:1.5}
.alert-error{background:#FEE7E9;color:#9B1C28;border:1px solid #FBC8CD}
.qr-wrap{text-align:center;padding:16px 0 8px}
.qr-wrap img{border:6px solid #fff;box-shadow:0 2px 12px rgba(0,0,0,.15);border-radius:4px;max-width:200px}
.secret-box{background:#F0F4FA;border:1px solid #D0D8EC;border-radius:6px;padding:10px 14px;font-family:"Courier New",monospace;font-size:.82rem;word-break:break-all;color:#3A4F6A;text-align:center;margin:12px 0}
.step-hint{font-size:.85rem;color:#3A4F6A;line-height:1.6;margin-bottom:16px}
.step-hint ol{padding-left:18px;margin-top:6px}
.step-hint li{margin-bottom:4px}
</style>
</head>
<body>
<div class="page-wrap">
<div class="card">
  <div class="card-header">
    <div class="card-icon">&#128241;</div>
    <h1>Configura MFA</h1>
    <p class="card-subtitle">Scansiona il QR code con la tua app authenticator.</p>
  </div>
  <div class="card-body">
    <?php if ($error): ?>
      <div class="alert alert-error"><?php echo h($error); ?></div>
    <?php endif; ?>

    <div class="qr-wrap">
      <img src="<?php echo $qrDataUri; ?>" alt="QR Code TOTP" width="200" height="200">
    </div>
    <p style="font-size:.78rem;color:#8892A4;text-align:center;margin-bottom:8px">
      Oppure inserisci il codice manuale:
    </p>
    <div class="secret-box"><?php echo h(implode(' ', str_split($setupData['secret'], 4))); ?></div>

    <div class="step-hint">
      <strong>Istruzioni:</strong>
      <ol>
        <li>Apri la tua app authenticator (Google Authenticator, Microsoft Authenticator, Authy…)</li>
        <li>Aggiungi un nuovo account scansionando il QR code sopra</li>
        <li>Inserisci il codice a 6 cifre mostrato dall'app per confermare</li>
      </ol>
    </div>

    <form method="post" action="" autocomplete="off">
      <input type="hidden" name="csrf_token" value="<?php echo h($csrf); ?>">
      <input type="hidden" name="user" value="<?php echo $preserveUser; ?>">
      <input type="hidden" name="jxapp" value="<?php echo $preserveApp; ?>">
      <div class="form-group">
        <label for="otp_code">Codice OTP di conferma</label>
        <input type="text" id="otp_code" name="otp_code"
               class="input input-otp"
               inputmode="numeric" pattern="\d{6}" maxlength="6"
               placeholder="000000" required autofocus>
      </div>
      <button type="submit" class="btn btn-primary btn-full">Conferma e completa setup</button>
    </form>
  </div>
</div>
</div>
</body>
</html>
<?php
die();
