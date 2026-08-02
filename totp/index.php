<?php
declare(strict_types=1);
require_once __DIR__ . '/_bootstrap.php';

/**
 * TOTP second-factor verification — embedded page, never reachable via direct URL.
 *
 * Included by jxtotp.php after Auth::startMfaChallenge() has set
 * STATE_PENDING_TOTP for the session user (or when jxtotp.php's guard
 * recognizes a continuation of an already-started flow).
 *
 * Return contract to jxtotp.php:
 * - Second factor NOT satisfied: prints the form/error and die()s.
 * - Second factor satisfied (valid code, or an already-trusted device):
 *   prints nothing and `return true`.
 */

// Trusted device ("remember me"): a valid cookie for the session user
// short-circuits straight to STATE_AUTHENTICATED, skipping the OTP form.
if ($auth->checkTrustedDevice()) {
    return true;
}

$error = '';

// Real OTP submission, not the inherited Janox login POST: jxtotp.php
// includes this file within the SAME request as the Janox login form, so
// REQUEST_METHOD is already 'POST' before any OTP code has been entered.
// Checking REQUEST_METHOD alone caused a premature "invalid code" error on
// first hit — keep isset($_POST['otp_code']) here, do not simplify it away.
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['otp_code'])) {

    // CSRF check before any processing.
    if (!$auth->verifyCsrf($_POST['csrf_token'] ?? '')) {
        $error = 'Richiesta non valida.';
    } else {
        $code     = trim($_POST['otp_code'] ?? '');
        $clientIp = getClientIp();

        // Enforces per-account lockout and per-IP rate limiting internally.
        if ($auth->verifyTotp($code, $clientIp)) {

            // Must run BEFORE setTrustedDevice(): it deletes ALL trusted_devices
            // rows for this user, so calling it after would silently wipe out
            // the device just registered below in the same login.
            if (!empty($_POST['revoke_devices'])) {
                $auth->revokeAllTrustedDevices();
            }

            // Marks this browser trusted for TRUSTED_DEVICE_TTL (10 days).
            if (!empty($_POST['remember_device'])) {
                $auth->setTrustedDevice();
            }

            // Second factor satisfied: no output, control returns to jxtotp.php.
            return true;
        }

        $error = 'Codice OTP non valido, scaduto, o già usato.';
    }
}

// No second factor satisfied yet (first hit, or a failed code): render the form.
$csrf = $auth->getCsrfToken();

// 'user'/'jxapp' are re-read by jxtotp.php on the next hit and forwarded to
// app_login() as hidden fields; password is never re-requested (already
// verified by Janox on the first hit, no longer present in $_REQUEST).
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
        <!-- 'user'/'jxapp' let jxtotp.php recognize a continuing MFA flow
             (see app_check_user()) and are forwarded to app_login(). -->
        <input type="hidden" name="user" value="<?php echo $preserveUser; ?>">
        <input type="hidden" name="jxapp" value="<?php echo $preserveApp; ?>">
        <div class="form-group">
          <label for="otp_code">Codice OTP</label>
          <input type="text" id="otp_code" name="otp_code"
                 class="input input-otp"
                 inputmode="numeric" pattern="\d{6}" maxlength="6"
                 placeholder="000000" required autofocus>
        </div>
        <!-- flex-start + flex-shrink:0 keep both checkboxes visually
             consistent regardless of label line count. -->
        <div class="form-group" style="display:flex;align-items:flex-start;gap:8px;margin-top:4px">
          <input type="checkbox" id="remember_device" name="remember_device"
                 value="1" style="width:16px;height:16px;flex-shrink:0;margin-top:2px;cursor:pointer">
          <label for="remember_device" style="margin:0;font-size:.86rem;font-weight:400;cursor:pointer;color:#3A4F6A">
            Considera questo dispositivo attendibile (non richiedere OTP per 10 giorni)
          </label>
        </div>
        <!-- Shown only here (post-setup), never in register.php — no trusted
             devices can exist yet at initial setup. Server-side ordering
             ensures a newly registered device survives if "remember me" is
             also checked. -->
        <div class="form-group" style="display:flex;align-items:flex-start;gap:8px;margin-top:4px">
          <input type="checkbox" id="revoke_devices" name="revoke_devices"
                 value="1" style="width:16px;height:16px;flex-shrink:0;margin-top:2px;cursor:pointer">
          <label for="revoke_devices" style="margin:0;font-size:.86rem;font-weight:400;cursor:pointer;color:#3A4F6A">
            Elimina eventuali altri dispositivi attendibili già memorizzati
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
// Response already sent; jxtotp.php does not regain control (see file header).
die();
