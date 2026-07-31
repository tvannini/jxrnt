<?php

/**
 * Janox Application Module
 * PHP7-8
 *
 *
 * This file is part of Janox.
 *
 * Janox is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * Janox is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This script provides an access to Janox applications with protocol TOTP as a
 * Multi-Factor Authentication.
 *
 * @name      jxtotp
 * @package   janox/htdocs/jxtotp.php
 * @version   3.1
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007-2026
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

// ============================================================= DEFINE MAIN VARIABLES ===

/**
 * Janox runtime path.
 *
 */
$jxrnt = jxload();


/**
 * Application main script ("<app>/htdocs/<app>.php")
 *
 * @see decode_app()
 */
$app_main_path = get_app_path();


// ============= Start session to store informations needed across requests (for TOTP) ===
session_start();


/**
 * Application main URL ("https://<host>/<...>/<app>.php")
 *
 * @see get_app_url()
 */
$app_main_url = get_app_url($app_main_path);


// ===================================================== Process application for login ===
process_app($app_main_path);


/**
 * Load Janox runtime
 *
 */
function jxload() {

    // __________________________ Set Janox runtime when script served from Janox host ___
    if (file_exists(__DIR__.'/../jxrnt.php')) {
        $jxrnt = realpath(__DIR__.'/../jxrnt.php');
        }
    // ____________ No Janox runtime found, send error message and terminate execution ___
    else {
        error_send("Can't find a valid Janox runtime");
        }
    // ____________________________________________________________ Load Janox runtime ___
    require_once $jxrnt;
    return $jxrnt;

    }


/**
 * Return physical path to application main script.
 * If $_REQUEST['jxapp'] is passed as a path it is used, else value is used as alias for
 * the apllication and physical path is retrieved from "[registered_apps]" array in
 * runtime configuration INI.
 *
 * TODO:
 * Remove passed $_REQUEST['jxapp'] from TOTP and set only first time from login, after
 * use from $_SESSION['o2_app'].
 *
 * @return string   Physical path to app main script
 */
function get_app_path() {

    // _______________________________________ Check if app is passed as physical path ___
    if (!file_exists($_REQUEST['jxapp'])) {
        $rnt = $GLOBALS['o2_runtime'];
        $app_alias = $_REQUEST['jxapp'];
        if (isset($rnt->registered_apps[$app_alias]) &&
            file_exists($rnt->registered_apps[$app_alias])) {
            /**
             * TODO:
             * Needed to preserve $_REQUEST['jxapp'] throw TOTP logics.
             * Remove passing $_REQUEST['jxapp'] from TOTP.
             */
            $_REQUEST['jxapp'] = $rnt->registered_apps[$app_alias];
            return $rnt->registered_apps[$app_alias];
            }
        else {
            error_send('Error: can\'t reach application '.$app_alias);
            }
        }
    else {
        return $_REQUEST['jxapp'];
        }

    }


/**
 * Return application main-page URL.
 * First time URL is retrieved combining application main file and
 * $_SERVER['HTTP_REFERER'] (application login page).
 * After first definition URL is stored as "jxapp->referer".
 *
 * @param string $app_main_path   Appication main script physical path
 */
function get_app_url($app_main_path) {

    if (!isset($_SESSION['o2_app'])) {
        $app_name     = basename(str_replace('\\', '/', $app_main_path));
		$refParts     = parse_url($_SERVER['HTTP_REFERER']);
		$path         = $refParts['path'] ?? '/';
        if (preg_match('/\.[a-zA-Z0-9]+$/', $path)) {
            $path = dirname($path);
            }
        $app_main_url = ($refParts['scheme'] ?? 'https').'://'.
                        ($refParts['host'] ?? '').
                        rtrim($path, '/').'/'.
                        $app_name;
        $context = stream_context_create(['ssl'  => ['verify_peer'      => false,
                                                     'verify_peer_name' => false],
                                          'http' => ['timeout' => 5]]);
        if (@get_headers($app_main_url, false, $context) === false) {
            error_send('Error: can\'t reach URL '.$app_main_url);
            }
        return $app_main_url;
        }
    else {
        return $_SESSION['o2_app']->referer;
        }

    }


