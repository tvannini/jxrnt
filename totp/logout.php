<?php
declare(strict_types=1);
require_once __DIR__ . '/_bootstrap.php';

/**
 * Invalida la sessione MFA autonoma del modulo JXTOTP.
 *
 * Non fa parte del flusso di aggancio attuale: jxtotp.php, nella versione
 * corrente, non richiama mai questo file in nessun punto. È fornito come
 * utility indipendente per due scopi possibili in futuro:
 *   - un eventuale punto di ingresso HTTP dedicato, se una diversa
 *     configurazione dell'ERP dovesse averne bisogno;
 *   - interrompere manualmente (es. da riga di comando o da uno script di
 *     amministrazione) una sessione MFA rimasta bloccata a metà flusso.
 *
 * Auth::logout() distrugge l'intera sessione autonoma di questo modulo — è
 * sicuro farlo perché questa sessione non è mai condivisa con Janox (vedi
 * _bootstrap.php): non c'è alcun rischio di invalidare dati di sessione che
 * non appartengono al modulo TOTP.
 */
$auth->logout();

header('Content-Type: text/plain; charset=UTF-8');
echo "Sessione MFA terminata.\n";
