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
 * This script provides a SAML2 access to Janox applications and it is based on the
 * SimpleSAMLPHP library (www.simplesamlphp.org).
 *
 * Download the library and place it in your runtime/application structure and follow
 * the instructions from library reference to configure it.
 *
 * To configure this script you can use variables definitions below or create a saml.ini
 * file in this script parent directory (see janox/jxrnt/saml.ini.std in runtime
 * distribution).
 *
 *
 * @name      saml_login
 * @package   janox/htdocs/saml.php
 * @version   2.9
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

// ============================================================= DEFINE MAIN VARIABLES ===

/**
 * Janox runtime, usually you can leave it unset.
 * If unset script will look for Janox runtime in "../jxrnt.php" (if SAML
 * login is served from inside the runtime) or in application
 * configuration (if SAML login is provided from inside application).
 *
 * @var String   Filesystem absolute or relative (to __DIR__) path
 */
$jxrnt = '';


/**
 * Application main script ("<app>/htdocs/<app>.php")
 *
 * @var String   Filesystem absolute or relative (to __DIR__) path
 */
$app_main_path = '/path_to_myapp/htdocs/myapp.php';


/**
 * Application main URL ("https://<host>/<...>/<app>.php")
 *
 * @var String   Absolute or relative (to host) URL
 */
$app_main_url = 'https://myhost/myapp.php';


/**
 * SAML SP-ID.
 * This ID must match one of the SP-keys defined in SimpleSAMLPHP library
 * ("SimpleSAMLPHP/config/authsources.php").
 *
 * @var String
 */
$saml_sp_id = 'google-workspace';


/**
 * SimpleSAMLPHP autoload script ("<simplesamlphp>/src/_autoload.php")
 *
 * @var String   Filesystem absolute or relative (to __DIR__) path
 */
$saml_lib_autoload = '../simplesamlphp/src/_autoload.php';


/**
 * Domain to be stripped out from Name-IDs.
 * Sample: if you set "saml_domain = mydomain.com" and you have an authorized user as
 * "myuser@mydomain.com", then login to the application will be requested for user
 * "myuser".
 *
 * @var string   Domain name to strip out from email addresses
 */
$saml_domain = 'mydomain.it';


// ================================================== READ CONFIGURATION FROM INI FILE ===
list($app_main_path,
     $app_main_url,
     $saml_sp_id,
     $jxrnt,
     $saml_lib_autoload,
     $saml_domain) = get_conf($app_main_path,
                              $app_main_url,
                              $saml_sp_id,
                              $jxrnt,
                              $saml_lib_autoload,
                              $saml_domain);


// =================================================================== EXECUTION START ===
// __________________ Check user for authorization or request authorization to SAML-SP ___
if ($GLOBALS['name_id'] = saml_auth($saml_lib_autoload, $saml_sp_id)) {
    // ____________________________________________________________ Load Janox runtime ___
    jxload($jxrnt);
    // __________________________________ Login to application with generated OTP code ___
    app_login($app_main_path, $app_main_url, get_app_otp($app_main_path, $saml_domain));
    }
// ===================================================================== EXECUTION END ===


/**
 * SAML authorization.
 * If user is granted this function returns the authorized Name-ID, else request for a
 * SAML authorization to the passed SAML-SP.
 *
 * @param  string $saml_lib_autoload   Path to the SImpleSAMLPHP autoload script
 * @param  string $saml_sp_id          SAML SP-ID as defined in SimpleSAMLPHP library
 * @return string|boolea
 */
function saml_auth($saml_lib_autoload, $saml_sp_id) {

    // ____________________________________________________________ Load SimpleSAMLPHP ___
    require_once($saml_lib_autoload);
    // _____________________________________________ Create SAML authentication for SP ___
    $auth = new \SimpleSAML\Auth\Simple($saml_sp_id);
    // _______________________________________ Check if user is already authtenticated ___
    if ($auth->isAuthenticated()) {
        $name = $auth->getAuthData('saml:sp:NameID');
        return $name->getValue();
        }
    // _______________________________ If not authenticated request for authentication ___
    \SimpleSAML\Session::getSessionFromRequest()->cleanup();
    $auth->requireAuth();
    print '<br><br>Authorization required!';
    return false;

    }