/**
 * Read application structure, check for autohorized user to be defined and generate the
 * correct login options
 *
 * @param  string $app_main_path   Path to application main file
 */
function process_app($app_main_path) {

    if (file_exists($app_main_path)) {
        $main_info = pathinfo($app_main_path);
        $app_name  = $main_info['filename'];
        $app_dir   = dirname($main_info['dirname']);
        $app_ini   = parse_ini_file($app_dir.DIRECTORY_SEPARATOR.$app_name.'.ini');
        $app_dbs   = $app_dir.DIRECTORY_SEPARATOR.'prgs'.DIRECTORY_SEPARATOR.
                     ($app_ini['dbs'] ? $app_ini['dbs'] : 'db_repository.inc');
        $app_tabs  = $app_dir.DIRECTORY_SEPARATOR.'prgs'.DIRECTORY_SEPARATOR.
                     ($app_ini['tables'] ? $app_ini['tables'] : 'file_repository.inc');
        // ______________________________ Create a pseudo object for Janox application ___
        if (!isset($_SESSION['o2_app'])) {
            $_SESSION['o2_app']               = new stdClass();
            $_SESSION['o2_app']->nome         = $app_name;
            $_SESSION['o2_app']->referer      = $GLOBALS['app_main_url'];
            $_SESSION['o2_app']->alias        = dirname($_SESSION['o2_app']->referer);
            $_SESSION['o2_app']->request_ori  = $_REQUEST;
            $_SESSION['o2_app']->chr_encoding = $app_ini['encoding'] ?? 'UTF-8';
            $_SESSION['o2_app']->dir_data     = $app_dir.DIRECTORY_SEPARATOR.'data'.
                                                DIRECTORY_SEPARATOR;
            $_SESSION['o2_app']->error_mode   = 'EXE';
            if (isset($app_ini['nologin']) && $app_ini['nologin']) {
                $nologin = $app_ini['nologin'];
                if (strpos($nologin, 'http') === false) {
                    $refParts = parse_url($_SERVER['HTTP_REFERER']);
                    $nologin  = ($refParts['scheme'] ?? 'https').'://'.
                                ($refParts['host'] ?? '').
                                rtrim(dirname($refParts['path'] ?? '/'), '/').'/'.
                                $nologin;
                    }
                $_SESSION['o2_app']->no_login = $nologin ?? '';
                }
            // ______________________________________ Load application servers and dbs ___
            require_once $app_dbs;
            }
        // _________________________________ Read application tables definition script ___
        $tabs_code = file_get_contents($app_tabs);
        // _____________________ Check if user exists in application and perform login ___
        app_check_user($tabs_code);
        }
    else {
        error_send("Can't find application ".$app_main_path);
        }

    }


/**
 * Read users-table structure from repository and check if authorized user is defined in
 * application.
 * If user is defined and not set for TOTP-MFA login, then a standard login to application
 * is performed.
 * If user is defined and set for TOTP-MFA login then check against TOTP and, if verified,
 * execute a login to application with OTP code generated for the user.
 *
 * @param  string $tabs_code     Tables repository code
 */
