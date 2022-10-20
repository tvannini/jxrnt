<?php

/**
 * Janox HTML Module
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
 * This script contains Janox HTML GUI gateway, drawing HTML GUIs for Janox controls
 * objects:
 *  .: HTML widgets objects
 *  .: HTML specific runtime funtions
 *  .: HTML specific application funtions
 *  .: HTML interfaces signals handling
 *
 *
 * @name      jxhtml
 * @package   janox/jxhtml.inc
 * @version   2.8
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * o2html static methods collection
 *
 */
class o2html {

    /**
     * Define application in HTML WEB context
     *
     * @param string $release
     */
    static function app($release = "") {

        // _____________________________________________________________ Session start ___
        if (!count($_GET)) {
            session_cache_limiter('must_revalidate');
            }
        if (isset($_REQUEST["JXSESSNAME"])) {
            if (preg_match('/\W/', $_REQUEST["JXSESSNAME"])) {
                header($_SERVER["SERVER_PROTOCOL"]." 400 Bad Request", true, 400);
                exit("Fatal error: wrong session name");
                }
            $sess_name = $_REQUEST["JXSESSNAME"];
            }
        else {
            $sess_name = "JXSESS".($GLOBALS['o2_runtime']->multisession ? time() : "");
            }
        session_name($sess_name);
        session_start();
        if (isset($_REQUEST['user']) && isset($_SESSION['o2_app'])) {
            session_regenerate_id(true);
            session_unset();
            session_start();
            }
        if (!isset($_SESSION['o2_app'])) {
            $app = new o2_app($release);
            // _________________________ Client side extra script inclusion (extra.js) ___
            if ($app->file_client_inc) {
                $app->extra_html_head = file_get_contents($app->dir_progs.
                                                          $app->file_client_inc);
                }
            }
        // ______________________________________________________ Set error management ___
        elseif ($_SESSION['o2_app']->error_mode) {
            $_SESSION['o2_app']->set_error_mode();
            }
        // _______________________________________ Application execution as RPC Server ___
        if ($_SESSION['o2_app']->rpc_server) {
            $_SESSION['o2_app']->rpc_exe();
            }
        // ____________________________________________ Standard application execution ___
        else {
            o2html::client_info();
            o2html::page();
            }

        }


    /**
     * Print out HTML interface page
     *
     */
    static function page($goto = false) {

        $pmark     = time();
        $app       = $_SESSION['o2_app'];
        $req_pmark = $_REQUEST['o2pagemark'];
        // ____________________________________________________ Output buffering start ___
        ob_start();
        // _________________________________________________ HTTP-header character-set ___
        @header("Content-type: text/html; charset=".$app->chr_encoding);
        if ($app->chr_encoding == "UTF-8") {
            mb_http_output($app->chr_encoding);
            }
        // _____________________________________________________________________ JXSQL ___
        if ($app->jxsql) {
            include_once $app->runtime->dir_dbms.'jxsql.inc';
            }
        // ____________________________ Server side extra script inclusion (extra.inc) ___
        if ($app->file_server_inc) {
            $app->include_extra();
            }
        // _______________________________________________________________ Page prefix ___
        o2html::page_prefix($pmark);
        // _____________________________________________________ Force password change ___
        if ($app->force_pwd_change) {
            $GLOBALS['jxPwdChangeCode'] = $app->force_pwd_change;
            $app->force_pwd_change      = false;
            $app->menu_attivo           = false;
            o2act::gotoprg('tools/jxgranting_pwd', false);
            }
        /* _________________________________________________________________ Execution ___
         * Request unique page-mark is checked to avoid re-executions of old pages (from
         * browser cache). Request is considered new and executed if:
         *  - No page-mark is provided by request (login, GET, ...)
         *  - Request page-mark match last session page-mark (normal interaction)
         *  - Coming from a goto: page-mark has already been checked at the very start
         */
        try {
            if ($goto || !$req_pmark || $req_pmark == $app->page_mark) {
                $app->page_mark = $pmark;
                $app->esecutivo();
                }
            // _____________________ Old request: resend actual HTML without execution ___
            else {
                $app->statusbar_text = "ATTENTION! This pages are dinamic context. ".
                                       "Avoid using browser history!";
                $app->page_mark      = $pmark;
                $app->esecutivo(true);
                }
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            return false;
            }
        // _______________________________________________________________ Page suffix ___
        o2html::page_suffix();
        // ___________________________________________________________ Custom response ___
        if ($app->custom_response) {
            $resp = ob_get_clean();
            header("Content-type: text/plain; charset=". $app->chr_encoding);
            print substr($resp, 0, $app->custom_response);
            $app->custom_response = false;
            return;
            }
        // ______________________________________________________ Output buffering end ___
        ob_end_flush();

        }


    /**
     * Prints out HTML before application execution
     *
     * @param integer $page_mark
     */
    static function page_prefix($page_mark = 0) {

        $app            = $_SESSION['o2_app'];
        $app->open_wins = array();
        // _______________________________________________ Skip head in AJAX responses ___
        if (!$GLOBALS['jxjsorigin']) {
            // ___________________________________________________ HTML document start ___
            $cli_lang = $app->option_get("cli_lang");
            print "<!DOCTYPE HTML>\n".
                  "<html".($cli_lang ? ' lang="'.$cli_lang.'"' : "").
                  ">\n<head>\n<meta charset=\"".$app->chr_encoding."\">\n".
                  "<meta name=\"application-name\" content=\"".$app->nome.
                  "\">\n<meta name=\"description\" content=\"".$app->titolo.
                  "\">\n<meta name=\"GENERATOR\" content=\"Janox - www.janox.it\">\n".
                  "<title>".$app->titolo."</title>\n";
            // _________________________________________________________ Favorite icon ___
            print '<link rel="icon" href="'.$app->favicon."\" type=\"image/ico\">\n";
            // _____________________________________________________________ CSS links ___
            print o2html::style();
            // ____________________________________________________________ Javascript ___
            o2html::javascript();
            // _______________________________________________________ Extra HTML HEAD ___
            print $app->extra_html_head;
            // ___________________________________________________ Document BODY start ___
            print "</head>\n<body class=\"o2_sfondo\" onLoad=\"o2jse.init();\" ".
                  "onBeforeUnload=\"o2jse.cmd.showClock();\">\n";
            // ______________________________________________________ Page HTML prefix ___
            if ($app->pagina_prima) {
                include_once $app->dir_htdocs.$app->pagina_prima;
                }
            }
        // ____________________________________________ Actual menu bar height & width ___
        $mbh = ($app->menu_attivo && $app->menu_height ?
                ' value="'.$app->menu_height.'"' : "");
        $mbw = ($app->menu_attivo && $app->menu_width ?
                ' value="'.$app->menu_width.'"' : "");
        // __________________________________________________ Actual status bar height ___
        $mbh = ' value="'.$app->status_height.'"';
        // ___________________________________________________________ Janox data form ___
        print '<form name="o2form" method="POST" action="'.$app->referer.
              '" enctype="multipart/form-data" autocomplete="off" '.
              "style=\"margin:0;border:0 none;\" onSubmit='return false;'>\n".
              '<input type="hidden" name="JXSESSNAME" value="'.$app->session_name.
              "\">\n<input type=\"hidden\" name=\"o2pagemark\" value=\"".$page_mark.
              "\">\n<input type=\"hidden\" name=\"win_list\">\n".
              "<input type=\"hidden\" name=\"win_pos\">\n".
              "<input type=\"hidden\" name=\"win_size\">\n".
              "<input type=\"hidden\" name=\"jxcsw\">\n".
              "<input type=\"hidden\" name=\"jxcsh\">\n".
              "<input type=\"hidden\" name=\"jxmbh\"".$mbh.">\n".
              "<input type=\"hidden\" name=\"jxmbw\"".$mbw.">\n".
              "<input type=\"hidden\" name=\"jxsbh\"".$sbh.">\n".
              "<input type=\"hidden\" name=\"o2lastform\">\n".
              "<input type=\"hidden\" name=\"o2lastctrl\">\n".
              "<input type=\"hidden\" name=\"o2_prgexeid\">\n".
              "<input type=\"hidden\" name=\"o2_action\">\n".
              "<input type=\"hidden\" name=\"o2_modfields\">\n";
        $app->html_started = true;

        }


    /**
     * Prints out HTML after application execution
     *
     */
    static function page_suffix() {

        $app = $_SESSION['o2_app'];
        // ______________________________________________________________________ Menu ___
        o2html::menu();
        // ________________________________________________________________ Status-bar ___
        o2html::statusbar();
        // _________________________________________________________ Page HTML sufffix ___
        if ($app->pagina_dopo) {
            include $app->dir_htdocs.$app->pagina_dopo;
            }
        // _________________________________________________ Manage JS cached commands ___
        if ($app->js_cache) {
            print "<script>\n".$app->js_cache."\n</script>\n";
            $app->js_cache = "";
            }
        // _________________________________________________________ Caret positioning ___
        print o2html::get_caret();
        // ______________________________________________________________ Execution ID ___
        print "<script> o2jse.setExeId(".$app->progressivo_istanze."); </script>\n";
        // _____________________ (DEVELOPMENT) Console status flags and execution tree ___
        if ($app->run_level == "DEV") {
            print "<script> ".($app->log2file || $app->sqltrace ?
                               "o2jse.lab.status(".($app->log2file ? "true," : "false,").
                                                   ($app->sqltrace ? "true," : "false,").
                                                   ($app->mutelog ? "true" : "false").
                               "); " : "").$app->exe_tree()."</script>\n";
            }
        // ________________________________________________________________ Documentor ___
        if ($app->doc_enabled) {
            $prg_name = ($app->progressivo_istanze ?
                         $app->istanze_prg[$app->progressivo_istanze]->nome : "");
            print '<input type="hidden" name="o2_prgname" value="'.$prg_name."\">\n";
            }
        // ____________________________________ Skip BODY & HTML end in AJAX responses ___
        if (!$GLOBALS['jxjsorigin']) {
            // _____________________________________________________ Document BODY end ___
            print '</form></body></html>';
            }

        }


    /**
     * Returns code for style setting in HTML context
     *
     */
    static function style() {

        $app     = $_SESSION['o2_app'];
        $code    = "";
        $css_str = $app->css;
        // ______ An exclamation mark (!) inside CSS string avoid system style loading ___
        if (strpos($css_str, "!") !== false) {
            $css_str = str_replace("!", "", $css_str);
            }
        // _______________________________________________ Janox default style & stock ___
        else {
            $code.= '<link rel="stylesheet" type="text/css" href="'.
                    $app->runtime->alias."css/janox.css\">\n";
            }
        $css_list = $app->option_get('css_str');
        $css_list = trim($css_str).($css_list ? "," : "").trim($css_list);
        // _____________________________________________________ Include all CSS files ___
        foreach (explode(",", $css_list) as $single_css) {
            if ($css_ref = $app->file_in_uso("CSS", $single_css)) {
                $code.= '<link rel="stylesheet" type="text/css" href="'.$css_ref."\">\n";
                }
            }
        return $code;

        }


    /**
     * Outputs javascript includes and settings
     *
     */
    static function javascript() {

        $app       = $_SESSION['o2_app'];
        $rnt_alias = $app->runtime->alias;
        $so        = '<script src="';
        $sc        = "\"></script>\n";
        $lu_time   = $app->option_get('lookup_time');
        print $so.$rnt_alias.'js/'.$app->js_file.$sc.
              ($app->run_level == 'DEV' ? $so.$rnt_alias.'js/dev.js'.$sc : '').
              ($app->menu_link ? $so.$app->menu_link.$sc : '').
              "<script>\no2jse.conf.user('".$app->user."');\n".
              ($app->user_is_admin ? "o2jse.conf.superUser();\n" : '').
              "o2jse.conf.rntAlias('".$rnt_alias."');\n".
              "o2jse.conf.sessName('".$app->session_name."');\n".
              ($app->keep_alive ?
               'o2jse.conf.keepAlive('.($app->timeout * 60 - 10).", '".
                                        $app->instance_id."');\n" : '').
              ($app->refresh_prg && $app->refresh_time && !$app->lock ?
               'o2jse.conf.setRefresh('.$app->refresh_time.");\n" : '').
              ($lu_time >= 300 ? 'o2jse.conf.luReqTime('.$lu_time.");\n" : '').
              'o2jse.conf.setMaxReq('.$app->max_req_n.");\n".
              'o2jse.conf.setFastMsgTime('.$app->fastmsg_time.");\n".
              "o2jse.conf.decimalsPoint('".$app->decimals_point()."');\n".
              "o2jse.conf.thousandsPoint('".$app->thousands_point()."');\n".
              ($app->grid_plus ? 'o2jse.conf.gridPlus('.
                                ($app->option_get('tab_settings_save') ? '"'.
                                 $app->option_get('tab_settings_save').'"' : '!1').','.
                                ($app->grid_multidel ? '!0' : '!1').");\n" : '').
              "o2jse.conf.gridDblClick('".($app->option_get('tab_dblclick'))."');\n".
              "o2jse.conf.gridWheel('".($app->option_get('tab_wheel'))."');\n".
              ($app->menu_style != 'T' ?
               "o2jse.conf.menuStyle('".($app->menu_style)."');\n" : '').
              "o2jse.conf.contextMenu({app:".($app->option_get('menu_app') ? '!0' : '!1').
                                     ',newsess:'.
                                       ($app->option_get('menu_newsess') ? '!0' : '!1').
                                     ',help:'.
                                       ($app->doc_enabled &&
                                        $app->option_get('menu_help') ? '!0' : '!1').
                                     "});\n".
              ($app->profiling ? "o2jse.conf.setProfiling('".$app->profiling."');\n" :'').
              ($app->doc_enabled ? "o2jse.conf.doc();\n" : '').
              ($app->run_level == 'DEV' ?
               "o2jse.conf.devel('".$app->developer."');\n".$app->rights_list().
                                                            $app->roles_list().
                                                            $app->reqdata_list() : '');
        // __________________________________________________________ Keyboard control ___
        $ctrls_list   = array('sele'   => 15,
                              'detail' => 16,
                              'new'    => 11,
                              'del'    => 12,
                              'save'   => 13,
                              'undo'   => 14,
                              'first'  => 1,
                              'prevpg' => 2,
                              'prev'   => 3,
                              'next'   => 4,
                              'nextpg' => 5,
                              'last'   => 6);
        $script_local = "";
        // ______________________________________________ Navigation & record controls ___
        foreach ($ctrls_list as $code_local => $function_code) {
            $splitted_local = explode(';', $app->option_get('keyb_nav_'.$code_local));
            $key_local      = intval($splitted_local[0]);
            $shift_local    = ($splitted_local[1] ? '!0' : '!1');
            $ctrl_local     = ($splitted_local[2] ? '!0' : '!1');
            $alt_local      = ($splitted_local[3] ? '!0' : '!1');
            print '/* '.str_pad($code_local, 7).' */ o2jse.conf.addKey('.
                  $key_local.','.
                  $shift_local.','.
                  $ctrl_local.','.
                  $alt_local.','.
                  $function_code.");\n";
            }
        // ___________________________________________________________________ Refresh ___
        $splitted_local = explode(';', $app->option_get('keyb_form_refresh'));
        $key_local      = intval($splitted_local[0]);
        $shift_local    = ($splitted_local[1] ? '!0' : '!1');
        $ctrl_local     = ($splitted_local[2] ? '!0' : '!1');
        $alt_local      = ($splitted_local[3] ? '!0' : '!1');
        print '/* Refresh */ o2jse.conf.addKey('.
                  $key_local.','.
                  $shift_local.','.
                  $ctrl_local.','.
                  $alt_local.",17);\n";
        // ______________________________________________________________________ Exit ___
        $splitted_local = explode(';', $app->option_get('keyb_form_exit'));
        $key_local      = intval($splitted_local[0]);
        $shift_local    = ($splitted_local[1] ? '!0' : '!1');
        $ctrl_local     = ($splitted_local[2] ? '!0' : '!1');
        $alt_local      = ($splitted_local[3] ? '!0' : '!1');
        print '/* Exit    */ o2jse.conf.addKey('.
                  $key_local.','.
                  $shift_local.','.
                  $ctrl_local.','.
                  $alt_local.",19);\n";
        // ______________________________________________________________________ Zoom ___
        if ($app->zoom) {
            $splitted_local = explode(';', $app->option_get('keyb_ctrl_zoom'));
            $key_local      = intval($splitted_local[0]);
            $shift_local    = ($splitted_local[1] ? '!0' : '!1');
            $ctrl_local     = ($splitted_local[2] ? '!0' : '!1');
            $alt_local      = ($splitted_local[3] ? '!0' : '!1');
            print '/* Zoom    */ o2jse.conf.addKey('.
                      $key_local.','.
                      $shift_local.','.
                      $ctrl_local.','.
                      $alt_local.",20);\n";
            }
        // _________________________________________________________ Documentor (help) ___
        if ($app->doc_enabled) {
            $splitted_local = explode(';', $app->option_get('keyb_help'));
            $key_local      = intval($splitted_local[0]);
            $shift_local    = ($splitted_local[1] ? '!0' : '!1');
            $ctrl_local     = ($splitted_local[2] ? '!0' : '!1');
            $alt_local      = ($splitted_local[3] ? '!0' : '!1');
            if (!$key_local) {
                $key_local   = '112';
                $shift_local = '!0';
                $ctrl_local  = '!1';
                $alt_local   = '!1';
                }
            print '/* Doc     */ o2jse.conf.addKey('.
                      $key_local.','.
                      $shift_local.','.
                      $ctrl_local.','.
                      $alt_local.",18);\n";
            }
        print "</script>\n";

        }


    /**
     * Print out Javascript code to display the Fast Message box
     *
     * @param string $msg_code   HTML code to display in the box
     */
    static function fast_msg($msg_code) {

        if (substr(ltrim($msg_code), 0, 1) !== '<') {
            $msg_code = '<span>'.$msg_code.'</span>';
            }
        print "<script> o2jse.fastMsg.show('".addcslashes(jx_encode($msg_code),"'\\\r\n").
              "'); </script>\n";

        }


    /**
     * Returns javascript code needed for cursor positioning (focus)
     *
     * @param  boolean $scriptless   Suppress script tag (for AJAX response)
     * @return string
     */
    static function get_caret($scriptless = false) {

        $app      = $_SESSION['o2_app'];
        $prg_id   = $app->progressivo_istanze;
        $js_local = "";
        if (isset($app->caret_position[$prg_id])) {
            $ctrl_name = $app->caret_position[$prg_id]['ctrl'];
            $html_form = $app->caret_position[$prg_id]['form'];
            $sele_mode = $app->caret_position[$prg_id]['sele'];
            $internal  = $app->caret_position[$prg_id]['int'];
            $o2_form   = str_replace("_".$prg_id."_html_form", "", $html_form);
            $form_obj  = $app->istanze_prg[$prg_id]->form[$o2_form];
            if (!$internal && !$form_obj) {
                throw new o2_exception("<b>FOCUS SETTING ERROR:</b> cannot find form <i>".
                                       substr($o2_form, 0, -5)."</i>.",
                                       o2error_UNKNOWNFORM);
                return false;
                }
            elseif (!$internal &&
                    !isset($form_obj->controlli[$ctrl_name]) &&
                    $form_obj->defined) {
                throw new o2_exception("<b>FOCUS SETTING ERROR:</b> ".
                                       "cannot find control <i>".$ctrl_name.
                                       "</i> in form <i>".substr($o2_form, 0, -5)."</i>.",
                                       o2error_UNKNOWNCTRL);
                return false;
                }
            elseif (!$form_obj->chiusa &&
                    isset($form_obj->controlli[$ctrl_name]) &&
                    $form_obj->controlli[$ctrl_name]->is_visible() &&
                    ($form_obj->controlli[$ctrl_name]->modificabile ||
                     $form_obj->controlli[$ctrl_name]->tipo_ctrl == 'tab')) {
                $sele_mode = ($sele_mode === null ? "null" :
                              ($sele_mode === 0 ? "0" : "1"));
                $js_local  = ($scriptless ? "" : "<script> o2jse.cmd.focus(").
                              "'".$ctrl_name.$prg_id."',".$sele_mode.
                              ($scriptless ? "" : "); </script>\n");
                }
            unset($app->caret_position[$prg_id]);
            }
        return $js_local;

        }


    /**
     * Receives AJAX style requests for lookup control
     *
     */
    static function popup_req($exp) {

        $app = $_SESSION['o2_app'];
        ob_end_clean();
        header("Content-type: text/javascript; charset=".$app->chr_encoding);
        eval("\$cont = ".$exp.";");
        print $cont;
        die();

        }


    /**
     * Define menu in HTML/JavaScript context and call menu composition method
     *
     */
    static function menu() {

        $app       = $_SESSION['o2_app'];
        $menu_name = $app->menu_attivo;
        $with_list = ($app->profiling == "M" || $app->profiling == "B");
        // ____________________________________________________________ Prints out bar ___
        if ($menu_name) {
            if (!$app->menu_cache) {
                if ($app->multilang) {
                    // ______________________________________________ /app/lang/<lang> ___
                    $lang_dir = new o2_dir($app->dir_home.'/lang/'.$app->multilang.'/');
                    // __________________ MENU repository: translate values from label ___
                    if (!isset($app->translation['#MENU'])) {
                        // _________________________________________ Translation table ___
                        if ($lang_dir->exists('#MENU.lng')) {
                            $app->translation['#MENU'] = jx_parse_conf($lang_dir.
                                                                        '#MENU.lng');
                            $app->translate_menu();
                            }
                        unset($app->translation['#MENU']);
                        }
                    }
                if ($with_list) {
                    $app->menu_prgs = array();
                    }
                $app->menu_cache = o2html::menu_bar($app->menu[$menu_name], true);
                }
            print $app->menu_cache;
            }
        else {
            print "<script> o2jse.menu.appMainMenu=false; </script>\n";
            }
        // _____________________________________________________ Menu must be produced ___
        if (!$app->menu_link &&
            ($menu_name || $app->user_is_admin || $app->run_level == "DEV")) {
            print "<script>\n";
            // ____________________________________________________ Start caching code ___
            $htmllen = ob_get_length();
            // _________________________________________________________ Standard menu ___
            if ($menu_name) {
                print "o2jse.menu.appMainMenu='".$menu_name.
                      "'\no2jse.menu.appMenu = function() {\no2jse.menu.addMenu('".
                      $menu_name."');\n";
                o2html::menu_item($app->menu[$menu_name]);
                print "};\n";
                }
            // ____________________________________________________________ Admin menu ___
            if ($app->user_is_admin || $app->run_level == "DEV") {
                // ____________________________________________ Menu translation tools ___
                if ($app->multilang) {
                    o2html::lang_menu($app->menu["o2admin"]);
                    }
                print "o2jse.menu.admMenu = function() {\n";
                o2html::menu_item_adm($app->menu["o2admin"]);
                print "};\n";
                }
            $menufile = $app->dir_tmp.($menu_name ? $menu_name : "adm").".js";
            file_put_contents($menufile, substr(ob_get_contents(), $htmllen));
            print "</script>\n";
            $app->menu_link = $app->file2http($menufile, 0, true);
            $app->menu      = array();
            }

        }


    /**
     * Create language menu and add it to passed menu
     *
     * @param o2_menubar $o2ctrl_obj   Related o2_menubar object
     */
    static function lang_menu($o2ctrl_obj) {

        $app = $_SESSION['o2_app'];
        unset($app->menu["jxmlt_manager"]);
        unset($app->menu["jxmlt_dict"]);
        unset($app->menu["jxmlt_table"]);
        // _____________________________________________________________ Language menu ___
        $app->menu["jxmultilang"] = new o2_menubar("jxmultilang", "lang.png");
        $lang_menu                = $app->menu["jxmultilang"];
        // ______________________________________________________ Translations manager ___
        $lang_menu->voce("P",
                         "jxmlt_manager",
                         "Translations manager",
                         "tools/jxtranslate",
                         true,
                         true,
                         "mlt_manager.png");
        $lang_menu->voce("S", "jxmltsep");
        // ________________________________________________ Manage dictionaries folder ___
        $lang_menu->voce("P",
                         "jxmlt_dict",
                         "Dictionaries folder",
                         "tools/jxmlt_folder",
                         true,
                         true,
                         "mlt_folder.png");
        // _________________________________________________ Browse translations table ___
        $lang_menu->voce("P",
                         "jxmlt_table",
                         "Browse translations data",
                         "tools/jxmlt_data",
                         true,
                         true,
                         "mlt_data.png");
        // ___________________________________________ Add language menu to admin menu ___
        $o2ctrl_obj->voce("M", "jxmultilang", "Language", "", true, true, "lang.png");

        }