/**
 * Load Janox runtime
 *
 * @vparam string $jxrnt   Absolute or relative path to Janox runtime script
 */
function jxload($jxrnt) {

    // __________________________________________________ Set Janox runtime if not set ___
    if (!$jxrnt) {
        if (file_exists(__DIR__.'/../jxrnt.php')) {
            $jxrnt = __DIR__.'/../jxrnt.php';
            }
        else {
            error_send("Can't find Janox runtime");
            }
        }
    // ____________________________________________________________ Load Janox runtime ___
    require_once($jxrnt);

    }


/**
 * Read application structure, check for autohorized user to be defined and generate an
 * OTP-code to login
 *
 * @param  string $app_main_path   Path to application main file
 * @param  string $saml_domain     Domain to be stripped from user-ID
 * @return string                  Generated OTP-code to login
 */
function get_app_otp($app_main_path, $saml_domain) {

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
        $_SESSION['o2_app'] = new stdClass();
        // __________________________________________ Load application servers and dbs ___
        require_once($app_dbs);
        // _________________________________ Read application tables definition script ___
        $tabs_code = file_get_contents($app_tabs);
        // ____________________________ Check if authorized user exists in application ___
        app_check_user($tabs_code, $saml_domain);
        // _________________________ Generate and retrieve an OTP code for application ___
        $otp = app_generate_otp($tabs_code);
        }
    else {
        error_send("Can't find application ".$app_main_path);
        }
    return $otp;

    }


/**
 * Read users-table structure from repository and check if authorized user is defined in
 * application
 *
 * @param  string $tabs_code     Tables repository code
 * @param  string $saml_domain   Domain to be stripped from user-ID
 * @return boolean               If user is defined
 */
function app_check_user($tabs_code, $saml_domain) {

    // ________________________________________________ Strip SAML domain from user-ID ___
    $user = strtolower($GLOBALS['name_id']);
    if ($saml_domain) {
        $user = substr($user, 0, strrpos($user, '@'.strtolower($saml_domain)));
        }
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
    $res   = o2_gateway::verifyrec($type,
                                   $server->server,
                                   $server->user,
                                   $server->password,
                                   $users_db->nome,
                                   $users_db->proprietario,
                                   $users_tab,
                                   'o2_users',
                                   '*',
                                   $where,
                                   '');
    if (!$res) {
        error_send('Sorry, you ('.$user.') are not allowed to the application.');
        }
    $GLOBALS['name_id'] = $user;

    }


/**
 * Read OTP-table structure from repository and create an OTP-code in application for the
 * authorized user
 *
 * @param  string $tabs_code   Tables repository code
 * @return string              OTP-code
 */
function app_generate_otp($tabs_code) {

    // ___________________________________________ Get OTPs table db and physical name ___
    $parts = array();
    preg_match_all('/o2def::tab\("jx_otp", "(.*)", "(.*)", "code"\);/',
                   $tabs_code,
                   $parts);
    $otp_db  = $_SESSION['o2_app']->db[$parts[1][0]];
    $otp_tab = $parts[2][0];
    $server  = $otp_db->server;
    $type    = $server->type;
    $GLOBALS['o2_runtime']->load_gateway($type);
    $co      = constant('o2_'.$type.'_o');
    $cc      = constant('o2_'.$type.'_c');
    $otp_len = 6;
    // _____________________________________________________________ Generate OTP code ___
    $code    = substr(str_shuffle(str_repeat('0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                                             '1'.(substr(microtime(true) * 1000000,
                                                        -1) + 0))), -$otp_len);
    // __________________________ Insert OTP code in application for uthenticeted user ___
    $fields = array($co.'code'.$cc, $co.'user'.$cc, $co.'create_time'.$cc);
    $values = array("'".md5($code)."'", "'".$GLOBALS['name_id']."'", time());
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
 * Post login to application with OTP code
 *
 * @param string $app_main_path
 * @param string $app_main_url
 * @param string $otp_code
 */