function app_check_user($tabs_code) {

    // ═══════════════════════════════════════════════════════ [TOTP-MFA] ═══════
    // Integrazione modulo secondo fattore (TOTP) — blocco AGGIUNTO, inizio.
    //
    // Il modulo TOTP (cartella "../totp/") gestisce una PROPRIA sessione PHP,
    // del tutto autonoma e indipendente da qualsiasi sessione applicativa
    // Janox: questo script è la pagina di login stessa e gira PRIMA che una
    // sessione Janox esista. require_once avviene qui, in cima alla funzione,
    // per due motivi:
    //   1. Deve avvenire su OGNI richiesta (la prima e tutte le successive):
    //      la sessione autonoma del modulo (cookie proprio) è il solo modo
    //      per sapere se un utente sta già completando il secondo fattore.
    //   2. Deve avvenire PRIMA del lookup utente qui sotto, per poter
    //      saltare quel lookup quando non serve (vedi guardia sotto).
    require_once __DIR__ . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR .
                 'totp' . DIRECTORY_SEPARATOR . '_bootstrap.php';

    // ── Guardia: prosegui un flusso MFA già iniziato ────────────────────────
    // Se la nostra sessione autonoma indica che l'utente sta già completando
    // il secondo fattore (ha già visto il form di setup/OTP e lo sta
    // sottomettendo), NON ripetiamo il lookup utente/password sottostante:
    // in questo hit intermedio il form del modulo non rinvia più la password
    // (non serve più, ed è una scelta di sicurezza non richiederla due volte).
    // Il confronto con lo username richiesto evita di riprendere per errore
    // un flusso MFA abbandonato da un login precedente con un altro utente.
    $totpChallengeState = $auth->getState();
    $totpChallengeUser  = $auth->getChallengeUser();
    if (($totpChallengeState === Auth::STATE_PENDING_TOTP
         || $totpChallengeState === Auth::STATE_PENDING_SETUP)
        && $totpChallengeUser === strtolower(trim($_REQUEST['user'] ?? ''))) {

        $totpPage = ($totpChallengeState === Auth::STATE_PENDING_SETUP)
                    ? 'register.php' : 'index.php';
        $totpOk   = include_once __DIR__ . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR .
                                  'totp' . DIRECTORY_SEPARATOR . $totpPage;
        if ($totpOk === true) {
            // Secondo fattore soddisfatto (codice corretto, o dispositivo già
            // attendibile): si procede come per un login riuscito, con l'OTP
            // interno di Janox — esattamente come per un utente senza MFA. Il
            // modulo TOTP non chiama mai direttamente app_login()/
            // app_generate_otp(): si limita a segnalare l'esito col valore di
            // ritorno dell'include.
            app_login(true, app_generate_otp($tabs_code));
            }
        // Se $totpOk non è true, la pagina inclusa ha già stampato il
        // form/errore e terminato con die(): non si arriva mai qui in quel caso.
        return;
        }
    // ═══════════════════════════════════════════════════════ [TOTP-MFA] ═══════
    // Integrazione modulo secondo fattore (TOTP) — blocco AGGIUNTO, fine.

    $user = strtolower($_REQUEST['user']);
    // __________________________________________ Get users table db and physical name ___
    $parts = array();
    preg_match_all('/o2def::tab\("o2_users", "(.*)", "(.*)", "o2user"\);/',
                   $tabs_code,
                   $parts);
    $users_db  = $_SESSION['o2_app']->db[$parts[1][0]];
    $users_tab = $parts[2][0];
    $server    = $users_db->server;
    $type      = $server->type;
    $GLOBALS['o2_runtime']->load_gateway($type);
    $co    = constant('o2_'.$type.'_o');
    $cc    = constant('o2_'.$type.'_c');
    $where = $co.'o2user'.$cc."='".$user."'";
    // __________________________________________ Verify user existance in application ___
    $res   = o2_gateway::recordset($type,
                                   $server->server,
                                   $server->user,
                                   $server->password,
                                   $users_db->nome,
                                   $users_db->proprietario,
                                   $users_tab,
                                   'o2_users',
                                   '*',
                                   $where,
                                   '',
                                   1);

    if (!$res) {
        error_send('Sorry, you are not allowed.');
        }
    $res = array_change_key_case($res[0]);
    // ___________________________________ Check if user is anabled for TOTP-MFA login ___
    if (!isset($res['mfa']) || $res['mfa'] != 'T') {
        // __________________________________ A standard login to application is fired ___
        app_login();
        }
    // ____________________________ Verify user standard credentials (user & password) ___
    elseif (isset($_REQUEST['password']) &&
            (password_verify($_REQUEST['password'], $res['o2password']) === false)) {
            error_send('Sorry, you are not allowed');
            die();
            }
    // __________________________________________________ User is enabled for TOTP-MFA ___
    else {
        // ═══════════════════════════════════════════════════ [TOTP-MFA] ═══════
        // Integrazione modulo secondo fattore (TOTP) — blocco AGGIUNTO, inizio.
        //
        // Sostituisce il vecchio blocco di test (rimosso) che forzava
        // manualmente $_SESSION[Auth::SESSION_STATE] = Auth::STATE_PENDING_TOTP:
        // quel test serviva solo a verificare che la direzione fosse quella
        // giusta, non era un contratto definitivo. La decisione se l'utente
        // debba completare il setup (prima volta) o inserire un codice OTP
        // (già configurato) resta interamente dentro
        // Auth::startMfaChallenge()/isTotpConfigured(): non viene mai forzata
        // da questo file. Rimossi anche $GLOBALS['jxUserData']/
        // ['jxUserPassword'] del vecchio blocco: il modulo TOTP non legge la
        // password (già verificata sopra da Janox) né il record utente
        // completo, gli basta lo username.
        //
        // La password è già stata verificata qui sopra (ramo elseif
        // precedente, invariato): da questo punto il modulo TOTP gestisce
        // SOLO il secondo fattore. $auth è già disponibile: il require_once
        // di _bootstrap.php è stato eseguito in cima a questa funzione.
        $auth->startMfaChallenge($user);

        $totpPage = ($auth->getState() === Auth::STATE_PENDING_SETUP)
                    ? 'register.php' : 'index.php';
        $totpOk   = require_once __DIR__ . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR .
                                  'totp' . DIRECTORY_SEPARATOR . $totpPage;
        if ($totpOk === true) {
            // Il dispositivo era già "attendibile": il secondo fattore non è
            // stato nemmeno richiesto in questo giro, si procede comunque con
            // l'OTP interno di Janox per completare l'accesso all'app.
            app_login(true, app_generate_otp($tabs_code));
            }
        // Se $totpOk non è true, la pagina inclusa ha già stampato il form
        // (setup QR o richiesta OTP) e terminato con die().
        // ═══════════════════════════════════════════════════ [TOTP-MFA] ═══════
        // Integrazione modulo secondo fattore (TOTP) — blocco AGGIUNTO, fine.
        }

    }