    /**
     * Descend a menu recursively and compose structure in HTML/JavaScript context
     *
     * @param o2_menubar $o2ctrl_obj         Related o2_menubar object
     * @param boolean    $create_prgs_list   If TRUE creates list of prgs callable by URL
     */
    static function menu_item($o2ctrl_obj, $create_prgs_list = false) {

        $app       = $_SESSION['o2_app'];
        $a         = array();
        $t         = "with (o2jse.menu.menuList['".$o2ctrl_obj->nome."']) {\n";
        $with_list = ($app->profiling == 'M' || $app->profiling == 'B');
        if (is_array($o2ctrl_obj->voci)) {
            $first_item = true;
            $last_sep   = false;
            foreach ($o2ctrl_obj->voci as $item) {
                $add = true;
                // __________________________________________ Skip multiple separators ___
                if ($item->tipo == 'S') {
                    if ($last_sep || $first_item) {
                        $add = false;
                        }
                    $last_sep = true;
                    }
                elseif ($item->tipo == 'M') {
                    $sub_items = $app->menu[$item->nome]->voci;
                    // ______________________________________________ Skip empty menus ___
                    if (is_array($sub_items) && count($sub_items)) {
                        $a[] = $item->nome;
                        }
                    else {
                        $add = false;
                        }
                    $last_sep = false;
                    }
                else {
                    $last_sep = false;
                    }
                // __________________________________________________________ Add item ___
                if ($add) {
                    $t.= 'addItem("'.$item->tipo.'","'.
                                     $item->nome.'","'.
                                     jx_encode($item->label).'","'.
                                     $item->data.'","'.
                                     ($item->ico ? 'img/'.$item->ico : '')."\");\n";
                    if ($with_list && $item->tipo == 'P') {
                        $app->menu_prgs[$item->data] = true;
                        }
                    $first_item = false;
                    }
                }
            }
        print $t."}\n";
        foreach ($a as $item) {
            o2html::menu_item($app->menu[$item]);
            }

        }


    /**
     * Descend an administrative menu recursively and compose structure in JavaScript
     * context
     *
     * @param o2_menubar $o2ctrl_obj   Related o2_menubar object
     */
    static function menu_item_adm($o2ctrl_obj) {

        $a         = array();
        $t         = "with (o2jse.menu.menuList['".$o2ctrl_obj->nome."']) {\n";
        $app       = $_SESSION['o2_app'];
        $rnt_alias = $app->runtime->alias;
        $with_list = ($app->profiling == "M" || $app->profiling == "B");
        foreach ($o2ctrl_obj->voci as $item) {
            $t.= 'addItem("'.$item->tipo.'","'.
                             $item->nome.'","'.
                             jx_encode($item->label).'","'.
                             $item->data.'","'.
                             $rnt_alias."img/".$item->ico."\");\n";
            if ($item->tipo == "M") {
                $a[] = $item->nome;
                }
            if ($with_list && $item->tipo == "P") {
                $app->menu_prgs[$item->data] = true;
                }
            }
        print $t."}\n";
        foreach ($a as $item) {
            o2html::menu_item_adm($app->menu[$item]);
            }

        }


    /**
     * Display menu as menu bar
     *
     * @param o2_menubar $o2ctrl_obj   Related o2_menubar object
     * @param boolean    $as_code
     */
    static function menu_bar($o2ctrl_obj, $as_code = false) {

        $app       = $_SESSION['o2_app'];
        $hide      = $app->menu_style == 'H';
        $left      = $app->menu_style == 'L' || $hide;
        $html      = "<!-- menu start -->\n<div id=\"jxMenuBar\" class=\"o2menuBar".
                     ($hide ? 'H' : ($left ? 'V' : '')).
                   "\" onMouseOut='o2jse.menu.closeByTime();' style='position:fixed;".
                   ($hide ? 'display:none;' : '')."'>\n";
        $with_list = ($app->profiling == "M" || $app->profiling == "B");
        foreach ($o2ctrl_obj->voci as $item) {
            if ($item->tipo == "S") {
                $html.= "<div class='o2menuSep".($left ? '' : 'V')."'>&nbsp;</div>\n";
                }
            else {
                $label = jx_encode($item->label);
                $icon  = ($item->ico ?
                          '<div class="o2menuImg"><img style="display:inline;" src="'.
                          $app->alias.'img/'.$item->ico.'"></div>' : '');
                $o2p   = 'o2="c:\''.$item->nome.
                         '\',cT:\'menuitem\',itemLabel:\''.$label.'\'" ';
                $html .= '<div class="o2menuItem" '.$o2p.
                         ($app->option_get("menu_open_on") != "O" ?
                          'onMouseOver="o2jse.menu.onOverBarItem(this,\''.$item->tipo.
                          '\',\''.$item->nome.
                          '\');" onClick="o2jse.menu.onClickBarItem(this,\''.$item->tipo.
                          '\',\''.$item->nome.'\''.
                          ($item->tipo == "P" ? ",'".$item->data."'" : '').');"' :
                          'onMouseOver="o2jse.menu.activateBarItem(this,\''.$item->tipo.
                          "','".$item->nome.'\');"'.
                          ($item->tipo == "P" ?
                           ' onClick="o2jse.menu.onClickBarItem(this,\''.$item->tipo.
                           '\',\''.$item->nome."','".$item->data."');\"" : '')).
                          ' onMouseOut="o2jse.menu.onOutBarItem(this,\''.$item->tipo.
                          '\',\''.$item->nome.'\');">'.$icon.
                          '<span class="o2menuLabel">'.$label."</span></div>\n";
                if ($with_list && $item->tipo == "P") {
                    $app->menu_prgs[$item->data] = true;
                    }
                }
            }
        $html.= "</div>\n<script> o2jse.menu.appMainMenu='".$o2ctrl_obj->nome.
                "'; </script>\n<!-- menu end -->\n";
        if (!$as_code) {
            print $html;
            }
        if ($hide) {
            $html.= o2html::menu_bar_button($as_code);
            }
        return $html;

        }


    /**
     * Display button to show/hide menu bar
     *
     * @param boolean    $as_code
     */
    static function menu_bar_button($as_code =  false) {

        $html = '<div id="jxMenuButton" class="o2menuButton" style="position:fixed" '.
                'onClick="o2jse.menu.menuBtn();">&nbsp;</div>';
        if (!$as_code) {
            print $html;
            }
        return $html;

        }


    static function statusbar() {

        $app     = $_SESSION['o2_app'];
        $refresh = ($app->refresh_prg && $app->refresh_time && !$app->lock);
        if ($refresh) {
            $time = time();
            // ____________ Unified (server & client side) timeout for refresh program ___
            if (($time - $app->refresh_last) > $app->refresh_time) {
                $app->refresh_last = $time;
                $app->intcall($app->refresh_prg);
                }
            }
        // _____________________________________________ User & Developer informations ___
        $user_info = '<img style="display:inline" src="'.$app->runtime->alias.
                     'img/user.png" width="12px" height="12px"> &nbsp;'.
                     ($app->user != 'default' ? $app->user : 'anonymous');
        $dev_info  = ($app->run_level == 'DEV' ?
                      '&nbsp;<img src="'.$app->runtime->alias.
                      'img/dev.png" width="12px" height="12px"> &nbsp;'.
                      $app->developer : '');
        $info      = '<div class="o2statusinfo" onclick="o2jse.cMenu.setOn();">'.
                     $user_info.$dev_info.'</div>';
        // _________________________________________________ Custom HTML on status-bar ___
        if ($app->status_custom) {
            $c_txt     = jx_encode($app->status_custom);
            $c_txt_len = strlen($app->status_custom);
            if ($app->status_custom{0} == '|') {
                $custom_info = $info.'<div class="o2statustext">'.
                               jx_encode(substr($app->status_custom, 1)).'</div>';
                }
            elseif ($app->status_custom{$c_txt_len - 1} == '|') {
                $custom_info = '<div class="o2statustext">'.
                               jx_encode(substr($app->status_custom, 0, -1)).
                               '</div>'.$info;
                }
            else {
                $custom_info = '<div class="o2statustext">'.
                               jx_encode($app->status_custom).'</div>';
                }
            }
        else {
            $custom_info = $info;
            }
        if ($app->statusbar_text) {
            $status = $app->statusbar_text;
            $app->statusbar_text = '';
            }
        // _________________________________________________________ Notification area ___
        $dispatcher = o2_dispatcher::get_dispatcher();
        $active     = false;
        $count      = $dispatcher->count();
        // ___________________________________________________________ Status 1 (info) ___
        if ($count) {
            $tooltip    = 'You have '.$count.' unread messages';
            $notify_img = $app->runtime->alias.'img/notify/msg.png';
            $active     = true;
            }
        // __________________________________________________________ Status 0 (empty) ___
        else {
            $tooltip    = 'No messages';
            $notify_img = $app->runtime->alias.'img/notify/'.
                          ($refresh ? 'empty.png' : 'off.png');
            $active     = false;
            }
        if ($active || $refresh) {
            $notify_js  = ' onClick="o2jse.notify.clickOnIcon();"';
            $notify_css = ' style="cursor:pointer;"';
            }
        else {
            $notify_js  = '';
            $notify_css = '';
            }
        $notify    = '<div id="jxnotify" class="o2notify"'.$notify_js.$notify_css.
                     ' title="'.$tooltip.'"><img src="'.$notify_img.'"><div>'.
                     ($count ? $count : '&nbsp;').'</div></div>';
        // _______________________________________________________________ Compose all ___
        print '<div id="o2status" class="o2statusbar">'.$custom_info.
              '<div id="o2statustext" class="o2statustext">'.$status.'</div>'.
              $notify."</div>\n";
        $alert = $dispatcher->get_alert();
        if ($alert) {
            print "<script> alert('".addcslashes($alert, "\r\n\'\"\\")."');\n </script>";
            }

        }


    /**
     * Reads submitted values from request
     *
     */
    static function receive() {

//o2log($_REQUEST, 1);
        $app          = $_SESSION['o2_app'];
        $posted_ctrls = array();
        $app->ritorno = false;
        $prgexe_id    = intval($_REQUEST['o2_prgexeid']);
        // ___________ Sets actual executing prg to EXEID coming from POST (Menu form) ___
        if (!$prgexe_id) {
            $prgexe_id = $app->progressivo_istanze;
            }
        // _____________________________________ Programs called down from the exe one ___
        elseif ($prgexe_id < $app->progressivo_istanze) {
            for ($exe_id = $app->progressivo_istanze; $exe_id > $prgexe_id; $exe_id--) {
                // _______________________________________________________ End program ___
                $app->termina_prg($exe_id);
                // ________________________________________ Delete call and parameters ___
                unset($app->chiamate[$exe_id]);
                }
            $app->progressivo_istanze = $prgexe_id;
            }
        elseif ($prgexe_id > $app->progressivo_istanze) {
            return false;
            }
        $prg_local = $app->istanze_prg[$app->progressivo_istanze];
        // _______________________________________________________________ Get uploads ___
        o2html::get_uploads();
        // ___________________________________________________ Get client informations ___
        o2html::windows_info();
        // ______________________________________________________ Sync views selection ___
        if (is_array($prg_local->contesto)) {
            foreach ($prg_local->contesto as &$single_view) {
                $sele_name = $single_view->nome."_vs".$prgexe_id;
                if ($_REQUEST[$sele_name]) {
                    $sele_value = $_REQUEST[$sele_name];
                    if ($single_view->selezione !== ($sele_value - 1)) {
                        if ($single_view->record_suffix()) {
                            $single_view->seleziona_riga($sele_value - 1);
                            }
                        }
                    }
                }
            }
        // _________________________________________________ Gets modified fields list ___
        if (strpos($_REQUEST["o2_modfields"], ";") !== false) {
            $posted_ctrls = explode(";", $_REQUEST["o2_modfields"]);
            array_pop($posted_ctrls);
            $selector  = false;
            $check_soc = false;
            // ___________________________________________ Loop on all posted controls ___
            foreach ($posted_ctrls as $single_ctrl) {
                $ctrl_name = $single_ctrl.$app->progressivo_istanze;
                // ________________________________ Recover controls from posted names ___
                $ctrl_obj = false;
                foreach ($prg_local->form as &$single_form) {
                    // _____________________________________ Gets window close actions ___
                    if ($_REQUEST[$single_form->nome.$app->progressivo_istanze.
                                  '_jxcmd'] == '1') {
                        $single_form->close();
                        $ctrl_obj = true;
                        break;
                        }
                    // __________________________ Gets window maximize/restore actions ___
                    elseif ($_REQUEST[$single_form->nome.$app->progressivo_istanze.
                                      '_jxcmd'] == '2') {
                        $single_form->max_restore();
                        $ctrl_obj = true;
                        break;
                        }
                    // ____________________________________ Gets window resize actions ___
                    elseif ($_REQUEST[$single_form->nome.$app->progressivo_istanze.
                                      '_jxcmd'] == '3') {
                        $single_form->maximized   = false;
                        $ctrl_obj                 = true;
                        break;
                        }
                    // ________________ Force resize maximized window (browser resize) ___
                    elseif ($_REQUEST[$single_form->nome.$app->progressivo_istanze.
                                      '_jxcmd'] == '4') {
                        $single_form->maximized   = false;
                        $single_form->max_restore();
                        $single_form->restore_width  = 0;
                        $single_form->restore_height = 0;
                        $ctrl_obj                    = true;
                        break;
                        }
                    elseif (isset($single_form->controlli[$single_ctrl])) {
                        $ctrl_obj = $single_form->controlli[$single_ctrl];
                        break;
                        }
                    }
                if ($ctrl_obj) {
                    $view_obj = $prg_local->contesto[$ctrl_obj->task];
                    // ______________________________ Receive controls to valued field ___
                    /**
                     * NOTE: Submit-on-change on edit fields are relative to source row,
                     *       but submit-on-change on check-boxes are relative to target
                     *       row.
                     */
                    if ($ctrl_obj->campo_in_ctrl &&
                        ($ctrl_obj->tipo_ctrl != 'checkbox' ||
                         !$ctrl_obj->forza_submit)) {
                        // ____________ Full-AJAX mode (convert sent value from UTF-8) ___
                        if ($GLOBALS['jxjs']) {
                            $ctrl_obj->valore = iconv("UTF-8",
                                                      $app->chr_encoding."//TRANSLIT",
                                                      $_REQUEST[$ctrl_name]);
                            }
                        else {
                            $ctrl_obj->valore = $_REQUEST[$ctrl_name];
                            }
                        $ctrl_obj->valore = $ctrl_obj->ctrl2sql();
                        // ____________________ Lookup: server-side description decode ___
                        if ($ctrl_obj->tipo_ctrl == 'listcombo' &&
                            isset($_REQUEST[$ctrl_obj->nome.$ctrl_obj->id_esecuzione.
                                            '_force_decode'])) {
                            $descfield = $ctrl_obj->nome.$ctrl_obj->id_esecuzione.
                                         '_force_decode';
                            $descval   = ($GLOBALS['jxjs'] ?
                                          iconv("UTF-8",
                                                $app->chr_encoding."//TRANSLIT",
                                                $_REQUEST[$descfield]) :
                                          $_REQUEST[$descfield]);
                            // _______________________ Set code value from description ___
                            $view_obj->imposta($ctrl_obj->campo_in_ctrl,
                                               $ctrl_obj->code_from_desc($descval));
                            // _________________ Execution of action on control change ___
                            if ($ctrl_obj->azione_submit) {
                                $prg_local->esegui_azione($ctrl_obj->azione_submit);
                                }
                            }
                        // _______ Treeview: manage value, selection and submit action ___
                        elseif ($ctrl_obj->tipo_ctrl == 'tree') {
                            $field = (get_class($view_obj) == 'o2_virtualview' ?
                                      $view_obj->variabili[$ctrl_obj->campo_in_ctrl] :
                                      $view_obj->campi[$ctrl_obj->campo_in_ctrl]);
                            // _____________________ Structure field: set to node item ___
                            if ($field->maschera->tipo == 'S') {
                                $value = $ctrl_obj->list[$ctrl_obj->valore];
                                }
                            // ____________________ Other fields: set to node ID value ___
                            else {
                                $value = $ctrl_obj->valore;
                                }
                            // ___________________________________ Get vertical scroll ___
                            if (isset($_REQUEST[$ctrl_obj->nome.$ctrl_obj->id_esecuzione.
                                "_vscroll"])) {
                                $ctrl_obj->vscroll = intval($_REQUEST[$ctrl_obj->nome.
                                                            $ctrl_obj->id_esecuzione.
                                                            "_vscroll"]);
                                }
                            $view_obj->imposta($ctrl_obj->campo_in_ctrl, $value);
                            $ctrl_obj->node_select($ctrl_obj->valore);
                            }
                        // ____________________ Imgs-lister: manage value, and actions ___
                        elseif ($ctrl_obj->tipo_ctrl == 'imglist') {
                            $field = (get_class($view_obj) == 'o2_virtualview' ?
                                      $view_obj->variabili[$ctrl_obj->campo_in_ctrl] :
                                      $view_obj->campi[$ctrl_obj->campo_in_ctrl]);
                            // __________________________ Structure field: set to item ___
                            if ($field->maschera->tipo == 'S') {
                                $value = $ctrl_obj->list[$ctrl_obj->valore];
                                }
                            // ____________________ Other fields: set to item ID value ___
                            else {
                                $value = $ctrl_obj->valore;
                                }
                            $view_obj->imposta($ctrl_obj->campo_in_ctrl, $value);
                            if (isset($_REQUEST[$ctrl_name.'_delete'])) {
                                $ctrl_obj->item_delete($ctrl_obj->valore);
                                }
                            else {
                                $ctrl_obj->item_activate($ctrl_obj->valore);
                                }
                            // ___________________________________ Get vertical scroll ___
                            if (isset($_REQUEST[$ctrl_obj->nome.$ctrl_obj->id_esecuzione.
                                "_vscroll"])) {
                                $ctrl_obj->vscroll = intval($_REQUEST[$ctrl_obj->nome.
                                                            $ctrl_obj->id_esecuzione.
                                                            "_vscroll"]);
                                }
                            }
                        // _____________________________________ Other valued controls ___
                        else {
                            $view_obj->imposta($ctrl_obj->campo_in_ctrl,
                                               $ctrl_obj->valore);
                            // _________________ Execution of action on control change ___
                            if ($ctrl_obj->azione_submit) {
                                $prg_local->esegui_azione($ctrl_obj->azione_submit);
                                }
                            }
                        }
                    switch ($ctrl_obj->tipo_ctrl) {
                        // _____________________________________ Receive table actions ___
                        case "tab":
                            $form_obj = $prg_local->form[$ctrl_obj->form];
                            // _________________________________ Get horizontal scroll ___
                            if ($_REQUEST[$ctrl_obj->nome.$ctrl_obj->id_esecuzione.
                                "_hscroll"] !== "") {
                                $ctrl_obj->hscroll = intval($_REQUEST[$ctrl_obj->nome.
                                                            $ctrl_obj->id_esecuzione.
                                                            "_hscroll"]);
                                }
                            // ___________________________________ Get vertical scroll ___
                            if ($_REQUEST[$ctrl_obj->nome.$ctrl_obj->id_esecuzione.
                                "_vscroll"] !== "") {
                                $ctrl_obj->vscroll = intval($_REQUEST[$ctrl_obj->nome.
                                                            $ctrl_obj->id_esecuzione.
                                                            "_vscroll"]);
                                }
                            // ________________________________ Grid specific commands ___
                            $colex_cmd = $single_ctrl.'_jxcexe'.$app->progressivo_istanze;
                            $sort_cmd  = $single_ctrl.'_jxsort'.$app->progressivo_istanze;
                            $size_cmd  = $single_ctrl.'_jxsize'.$app->progressivo_istanze;
                            $vis_cmd   = $single_ctrl.'_jxvcol'.$app->progressivo_istanze;
                            $fltr_cmd  = $single_ctrl.'_jxfltr'.$app->progressivo_istanze;
                            $msort_cmd = $single_ctrl.'_jxscol'.$app->progressivo_istanze;
                            $c_ord_cmd = $single_ctrl.'_jxocol'.$app->progressivo_istanze;
                            $rowsn_cmd = $single_ctrl.'_jxrown'.$app->progressivo_istanze;
                            $exp_cmd   = $single_ctrl.'_jxdexp'.$app->progressivo_istanze;
                            $del_cmd   = $single_ctrl.'_jxddel'.$app->progressivo_istanze;
                            $save_cmd  = $single_ctrl.'_jxsave'.$app->progressivo_istanze;
                            $reset_cmd = $single_ctrl.'_jxresc'.$app->progressivo_istanze;
                            // _______________________ Get table column header actions ___
                            if ($_REQUEST[$colex_cmd]) {
                                $prg_local->esegui_azione($_REQUEST[$colex_cmd]);
                                unset($_REQUEST["o2_modfields"]);
                                }
                            // __________ Get table columns sorting (by column header) ___
                            elseif ($_REQUEST[$sort_cmd]) {
                                $segment = $form_obj->controlli[$_REQUEST[$sort_cmd]]->
                                            campo_in_ctrl;
                                if (array_key_exists($segment, $view_obj->sort)) {
                                    if ($view_obj->sort[$segment] == "A") {
                                        $view_obj->sort[$segment] = "D";
                                        }
                                    else {
                                        unset($view_obj->sort[$segment]);
                                        }
                                    }
                                else {
                                    $view_obj->sort[$segment] = "A";
                                    }
                                $view_obj->indici[$view_obj->file->indice] = "";
                                $view_obj->record_primo();
                                }
                            // ______________________________ Get table columns sizing ___
                            elseif ($_REQUEST[$size_cmd]) {
                                list($col, $size) = explode(":", $_REQUEST[$size_cmd]);
                                $ctrl_obj->user_set_size($col, $size);
                                unset($_REQUEST["o2_modfields"]);
                                // ____________________ Needed to resend table content ___
                                $ctrl_obj->view_obj->relocated = true;
                                }
                            // __________________________ Get table columns visibility ___
                            elseif (isset($_REQUEST[$vis_cmd])) {
                                $ctrl_obj->user_set_vis(explode(";",$_REQUEST[$vis_cmd]));
                                unset($_REQUEST["o2_modfields"]);
                                // ____________________ Needed to resend table content ___
                                $ctrl_obj->view_obj->relocated = true;
                                }
                            // _______________________________ Get table columns order ___
                            elseif (isset($_REQUEST[$c_ord_cmd])) {
                                $ctrl_obj->user_set_order($_REQUEST[$c_ord_cmd]);
                                unset($_REQUEST["o2_modfields"]);
                                // ____________________ Needed to resend table content ___
                                $ctrl_obj->view_obj->relocated = true;
                                }
                            // _____________________________________ Filter table data ___
                            elseif (isset($_REQUEST[$fltr_cmd])) {
                                $app->intcall("tools/o2sys_table_filter",
                                              0,
                                              substr($ctrl_obj->form, 0, -5),
                                              $single_ctrl);
                                }
                            // ___________ Get table columns sorting (by context menu) ___
                            elseif (isset($_REQUEST[$msort_cmd])) {
                                $ctrl_obj->user_set_sort($_REQUEST[$msort_cmd]);
                                unset($_REQUEST["o2_modfields"]);
                                // ____________________ Needed to resend table content ___
                                $ctrl_obj->view_obj->relocated = true;
                                }
                            // _______________________________________ Set rows number ___
                            elseif (isset($_REQUEST[$rowsn_cmd])) {
                                $ctrl_obj->user_set_rows(intval($_REQUEST[$rowsn_cmd]));
                                }
                            // _____________________________________ Export table data ___
                            elseif (isset($_REQUEST[$exp_cmd])) {
                                $app->intcall("tools/o2sys_export_csv", $ctrl_obj->task);
                                }
                            // _____________________________________ Delete table data ___
                            elseif (isset($_REQUEST[$del_cmd])) {
                                $app->intcall("tools/o2sys_del_grid_data",
                                              $_REQUEST[$del_cmd]); // Navigator name
                                }
                            // ___________________________ Save columns customizations ___
                            elseif (isset($_REQUEST[$save_cmd])) {
                                $ctrl_obj->user_save();
                                unset($_REQUEST["o2_modfields"]);
                                }
                            // __________________________ Reset columns customizations ___
                            elseif (isset($_REQUEST[$reset_cmd])) {
                                $ctrl_obj->user_reset();
                                unset($_REQUEST["o2_modfields"]);
                                // ____________________ Needed to resend table content ___
                                $ctrl_obj->view_obj->relocated = true;
                                }
                            else {
                                // ___________________________ Receive table selection ___
                                if ($view_obj->record_suffix()) {
                                    // ______________ Save left row for Full-AJAX mode ___
                                    if ($GLOBALS['jxjs']) {
                                        $GLOBALS['jxtablastrow'] = $view_obj->selezione +
                                                                   1;
                                        }
                                    $view_obj->seleziona_riga(
                                                       intval($_REQUEST[$ctrl_name]) - 1);
                                    }
                                else {
                                    return;
                                    }
                                }
                            break;
                        // _________________________________ Receive navigator actions ___
                        case "navigator":
                            $go_on    = true;
                            $sel_cmd  = $single_ctrl."_o2sel".$app->progressivo_istanze;
                            $sel_to   = (isset($_REQUEST[$sel_cmd]) ?
                                         intval($_REQUEST[$sel_cmd]) :
                                         false);
                            if ($sel_to) {
                                $go_on = $view_obj->record_suffix();
                                if ($go_on) {
                                    $view_obj->seleziona_riga($sel_to - 1);
                                    }
                                }
                            if ($go_on) {
                                switch (intval($_REQUEST[$ctrl_name])) {
                                    case 1:
                                        $view_obj->record_primo();
                                        break;
                                    case 2:
                                        $view_obj->record_pg_indietro();
                                        break;
                                    case 3:
                                        $view_obj->record_indietro();
                                        break;
                                    case 4:
                                        $view_obj->record_avanti();
                                        break;
                                    case 5:
                                        $view_obj->record_pg_avanti();
                                        break;
                                    case 6:
                                        $view_obj->record_ultimo();
                                        break;
                                    case 11:
                                        $prg_local->esegui_azione($ctrl_obj->act_new);
                                        break;
                                    case 12:
                                        $prg_local->esegui_azione($ctrl_obj->act_del);
                                        break;
                                    case 13:
                                        $prg_local->esegui_azione($ctrl_obj->act_save);
                                        break;
                                    case 14:
                                        $prg_local->esegui_azione($ctrl_obj->act_undo);
                                        break;
                                    case 15:
                                        $prg_local->esegui_azione($ctrl_obj->act_select);
                                        break;
                                    case 16:
                                        $prg_local->esegui_azione($ctrl_obj->act_detail);
                                        break;
                                    }
                                }
                            break;
                        // _________________________________ Checkbox as view-selector ___
                        case "checkbox":
                            // ____________________________ Check-box as view selector ___
                            if ($ctrl_obj->selector) {
                                $selector = array($view_obj, $ctrl_obj);
                                }
                            // _________________ Execution of action on control change ___
                            elseif ($ctrl_obj->forza_submit) {
                                $check_soc = array($view_obj, $ctrl_obj);
                                }
                            break;
                        }
                    }
                }
            }
        // ________________ Program actions from buttons, o2_act4js() and form refresh ___
        if ($_REQUEST["o2_action"]) {
            $act = $_REQUEST["o2_action"];
            if (isset($prg_local->azioni[$act])) {
                $prg_local->esegui_azione($act);
                }
            else {
                throw new o2_exception("Cannot find definition for action <i>".$act.
                                       "</i>", o2error_UNKNOWNACTION);
                }
            }
        // ___________________________________________________ Execute zoom on control ___
        elseif ($_REQUEST['jxzoom']) {
            $zoom_ctrl = $_REQUEST['jxzoom'];
            $ctrl_obj  = false;
            foreach ($prg_local->form as &$single_form) {
                // ________________________________________________ Get zoomed control ___
                if (isset($single_form->controlli[$zoom_ctrl])) {
                    $ctrl_obj = $single_form->controlli[$zoom_ctrl];
                    break;
                    }
                }
            if ($ctrl_obj) {
                return $ctrl_obj->zoom();
                }
            }
        // ______________________________________ Treeview control (old scripting way) ___
        elseif ($_REQUEST['jxtree']) {
            $tree_ctrl = $_REQUEST['jxtree'];
            $ctrl_obj  = false;
            // __________________________________________________ Get treeview control ___
            if (isset($prg_local->tree_ctrls[$tree_ctrl])) {
                $ctrl_obj = $prg_local->tree_ctrls[$tree_ctrl];
                }
            else {
                foreach ($prg_local->form as &$single_form) {
                    if (isset($single_form->controlli[$tree_ctrl])) {
                        $ctrl_obj = $single_form->controlli[$tree_ctrl];
                        break;
                        }
                    }
                }
            if ($ctrl_obj) {
                // _______________________________________________ Fold/unfold request ___
                if ($_REQUEST['jxtreefold']) {
                    $node_id = $_REQUEST['jxtreefold'];
                    if (isset($ctrl_obj->open_nodes[$node_id])) {
                        unset ($ctrl_obj->open_nodes[$node_id]);
                        }
                    else {
                        $ctrl_obj->open_nodes[$node_id] = true;
                        }
                    if ($_REQUEST['jxtvlastctrl']) {
                        $ctrl_obj->focus_node = $node_id;
                        }
                    }
                // _____________________________________________ Activate node request ___
                elseif ($_REQUEST['jxtreenode']) {
                    $ctrl_obj->node_select($_REQUEST['jxtreenode']);
                    }
                // ___________________________________________ Fold all foldable nodes ___
                elseif ($_REQUEST['jxtreefoldall']) {
                    $ctrl_obj->fold_all();
                    }
                // _________________________________________ Unfold all foldable nodes ___
                elseif ($_REQUEST['jxtreeunfoldall']) {
                    $ctrl_obj->unfold_all();
                    }
                // _________________________________________________ Export tree nodes ___
                elseif ($_REQUEST['jxtreeexport']) {
                    o2_send(o2_path($ctrl_obj->export()));
                    }
                // _________________________________________________ Sort/unsort nodes ___
                elseif ($_REQUEST['jxtreesort']) {
                    $ctrl_obj->switch_sort();
                    }
                }
            // ___________________________________ Get vertical scroll ___
            if (isset($_REQUEST[$ctrl_obj->nome.$ctrl_obj->id_esecuzione.'_vscroll'])) {
                $ctrl_obj->vscroll = intval($_REQUEST[$ctrl_obj->nome.
                                                      $ctrl_obj->id_esecuzione.
                                                      '_vscroll']);
                }
            }
        // _________________________________ Images lister control (old scripting way) ___
        elseif ($_REQUEST['jximglist']) {
            $imglist_ctrl = $_REQUEST['jximglist'];
            $ctrl_obj     = false;
            // _____________________________________________ Get images lister control ___
            if (isset($prg_local->imglist_ctrls[$imglist_ctrl])) {
                $ctrl_obj = $prg_local->imglist_ctrls[$imglist_ctrl];
                // ______________________________________________________ First action ___
                if (isset($_REQUEST['jximglistitem'])) {
                    $ctrl_obj->item_activate($_REQUEST['jximglistitem']);
                    }
                // _____________________________________________________ Second action ___
                elseif (isset($_REQUEST['jximglistdelete'])) {
                    $ctrl_obj->item_delete($_REQUEST['jximglistdelete']);
                    }
                }
            }
        // ______________________________________________________ JXDocumentor request ___
        elseif ($_REQUEST['jxdocreq'] && $app->doc_activate()) {
            $app->doc->show_node($app->istanze_prg[$_REQUEST['jxdocexeid']]->nome,
                                 $_REQUEST['o2_formname'],
                                 $_REQUEST['o2_ctrlname'],
                                 $_REQUEST['o2_nodeid']);
            }
        // _________________________________________________ View-selector by checkbox ___
        elseif ($selector) {
            $view_obj = $selector[0];
            $ctrl_obj = $selector[1];
            $item     = $view_obj->chiave_corrente($view_obj->file,
                                                   false,
                                                   false,
                                                   false,
                                                   true,
                                                   true,
                                                   true);
            if (($key = array_search($item, $view_obj->selections)) !== false) {
                unset($view_obj->selections[$key]);
                $value = false;
                }
            else {
                $view_obj->selections[] = $item;
                $value                  = true;
                }
            $ctrl_obj->valore                               = $value;
            $view_obj->corrente[$ctrl_obj->campo_in_ctrl]   = $value;
            $view_obj->precedente[$ctrl_obj->campo_in_ctrl] = $value;
            // _________________________________ Execution of action on control change ___
            if ($ctrl_obj->azione_submit) {
                $prg_local->esegui_azione($ctrl_obj->azione_submit);
                }
            }
        elseif ($check_soc) {
            $view_obj         = $check_soc[0];
            $ctrl_obj         = $check_soc[1];
            $ctrl_name        = $ctrl_obj->nome.$app->progressivo_istanze;
            $ctrl_obj->valore = ($_REQUEST[$ctrl_name] ? true : false);
            $view_obj->imposta($ctrl_obj->campo_in_ctrl, $ctrl_obj->valore);
            // _________________________________ Execution of action on control change ___
            if ($ctrl_obj->azione_submit) {
                $prg_local->esegui_azione($ctrl_obj->azione_submit);
                }
            }
        // __________________________________________________________ Data log history ___
        if ($_REQUEST['jxlogctrl']) {
            $app->intcall("tools/o2sys_datahistory",
                          $prg_local->log_history($_REQUEST['jxlogctrl'], true),
                          ($app->developer || $app->user_is_admin));
            }
        // ____________________________________________________ Execute system actions ___
        if ($_REQUEST['jxact']) {
            return o2html::sys_action();
            }

        }