function app_login($app_main_path, $app_main_url, $otp_code) {

    // ________________________________________________ Read application configuration ___
    if (file_exists($app_main_path)) {
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
        /* _________________________________________________ Compose form to submit ___ */
                     '<form name="jxSamlLogin" method="POST" action="'.$app_main_url.
                     '" enctype="multipart/form-data">'.
                     '<input type="hidden" name="user" value="'.$GLOBALS['name_id'].
                     '"><input type="hidden" name="jxotp" value="'.$otp_code.
                     '"><input type="hidden" name="auth" value="local"</form>'.
                     '<br><center><h4 style="font-family:arial;">Logging into '.$app_name.
                     '...<h4></center>'.
        /* ____________________________________________________________ Submit form ___ */
                     '<script> document.forms.jxSamlLogin.submit(); </script>'.
                     '</body></html>';
        print $html;
        die();

        }

    };


/**
 * Reads configuration from INI file, if any.
 * Function looks from INI file in parent directory of this current scrip, so that:
 *  - if this script is in <app>/htdocs/ it will look in <app> folder;
 *  - if this script is called from within jxrn/htdocs/ it will look in jxrnt folder.
 * See jxrnt/saml.ini.std file for more informations.
 *
 * @param string $app_main_path
 * @param string $app_main_url
 * @param string $saml_sp_id
 * @param string $jxrnt
 * @param string $saml_lib_autoload
 * @param string $saml_domain
 *  */
function get_conf($app_main_path,
                  $app_main_url,
                  $saml_sp_id,
                  $jxrnt,
                  $saml_lib_autoload,
                  $saml_domain) {

    // __________________________ Look for SAML configuration file in parent directory ___
    if (file_exists(__DIR__.DIRECTORY_SEPARATOR.'..'.DIRECTORY_SEPARATOR.'saml.ini')) {
        $saml_conf = parse_ini_file(__DIR__.DIRECTORY_SEPARATOR.'..'.DIRECTORY_SEPARATOR.
                                    'saml.ini');
        if (isset($saml_conf['jxrnt'])) {
            $jxrnt = $saml_conf['jxrnt'];
            }
        if (isset($saml_conf['app_main_path'])) {
            $app_main_path = $saml_conf['app_main_path'];
            }
        if (isset($saml_conf['app_main_url'])) {
            $app_main_url = $saml_conf['app_main_url'];
            }
        if (isset($saml_conf['saml_lib_autoload'])) {
            $saml_lib_autoload = $saml_conf['saml_lib_autoload'];
            }
        if (isset($saml_conf['saml_sp_id'])) {
            $saml_sp_id = $saml_conf['saml_sp_id'];
            }
        if (isset($saml_conf['saml_domain'])) {
            $saml_domain = $saml_conf['saml_domain'];
            }
        }
    return array($app_main_path,
                 $app_main_url,
                 $saml_sp_id,
                 $jxrnt,
                 $saml_lib_autoload,
                 $saml_domain);

    }


/**
 * Outputs en error message, send 403 header and terminate execution
 */
function error_send($msg) {

    $html = "<!DOCTYPE HTML>\n".
            '<html><head><meta charset="UTF-8">'.
            '<meta name="GENERATOR" content="Janox - www.janox.it">'.
            '<title>Janox SAML login error</title></head><body>'.
            '<br><center><h4 style="font-family:arial;">'.$msg.
            '</h4></center></body></html>';
    http_response_code(401);
    die($html);

    }

?>