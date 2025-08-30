<?php

/**
 * Janox JavaScript Based Controls Module
 * PHP7/8 - HTML4.01 - JavaScript1.2
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
 * This script contains Janox HTML controls built by JavaScript definition.
 *
 *
 * @name      jxjs
 * @package   janox/jxjs.inc
 * @version   3.0
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007-2025
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * jxjs static methods collection
 *
 */
class jxjs {


    /**
     * Start controls definition in full AJAX WEB 2.0 context
     *
     * @param integer $reqid   Request ID to check against JS context
     */
    static function start($reqid) {

        $app = $_SESSION['o2_app'];
        // _______________________________________________________ Set response header ___
        header("Content-type: text/javascript; charset=".$app->chr_encoding);
        // __________________________________________________ Manage open windows list ___
        if (count($app->open_wins)) {
            foreach ($app->open_wins as $prg_id => $prg_wins) {
                foreach ($prg_wins as $win_name => $win_state) {
                    if ($win_state['V']) {
                        $app->open_wins[$prg_id][$win_name]['V'] = false;
                        }
                    elseif ($prg_id < $app->progressivo_istanze) {
                        $app->open_wins[$prg_id][$win_name]['A'] = false;
                        }
                    else {
                        unset($app->open_wins[$prg_id][$win_name]);
                        }
                    }
                }
            }
        // ______________________________________________________ Open response syntax ___
        print "if(jxi(".$reqid.")){\n".
              ($app->run_level == "DEV" ? $app->reqdata_list() : "");

        }


    /**
     * End controls definition in full AJAX WEB 2.0 context
     *
     * @param boolean $from_exception   If method is called from within exception handler
     *                                  (no execution)
     */
    static function end($from_exception = false) {

        $app   = $_SESSION['o2_app'];
        $exeid = $app->progressivo_istanze;
        // ___________________________________________________________ Custom response ___
        if ($app->custom_response) {
            $resp = ob_get_clean();
            header("Content-type: text/plain; charset=". $app->chr_encoding);
            print substr($resp, 0, $app->custom_response);
            $app->custom_response = false;
            return;
            }
        // __________________________________________________ Manage open windows list ___
        if (is_array($app->open_wins) && count($app->open_wins)) {
            $last_id = max(array_keys($app->open_wins));
            }
        else {
            $last_id = 0;
            }
        if ($exeid != $last_id) {
            // ________________________________________ Replace the whole page content ___
            if (!$from_exception) {
                jxjs::page();
                }
            }
        elseif (count($app->open_wins)) {
            $prg_wins = $app->istanze_prg[$last_id]->form;
            foreach ($app->open_wins[$last_id] as $win_name => $win_state) {
                if (!$win_state['V']) {
                    $win_obj = (isset($prg_wins[$win_name]) ?
                                $prg_wins[$win_name] : false);
                    if ($win_obj && $win_obj->linkto) {
                        $parent_win = $prg_wins[$win_obj->linkto];
                        $link_id    = 0;
                        foreach ($parent_win->linked as $linked_form_name => $dummy) {
                            $link_id++;
                            if ($linked_form_name == $win_name) {
                                break;
                                }
                            }
                        $div_name = $win_obj->linkto."_".$win_obj->id_esecuzione.$link_id;
                        jxjs::win_close($win_name, $div_name);
                        }
                    else {
                        jxjs::win_close($win_name);
                        }
                    }
                }
            }
        // _________________________________________________ Manage JS cached commands ___
        if ($app->js_cache) {
            print $app->js_cache;
            $app->js_cache = "";
            }
        // _________________________________ Set focus (preserved when forcing-reload) ___
        if (isset($app->caret_position[$exeid])) {
            // ____________________________________________________ Focus natural flow ___
            if ($app->caret_position[$exeid]["int"] && isset($_REQUEST['jxnofocus'])) {
                $caret = o2html::get_caret(true);
                if ($caret) {
                    $cl = explode(",", $caret);
                    print "o2jse.ctrl.focusFallBack=".reset($cl).";\n";
                    }
                }
            else {
                print "o2jse.cmd.focus(".o2html::get_caret(true).");\n";
                }
            }
        // ___________________________________ Set exe-tree informations for developer ___
        if ($app->run_level == "DEV") {
            print $app->exe_tree()."\n";
            }
        // _________________________________________ Set actual exe-ID and program name___
        $prg = ($exeid ? $app->istanze_prg[$exeid]->nome : '');
        print 'o2jse.setExeId('.$exeid.",'".$prg."');\n}\n";

        }


    /**
     * Replace the whole page content in FULL-AJAX WEB 2.0 context
     *
     * @param string $action
     */
    static function page($action = "") {

        $app = $_SESSION['o2_app'];
        // ____________________________________________ Replace the whole page content ___
        ob_clean();
        $GLOBALS['jxjs']       = false;
        $GLOBALS['jxjsorigin'] = true;
        if ($action) {
            $_REQUEST = array("o2_action" => $action);
            }
        else {
            $_REQUEST = array();
            }
        o2html::page();
        $code = ob_get_clean();
        // ______________________________________ Replace the inner HTML of <BODY> tag ___
        print "if(o2jse.jxRefresh){ clearTimeout(o2jse.jxRefresh); }\njxp(".
              jxjs::html_embed($code.(isset($GLOBALS['jxlasterror']) ?
                                      "\n".$GLOBALS['jxlasterror']->to_html() : '')).
              ");";
        $app->commit_all(true);
        die();

        }


    /**
     * Stop JS info and force client submit (refresh) in full AJAX WEB 2.0 context
     *
     */
    static function force_reload() {

        $app            = $_SESSION['o2_app'];
        $app->open_wins = array();
        $prgexe_id      = $app->progressivo_istanze;
        $prg            = $app->istanze_prg[$prgexe_id];
        if (is_array($prg->contesto)) {
            foreach ($prg->contesto as $view) {
                if (is_a($view, 'o2_dbview')) {
                    $sele_name = $view->nome.'_vs'.$prgexe_id;
                    print "o2jse.infoForm['".$sele_name."'].value=0;\n";
                    }
                }
            }
        print "o2jse.infoForm['o2lastform'].value='';".
              "o2jse.infoForm['o2lastctrl'].value='';".
              "o2jse.infoForm['o2_action'].value='';".
              "o2jse.submitting=false;".
              "o2jse.reposWins=[];jxjs.scriptsList=[];\no2jse.cmd.submit(".$prgexe_id.
              ");\n";

        }