/**
 * Read OTP-table structure from repository and create an OTP-code in application for the
 * authorized user
 *
 * @param  string $tabs_code   Tables repository code
 * @return string              OTP-code
 */
function app_generate_otp($tabs_code) {

    $jxrnt = $GLOBALS['o2_runtime'];
    $user  = $_REQUEST['user'];
    // ____________ Check if OTP table is defined in repository or get it from runtime ___
    if (strpos($tabs_code, 'o2def::tab("jx_otp"') === false) {
        $tabs_code = file_get_contents($jxrnt->root.'lib'.DIRECTORY_SEPARATOR.
                                                    'prgs'.DIRECTORY_SEPARATOR.
                                                    'tables.inc');
        }
    // ___________________________________________ Get OTPs table db and physical name ___
    $parts = array();
    preg_match_all('/o2def::tab\("jx_otp", "(.*)", "(.*)", "code"\);/',
                   $tabs_code,
                   $parts);
    $otp_db  = $_SESSION['o2_app']->db[$parts[1][0]];
    $otp_tab = $parts[2][0];
    $server  = $otp_db->server;
    $type    = $server->type;
    $jxrnt->load_gateway($type);
    $co      = constant('o2_'.$type.'_o');
    $cc      = constant('o2_'.$type.'_c');
    // ____________________________________________________ Generate OTP code for user ___
    $code    = $jxrnt->generate_otp(6, $user);

    // _______________________________________ Insert OTP code in application for user ___
    $fields  = array($co.'code'.$cc, $co.'user'.$cc, $co.'create_time'.$cc);
    $values  = array("'".$jxrnt->crypt($code)."'", "'".$user."'", time());
    o2_gateway::insertrec($type,
                          $server->server,
                          $server->user,
                          $server->password,
                          $otp_db->nome,
                          $otp_db->proprietario,
                          $otp_tab,
                          'jx_otp',
                          $fields,
                          $values);
    o2_gateway::commit($server->type,
                       $server->server,
                       $server->user,
                       $server->password);
    return $code;

    }


