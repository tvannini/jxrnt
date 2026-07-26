<?php
declare(strict_types=1);
require_once __DIR__ . '/_bootstrap.php';

/**
 * Utility di generazione QR code "al volo" — endpoint tipo
 * <img src="qrcode.php?data=...&scale=6">.
 *
 * NON raggiungibile via URL in questa installazione: questa cartella non è
 * in nessuna document root di alcun web server (solo jxtotp.php lo è).
 * register.php genera già il proprio QR code in memoria come data URI
 * base64, incorporato direttamente nell'HTML: non ha bisogno di richiamare
 * questo file.
 *
 * Mantenuto come utility indipendente per un eventuale uso futuro (es. un
 * endpoint HTTP dedicato in una diversa configurazione dell'ERP). Se mai
 * venisse esposto via URL, resta protetto esattamente come nel prototipo
 * standalone originale: risponde solo mentre un setup TOTP è in corso.
 */
if ($auth->getState() !== Auth::STATE_PENDING_SETUP) {
    http_response_code(403);
    exit;
}

$data  = $_GET['data'] ?? '';
$scale = max(2, min(12, (int) ($_GET['scale'] ?? 6)));

if ($data === '') {
    http_response_code(400);
    exit;
}

$data = base64_decode($data, true);
if ($data === false) {
    http_response_code(400);
    exit;
}

try {
    $png = QrCode::generate($data, $scale, 4);
} catch (\Exception $e) {
    // $e non usata: la variabile è richiesta dalla sintassi PHP < 8.0.
    http_response_code(500);
    exit;
}

header('Content-Type: image/png');
header('Cache-Control: no-store, no-cache');
echo $png;