    /**
     * Embed HTML code in JavaScript response in full AJAX WEB 2.0 context
     *
     */
    static function html_embed($code) {

        return "'".addcslashes($code, "'\\\r\n")."'";

        }


    /**
     * Define standard control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl $o2ctrl_obj   Janox control object related to control
     * @param string  $specific     Control specific definition items
     * @param string  $o2prop       Control properties in the form of "o2" attribute
     */
    static function ctrl($o2ctrl_obj, $specific = "", $o2prop = "") {

        return "jxc({i:'".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.
               "',t:'".$o2ctrl_obj->tipo_ctrl.
               "',v:".($o2ctrl_obj->visibile ? "!0" : "!1").
               ",e:".($o2ctrl_obj->is_enabled() ? "!0" : "!1").
               ($o2ctrl_obj->tab_index ? ",r:".$o2ctrl_obj->tab_index : "").
               ($specific ? ",".$specific : "").
               ($o2prop ? ",p:".$o2prop : "")."});\n";

        }


    /**
     * Define label control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_label $o2ctrl_obj   Janox label control object related to control
     */
    static function ctrl_label($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
            $app    = $_SESSION['o2_app'];
            $form   = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->form[$o2ctrl_obj->form];
            $parent = "pT:'".($o2ctrl_obj->padre ?
                            $form->controlli[$o2ctrl_obj->padre]->tipo_ctrl :
                            'form')."'";
            // _________________________ Labels are used for fields in disabled grids! ___
            $exeid  = ($o2ctrl_obj->popup_exp || $o2ctrl_obj->log_level ?
                       ',e:'.$o2ctrl_obj->id_esecuzione : '');
            $popup  = ($o2ctrl_obj->popup_exp ?
                       ",puact:'".$o2ctrl_obj->popup_act.
                       "',puexp:'".$o2ctrl_obj->popup_exp."'" : '');
            $log    = ($o2ctrl_obj->log_level ?
                       ",log:!0,c:'".$o2ctrl_obj->nome."'"  : '');
            if ($app->multilang) {
                $label = $app->translate($o2ctrl_obj->nome,
                                         'label',
                                         0,
                                         $o2ctrl_obj->label);
                }
            else {
                $label = $o2ctrl_obj->label;
                }
            print jxjs::ctrl($o2ctrl_obj,
                             "l:'".str_replace(array("\n","\r"), ' ',
                                               addslashes(jx_encode($label))).
                             "',s:'".$o2ctrl_obj->css.
                             "'".(!$o2ctrl_obj->tab_index ?
                                  ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                             ',w:'.$o2ctrl_obj->larghezza.
                             ',h:'.$o2ctrl_obj->altezza,
                             '{'.$parent.$exeid.$popup.$log.'}');
            }
        }


    /**
     * Define line control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_line $o2ctrl_obj   Janox line control object related to control
     */
    static function ctrl_line($o2ctrl_obj) {

        print jxjs::ctrl($o2ctrl_obj, "");

        }


    /**
     * Define navigator control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_nav $o2ctrl_obj   Janox navigator control object related to control
     */
    static function ctrl_nav($o2ctrl_obj) {

        $code   = "''";
        $parent = '{}';
        if ($o2ctrl_obj->visibile) {
            $app    = $_SESSION['o2_app'];
            $form   = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->
                                                                  form[$o2ctrl_obj->form];
            $parent = "{pT:'".($o2ctrl_obj->padre ?
                               $form->controlli[$o2ctrl_obj->padre]->tipo_ctrl :
                               'form')."'}";
            ob_start();
            $GLOBALS['jxjs'] = false;
            o2html::ctrl_nav($o2ctrl_obj);
            $code            = jxjs::html_embed(ob_get_clean());
            $GLOBALS['jxjs'] = true;
            }
        print jxjs::ctrl($o2ctrl_obj,
                         'code:'.$code.
                         ',x:'.$o2ctrl_obj->x.
                         ',y:'.$o2ctrl_obj->y.
                         ',w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza,
                         $parent);

        }


    /**
     * Define edit control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_edit $o2ctrl_obj   Janox edit control object related to control
     */
    static function ctrl_edit($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
            $enabled = $o2ctrl_obj->is_enabled();
            $zoom    = $o2ctrl_obj->is_zoomable();
            print jxjs::ctrl($o2ctrl_obj,
                             "d:'".$o2ctrl_obj->sql2ctrl().
                             "',a:'".($o2ctrl_obj->maschera->allineamento == 'Right' ?
                                      'right' : 'left').
                             "',s:'".$o2ctrl_obj->css.($enabled ? '' : '_dis').
                                                      ($zoom ? ' jxzoom' : '').
                                                      ($o2ctrl_obj->forza_submit &&
                                                       $enabled ? ' jxsoc' : '').
                             "'".(!$o2ctrl_obj->tab_index ?
                                  ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                             ',w:'.$o2ctrl_obj->larghezza.
                             ',h:'.$o2ctrl_obj->altezza,
                             $o2ctrl_obj->js_props(($zoom ? "z:!0" : "z:!1"), true));
            }

        }


    /**
     * Define text-area control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_text $o2ctrl_obj   Janox text-area control object related to control
     */
    static function ctrl_text($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
            $enabled = $o2ctrl_obj->is_enabled();
            $zoom    = $o2ctrl_obj->is_zoomable();
            if ($o2ctrl_obj->html) {
                $css = $o2ctrl_obj->css."_edit";
                $rte = "rte:!0,";
                }
            else {
                $css = $o2ctrl_obj->css;
                $rte = "";
                }
            print jxjs::ctrl($o2ctrl_obj,
                            "text:'".$o2ctrl_obj->sql2ctrl().
                            "',s:'".$o2ctrl_obj->css.($enabled ?"" : "_dis").
                                                    ($zoom ? ' jxzoom' : '').
                                                    ($o2ctrl_obj->forza_submit &&
                                                    $enabled ? ' jxsoc' : '').
                            "'".(!$o2ctrl_obj->tab_index ?
                                ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                            ',w:'.$o2ctrl_obj->larghezza.
                            ',h:'.$o2ctrl_obj->altezza,
                            $o2ctrl_obj->js_props("cssc:'".$css."',".$rte.
                                                ($zoom ? "z:!0" : "z:!1"),
                                                true));
            }

        }