    /**
     * Retrieves informations about client size
     *
     */
    static function client_info() {

        $app = $_SESSION['o2_app'];
        /* ======================== Browser window size =============================== *
         * CTRL          POST FIELD                   VALUE                             *
         * window        jxcsw                        width (in px)                     *
         * window        jxcsh                        height (in px)                    *
         * menu-bar      jxmbw                        width (in px)                     *
         * menu-bar      jxmbh                        height (in px)                    *
         * status-bar    jxsbh                        height (in px)                    *
         ********************************************************************************/
        if ($_REQUEST['jxcsw']) {
            $app->client_width  = intval($_REQUEST['jxcsw']);
            $app->client_height = intval($_REQUEST['jxcsh']);
            $app->menu_width    = intval($_REQUEST['jxmbw']);
            $app->menu_height   = intval($_REQUEST['jxmbh']);
            $app->status_height = intval($_REQUEST['jxsbh']);
            }

        }


    /**
     * Retrieves informations about windows size and position from client
     *
     */
    static function windows_info() {

        $app = $_SESSION['o2_app'];
        /* ================== Windows last client position and size =================== *
         * CTRL              POST FIELD               VALUE                             *
         * form              "win_list"               win1; win2; ...                   *
         * form              "win_pos"                x1,y1;x2,y2;...                   *
         * form              "win_size"               w1,h1;w2,h2;...                   *
         ********************************************************************************/
        if ($_REQUEST['win_list']) {
            $list_local = explode(";", $_REQUEST['win_list']);
            $pos        = explode(";", $_REQUEST['win_pos']);
            $size       = explode(";", $_REQUEST['win_size']);
            foreach ($list_local as $win_index => $single_win) {
                $id_local   = intval(substr(strrchr($single_win, "_"), 1));
                $name_local = substr($single_win, 0, strrpos($single_win, "_"));
                if ($id_local && isset($app->istanze_prg[$id_local])) {
                    $form_obj = $app->istanze_prg[$id_local]->form[$name_local];
                    list($x, $y)          = explode(",", $pos[$win_index]);
                    list($width, $height) = explode(",", $size[$win_index]);
                    if ($x || $y) {
                        $form_obj->align_o    = "left";
                        $form_obj->align_v    = "top";
                        $form_obj->x          = $x;
                        $form_obj->y          = $y;
                        $form_obj->absolute_x = $x;
                        $form_obj->absolute_y = $y;
                        $form_obj->custom_pos = true;
                        }
                    if ($width || $height) {
                        $form_obj->larghezza   = $width;
                        $form_obj->altezza     = $height;
                        $form_obj->custom_size = true;
                        }
                    }
                }
            }
        elseif ($_REQUEST['jxresizeall']) {
            foreach ($app->istanze_prg as $exe_id => $prg) {
                $app->progressivo_istanze = $exe_id;
                unset($app->open_wins[$exe_id]);
                foreach ($prg->form as $form) {
                    if ($form->maximized) {
                        $form->maximized = false;
                        $form->max_restore();
                        $form->resize();
                        }
                    }
                }
            }
        // ___________________________________________ Verifico la posizione del caret ___
        if ($_REQUEST['o2lastctrl']) {
            $l_form           = $_REQUEST['o2lastform'];
            $l_ctrl           = $_REQUEST['o2lastctrl'];
            $app->info_submit = array("form" => $l_form, "ctrl" => $l_ctrl);
            $app->istanze_prg[$app->progressivo_istanze]->set_caret($l_form, $l_ctrl);
            }

        }


    /**
     * Dispose URL for download
     *
     * @param string $file_url   URL to file to be sent
     */
    static function send_file($file_url) {

        // _________________________________________________________________ Full-AJAX ___
        if ($GLOBALS['jxjs']) {
            $_SESSION['o2_app']->js_cache.=
                    "var d=o2jse.createEl(document.body, 'iframe'); ".
                    "d.style.visibility='hidden'; d.src='".$file_url."';\n";
            }
        // ________________________________________________________ Standard HTML mode ___
        else {
            print '<iframe src="'.$file_url.'" style="visibility:hidden"></iframe>';
            }

        }


    /**
     * Riceve e salva i file in upload
     *
     */
    static function get_uploads() {

        $app       = $_SESSION['o2_app'];
        $prg_local = $app->istanze_prg[$app->progressivo_istanze];
        foreach ($_FILES as $ctrl_name => $singolo_file) {
            if (($singolo_file['tmp_name'] == "" && $singolo_file['size'] == 0) ||
                is_uploaded_file($singolo_file['tmp_name'])) {
                // ______________________________________________ Find related control ___
                $ctrl_name = substr($ctrl_name,
                                    0,
                                    strlen($ctrl_name) -
                                    strlen($app->progressivo_istanze));
                $ctrl_obj  = false;
                foreach ($prg_local->form as $single_form) {
                    if (isset($single_form->controlli[$ctrl_name])) {
                        $ctrl_obj = $single_form->controlli[$ctrl_name];
                        break;
                        }
                    }
                if ($ctrl_obj) {
                    // ________________________________________________ Field is blank ___
                    if (($singolo_file['tmp_name'] == "" && $singolo_file['size'] == 0)) {
                        if ($ctrl_obj->campo_in_ctrl) {
                            $prg_local->contesto[$ctrl_obj->task]->
                             imposta($ctrl_obj->campo_in_ctrl, array());
                            }
                        }
                    // _____________________________________ Field is an uploaded file ___
                    else {
                        $nome_local = $singolo_file['name'];
                        if ($ctrl_obj->salva_con_nome) {
                            $nome_local = $ctrl_obj->salva_con_nome;
                            }
                        clearstatcache();
                        if (file_exists($app->dir_uploads.$nome_local)) {
                            unlink($app->dir_uploads.$nome_local);
                            }
                        if (move_uploaded_file($singolo_file['tmp_name'],
                                               $app->dir_uploads.$nome_local)) {
                            $obj_local = new o2_fsitem($app->dir_uploads.$nome_local);
                            $obj_local->http_mime      = $singolo_file['type'];
                            $obj_local->nome_originale = $singolo_file['name'];
                            if ($ctrl_obj->campo_in_ctrl) {
                                $prg_local->contesto[$ctrl_obj->task]->
                                 imposta($ctrl_obj->campo_in_ctrl, obj2array($obj_local));
                                }
                            }
                        }
                    }
                }
            elseif ($singolo_file['error'] != 4) {
                $error_local = "";
                switch($singolo_file['error']){
                    // ______________________________ no error (don't throw exception) ___
                    case UPLOAD_ERR_OK:
                        $error_local = "sent file correctly received.";
                        break;
                    // __________ exceeds the upload_max_filesize directive in php.ini ___
                    case UPLOAD_ERR_INI_SIZE:
                        $error_local = "sent file is too big for php.ini directive.";
                        break;
                    // ________________ exceeds MAX_FILE_SIZE directive from html form ___
                    case UPLOAD_ERR_FORM_SIZE:
                        $error_local = "sent file is too big for HTML FORM directive.";
                        break;
                    // _____________________ uploaded file was only partially uploaded ___
                    case UPLOAD_ERR_PARTIAL:
                        $error_local = "sent file was only partially received.";
                        break;
                    // __________________________________________ no file was uploaded ___
                    case UPLOAD_ERR_NO_FILE:
                        $error_local = "no file was received.";
                        break;
                    // _____________________________________ Missing temporary forlder ___
                    case UPLOAD_ERR_NO_TMP_DIR:
                        $error_local = "missing temporary folder directive in php.ini.";
                        break;
                    // __________________________________ Failed to write file to disk ___
                    case UPLOAD_ERR_CANT_WRITE:
                        $error_local = "failed to write uploaded file to disk.";
                        break;
                    // _____________________________________ File stopped by extension ___
                    case UPLOAD_ERR_EXTENSION:
                        $error_local = "sent file type is not allowed.";
                        break;
                    // _________________________________________________ default error ___
                    default:
                        $error_local = "sent file not received.";
                        break;
                    }
                throw new o2_exception("Cannot receive file <i>".
                                       $singolo_file['tmp_name']."</i>:".$error_local,
                                       o2error_FILEUPLOAD);
                }
            }

        }


    /**
     * Receives AJAX style requests for notification area and returns dispatches list
     *
     */
    static function notify_response() {

        ob_end_clean();
        $app        = $_SESSION['o2_app'];
        $list       = array();
        $dispatcher = o2_dispatcher::get_dispatcher();
        $rawlist    = $dispatcher->get_list();
        foreach ($rawlist as $msg_id) {
            $msg    = $dispatcher->get_dispatch($msg_id);
            $title  = htmlentities($msg["title"], ENT_QUOTES, $app->chr_encoding);
            $body   = str_replace(array("\n", "\r", "\t"), " ", $msg["body"]);
            $body   = (strlen($body) > 100 ? substr($body, 0, 94)."[...]" : $body);
            $body   = htmlentities($body, ENT_QUOTES, $app->chr_encoding);
            $img    = ($msg["img"] ? "'".$msg["img"]."'" : "!1");
            $active = ($msg["prg"] ? "!0" : "!1");
            $label  = ($msg["label"] ? "!0" : "!1");
            $list[] = $msg_id.":['".$title."','".
                                    $body."',".
                                    $img.",".
                                    $active.",".
                                    $label."]";
            }
        print "o2jse.notify.itemsList={".implode(",\n", $list)."};\n";
        $alert = $dispatcher->get_alert();
        if ($alert) {
            print "alert('".addcslashes($alert, "\r\n\'\"\\")."');\n";
            }
        die();

        }


    /**
     * Receives HTML system actions to execute, either than standard FORMs POST and
     * calling program (o2c).
     * Used to receive AJAX style requests.
     *
     */
    static function sys_action() {

        if ($_REQUEST['jxact'] == "dispatch") {
            // _____________________________________________________ Activate dispatch ___
            $dispatcher = o2_dispatcher::get_dispatcher();
            $dispatcher->dispatch_activate($_REQUEST['jxmsgid']);
            }

        }


    /**
     * Creates HTML interface related to o2 forms.
     *
     * @param o2_form $o2form_obj   Related o2_form object
     */
    static function ctrl_win($o2form_obj) {

        print "<!-- win start -->\n";
        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$o2form_obj->id_esecuzione];
        // ____________________________________ Register window open in Full-AJAX mode ___
        $app->open_wins[$o2form_obj->id_esecuzione][$o2form_obj->nome] =
                                                          array("V" => true, "A" => true);
        $repos  = "";
        $menu_v = $app->menu_attivo && $app->menu_style == 'L';
        $menu_o = $app->menu_attivo && $app->menu_style == 'T';
        // ___________________________________________________________ NOT linked form ___
        if (!$o2form_obj->linkto) {
            $div_name = $o2form_obj->nome."_".$o2form_obj->id_esecuzione;
            // ______________________________________________________________ Position ___
            $cli_w = $app->client_width;
            $cli_h = $app->client_height;
            $bar_w = ($menu_v ? $app->menu_width : 0);
            $bar_h = ($menu_o ? $app->menu_height : 0);
            $stb_h = $app->status_height;
            // _______________ Repos is needed if client size informations are missing ___
            if (!$cli_w || ($menu_o && !$bar_v) || ($menu_v && !$bar_w)) {
                $repos = "<script> o2jse.win.needRepos('".$div_name."'); </script>\n";
                }
            // __________________________________________________ Horizontal alignment ___
            $align_buffer = "";
            switch ($o2form_obj->align_o) {
                case "center":
                    $halign_val             = max(0, intval($cli_w / 2 - $o2form_obj->x +
                                                            $bar_w / 2));
                    $align_buffer          .= "left:".$halign_val."px;";
                    $o2form_obj->absolute_x = $halign_val;
                    $halign_val             = $o2form_obj->x;
                    break;
                case "right":
                    $halign_val             = -($o2form_obj->x - $o2form_obj->larghezza);
                    $align_buffer          .= "right:".$o2form_obj->x."px;";
                    $o2form_obj->absolute_x = $halign_val;
                    break;
                default:
                    $halign_val             = $o2form_obj->x + $bar_w;
                    $align_buffer          .= "left:".$halign_val."px;";
                    $o2form_obj->absolute_x = $halign_val;
                    break;
                }
            // ____________________________________________________ Vertical alignment ___
            switch ($o2form_obj->align_v) {
                case "middle":
                    $valign_val             = max(0, intval($cli_h / 2 - $o2form_obj->y +
                                                            $bar_h / 2 - $stb_h / 2));
                    $align_buffer          .= "top:".$valign_val."px;";
                    $o2form_obj->absolute_y = $valign_val;
                    $valign_val             = $o2form_obj->y;
                    break;
                case "bottom":
                    $valign_val             = -($o2form_obj->y - $o2form_obj->altezza);
                    $align_buffer          .= "bottom:".($o2form_obj->y + $stb_h)."px;";
                    $o2form_obj->absolute_y = $valign_val;
                    break;
                default:
                    $valign_val             = $o2form_obj->y + $bar_h;
                    $align_buffer          .= "top:".$valign_val."px;";
                    $o2form_obj->absolute_y = $valign_val;
                    break;
                }
            }
        // _______________________________________________________________ Linked form ___
        else {
            $bar_h      = ($menu_o ? ($app->menu_height ? $app->menu_height : 25) : 0);
            $bar_w      = ($menu_v ? ($app->menu_width ? $app->menu_width : 100) : 0);
            $parent_win = $prg->form[$o2form_obj->linkto];
            // _____________________________________________________ Relative position ___
            $align_buffer = 'left:'.($parent_win->absolute_x + $o2form_obj->x).
                            'px; top:'.($parent_win->absolute_y + $o2form_obj->y).'px;';
            $link_id      = 0;
            foreach ($parent_win->linked as $linked_form_name => $dummy_value) {
                $link_id++;
                if ($linked_form_name == $o2form_obj->nome) {
                    break;
                    }
                }
            $div_name = $o2form_obj->linkto."_".$o2form_obj->id_esecuzione.$link_id;
            }
        $div_in_name = $div_name."_in";
        // ___________________________________________________________ ENABLE KEYBOARD ___
        if ($o2form_obj->attiva) {
            $keyboard_script_local = ' onKeyDown="o2jse.ctrl.k(event);"';
            }
        // ========================= [   INIZIO FINESTRA o2_form   ] =====================
        $nowin = false;
        if ($o2form_obj->css_finestra == "nowindow") {
            $nowin = "nowindow";
            }
        print '<div id="'.$div_name.
                 '" o2="e:'.$o2form_obj->id_esecuzione.
                      ",subf:".count($o2form_obj->linked).
                      ",cT:'form',f:'".$o2form_obj->nome.
                      "',alignH:'".$o2form_obj->align_o.
                      "',alignV:'".$o2form_obj->align_v.
                      "'".($halign_val ? ",x:".$halign_val : '').
                       ($valign_val ? ",y:".$valign_val : '').
                      ($o2form_obj->refresh_action ?
                       ",r:'".$o2form_obj->refresh_action."'" : '').
                       ",exit:".($o2form_obj->title_bar &&
                                 $o2form_obj->attiva    &&
                                !$o2form_obj->linkto    ? "true" : "false").
                      ($o2form_obj->maximized ? ',max:true' : '').
                 '" class="'.($nowin ? $nowin : "winshadow").
                 '" style="position:absolute;'.$align_buffer.
                 "\">\n<div class=\"".$o2form_obj->css_finestra.
                      '" style="width:'.$o2form_obj->larghezza."px;\">\n";