/**
 * Post login to application with standard credentials or with OTP code
 *
 * @param integer $otp        TRUE if login is with OTP code, FALSE for standard login
 * @param string  $otp_code   OTP code for authentication
 */
function app_login($otp = false, $otp_code = false) {

    // ________________________________________________ Read application configuration ___
    if (file_exists($GLOBALS['app_main_path'])) {
        $app_main_path = $GLOBALS['app_main_path'];
        $app_main_url  = $_SESSION['o2_app']->referer;
        $main_info = pathinfo($app_main_path);
        $app_name  = $main_info['filename'];
        $app_dir   = dirname($main_info['dirname']);
        $app_ini   = parse_ini_file($app_dir.DIRECTORY_SEPARATOR.$app_name.'.ini');
        $html      = "<!DOCTYPE HTML>\n".
                     '<html><head><meta charset="'.$app_ini['encoding'].'">'.
                     '<meta name="application-name" content="'.$app_name.
                     '"><meta name="description" content="'.$app_ini['title'].
                     '"><meta name="GENERATOR" content="Janox - www.janox.it">'.
                     '<title>'.$app_ini['title'].'</title></head><body>'.
        // ____________________________________________________ Compose form to submit ___
                     '<form name="jxlogin" method="POST" action="'.$app_main_url.'">';
        // _______________________________________________________ Login with OTP code ___
        if ($otp) {
            // ________________ Remove original password from request and add OTP code ___
            unset($_SESSION['o2_app']->request_ori['password']);
            $html.= '<input type="hidden" name="jxotp" value="'.$otp_code.
                    '"><input type="hidden" name="auth" value="local">';
            }
        // _______________________________________________ Add original request fields ___
        foreach ($_SESSION['o2_app']->request_ori as $key => $val) {
            $html.= '<input type="hidden" name="'.htmlspecialchars($key).
                                       '" value="'.htmlspecialchars($val).'">';
            }
        // _______________________________________________________________ Submit form ___
        $html.= '</form><br><center><h4 style="font-family:arial;">Logging into '.
                $app_name.'...<h4></center>'.
                '<script> document.forms.jxlogin.submit(); </script></body></html>';
        session_unset();
        if (ini_get("session.use_cookies")) {
            $params = session_get_cookie_params();
            setcookie(session_name(), '', time() - 42000,
                      $params["path"], $params["domain"],
                      $params["secure"], $params["httponly"]);
            }
        session_destroy();
        print $html;
        }

    }


/**
 * Outputs en error message, send 401 header and terminate execution.
 * If session is started and $_SESSION['o2_app'] is defined, then use application no-login
 * page to fall-back.
 *
 * @param string $msg   Error message to be displayed
 */
function error_send($msg) {

    http_response_code(401);
    if (isset($_SESSION['o2_app']) && isset($_SESSION['o2_app']->no_login)) {
        $nologin = $_SESSION['o2_app']->no_login;
        if (strpos($nologin, 'http') === false) {
            $nologin = $_SESSION['o2_app']->alias.$nologin;
            }
        $html = '<form name="jxlogout" method="POST" enctype="text/plain" action="'.
                $nologin.'"></form><script>document.forms["jxlogout"].submit();</script>';
        }
    else {
        $html = "<!DOCTYPE HTML>\n".
                '<html><head><meta charset="UTF-8">'.
                '<meta name="GENERATOR" content="Janox - www.janox.it">'.
                '<title>Janox TOTP login error</title></head><body>'.
                '<br><center><h4 style="font-family:arial;">'.$msg.
                '</h4></center></body></html>';
        }
    session_unset();
    if (ini_get("session.use_cookies")) {
        $params = session_get_cookie_params();
        setcookie(session_name(), '', time() - 42000,
                  $params["path"], $params["domain"],
                  $params["secure"], $params["httponly"]);
        }
    session_destroy();
    die($html);

    }


/**
 * Logs a message to file
 *
 * @param string $msg   Message to be logged
 */
function mylog($msg) {

    file_put_contents(__DIR__.DIRECTORY_SEPARATOR.'totp.log', $msg."\n\n", FILE_APPEND);

    }

?>