    /**
     * Define checkbox control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_check $o2ctrl_obj   Janox checkbox control object related to control
     */
    static function ctrl_check($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
            if ($o2ctrl_obj->is_enabled()) {
                if ($o2ctrl_obj->padre) {
                    $app     = $_SESSION['o2_app'];
                    $prg     = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
                    $parent  = $prg->form[$o2ctrl_obj->form]->
                                controlli[$o2ctrl_obj->padre];
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
                       ($o2ctrl_obj->tab_row ? '!1' : '!0') : '!1');
            print "jxc({i:'".$o2ctrl_obj->nome.$o2ctrl_obj->id_esecuzione.
                  "',t:'checkbox',v:".($o2ctrl_obj->visibile ? "!0" : "!1").
                  ",e:".($active ? "!0" : "!1").
                  ($o2ctrl_obj->tab_index ? ",r:".$o2ctrl_obj->tab_index : "").
                  ",vS:".($submit).
                  ",d:".($o2ctrl_obj->sql2ctrl() ? "!0" : "!1").
                  ",s:'".$o2ctrl_obj->css.
                  "'".(!$o2ctrl_obj->tab_index ?
                      ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                  ",p:".$o2ctrl_obj->js_props("", true)."});\n";
            }

        }


    /**
     * Define list and combo box controls in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_listcombo $o2ctrl_obj   Janox list/combo-box control object related
     *                                        to control
     */
    static function ctrl_listcombo($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
            $app            = $_SESSION['o2_app'];
            $prg            = $app->istanze_prg[$o2ctrl_obj->id_esecuzione];
            $enabled_in_tab = $o2ctrl_obj->active_by_tab;
            $enabled        = $o2ctrl_obj->is_enabled();
            $active         = $o2ctrl_obj->is_active();
            $zoom           = $o2ctrl_obj->is_zoomable();
            $dyn_list       = '!1';
            $static_list    = '';
            $code_value     = '';
            // __________________________________________________________ Lookup combo ___
            if ($o2ctrl_obj->righe == 1) {
                $code_value = $o2ctrl_obj->ctrl2sql();
                $desc_value = ($o2ctrl_obj->visibile ? $o2ctrl_obj->get_desc() : "");
                if ($o2ctrl_obj->items_view) {
                    $dyn_list = "!0";
                    $rows     = $prg->contesto[$o2ctrl_obj->items_view]->righe_vis;
                    }
                else {
                    // ____________________________ Enabled and active (loop on items) ___
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
                            $static_list[] = "['".addslashes($codice)."','".
                                                  addslashes($descrizione)."']";
                            if ((string) $code_value === trim($codice)) {
                                $esiste_valore = true;
                                }
                            }
                        $static_list = ",items:[".implode(",", $static_list)."]";
                        // ________________________________ Set default value in field ___
                        if ($o2ctrl_obj->visibile && !$esiste_valore && $enabled) {
                            $prg->contesto[$o2ctrl_obj->task]->
                                            imposta($o2ctrl_obj->campo_in_ctrl,
                                                    $cod_default);
                            $desc_value = addslashes($desc_default);
                            }
                        }
                    $rows = count($o2ctrl_obj->valori);
                    }
                print jxjs::ctrl($o2ctrl_obj,
                                 "d:'".addcslashes($code_value, "'\\\n\r").
                               "',dyn:".$dyn_list.$static_list.
                                ",desc:'".$desc_value.
                               "',boxtype:'lookup',s:'".
                                 $o2ctrl_obj->css.($enabled ? "" : "_dis").
                               "',er:".($o2ctrl_obj->error ? "!0" : "!1").
                                 (!$o2ctrl_obj->tab_index ?
                                  ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                                ',w:'.$o2ctrl_obj->larghezza.
                                ',h:'.$o2ctrl_obj->altezza.
                                 ($app->option_get('lookup_handler') ? ',o:!0' : ',o:!1'),
                            $o2ctrl_obj->js_props("cssf:'".$o2ctrl_obj->css.
                                                  ($enabled ? '_field' : '_field_dis').
                                                  ($zoom ? ' jxzoom' : '').
                                                  ($o2ctrl_obj->forza_submit && $enabled ?
                                                   ' jxsoc' : '').
                                                  ($o2ctrl_obj->error ? ' jxerror' : '').
                                                  "',cssl:'".$o2ctrl_obj->css.
                                                  "_list',rows:".($rows ? $rows : "0").
                                                  ($zoom ? ",z:!0" : ",z:!1"),
                                                  true));
                }
            // ______________________________________________________________ List-box ___
            else {
                $mask = $o2ctrl_obj->maschera;
                // __________________________________________ Values list from picture ___
                if (!count($o2ctrl_obj->valori) && is_array($mask->valori)) {
                    $o2ctrl_obj->valori = $mask->valori;
                    }
                $cod_default       = false;
                $items             = "";
                $o2ctrl_obj->error = true;
                foreach ($o2ctrl_obj->valori as $codice => $descrizione) {
                    if ($app->multilang) {
                        $descrizione = $app->translate($o2ctrl_obj->nome,
                                                       'value',
                                                       $codice,
                                                       $descrizione);
                        }
                    $cod_default = ($cod_default !== false ? $cod_default : $codice);
                    $v_local     = substr($o2ctrl_obj->ctrl2sql(), 0, $mask->dimensione);
                    $items      .= "['".addslashes(trim($codice))."','".
                                        addcslashes($descrizione, "'\\\n\r\"")."'],";
                    if (trim($v_local) === trim($codice)) {
                        $code_value        = $codice;
                        $o2ctrl_obj->error = false;
                        }
                    }
                print jxjs::ctrl($o2ctrl_obj,
                                 "d:'".$code_value.
                               "',boxtype:'list',s:'".
                                  $o2ctrl_obj->css.($enabled ? '' : '_dis').
                                                   ($zoom ? ' jxzoom' : '').
                                                   ($o2ctrl_obj->forza_submit &&
                                                    $enabled ? ' jxsoc' : '').
                               "',items:[".($items ? substr($items, 0, -1) : '').
                                       '],x:'.$o2ctrl_obj->x.
                                        ',y:'.$o2ctrl_obj->y.
                                        ',w:'.$o2ctrl_obj->larghezza.
                                        ',h:'.$o2ctrl_obj->altezza,
                                 $o2ctrl_obj->js_props("boxtype:'list',".
                                                       ($zoom ? 'z:!0' : 'z:!1'), true));
                }
            }

        }


    /**
     * Define button control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_button $o2ctrl_obj   Janox button control object related to control
     */
    static function ctrl_button($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
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
            // _____________________________ Keyboard short-cuts are defined only once ___
            if ($o2ctrl_obj->tab_row) {
                $o2ctrl_obj->forza_submit = false;
                $label                    = str_replace('&', '', $label);
                }
            else {
                $o2ctrl_obj->forza_submit = true;
                // ___________________________________ Modify label with short-cuts set___
                if (($key_pos = strpos($label, '&')) !== false) {
                    $key_letter = trim(substr($label, $key_pos + 1, 1));
                    if ($key_letter) {
                        $label = substr($label, 0, $key_pos).'<u>'.$key_letter.'</u>'.
                                 substr($label, $key_pos + 2);
                        }
                    }
                }
            if (strpos($o2ctrl_obj->css, ' ') !== false) {
                list($hover_before, $hover_after) = explode(' ', $o2ctrl_obj->css, 2);
                }
            else {
                $hover_before = $o2ctrl_obj->css;
                $hover_after  = '';
                }
            print jxjs::ctrl($o2ctrl_obj,
                             "l:'".addslashes(jx_encode($label)).
                             "',s:'".$hover_before.($o2ctrl_obj->modificabile ?
                                                    '' : '_dis').
                                     ($hover_after ? ' '.$hover_after : '').
                             "'".(!$o2ctrl_obj->tab_index ?
                                  ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                             ',w:'.$o2ctrl_obj->larghezza.
                             ',h:'.$o2ctrl_obj->altezza,
                             $o2ctrl_obj->js_props("action:'".
                                                   $o2ctrl_obj->azione_submit."'", true));
            }

        }


    /**
     * Define file-upload control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_file $o2ctrl_obj   Janox file-upload control object related to
     *                                   control
     */
    static function ctrl_file($o2ctrl_obj) {

        $app    = $_SESSION['o2_app'];
        $form   = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->form[$o2ctrl_obj->form];
        $parent = "{pT:'".($o2ctrl_obj->padre ?
                           $form->controlli[$o2ctrl_obj->padre]->tipo_ctrl :
                           'form')."'}";
        print jxjs::ctrl($o2ctrl_obj,
                         'x:'.$o2ctrl_obj->x.
                        ',y:'.$o2ctrl_obj->y.
                        ',w:'.$o2ctrl_obj->larghezza.
                        ',h:'.$o2ctrl_obj->altezza,
                        $parent);

        }


    /**
     * Define image control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_image $o2ctrl_obj   Janox image control object related to control
     */
    static function ctrl_image($o2ctrl_obj) {

        if ($o2ctrl_obj->check_dependences()) {
            $app = $_SESSION['o2_app'];
            $on  = false;
            if ($o2ctrl_obj->is_enabled()) {
                $o2ctrl_obj->forza_submit = !$o2ctrl_obj->tab_row;
                $on                       = ($o2ctrl_obj->azione_submit ? true : false);
                }
            if (!$on) {
                $form   = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->
                                                                  form[$o2ctrl_obj->form];
                $parent = "{pT:'".($o2ctrl_obj->padre ?
                                   $form->controlli[$o2ctrl_obj->padre]->tipo_ctrl :
                                   'form')."'}";
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
            print jxjs::ctrl($o2ctrl_obj,
                             "l:'".addcslashes(jx_encode($label), "\"'\n\r\\").
                             "',src:'".$o2ctrl_obj->src().
                             "',srcover:'".$o2ctrl_obj->srcover().
                             "',s:'".$o2ctrl_obj->css.
                             "',e:".($on ? '!0' : '!1').
                             (!$o2ctrl_obj->tab_index ?
                             ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                             ',w:'.$o2ctrl_obj->larghezza.
                             ',h:'.$o2ctrl_obj->altezza,
                             ($on ?
                              $o2ctrl_obj->js_props("action:'".$o2ctrl_obj->azione_submit.
                                                    "'", true) :
                              $parent));
            }

        }


    /**
     * Define multipage control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_multipage $o2ctrl_obj   Janox multipage control object related to
     *                                        control
     */
    static function ctrl_multipage($o2ctrl_obj) {

        $value     = $o2ctrl_obj->sql2ctrl();
        $prg_local = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form_obj  = $prg_local->form[$o2ctrl_obj->form];
        $reselect  = true;
        $mod_ctrls = (isset($_REQUEST['o2_modfields']) ?
                      explode(";", $_REQUEST['o2_modfields']) : array());
        array_pop($mod_ctrls);
        $o2ctrl_obj->pagine = count($o2ctrl_obj->label);
        if ($o2ctrl_obj->pagine > 1) {
            foreach ($mod_ctrls as $ctrl) {
                // ____________________ Selection on multipage (preserve client focus) ___
                if ($ctrl == $o2ctrl_obj->nome) {
                    $reselect = false;
                    break;
                    }
                }
            }
        else {
            $reselect = false;
            }
        foreach ($o2ctrl_obj->label as $label_index => $single_label) {
            // ___________________________________________________________ Active page ___
            if (intval($o2ctrl_obj->valore) == $label_index) {
                // _______________________________________ If HTML code has to be sent ___
                $send_all = !isset($o2ctrl_obj->sent_pages[$label_index]);
                if ($send_all) {
                    ob_start();
                    $GLOBALS['jxjs'] = false;
                    }
                foreach ($form_obj->controlli as &$singolo_ctrl) {
                        // _____________________________________ Controls in multipage ___
                        if ($singolo_ctrl->padre == $o2ctrl_obj->nome &&
                            $singolo_ctrl->info_padre[0] == $label_index) {
                            // ___________________________________ Read-only multipage ___
                            if (!$o2ctrl_obj->modificabile) {
                                switch ($singolo_ctrl->tipo_ctrl) {
                                    case "tab":
                                        $singolo_ctrl->modificabile = false;
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
                            if ($send_all) {
                                print '<div style="position:absolute;left:'.
                                      $singolo_ctrl->x.'px;top:'.$singolo_ctrl->y.
                                      "px;padding:0;margin:0;white-space:nowrap;\">\n";
                                }
                            $singolo_ctrl->disegna();
                            if ($send_all) {
                                print "</div>\n";
                                }
                            }
                        }
                if ($send_all) {
                    $code            = jxjs::html_embed(ob_get_clean());
                    $GLOBALS['jxjs'] = true;
                    $o2ctrl_obj->sent_pages[$label_index] = true;
                    }
                else {
                    $code = "!1";
                    }
                // _________________________________ Only first time HTML code is sent ___
                print jxjs::ctrl($o2ctrl_obj,
                                 'd:'.($value ? $value : '0').
                                 ",select:".($reselect ? '!0' : '!1').
                                 ",ctrlName:'".$o2ctrl_obj->nome.
                                 "',cssOn:'".$o2ctrl_obj->css_label_on.
                                 "',cssOff:'".$o2ctrl_obj->css_label_off.
                                 "',code:".$code.
                                 ',x:'.$o2ctrl_obj->x.
                                 ',y:'.$o2ctrl_obj->y.
                                 ',w:'.$o2ctrl_obj->larghezza.
                                 ',h:'.$o2ctrl_obj->altezza,
                                 $o2ctrl_obj->js_props("", true));
                }
            // ______________________________________________________ Not active pages ___
            elseif (isset($o2ctrl_obj->sent_pages[$label_index]) &&
                    $o2ctrl_obj->sent_pages[$label_index]) {
                foreach ($form_obj->controlli as $ctrl) {
                    // _________________________________________ Controls in multipage ___
                    if ($ctrl->padre == $o2ctrl_obj->nome &&
                        $ctrl->info_padre[0] == $label_index &&
                        $ctrl->check_dependences()) {
                        // ____________________ Resend page next time, to show changes ___
                        unset($o2ctrl_obj->sent_pages[$label_index]);
                        break;
                        }
                    }
                }
            }

        }


    /**
     * Define flowbox control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_flowbox $o2ctrl_obj   Janox flowbox control object related to
     *                                      control
     */
    static function ctrl_flowbox($o2ctrl_obj) {

        $prg_local = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form_obj  = $prg_local->form[$o2ctrl_obj->form];
        foreach ($form_obj->controlli as &$singolo_ctrl) {
            // ___________________________________________________ Controls in flowbox ___
            if ($singolo_ctrl->padre == $o2ctrl_obj->nome) {
                $singolo_ctrl->disegna();
                }
            }
        print jxjs::ctrl($o2ctrl_obj,
                         'x:'.$o2ctrl_obj->x.
                         ',y:'.$o2ctrl_obj->y.
                         ',w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza.
                         ",s:'".$o2ctrl_obj->css."'",
                         $o2ctrl_obj->js_props("", true));

        }


    /**
     * Define frame control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_frame $o2ctrl_obj   Janox frame control object related to control
     */
    static function ctrl_frame($o2ctrl_obj) {

        $prg_local = $_SESSION['o2_app']->istanze_prg[$o2ctrl_obj->id_esecuzione];
        $form_obj  = $prg_local->form[$o2ctrl_obj->form];
        foreach ($form_obj->controlli as &$singolo_ctrl) {
            // _____________________________________________________ Controls in frame ___
            if ($singolo_ctrl->padre == $o2ctrl_obj->nome) {
                // ___________________________________________________ Read-only frame ___
                if (!$o2ctrl_obj->modificabile) {
                    switch ($singolo_ctrl->tipo_ctrl) {
                        case "tab":
                            $singolo_ctrl->modificabile = false;
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
                $singolo_ctrl->disegna();
                }
            }
        print jxjs::ctrl($o2ctrl_obj,
                         'w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza.
                         ",s:'".$o2ctrl_obj->css."'",
                         $o2ctrl_obj->js_props("", true));

        }


    /**
     * Define area control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_area $o2ctrl_obj   Janox area control object related to control
     */
    static function ctrl_area($o2ctrl_obj) {

        $app   = $_SESSION['o2_app'];
        $form  = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->form[$o2ctrl_obj->form];
        $props = "{pT:'".($o2ctrl_obj->padre ?
                          $form->controlli[$o2ctrl_obj->padre]->tipo_ctrl :
                          'form')."',f:'".$o2ctrl_obj->form."'}";
        // _________________________________________ NOT EXPOSED! Used only internally ___
        if ($o2ctrl_obj->campo_in_ctrl) {
            $html = $o2ctrl_obj->valore;
            }
        else {
            $html = $o2ctrl_obj->html;
            }
        print jxjs::ctrl($o2ctrl_obj,
                         "code:".jxjs::html_embed($html).
                         ",s:'".$o2ctrl_obj->css.
                        "',x:".$o2ctrl_obj->x.
                         ',y:'.$o2ctrl_obj->y.
                         ',w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza,
                         $props);

        }


    /**
     * Define treeview control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_tree $o2ctrl_obj   Janox treeview object related to control
     */
    static function ctrl_tree($o2ctrl_obj) {

        $jsp = $o2ctrl_obj->js_props('std:'.($o2ctrl_obj->scripting ? '!1' : '!0').
                                     ',sorted:'.($o2ctrl_obj->sorted ? "!0" : "!1").
                                     ',aL:'.($o2ctrl_obj->act_level),
                                     true);
        $value = $o2ctrl_obj->sql2ctrl();
        if ($o2ctrl_obj->sele_node != $value) {
            $o2ctrl_obj->node_select($value, true);
            }
        $html   = $o2ctrl_obj->get_node_code(0, -1);
        print jxjs::ctrl($o2ctrl_obj,
                         'code:'.jxjs::html_embed($html).
                         ",s:'".$o2ctrl_obj->css.
                        "',x:".$o2ctrl_obj->x.
                         ',y:'.$o2ctrl_obj->y.
                         ',w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza,
                         $jsp);

        }


    /**
     * Define imglist control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_imglist $o2ctrl_obj   Janox imglist object related to control
     */
    static function ctrl_imglist($o2ctrl_obj) {

        $app    = $_SESSION['o2_app'];
        $form   = $app->istanze_prg[$o2ctrl_obj->id_esecuzione]->form[$o2ctrl_obj->form];
        $jsp    = $o2ctrl_obj->js_props('std:'.($o2ctrl_obj->scripting ? '!1' : '!0'),
                                        true);
        $html   = $o2ctrl_obj->get_code();
        print jxjs::ctrl($o2ctrl_obj,
                         "code:".jxjs::html_embed($html).
                         ",s:'".$o2ctrl_obj->css.
                        "',x:".$o2ctrl_obj->x.
                         ',y:'.$o2ctrl_obj->y.
                         ',w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza,
                         $jsp);

        }


    /**
     * Define progress-bar control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_progress $o2ctrl_obj   Janox progress-bar object related to control
     */
    static function ctrl_progress($o2ctrl_obj) {

        $val = intval($o2ctrl_obj->valore);
        $jsp = $o2ctrl_obj->js_props('std:!0', true);
        print jxjs::ctrl($o2ctrl_obj,
                         "vl:".$val.
                         ",a:".($o2ctrl_obj->modificabile ? '!0' : '!1').
                         ",s:'".$o2ctrl_obj->css_out.($o2ctrl_obj->modificabile ?
                                                      '' : '_dis').
                        "',x:".$o2ctrl_obj->x.
                         ',y:'.$o2ctrl_obj->y.
                         ',w:'.$o2ctrl_obj->larghezza.
                         ',h:'.$o2ctrl_obj->altezza,
                         $jsp);

        }


    /**
     * Define document control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_doc $o2ctrl_obj   Janox document control object related to control
     */
    static function ctrl_doc($o2ctrl_obj) {

        print jxjs::ctrl($o2ctrl_obj, "u:'".$o2ctrl_obj->path."'".
                                      (!$o2ctrl_obj->tab_index ?
                                       ',x:'.$o2ctrl_obj->x.',y:'.$o2ctrl_obj->y : '').
                                      ',w:'.$o2ctrl_obj->larghezza.
                                      ',h:'.$o2ctrl_obj->altezza);

        }


    /**
     * Define table control in full AJAX WEB 2.0 context
     *
     * @param o2_ctrl_tab $o2ctrl_obj   Janox table control object related to control
     */
    static function ctrl_tab($o2ctrl_obj) {

        // ____________________________________________ Logic to preserve client focus ___
        $app       = $_SESSION['o2_app'];
        $send_all  = $o2ctrl_obj->view_shared || $o2ctrl_obj->check_dependences();
        $tab_name  = $o2ctrl_obj->nome;
        $mod_ctrls = (isset($_REQUEST['o2_modfields']) ?
                      explode(";", $_REQUEST['o2_modfields']) : array());
        array_pop($mod_ctrls);
        foreach ($mod_ctrls as $ctrl) {
            // _________________________ Navigation on DB-Grid (preserve client focus) ___
            if ($ctrl == $tab_name) {
                $send_all = false;
                break;
                }
            if (isset($o2ctrl_obj->form_obj->controlli[$ctrl])) {
                $field_obj = $o2ctrl_obj->form_obj->controlli[$ctrl];
                // _________ Action from a control within grid (preserve client focus) ___
                if ($field_obj->padre == $tab_name &&
                    $field_obj->tipo_ctrl != 'navigator') {
                    $send_all = false;
                    break;
                    }
                }
            }
        // ______________ If table view relocated send-all (NOT preserve client focus) ___
        if ($o2ctrl_obj->view_obj->relocated) {
            $send_all                        = true;
            $o2ctrl_obj->view_obj->relocated = false;
            }
        // ___________________________ If view changed range rebuild view and send-all ___
        elseif (!$send_all && $o2ctrl_obj->view_obj->range_mod()) {
            $send_all                           = true;
            $o2ctrl_obj->view_obj->ultima_where = array();
            }
        // ___________________ DB-grid HTML code is resent (NOT preserve client focus) ___
        if ($send_all) {
            $o2ctrl_obj->view_obj->sele_by_table = false;
            $code = "''";
            if ($o2ctrl_obj->visibile) {
                ob_start();
                $GLOBALS['jxjs'] = false;
                if (isset($app->caret_position[$o2ctrl_obj->id_esecuzione]) &&
                    $app->caret_position[$o2ctrl_obj->id_esecuzione]["int"] &&
                    !isset($_REQUEST['jxnofocus'])) {
                    $caret_code        = o2html::get_caret(true);
                    $o2ctrl_obj->focus = substr($caret_code,
                                                0,
                                                strcspn($caret_code, ","));
                    }
                else {
                    $o2ctrl_obj->focus = "''";
                    }
                o2html::ctrl_tab($o2ctrl_obj);
                $code            = jxjs::html_embed(ob_get_clean());
                $GLOBALS['jxjs'] = true;
                }
            print jxjs::ctrl($o2ctrl_obj, 'code:'.$code.
                                          ",s:'".$o2ctrl_obj->spazio_class.
                                         "',x:".$o2ctrl_obj->x.
                                          ',y:'.$o2ctrl_obj->y.
                                          ',w:'.$o2ctrl_obj->larghezza.
                                          ',h:'.$o2ctrl_obj->altezza,
                                          '{}');
            }
        // ___________________________ DB-grid got user action (preserve client focus) ___
        else {
            $o2ctrl_obj->view_obj->sele_by_table = true;
            $o2ctrl_obj->riga_corrente           = true;
            // ________________ Save ->modificabile to preserve it throw redefinitions ___
            $o2ctrl_obj->save_modify = $o2ctrl_obj->modificabile;
            $o2ctrl_obj->form_obj->read_def();
            $o2ctrl_obj->modificabile = $o2ctrl_obj->save_modify;
            $tab_index                = $o2ctrl_obj->view_obj->selezione + 1;
            // ________ Open table controls list (row to match against view selection) ___
            print "if(o2jse.infoForm['".$o2ctrl_obj->view_obj->nome.'_vs'.
                                        $o2ctrl_obj->id_esecuzione."'].value==".
                      $tab_index."){\n";
            $rm_col = ($o2ctrl_obj->row_marker ? 0 : 1);
            $footer = array();
            foreach ($o2ctrl_obj->form_obj->controlli as &$ctrl) {
                // _________________________________________ Controls inside the table ___
                if ($ctrl->padre == $tab_name) {
                    if (!$o2ctrl_obj->modificabile && ($ctrl->tipo_ctrl == 'edit' ||
                                                       $ctrl->tipo_ctrl == 'text' ||
                                                       $ctrl->tipo_ctrl == 'listcombo' ||
                                                       $ctrl->tipo_ctrl == 'file')) {
                        $valore_local = ($ctrl->tipo_ctrl == 'listcombo' ?
                                         $ctrl->get_desc() :
                                         $ctrl->sql2ctrl());
                        if (trim($valore_local) == '') {
                            $valore_local = '&nbsp;';
                            }
                        elseif (($ctrl->tipo_ctrl == 'text') &&
                                (strpos($valore_local, '\n') !== false)) {
                            $valore_local = str_replace('\n', '<br>', $valore_local);
                            }
                        $l_ctrl                = new o2_ctrl_label();
                        $l_ctrl->nome          = $ctrl->nome;
                        $l_ctrl->padre         = $tab_name;
                        $l_ctrl->id_esecuzione = $ctrl->id_esecuzione;
                        $l_ctrl->form          = $ctrl->form;
                        $l_ctrl->tab_index     = $tab_index;
                        $l_ctrl->label         = stripslashes($valore_local);
                        $l_ctrl->larghezza     = $ctrl->larghezza;
                        $l_ctrl->altezza       = $ctrl->altezza;
                        $l_ctrl->css           = false;
                        $l_ctrl->log_level     = $ctrl->log_level;
                        $l_ctrl->dependences   = $ctrl->dependences;
                        $l_ctrl->disegna();
                        unset($l_ctrl);
                        }
                    else {
                        $ctrl->active_by_tab = ($o2ctrl_obj->modificabile &&
                                                $o2ctrl_obj->riga_corrente);
                        $ctrl->tab_index     = $tab_index;
                        $ctrl->disegna();
                        $ctrl->active_by_tab = true;
                        unset($ctrl->tab_index);
                        }
                    // _____________________________________________ Footer expression ___
                    if (isset($ctrl->info_padre[8]) && $ctrl->info_padre[8]) {
                        if ($app->multilang) {
                            $f = jx_encode($app->translate($ctrl->nome,
                                                        'grid_foot',
                                                        0,
                                                        $ctrl->info_padre[8]));
                            }
                        else {
                            $f = jx_encode($ctrl->info_padre[8]);
                            }
                        $footer[] = '{c:'.(intval($ctrl->info_padre[1]) - $rm_col).
                                    ',r:'.(intval($ctrl->info_padre[0]) - 1).
                                    ",l:'".$f."'}";
                        }
                    }
                }
            // _____________________________________________ Close table controls list ___
            print "}\n";
            // ________________________________ Moved from selection (resend last row) ___
            if (isset($GLOBALS['jxtablastrow']) &&
                in_array($o2ctrl_obj->nome, $mod_ctrls) &&
                ($o2ctrl_obj->id_esecuzione == $app->istanza_attiva)) {
                $o2ctrl_obj->status_open();
                $view      = $o2ctrl_obj->view_obj;
                $selection = (intval($GLOBALS['jxtablastrow']) - 1);
                unset($GLOBALS['jxtablastrow']);
                if ($selection != $view->selezione) {
                    $row_code        = "";
                    $view->selezione = $selection;
                    $view->corrente  = &$view->recordset[$selection];
                    $view->comunica_variazioni();
                    $o2ctrl_obj->form_obj->read_def();
                    $o2ctrl_obj->riga_corrente = false;
                    $css_row   = (($selection % 2 === 0) ?
                                  $o2ctrl_obj->riga_class :
                                  $o2ctrl_obj->alt_class);
                    $css_cella = $o2ctrl_obj->corpo_class;
                    $rowprop   = 'rowprop:{e:'.$o2ctrl_obj->id_esecuzione.
                                         ",c:'".$o2ctrl_obj->nome.
                                        "',v:'".$o2ctrl_obj->task.
                                        "',f:'".$o2ctrl_obj->form.
                                        "',cT:'".$o2ctrl_obj->tipo_ctrl.
                                        "',lines:".$o2ctrl_obj->lines_per_row.
                                        ",tabRow:!0,fret:!0,msg:'',cssp:'".
                                                                 $o2ctrl_obj->point_class.
                                        "',cssc:'".$css_row."'}";
                    foreach ($o2ctrl_obj->form_obj->controlli as &$ctrl) {
                        // __________________ Controls (no navigator) inside the table ___
                        if ($ctrl->padre == $tab_name &&
                            $ctrl->tipo_ctrl != 'navigator') {
                            $ctrl_code = "v:''";
                            $cssc      = '';
                            // __________________________________ CSS from parent-info ___
                            if (isset($ctrl->info_padre[5]) && $ctrl->info_padre[5]) {
                                $info_css = $ctrl->info_padre[5];
                                $cssc     = ($info_css[0] === ' ' ? $css_cella : '').
                                             $info_css;
                                }
                            else {
                                $cssc = $css_cella;
                                }
                            // ___________ Controls binding to field (edit, text, ...) ___
                            if ($ctrl->campo_in_ctrl) {
                                $ctrl->valore = $o2ctrl_obj->prg_obj->
                                                 contesto[$ctrl->task]->
                                                  campo($ctrl->campo_in_ctrl);
                                if ($ctrl->tipo_ctrl == "listcombo") {
                                    $ctrl_code = "v:'".
                                                 htmlspecialchars($ctrl->get_desc(),
                                                                  ENT_QUOTES,
                                                                  $app->chr_encoding)."'";
                                    }
                                // __________________________________________ Checkbox ___
                                elseif ($ctrl->tipo_ctrl == 'checkbox') {
                                    $ctrl_code = 'v:'.($ctrl->sql2ctrl() ? '!0' : '!1');
                                    }
                                // _________________________________________ HTML area ___
                                elseif ($ctrl->tipo_ctrl == 'html') {
                                    // _____________ NOT EXPOSED! Used only internally ___
                                    $ctrl_code = 'v:'.jxjs::html_embed($ctrl->valore);
                                    }
                                else {
                                    $ctrl_code = "v:'".
                                                 htmlspecialchars($ctrl->sql2ctrl(),
                                                                  ENT_QUOTES,
                                                                  $app->chr_encoding)."'";
                                    }
                                }
                            // _________________________________________________ Image ___
                            elseif ($ctrl->tipo_ctrl == 'img') {
                                $ctrl_code = "v:'".$ctrl->src()."'";
                                }
                            // ________________________________________________ Button ___
                            elseif ($ctrl->tipo_ctrl == 'button') {
                                $ctrl_code = "v:'".
                                             addcslashes(str_replace('&',
                                                                     '',
                                                                     $ctrl->label),
                                                         "'")."'";
                                }
                            // _________________________________________________ Label ___
                            elseif (isset($ctrl->label) && $ctrl->label) {
                                $ctrl_code = "v:'".
                                             str_replace(array("\n", "\r"), ' ',
                                                               htmlentities($ctrl->label,
                                                                            ENT_QUOTES,
                                                                     $app->chr_encoding)).
                                             "'";
                                }
                            // _____________________________________________ HTML area ___
                            elseif ($ctrl->tipo_ctrl == 'html') {
                                $ctrl_code = 'v:'.jxjs::html_embed($ctrl->html);
                                }
                            $row_code.= '{c:'.(intval($ctrl->info_padre[1]) - $rm_col).
                                        ',r:'.(intval($ctrl->info_padre[0]) - 1).
                                        ",s:'".$cssc.
                                        "',t:'".$ctrl->css.
                                                ($ctrl->modificabile ? '' : '_dis').
                                        "',".$ctrl_code."},";
                            }
                        }
                    $o2ctrl_obj->status_close();
                    print jxjs::ctrl($o2ctrl_obj,
                                     'lastRow:'.$selection.
                                     ',lines:'.$o2ctrl_obj->lines_per_row.
                                     ',rowList:['.substr($row_code, 0, -1).'],'.
                                     ($footer ? 'foot:['.implode(',', $footer).'],' : '').
                                     'x:'.$o2ctrl_obj->x.
                                     ',y:'.$o2ctrl_obj->y.
                                     ',w:'.$o2ctrl_obj->larghezza.
                                     ',h:'.$o2ctrl_obj->altezza.','.
                                     ($o2ctrl_obj->hscroll ?
                                      'hs:'.$o2ctrl_obj->hscroll.',' : '').
                                     ($o2ctrl_obj->vscroll ?
                                      'vs:'.$o2ctrl_obj->vscroll.',' : '').
                                     $rowprop,
                                     '{}');
                    }
                }
            elseif ($footer) {
                print jxjs::ctrl($o2ctrl_obj,
                                'lastRow:'.$o2ctrl_obj->view_obj->selezione.
                                ',rowList:{},rowprop:{},'.
                                'foot:['.implode(',', $footer).']',
                                '{}');
                }
            }

        }


    /**
     * Define window in full AJAX WEB 2.0 context
     *
     * @param o2_form $o2form_obj   Janox form object related to window
     * @param boolean $close        Close window, instead of sending code
     */
    static function ctrl_win($o2form_obj) {

        $app  = $_SESSION['o2_app'];
        $wins = (isset($app->open_wins[$o2form_obj->id_esecuzione]) ?
                 $app->open_wins[$o2form_obj->id_esecuzione] : false);
        // ______________________________ Get form-ID or parent form-ID (if linked to) ___
        if ($o2form_obj->linkto) {
            $prg        = $app->istanze_prg[$app->progressivo_istanze];
            $parent_win = $prg->form[$o2form_obj->linkto];
            $linkid     = 0;
            foreach ($parent_win->linked as $linked_form_name => $dummy_value) {
                $linkid++;
                if ($linked_form_name == $o2form_obj->nome) {
                    break;
                    }
                }
            $form_id = $o2form_obj->linkto."_".$o2form_obj->id_esecuzione.$linkid;
            }
        else {
            $form_id = $o2form_obj->nome."_".$o2form_obj->id_esecuzione;
            }
        // ______________________________________________________ Form is already open ___
        if (is_array($wins) &&
            array_key_exists($o2form_obj->nome, $wins) &&
            $wins[$o2form_obj->nome]['A'] == $o2form_obj->attiva) {
            $pos_buffer = '';
            // ______________________________________ Active forms and menu forms only ___
            if ($o2form_obj->attiva || $o2form_obj->menu_behavior) {
                $o2form_obj->leggi_valori_vista();
                foreach ($o2form_obj->controlli as &$single_ctrl) {
                    if (!$single_ctrl->padre) {
                        $single_ctrl->disegna();
                        }
                    }
                o2html::ctrl_win_position($o2form_obj);
                if (!$o2form_obj->linkto &&
                    ($o2form_obj->x != $o2form_obj->last_x ||
                     $o2form_obj->y != $o2form_obj->last_y)) {
                    $pos_buffer = ',x:'.$o2form_obj->absolute_x.
                                  ',y:'.$o2form_obj->absolute_y;
                    }
                }
            if ($app->multilang) {
                $title = $app->translate($o2form_obj->nome,
                                         'title',
                                         0,
                                         $o2form_obj->titolo);
                }
            else {
                $title = $o2form_obj->titolo;
                }
            print "jxc({i:'".$form_id."',t:'win',v:!0,title:'".
                  addslashes(jx_encode($title)).
                  "',w:".$o2form_obj->larghezza.
                  ",h:".max(0,
                            ($o2form_obj->altezza -
                             ($o2form_obj->title_bar && !$o2form_obj->linkto ?
                              30 : 0))).
                  $pos_buffer.
                  ",code:!1});\n";
            // ________________________________________________ Manage refresh timeout ___
            if ($o2form_obj->refresh_exp) {
                $time_local = false;
                eval("\$time_local = ".$o2form_obj->refresh_exp.";");
                if ($time_local) {
                    print "if(o2jse.jxRefresh){ clearTimeout(o2jse.jxRefresh); }\n";
                    if ($o2form_obj->attiva) {
                        print 'o2jse.jxRefresh=setTimeout(\'o2jse.cmd.exe("",'.
                              $o2form_obj->id_esecuzione.',"'.
                              $o2form_obj->refresh_action.
                              '");delete(o2jse.jxRefresh);\','.($time_local * 1000).
                              ");\n";
                        }
                    }
                }
            }
        // _______ New form in the same program or form passed from active to inactive ___
        else {
            $GLOBALS['jxjs'] = false;
            ob_start();
            o2html::ctrl_win($o2form_obj);
            $code            = ob_get_clean();
            $GLOBALS['jxjs'] = true;
            print "jxc({i:'".$form_id."',t:'win',v:!0,title:'',code:".
                  jxjs::html_embed($code)."});\n";
            }
        jxjs::win_open($o2form_obj);

        }


    /**
     * Set window open in full AJAX WEB 2.0 context
     *
     * @param o2_form $o2form_obj   Janox form object related to window
     */
    static function win_open($o2form_obj) {

        $app   = $_SESSION['o2_app'];
        $exeid = $o2form_obj->id_esecuzione;
        if (!is_array($app->open_wins[$exeid])) {
            $app->open_wins[$exeid] = array();
            }
        $app->open_wins[$exeid][$o2form_obj->nome] = array('V' => true,
                                                           'A' => $o2form_obj->attiva);

        }


    /**
     * Set window close in full AJAX WEB 2.0 context
     *
     * @param o2_form $o2form_obj   Janox form object related to window
     * @param string  $child_form   External DIV name for child (linked) forms
     */
    static function win_close($o2form_name, $child_form = false) {

        $app = $_SESSION['o2_app'];
        $id  = $app->progressivo_istanze;
        unset($app->open_wins[$id][$o2form_name]);
        if ($child_form) {
            $o2form_name = $child_form;
            }
        else {
            $o2form_name = $o2form_name."_".$id;
            }
        print "jxc({i:'".$o2form_name."',t:'win',v:!1});\n";

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
        $_SESSION['o2_app']->js_cache.= "o2jse.fastMsg.show(".
                                        jxjs::html_embed(jx_encode($msg_code)).");\n";

        }


    /**
     * Print out Javascript code to display the status-bar content
     *
     * @param string $msg_code   HTML code to display in the status-bar
     */
    static function status_msg($msg_code) {

        $_SESSION['o2_app']->js_cache.= "o2jse.statusMsg(".
                                        jxjs::html_embed(jx_encode($msg_code)).");\n";

        }

    }
?>