        // ===================== NOME E PROPRIETA' CLIENT DELLA FORM =====================
        if ($o2form_obj->attiva) {
            print '<input type="hidden" name="'.$o2form_obj->nome.
                                               $o2form_obj->id_esecuzione."_jxcmd\">\n";
            }
        // ======================= [###### BARRA DEL TITOLO ######] ======================
        if ($o2form_obj->title_bar && !$o2form_obj->linkto && !$nowin) {
            if ($app->multilang) {
                $title = $app->translate(substr($o2form_obj->nome, 0, -5),
                                         'title',
                                         0,
                                         $o2form_obj->titolo);
                }
            else {
                $title = $o2form_obj->titolo;
                }
            print '<table cellspacing="0" cellpadding="0" width="100%" class="'.
                  ($o2form_obj->attiva ?
                   $o2form_obj->css_titlebar :
                   $o2form_obj->css_titlebar_no).
                  "\" style=\"cursor:default;\"><tr>\n".
                  '<td title="Refresh" class="'.$o2form_obj->css_doc.'" '.
                           ($o2form_obj->attiva ?
                            ($o2form_obj->refresh_action ?
                             'onClick="o2jse.cmd.exe(\'\','.$o2form_obj->id_esecuzione.
                             ",'".$o2form_obj->refresh_action.'\');"' :
                             'onClick="o2jse.cmd.submit('.$o2form_obj->id_esecuzione.
                                                           ');"') :
/*
                             'onClick="o2jse.win.menu(this);"') :
*/
                            "").
                        ">&nbsp;</td>\n".
                  '<td class="'.($o2form_obj->attiva    ?
                                 $o2form_obj->css_title :
                                 $o2form_obj->css_title_no).
                    '" onmousedown="o2jse.win.startMoving(this,event);"><span id="'.
                    $div_name.'_title">'.jx_encode($title)."</span></td>\n".
                  ($o2form_obj->status_bar ?
                   '<td title="Maximize/restore" class="'.
                   ($o2form_obj->maximized ?
                    $o2form_obj->css_restore :
                    $o2form_obj->css_full).
                   '" onclick="return o2jse.win.maxRest(this);">&nbsp;</td>' : '').
                  '<td id="'.$div_name.
                  '_btn" title="Minimize/restore" class="'.$o2form_obj->css_close.
                  '" onclick="return o2jse.win.openClose(\''.
                                                         $div_name."','".
                                                         $o2form_obj->css_open."','".
                                                         $o2form_obj->css_close.
                  '\');">&nbsp;</td><td title="Quit" class="'.$o2form_obj->css_exit.
                  ($o2form_obj->attiva ? '" onClick="o2jse.win.exit(this);"' : '"').
                  ">&nbsp;</td></tr></table>\n";
            }
        // __________________________________________________ Visualizzazione <iframe> ___
        if ($o2form_obj->url) {
            if (isset($_REQUEST[$div_in_name])) {
                $url = $_REQUEST[$div_in_name];
                }
            else {
                eval("\$url = (".$o2form_obj->url.");");
                }
            print '<iframe id="'.$div_in_name.
                        '" src="'.$url.
                        '" class="'.($nowin ? $nowin : $o2form_obj->css_body).
                        '" style="position:relative;width:100%;height:'.
                        max(0, ($o2form_obj->altezza -
                            ($o2form_obj->title_bar && !$o2form_obj->linkto ? 30 : 0))).
                      "px;overflow:auto;\"></iframe>\n";
            }
        // __________________________________________________ Visualizzazione standard ___
        else {
            if ($o2form_obj->attiva || !$o2form_obj->html_cache) {
                $html_length_local = ob_get_length();
                if ($o2form_obj->attiva) {
                    $o2form_obj->leggi_valori_vista();
                    }
                // ==================== [   INIZIO <div> interno  ] ======================
                print '<div id="'.$div_in_name.'"'.$keyboard_script_local.
                          ' class="'.($nowin ? $nowin : $o2form_obj->css_body).
                         '" style="position:relative;height:'.
                         max(0, ($o2form_obj->altezza -
                              ($o2form_obj->title_bar && !$o2form_obj->linkto ? 30 : 0))).
                      "px;\">\n";

                // ============================ ** CONTROLLI ** ==========================
                foreach ($o2form_obj->controlli as &$single_ctrl) {
                    if (!$single_ctrl->padre) {
                        print '<div style="position:absolute;left:'.
                              $single_ctrl->x.'px;top:'.
                              $single_ctrl->y."px;\">\n";
                        $single_ctrl->disegna();
                        print "</div>\n";
                        }
                    }
                // =======================================================================
                print "</div>\n";
                // ___________________________________________________ Cursor position ___
                if (!$app->caret_position[$o2form_obj->id_esecuzione]) {
                    $set_to_field = $o2form_obj->controlli_posizione();
                    if ($set_to_field) {
                        $prg->set_caret($o2form_obj->html_name, $set_to_field);
                        }
                    }
                $o2form_obj->html_cache = (!$o2form_obj->attiva ?
                                           substr(ob_get_contents(), $html_length_local) :
                                           "");
                }
            else {
                print $o2form_obj->html_cache;
                }
            }
        // ========================== ['FORM'_status_bar][########][/] ===================
        if (!$o2form_obj->linkto && !$nowin) {
            $cs = ($o2form_obj->status_bar ? "" : "_dis");
            print '<table class="'.$o2form_obj->css_status.$cs.
                       '" cellspacing="0" cellpadding="0"><tr><td class="'.
                                   $o2form_obj->css_corner.$cs.
                  '">&nbsp;</td><td> &nbsp; </td><td class="'.$o2form_obj->css_resize.
                  $cs.'"'.($o2form_obj->status_bar ?
                      ' title="Resize" onMouseDown="o2jse.win.startSizing(this,event);"' :
                      '').
                  ">&nbsp;</td></tr></table>\n";
            }
        print "</div></div>\n".$repos;
        // __________________________ Manage refresh timeout and browser window resize ___
        if ($o2form_obj->attiva) {
            if ($o2form_obj->refresh_exp) {
                $time_local = false;
                eval("\$time_local = ".$o2form_obj->refresh_exp.";");
                if ($time_local) {
                    print '<script> o2jse.jxRefresh=setTimeout(\'o2jse.cmd.exe("",'.
                          $o2form_obj->id_esecuzione.',"'.
                          $o2form_obj->refresh_action.'");delete(o2jse.jxRefresh)\','.
                          ($time_local * 1000)."); </script>\n";
                    }
                }
            if ($o2form_obj->maximized) {
                print "<script>o2jse.maximizedWin=['".$o2form_obj->nome."','".
                                                      $o2form_obj->id_esecuzione.
                      "'];</script>\n";
                }
            }
        print "<!-- win end -->\n";

        }


    static function ctrl_win_position($o2form_obj) {

        $app    = $_SESSION['o2_app'];
        $menu_v = $app->menu_attivo && $app->menu_style == 'L';
        $menu_o = $app->menu_attivo && $app->menu_style == 'T';
        // ___________________________________________________________ NOT linked form ___
        if (!$o2form_obj->linkto) {
            // ______________________________________________________________ Position ___
            $cli_w = $app->client_width;
            $cli_h = $app->client_height;
            $bar_w = ($menu_v ? $app->menu_width : 0);
            $bar_h = ($menu_o ? $app->menu_height : 0);
            $stb_h = $app->status_height;
            // __________________________________________________ Horizontal alignment ___
            switch ($o2form_obj->align_o) {
                case "center":
                    $o2form_obj->absolute_x = max(0, intval($cli_w / 2 - $o2form_obj->x +
                                                            $bar_w / 2));
                    break;
                case "right":
                    $o2form_obj->absolute_x = -($o2form_obj->x - $o2form_obj->larghezza);
                    break;
                default:
                    $o2form_obj->absolute_x = $o2form_obj->x + $bar_w;
                    break;
                }
            // ____________________________________________________ Vertical alignment ___
            switch ($o2form_obj->align_v) {
                case "middle":
                    $o2form_obj->absolute_y = max(0, intval($cli_h / 2 - $o2form_obj->y));
                    break;
                case "bottom":
                    $o2form_obj->absolute_y = -($o2form_obj->y - $o2form_obj->altezza);
                    break;
                default:
                    $o2form_obj->absolute_y = $o2form_obj->y + $bar_h;
                    break;
                }
            }

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_label $o2ctrl_obj   Related o2_ctrl_label object
     */
    static function ctrl_label($o2ctrl_obj) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $label = $app->translate($o2ctrl_obj->nome,
                                     'label',
                                     0,
                                     $o2ctrl_obj->label);
            }
        else {
            $label = $o2ctrl_obj->label;
            }
        print '<div'.($o2ctrl_obj->tab_row ? "" :
                      ' id="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.'"').
                   ' class="'.$o2ctrl_obj->css.
                  '" style="width:'.$o2ctrl_obj->larghezza.
                        'px;height:'.$o2ctrl_obj->altezza."px;".
              ($o2ctrl_obj->visibile ? "" : "display:none;").'"'.
              ($o2ctrl_obj->popup_exp ?
              ' o2="e:'.$o2ctrl_obj->id_esecuzione.
                 ',puact:\''.$o2ctrl_obj->popup_act.
                "',puexp:'".$o2ctrl_obj->popup_exp.
              '\'" onMouseOver="o2jse.pu.i(event)" onMouseOut="o2jse.pu.o()"' : '').
              '>'.jx_encode($label)."</div>\n";

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_edit $o2ctrl_obj   Related o2_ctrl_edit object
     */
    static function ctrl_edit($o2ctrl_obj) {

        $active  = $o2ctrl_obj->is_active();
        $enabled = $o2ctrl_obj->is_enabled();
        $zoom    = $o2ctrl_obj->is_zoomable();
        $aligned = ($o2ctrl_obj->maschera->allineamento == 'Right' ?
                    'text-align:right;' : '');
        $name    = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        if ($active && $enabled) {
            $js_change = $o2ctrl_obj->js_props($zoom ? 'z:!0' : 'z:!1').
                         'onChange="o2jse.ctrl.c(this);" '.
                         ($zoom ? 'onDblClick="o2jse.ctrl.zoom(event);" ' : '').
                         'onFocus="o2jse.ctrl.f(this);" onBlur="o2jse.ctrl.b(this);"';
            print '<input type="'.($o2ctrl_obj->password ? "password" : "text").
                       '" name="'.$name.
                       '" value="'.$o2ctrl_obj->sql2ctrl().
                       '" maxlength="'.$o2ctrl_obj->maschera->dimensione.
                       '" class="'.trim($o2ctrl_obj->css).
                       ($zoom ? ' jxzoom' : '').
                       ($o2ctrl_obj->forza_submit ? " jxsoc" : "").
                       '" style="width:'.$o2ctrl_obj->larghezza.
                       'px;height:'.$o2ctrl_obj->altezza."px;".$aligned.
                       ($o2ctrl_obj->visibile ? "" : "display:none;").
                       ($o2ctrl_obj->popup_exp ?
                        '" onMouseOver="o2jse.pu.i(event);" onMouseOut="o2jse.pu.o();" ' :
                        '" ').$js_change.">\n";
            }
        else {
            $js_change = '';
            if ($active && $zoom) {
                $js_change = $o2ctrl_obj->js_props("z:!0").
                             'onDblClick="o2jse.ctrl.zoom(event);"';
                }
            else {
                $js_change = $o2ctrl_obj->js_props();
                }
            print '<div id="'.$name.
                     '" class="'.trim($o2ctrl_obj->css).
                                 ($enabled ? "" : "_dis").($zoom ? ' jxzoom' : '').
                     '" style="width:'.$o2ctrl_obj->larghezza.
                           'px;height:'.$o2ctrl_obj->altezza.'px;'.$aligned.
                        ($o2ctrl_obj->visibile ? "" : "display:none;").
                        ($active && $o2ctrl_obj->popup_exp ?
                        '" onMouseOver="o2jse.pu.i(event);" onMouseOut="o2jse.pu.o();" ' :
                        '" ').$js_change.'>'.$o2ctrl_obj->sql2ctrl()."</div>\n";
            }


        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_text $o2ctrl_obj   Related o2_ctrl_text object
     */
    static function ctrl_text($o2ctrl_obj) {

        $active    = $o2ctrl_obj->is_active();
        $enabled   = $o2ctrl_obj->is_enabled();
        $zoom      = $o2ctrl_obj->is_zoomable();
        $area_name = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        // ____________________________________________________________ Active control ___
        if ($active && $enabled) {
            // _________________________________________________________ WYSIWYG editor___
            if ($o2ctrl_obj->html) {
                // ___________________________________________________ Command buttons ___
                $btnlst = array(array("bold",
                                      "text_bold",
                                      "Bold"),
                                array("italic",
                                      "text_italic",
                                      "Italic"),
                                array("underline",
                                      "text_underlined",
                                      "Underlined"),
                                array(""),
                                array("insertOrderedList",
                                      "text_ol",
                                      "Ordered list"),
                                array("insertUnorderedList",
                                      "text_ul",
                                      "Unordered list"),
                                array("indent",
                                      "text_indent",
                                      "Indent paragraph"),
                                array("outdent",
                                      "text_outdent",
                                      "Unindent paragraph"),
                                array(""),
                                array("justifyLeft",
                                      "text_left",
                                      "Align Left"),
                                array("justifyFull",
                                      "text_justify",
                                      "Justify"),
                                array("justifyCenter",
                                      "text_center",
                                      "Align Center"),
                                array("justifyRight",
                                      "text_right",
                                      "Align Right"));
                // ______________________________________________________ Buttons code ___
                $rnt_alias = $_SESSION['o2_app']->runtime->alias;
                $ico_path  = $rnt_alias."img/editor/";
                $ico_style = ' style="display:inline;cursor:pointer;"';
                $js_cmds   = ' onClick="o2jse.rte.btnExe(\''.$area_name.'\',\'';
                $js_on     = ' onmouseover="this.src=\''.$ico_path;
                $js_out    = ' onmouseout="this.src=\''.$ico_path;
                $btncode   = "";
                foreach ($btnlst as $btn) {
                    if ($btn[0]) {
                        $btncode.= '<img src="'.$ico_path.$btn[1].'.png" title="'.$btn[2].
                                   '"'.$ico_style.$js_cmds.$btn[0].'\');"'.
                                   $js_on.$btn[1].'_on.png\';"'.$js_out.$btn[1].
                                   '.png\';" />';
                        }
                    else {
                        $btncode.= '<img src="'.$ico_path.'separator.png"'.$ico_style.
                                   ' />';
                        }
                    }
                // ________________________________________________ Hidden input field ___
                $hiddenFld  = '<input type="hidden" id="'.$area_name.
                                               '" name="'.$area_name.
                                               '" value="'.$o2ctrl_obj->sql2ctrl().
                                               '">';
                // _________________________________________________________ Edit area ___
                $js_change = $o2ctrl_obj->js_props("cssc:'".$o2ctrl_obj->css.
                              "_edit',rte:true,".($zoom ? "z:!0" : "z:!1")).
                             ($zoom ? 'onDblClick="o2jse.ctrl.zoom(event);" ' : '').
                             'onFocus="o2jse.ctrl.f(this);" '.
                             'onBlur="o2jse.rte.b(this);" '.
                             ($o2ctrl_obj->popup_exp ?
                          ' onMouseOver="o2jse.pu.i(event);" onMouseOut="o2jse.pu.o();"' :
                              '');
                $edit_area  = '<div contenteditable="true" id="'.$area_name.
                              '_edit" style="overflow:auto;width:100%;height:'.
                              ($o2ctrl_obj->altezza - 25).'px;" class="'.$o2ctrl_obj->css.
                              "_edit".($zoom ? ' jxzoom' : '').
                                      ($o2ctrl_obj->forza_submit ? ' jxsoc' : '')."\" ".
                              $js_change.">".
                              $o2ctrl_obj->valore."</div>";
                // _______________________________________________________ Compose all ___
                print '<div style="width:'.$o2ctrl_obj->larghezza.'px;'.
                                  ($o2ctrl_obj->visibile ? "" : "display:none;").
                      '" class="'.$o2ctrl_obj->css.'_cont"><div class="'.
                      $o2ctrl_obj->css.'_bar">'.$btncode."</div>".
                      $edit_area.$hiddenFld."</div>";
                }
            // _________________________________________ Normal text-area (no WYSIWYG) ___
            else {
                $js_change = $o2ctrl_obj->js_props($active && $zoom ? "z:!0" : "z:!1")
                            .'onChange="o2jse.ctrl.c(this);" '.
                             ($active && $zoom ?
                              'onDblClick="o2jse.ctrl.zoom(event);" ' : '').
                             'onFocus="o2jse.ctrl.f(this);" '.
                             'onBlur="o2jse.ctrl.b(this);" '.
                             'onKeyPress="return o2jse.ctrl.l(event);" ';
                print '<textarea name="'.$area_name.
                             '" id="'.$area_name.
                             '" class="'.trim($o2ctrl_obj->css).
                                         ($zoom ? ' jxzoom' : '').
                                         ($o2ctrl_obj->forza_submit ? ' jxsoc' : '').
                             '" style="width:'.$o2ctrl_obj->larghezza.
                                  'px;height:'.$o2ctrl_obj->altezza.'px;'.
                             ($o2ctrl_obj->visibile ? "" : "display:none;")."\"\n".
                             $js_change.
                             ($o2ctrl_obj->popup_exp ?
                          ' onMouseOver="o2jse.pu.i(event);" onMouseOut="o2jse.pu.o();"' :
                              '')."\n>".$o2ctrl_obj->sql2ctrl()."</textarea>\n";
                }
            }
        // _________________________________________________________NOT active control ___
        else {
            $js_change = "";
            if ($zoom) {
                $js_change = $o2ctrl_obj->js_props("z:!0").
                             'onDblClick="o2jse.ctrl.zoom(event);"';
                }
            print '<div id="'.$area_name.
                     '" class="'.trim($o2ctrl_obj->css).($enabled ? "" : "_dis").
                     '" style="width:'.$o2ctrl_obj->larghezza.
                          'px;height:'.$o2ctrl_obj->altezza."px;".
                          ($o2ctrl_obj->visibile ? "" : "display:none;").'" '.
                     $js_change.
                     ($active && $o2ctrl_obj->popup_exp ?
                      ' onMouseOver="o2jse.pu.i(event);" onMouseOut="o2jse.pu.o();"' :
                      '')."\n>".nl2br($o2ctrl_obj->sql2ctrl())."</div>\n";
            }

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_check $o2ctrl_obj   Related o2_ctrl_check object
     */
    static function ctrl_check($o2ctrl_obj) {

        if ($o2ctrl_obj->is_enabled()) {
            if ($o2ctrl_obj->padre) {
                $app     = $_SESSION['o2_app'];
                $prg     = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
                $parent  = $prg->form[$o2ctrl_obj->form]->controlli[$o2ctrl_obj->padre];
                $enabled = $parent->modificabile;
                }
            else {
                $enabled = true;
                }
            }
        else {
            $enabled = false;
            }
        $active = $o2ctrl_obj->is_active() &&
                  ($enabled && (!$o2ctrl_obj->tab_row || $o2ctrl_obj->forza_submit) ||
                   $o2ctrl_obj->selector);
        $submit = ($o2ctrl_obj->forza_submit || $o2ctrl_obj->selector ?
                   ($o2ctrl_obj->tab_row ? '!1' : '!0') :
                   '!1');
        print '<input type="checkbox" class="'.$o2ctrl_obj->css.'"'.
              (!$o2ctrl_obj->tab_row ?
               ' name="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.'" ' : "").
              ($o2ctrl_obj->sql2ctrl() ? " CHECKED " : "").
              $o2ctrl_obj->js_props("vS:!0").
              ($o2ctrl_obj->visibile ? '' : 'style="display:none;" ').
              ($active ? 'onClick="o2jse.cb.c(this,'.$submit.');" ' : 'DISABLED').
              ($o2ctrl_obj->is_active() && $o2ctrl_obj->popup_exp ?
               ' onMouseOver="o2jse.pu.i(event)" onMouseOut="o2jse.pu.o()"' : '').
              ">\n";

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_listcombo $o2ctrl_obj   Related o2_ctrl_listcombo object
     */
    static function ctrl_listcombo($o2ctrl_obj) {

        if ($o2ctrl_obj->righe == 1) {
            return o2html::ctrl_lookup($o2ctrl_obj);
            }
        else {
            return o2html::ctrl_list($o2ctrl_obj);
            }

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_listcombo $o2ctrl_obj   Related o2_ctrl_listcombo object
     */
    static function ctrl_list($o2ctrl_obj) {

        $app            = $_SESSION['o2_app'];
        $mask           = $o2ctrl_obj->maschera;
        $task_obj       = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->
                           contesto[$o2ctrl_obj->task];
        $form_local     = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->
                           form[$o2ctrl_obj->form];
        $enabled_in_tab = $o2ctrl_obj->active_by_tab;
        $active         = $o2ctrl_obj->is_active();
        $enabled        = $o2ctrl_obj->is_enabled();
        $v_local        = trim($o2ctrl_obj->ctrl2sql());
        $zoom           = $o2ctrl_obj->is_zoomable();
        // __________________________________________________ Values list from picture ___
        if (!count($o2ctrl_obj->valori) && is_array($mask->valori)) {
            $o2ctrl_obj->valori = $mask->valori;
            }
        $jsl = "";
        $jsi = "";
        if ($active && $enabled) {
            $jsi = 'tabindex="0" onClick="o2jse.lb.m(event);" '.
                   'onkeydown="o2jse.lb.k(event);" ';
            $jsl = ($zoom ? 'onDblClick="o2jse.ctrl.zoom(event);" ' : '');
            }
        elseif ($zoom && $active) {
            $jsl = 'onDblClick="o2jse.ctrl.zoom(event);" ';
            }
        if ($enabled_in_tab) {
            $cod_default = false;
            print '<div id="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.
                  '" class="'.$o2ctrl_obj->css.($enabled ? "" : "_dis").
                                               ($zoom && $active ? ' jxzoom' : '').
                                               ($o2ctrl_obj->forza_submit &&
                                                $active && $enabled ? ' jxsoc' :'').
                  '" style="width:'.
                  $o2ctrl_obj->larghezza.'px;height:'.$o2ctrl_obj->altezza."px;".
                  ($o2ctrl_obj->visibile ? "" : "display:none;")."\"\n".
                  $o2ctrl_obj->js_props($zoom ? "z:!0" : "z:!1").$jsi.$jsl.">\n";
            // _____________________________ Draw items list and verify code existence ___
            $o2ctrl_obj->error = true;
            $list              = '';
            foreach ($o2ctrl_obj->valori as $codice => $descrizione) {
                if ($app->multilang) {
                    $descrizione = $app->translate($o2ctrl_obj->nome,
                                                   'item',
                                                   $codice,
                                                   $descrizione);
                    }
                $cod_default = ($cod_default !== false ? $cod_default : $codice);
                $list.= '<div id="jxdata:'.$codice.'" ';
                if ($v_local === trim($codice)) {
                    $list             .= "class='jxlsr'";
                    $o2ctrl_obj->error = false;
                    }
                if (!$descrizione) {
                    $descrizione = "&nbsp;";
                    }
                else  {
                    $descrizione = htmlentities($descrizione, ENT_QUOTES,
                                                              $app->chr_encoding);
                    }
                $list.= ">".$descrizione."</div>\n";
                }
            if ($o2ctrl_obj->error && $v_local) {
                $list = '<div id="jxdata:'.$v_local.'" class="jxlsr jxerror">['.
                        $v_local.'] Value not in list</div>'.$list;
                }
            print $list.'</div><input type="hidden" name="'.
                  $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.
                  "\" value='".$cod_default."'>\n";
            }
        else {
            $valore_local = "";
            foreach ($o2ctrl_obj->valori as $codice => $descrizione) {
                if ($app->multilang) {
                    $descrizione = $app->translate($o2ctrl_obj->nome,
                                                   'item',
                                                   $codice,
                                                   $descrizione);
                    }
                $valore_local.= htmlentities($descrizione,
                                             ENT_QUOTES,
                                             $app->chr_encoding).'<br />';
                }
            if (!$valore_local) {
                $valore_local = "&nbsp;";
                }
            print '<div style="width:'.$o2ctrl_obj->larghezza.
                          'px;height:'.$o2ctrl_obj->altezza.'px;'.
                  ($o2ctrl_obj->visibile ? "" : "display:none;").'" '.
                  $o2ctrl_obj->js_props().">".$valore_local."</div>\n";
            }

        }


    /**
     * Draw listcombo control as a lookup control
     *
     * @param o2_ctrl_listcombo $o2ctrl_obj   Related o2_ctrl_listcombo object
     */
    static function ctrl_lookup($o2ctrl_obj) {

        $app             = $_SESSION['o2_app'];
        $prg             = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $enabled_in_tab  = $o2ctrl_obj->active_by_tab;
        $enabled         = $o2ctrl_obj->is_enabled();
        $active          = $o2ctrl_obj->is_active();
        $zoom            = $o2ctrl_obj->is_zoomable();
        $static_list     = "";
        $code_value      = $o2ctrl_obj->ctrl2sql();
        $desc_value      = ($o2ctrl_obj->visibile ? $o2ctrl_obj->get_desc() : "");
        if ($o2ctrl_obj->items_view) {
            $list_type = "true";
            $rows      = $prg->contesto[$o2ctrl_obj->items_view]->righe_vis;
            }
        else {
            $list_type = "false";
            // ____________________________________ Enabled and active (loop on items) ___
            if ($enabled_in_tab && $active) {
                $esiste_valore = false;
                $cod_default   = false;
                $desc_default  = false;
                $static_list   = array();
                foreach ($o2ctrl_obj->valori as $codice => $descrizione) {
                    if ($app->multilang) {
                        $descrizione = $app->translate($o2ctrl_obj->nome,
                                                       'item',
                                                       $codice,
                                                       $descrizione);
                        }
                    if ($cod_default === false) {
                        $cod_default  = $codice;
                        $desc_default = $descrizione;
                        }
                    $static_list[] = "['".htmlspecialchars(addcslashes($codice, "'"),
                                                           ENT_QUOTES,
                                                           $app->chr_encoding)."','".
                                          htmlspecialchars(addcslashes($descrizione, "'"),
                                                           ENT_QUOTES,
                                                           $app->chr_encoding)."']";
                    if ((string) $code_value === trim($codice)) {
                        $esiste_valore = true;
                        }
                    }
                $static_list = ",items:[".implode(",", $static_list)."]";
                // _________________________ If not valued, set default value in field ___
                if (!$esiste_valore &&
                    !$code_value &&
                    $o2ctrl_obj->visibile &&
                    $enabled) {
                    $prg->contesto[$o2ctrl_obj->task]->imposta($o2ctrl_obj->campo_in_ctrl,
                                                               $cod_default);
                    $desc_value = $desc_default;
                    }
                }
            $rows = count($o2ctrl_obj->valori);
            }
        $js_prop  = $o2ctrl_obj->js_props("dyn:".$list_type.$static_list.
                                          ",cssf:'".$o2ctrl_obj->css.
                                          "_field',cssl:'".$o2ctrl_obj->css.
                                          "_list',rows:".($rows ? $rows : "0").
                                          ($zoom ? ",z:!0" : ",z:!1"));
        $htmlname = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        if ($enabled && $enabled_in_tab && $active) {
            // ___________________________ Handler icon to open or click on full field ___
            $handler = $app->option_get('lookup_handler');
            $js_change = 'onKeyDown="o2jse.lu.k(event, this);" '.
                         'onClick="o2jse.lu.ck(this,'.($handler ? '!0' : '!1').');" '.
                         'onPaste="o2jse.lu.p(this);" '.
                         ($zoom ? 'onDblClick="o2jse.ctrl.zoom(event);" ' : '').
                         'onFocus="o2jse.lu.f(this);" onBlur="o2jse.lu.b(this);"';
            print '<div style="position:relative;width:'.$o2ctrl_obj->larghezza.'px;" '.
                  $js_prop.'><input type="text" id="'.$htmlname.
                  '_desc" value="'.str_replace('"', "&quot;", $desc_value).
                       '" class="'.$o2ctrl_obj->css."_field".($zoom ? ' jxzoom' : '').
                                              ($o2ctrl_obj->forza_submit ? ' jxsoc' : '').
                                                   ($o2ctrl_obj->error ? ' jxerror' : '').
                       '" style="width:'.$o2ctrl_obj->larghezza.'px;height:'.
                                         $o2ctrl_obj->altezza.
                       "px;cursor:".($handler ? 'auto' : 'pointer').
                       ($o2ctrl_obj->visibile ? "" : ";display:none")."\"\n".
                  $js_prop.$js_change.
                  ($o2ctrl_obj->popup_exp ?
                   ' onMouseOver="o2jse.pu.i(event)" onMouseOut="o2jse.pu.o()"' : '').
                  ">\n";
            // _________________________________________ Add DIV for open icon handler ___
            if ($handler && $o2ctrl_obj->visibile) {
                $effect = $app->option_get('lookup_effect');
                print '<div class="'.$o2ctrl_obj->css.'_handler" '.
                      'style="width:'.$o2ctrl_obj->altezza.
                      'px;height:'.$o2ctrl_obj->altezza.'px;" '.
                      'onClick="o2jse.lu.ick(this);"'.
                      ($effect ? ' onMouseOver="o2jse.lu.onI(this);"' : '').
                      '>&nbsp;</div>';
                }
            print '<input type="hidden" name="'.$htmlname.'" value="'.$code_value.'" '.
                  $js_prop."></div>\n";
            }
        else {
            if (!$desc_value) {
                $desc_value = "&nbsp;";
                }
            if ($active && $zoom) {
                $js_change = ' onDblClick="o2jse.ctrl.zoom(event);"';
                }
            if ($active && $enabled_in_tab) {
                print '<div style="position:relative;width:'.$o2ctrl_obj->larghezza.'px;" '.
                      $js_prop.'><div class="'.$o2ctrl_obj->css.($enabled ? "_field" :"_field_dis").
                         '" id="'.$htmlname.'_desc" style="width:'.$o2ctrl_obj->larghezza.
                                                  'px;height:'.$o2ctrl_obj->altezza.'px;'.
                                           ($o2ctrl_obj->visibile ? "" : "display:none;").
                      '" '.$js_prop.$js_change.
                      ($o2ctrl_obj->popup_exp ?
                       ' onMouseOver="o2jse.pu.i(event)" onMouseOut="o2jse.pu.o()"' : '').
                      '>'.$desc_value."</div>\n".
                   '<input type="hidden" name="'.$htmlname.
                        '" value="'.$code_value.'" '.$js_prop.'></div>';
                }
            else {
                print '<div '.
                      ($enabled_in_tab ?
                       'class="'.$o2ctrl_obj->css.($enabled ? "_field" :"_field_dis").'"'
                       : '').
                      ' style="width:'.$o2ctrl_obj->larghezza.
                           'px;height:'.$o2ctrl_obj->altezza.'px;'.
                      ($o2ctrl_obj->visibile ? "" : "display:none;").'" '.$js_prop.'>'.
                      $desc_value."</div>\n";
                }
            }

        }


    /**
     * Receives AJAX style requests for lookup control
     *
     */
    static function ctrl_lookup_req() {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[(isset($_REQUEST['o2_prgexeid']) ?
                                  $_REQUEST['o2_prgexeid'] :
                                  $app->progressivo_istanze)];
        ob_end_clean();
        header("Content-type: text/javascript; charset=".$app->chr_encoding);
        if ($prg) {
            $form = $prg->form[$_REQUEST['o2lastform']];
            if ($form) {
                $ctrl = $form->controlli[$_REQUEST['o2lastctrl']];
                if ($ctrl) {
                    $filter = $_REQUEST[$ctrl->nome.$ctrl->id_esecuzione."_desc"];
                    switch ($_REQUEST['jxluact']) {
                        case "lulist": // __________________________ Lookup items list ___
                            print $ctrl->xlist(false, $filter);
                            break;
                        case "luprevpg": // ________ Lookup items list - previous page ___
                            print $ctrl->xlist("P", $filter);
                            break;
                        case "lunextpg": // ____________ Lookup items list - next page ___
                            print $ctrl->xlist("N", $filter);
                            break;
                        }
                    }
                }
            }
        // _____________________________________________________________ SQL Satistics ___
        if ($app->sqltrace) {
            $app->sql_stat();
            }
        if ($app->js_cache) {
            print ';'.$app->js_cache;
            $app->js_cache = '';
            }
        die();

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_button $o2ctrl_obj   Related o2_ctrl_button object
     */
    static function ctrl_button($o2ctrl_obj) {

        $app      = $_SESSION['o2_app'];
        $active   = $o2ctrl_obj->is_active();
        $enabled  = $o2ctrl_obj->modificabile;
        $title    = "";
        $shortcut = "";
        if ($app->multilang) {
            $label = $app->translate($o2ctrl_obj->nome,
                                     'label',
                                     0,
                                     $o2ctrl_obj->label);
            }
        else {
            $label = $o2ctrl_obj->label;
            }
        if ($active && $enabled) {
            $o2ctrl_obj->forza_submit = !$o2ctrl_obj->tab_row;
            $js_change = 'onClick="o2jse.ctrl.btnExe(this)" '.
                         'onFocus="o2jse.ctrl.f(this)" onBlur="o2jse.ctrl.b(this)" '.
                         ($o2ctrl_obj->popup_exp ?
                          'onMouseOver="o2jse.pu.i(event)" onMouseOut="o2jse.pu.o()"' :
                          "");
            $js_props  = $o2ctrl_obj->js_props("action:'".$o2ctrl_obj->azione_submit."'");
            // _____________________________ Keyboard short-cuts are defined only once ___
            if ($o2ctrl_obj->tab_row) {
                $label = str_replace("&", "", $label);
                }
            else {
                // _________________________________ Bind function key event to button ___
                if ($o2ctrl_obj->has_key_event()) {
                    $key_code = $o2ctrl_obj->get_key_code();
                    $title = ' title="F'.$key_code.'"';
                    $shortcut = "<script> o2jse.conf.addKey(".
                                (111 + $key_code).",!1,!1,!1,\"".
                                $o2ctrl_obj->nome."\"); </script>\n";
                    }
                // _________________________________ Bind ALT+[letter] event to button ___
                elseif (($key_pos = strpos($label, "&")) !== false) {
                    $key_letter = trim(substr($label, $key_pos + 1, 1));
                    if ($key_letter) {
                        $label    = substr($label, 0, $key_pos)."<u>".$key_letter."</u>".
                                    substr($label, $key_pos + 2);
                        $title    = ' title="ALT+'.$key_letter.'"';
                        $key_code = ord(strtolower($key_letter)) - 32;
                        $shortcut = "<script> o2jse.conf.addKey(".
                                    $key_code.",!1,!1,!0,\"".
                                    $o2ctrl_obj->nome."\"); </script>\n";
                        }
                    }
                }
            }
        else {
            if ($active && $o2ctrl_obj->popup_exp) {
                $js_change = 'onMouseOver="o2jse.pu.i(event)" onMouseOut="o2jse.pu.o()"';
                }
            else {
                $js_change = "";
                }
            $js_props = $o2ctrl_obj->js_props();
            $label    = str_replace("&", "", $label);
            }
        list($css_before, $css_after) = explode(" ", $o2ctrl_obj->css, 2);
        print '<div '.($o2ctrl_obj->tab_row ? '' :
                       'id="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.'" ').
              'class="'.$css_before.($o2ctrl_obj->modificabile ? '' : '_dis').
                       ($css_after ? " ".$css_after : '').'"'.
              $title.
              ' style="width:'.$o2ctrl_obj->larghezza.'px;height:'.$o2ctrl_obj->altezza.
                      "px;line-height:".$o2ctrl_obj->altezza."px;".
                      ($o2ctrl_obj->visibile ? "" : "display:none;").'" '.
              ($enabled ? 'tabindex="0" ' : "").$js_props.$js_change.'>'.
              jx_encode($label)."</div>\n".$shortcut;

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_img $o2ctrl_obj   Related o2_ctrl_img object
     */
    static function ctrl_image($o2ctrl_obj) {

        $app        = $_SESSION['o2_app'];
        $prg        = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form       = $prg->form[$o2ctrl_obj->form];
        $html_name  = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        $map_buffer = "<map name='".$html_name."_o2map'>\n";
        $with_map   = false;
        if ($o2ctrl_obj->is_active() && $o2ctrl_obj->is_enabled()) {
            $o2ctrl_obj->forza_submit = !$o2ctrl_obj->tab_row;
            foreach ($form->controlli as $single_ctrl) {
                // _____________ Se trovo un controllo map che appartiene all'immagine ___
                if ($single_ctrl->padre == $o2ctrl_obj->nome &&
                    $single_ctrl->visibile                   &&
                    $single_ctrl->modificabile) {
                    $map_buffer.= "<area ".$o2ctrl_obj->js_props("action:'".
                                                              $single_ctrl->azione_submit.
                                                                 "'").
                                  "shape='rect' coords='".$single_ctrl->x.",".
                                                          $single_ctrl->y.",".
                                          ($single_ctrl->x + $single_ctrl->larghezza).",".
                                          ($single_ctrl->y + $single_ctrl->altezza).
                                  "'\nonClick='o2jse.ctrl.btnExe(this);' ".
                                  "style='cursor:pointer;' target='_self'>\n";
                    $with_map = true;
                    }
                }
            $map_buffer.= "</map>\n";
            if (!$with_map && $o2ctrl_obj->azione_submit) {
                $js_change_local  = $o2ctrl_obj->js_props("action:'".
                                                          $o2ctrl_obj->azione_submit."'").
                                    "onClick='o2jse.ctrl.btnExe(this);'\n";
                $css_cursor_local = " cursor:pointer;";
                }
            else {
                $js_change_local  = "";
                $css_cursor_local = "";
                }
            }
        else {
            $js_change_local  = "";
            $css_cursor_local = "";
            }
        $src      = $o2ctrl_obj->src();
        $src_over = $o2ctrl_obj->srcover();
        if ($o2ctrl_obj->imgoverexp) {
            print "<script>(new Image()).src='".$src_over."';</script>\n";
            }
        if ($app->multilang) {
            $label = $app->translate($o2ctrl_obj->nome,
                                     'label',
                                     0,
                                     $o2ctrl_obj->label);
            }
        else {
            $label = $o2ctrl_obj->label;
            }
        print "<img name='".$html_name."' ".
                    $js_change_local.
                   "alt='".$label.
                 "' src='".$src.
                 "' class='".$o2ctrl_obj->css.
                 "' title='".jx_encode($label).
                 "' style='width:".$o2ctrl_obj->larghezza.
                      "px; height:".$o2ctrl_obj->altezza."px;".
                           $css_cursor_local.
                           ($o2ctrl_obj->visibile ? "" : "display:none;")."'";
        if ($o2ctrl_obj->imgoverexp && $o2ctrl_obj->modificabile) {
            print "onmouseover='this.src=\"".$src_over.
                  "\";'\nonmouseout='this.src=\"".$src."\";' ";
            }
        if ($with_map) {
            print " usemap='#".$html_name."_o2map'>\n".$map_buffer;
            }
        else {
            print ">\n";
            }

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_flowbox $o2ctrl_obj   Related o2_ctrl_flowbox object
     */
    static function ctrl_flowbox($o2ctrl_obj) {

        $html_name          = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        $o2ctrl_obj->pagine = count($o2ctrl_obj->label);
        print '<div id="'.$html_name.'" class="'.$o2ctrl_obj->css.'" style="width:'.
              $o2ctrl_obj->larghezza.'px;height:'.$o2ctrl_obj->altezza.'px;'.
              ($o2ctrl_obj->visibile ? "" : "display:none;")."\">\n";
        $prg   = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form  = $prg->form[$o2ctrl_obj->form];
        $ctrls = array();
        foreach ($form->controlli as $singolo_ctrl) {
            // ___________________________________________________ Controls in flowbox ___
            if ($singolo_ctrl->padre == $o2ctrl_obj->nome) {
                $ctrls[$singolo_ctrl->info_padre[0]] = $singolo_ctrl;
                }
            }
        ksort($ctrls);
        foreach ($ctrls as $ctrl) {
            $ctrl->disegna();
            }
        unset($ctrls);
        print "</div>\n";

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_frame $o2ctrl_obj   Related o2_ctrl_frame object
     */
    static function ctrl_frame($o2ctrl_obj) {

        $html_name = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        $prg       = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form      = $prg->form[$o2ctrl_obj->form];
        print '<div id="'.$html_name.'" class="'.$o2ctrl_obj->css.'" style="'.
              ($o2ctrl_obj->visibile ? "" : "display:none;")."\">\n";
        print '<table class="o2_ctrl_box_title"><tr><th>'.
              $form->controlli[$o2ctrl_obj->padre]->label[$o2ctrl_obj->info_padre[0]].
              '</th></tr></table><div style="position:relative;width:'.
              $o2ctrl_obj->larghezza.'px;height:'.$o2ctrl_obj->altezza.'px;">';
        foreach ($form->controlli as &$singolo_ctrl) {
            // ___________________________________________________ Controls in flowbox ___
            if ($singolo_ctrl->padre == $o2ctrl_obj->nome) {
                print '<div style="position:absolute;left:'.$singolo_ctrl->x.
                      'px;top:'.$singolo_ctrl->y.
                      "px;padding:0;margin:0;white-space:nowrap;\">\n";
                if (!$o2ctrl_obj->modificabile) {
                    switch ($singolo_ctrl->tipo_ctrl) {
                        case "tab":
                            $singolo_ctrl->modificabile  = false;
                            $singolo_ctrl->dis_by_parent = true;
                            foreach ($form->controlli as &$tmp_ctrl) {
                                if ($tmp_ctrl->tipo_ctrl == "navigator" &&
                                    $tmp_ctrl->padre == $singolo_ctrl->nome) {
                                    $tmp_ctrl->enable_del(false);
                                    $tmp_ctrl->enable_detail(false);
                                    $tmp_ctrl->enable_new(false);
                                    $tmp_ctrl->enable_save(false);
                                    $tmp_ctrl->enable_select(false);
                                    $tmp_ctrl->enable_undo(false);
                                    break;
                                    }
                                }
                            break;
                        case "navigator":
                            $singolo_ctrl->enable_del(false);
                            $singolo_ctrl->enable_detail(false);
                            $singolo_ctrl->enable_new(false);
                            $singolo_ctrl->enable_save(false);
                            $singolo_ctrl->enable_select(false);
                            $singolo_ctrl->enable_undo(false);
                            break;
                        default:
                            $singolo_ctrl->modificabile = false;
                            break;
                        }
                    }
                elseif ($singolo_ctrl->tipo_ctrl == "tab") {
                    $singolo_ctrl->dis_by_parent = false;
                    }
                $singolo_ctrl->disegna();
                print "</div>\n";
                }
            }
        print "</div></div>\n";

        }


    static function ctrl_multipage($o2ctrl_obj) {

        $html_name          = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        $o2ctrl_obj->pagine = count($o2ctrl_obj->label);
        print '<div id="'.$html_name.'_out" class="'.$o2ctrl_obj->css_spazio_out.
                 '" style="width:'.$o2ctrl_obj->larghezza.
                       'px;height:'.$o2ctrl_obj->altezza.
                       'px;overflow:hidden;'.
                       ($o2ctrl_obj->visibile ? "" : "display:none;")."\">\n";
        if ($o2ctrl_obj->pagine > 1) {
            print '<input type="hidden" name="'.$html_name.'" '.
                  $o2ctrl_obj->js_props("csson:'".$o2ctrl_obj->css_label_on.
                                      "',cssoff:'".$o2ctrl_obj->css_label_off."'").
                  ' value="'.$o2ctrl_obj->valore."\">\n";
            }
        // ___________________________________________________________ Top buttons bar ___
        if ($o2ctrl_obj->posizione_ctrls == "T") {
            o2html::ctrl_multipage_h($o2ctrl_obj);
            }
        // _____________________________________________________ Left side buttons bar ___
        if ($o2ctrl_obj->posizione_ctrls == "L") {
            o2html::ctrl_multipage_v($o2ctrl_obj);
            }
        $prg_local = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form_obj  = $prg_local->form[$o2ctrl_obj->form];
        foreach ($o2ctrl_obj->label as $label_index => $single_label) {
            $active_page = ($o2ctrl_obj->valore == $label_index);
            print '<div id="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.
                  '_p'.$label_index.'" class="'.$o2ctrl_obj->css_spazio_in.'"'.
                  ($active_page ? '' : ' style="display:none;"').">\n";
            if ($active_page) {
                $o2ctrl_obj->sent_pages[$label_index] = true;
                foreach ($form_obj->controlli as &$singolo_ctrl) {
                    // _____________________________________ Controls in multipage ___
                    if ($singolo_ctrl->padre == $o2ctrl_obj->nome &&
                        $singolo_ctrl->info_padre[0] == $label_index) {
                        if (!$o2ctrl_obj->modificabile) {
                            switch ($singolo_ctrl->tipo_ctrl) {
                                case "tab":
                                    $singolo_ctrl->modificabile  = false;
                                    $singolo_ctrl->dis_by_parent = true;
                                    foreach ($form_obj->controlli as &$tmp_ctrl) {
                                        if ($tmp_ctrl->tipo_ctrl == "navigator" &&
                                            $tmp_ctrl->padre == $singolo_ctrl->nome) {
                                            $tmp_ctrl->enable_del(false);
                                            $tmp_ctrl->enable_detail(false);
                                            $tmp_ctrl->enable_new(false);
                                            $tmp_ctrl->enable_save(false);
                                            $tmp_ctrl->enable_select(false);
                                            $tmp_ctrl->enable_undo(false);
                                            break;
                                            }
                                        }
                                    break;
                                case "navigator":
                                    $singolo_ctrl->enable_del(false);
                                    $singolo_ctrl->enable_detail(false);
                                    $singolo_ctrl->enable_new(false);
                                    $singolo_ctrl->enable_save(false);
                                    $singolo_ctrl->enable_select(false);
                                    $singolo_ctrl->enable_undo(false);
                                    break;
                                default:
                                    $singolo_ctrl->modificabile = false;
                                    break;
                                }
                            }
                        elseif ($singolo_ctrl->tipo_ctrl == "tab") {
                            $singolo_ctrl->dis_by_parent = false;
                            }
                        print '<div style="position:absolute;left:'.$singolo_ctrl->x.
                              'px;top:'.$singolo_ctrl->y.
                              "px;padding:0;margin:0;white-space:nowrap;\">\n";
                        $singolo_ctrl->disegna();
                        print "</div>\n";
                        }
                    }
                }
            print "</div>\n";
            }
        // ____________________________________________________ Right side buttons bar ___
        if ($o2ctrl_obj->posizione_ctrls == "R") {
            o2html::ctrl_multipage_v($o2ctrl_obj);
            }
        // ________________________________________________________ Bottom buttons bar ___
        if ($o2ctrl_obj->posizione_ctrls == "B") {
            o2html::ctrl_multipage_h($o2ctrl_obj);
            }
        print "</div>\n";

        }


    /**
     * Draws horizontal buttons bar for multipage control
     *
     * @param o2_ctrl_multipage $o2ctrl_obj   Related o2_ctrl_multipage object
     */
    static function ctrl_multipage_h($o2ctrl_obj) {

        $app = $_SESSION['o2_app'];
        if ($o2ctrl_obj->pagine > 1) {
            $active = $o2ctrl_obj->is_active();
            foreach ($o2ctrl_obj->label as $page_index => $title) {
                $actual = $o2ctrl_obj->valore == $page_index;
                if ($active) {
                    $js_script_local = $o2ctrl_obj->js_props("page:".$page_index).
                                       ($actual ? "" :
                                     'onClick="o2jse.ctrl.multiPage(this)" tabindex="0"');
                    }
                if ($app->multilang) {
                    $title = $app->translate($o2ctrl_obj->nome,
                                             'page',
                                             $page_index,
                                             $title);
                    }
                print '<div id="'.
                      $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.'_mpb'.$page_index.
                      '" class="'.($actual ?
                                   $o2ctrl_obj->css_label_on :
                                   $o2ctrl_obj->css_label_off).
                      '" '.$js_script_local.'>'.jx_encode($title)."</div>";
                }
            }
        else {
            if ($app->multilang) {
                $title = $app->translate($o2ctrl_obj->nome,
                                         'page',
                                         0,
                                         $o2ctrl_obj->label[0]);
                }
            else {
                $title = $o2ctrl_obj->label[0];
                }
            print '<div class="'.$o2ctrl_obj->css_label_on.'">'.
                  jx_encode($title).'</div>';
            }

        }


    /**
     * Draws horizontal buttons bar for multipage control
     *
     * TODO: Not used - To be reviewed - May be unuseful
     *
     * @param o2_ctrl_multipage $o2ctrl_obj   Related o2_ctrl_multipage object
     */
    static function ctrl_multipage_v($o2ctrl_obj) {

        $app = $_SESSION['o2_app'];
        if ($o2ctrl_obj->pagine > 1) {
            $active = $o2ctrl_obj->is_active();
            foreach ($o2ctrl_obj->label as $page_index => $title) {
                $actual = ($o2ctrl_obj->valore == $page_index);
                if ($active) {
                    $js_script_local = $o2ctrl_obj->js_props("page:".$page_index).
                                       ($actual ? "" :
                                     'onClick="o2jse.ctrl.multiPage(this)" tabindex="0"');
                    }
                else {
                    $js_script_local = "";
                    }
                print '<div id="'.
                      $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_mpb".$page_index.
                      '" class="'.($actual ?
                                   $o2ctrl_obj->css_label_on :
                                   $o2ctrl_obj->css_label_off).
                      '" '.$js_script_local.'>'.jx_encode($title)."</div>";
                }
            }
        else {
            print "<input class='".$o2ctrl_obj->css_label_on."'>".
                  jx_encode($o2ctrl_obj->label[0])."</div>";
            }

        }


    /**
     * Creates HTML interface related to o2_ctrl_tab control.
     * This method manages internal navigator too, if any.
     *
     * @param o2_ctrl_tab $o2ctrl_obj   Related o2_ctrl_tab object
     */
    static function ctrl_tab($o2ctrl_obj) {

        $rm_col = ($o2ctrl_obj->row_marker ? 1 : 0);
        $o2ctrl_obj->colonne = 0;
        if ($o2ctrl_obj->view_obj) {
            print "<!-- table start -->\n";
            $app       = $_SESSION['o2_app'];
            $html_name = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
            // __________________________________________ Check for internal navigator ___
            o2html::ctrl_tab_getnav($o2ctrl_obj);
            // ___________________________ All grid-plus functions are enabled on grid ___
            if ($o2ctrl_obj->gp_vis_cols &&
                $o2ctrl_obj->gp_ord_cols &&
                $o2ctrl_obj->gp_size_cols &&
                $o2ctrl_obj->gp_filter_recs &&
                $o2ctrl_obj->gp_sort_recs &&
                $o2ctrl_obj->gp_rows_num &&
                $o2ctrl_obj->gp_export &&
                $o2ctrl_obj->gp_delete) {
                $custom_plus = false;
                }
            // ___________________ One or more grid-plus function are disabled on grid ___
            else {
                // ___________________________________ NOTE: TRUE flag means DISABLED! ___
                $custom_plus = 'gp_vc:'.($o2ctrl_obj->gp_vis_cols ? '!1' : '!0').','.
                               'gp_oc:'.($o2ctrl_obj->gp_ord_cols ? '!1' : '!0').','.
                               'gp_sc:'.($o2ctrl_obj->gp_size_cols ? '!1' : '!0').','.
                               'gp_fr:'.($o2ctrl_obj->gp_filter_recs ? '!1' : '!0').','.
                               'gp_sr:'.($o2ctrl_obj->gp_sort_recs ? '!1' : '!0').','.
                               'gp_rn:'.($o2ctrl_obj->gp_rows_num ? '!1' : '!0').','.
                               'gp_xp:'.($o2ctrl_obj->gp_export ? '!1' : '!0').','.
                               'gp_dl:'.($o2ctrl_obj->gp_delete ? '!1' : '!0');
                }
            // -------------------------=[ START EXTERNAL DIV ]=--------------------------
            $js_props = ($o2ctrl_obj->pinned_cols ?
                         'pc:'.$o2ctrl_obj->pinned_cols : '').
                        ($o2ctrl_obj->pinned_cols && $custom_plus ? ',' : '').
                        $custom_plus;
            print "<div id='".$html_name."' ".
                  $o2ctrl_obj->js_props($js_props ? $js_props : false).
                      " class='".$o2ctrl_obj->spazio_class.
                                 ($o2ctrl_obj->modificabile ? " jxtabedit" : "").
                     "' style='width:".$o2ctrl_obj->larghezza.
                           "px;height:".$o2ctrl_obj->altezza."px;".
                      ($o2ctrl_obj->visibile ? "" : "display:none;")."'>\n";
            // ______________________________________________________ Form input field ___
            $active_form = $o2ctrl_obj->form_obj->attiva;
            if ($active_form) {
                print "<input type='hidden' name='".$html_name."'>\n";
                }
            // ______________________________________________ Main container (contAll) ___
            print "<div style='position:absolute;'>\n";
            // ______________________________________________________________ Viewport ___
            $tab_width  = $o2ctrl_obj->larghezza;
            $tab_height = $o2ctrl_obj->altezza;
            if ($o2ctrl_obj->internal_nav) {
                if ($o2ctrl_obj->internal_nav->direction != "V") {
                    $tab_height-= $o2ctrl_obj->internal_nav->altezza;
                    }
                else {
                    $tab_width-= $o2ctrl_obj->internal_nav->larghezza;
                    }
                }
            print "<div class='".$o2ctrl_obj->tab_class.
                     "' style='overflow:auto;width:".$tab_width.
                                         "px;height:".$tab_height."px;'>\n";
            // _____________________________________________________________ Recordset ___
            $view = $o2ctrl_obj->view_obj;
            if (($o2ctrl_obj->form_obj->attiva && $view->refresh()) ||
                (!$view->vista && $view->record_primo())) {
                $o2ctrl_obj->form_obj->leggi_valori_vista();
                }
            $view_recs = count($view->recordset);
            // ____________________________________________________________ HTML table ___
            print "<table id='".$html_name.
                  "_tab' cellspacing='0' cellpadding='0' style='width:100%;'>\n";
            // ------------------------------=[ HEADER ]=---------------------------------
            o2html::ctrl_tab_header($o2ctrl_obj);
            // ------------------------------=[ FOOTER ]=---------------------------------
            if ($o2ctrl_obj->footer) {
                o2html::ctrl_tab_footer($o2ctrl_obj);
                }
            // -------------------------------=[ BODY ]=----------------------------------
            print "<tbody>";
            // _________ Se almeno un record nel recordset oppure siamo in inserimento ___
            if($view->recordset[0] || $view->status == "I") {
                $alt = false;
                $o2ctrl_obj->status_open();
                // ____________ Save ->modificabile to preserve it throw redefinitions ___
                $o2ctrl_obj->save_modify = $o2ctrl_obj->modificabile;
                // ______________________________________________________ Loop on rows ___
                for ($view->selezione = 0;
                     $view->selezione < $view_recs;
                     $view->selezione++) {
                    $alt = !$alt;
                    $view->corrente = &$view->recordset[$view->selezione];
                    $view->comunica_variazioni();
                    $o2ctrl_obj->riga_corrente = ($view->selezione !=
                                                  $o2ctrl_obj->views_sel[$view->nome] ?
                                                  false : true);
                    $o2ctrl_obj->form_obj->read_def();
                    // _______________________ Reset ->modificabile after redefinition ___
                    $o2ctrl_obj->modificabile = $o2ctrl_obj->save_modify;
                    o2html::ctrl_tab_row($o2ctrl_obj, $alt);
                    }
                $o2ctrl_obj->status_close();
                }
             // __________________________________________ No records within selection ___
             else {
                print "<tr class='".$o2ctrl_obj->riga_class."'><td colspan='".
                      ($o2ctrl_obj->colonne + $rm_col + 2).
                      "' style='background-color: transparent;'>\n<center><em>".
                      $o2ctrl_obj->norec_msg."</em></center>\n</td></tr>\n";
                }
            print "</tbody></table></div></div>";
            // ________ Reset ->modificabile for internal-navigator after redefinition ___
            if ($o2ctrl_obj->internal_nav &&
                !$o2ctrl_obj->modificabile &&
                $o2ctrl_obj->dis_by_parent) {
                $o2ctrl_obj->internal_nav->enable_del(false);
                $o2ctrl_obj->internal_nav->enable_detail(false);
                $o2ctrl_obj->internal_nav->enable_new(false);
                $o2ctrl_obj->internal_nav->enable_save(false);
                $o2ctrl_obj->internal_nav->enable_select(false);
                $o2ctrl_obj->internal_nav->enable_undo(false);
                }
            // ___________________________________________ Internal VERTICAL navigator ___
            if ($o2ctrl_obj->internal_nav &&
                $o2ctrl_obj->internal_nav->direction == "V") {
                print "<div style='display:inline;float:right;height:100%;'>";
                $o2ctrl_obj->internal_nav->disegna();
                print "</div>";
                }
            // _________________________________________ Internal HORIZONTAL navigator ___
            if ($o2ctrl_obj->internal_nav &&
                $o2ctrl_obj->internal_nav->direction != "V") {
                print "<div style='position:absolute;bottom:0;width:100%;'>";
                $o2ctrl_obj->internal_nav->disegna();
                print "</div>";
                }
            $o2ctrl_obj->fill_rows = ($view->righe_vis - $view_recs);
            // ______________________________ In case of "refresh" scroll is not reset ___
            if ($o2ctrl_obj->view_obj->relocated &&
                $o2ctrl_obj->view_obj->relocated !== "refresh") {
                $hscroll = "0";
                $vscroll = "0";
                $o2ctrl_obj->hscroll = 0;
                $o2ctrl_obj->vscroll = 0;
                }
            else {
                $hscroll = $o2ctrl_obj->hscroll;
                $vscroll = $o2ctrl_obj->vscroll;
                }
            print "</div>\n<script> o2jse.tab.set(\"".$html_name."\",".
                  $o2ctrl_obj->fill_rows.",".$hscroll.",".$vscroll.",".
                  ($active_form && $o2ctrl_obj->focus ? $o2ctrl_obj->focus : '""').
                  "); </script>\n<!-- table end -->\n";
            $o2ctrl_obj->view_obj->relocated = false;
            }

        }


    /**
     * Set internal navigator control for the table
     *
     * @param o2_ctrl_tab $o2ctrl_obj
     */
    static function ctrl_tab_getnav($o2ctrl_obj) {

        if (!$o2ctrl_obj->internal_nav) {
            foreach ($o2ctrl_obj->form_obj->controlli as $single_ctrl) {
                if ($single_ctrl->tipo_ctrl == 'navigator'   &&
                    $single_ctrl->padre == $o2ctrl_obj->nome &&
                    $single_ctrl->task == $o2ctrl_obj->task  &&
                    $single_ctrl->visibile) {
                    $o2ctrl_obj->internal_nav = $single_ctrl;
                    break;
                    }
                }
            }

        }


    /**
     * Prints out HTML table header
     *
     * @param o2_ctrl_tab $o2ctrl_obj   Related o2_ctrl_tab object
     */
    static function ctrl_tab_header($o2ctrl_obj) {

        $app                  = $_SESSION['o2_app'];
        $sizable              = $app->grid_plus && $o2ctrl_obj->gp_size_cols;
        $prg                  = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $row_index            = 1;
        $puntatore_col        = 0;
        $view                 = $o2ctrl_obj->view_obj;
        $form                 = $o2ctrl_obj->form_obj;
        $rm_col               = ($o2ctrl_obj->row_marker ? 0 : 1);
        $o2ctrl_obj->footer   = false;
        $code                 = "";
        $o2ctrl_obj->rows_vis = array();
        print "<thead>\n";
        foreach ($form->controlli as $ctrl) {
            $ordinabile = "";
            // ______________ If control is inside of table, and it is not a navigator ___
            if ($ctrl->padre == $o2ctrl_obj->nome && $ctrl->tipo_ctrl != 'navigator') {
                // ________________________ Check column visibility for row visibility ___
                if ($ctrl->visibile) {
                    $o2ctrl_obj->rows_vis[intval($ctrl->info_padre[0])] = true;
                    }
                // __________________________________________________ Check for footer ___
                $o2ctrl_obj->footer = $o2ctrl_obj->footer ||
                                      (isset($ctrl->info_padre[8]) &&
                                       $ctrl->info_padre[8]);
                $ord_js    = "";
                $ord_title = "";
                $ord_style = "cursor:default;";
                // _____________________________________ Action on column header click ___
                if ($ctrl->info_padre[11]) {
                    $ord_js    = ' onClick=\'o2jse.tab.colExe(event,'.
                                                          $o2ctrl_obj->id_esecuzione.',"'.
                                                          $ctrl->info_padre[11].'","'.
                                                          $ctrl->nome.'");\'';
                    $ord_style = "cursor:pointer;";
                    }
                // ___________________________________________________ Order by column ___
                elseif ($ctrl->campo_in_ctrl) {
                    $campo_obj = $o2ctrl_obj->prg_obj->contesto[$ctrl->task]->
                                  campo_per_controllo($ctrl->campo_in_ctrl);
                    // ______________________________________________ Ordinable column ___
                    if (($campo_obj->file == $view->file->indice ||
                         $view->files[$campo_obj->file]->link_by_sql) &&
                        (!isset($ctrl->info_padre[4]) || !$ctrl->info_padre[4])) {
                        $css_sort    = $o2ctrl_obj->sort_class;
                        $indice_sort = "";
                        $ord_title   = ' title="Click to order by this column"';
                        $ord_style   = "cursor:pointer;";
                        if (array_key_exists($ctrl->campo_in_ctrl, $view->sort)) {
                            $asc         = ($view->sort[$ctrl->campo_in_ctrl] == "A");
                            $css_sort    = ($asc ? $o2ctrl_obj->sorta_class :
                                                   $o2ctrl_obj->sortd_class);
                            $indice_sort = "<span class='o2_tab_sorti'>".
                                           (array_search($ctrl->campo_in_ctrl,
                                                         array_keys($view->sort)) + 1).
                                           "</span>";
                            $ord_title   = ($asc ?
                                     ' title="Click to order by this column descending"' :
                                    ' title="Remove this column from ordering criteria"');
                            }
                        $ordinabile = '<span class="'.$css_sort.'"> &nbsp; </span>'.
                                      $indice_sort;
                        if ($form->attiva) {
                            $ord_js = ' onClick="o2jse.tab.sort(this);"';
                            }
                        }
                    }
                if (trim($ctrl->info_padre[3]) != "") {
                    if ($app->multilang) {
                        $titolo = jx_encode($app->translate($ctrl->nome,
                                                            'grid_title',
                                                            0,
                                                            $ctrl->info_padre[3]));
                        }
                    else {
                        $titolo = jx_encode($ctrl->info_padre[3]);
                        }
                    }
                else {
                    $titolo = "&nbsp;";
                    }
                // __________________________________________________ Avanzamento riga ___
                if ($ctrl->info_padre[0] > $row_index) {
                    print "<tr".(!$o2ctrl_obj->rows_vis[$row_index] ?
                                 ' style="display:none;"' : "").">\n".
                                ($o2ctrl_obj->row_marker ?
                                 '<th class="'.$o2ctrl_obj->marker_class.
                                 "\"> &nbsp; </th>\n" : "").$code."</tr>\n";
                    $code          = "";
                    $puntatore_col = 0;
                    $row_index     = $ctrl->info_padre[0];
                    }
                // _______________________________________________ Avanzamento colonna ___
                if ($ctrl->info_padre[1] > ($puntatore_col + 1)) {
                    $code.= '<th class="'.$o2ctrl_obj->testa_class.'" colspan="'.
                            ($ctrl->info_padre[1] - ($puntatore_col + 1)).
                            "\"> &nbsp; </th>\n";
                    }
                $client_size = $prg->custom_grids[$o2ctrl_obj->nome]["S"][$ctrl->nome];
                $client_vis  = $prg->custom_grids[$o2ctrl_obj->nome]["V"][$ctrl->nome];
                if ($client_size) {
                    $col_size = ($client_size + $ctrl->larghezza);
                    if ($col_size < 5) {
                        $col_size = 5;
                        $prg->custom_grids[$o2ctrl_obj->nome]["S"][$ctrl->nome] =
                                                                    -$ctrl->larghezza + 5;
                        }
                    }
                else {
                    $col_size = false;
                    }
                $col_js = ($o2ctrl_obj->gp_ord_cols ?
                           ' onmousedown="o2jse.tab.startColMovingTO(event);"' : '');
                $o2ctrl_obj->colonne = max($ctrl->info_padre[2], $o2ctrl_obj->colonne);
                $puntatore_col       = $ctrl->info_padre[2];
                if ($app->multilang) {
                    $tip = $app->translate($ctrl->nome,
                                           'grid_tip',
                                           0,
                                           $ctrl->info_padre[10]);
                    }
                else {
                    $tip = $ctrl->info_padre[10];
                    }
                $cssc = $ctrl->info_padre[7];
                $cssc = ($cssc ?
                         ($cssc{0} === ' ' ? $o2ctrl_obj->testa_class : '').$cssc :
                         $o2ctrl_obj->testa_class);
                $code.= '<th o2="e:'.$o2ctrl_obj->id_esecuzione.
                           ",f:'".$o2ctrl_obj->form.
                          "',cT:'".$o2ctrl_obj->tipo_ctrl.
                          "',c:'".$o2ctrl_obj->nome.
                          "',inctrl:'".$ctrl->nome.
                          "',col:".($puntatore_col - $rm_col).
                           ",rows:".$view->righe_vis.
                          ',v:'.($ctrl->visibile ? '!0' : '!1').
                           ',fret:!0,msg:\'\'" class="'.$cssc.
                                                        ($ordinabile ? ' jxord' : '').'"'.
                          (($ctrl->info_padre[2] - $ctrl->info_padre[1]) ?
                           ' colspan="'.($ctrl->info_padre[2] - $ctrl->info_padre[1] + 1).
                           '"' : '').
                        ' style="'.($col_size ? 'width:'.$col_size.'px;' : "").$ord_style.
                        ($ctrl->visibile && !$client_vis ? "" : "display:none;").'"'.
                        ($tip ? ' title="'.$tip.'"' : $ord_title).$ord_js.$col_js.'>'.
                        ($code && $sizable ?
                         '<div class="o2_tab_resize" title="Click & drag to resize '.
                         'column" onmousedown="o2jse.tab.startSizing(event);"> &nbsp; '.
                         '</div>' : '').$titolo.$ordinabile."</th>\n";
                }
            }
        // _________________________________________________________ Last column sizer ___
        if ($sizable) {
            $code.= '<th o2="e:'.$o2ctrl_obj->id_esecuzione.
                       ",f:'".$o2ctrl_obj->form.
                      "',cT:'".$o2ctrl_obj->tipo_ctrl.
                      "',c:'".$o2ctrl_obj->nome.
                      "',col:".($puntatore_col + 1 - $rm_col).
                       ',fret:!0,msg:\'\'" class="'.$o2ctrl_obj->testa_class.
                '" style="width:2px;"><div class="o2_tab_resize" title="Click & drag to '.
                    'resize column" onmousedown="o2jse.tab.startSizing(event);"> &nbsp; '.
                    "</div></th>\n";
            }
        // ________________________________________________________________ Output row ___
        print "<tr".(!$o2ctrl_obj->rows_vis[$row_index] ? ' style="display:none;"' : "").
              ">\n".($o2ctrl_obj->row_marker ?
                     '<th class="'.$o2ctrl_obj->marker_class."\">&nbsp;</th>\n" : "").
              $code."</tr>\n</thead>\n";
        $o2ctrl_obj->lines_per_row = $row_index;

        }


    /**
     * Prints out current body row
     *
     * @param o2_ctrl_tab $o2ctrl_obj   Related o2_ctrl_tab object
     * @param boolean     $alt
     */
    static function ctrl_tab_row($o2ctrl_obj, $alt = false) {

        $app            = $_SESSION['o2_app'];
        $prg            = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $coda           = "";
        $puntatore_riga = 0;
        $puntatore_col  = 0;
        $row_class      = ($alt ? $o2ctrl_obj->riga_class : $o2ctrl_obj->alt_class);
        $active_form    = $o2ctrl_obj->form_obj->attiva;
        $row_vis        = ($o2ctrl_obj->rows_vis[1] ? ' style="display:none;"' : "");
        $css_cella      = ($o2ctrl_obj->riga_corrente ?
                           $o2ctrl_obj->sele_class :
                           $o2ctrl_obj->corpo_class);
        foreach ($o2ctrl_obj->form_obj->controlli as $indice_controllo => &$ctrl) {
            // ______________________________ Controls (no navigator) inside the table ___
            if ($ctrl->padre == $o2ctrl_obj->nome && $ctrl->tipo_ctrl != 'navigator') {
                // ______________________________________________________ PARENT INFOs ___
                $info_row       = $ctrl->info_padre[0];
                $info_col_start = $ctrl->info_padre[1];
                $info_col_end   = $ctrl->info_padre[2];
                $info_css       = $ctrl->info_padre[5];
                // __________________________________________________ Avanzamento riga ___
                if ($info_row > $puntatore_riga) {
                    // __________________ Visibility check in ctrl_tab_header() method ___
                    $row_style = (!$o2ctrl_obj->rows_vis[intval($info_row)] ?
                                  ' style="display:none;" ' : " ");
                    // ______________________________________ Table row in active form ___
                    if ($active_form) {
                        // ________________________________ Current active edit record ___
                        if ($o2ctrl_obj->riga_corrente && $o2ctrl_obj->modificabile) {
                            $tr = '<tr'.$row_style.$o2ctrl_obj->js_props().
                                  ' onDblClick="o2jse.tab.rowDblClick(event);"'.
                                  ' class="'.$row_class." o2tab_current\">\n";
                            }
                            // ____________ Current read-only record and other records ___
                        else {
                            $tr = "<tr ".$o2ctrl_obj->js_props(
                                       "lines:".$o2ctrl_obj->lines_per_row.",".
                                       ($o2ctrl_obj->riga_corrente ? "" : "tabRow:!0,").
                                                     "fret:!0,msg:'',cssp:'".
                                      $o2ctrl_obj->point_class."',cssc:'".$row_class."'").
                                  "class=\"".$row_class.
                                  ($o2ctrl_obj->riga_corrente ? " o2tab_current" : "").
                                  '" onMouseOver="o2jse.tab.rowOn(this,event'.
                                  ($o2ctrl_obj->riga_corrente ? ",'o2tab_current'" : "").
                                  ');" onMouseOut="o2jse.tab.rowOut(this,event'.
                                  ($o2ctrl_obj->riga_corrente ? ",'o2tab_current'" : "").
                                  ');"'.
                                  ($o2ctrl_obj->riga_corrente ? ($o2ctrl_obj->row_marker ?
                                   ' onClick="o2jse.tab.activateMarker(o2jse.infoForm[\''.
                                   $o2ctrl_obj->nome.'_sele'.$o2ctrl_obj->id_esecuzione.
                                   '\'], event);"' : "") :
                                   ' onClick="o2jse.tab.select(this,event);"').
                                   ' onDblClick="o2jse.tab.rowDblClick(event);"'.
                                   $row_style.
                                   '>';
                            }
                        }
                    // ____________________________________ Table row in inactive form ___
                    else {
                        $tr = '<tr'.$row_style.' class="'.$row_class.
                              ($o2ctrl_obj->riga_corrente ? " o2tab_current" : "").
                              "\">\n";
                        }
                    print $coda.$tr;
                    // ____________________________________________________ Row marker ___
                    if ($o2ctrl_obj->row_marker) {
                        if (!$o2ctrl_obj->riga_corrente) {
                            $marker = '<td class="'.$o2ctrl_obj->fcol_class.
                                      '" cssp="'.$o2ctrl_obj->fcol_class.
                                      '"><div class="'.$o2ctrl_obj->marker_class.
                                      '">&nbsp;</div>';
                            }
                        elseif (!$o2ctrl_obj->modificabile && $active_form) {
                            $tot_rec = count($o2ctrl_obj->view_obj->recordset);
                            $marker  = '<td class="'.$o2ctrl_obj->fcol_class.
                                     '" cssc="'.$o2ctrl_obj->fcol_class.
                                     '"><input type="button" name="'.$o2ctrl_obj->nome.
                                     '_sele'.$o2ctrl_obj->id_esecuzione.
                                     '" class="'.$o2ctrl_obj->marker_class.'_a" '.
                                     $o2ctrl_obj->js_props("pointer:!0,rows:".$tot_rec).
                                     'onFocus="this.className=\''.
                                     $o2ctrl_obj->marker_class.
                                     '_focus\';" onBlur="this.className=\''.
                                     $o2ctrl_obj->marker_class."_a';\">\n";
                            }
                        else {
                            $marker = '<td class="'.$o2ctrl_obj->fcol_class.
                                      '" cssc="'.$o2ctrl_obj->fcol_class.
                                      '"><div class="'.$o2ctrl_obj->marker_class.
                                      '_a">&nbsp;</div>';
                            }
                        print $marker."</td>\n";
                        }
                    $puntatore_col  = 0;
                    $puntatore_riga = $info_row;
                    }
                // _______________________________________________ Avanzamento colonna ___
                if ($info_col_start > ($puntatore_col + 1)) {
                    print '<td class="'.$o2ctrl_obj->corpo_class.'" colspan="'.
                          ($info_col_start - $puntatore_col - 1)."\">&nbsp;</td>\n";
                    }
                // ____________________________________________ Get column client size ___
                $client_size = $prg->custom_grids[$o2ctrl_obj->nome]["S"][$ctrl->nome];
                $col_size    = ($client_size + $ctrl->larghezza);
                // ______________________________________ Get column client visibility ___
                $client_vis  = $prg->custom_grids[$o2ctrl_obj->nome]["V"][$ctrl->nome];
                // _____________________________________________________________ CELLA ___
                $csst = ($info_css ?
                         ($info_css{0} === ' ' ? $css_cella : '').$info_css :
                         $css_cella);
                $cssc = ($info_css ?
                        ($info_css{0} === ' ' ? $o2ctrl_obj->corpo_class : '').$info_css :
                         $o2ctrl_obj->corpo_class);
                $cssp = ($info_css ?
                         ($info_css{0} === ' ' ? $o2ctrl_obj->sele_class : '').$info_css :
                         $o2ctrl_obj->sele_class);
                print '<td class="'.$csst.'"'.
                      (($info_col_end - $info_col_start) ?
                       ' colspan="'.($info_col_end - $info_col_start + 1).'"' :
                       '').
                      ($o2ctrl_obj->riga_corrente ?
                       (!$o2ctrl_obj->modificabile && $ctrl->popup_exp ?
                        ' o2="e:'.$o2ctrl_obj->id_esecuzione.
                           ',puact:\''.$ctrl->popup_act.
                          "',puexp:'".$ctrl->popup_exp.
                      '\'" onMouseOver="o2jse.pu.i(event);" onMouseOut="o2jse.pu.o();" ' :
                        '').' cssc="'.$cssc.'"' :
                       ' cssp="'.$cssp.'"').
                      ' style="width:'.$col_size.'px;'.
                       ($ctrl->visibile && !$client_vis ? "" : "display:none;")."\">";
                $o2ctrl_obj->colonne = max($info_col_end, $o2ctrl_obj->colonne);
                // _____________________________ Impostazione del valore del controllo ___
                if ($ctrl->campo_in_ctrl) {
                    $ctrl->valore = $o2ctrl_obj->prg_obj->contesto[$ctrl->task]->
                                     campo($ctrl->campo_in_ctrl);
                    }
                // ______________________________________________ Disegno il controllo ___
                $ultimo_val_local = (isset($last_values[$indice_controllo]) ?
                                           $last_values[$indice_controllo] : "");
                if ((!$o2ctrl_obj->riga_corrente || !$o2ctrl_obj->modificabile) &&
                    ($ctrl->tipo_ctrl == "edit"  ||
                     $ctrl->tipo_ctrl == "text"  ||
                     $ctrl->tipo_ctrl == "file")) {
                    $valore_local = $ctrl->sql2ctrl();
                    if (trim($valore_local) == "") {
                        $valore_local = "&nbsp;";
                        }
                    $ctrl->active_by_tab = false;
                    $ctrl->tab_row       = !$o2ctrl_obj->riga_corrente;
                    print '<div '.
                          ($o2ctrl_obj->riga_corrente ?
                           ' id="'.$ctrl->nome.$o2ctrl_obj->id_esecuzione.'"' : '').
                          ' style="max-width:'.$col_size.
                                  'px;height:'.$ctrl->altezza.'px;'.
                                  ($ctrl->maschera->allineamento == "Right" ?
                                  'float:right;" ' : '" ').
                          $ctrl->js_props().'>'.$valore_local."</div>";
                    }
                else {
                    $ctrl->active_by_tab = ($o2ctrl_obj->modificabile &&
                                            $o2ctrl_obj->riga_corrente);
                    $ctrl->tab_row       = !$o2ctrl_obj->riga_corrente;
                    // _________________________________ Client-sizeable controls only ___
                    if ($ctrl->tipo_ctrl == "edit"      ||
                        $ctrl->tipo_ctrl == "listcombo" ||
                        $ctrl->tipo_ctrl == "html"      ||
                        $ctrl->tipo_ctrl == "text"      ||
                        $ctrl->tipo_ctrl == "file") {
                        $ctrl->larghezza = $col_size;
                        }
                    $ctrl->disegna();
                    }
                $ctrl->active_by_tab = true;
                $ctrl->tab_row       = false;
                print "</td>\n";
                $puntatore_col = $info_col_end;
                $coda          = "</tr>";
                }
            }
        print $coda;

        }


    /**
     * Prints out HTML table footer
     *
     * @param o2_ctrl_tab $o2ctrl_obj   Related o2_ctrl_tab object
     */
    static function ctrl_tab_footer($o2ctrl_obj) {

        $puntatore_riga = 0;
        $puntatore_col  = 0;
        $app            = $_SESSION['o2_app'];
        $prg            = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $view           = $o2ctrl_obj->view_obj;
        $form           = $o2ctrl_obj->form_obj;
        $sizable        = $app->grid_plus && $o2ctrl_obj->gp_size_cols;
        print "<tfoot>\n";
        foreach ($form->controlli as $ctrl) {
            // ________ If control is inside of table, it is visible and not navigator ___
            if ($ctrl->padre == $o2ctrl_obj->nome && $ctrl->tipo_ctrl != 'navigator') {
                // _________________________________________________ Footer expression ___
                if (isset($ctrl->info_padre[8]) && $ctrl->info_padre[8]) {
                    if ($app->multilang) {
                        $footer = jx_encode($app->translate($ctrl->nome,
                                                            'grid_foot',
                                                            0,
                                                            $ctrl->info_padre[8]));
                        }
                    else {
                        $footer = jx_encode($ctrl->info_padre[8]);
                        }
                    }
                // ______________________________________________________ Empty footer ___
                else {
                    $footer = " &nbsp; ";
                    }
                // __________________________________________________ Avanzamento riga ___
                if ($ctrl->info_padre[0] > $puntatore_riga) {
                    print "<tr>\n";
                    if ($o2ctrl_obj->row_marker) {
                        print '<td class="'.$o2ctrl_obj->marker_class."\">&nbsp;</td>\n";
                        }
                    $puntatore_col  = 0;
                    $puntatore_riga = $ctrl->info_padre[0];
                    }
                // _______________________________________________ Avanzamento colonna ___
                if ($ctrl->info_padre[1] > ($puntatore_col + 1)) {
                    print '<td class="'.$o2ctrl_obj->footer_class.'" colspan="'.
                          ($ctrl->info_padre[1] - $puntatore_col - 1)."\">&nbsp;</td>\n";
                    }
                // ______________________________________ Get column client visibility ___
                $client_vis  = $prg->custom_grids[$o2ctrl_obj->nome]["V"][$ctrl->nome];
                $style       = ' style="'.($ctrl->maschera->allineamento == "Right" ?
                                           ' text-align:right;' : '').
                               ($ctrl->visibile && !$client_vis ? '' : ' display:none;').
                               '"';
                $cssc = $ctrl->info_padre[9];
                $cssc = ($cssc ?
                         ($cssc{0} === ' ' ? $o2ctrl_obj->footer_class : '').$cssc :
                         $o2ctrl_obj->footer_class);
                print '<td class="'.$cssc.'"'.
                         (($ctrl->info_padre[2] - $ctrl->info_padre[1]) ?
                          ' colspan="'.($ctrl->info_padre[2] - $ctrl->info_padre[1] + 1).
                          '"' : '').$style.">".$footer."</td>\n";
                $puntatore_col = $ctrl->info_padre[2];
                }
            }
        // _________________________________________________________ Last column sizer ___
        if ($sizable) {
            print '<td class="'.$o2ctrl_obj->footer_class.
                '"> &nbsp; '."</th>\n";
            }
        print "</tfoot>\n";

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     */
    static function ctrl_nav($o2ctrl_obj) {

        $prg_local    = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $task_local   = $prg_local->contesto[$o2ctrl_obj->task];
        $form         = $prg_local->form[$o2ctrl_obj->form];
        $tab_internal = false;
        $css_space    = $o2ctrl_obj->css_space;
        $html_name    = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        if ($o2ctrl_obj->padre && $form->controlli[$o2ctrl_obj->padre]) {
            $parent = $form->controlli[$o2ctrl_obj->padre];
            if ($parent->tipo_ctrl == "tab") {
                $tab_internal = true;
                $css_space   .= "_int";
                // _____________ Disabled by parent of parent (table inside multipage) ___
                if (!$parent->modificabile && $parent->dis_by_parent) {
                    $o2ctrl_obj->enable_del(false);
                    $o2ctrl_obj->enable_detail(false);
                    $o2ctrl_obj->enable_new(false);
                    $o2ctrl_obj->enable_save(false);
                    $o2ctrl_obj->enable_select(false);
                    $o2ctrl_obj->enable_undo(false);
                    }
                }
            }
        print "<!-- navigator start -->\n";
        if ($o2ctrl_obj->is_active()) {
            $js_props = $o2ctrl_obj->js_props(
                        "navSele:".($o2ctrl_obj->act_select &&
                                    $o2ctrl_obj->vis_select &&
                                    $o2ctrl_obj->enable_select ? "true" : "false").
                        ",navDet:".($o2ctrl_obj->act_detail &&
                                    $o2ctrl_obj->vis_detail &&
                                    $o2ctrl_obj->enable_detail ? "true" : "false").
                        ",navNew:".($o2ctrl_obj->act_new &&
                                    $o2ctrl_obj->vis_new &&
                                    $o2ctrl_obj->enable_new ? "true" : "false").
                        ",navDel:".($o2ctrl_obj->act_del &&
                                    $o2ctrl_obj->vis_del &&
                                    $o2ctrl_obj->enable_del ? "true" : "false").
                       ",navSave:".($o2ctrl_obj->act_save &&
                                    $o2ctrl_obj->vis_save &&
                                    $o2ctrl_obj->enable_save ? "true" : "false").
                       ",navUndo:".($o2ctrl_obj->act_undo &&
                                    $o2ctrl_obj->vis_undo &&
                                    $o2ctrl_obj->enable_undo ? "true" : "false").
                        ",navNav:".($o2ctrl_obj->vis_nav ? "true" : "false"));
            print "<input type='hidden' name='".$html_name."' ".$js_props." value='0'>\n";
            }
        else {
            $js_props = "";
            }
        if ($o2ctrl_obj->direction != "V") {
            print "<table ".$js_props." cellpadding='0' cellspacing='0' class='".
                  $css_space."' style='".
                  ($tab_internal ? "" : 'width:'.$o2ctrl_obj->larghezza."px;").
                  ($o2ctrl_obj->visibile ? "" : "display:none;").
                  "'><tr style='line-height:5px'>\n";
            o2html::ctrl_nav_btn($o2ctrl_obj, $task_local);
            o2html::ctrl_nav_bar($o2ctrl_obj, $task_local);
            print "</tr></table>\n";
            }
        else {
            print "<table ".$js_props." cellpadding='0' cellspacing='0' class='".
                  $css_space."' style='height:".
                  ($tab_internal ? "100%" : $o2ctrl_obj->altezza."px").";".
                  ($o2ctrl_obj->visibile ? "" : "display:none;")."'>\n";
            o2html::ctrl_nav_btn($o2ctrl_obj, $task_local);
            o2html::ctrl_nav_bar($o2ctrl_obj, $task_local);
            print "</table>\n";
            }
        print "<!-- navigator end -->\n";

        }


    /**
     * Print out slide position bar and movement buttons for HTML navigator
     *
     * @param o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param o2_dbview         $view         Navigator related view object
     */
    static function ctrl_nav_bar($o2ctrl_obj, $view) {

        $active_local = $o2ctrl_obj->is_active();
        // ___________________________ If paging is active (totale_record > righe_vis) ___
        if (($view->offset_ini > 0 ||
             $view->offset_fine < ($view->totale_record - 1)) &&
            $o2ctrl_obj->vis_nav) {
            // ____________________________________________________ No records in view ___
            if ($view->totale_record < 1) {
                $sp_prima   = 0;
                $sp_attuale = 100;
                $sp_dopo    = 0;
                }
            // _________________________________ Slide bar proporsional space and size ___
            else {
                $sp_prima = ($view->offset_ini * 100) / $view->totale_record;
                $sp_dopo  = (($view->totale_record - $view->offset_fine - 1) * 100) /
                            $view->totale_record;
                if ($sp_prima) {
                    $sp_prima = max(10, min(80, intval($sp_prima)));
                    }
                if ($sp_dopo) {
                    $sp_dopo = max(10, min(80, intval($sp_dopo)));
                    }
                $sp_attuale = (100 - $sp_prima - $sp_dopo);
                }
            // ________________________________________________________ HORIZONTAL BAR ___
            if ($o2ctrl_obj->direction != "V") {
                // ------------------------------ [|<][<<][ < ] --------------------------
                print "<td class=\"o2_ctrl_nav_sep\"></td>\n<td>\n".
                      o2html::ctrl_nav_first($o2ctrl_obj, $view)."</td>\n<td>\n".
                      o2html::ctrl_nav_ppag($o2ctrl_obj, $view)."</td>\n<td>\n".
                      o2html::ctrl_nav_prev($o2ctrl_obj, $view)."</td>\n";
                // ------------------------ [     ######          ] ----------------------
                if ($sp_prima > 0) {
                    print '<td title="Move backward"  class="'.$o2ctrl_obj->css_e.
                            '" style="width:'.$sp_prima.'%;" '.
                          $o2ctrl_obj->js_props("action:2,rt:false").
                          ($active_local ? ' onclick="o2jse.nav.btnExe(this);"' : '').
                          ">&nbsp;</td>\n";
                    }
                print '<td class="'.$o2ctrl_obj->css_i.'" style="width:'.$sp_attuale.
                      "%;\">&nbsp;</td>\n";
                if ($sp_dopo > 0) {
                    print '<td title="Move forward" class="'.$o2ctrl_obj->css_e.
                            '" style="width:'.$sp_dopo.'%;" '.
                          $o2ctrl_obj->js_props("action:5,rt:false").
                          ($active_local ? ' onclick="o2jse.nav.btnExe(this);"' : '').
                          ">&nbsp;</td>\n";
                    }

                // ---------------------------- [ > ][>>][>|] ----------------------------
                print "<td>\n".o2html::ctrl_nav_next($o2ctrl_obj, $view)."</td>\n<td>\n".
                               o2html::ctrl_nav_npag($o2ctrl_obj, $view)."</td>\n<td>\n".
                               o2html::ctrl_nav_last($o2ctrl_obj, $view)."</td>\n";
                // =======================================================================
                }
            // __________________________________________________________ VERTICAL BAR ___
            else {
                // --------------------------- [|<][<<][ < ] -----------------------------
                print "<tr><td class=\"o2_ctrl_navv_sep\"></td></tr>\n<tr><td>\n".
                      o2html::ctrl_nav_first($o2ctrl_obj, $view).
                      "</td></tr>\n<tr><td>\n".o2html::ctrl_nav_ppag($o2ctrl_obj, $view).
                      "</td></tr>\n<tr><td>\n".o2html::ctrl_nav_prev($o2ctrl_obj, $view).
                      "</td></tr>\n";
                // ------------------------ [     ######          ] ----------------------
                if ($sp_prima > 0) {
                    print '<tr><td title="Move backward" class="'.$o2ctrl_obj->css_e.
                                '" style="height:'.$sp_prima.'%;" '.
                          $o2ctrl_obj->js_props("action:2,rt:false").
                          ($active_local ? ' onclick="o2jse.nav.btnExe(this);"' : '').
                          ">&nbsp;</td></tr>\n";
                    }
                print '<tr><td class="'.$o2ctrl_obj->css_i.'" style="height:'.$sp_attuale.
                      "%;\">&nbsp;</td></tr>\n";
                if ($sp_dopo > 0) {
                    print '<tr><td title="Move forward" class="'.$o2ctrl_obj->css_e.
                                '" style="height:'.$sp_dopo.'%;" '.
                          $o2ctrl_obj->js_props("action:5,rt:false").
                          ($active_local ? ' onclick="o2jse.nav.btnExe(this);"' : '').
                          ">&nbsp;</td></tr>\n";
                    }

                // ---------------------------- [ > ][>>][>|] ----------------------------
                print "<tr><td>\n".o2html::ctrl_nav_next($o2ctrl_obj, $view).
                      "</td></tr>\n<tr><td>\n".o2html::ctrl_nav_npag($o2ctrl_obj, $view).
                      "</td></tr>\n<tr><td>\n".o2html::ctrl_nav_last($o2ctrl_obj, $view).
                      "</td></tr>\n";
                // =======================================================================
                }
            }
        // _______________________ If paging is NOT active (totale_record < righe_vis) ___
        elseif ($view->totale_record > 1    &&
                count($view->recordset) > 1 &&
                $o2ctrl_obj->vis_nav) {
            if ($o2ctrl_obj->direction != "V") {
                print "<td class=\"o2_ctrl_nav_sep\"></td>\n<td>".
                      o2html::ctrl_nav_first($o2ctrl_obj, $view)."</td>\n<td>".
                      o2html::ctrl_nav_prev($o2ctrl_obj, $view)."</td>\n<td class='".
                      $o2ctrl_obj->css_i."' style='width:100%;'>&nbsp;</td>\n<td>".
                      o2html::ctrl_nav_next($o2ctrl_obj, $view)."</td>\n<td>".
                      o2html::ctrl_nav_last($o2ctrl_obj, $view)."</td>\n";
                }
            else {
                print "<tr><td class=\"o2_ctrl_navv_sep\"></td></tr>\n<tr><td>".
                      o2html::ctrl_nav_first($o2ctrl_obj, $view).
                      "</td></tr>\n<tr><td>".o2html::ctrl_nav_prev($o2ctrl_obj, $view).
                      "</td></tr>\n<tr><td class='".$o2ctrl_obj->css_i.
                      "' style='height:100%;'>&nbsp;</td></tr>\n<tr><td>".
                      o2html::ctrl_nav_next($o2ctrl_obj, $view)."</td></tr>\n<tr><td>".
                      o2html::ctrl_nav_last($o2ctrl_obj, $view)."</td></tr>\n";
                }
            }
        else {
            if ($o2ctrl_obj->direction != "V") {
                print "<td style='width:100%'>&nbsp;</td>\n";
                }
            else {
                print "<tr><td style='height:100%'>&nbsp;</td></tr>\n";
                }
            }
        return;

        }


    /**
     * Print out record actions buttons for HTML navigator
     *
     * @param o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param o2_dbview         $view         Navigator related view object
     */
    static function ctrl_nav_btn($o2ctrl_obj, $view) {
        // ____________________________________________________________ HORIZONTAL BAR ___
        if ($o2ctrl_obj->direction != "V") {
            // ---------------------------- [+][V][X][<] -------------------------------
            print ($o2ctrl_obj->act_select && $o2ctrl_obj->vis_select ?
                   "<td>\n".o2html::ctrl_nav_select($o2ctrl_obj, $view)."</td>\n" :
                   "").
                  ($o2ctrl_obj->act_detail && $o2ctrl_obj->vis_detail ?
                   "<td>\n".o2html::ctrl_nav_detail($o2ctrl_obj, $view)."</td>\n" :
                   "").
                  ($o2ctrl_obj->act_new && $o2ctrl_obj->vis_new ?
                   "<td>\n".o2html::ctrl_nav_new($o2ctrl_obj, $view)."</td>\n" :
                   "").
                  ($o2ctrl_obj->act_del && $o2ctrl_obj->vis_del ?
                   "<td>\n".o2html::ctrl_nav_del($o2ctrl_obj, $view)."</td>\n" :
                   "").
                  ($o2ctrl_obj->act_save && $o2ctrl_obj->vis_save ?
                   "<td>\n".o2html::ctrl_nav_save($o2ctrl_obj, $view)."</td>\n" :
                   "").
                  ($o2ctrl_obj->act_undo && $o2ctrl_obj->vis_undo ?
                   "<td>\n".o2html::ctrl_nav_undo($o2ctrl_obj, $view)."</td>\n" :
                   "");
            }
        // ______________________________________________________________ VERTICAL BAR ___
        else {
            // ---------------------------- [+][V][X][<] -------------------------------
            print ($o2ctrl_obj->act_select && $o2ctrl_obj->vis_select ?
                   "<tr><td>\n".o2html::ctrl_nav_select($o2ctrl_obj, $view).
                   "</td></tr>\n" :
                   "").
                  ($o2ctrl_obj->act_detail && $o2ctrl_obj->vis_detail ?
                   "<tr><td>\n".o2html::ctrl_nav_detail($o2ctrl_obj, $view).
                   "</td></tr>\n" :
                   "").
                  ($o2ctrl_obj->act_new && $o2ctrl_obj->vis_new ?
                   "<tr><td>\n".o2html::ctrl_nav_new($o2ctrl_obj, $view)."</td></tr>\n" :
                   "").
                  ($o2ctrl_obj->act_del && $o2ctrl_obj->vis_del ?
                   "<tr><td>\n".o2html::ctrl_nav_del($o2ctrl_obj, $view)."</td></tr>\n" :
                   "").
                  ($o2ctrl_obj->act_save && $o2ctrl_obj->vis_save ?
                   "<tr><td>\n".o2html::ctrl_nav_save($o2ctrl_obj, $view)."</td></tr>\n" :
                   "").
                  ($o2ctrl_obj->act_undo && $o2ctrl_obj->vis_undo ?
                   "<tr><td>\n".o2html::ctrl_nav_undo($o2ctrl_obj, $view)."</td></tr>\n" :
                   "");
            }

        }

    /**
     * Return HTML code for navigator NEW button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_new($o2ctrl_obj, $view) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $msg = $app->translate($o2ctrl_obj->nome,
                                   'msg_new',
                                   0,
                                   $o2ctrl_obj->msg_new);
            }
        else {
            $msg = $o2ctrl_obj->msg_new;
            }
        $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_newbtn' ".
                $o2ctrl_obj->js_props("msg:'".$msg."',action:11,rt:false").
                          " class='".$o2ctrl_obj->css_new.
                                     ($o2ctrl_obj->enable_new ? "" : "_dis").
                         "' title='Insert new record'";
        if ($o2ctrl_obj->is_active() && $o2ctrl_obj->enable_new) {
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html.= "></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator DELETE button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @param  boolean           $inline       If rendered for row-tools
     * @return string
     */
    static function ctrl_nav_del($o2ctrl_obj, $view, $inline = false) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $msg = $app->translate($o2ctrl_obj->nome,
                                   'msg_new',
                                   0,
                                   $o2ctrl_obj->msg_del);
            }
        else {
            $msg = $o2ctrl_obj->msg_del;
            }
        $recs = (count($view->recordset) > 0);
        if ($o2ctrl_obj->is_active() && $recs && $o2ctrl_obj->enable_del) {
            $html = "<div ".($inline ? "" : "id='".$o2ctrl_obj->nome.
                                                   $o2ctrl_obj->id_esecuzione.
                                                   "_delbtn' ").
                     $o2ctrl_obj->js_props("msg:'".$msg.
                                         "',action:12,rt:".($inline ? "true" : "false")).
                      "class='".$o2ctrl_obj->css_del.
                    "' title='Delete ".($inline ? "" : "current ")."record'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
             }
        else {
            $html.= "<div class='".$o2ctrl_obj->css_del.
                    ($recs && $o2ctrl_obj->enable_del ? "'" : "_dis'")."></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator SAVE button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_save($o2ctrl_obj, $view) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $msg = $app->translate($o2ctrl_obj->nome,
                                   'msg_new',
                                   0,
                                   $o2ctrl_obj->msg_save);
            }
        else {
            $msg = $o2ctrl_obj->msg_save;
            }
        $recs = (count($view->recordset) > 0);
        if ($o2ctrl_obj->is_active() && $recs && $o2ctrl_obj->enable_save) {
            $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_savebtn' ".
                    $o2ctrl_obj->js_props("msg:'".$msg."',action:13,rt:false").
                    " class='".$o2ctrl_obj->css_save.
                    "' title='Save current record updates' ";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html = "<div class='".$o2ctrl_obj->css_save.
                    ($recs && $o2ctrl_obj->enable_save ? "'" : "_dis'")."></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator UNDO button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_undo($o2ctrl_obj, $view) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $msg = $app->translate($o2ctrl_obj->nome,
                                   'msg_new',
                                   0,
                                   $o2ctrl_obj->msg_undo);
            }
        else {
            $msg = $o2ctrl_obj->msg_undo;
            }
        $recs = (count($view->recordset) > 0);
        if ($o2ctrl_obj->is_active() && $recs && $o2ctrl_obj->enable_undo) {
            $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_undobtn' ".
                    $o2ctrl_obj->js_props("msg:'".$msg."',action:14,rt:false").
                    " class='".$o2ctrl_obj->css_undo.
                    "' title='Discard current record updates' ";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html = "<div class='".$o2ctrl_obj->css_undo.
                    ($recs && $o2ctrl_obj->enable_undo ? "'" : "_dis'")."></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator SELECT button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @param  boolean           $inline       If rendered for row-tools
     * @return string
     */
    static function ctrl_nav_select($o2ctrl_obj, $view, $inline = false) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $msg = $app->translate($o2ctrl_obj->nome,
                                   'msg_new',
                                   0,
                                   $o2ctrl_obj->msg_select);
            }
        else {
            $msg = $o2ctrl_obj->msg_select;
            }
        $recs = (count($view->recordset) > 0);
        if ($o2ctrl_obj->is_active() && $recs && $o2ctrl_obj->enable_select) {
            $html = "<div ".($inline ? "" : "id='".$o2ctrl_obj->nome.
                                                   $o2ctrl_obj->id_esecuzione.
                                                   "_selebtn' ").
                    $o2ctrl_obj->js_props("msg:'".$msg.
                                          "',action:15,rt:".($inline ? "true" : "false")).
                    "class='".$o2ctrl_obj->css_select.
                    "' title='Select ".($inline ? "" : "current ")."record'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html.= "<div class='".$o2ctrl_obj->css_select.
                    ($recs && $o2ctrl_obj->enable_select ? "'" : "_dis'")."></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator DETAIL button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @param  boolean           $inline       If rendered for row-tools
     * @return string
     */
    static function ctrl_nav_detail($o2ctrl_obj, $view, $inline = false) {

        $app = $_SESSION['o2_app'];
        if ($app->multilang) {
            $msg = $app->translate($o2ctrl_obj->nome,
                                   'msg_new',
                                   0,
                                   $o2ctrl_obj->msg_detail);
            }
        else {
            $msg = $o2ctrl_obj->msg_detail;
            }
       $recs = (count($view->recordset) > 0);
        if ($o2ctrl_obj->is_active() && $recs && $o2ctrl_obj->enable_detail) {
            $html = "<div ".($inline ? "" : "id='".$o2ctrl_obj->nome.
                                                   $o2ctrl_obj->id_esecuzione.
                                                   "_detbtn' ").
                    $o2ctrl_obj->js_props("msg:'".$msg.
                                          "',action:16,rt:".($inline ? "true" : "false")).
                      "class='".$o2ctrl_obj->css_detail.
                    "' title='Show ".($inline ? "" : "current ")."record details'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html.= "<div class='".$o2ctrl_obj->css_detail.
                    ($recs && $o2ctrl_obj->enable_detail ? "'" : "_dis'")."></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator GOTO FIRST RECORD button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_first($o2ctrl_obj, $view) {

        if ($o2ctrl_obj->is_active() && ($view->offset_ini || $view->selezione)) {
            $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_fbtn' ".
                    $o2ctrl_obj->js_props("action:1,rt:false").
                    " class='".$o2ctrl_obj->css_primo."' title='Go to first record'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html = "<div class='".$o2ctrl_obj->css_primo." dis'></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator GOTO PREVIOUS PAGE button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_ppag($o2ctrl_obj, $view) {

        if ($view->righe_vis > 1) {
            if ($o2ctrl_obj->is_active() && $view->offset_ini) {
                $html = "<div id='".$o2ctrl_obj->nome.
                                    $o2ctrl_obj->id_esecuzione."_ppbtn' ".
                        $o2ctrl_obj->js_props("action:2,rt:false").
                         " class='".$o2ctrl_obj->css_precedente_pag.
                        "' title='Show previous ".$view->righe_vis." records'";
                if ($o2ctrl_obj->effect_on_over) {
                    $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                    }
                $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
                }
            else {
                $html = "<div class='".$o2ctrl_obj->css_precedente_pag." dis'></div>\n";
                }
            }
        return $html;

        }


    /**
     * Return HTML code for navigator GOTO PREVIOUS RECORD button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_prev($o2ctrl_obj, $view) {

        if ($o2ctrl_obj->is_active() && ($view->offset_ini || $view->selezione)) {
            $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_pbtn' ".
                    $o2ctrl_obj->js_props("action:3,rt:false").
                    " class='".$o2ctrl_obj->css_precedente.
                    "' title='Go to previous record'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html = "<div class='".$o2ctrl_obj->css_precedente." dis'></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator GOTO NEXT RECORD button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_next($o2ctrl_obj, $view) {

        if ($o2ctrl_obj->is_active() &&
           ($view->offset_fine < ($view->totale_record - 1) ||
            $view->selezione < (count($view->recordset) - 1))) {
            $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_nbtn' ".
                    $o2ctrl_obj->js_props("action:4,rt:false").
                    " class='".$o2ctrl_obj->css_successivo."' title='Go to next record'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html = "<div class='".$o2ctrl_obj->css_successivo." dis'></div>\n";
            }
        return $html;

        }


    /**
     * Return HTML code for navigator GOTO NEXT PAGE button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_npag($o2ctrl_obj, $view) {

        if ($view->righe_vis > 1) {
            if ($o2ctrl_obj->is_active() &&
                $view->offset_fine < $view->totale_record - 1) {
                $html = "<div id='".$o2ctrl_obj->nome.
                                    $o2ctrl_obj->id_esecuzione."_npbtn' ".
                        $o2ctrl_obj->js_props("action:5,rt:false").
                        " class='".$o2ctrl_obj->css_successiva_pag.
                        "' title='Show next ".$view->righe_vis." records'";
                if ($o2ctrl_obj->effect_on_over) {
                    $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                    }
                $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
                }
            else {
                $html = "<div class='".$o2ctrl_obj->css_successiva_pag." dis'></div>\n";
                }
            }
        return $html;

        }


    /**
     * Return HTML code for navigator GOTO LAST RECORD button
     *
     * @param  o2_ctrl_navigator $o2ctrl_obj   Related o2_ctrl_navigator object
     * @param  o2_dbview         $view         Navigator related view object
     * @return string
     */
    static function ctrl_nav_last($o2ctrl_obj, $view) {

        if ($o2ctrl_obj->is_active() &&
            ($view->offset_fine < $view->totale_record - 1 ||
             $view->selezione < (count($view->recordset) - 1))) {
            $html = "<div id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione."_lbtn' ".
                    $o2ctrl_obj->js_props("action:6,rt:false").
                    " class='".$o2ctrl_obj->css_ultimo."' title='Go to last record'";
            if ($o2ctrl_obj->effect_on_over) {
                $html.= " onmouseover='o2jse.nav.onBtn(this);'";
                }
            $html.= " onclick='o2jse.nav.btnExe(this);'></div>\n";
            }
        else {
            $html = "<div class='".$o2ctrl_obj->css_ultimo." dis'></div>\n";
            }
        return $html;

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_file $o2ctrl_obj   Related o2_ctrl_file object
     */
    static function ctrl_file($o2ctrl_obj) {

        $css = $o2ctrl_obj->css.($o2ctrl_obj->modificabile ? '' : '_dis');
        if ($o2ctrl_obj->is_active()) {
            $ctrl_name       = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
            $js_change_local = "onChange='o2jse.fu.c(\"".$ctrl_name.
                               "\");' onKeyDown='o2jse.fu.k(event, this);'\n";
            $filesize_local  = ini_get('upload_max_filesize');
            if (!is_numeric($filesize_local)) {
                $filesize_local = max(1, intval($filesize_local)) * 1048576;
                }
            // _______________________________________________ Whole control container ___
            print "<div id='".$ctrl_name."' title='Select a file' class='".$css.
                  "' style='width:".$o2ctrl_obj->larghezza.
                        "px;height:".$o2ctrl_obj->altezza."px;".
                  ($o2ctrl_obj->visibile ? "" : "display:none;").
                  "'><div id='".$ctrl_name."_int' class='o2_ctrl_file_txt'>&nbsp;</div>";
            // ______________________________________________ Real file upload control ___
            print "<input type='hidden' name='MAX_FILE_SIZE' value='".$filesize_local.
                  "' />\n<div style='overflow:hidden;'><input type='file' name='".
                  $ctrl_name."' size='".$o2ctrl_obj->larghezza.
                             "' style='width:100%;opacity:0;'\n".
                         ($o2ctrl_obj->modificabile  ? $js_change_local : "DISABLED\n").
                         "></div>\n";
            // __________________________________________________________ Reset button ___
            print "<div onclick='o2jse.fu.clear(\"".$ctrl_name.
                  "\")' class='o2_ctrl_file_clear' title='Clear'>&nbsp;</div></div>";
            }
        else {
            print "<div class='".$css.
                  "' style='width:".($o2ctrl_obj->larghezza - 8).
                       "px; height:".($o2ctrl_obj->altezza - 4)."px;".
                            ($o2ctrl_obj->visibile ? "" : "display:none;").
                  "'></div>\n";
            }

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_line $o2ctrl_obj   Related o2_ctrl_line object
     */
    static function ctrl_line($o2ctrl_obj) {

        print "<hr class='".$o2ctrl_obj->css.
                "' style='width:".$o2ctrl_obj->larghezza.
                      "px;height:".$o2ctrl_obj->altezza."px;".
                          ($o2ctrl_obj->visibile ? "" : "display:none;")."'/>\n";

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_html $o2ctrl_obj   Related o2_ctrl_html object
     */
    static function ctrl_area($o2ctrl_obj) {

        // _________________________________________ NOT EXPOSED! Used only internally ___
        if ($o2ctrl_obj->campo_in_ctrl) {
            $html = $o2ctrl_obj->valore;
            }
        else {
            $html = $o2ctrl_obj->html;
            }
        print '<div'.($o2ctrl_obj->active_by_tab ?
                      ' id="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.'"' : '').
                   ' class="'.$o2ctrl_obj->css.
                  '" style="width:'.$o2ctrl_obj->larghezza.
                        'px;height:'.$o2ctrl_obj->altezza.'px;'.
                     ($o2ctrl_obj->visibile ? '' : 'display:none;').
              '">'.($o2ctrl_obj->visibile ? $html : '')."</div>\n";

        }


    /**
     * Creates HTML interface related to o2 control.
     *
     * @param o2_ctrl_doc $o2ctrl_obj   Related o2_ctrl_doc object
     */
    static function ctrl_doc($o2ctrl_obj) {

        print "<iframe id='".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.
                    "' class='".$o2ctrl_obj->css.
                    "' style='width:".$o2ctrl_obj->larghezza."px;".
                             "height:".$o2ctrl_obj->altezza."px;".
                    ($o2ctrl_obj->visibile ? '' : 'display:none;').
                    "' src='".$o2ctrl_obj->path."' frameBorder='0'></iframe>\n";

        }


    /**
     * Creates HTML login page using the current application look&feel
     *
     * @param string $file         Name of the file to generate in <app>/htdocs/
     * @param string $app_name     Application name
     * @param string $mailto       Mail address to set "contact us"
     * @param string $image        Image file to set in the page
     * @param boolean $jxpowered   If show "jxpowered" icon and link
     */
    static function login_page($file      = "index.html",
                               $app_name  = "",
                               $mailto    = "",
                               $image     = "",
                               $jxpowered = true) {

        $app  = $_SESSION['o2_app'];
        $file = $app->dir_htdocs.($file ? $file : "index.html");
        $name = ($app_name ? $app_name : strtoupper($app->nome));
        $fphp = $app->alias.$app->nome.".php";
        $mail = ($mailto ?
                 '<a class="o2_ctrl_ref" style="float:right;margin:10px 0 0 0;" '.
                    'href="mailto:'.$mailto.'">Contact us</a>' : "");
        $img  = ($image ? "<center><img src=\"$image\"></center><br>" : "");
        $jxp  = ($jxpowered ? '<a style="float:right;margin:10px 0 0 0;" '.
                 'href="http://www.janox.it"><img class="o2_ctrl_img" border="0" src="'.
                 $app->runtime->alias.'/img/jxpowered.png"  alt="www.janox.it"></a>' :
                 "");
        $head = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" '.
                "\"http://www.w3.org/TR/html4/strict.dtd\">\n<html>\n".
                '<head><meta http-equiv="Content-Type" content="text/html; charset='.
                $app->chr_encoding.
                "\">\n<meta name=\"description\" content=\"".$app->titolo."\">\n".
                "<meta name=\"GENERATOR\" content=\"Janox - www.janox.it\">\n<title>".
                $app->titolo."</title>\n";
        // _____________________________________________________________ Favorite icon ___
        $head.= '<link rel="icon" href="'.$app->favicon."\" type=\"image/ico\">\n";
        // _________________________________________________________________ CSS links ___
        $head.= o2html::style();
        // ________________________________________________________________ Javascript ___
        $head.= "<script src='/janox/js/md5.js'></script>\n".
                "<script> function jx_init() {".
                " with(document.jxForm){".
                 " if(password.value.length<32){".
                     "password.value=hex_md5(password.value);}".
                 " jxcsw.value=(window.innerWidth!=null ? ".
                               "window.innerWidth : ".
                               "document.documentElement.clientWidth);".
                 " jxcsh.value=(window.innerHeight!=null ? ".
                               "window.innerHeight : ".
                               "document.documentElement.clientHeight);".
                 "}} </script>\n";
        // _______________________________________________________ Document BODY start ___
        $head.= "</head>\n<body class=\"o2_sfondo\" >\n";
        $page = <<<JANOX_PAGE_SCRIPT
<br>
<center><h1>Welcome to $name</h1></center>
<br>$img<br>
<div style="width:265px;margin-left:auto;margin-right:auto;">
 <div class="o2_ctrl_multi_out" style="height:180px;overflow:hidden">
  <input type="button" disabled="" class="o2_ctrl_multi_on" value="Registered user">
  <div class="o2_ctrl_multi_in">
   <br><br>

   <form name="jxForm" method="POST" action="$fphp" onsubmit="jx_init();">
    <div class="o2_ctrl_label"
         style="margin:0 0 0 20px;width:80px;height:20px;float:left;">User:</div>
    <input type="text" name="user" class="o2_ctrl_edit" style="width:140px;height:20px;">
    <div style="clear:both;"> &nbsp </div>
    <div class="o2_ctrl_label"
         style="margin:0 0 0 20px;width:80px;height:20px;float:left;">Password:</div>
    <input type="password" name="password" class="o2_ctrl_edit"
           style="width:140px;height:20px;">
    <div style="clear:both;"> &nbsp </div>
    <div style="clear:both;"> &nbsp </div>
    <input type="hidden" name="jxcsw" value="">
    <input type="hidden" name="jxcsh" value="">
    <center><input type="submit" value="Login" class="o2_ctrl_button"
            style="width:90px;height:30px;"></center>
   </form>

  </div>
 </div>
 $mail
    <div style="clear:both;"> &nbsp </div><br>
 $jxp
</div>
JANOX_PAGE_SCRIPT;
        $foot = "</body></html>\n";
        file_put_contents($file, $head.$page.$foot);

        }


    /**
     * Return HTML code for a tree view interface
     *
     * @param o2_ctrl_tree $o2ctrl_obj   Related o2_ctrl_tree object
     */
    static function ctrl_tree($o2ctrl_obj) {

        $app = $_SESSION['o2_app'];
        $act = '';
        // ______________________________________________________ If no focus node set ___
        if (!$o2ctrl_obj->focus_node) {
            $act = 'tabindex="0" onKeyDown="o2jse.tv.k(event, true);" ';
            }
        $ctrlId  = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        $scrollF = '<input type="hidden" name="'.$ctrlId.'_vscroll">';
        if ($o2ctrl_obj->vscroll || $o2ctrl_obj->scripting) {
            $js = "<script>o2jse.tv.set('".$ctrlId."',".
                                           $o2ctrl_obj->vscroll.');</script>';
            }
        else {
            $js = '';
            }
        if ($o2ctrl_obj->scripting) {
            $style = ' style="width:100%;height:100%"';
            $focus = '';
            $jsp   = "o2=\"e:".$o2ctrl_obj->id_esecuzione.
                         ",c:'".$o2ctrl_obj->nome.
                         "',std:!1,sorted:".($o2ctrl_obj->sorted ? "!0" : "!1").
                         ",aL:".($o2ctrl_obj->act_level).",cT:'tree',pT:'html'\"\n";
            // _____________________________________ Focus has been set by last action ___
            if ($_REQUEST['jxtvlastctrl'] == $o2ctrl_obj->nome) {
                // ________________________________ Override standard focus management ___
                unset($app->caret_position[$app->progressivo_istanze]);
                $focus = "<script>(fC = document.getElementById('".$o2ctrl_obj->nome.
                         "_jxFocusNode')) && fC.focus();</script>";
                }
            return '<div id="'.$ctrlId.'" '.$act.$jsp.$style.'>'.
                   $o2ctrl_obj->get_node_code(0, -1).'</div>'.$focus.$scrollF.$js;
            }
        else {
            $value = $o2ctrl_obj->sql2ctrl();
            if ($o2ctrl_obj->sele_node != $value) {
                $o2ctrl_obj->node_select($value, true);
                }
            $jsp = $o2ctrl_obj->js_props('std:!0,sorted:'.
                                         ($o2ctrl_obj->sorted ? "!0" : "!1").
                                         ',aL:'.($o2ctrl_obj->act_level));
            print '<div'.($o2ctrl_obj->active_by_tab ?
                          ' id="'.$ctrlId.'"' : '').
                       ' class="'.$o2ctrl_obj->css.
                      '" style="width:'.$o2ctrl_obj->larghezza.
                            'px;height:'.$o2ctrl_obj->altezza.'px;'.
                         ($o2ctrl_obj->visibile ? '' : 'display:none;').
                  '" onscroll="o2jse.lu.listOff();document.forms.o2form[\''.
                  $ctrlId.'_vscroll\'].value=this.scrollTop;" '.$act.$jsp.'>'.
                  $o2ctrl_obj->get_node_code(0, -1).'</div>'.$scrollF.$js;
            }

        }


    /**
     * Return HTML code for an images lister interface
     *
     * @param o2_ctrl_imglist $o2ctrl_obj   Related o2_ctrl_imglist object
     */
    static function ctrl_imglist($o2ctrl_obj) {

        $ctrlId  = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
        $scrollF = '<input type="hidden" name="'.$ctrlId.'_vscroll">';
        if ($o2ctrl_obj->vscroll) {
            $js = "<script>o2jse.il.set('".$ctrlId."',".
                                           $o2ctrl_obj->vscroll.');</script>';
            }
        else {
            $js = '';
            }
        if ($o2ctrl_obj->scripting) {
            $app    = $_SESSION['o2_app'];
            $jsp    = 'o2="e:'.$o2ctrl_obj->id_esecuzione.
                         ",c:'".$o2ctrl_obj->nome."',cT:'imglist',std:!1,pT:'html'\"\n";
            $style = ' style="width:100%;height:100%"';
            $act   = '';
            // _____________________________________ Focus has been set by last action ___
            return "<div ".$act.$jsp.$style.">".$o2ctrl_obj->get_code()."</div>".
                   $scrollF.$js;
            }
        else {
            $jsp = $o2ctrl_obj->js_props('std:!0');
            print '<div'.($o2ctrl_obj->active_by_tab ?
                          ' id="'.$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.'"' : '').
                       ' class="'.$o2ctrl_obj->css.
                      '" style="width:'.$o2ctrl_obj->larghezza.
                            'px;height:'.$o2ctrl_obj->altezza.'px;'.
                         ($o2ctrl_obj->visibile ? '' : 'display:none;').
                         '" onscroll="o2jse.lu.listOff();document.forms.o2form[\''.
                            $ctrlId.'_vscroll\'].value=this.scrollTop;"'.$jsp.'>'.
                  $o2ctrl_obj->get_code().'</div>'.$scrollF.$js;
            }

        }


    /**
     * Return HTML code for a progress-bar interface
     *
     * @param o2_ctrl_progress $o2ctrl_obj   Related o2_ctrl_progress object
     */
    static function ctrl_progress($o2ctrl_obj) {

        if ($o2ctrl_obj->scripting) {
            return $o2ctrl_obj->get_code();
            }
        else {
            $app  = $_SESSION['o2_app'];
            $prg  = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
            $form = $prg->form[$o2ctrl_obj->form];
            // _________________________ Progress-bar control in table: linked to view ___
            if ($o2ctrl_obj->padre &&
                $form->controlli[$o2ctrl_obj->padre]->tipo_ctrl == 'tab') {
                $view_name = $form->controlli[$o2ctrl_obj->padre]->task;
                $view_obj  = $prg->contesto[$view_name];
                $o2ctrl_obj->prepare_view($view_obj);
                $id  = $o2ctrl_obj->nome.$view_obj->corrente['JXPBI'].
                       $o2ctrl_obj->id_esecuzione;
                $val = $o2ctrl_obj->set_value($view_obj->corrente['JXPBV'],
                                              $view_obj->corrente['JXPBI']);
                }
            // ___________________________________________ Single progress-bar control ___
            else {
                $id  = $o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione;
                $val = $o2ctrl_obj->valore;
                }
            $act = ($o2ctrl_obj->modificabile && $val < 100 ?
                    "<script> o2jse.progress.start(); </script>\n" : "");
            $jsp = $o2ctrl_obj->js_props('std:!0');
            print '<div id="'.$id.'" class="'.
                  $o2ctrl_obj->css_out.($o2ctrl_obj->modificabile ? "" : "_dis").
                  '" style="width:'.$o2ctrl_obj->larghezza.
                        'px;height:'.$o2ctrl_obj->altezza.'px;'.
                  ($o2ctrl_obj->visibile ? '' : 'display:none;').'" '.$jsp.
                  '><div class="'.$o2ctrl_obj->css_bar.'" style="width:'.$val.
                  '%">&nbsp;</div><div class="'.$o2ctrl_obj->css_txt.'">'.$val.
                  "%</div></div>".$act;
            }

        }

    }

?>
