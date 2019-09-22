<?php

/**
 * Janox Ajax responder Module
 * PHP5 - HTML4.01 - JavaScript1.2
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
 * This script contains Janox response logics for system Ajax requests.
 *  .: AJAX activity response
 *  .: Lookup items
 *  .: Notification area items
 *  .: Dispatcher: remove dispatch
 *  .: Open new session
 *
 *
 * @name      jxr
 * @package   janox/htdocs/jxr.php
 * @version   2.6
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

// _________________________________________ Unrem next line to simulate network delay ___
//usleep(mt_rand(1, 1000) * 1000);
// ___________________________________________________________________ Include runtime ___
include_once '../jxrnt.php';
// ____________________________________________________ If called by a working session ___
session_name($_REQUEST["JXSESSNAME"]);
if (session_start()) {
    $app = $_SESSION['o2_app'];
    // _________________________________________________________________________ JXSQL ___
    if ($app->jxsql) {
        include_once $app->runtime->dir_dbms.'jxsql.inc';
        }
    // _____________________________________________________ Server side extra scripts ___
    if ($app->file_server_inc) {
        include_once $app->dir_progs.$app->file_server_inc;
        }
    // __________________________________________________ Get client size informations ___
    o2html::client_info();
    ob_start();
    switch ($_REQUEST['jxact']) {
        case "refresh": // ______________________________________________ Page refresh ___
            provide_prg($app);
            o2html::windows_info();
            $GLOBALS['jxjs'] = true;
            include_once '../lib/jxjs.inc';
            o2html::receive();
            jxjs::page($_REQUEST['o2_action']);
            break;
        case "pagepost": // ________________________________ Standard POST (FULL-AJAX) ___
            $GLOBALS['jxjs'] = true;
            include_once '../lib/jxjs.inc';
            // ___________________________________________________ Start AJAX response ___
            jxjs::start($_REQUEST['jxjsid']);
            if (is_a($app, "o2_app")) {
                try {
                    $app->esecutivo();
                    }
                catch (o2_exception $o2e) {
                    $o2e->send();
                    return false;
                    }
                }
            // _______________________________________________ Manage expired sessions ___
            else {
                header($_SERVER["SERVER_PROTOCOL"]." 401 Unauthorized", true, 401);
                print "window.location = '".$app->no_login."';\n";
                }
            jxjs::end();
            break;
        case "lookup": // _____________________________________________________ Lookup ___
            $GLOBALS['jxjs'] = true;
            provide_prg($app);
            o2html::ctrl_lookup_req();
            break;
        case "popup": // _______________________________________________________ Popup ___
            session_write_close();
            provide_prg($app);
            o2html::receive();
            o2html::popup_req($_REQUEST['jxpuexp']);
            break;
        case "notify": // __________________________________________ Notification area ___
            if (is_a($app, "o2_app")) {
                provide_db($app);
                if ($app->refresh_prg) {
                    $app->refresh_last = time();
                    $app->intcall($app->refresh_prg);
                    }
                o2html::notify_response();
                }
            break;
        case "progress": // _____________________________________________ Progress bar ___
            provide_prg($app);
            o2_ctrl_progress::get_values();
            break;
        case "remdispatch": // _______________________________________ Remove dispatch ___
            o2_dispatcher::get_dispatcher()->dispatch_remove($_REQUEST['jxmsgid']);
            break;
        case "sessopen": // _________________________________________ Open new session ___
            if (is_a($app, "o2_app")) {
                // _________________________________ Clear all previous output, if any ___
                ob_end_clean();
                // ________________________________ Add parameters to open new session ___
                $params = array('user'     => $app->user,
                                'password' => $app->password,
                                'auth'     => 'local');
                if ($app->client_width) {
                    $params['jxcsw'] = $app->client_width;
                    }
                if ($app->client_height) {
                    $params['jxcsh'] = $app->client_height;
                    }
                if ($app->runtime->developer) {
                    $params['dev'] = $app->runtime->developer;
                    $params['key'] = $app->runtime->dev_key;
                    }
                print "o2jse.cmd.post(false, ".json_encode($params).", true);\n";
                }
            break;
        case "jxdev": // _________________________________________ Development command ___
            header("Content-type: text/html; charset=".$app->chr_encoding);
            provide_prg($app);
            // ______________________________________________________ Execute PHP code ___
            if (isset($_REQUEST['jxcmdline'])) {
                $codeText = $_REQUEST['jxcmdline'];
                if ($codeText) {
                    eval("print_r(".iconv("UTF-8", "CP1252", $codeText).");");
                    }
                else {
                    print "\n";
                    }
                }
            // ___________________________________________________________ Log to file ___
            elseif ($_REQUEST['jxfilelog']) {
                $app->log2file = ($_REQUEST['jxfilelog'] == "on");
                print "o2jse.lab.status(".
                      ($_REQUEST['jxfilelog'] == "on" ? "true," : "false,").
                      ($app->sqltrace ? "true," : "false,").
                      ($app->mutelog ? "true" : "false").
                      ");\n";
                }
            // _____________________________________________________ Log DB activities ___
            elseif ($_REQUEST['jxsqllog']) {
                $app->sqltrace = ($_REQUEST['jxsqllog'] == "on");
                print "o2jse.lab.status(".
                      ($app->log2file ? "true," : "false,").
                      ($_REQUEST['jxsqllog'] == "on" ? "true," : "false,").
                      ($app->mutelog ? "true" : "false").
                      ");\n";
                }
            // ___________________________________ Mute console log - log only to file ___
            elseif ($_REQUEST['jxmutelog']) {
                $app->mutelog = ($_REQUEST['jxmutelog'] == "on");
                print "o2jse.lab.status(".
                      ($app->log2file ? "true," : "false,").
                      ($app->sqltrace ? "true," : "false,").
                      ($_REQUEST['jxmutelog'] == "on" ? "true" : "false").
                      ");\n";
                }
            // ________________________________________________ Request variables list ___
            elseif ($_REQUEST['jxvarlist']) {
                if ($_REQUEST['jxvarlist'] !== "1") {
                    $filter = strtolower($_REQUEST['jxvarlist']);
                    }
                else {
                    $filter = false;
                    }
                $prg = $app->istanze_prg[$app->progressivo_istanze];
                print "<br>";
                if (count($app->vars)) {
                    print "| SESSION VARIABLES\n";
                    foreach ($app->vars as $varname => $field) {
                        if (!$filter ||
                            (strpos(strtolower($varname), $filter) !== false)) {
                            $mask = "[".$field->maschera->tipo."] ".
                                    $field->maschera->maschera;
                            print "|   <a href='#' title='".$mask.
                                  "' onclick='o2jse.lab.addEl(\"".
                                  "o2val(\\\"_o2SESSION\\\",\\\"".$varname."\\\")\");'>".
                                  $varname."</a>\n";
                            }
                        }
                    }
                $last_view = "";
                if (count($prg->contesto)) {
                    foreach ($prg->contesto as $view) {
                        if ($view->campi) {
                            if ($last_view != $view->nome) {
                                print "|   [".$view->nome."]\n";
                                $last_view = $view;
                                }
                            foreach ($view->campi as $field) {
                                if (!$filter ||
                                    (strpos(strtolower($field->nome),
                                            $filter) !== false)) {
                                    $mask = "[".$field->maschera->tipo."] ".
                                            $field->maschera->maschera;
                                    print "|      <a href='#' title='".$mask.
                                          "' onclick='o2jse.lab.addEl(\"o2val(\\\"".
                                          $view->nome."\\\",\\\"".
                                          $field->nome."\\\")\");'>".
                                          $field->nome."</a>\n";
                                    }
                                }
                            foreach ($view->formule as $field) {
                                if (!$filter ||
                                    (strpos(strtolower($field->nome),
                                            $filter) !== false)) {
                                    $mask = "[".$field->maschera->tipo."] ".
                                            $field->maschera->maschera;
                                    print "|      <i><a href='#' title='Formula' ".
                                          "onclick='o2jse.lab.addEl(\"o2val(\\\"".
                                          $view->nome."\\\",\\\"".
                                          $field->nome."\\\")\");'>".
                                          $field->nome."</a></i>\n";
                                    }
                                }
                            }
                        else {
                            print "| PROGRAM VARIABLES\n";
                            foreach ($view->variabili as $field) {
                                if (!$filter ||
                                    (strpos(strtolower($field->nome),
                                            $filter) !== false)) {
                                    $mask = "[".$field->maschera->tipo."] ".
                                            $field->maschera->maschera;
                                    print "|   <a href='#' title='".$mask.
                                          "' onclick='o2jse.lab.addEl(\"o2val(\\\"".
                                          htmlentities($view->nome,
                                                       ENT_COMPAT | ENT_HTML5,
                                                       $app->chr_encoding)."\\\",\\\"".
                                          $field->nome."\\\")\");'>".
                                          $field->nome."</a>\n";
                                    }
                                }
                            if (count($prg->contesto) > 1) {
                                print "| FIELDS FROM VIEWS\n";
                                }
                            }
                        }
                    }
                }
            break;
        }
    }


/**
 * Provides program level informations for the script. To be called before using program
 * level data, like lookup items: not needed for application level data, like menus.
 *
 * @param o2_app $app
 */
function provide_prg($app) {

    // __________________________________________________ Provide database data access ___
    provide_db($app);
    // _______________________________________________ Include single programs scripts ___
    foreach ($app->istanze_prg as $singola_istanza) {
        $app->includi_prf($singola_istanza->nome, $singola_istanza->script);
        }

    }


/**
 * Provides database informations access for the script. To be called before quering data
 * from/to database, like lookup items: not needed for session data, like menus.
 *
 * @param o2_app $app
 */
function provide_db($app) {

    // _________________________________________________________ View models inclusion ___
    if ($app->file_viewmodels) {
        $app->includi_repository($app->file_viewmodels);
        }
    // _________________________________________________________________ Load gateways ___
    $app->load_gateways();

    }

?>
