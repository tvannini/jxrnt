<?php

/**
 * Janox User Interface Module
 * PHP5
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
 * This script contains all elements needed by Janox to manage windows and controls (for
 * user interface drawing see module jxhtml):
 *  .: Window object (o2_form)
 *  .: Controls object
 *
 *
 * @name      jxui
 * @package   janox/jxui.inc
 * @version   2.9
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * Form (window) object
 *
 */
class o2_form {

    /*     PROPERTIES ================================================================= */
    public $nome            = "";      /* Form name                                     */
    public $prg             = "";      /* Parent program name                           */
    public $id_esecuzione   = 0;       /* Parent program instance id (prg exe id)       */
    public $defined         = false;   /* If form _def() has ever been read             */
    public $html_name       = "";      /* Name of the HTML 'form' TAG                   */
    public $condizione      = "true";  /* String to eval to form visibility condition   */
    public $visible         = false;   /* If window is currently visible                */
    public $chiusa          = false;   /* Form closed by close button [x]               */
    public $controlli       = array(); /* Form controls list                            */
    public $attiva          = false;   /* Form active to user interaction               */
    public $linkto          = false;   /* Parent form this form is linked to            */
    public $linked          = array(); /* Linked (depending) forms list                 */
    public $url             = "";      /* URL to display (iframe) instead of controls   */
    public $action_on_close = "";      /* Action on window close button [X] clicking    */
    public $menu_behavior   = false;   /* Form is active even behind child programs     */
    public $internal        = false;   /* Form has internal definition and not "_def"   */
    public $html_cache      = "";      /* Last produced form HTML text                  */
    public $refresh_exp     = 0;       /* Returning time to elapse before next refresh  */
    public $refresh_action  = "";      /* Action to call on refresh                     */
    /*     ASPECT _____________________________________________________________________ */
    public $titolo          = "";   /* Title to appear on window title bar              */
    public $title_bar       = true; /* Title bar visibility                             */
    public $status_bar      = true; /* Status bar visibility                            */
    /*     CSS STYLE CLASSES __________________________________________________________ */
    public $css_finestra    = "o2_form_window";      /* Window external DIV             */
    public $css_title       = "o2_form_title";       /* Active title TD                 */
    public $css_title_no    = "o2_form_title_no";    /* Inactive title TD               */
    public $css_titlebar    = "o2_form_titlebar";    /* Active title bar TABLE          */
    public $css_titlebar_no = "o2_form_titlebar_no"; /* Inactive title bar TABLE        */
    public $css_body        = "o2_form_body";        /* Window internal DIV or IFRAME   */
    public $css_status      = "o2_form_status";      /* Status bar TABLE                */
    public $css_doc         = "o2_form_doc";         /* Top-left corner TD              */
    public $css_full        = "o2_form_full";        /* Maximize button TD              */
    public $css_restore     = "o2_form_restore";     /* Restore from maximized button TD*/
    public $css_open        = "o2_form_open";        /* Maximize button TD              */
    public $css_close       = "o2_form_close";       /* Minimize button TD              */
    public $css_exit        = "o2_form_exit";        /* Top-right (close button) TD     */
    public $css_corner      = "o2_form_corner";      /* Bottom-left corner TD           */
    public $css_resize      = "o2_form_resize";      /* Bottom-right (resize button) TD */
    /*     POSITION & DIMENSIONS ______________________________________________________ */
    public $x               = 0;     /*  -.                                             */
    public $y               = 0;     /*   |- Position & dimensions                      */
    public $larghezza       = 0;     /*   |                                             */
    public $altezza         = 0;     /*  -'                                             */
    public $design_width    = 0;     /* Original saved width value for resizing         */
    public $design_height   = 0;     /* Original saved height value for resizing        */
    public $last_width      = 0;     /* Last defined width value                        */
    public $last_height     = 0;     /* Last defined height value                       */
    public $restore_width   = 0;     /* Width to restore to from maximized              */
    public $restore_height  = 0;     /* Height to restore to from maximized             */
    public $align_o         = "";    /* Horizontal positioning criteria                 */
    public $align_v         = "";    /* Vertical positioning criteria                   */
    public $absolute_x      = 0;     /* Absolute x for linked forms position            */
    public $absolute_y      = 0;     /* Absolute y for linked forms position            */
    public $last_x          = 0;     /* Last defined horizontal pozition value          */
    public $last_y          = 0;     /* Last defined vertical pozition value            */
    public $restore_x       = 0;     /* X to restore to from maximized                  */
    public $restore_y       = 0;     /* Y to restore to from maximized                  */
    public $custom_pos      = false; /* If form has been positioned by user             */
    public $custom_size     = false; /* If form has been sized by user                  */
    public $maximized       = false; /* If form is actually maximized                   */
    public $force_resize    = false; /* Force resize (on controls visibility change)    */


    /**
     * Costruttore
     *
     * @param  string  $prg
     * @param  string  $id_esecuzione
     * @param  string  $nome
     * @param  string  $linkto
     * @param  boolean $menu_behavior
     * @param  string  $vis_cond
     * @param  string  $url
     * @return o2_form
     */
    function __construct($prg,
                         $id_esecuzione,
                         $nome          = "",
                         $linkto        = "",
                         $menu_behavior = false,
                         $vis_cond      = "true",
                         $url           = false) {

        $this->nome          = $nome;
        $this->prg           = $prg;
        $this->id_esecuzione = $id_esecuzione;
        $this->html_name     = $nome."_".$id_esecuzione."_html_form";
        $this->menu_behavior = $menu_behavior;
        $this->condizione    = $vis_cond;
        $this->url           = $url;
        if ($linkto) {
            $prg_local = $_SESSION['o2_app']->istanze_prg[$id_esecuzione];
            if (isset($prg_local->form[$linkto."_form"])) {
                $prg_local->form[$linkto."_form"]->linked[$this->nome] = true;
                $this->linkto = $linkto."_form";
                }
            else {
                throw new o2_exception("Cannot find form <i>".$linkto.
                                       "</i> requested as main by form <i>".
                                       $this->nome."</i>",
                                       o2error_UNKNOWNFORM);
                }
            }

        }


    function css_finestra($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_finestra = 'o2_form_window'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_finestra = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_finestra = 'o2_form_window';
            }
        return true;

        }


    function css_title($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_title = 'o2_form_title'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_title = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_title = 'o2_form_title';
            }
        return true;

        }


    function css_title_no($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_title_no = 'o2_form_title_no'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_title_no = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_title_no = 'o2_form_title_no';
            }
        return true;

        }


    function css_titlebar($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_titlebar = 'o2_form_titlebar'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_titlebar = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_titlebar = 'o2_form_titlebar';
            }
        return true;

        }


    function css_titlebar_no($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_titlebar_no = 'o2_form_titlebar_no'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_titlebar_no = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_titlebar_no = 'o2_form_titlebar_no';
            }
        return true;

        }


    function css_body($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_body = 'o2_form_body'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_body = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_body = 'o2_form_body';
            }
        return true;

        }


    function css_status($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_status = 'o2_form_status'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_status = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_status = 'o2_form_status';
            }
        return true;

        }


    function css_doc($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_doc = 'o2_form_doc'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_doc = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_doc = 'o2_form_doc';
            }
        return true;

        }


    function css_open($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_open = 'o2_form_open'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_open = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_open = 'o2_form_open';
            }
        return true;

        }


    function css_close($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_close = 'o2_form_close'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_close = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_close = 'o2_form_close';
            }
        return true;

        }


    function css_exit($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_exit = 'o2_form_exit'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_exit = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_exit = 'o2_form_exit';
            }
        return true;

        }


    function css_resize($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_resize = 'o2_form_resize'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_resize = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_resize = 'o2_form_resize';
            }
        return true;

        }


    function css_corner($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_corner = 'o2_form_corner'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_corner = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_corner = 'o2_form_corner';
            }
        return true;

        }


    function x($x = 0) {

        if (!$this->custom_pos) {
            $this->x = intval($x);
            }
        return true;

        }


    function x_exp($x = 0) {

        $x = intval($x);
        if (!$this->custom_pos || $this->last_x != $x) {
            $this->x      = $x;
            $this->last_x = $x;
            }
        return true;

        }


    function y($y = 0) {

        if (!$this->custom_pos) {
            $this->y = intval($y);
            }
        return true;

        }


    function y_exp($y = 0) {

        $y = intval($y);
        if (!$this->custom_pos || $this->last_y != $y) {
            $this->y      = $y;
            $this->last_y = $y;
            }
        return true;

        }


    function larghezza($larghezza = 0) {

        $this->design_width = intval($larghezza);
        if (!$this->custom_size) {
            $this->larghezza = intval($larghezza);
            }
        return true;

        }


    function larghezza_exp($larghezza = 0) {

        $width = intval($larghezza);
        if (!$this->custom_size || $this->last_width != $width) {
            $this->larghezza  = $width;
            $this->last_width = $width;
            }
        return true;

        }


    function altezza($altezza = 0) {

        $this->design_height = intval($altezza);
        if (!$this->custom_size) {
            $this->altezza = intval($altezza);
            }
        return true;

        }


    function altezza_exp($altezza = 0) {

        $height = intval($altezza);
        if (!$this->custom_size || $this->last_height != $height) {
            $this->altezza     = $height;
            $this->last_height = $height;
            }
        return true;

        }


    function align_o($pos = "") {

        $pos = trim(strtolower($pos));
        if (!$this->align_o && strpos("leftcenterright", $pos) !== false) {
            $this->align_o = $pos;
            }

        }


    function align_v($pos = "") {

        $pos = trim(strtolower($pos));
        if (!$this->align_v && strpos("topmiddlebottom", $pos) !== false) {
            $this->align_v = $pos;
            }

        }


    function titolo($titolo = "") {

        $this->titolo = $titolo;
        return true;

        }


    function frm_title($flag = false) {

        $this->title_bar = $flag;
        return true;

        }


    function frm_status($flag = false) {

        $this->status_bar = $flag;
        return true;

        }


    function close_action($action = "") {

        $this->action_on_close = $action;

        }


    function refresh_exp($exp) {

        $this->refresh_exp = $exp;

        }


    function refresh_action($action = "") {

        $this->refresh_action = $action;

        }


    /**
     * Determina il posizionamento del carret alla visualizzazione della form
     *
     * @param  string $padre
     * @return string
     */
    function controlli_posizione($padre = "") {

        $mpage      = false;
        $first_ctrl = false;
        // __________________________________________________ Active page in multipage ___
        if ($padre && $this->controlli[$padre]->tipo_ctrl == 'multipage') {
            $mpage = $this->controlli[$padre]->valore;
            }
        foreach ($this->controlli as $single_ctrl) {
            // _______________________________ Only controls of actual parent container___
            if ($single_ctrl->padre == $padre &&
               ($mpage === false || intval($single_ctrl->info_padre[0]) == $mpage)) {
                // __________________________________ If control is a parent container ___
                if ($single_ctrl->tipo_ctrl == 'tab') {
                    if ($single_ctrl->modificabile) {
                        $first_ctrl = $this->controlli_posizione($single_ctrl->nome);
                        }
                    else {
                        $first_ctrl = $single_ctrl->nome;
                        }
                    break;
                    }
                elseif ($single_ctrl->tipo_ctrl == 'multipage') {
                    $first_ctrl = $this->controlli_posizione($single_ctrl->nome);
                    break;
                    }
                // ___________________________ Look for first active focusable control ___
                elseif ($single_ctrl->tipo_ctrl != 'label'     &&
                        $single_ctrl->tipo_ctrl != 'img'       &&
                        $single_ctrl->tipo_ctrl != 'line'      &&
                        $single_ctrl->tipo_ctrl != 'navigator' &&
                        $single_ctrl->visibile                 &&
                        $single_ctrl->is_enabled()) {
                    $first_ctrl = $single_ctrl->nome;
                    break;
                    }
                }
            }
        return $first_ctrl;

        }


    /**
     * Display o2_form window object
     *
     * @return boolean
     */
    function vis() {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$this->id_esecuzione];
        // __________________________________________________________ Window is closed ___
        if ($this->chiusa) {
            $this->chiusa  = false;
            $this->visible = false;
            // ________________________________________ Remove focus to closed windows ___
            if ($app->caret_position[$this->id_esecuzione]['form'] == $this->nome) {
                unset($app->caret_position[$this->id_esecuzione]);
                }
            if (isset($app->open_wins[$this->id_esecuzione][$this->nome])) {
                if ($this->linkto) {
                    $parent_win = $prg->form[$this->linkto];
                    $link_id    = 0;
                    foreach ($parent_win->linked as $linked_form_name => $dummy) {
                        $link_id++;
                        if ($linked_form_name == $this->nome) {
                            break;
                            }
                        }
                    $div_name = $this->linkto."_".$this->id_esecuzione.$link_id;
                    jxjs::win_close($this->nome, $div_name);
                    }
                else {
                    jxjs::win_close($this->nome);
                    }
                }
            return true;
            }
        // ________________________________________________ Check visibility condition ___
        else {
            $cond_local = true;
            eval("\$cond_local = (".$this->condizione.");");
            // _________________________________________________ Window is NOT visible ___
            if (!$cond_local) {
                $this->chiusa  = false;
                $this->visible = false;
                // __________________________________ Remove focus from closed windows ___
                if ($app->caret_position[$this->id_esecuzione]['form'] == $this->nome) {
                    unset($app->caret_position[$this->id_esecuzione]);
                    }
                if (isset($app->open_wins[$this->id_esecuzione][$this->nome])) {
                    if ($this->linkto) {
                        $parent_win = $prg->form[$this->linkto];
                        $link_id    = 0;
                        foreach ($parent_win->linked as $linked_form_name => $dummy) {
                            $link_id++;
                            if ($linked_form_name == $this->nome) {
                                break;
                                }
                            }
                        $div_name = $this->linkto."_".$this->id_esecuzione.$link_id;
                        jxjs::win_close($this->nome, $div_name);
                        }
                    else {
                        jxjs::win_close($this->nome);
                        }
                    }
                return true;
                }
            }
        // _________________________________________________________ Window is visible ___
        $this->visible = true;
        $this->attiva  = ($this->id_esecuzione == $app->istanza_attiva ||
                          $this->menu_behavior);
        // ____________________________ Executes _def (active or undefined forms only) ___
        if ($this->attiva || !$this->defined) {
            $first_def = !$this->defined;
            $this->read_def();
            }
        if ($this->attiva) {
            // _____________________________________ Check if inside a try-catch block ___
            if (count($app->try_catch_nest)) {
                $try = array_shift($app->try_catch_nest);
                $e   = new o2_exception('Window <i>'.$this->nome.'</i> open '.
                                        'in TRY-CATCH is breaking TRY block.<br><br>'.
                                        'TRY started in program <i>'.$try['PRG'].
                                        ' ['.$try['ID'].']</i>, '.
                                        'action <i>'.$try['ACTION'].'</i>.',
                                        o2error_TRYCATCHFLOW);
                // ________________________________________ Force an uncatchable error ___
                $e->send();
                }
            if ($first_def) {
                // ______________________________________________ Resize form controls ___
                $this->resize();
                // ______________________________________________ Custom columns order ___
                if  ($prg->custom_grids) {
                    foreach ($this->controlli as $ctrl_idx => $ctrl_obj) {
                        if ($ctrl_obj->tipo_ctrl == 'tab' &&
                            isset($prg->custom_grids[$ctrl_idx]['O'])) {
                            /*
                            * ATTENTION: Next logic is needed to get the right moving of a
                            *            column in last position
                            */
                            $ol = $prg->custom_grids[$ctrl_idx]['O'];
                            asort($ol);
                            foreach ($ol as $lk => $lv) {
                                }
                            unset($ol[$lk]);
                            $ctrl_obj->move_column($lk, $lv);
                            foreach ($ol as $col => $pos) {
                                $ctrl_obj->move_column($col, $pos);
                                }
                            }
                        }
                    }
                }
            // ________________________ Resize all because controls changed visibility ___
            elseif ($this->force_resize) {
                $this->resize();
                }
            // ____________ Controls custom size and position (by scripting functions) ___
            if (isset($prg->custom_ctrls[$this->nome])) {
                foreach ($prg->custom_ctrls[$this->nome] as $ctrl_name => $pars) {
                    if (isset($this->controlli[$ctrl_name])) {
                        $ctrl = $this->controlli[$ctrl_name];
                        if (isset($pars["X"])) {
                            if ($pars["X"]) {
                                $ctrl->x        = $pars["X"];
                                $ctrl->custom_x = true;
                                }
                            else {
                                $ctrl->custom_x = false;
                                }
                            }
                        if (isset($pars["Y"])) {
                            if ($pars["Y"]) {
                                $ctrl->y        = $pars["Y"];
                                $ctrl->custom_y = true;
                                }
                            else {
                                $ctrl->custom_y = false;
                                }
                            }
                        if (isset($pars["W"])) {
                            if ($pars["W"]) {
                                $ctrl->larghezza    = $pars["W"];
                                $ctrl->custom_width = true;
                                }
                            else {
                                $ctrl->custom_width = false;
                                }
                            }
                        if (isset($pars["H"])) {
                            if ($pars["H"]) {
                                $ctrl->altezza       = $pars["H"];
                                $ctrl->custom_height = true;
                                }
                            else {
                                $ctrl->custom_height = false;
                                }
                            }
                        }
                    }
                unset($prg->custom_ctrls[$this->nome]);
                }
            }
        // ___________________________________________________________________ Display ___
        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_win($this);
            }
        else {
            o2html::ctrl_win($this);
            }
        return true;

        }


    /**
     * Close form and linked subforms, executing action, if any is defined as "action on
     * closing form"
     *
     */
    function close() {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$this->id_esecuzione];
        // _______________________________________________________ Closes linked forms ___
        foreach ($this->linked as $linked_form => $dummy_value) {
            $prg->form[$linked_form]->close();
            }
        // ______________________________________________________ Unset focus for form ___
        if ($app->caret_position[$this->id_esecuzione]['form'] == $this->nome) {
            unset($app->caret_position[$this->id_esecuzione]);
            }
        // ______________________________________________________ Execute close action ___
        if (array_key_exists($this->action_on_close, $prg->azioni)) {
            $prg->esegui_azione($this->action_on_close, true);
            }
        // __________________________________________________________ Closes main form ___
        if (isset($app->open_wins[$this->id_esecuzione][$this->nome])) {
            if ($GLOBALS['jxjs']) {
                if ($this->linkto) {
                    $parent_win = $prg->form[$this->linkto];
                    $link_id    = 0;
                    foreach ($parent_win->linked as $linked_form_name => $dummy_value) {
                        $link_id++;
                        if ($linked_form_name == $this->nome) {
                            break;
                            }
                        }
                    $div_name = $this->linkto."_".$this->id_esecuzione.$link_id;
                    jxjs::win_close($this->nome, $div_name);
                    }
                else {
                    jxjs::win_close($this->nome);
                    }
                }
            else {
                unset($app->open_wins[$this->id_esecuzione][$this->nome]);
                }
            }
        $this->chiusa  = true;
        $this->visible = false;

        }


    /**
     * Maximize or restore window
     *
     */
    function max_restore() {

        $app = $_SESSION['o2_app'];
        if ($this->maximized) {
            $this->larghezza  = ($this->restore_width ?
                                 $this->restore_width :
                                 $this->design_width);
            $this->altezza    = ($this->restore_height ?
                                 $this->restore_height :
                                 $this->design_height);
            $this->absolute_x = $this->restore_x;
            $this->absolute_y = $this->restore_y;
            $this->last_x     = $this->absolute_x;
            $this->last_y     = $this->absolute_y;
            $this->x          = $this->restore_x;
            $this->y          = $this->restore_y;
            $this->maximized  = false;
            }
        else {
            $this->restore_width  = $this->larghezza;
            $this->restore_height = $this->altezza;
            $this->last_x         = $this->absolute_x;
            $this->last_y         = $this->absolute_y;
            $this->restore_x      = $this->absolute_x;
            $this->restore_y      = $this->absolute_y;
            if ($app->menu_style == 'L' || $app->menu_style == 'A') {
                $this->larghezza = $app->client_width - $app->menu_width;
                $this->altezza   = $app->client_height - $app->status_height;
                }
            elseif ($app->menu_style == 'H') {
                $this->larghezza = $app->client_width;
                $this->altezza   = $app->client_height - $app->status_height;
                }
            else {
                $this->larghezza = $app->client_width;
                $this->altezza   = $app->client_height -
                                   $app->menu_height -
                                   $app->status_height;
                }
            $this->align_o     = "left";
            $this->align_v     = "top";
            $this->absolute_x  = 0;
            $this->absolute_y  = 0;
            $this->x           = 0;
            $this->y           = 0;
            $this->custom_size = true;
            $this->custom_pos  = true;
            $this->maximized   = true;
            }

        }


    /**
     * Legge i valori dei controlli dal contesto corrente della vista
     *
     */
    function leggi_valori_vista() {

        $app = $_SESSION['o2_app'];
        foreach ($this->controlli as &$ctrl) {
            // ________________________________________ If there is a field in control ___
            $field = $ctrl->campo_in_ctrl;
            if ($field) {
                $view = $app->istanze_prg[$this->id_esecuzione]->contesto[$ctrl->task];
                // ________________________________________ Gets field value from view ___
                $ctrl->valore = $view->campo($field);
                if (isset($view->formule[$field]) && $view->formule[$field]->selector) {
                    $ctrl->selector = true;
                    }
                }
            }

        }


    /**
     * Definisce un controllo per la form
     *
     * @param  string $alias_ctrl
     * @param  string $tipo
     * @param  string $padre
     * @param  string $task
     * @param  string $campo_task
     * @return o2_ctrl
     */
    function ctrldef($alias_ctrl, $tipo, $padre, $task, $campo_task) {

        $app        = $_SESSION['o2_app'];
        $prg        = $app->istanze_prg[$this->id_esecuzione];
        $campo_task = strtoupper($campo_task);
        if (!isset($this->controlli[$alias_ctrl])) {
            switch ($tipo) {
                case 'button':
                    $this->controlli[$alias_ctrl]      = new o2_ctrl_button();
                    $prg->ctrls['button'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                 = $this->controlli[$alias_ctrl];
                    break;
                case 'map':
                    $this->controlli[$alias_ctrl]   = new o2_ctrl_button();
                    $prg->ctrls['map'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl              = false;
                    break;
                case 'check':
                    $this->controlli[$alias_ctrl]     = new o2_ctrl_check();
                    $prg->ctrls['check'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                = $this->controlli[$alias_ctrl];
                    break;
                case 'document':
                    $this->controlli[$alias_ctrl]        = new o2_ctrl_doc();
                    $prg->ctrls['document'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                   = false;
                    break;
                case 'edit':
                    $this->controlli[$alias_ctrl]    = new o2_ctrl_edit();
                    $prg->ctrls['edit'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl               = $this->controlli[$alias_ctrl];
                    break;
                case 'file':
                    $this->controlli[$alias_ctrl]    = new o2_ctrl_file();
                    $prg->ctrls['file'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl               = false;
                    break;
                case 'htmlarea':
                    $this->controlli[$alias_ctrl]        = new o2_ctrl_html();
                    $prg->ctrls['htmlarea'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                   = false;
                    break;
                case 'img':
                    $this->controlli[$alias_ctrl]   = new o2_ctrl_img();
                    $prg->ctrls['img'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl              = $this->controlli[$alias_ctrl];
                    break;
                case 'label':
                    $this->controlli[$alias_ctrl]     = new o2_ctrl_label();
                    $prg->ctrls['label'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                = $this->controlli[$alias_ctrl];
                    break;
                case 'line':
                    $this->controlli[$alias_ctrl]    = new o2_ctrl_line();
                    $prg->ctrls['line'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl               = false;
                    break;
                case 'listcombo':
                    $this->controlli[$alias_ctrl]         = new o2_ctrl_listcombo();
                    $prg->ctrls['listcombo'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                    = $this->controlli[$alias_ctrl];
                    break;
                case 'multipage':
                    $this->controlli[$alias_ctrl]         = new o2_ctrl_multipage();
                    $prg->ctrls['multipage'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                    = false;
                    break;
                case 'navigator':
                    $this->controlli[$alias_ctrl]         = new o2_ctrl_navigator();
                    $prg->ctrls['navigator'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                    = false;
                    break;
                case 'tab':
                    $this->controlli[$alias_ctrl]   = new o2_ctrl_tab();
                    $prg->ctrls['tab'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl              = false;
                    break;
                case 'text':
                    $this->controlli[$alias_ctrl]    = new o2_ctrl_text();
                    $prg->ctrls['text'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl               = $this->controlli[$alias_ctrl];
                    break;
                case 'tree':
                    $this->controlli[$alias_ctrl]    = new o2_ctrl_tree();
                    $prg->ctrls['tree'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl               = false;
                    break;
                case 'imglist':
                    $this->controlli[$alias_ctrl]       = new o2_ctrl_imglist();
                    $prg->ctrls['imglist'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                  = false;
                    break;
                case 'progress':
                    $this->controlli[$alias_ctrl]        = new o2_ctrl_progress();
                    $prg->ctrls['progress'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                   = false;
                    break;
                case 'flowbox':
                    $this->controlli[$alias_ctrl]       = new o2_ctrl_flowbox();
                    $prg->ctrls['flowbox'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                  = false;
                    break;
                case 'frame':
                    $this->controlli[$alias_ctrl]     = new o2_ctrl_frame();
                    $prg->ctrls['frame'][$alias_ctrl] = $this->controlli[$alias_ctrl];
                    $prg->drawing_ctrl                = false;
                    break;
                }
            $new_ctrl                = $this->controlli[$alias_ctrl];
            $new_ctrl->nome          = $alias_ctrl;
            $new_ctrl->id_esecuzione = $this->id_esecuzione;
            $new_ctrl->form          = $this->nome;
            $new_ctrl->prg           = $this->prg;
            $new_ctrl->padre         = $padre;
            $new_ctrl->html_form     = $this->nome.'_'.$this->id_esecuzione.'_html_form';
            $new_ctrl->js_form       = 'document.o2form';
            $new_ctrl->js_name       = $new_ctrl->js_form.'.'.$new_ctrl->nome;
            if ($padre) {
                $parent = $this->controlli[$padre];
                if ($parent->tipo_ctrl == 'tab') {
                    $prg->drawing_grid = $parent;
                    }
                else {
                    $prg->drawing_grid = false;
                    }
                }
            else {
                $prg->drawing_grid = false;
                }
            if ($task) {
                $new_ctrl->task = $task;
                $view_obj       = $prg->contesto[$task];
                // _______________________________ To set table used objects reference ___
                if ($tipo == 'tab') {
                    $new_ctrl->prg_obj  = $prg;
                    $new_ctrl->view_obj = $view_obj;
                    $new_ctrl->form_obj = $this;
                    $prg->drawing_grid  = $new_ctrl;
                    if (isset($prg->view_grids[$task])) {
                        $share_grid = $prg->view_grids[$task];
                        // ___________________________________ View used by other grid ___
                        if ($share_grid != $alias_ctrl) {
                            $new_ctrl->view_shared = true;
                            // _____________________ Sharing-grid is on this same form ___
                            if (isset($this->controlli[$share_grid])) {
                                $this->controlli[$share_grid]->view_shared = true;
                                }
                            // _________________________ Sharing-grid is on other form ___
                            else {
                                foreach ($prg->form as $single_form) {
                                    if (isset($single_form->controlli[$share_grid])) {
                                        $single_form->controlli[$share_grid]->view_shared
                                                                                   = true;
                                        break;
                                        }
                                    }
                                }
                            }
                        }
                    // _________________________________________ Set view used by grid ___
                    else {
                        $prg->view_grids[$task] = $alias_ctrl;
                        }
                    }
                elseif ($prg->drawing_ctrl && $view_obj->dbdata) {
                    $new_ctrl->dependences[$task] = false;
                    }
                if ($campo_task) {
                    $field_obj = $view_obj->campo_per_controllo($campo_task);
                    $new_ctrl->campo_in_ctrl = $campo_task;
                    $new_ctrl->maschera      = $field_obj->maschera;
                    if ($prg->drawing_ctrl && !$view_obj->dbdata) {
                        $new_ctrl->dependences[$task][$campo_task] = false;
                        }
                    }
                // __________________ Check log activated for table binding to control ___
                if ($app->tables_log && $campo_task && $view_obj->dbdata) {
                    $field_obj = $view_obj->campo_per_controllo($campo_task);
                    // ____________________ If object belongs to a table (not virtual) ___
                    if ($field_obj->file) {
                        $field_tab = $view_obj->files[$field_obj->file];
                        if ($field_tab->log_level) {
                            $new_ctrl->log_level = $field_tab->log_level;
                            }
                        }
                    }
                }
            }
        else {
            $new_ctrl               = $this->controlli[$alias_ctrl];
            $new_ctrl->modificabile = true;
            if ($task) {
                $view_obj = $prg->contesto[$task];
                // ______________ This is needed when controls are disabled by parents ___
                if ($tipo == 'navigator') {
                    $new_ctrl->enable_del(true);
                    $new_ctrl->enable_detail(true);
                    $new_ctrl->enable_new(true);
                    $new_ctrl->enable_save(true);
                    $new_ctrl->enable_select(true);
                    $new_ctrl->enable_undo(true);
                    }
                }
            }
        // _____________________________________ Set enabled/disabled by table in view ___
        if ($campo_task && $view_obj->dbdata) {
            $field_obj              = $view_obj->campo_per_controllo($campo_task);
            $new_ctrl->modificabile = $field_obj->file == $view_obj->file->indice;
            }
        // _________________________ Set drawing control to update control dependences ___
        switch ($tipo) {
            case 'button':
            case 'edit':
            case 'label':
            case 'text':
            case 'check':
            case 'img':
                $prg->drawing_ctrl = $new_ctrl;
                break;
            case 'listcombo':
                // _______________ Per attivare view su o2_view2list() per lookup ctrl ___
                $app->defining_combo = $new_ctrl;
                $prg->drawing_ctrl   = $new_ctrl;
                break;
            }
        return $new_ctrl;

        }


    /**
     * Reads form definition (function _def in prf module) and redefines controls
     *
     */
    function read_def() {

        $app               = $_SESSION['o2_app'];
        $def_function      = $this->prg."§§".$this->nome."_def";
        if (is_callable($def_function)) {
            $prg = $app->istanze_prg[$this->id_esecuzione];
            $def_function($this);
            $this->defined     = true;
            $prg->drawing_grid = false;
            $prg->drawing_ctrl = false;
            // ____________________________________________________ Controls profiling ___
            if (($app->profiling == "C" || $app->profiling == "B") &&
                ($this->id_esecuzione == count($app->istanze_prg)) &&
                !$app->user_is_admin && !$prg->internal_tool) {
                $app->intcall("tools/o2sys_filter_ctrl", $this->nome);
                }
            }
        elseif (!$this->internal) {
            throw new o2_exception("<b>CAN'T FIND DEFINITION</b> for form <i>".
                                   $this->nome."</i>",
                                   o2error_IO);
            }

        }


    /**
     * System for auto-resizing form controls
     *
     */
    function resize() {

        $d_x = max(0, $this->larghezza - $this->design_width);
        $d_y = max(0, $this->altezza - $this->design_height);
        // __________________________________________ Reset all controls default values___
        foreach ($this->controlli as $ctrl) {
            $ctrl->expand_x      = false;
            $ctrl->expand_y      = false;
            $ctrl->expand_width  = false;
            $ctrl->expand_height = false;
            }
        $this->read_def();
        $this->defined = true;
        // ____________________________________ Compose lists of controls sharing axes ___
        list($x_list, $y_list) = $this->resize_analysis();
        // _________________________________________________ Resize and repos controls ___
        $this->resize_expand($x_list, $y_list, $d_x, $d_y);

        }


    /**
     * Compose lists of expandable controls, each one with a list of controls moved (right
     * or down on the axis) by its expansion.
     *
     * This is the model on the horizontal axis, where A is an expandable control and
     * C1-C4 are different positions can be a control to be moved by expansion of A.
     *
     *               +-----+                           +-----+ - - -> $c_top
     * +--------+ - -|- - -|- - - - - - - - - - - - - -|- - -|- - - - - - - - -> $min_y
     * |        |    |     |    +-----+                |     |
     * |        |    | C1  |    |     |                |     |
     * |   A    |    |     |    | C2  |                | C4  |
     * |        |    +-----+    |     |    +-----+     |     |
     * |        |               +-----+    |     |     |     |
     * +--------+ - - - - - - - - - - - - -|- - -|- - -|- - -|- - - - - - - - -> $max_y
     *                                     | C3  |     +-----+ - - -> $c_bot
     *                                     |     |
     *                                     +-----+
     *
     * @param  string  $parent
     * @param  integer $page
     * @return array
     */
    function resize_analysis($parent = '', $page = false) {

        // ____________________________________ Compose lists of controls sharing axes ___
        $x_list = array();
        $y_list = array();
        $excl_x = array();
        $excl_y = array();
        $c_list = $this->controlli;
        // ________________________________________________________ Sort controls by X ___
        usort($c_list, function($a, $b) {
                          return ($a->x == $b->x ? 0 : ($a->x > $b->x ? 1 : -1));
                          });
        foreach ($c_list as $ctrl) {
            if ($ctrl->padre == $parent &&
                ($page === false || $ctrl->info_padre[0] == $page)) {
                // ___________________________________ Control can expand horizontally ___
                if ($ctrl->expand == 'H' || $ctrl->expand == 'B') {
                    $min_y               = $ctrl->y;
                    $max_y               = $ctrl->y + $ctrl->altezza;
                    $x_list[$ctrl->nome] = array('#share' => 1);
                    $first_share_c       = false;
                    // ____________________ Check controls on the same horizontal axis ___
                    foreach ($c_list as $c_obj) {
                        $c_name = $c_obj->nome;
                        if ($c_name != $ctrl->nome &&
                            $c_obj->padre == $parent &&
                            ($page === false || $c_obj->info_padre[0] == $page) &&
                            $c_obj->is_visible()) {
                            $c_top = $c_obj->y;
                            $c_bot = $c_obj->y + $c_obj->altezza;
                            // ____________________________________________________ C2 ___
                            if (($c_top >= $min_y && $c_bot <= $max_y) ||
                                // ___________________________________________ C1 & C4 ___
                                ($c_top < $min_y && $c_bot > $min_y) ||
                                // ___________________________________________ C3 & C4 ___
                                ($c_top < $max_y && $c_bot > $max_y)) {
                                // _____________ Check if control is at the right of A ___
                                if ($c_obj->x > $ctrl->x + $ctrl->larghezza - 1) {
                                    // ________________ Add control to horizontal list ___
                                    $x_list[$ctrl->nome][] = $c_name;
                                    $min_y = ($c_top < $min_y ? $c_top : $min_y);
                                    $max_y = ($c_bot > $max_y ? $c_bot : $max_y);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        // _____________________________________________________________ Refine X-axis ___
        foreach ($x_list as $expanded_ctrl => $moved_ctrls) {
            // ____________________________________________ Get maximum SHARE and PATH ___
            /**
             * SHARE: number on expandable controls on an axis
             * PATH:  list of expandable controls sharing an axis
             */
            list($share, $path) = self::resize_get_axis($expanded_ctrl, $x_list);
            $x_list[$expanded_ctrl]['#share'] = $share;
            $x_list[$expanded_ctrl]['#path']  = $path;
            $x_list[$expanded_ctrl]['#cells'] = $share;
            $x_list[$expanded_ctrl]['#used']  = 0;
            }
        // _ For each control set number of cells shared by other controls on its path ___
        $x_axis = array();
        // _______________________________________________ Loop on expandable controls ___
        foreach ($x_list as $expanded_ctrl => $moved_ctrls) {
            // _______________________ Loop on controls moved by each expanded control ___
            foreach ($moved_ctrls as $moved_ctrl) {
                // ____________________________________ If moved control is expandable ___
                if (is_string($moved_ctrl) && isset($x_list[$moved_ctrl])) {
                    if ($x_list[$moved_ctrl]['#cells'] <
                        $x_list[$expanded_ctrl]['#share']) {
                        // ______________ Control CELLS is maximum SHARE found on path ___
                        $x_list[$moved_ctrl]['#cells']= $x_list[$expanded_ctrl]['#share'];
                        }
                    $x_list[$moved_ctrl]['#used']++;
                    }
                }
            $path = $x_list[$expanded_ctrl]['#path'];
            // _____________________ Check if control path is contained in other paths ___
            if ($x_axis) {
                foreach ($x_axis as $axis_ctrl => $axis_path) {
                    // _______ Path is a new axis: remove controls moved by other axis ___
                    $contained = array_intersect($path, $axis_path);
                    // ____________________________________________ Path is a new axis ___
                    if ($contained != $axis_path && $contained != $path) {
                        // _______________________ Conserve controls on max-share axis ___
                        if ($x_list[$axis_ctrl]['#cells'] <
                            $x_list[$expanded_ctrl]['#cells']) {
                            $save_ctrl  = $axis_ctrl;
                            $save_share = $x_list[$axis_ctrl]['#share'];
                            $save_path  = $x_list[$axis_ctrl]['#path'];
                            $save_cells = $x_list[$axis_ctrl]['#cells'];
                            $save_used  = $x_list[$axis_ctrl]['#used'];
                            $x_list[$axis_ctrl] =
                                array_udiff($x_list[$axis_ctrl],
                                            $x_list[$expanded_ctrl],
                                            array($this, 'a_comp'));
                            }
                        elseif ($x_list[$axis_ctrl]['#cells'] >
                            $x_list[$expanded_ctrl]['#cells']) {
                            $save_ctrl  = $expanded_ctrl;
                            $save_share = $x_list[$expanded_ctrl]['#share'];
                            $save_path  = $x_list[$expanded_ctrl]['#path'];
                            $save_cells = $x_list[$expanded_ctrl]['#cells'];
                            $save_used  = $x_list[$expanded_ctrl]['#used'];
                            $x_list[$expanded_ctrl] =
                                array_udiff($x_list[$expanded_ctrl],
                                            $x_list[$axis_ctrl],
                                            array($this, 'a_comp'));
                            }
                        else {
                            $save_ctrl  = $expanded_ctrl;
                            $save_share = $x_list[$expanded_ctrl]['#share'];
                            $save_path  = $x_list[$expanded_ctrl]['#path'];
                            $save_cells = $x_list[$expanded_ctrl]['#cells'];
                            $save_used  = $x_list[$expanded_ctrl]['#used'];
                            if (!in_array($expanded_ctrl, $x_list[$axis_ctrl], true)) {
                                $x_list[$expanded_ctrl] =
                                    array_udiff($x_list[$expanded_ctrl],
                                                        $x_list[$axis_ctrl],
                                                        array($this, 'a_comp'));
                                }
                            }
                        // __________________ Restore elements dropped by array_diff() ___
                        $x_list[$save_ctrl]['#share'] = $save_share;
                        $x_list[$save_ctrl]['#path']  = $save_path;
                        $x_list[$save_ctrl]['#cells'] = $save_cells;
                        $x_list[$save_ctrl]['#used']  = $save_used;
                        }
                    }
                }
            $x_axis[$expanded_ctrl] = $path;
            }
        unset($x_axis);
        // ________________________________________________________ Sort controls by Y ___
        usort($c_list, function($a, $b) {
                          return ($a->y == $b->y ? 0 : ($a->y > $b->y ? 1 : -1));
                          });
        foreach ($c_list as $ctrl) {
            if ($ctrl->padre == $parent &&
                ($page === false || $ctrl->info_padre[0] == $page)) {
                // _____________________________________ Control can expand vertically ___
                if ($ctrl->expand == 'V' || $ctrl->expand == 'B') {
                    $min_x               = $ctrl->x;
                    $max_x               = $ctrl->x + $ctrl->larghezza;
                    $y_list[$ctrl->nome] = array('#share' => 1);
                    $first_share_c       = false;
                    // ______________________ Check controls on the same vertical axis ___
                    foreach ($c_list as $c_obj) {
                        $c_name = $c_obj->nome;
                        if ($c_name != $ctrl->nome &&
                            $c_obj->padre == $parent &&
                            ($page === false || $c_obj->info_padre[0] == $page) &&
                            $c_obj->is_visible()) {
                            $c_left  = $c_obj->x;
                            $c_right = $c_obj->x + $c_obj->larghezza;
                            // ____________________________________ Same vertical axis ___
                            if (($c_left >= $min_x && $c_right <= $max_x) ||
                                ($c_left <  $min_x && $c_right >  $min_x) ||
                                ($c_left <  $max_x && $c_right >  $max_x)) {
                                // _______________________ Check if control is below A ___
                                if ($c_obj->y > $ctrl->y + $ctrl->altezza - 1) {
                                    // __________________ Add control to vertical list ___
                                    $y_list[$ctrl->nome][] = $c_name;
                                    $min_x = ($c_left < $min_x ? $c_left : $min_x);
                                    $max_x = ($c_right > $max_x ? $c_right : $max_x);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        // _____________________________________________________________ Refine X-axis ___
        foreach ($y_list as $expanded_ctrl => $moved_ctrls) {
            // ____________________________________________ Get maximum SHARE and PATH ___
            /**
             * SHARE: number on expandable controls on an axis
             * PATH:  list of expandable controls sharing an axis
             */
            list($share, $path) = self::resize_get_axis($expanded_ctrl, $y_list);
            $y_list[$expanded_ctrl]['#share'] = $share;
            $y_list[$expanded_ctrl]['#path']  = $path;
            $y_list[$expanded_ctrl]['#cells'] = $share;
            $y_list[$expanded_ctrl]['#used']  = 0;
            }
        // _ For each control set number of cells shared by other controls on its path ___
        $y_axis = array();
        // _______________________________________________ Loop on expandable controls ___
        foreach ($y_list as $expanded_ctrl => $moved_ctrls) {
            // _______________________ Loop on controls moved by each expanded control ___
            foreach ($moved_ctrls as $moved_ctrl) {
                // ____________________________________ If moved control is expandable ___
                if (is_string($moved_ctrl) && isset($y_list[$moved_ctrl])) {
                    if ($y_list[$moved_ctrl]['#cells'] <
                        $y_list[$expanded_ctrl]['#share']) {
                        // ______________ Control CELLS is maximum SHARE found on path ___
                        $y_list[$moved_ctrl]['#cells']= $y_list[$expanded_ctrl]['#share'];
                        }
                    $y_list[$moved_ctrl]['#used']++;
                    }
                }
            $path = $y_list[$expanded_ctrl]['#path'];
            // _____________________ Check if control path is contained in other paths ___
            if ($y_axis) {
                foreach ($y_axis as $axis_ctrl => $axis_path) {
                    // _______ Path is a new axis: remove controls moved by other axis ___
                    $contained = array_intersect($path, $axis_path);
                    // ____________________________________________ Path is a new axis ___
                    if ($contained != $axis_path && $contained != $path) {
                        // _______________________ Conserve controls on max-share axis ___
                        if ($y_list[$axis_ctrl]['#cells'] <
                            $y_list[$expanded_ctrl]['#cells']) {
                            $save_ctrl  = $axis_ctrl;
                            $save_share = $y_list[$axis_ctrl]['#share'];
                            $save_path  = $y_list[$axis_ctrl]['#path'];
                            $save_cells = $y_list[$axis_ctrl]['#cells'];
                            $save_used  = $y_list[$axis_ctrl]['#used'];
                            $y_list[$axis_ctrl] =
                                array_udiff($y_list[$axis_ctrl],
                                            $y_list[$expanded_ctrl],
                                            array($this, 'a_comp'));
                            }
                        elseif ($y_list[$axis_ctrl]['#cells'] >
                            $y_list[$expanded_ctrl]['#cells']) {
                            $save_ctrl  = $expanded_ctrl;
                            $save_share = $y_list[$expanded_ctrl]['#share'];
                            $save_path  = $y_list[$expanded_ctrl]['#path'];
                            $save_cells = $y_list[$expanded_ctrl]['#cells'];
                            $save_used  = $y_list[$expanded_ctrl]['#used'];
                            $y_list[$expanded_ctrl] =
                                array_udiff($y_list[$expanded_ctrl],
                                            $y_list[$axis_ctrl],
                                            array($this, 'a_comp'));
                            }
                        // _______________ If paths are the same don't change anything ___
                        else {
                            $save_ctrl  = $expanded_ctrl;
                            $save_share = $y_list[$expanded_ctrl]['#share'];
                            $save_path  = $y_list[$expanded_ctrl]['#path'];
                            $save_cells = $y_list[$expanded_ctrl]['#cells'];
                            $save_used  = $y_list[$expanded_ctrl]['#used'];
                            if (!in_array($expanded_ctrl, $y_list[$axis_ctrl], true)) {
                                $y_list[$expanded_ctrl] =
                                    array_udiff($y_list[$expanded_ctrl],
                                                $y_list[$axis_ctrl],
                                                array($this, 'a_comp'));
                                }
                            }
                        // __________________ Restore elements dropped by array_diff() ___
                        $y_list[$save_ctrl]['#share'] = $save_share;
                        $y_list[$save_ctrl]['#path']  = $save_path;
                        $y_list[$save_ctrl]['#cells'] = $save_cells;
                        $y_list[$save_ctrl]['#used']  = $save_used;
                        }
                    }
                }
            $y_axis[$expanded_ctrl] = $path;
            }
        unset($y_axis);
        // _____________________  Return controls informations for moving and resizing ___
        return array($x_list, $y_list);

        }

    /**
     * This method is used as call-back function for comparing axis information in method
     * "resize_analysis".
     * This function replicates the behaviour of internal comparng function for array_diff
     * without errors on comparing array elements.
     */
    function a_comp($a, $b) {

        if (is_array($a)) {
            $a = 'Array';
            }
        if (is_array($b)) {
            $b = 'Array';
            }
        if ((string) $a > (string) $b) {
            return 1;
            }
        elseif ((string) $a < (string) $b) {
            return -1;
            }
        else {
            return 0;
            }

        }


    /**
     * This method is used to distinguish single axis from each other.
     * An horizonthal axis is a list of expandable controls so that each control is moved
     * by previous controls in the list.
     * For each conrol MAX-SHARE and MAX-PATH are returned:
     *  MAX-SHARE: Maximum number of expandable controls moved by control
     *  MAX-PATH:  List of controls sharing MAX-SHARE with control
     *
     * @param  string $expanded_ctrl
     * @param  array  $list
     * @param  array  $path
     * @return array
     */
    function resize_get_axis($expanded_ctrl, $list, $path = array()) {

        $max_share  = 1;
        $max_path   = $path;
        $max_path[] = $expanded_ctrl;
        foreach ($list[$expanded_ctrl] as $moved_ctrl) {
            // ________________________________________ If moved control is expandable ___
            if (isset($list[$moved_ctrl])) {
                $local_share                    = 0;
                $path_local                     = $path;
                $path_local[]                   = $expanded_ctrl;
                list($local_share, $path_local) = self::resize_get_axis($moved_ctrl,
                                                                        $list,
                                                                        $path_local);
                $local_share++;
                if ($local_share > $max_share) {
                    $max_share = $local_share;
                    $max_path  = $path_local;
                    }
                }
            }
        return array($max_share, $max_path);

        }


    function resize_expand($x_list, $y_list, $d_x, $d_y) {

        $containers = array();
        // ________________________ Loop on expandable controls on the horizontal axis ___
        foreach ($x_list as $expand_ctrl => $moved_ctrls) {
            $cells = $moved_ctrls['#cells'];
            $used  = ($moved_ctrls['#used'] ?
                      $moved_ctrls['#used'] :
                      $moved_ctrls['#cells'] - 1);
            unset($moved_ctrls['#used']);
            unset($moved_ctrls['#share']);
            unset($moved_ctrls['#cells']);
            $add = intval($d_x / $cells); // * ($cells - $used));
            // __________________________________________________ Expand control width ___
            $exp_c = $this->controlli[$expand_ctrl];
            if (!$exp_c->custom_width) {
                $exp_c->larghezza   += $add;
                $exp_c->expand_width = true;
                }
            // ____________________________________________ Move controls on the right ___
            foreach ($moved_ctrls as $moved_ctrl) {
                if (is_string($moved_ctrl)) {
                    $mov_c = $this->controlli[$moved_ctrl];
                    if (!$mov_c->custom_x) {
                        $mov_c->x       += $add;
                        $mov_c->expand_x = true;
                        }
                    }
                }
            // ___________________________________ If container add to containers list ___
            if ($exp_c->tipo_ctrl == 'multipage' || $exp_c->tipo_ctrl == 'tab') {
                if (!isset($containers[$expand_ctrl])) {
                    $containers[$expand_ctrl] = array();
                    }
                $containers[$expand_ctrl]['X'] = $add;
                }
            }
        // __________________________ Loop on expandable controls on the vertical axis ___
        foreach ($y_list as $expand_ctrl => $moved_ctrls) {
            $cells = $moved_ctrls['#cells'];
            $used  = ($moved_ctrls['#used'] ?
                      $moved_ctrls['#used'] :
                      $moved_ctrls['#cells'] - 1);
            unset($moved_ctrls['#used']);
            unset($moved_ctrls['#share']);
            unset($moved_ctrls['#cells']);
            $add = intval($d_y / $cells); // * ($cells - $used));
            // _________________________________________________ Expand control height ___
            $exp_c = $this->controlli[$expand_ctrl];
            if (!$exp_c->custom_height) {
                $exp_c->altezza      += $add;
                $exp_c->expand_height = true;
                }
            // ___________________________________________ Move controls to the bottom ___
            foreach ($moved_ctrls as $moved_ctrl) {
                if (is_string($moved_ctrl)) {
                    $mov_c = $this->controlli[$moved_ctrl];
                    if (!$mov_c->custom_y) {
                        $mov_c->y       += $add;
                        $mov_c->expand_y = true;
                        }
                    }
                }
            // ___________________________________ If container add to containers list ___
            if ($exp_c->tipo_ctrl == 'multipage' || $exp_c->tipo_ctrl == 'tab') {
                if (!isset($containers[$expand_ctrl])) {
                    $containers[$expand_ctrl] = array();
                    }
                $containers[$expand_ctrl]['Y'] = $add;
                }
            }
        // ______________________________ Loop on containers to expand controls inside ___
        foreach ($containers as $ctrl => $values) {
            if ($this->controlli[$ctrl]->tipo_ctrl == 'multipage') {
                foreach ($this->controlli[$ctrl]->label as $label => $label_desc) {
                    // ________________________ Compose lists of controls sharing axes ___
                    list($cx_list, $cy_list) = $this->resize_analysis($ctrl, $label);
                    // _____________________________________ Resize and repos controls ___
                    $this->resize_expand($cx_list, $cy_list, $values['X'], $values['Y']);
                    }
                }
            else {
                // ____________________________ Compose lists of controls sharing axes ___
                list($cx_list, $cy_list) = $this->resize_analysis($ctrl);
                // _________________________________________ Resize and repos controls ___
                $this->resize_expand($cx_list, $cy_list, $values['X'], $values['Y']);
                }
            }

        }

    }


/**
 * Ancestor class for all controls type
 *
 */
class o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $nome            = "";      /* Control name                                  */
    public $tipo_ctrl       = "";      /* Control type
                                          +-----------------------------+
                                          |=[type]======[description]===|
                                          |  label       Label          |
                                          |  line        Separator      |
                                          |  navigator   Navigator      |
                                          |  button      Button         |
                                          |  edit        Edit           |
                                          |  text        Text area      |
                                          |  checkbox    Check box      |
                                          |  listcombo   List/combo box |
                                          |  file        File upload    |
                                          |  img         Image          |
                                          |  tab         Table          |
                                          |  doc         Document       |
                                          |  html        HTML Area      |
                                          |  multipage   Multipage      |
                                          +-----------------------------+               */
    public $form            = "";      /* Name of form containing control               */
    public $padre           = "";      /* Name of parent (container) control (""=form)  */
    public $info_padre      = array(); /* Control informations for parent               */
    public $prg             = "";      /* Name of prg containing control                */
    public $id_esecuzione   = 0;       /* Program execution id (PRG_EXE_ID)             */
    public $task            = "";      /* View name if one is binded to control         */
    public $campo_in_ctrl   = "";      /* Field name if one is binded to control        */
    public $html_form       = "";      /* Name of HTML form containing control          */
    public $js_name         = "";      /* JS name (document.forms[frn_name].ctrl_name)  */
    public $js_form         = "";      /* JS form name (document.forms[frn_name])       */
    public $tab_row         = false;   /* If control is not in current row of table     */
    public $active_by_tab   = true;    /* If control is "active by table": not in table
                                          or in current row of active table             */
    public $tab_index       = false;   /* Row-ID if control is inside a dbgrid          */
    public $rows_vis        = array(); /* Rows visibility array                         */
    public $dependences     = array(); /* List of variables/fields this ctrl depends on */
    public $log_level       = false;   /* If ctrl has log level inherited from field/tab*/
    /*     _____ Visuals ______________________________________________________________ */
    public $valore          = "";      /* Control value                                 */
    /**
     *  @var o2_maschera
     */
    public $maschera        = "";      /* Control data value input/output mask          */
    public $visibile        = true;    /* If control is visible                         */
    public $modificabile    = true;    /* If control is modifiable                      */
    public $x               = 0;       /* Control horizontal position in px             */
    public $y               = 0;       /* Control vertical position in px               */
    public $larghezza       = 0;       /* Control width in px                           */
    public $altezza         = 0;       /* Control height in px                          */
    public $wide            = "";      /* Widing style: [H]orizontal [V]ertical [B]oth  */
    public $custom_width    = false;   /* If control width is set programmatically      */
    public $custom_height   = false;   /* If control height is set programmatically     */
    public $custom_x        = false;   /* If control x position is set programmatically */
    public $custom_y        = false;   /* If control y position is set programmatically */
    public $expand          = false;   /* If control is expandable                      */
    public $expand_width    = false;   /* If control width is set by resize             */
    public $expand_height   = false;   /* If control height is set by resize            */
    public $expand_x        = false;   /* If control x position is set by resize        */
    public $expand_y        = false;   /* If control y position is set by resize        */
    public $popup_exp       = false;   /* Expression to be displayed on popup over ctrl */
    public $popup_act       = false;   /* Action to be executed on popup over control   */
    /*     _____ Actions ______________________________________________________________ */
    public $forza_submit    = false;   /* If to return to server on control change      */
    public $azione_submit   = "";      /* Action to be executed on control change       */
    public $confirm_message = "";      /* Client side confirm message text              */
    /*     _____ CSS WHOLE CLASS ______________________________________________________ */
    public $css             = false;   /* Whole control main class                      */


    /**
     * Ritorna il valore client del controllo nel formato sql per la scrittura sul
     * recordset
     *
     * @return string
     */
    function ctrl2sql() {

        $sql_valore_local = "";
        $mask = $this->maschera;
        switch ($mask->tipo) {
            // =============================== ALPHA =====================================
            case "A":
                if (strlen($this->valore) > $mask->dimensione) {
                    $sql_valore_local = substr($this->valore, 0, $mask->dimensione);
                    }
                else {
                    $sql_valore_local = (trim($this->valore) == '' ?
                                         $mask->default : $this->valore);
                    }
                break;
            // ================================ DATE =====================================
            case "D":
                if (trim($this->valore == '')) {
                    $sql_valore_local = $mask->default;
                    }
                else {
                    $d   = "01";
                    $m   = "01";
                    $y   = "2000";
                    $sep = $mask->separatore;
                    if (strpos($this->valore, $sep) !== false ||
                        strlen($this->valore) != 8) {
                        $vars   = explode($sep, $mask->expression);
                        $vars_n = count($vars);
                        for ($i = 0; $i < $vars_n; $i++) {
                            $vars[$i] = "\$".$vars[$i];
                            }
                        eval("list(".implode(",",$vars).
                             ", \$dv1, \$dv2, \$dv3) = explode(\$sep, \$this->valore);");
                        if (strlen($y) == 2) {
                            if ($d != "00" && $m != "00") {
                                $y = "20".$y;
                                }
                            else {
                                $y = "0000";
                                }
                            }
                        $sql_valore_local = $y.$m.$d;
                        }
                    else {
                        $sql_valore_local = $this->valore;
                        }
                    }
                break;
            // ================================ TIME =====================================
            case "O":
                if (trim($this->valore == '')) {
                    $sql_valore_local = $mask->default;
                    }
                else {
                    $h   = "00";
                    $m   = "00";
                    $s   = "00";
                    $sep = $mask->separatore;
                    if (strpos($this->valore, $sep) !== false ||
                        strlen($this->valore) != 6) {
                        $vars   = explode($sep, $mask->expression);
                        $vars_n = count($vars);
                        for ($i = 0; $i < $vars_n; $i++) {
                            $vars[$i] = "\$".$vars[$i];
                            }
                        eval("list(".implode(",",$vars).") = ".
                             "explode(\$this->maschera->separatore, \$this->valore);");
                             $sql_valore_local = $h.$m.$s;
                        }
                     else {
                        $sql_valore_local = $this->valore;
                        }
                    }
                break;
            // ============================== NUMERIC ====================================
            case "N":
                $sql_valore_local = (trim(strval($this->valore)) == "" ?
                                     $mask->default :
                                     str_replace($mask->separatore, ".",
                                      str_replace($mask->migliaia, "",
                                       str_replace($mask->prefisso, "",
                                     $this->valore))));
                if (!$mask->negativo) {
                    $sql_valore_local = abs($sql_valore_local);
                    }
                $sql_valore_local = number_format($sql_valore_local,
                                                  $mask->decimali,
                                                  ".",
                                                  "");
                break;
            // ============================= STRUCTURE ===================================
            case "S":
                $sql_valore_local = $this->valore;
                break;
            // ============================== LOGICAL ====================================
            case "L":
                $sql_valore_local = ($this->valore ? 1 : 0);
                break;
            // =========================== OTHER =====================================
            default:
                $sql_valore_local = (trim($this->valore) == "" ?
                                     $mask->default :
                                     $this->valore);
                break;
            }
        return $sql_valore_local;

        }


    /**
     * Ritorna il valore sql del controllo nel formato client per la visualizzazione
     *
     * @return string
     */
    function sql2ctrl() {

        $app  = $_SESSION['o2_app'];
        $mask = $this->maschera;
        if (isset($this->password) && $this->password) {
            if (trim($this->valore) != "") {
                $ctrl_valore_local = str_repeat("*", $mask->dimensione);
                }
            else {
                $ctrl_valore_local = "";
                }
            }
        else {
            $ctrl_valore_local = "";
            switch ($mask->tipo) {
                // ============================== DATE ===================================
                case "D":
                    if (trim($this->valore) != "" &&
                        $this->valore != $mask->default) {
                            $ctrl_valore_local = str_replace(array("d", "m", "y"),
                                                             array($this->valore[6].
                                                                   $this->valore[7],
                                                                   $this->valore[4].
                                                                   $this->valore[5],
                                                                  ($mask->shortyear ? "" :
                                                                    $this->valore[0].
                                                                    $this->valore[1]).
                                                                   $this->valore[2].
                                                                   $this->valore[3]),
                                                           strtolower($mask->expression));
                        }
                    elseif (!$mask->zeroff) {
                        $ctrl_valore_local = str_replace(array("d", "m", "y"),
                                                         array("00",
                                                               "00",
                                                               ($mask->shortyear ?
                                                                "00" :
                                                                "0000")),
                                                               $mask->expression);
                        }
                    else {
                        $ctrl_valore_local = "";
                        }
                    break;

                // ============================== TIME ===================================
                case "O":
                    if (trim($this->valore) != "" &&
                        $this->valore != $mask->default) {
                        $ctrl_valore_local = str_replace(array("h", "m", "s"),
                                                         array($this->valore[0].
                                                               $this->valore[1],
                                                               $this->valore[2].
                                                               $this->valore[3],
                                                               $this->valore[4].
                                                               $this->valore[5]),
                                                           strtolower($mask->expression));
                        }
                    elseif (!$mask->zeroff) {
                        $ctrl_valore_local = str_replace(array("h", "m", "s"),
                                                         array("00", "00", "00"),
                                                         $mask->expression);
                        }
                    else {
                        $ctrl_valore_local = "";
                        }
                    break;

                // ============================ NUMERIC ==================================
                case "N":
                    if ((trim($this->valore) != "" && $this->valore != $mask->default) ||
                        !$mask->zeroff) {
                        $valore_local = floatval($this->valore);
                        if (!$mask->negativo) {
                            $valore_local = abs($valore_local);
                            }
                        $ctrl_valore_local = $mask->prefisso.
                                             number_format($valore_local,
                                                           $mask->decimali,
                                                           $mask->separatore,
                                                           $mask->migliaia);
                        }
                    else {
                        $ctrl_valore_local = "";
                        }
                    break;

                // ============================== ALFA ===================================
                case "A":
                    if (count($mask->valori) > 0) {
                        foreach ($mask->valori as $idx => $singolo_valore) {
                            if ($idx == substr($this->valore, 0, $mask->dimensione)) {
                                $ctrl_valore_local = $singolo_valore;
                                break;
                                }
                            }
                        }
                    else {
                        $ctrl_valore_local = substr($this->valore, 0, $mask->dimensione);
                        }
                    break;

                // =========================== STRUCTURE =================================
                case "S":
                    $ctrl_valore_local = var_export($this->valore, true);
                    break;

                // ============================ LOGICAL ==================================
                case "L":
                    $ctrl_valore_local = ($this->valore ? true : false);
                    break;

                // ============================= OTHER ===================================
                default:
                    $ctrl_valore_local = $this->valore;
                    break;
                }
            }
        if ($GLOBALS['jxjs']) {
            return addcslashes($ctrl_valore_local, "'\\\n\r");
            }
        else {
            return htmlspecialchars($ctrl_valore_local, ENT_QUOTES, $app->chr_encoding);
            }

        }


    /**
     * Returns enabled status for the control according with view data context
     *
     * @return boolean
     */
    function is_enabled() {

        if ($this->modificabile) {
            $enabled = true;
            $prg     = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
            // _____________________________________ If control field is from a dbview ___
            if ($this->task && $this->task != "prg§_§var") {
                $view = $prg->contesto[$this->task];
                // ________________ If dbview has current record or is in INSERT state ___
                if (!$view->recordset[0] && $view->status != "I") {
                    $enabled = false;
                    }
                }
            }
        else {
            $enabled = false;
            }
        return $enabled;

        }


    /**
     * Returns visibility status for the control according with parents
     *
     * @return boolean
     */
    function is_visible() {

        if ($this->visibile) {
            $visible = true;
            $prg     = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
            // _________________________________________ If control field has a parent ___
            if ($this->padre) {
                $visible = $prg->form[$this->form]->controlli[$this->padre]->is_visible();
                }
            else {
                $visible = !$prg->form[$this->form]->chiusa;
                }
            }
        else {
            $visible = false;
            }
        return $visible;

        }


    /**
     * Returns active status for the control according with form status
     *
     * @return boolean
     */
    function is_active() {

        // _________________________________________________________ If form is active ___
        return $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione]->
                form[$this->form]->attiva;

        }


    /**
     * Check if control dependences are changed since last output send
     *
     */
    function check_dependences() {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$this->id_esecuzione];
        $chs = $prg->changes;
        $dps = $this->dependences;
        if (is_array($dps)) {
            if (isset($dps['_o2SESSION'])) {
                return true;
                }
            if ($chs) {
                foreach($dps as $view => $fields) {
                    if (isset($chs[$view]) && $chs[$view]) {
                        if ($chs[$view] === true) {
                            return true;
                            }
                        elseif ($dps[$view] === false) {
                            return true;
                            }
                        else {
                            foreach($fields as $field => $dummy_val) {
                                if (isset($chs[$view][$field])) {
                                    return true;
                                    }
                                }
                            }
                        }
                    }
                }
            return false;
            }
        else {
            return false;
            }

        }


    // ============================== METODI PUBBLICI ====================================
    function x($x = 10){

        if (!$this->expand_x && !$this->custom_x) {
            $this->x = intval($x);
            }
        return true;

        }


    function y($y = 10){

        if (!$this->expand_y && !$this->custom_y) {
            $this->y = intval($y);
            }
        return true;

        }


    function larghezza($larghezza = 10){

        if (!$this->expand_width && !$this->custom_width) {
            $this->larghezza = intval($larghezza);
            }
        return true;

        }


    function altezza($altezza = 10){

        if (!$this->expand_height && !$this->custom_height) {
            $this->altezza = intval($altezza);
            }
        return true;

        }


    function modificabile($modificabile = true){

        $this->modificabile = ($modificabile ? true : false);
        return true;

        }


    function visibile($visibile = true){

        $app      = $_SESSION['o2_app'];
        $form     = $app->istanze_prg[$this->id_esecuzione]->form[$this->form];
        $visibile = ($visibile ? true : false);
        if ($this->visibile === $visibile) {
            $form->force_resize = false;
            }
        else {
            $form->force_resize = true;
            }
        $this->visibile = $visibile;
        return true;

        }


    /**
     * Control properties for container
     *
     * Container Multipage & Flowbox
     *  - list of pages
     *
     * Container Table
     *  [0]  : Row
     *  [1]  : Start column
     *  [2]  : End column
     *  [3]  : Title
     *  [4]  : Hide sort button
     *  [5]  : CSS cell
     *  [6]  : ??? Not used ???
     *  [7]  : CSS hader
     *  [8]  : Footer
     *  [9]  : CSS footer
     *  [10] : Tooltip
     *  [11] : Action
     *
     */
    function info_padre($info_padre = null){

        $app = $_SESSION['o2_app'];
        if ($info_padre === null) {
            $info_padre = array();
            }
        $parent = $app->istanze_prg[$this->id_esecuzione]->form[$this->form]->
                   controlli[$this->padre];
        if ($parent->tipo_ctrl == 'tab') {
            // ____________________________ Custom columns order: not replace position ___
            if ($parent->custom_colsord) {
                $info_padre[0] = $this->info_padre[0];
                $info_padre[1] = $this->info_padre[1];
                $info_padre[2] = $this->info_padre[2];
                }
            else {
                $info_padre[0] = intval($info_padre[0]);
                $info_padre[1] = intval($info_padre[1]);
                $info_padre[2] = intval($info_padre[2]);
                }
            }
        $this->info_padre = $info_padre;
        return true;

        }


    function maschera($maschera = ""){

        if ($maschera && $_SESSION['o2_app']->maschere[$maschera]) {
            $this->maschera = $_SESSION['o2_app']->maschere[$maschera];
            }
        return true;

        }


    /**
     * Imposta un'azione da eseguire alla modifica del controllo
     *
     * @param  string $azione
     * @return boolean
     */
    function azione($azione = "") {

        $this->azione_submit = $azione;
        return true;

        }


    /**
     * Sets control widing style
     *
     * !!! Deprecated !!!
     *
     * @param string $wide_flag
     */
    function wide($wide_flag = "3") {

        o2_exception::deprecate('wide', 'expand', 'property');
        switch ($wide_flag) {
            case "1":
                $this->wide = "H";
                break;
            case "2":
                $this->wide = "V";
                break;
            case "3":
                $this->wide = "B";
                break;
            default:
                $this->wide = "";
                break;
            }

        }


    /**
     * Sets control expanding style
     *
     * @param string $wide_flag
     */
    function expand($expand = "") {

        $this->expand = $expand;

        }


    /**
     * Set control as last focused control in program.
     * This method is equivalent to setting o2form.lastform and o2form.lastctrl fields
     * by javascript.
     *
     */
    function set_focus() {

        $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione]->set_caret($this->form,
                                                                          $this->nome);

        }


    /**
     * Return TRUE if control is activable, by zoom-prg (from model) or by zoom-action.
     *
     */
    function is_zoomable() {

        if ($_SESSION['o2_app']->zoom) {
            // ________________________________________________ Zoom action on control ___
            if ($this->zoom_act) {
                return true;
                }
            // __________________________________________ Zoom program from data model ___
            elseif ($this->maschera->zoom_prg) {
                return true;
                }
            }
        // ___________________________________________________________________ No zoom ___
        return false;

        }


    /**
     * Activate the zoom program for the control, if any, passing field as parameter.
     * If a submit-on-change action is present on the control, that action is executed
     * after zoom-action end.
     * If control is combolist from view, view is refreshed after zoom-action end.
     *
     */
    function zoom() {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$this->id_esecuzione];
        // ____________________________________________________ Zoom action on control ___
        if ($this->zoom_act) {
            $prg->esegui_azione($this->zoom_act, true);
            }
        // ______________________________________________ Zoom program from data model ___
        elseif ($this->maschera->zoom_prg) {
            // _______________________________________ Action name includes prg exe-id ___
            $act_name     = $this->nome.$this->id_esecuzione."_zoom";
            $exe_count    = count($app->esecuzioni);
            $is_enabled   = $this->is_enabled();
            $this->valore = $prg->contesto[$this->task]->campo($this->campo_in_ctrl);
            $fld_value    = $this->sql2ctrl();
            $ctrl_field   = ($is_enabled ?
                             $this->task."§§".$this->campo_in_ctrl :
                             $fld_value);
            // _________________________________________________________ Define action ___
            if (!is_callable($prg->nome."§§".$act_name."_act")) {
                $prg_exe = $_SESSION['o2_app']->progressivo_istanze;
                /*
                 * <prg>§§<action>_cond() is a function that returns TRUE if field value
                 * is changed: it is used as condition to run submit-on-change action.
                 */
                $act_def = "function ".$prg->nome."§§".$act_name."_cond() { ".
                           "return (\$_SESSION['o2_app']->istanze_prg[".$prg_exe.
                           "]->contesto['".$this->task."']->campo('".$this->campo_in_ctrl.
                           "') != '".$fld_value."'); } ".
                           "function ".$prg->nome."§§".$act_name.
                           "_act(\$o2exe) { \$o2exe->s(1) && o2act::call(\"".
                           $this->maschera->zoom_prg."\", false, \"".$ctrl_field.
                           "\") && \$o2exe->e(); ".
                           ($this->azione_submit && $is_enabled ?
                           "\$o2exe->s(2) && (".$prg->nome."§§".$act_name.
                           "_cond() || \$o2exe->e()) && o2act::exe(\"".
                           $this->azione_submit."\") && \$o2exe->e(); " : "").
                           ($this->tipo_ctrl == "listcombo" && $this->items_view ?
                           "\$o2exe->s(3) && o2act::view(\"".$this->items_view.
                           "\", \"1\") && \$o2exe->e(); " : "")."}";
                eval($act_def);
                }
            // _____________________________________________ If action stops (on-line) ___
            if (!$prg->esegui_azione($act_name, true)) {
                // _______________________________ Recover execution started by action ___
                $exe_act = $app->esecuzioni[count($app->esecuzioni) - $exe_count - 1];
                // _______________________ Set definition script on action (HTML only) ___
                $exe_act->def_script = $act_def;
                }
            }
        // ____________________________________________ No ZOOM program/action defined ___
        else {
            throw new o2_exception("Cannot find activation action or program ".
                                   "for control <i>".$this->nome.
                                   "</i> requested for zoom.",
                                   o2error_UNKNOWNACTION);
            }

        }


    /**
     * Set action to be executed and expression to be displayed on popup on control
     *
     * NOTE: Second parameter $act is implemented but not exported by Interface
     *
     * @param string $exp
     * @param string $act
     */
    function popup($exp, $act = false) {

        $this->popup_exp = $exp;
        $this->popup_act = $act;

        }


    /**
     * Returns JS-style parameters for the control
     *
     * @param  string  $extraProps   Extra specific props to be included for control
     * @param  boolean $as_obj       If TRUE props are returned as JS object, else as HTML
     *                               attribute content
     * @return string
     */
    function js_props($extraProps = "", $as_obj = false) {

        $app        = $_SESSION['o2_app'];
        $prg_local  = $app->istanze_prg[$this->id_esecuzione];
        $form_local = $prg_local->form[$this->form];
        $parent     = ($this->padre ? $form_local->controlli[$this->padre] : false);
        $nav_local  = "";
        $log        = "";
        if ($this->task) {
            // _________________________________________ Table with internal navigator ___
            if (($this->tipo_ctrl == 'tab') && $this->internal_nav) {
                $nav_local = $this->internal_nav->nome;
                }
            // ______________________________ Control in table with internal navigator ___
            elseif ($parent && ($parent->tipo_ctrl == 'tab') && $parent->internal_nav) {
                $nav_local = $parent->internal_nav->nome;
                }
            // ____________________________ Look for a navigator matching control view ___
            else {
                foreach ($form_local->controlli as $single_ctrl) {
                    if ($single_ctrl->tipo_ctrl == 'navigator' &&
                        $single_ctrl->visibile                 &&
                        $single_ctrl->task == $this->task) {
                        $nav_local = $single_ctrl->nome;
                        break;
                        }
                    }
                }
            // _____________________________________ Check active log level on control ___
            if ($this->log_level && !$this->tab_row) {
                $log = ',log:!0';
                }
            }
        // _________________________________________________ Multilanguage translation ___
        if ($app->multilang) {
            $msg = $app->translate($this->nome,
                                   'msg_message',
                                   0,
                                   $this->confirm_message);
            }
        else {
            $msg = $this->confirm_message;
            }
        return  ($as_obj ? "{" : "o2=\"").
                "e:".$this->id_esecuzione.
                ",c:'".$this->nome.
                "',cT:'".$this->tipo_ctrl.
                "',v:'".$this->task.
                "',f:'".$this->form.
                "',pT:'".($parent ? $parent->tipo_ctrl : 'form')."'".
                ($this->maschera ?
                 ",dT:'".$this->maschera->tipo.
                 "',m:'".addslashes($this->maschera->maschera).
                 "',s:".intval($this->maschera->dimensione) : '').
                $log.
                ",fret:".($this->forza_submit ? '!0' : '!1').
                ($this->tab_row ? ',tabRow:!0' : '').
                ",nav:'".$nav_local.
                ($this->css ? "',cssc:'".$this->css : "").
                (strpos($extraProps, "msg:'") !== false ?
                 "'" :
                 "',msg:'".($as_obj ?
                            addcslashes($msg, "\r\n\'\"\\") :
                            str_replace("'", "\'", addcslashes($msg, "\r\n")))."'").
                ($this->popup_exp ? ",puact:'".$this->popup_act.
                                    "',puexp:'".$this->popup_exp."'" : "").
                ($extraProps ? ','.$extraProps : '').
                ($as_obj ? "}" : "\"\n");

        }

    }


/**
 * Label text control
 *
 */
class o2_ctrl_label extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $label = ""; /* Label text                                                   */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css   = "o2_ctrl_label"; /* Whole control <DIV>                             */
    public $no_id = false;           /* Used by tables for NOT current rows             */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl = "label";

        }


    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_label'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_label';
            }
        return true;

        }


    /**
     * Set text for the label
     *
     * @param string $label   Text to be displayed in the label control
     */
    function label($label = "") {

        $this->label = $label;
        return true;

        }


    /**
     * Disegna il controllo label
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_label($this);
            }
        else {
            o2html::ctrl_label($this);
            }

        }

    }


/**
 * Horizontal line divider
 *
 */
class o2_ctrl_line extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $css = "o2_ctrl_line"; /* Whole control <HR>                                 */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl = "line";

        }


    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_separatore'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_separatore';
            }
        return true;

        }


    /**
     * Disegna il controllo line-separator
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_line($this);
            }
        else {
            o2html::ctrl_line($this);
            }

        }

    }


/**
 * Navigator control
 *
 */
class o2_ctrl_navigator extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $direction          = ""; /* [V] for vertical, any other value for horizontal*/
    /*     _____ Actions ______________________________________________________________ */
    public $act_new            = false; /* "New" button                                 */
    public $act_del            = false; /* "Delete" button                              */
    public $act_save           = false; /* "Save" button                                */
    public $act_undo           = false; /* "Undo" button                                */
    public $act_select         = false; /* "Select" button                              */
    public $act_detail         = false; /* "Detail" button                              */
    /*     _____ Confirm messages _____________________________________________________ */
    public $msg_new            = ""; /* "New" button                                    */
    public $msg_del            = ""; /* "Delete" button                                 */
    public $msg_save           = ""; /* "Save" button                                   */
    public $msg_undo           = ""; /* "Undo" button                                   */
    public $msg_select         = ""; /* "Select" button                                 */
    public $msg_detail         = ""; /* "Detail" button                                 */
    /*     _____ Visibility conditions ________________________________________________ */
    public $vis_new            = true; /* "New" button                                  */
    public $vis_del            = true; /* "Delete" button                               */
    public $vis_save           = true; /* "Save" button                                 */
    public $vis_undo           = true; /* "Undo" button                                 */
    public $vis_select         = true; /* "Select" button                               */
    public $vis_detail         = true; /* "Detail" button                               */
    public $vis_nav            = true; /* Navigation buttons block                      */
    /*     _____ Habilitation conditions ______________________________________________ */
    public $enable_new         = true; /* "New" button                                  */
    public $enable_del         = true; /* "Delete" button                               */
    public $enable_save        = true; /* "Save" button                                 */
    public $enable_undo        = true; /* "Undo" button                                 */
    public $enable_select      = true; /* "Select" button                               */
    public $enable_detail      = true; /* "Detail" button                               */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css_space          = "o2_ctrl_nav_space";  /* Whole control space <TABLE>   */
    public $css_i              = "o2_ctrl_nav_i";      /* Slide bar <TD>                */
    public $css_e              = "o2_ctrl_nav_e";      /* Slide space <TD>              */
    public $css_new            = "o2_ctrl_nav_new";    /* "New" button <DIV>            */
    public $css_del            = "o2_ctrl_nav_del";    /* "Delete" button <DIV>         */
    public $css_save           = "o2_ctrl_nav_save";   /* "Save" button <DIV>           */
    public $css_undo           = "o2_ctrl_nav_undo";   /* "Undo" button <DIV>           */
    public $css_select         = "o2_ctrl_nav_sele";   /* "Select" button <DIV>         */
    public $css_detail         = "o2_ctrl_nav_det";    /* "Detail" button <DIV>         */
    public $css_primo          = "o2_ctrl_nav_first";  /* "First" button <DIV>          */
    public $css_precedente_pag = "o2_ctrl_nav_prevpg"; /* "Previous page" button <DIV>  */
    public $css_precedente     = "o2_ctrl_nav_prev";   /* "Previous" button <DIV>       */
    public $css_successivo     = "o2_ctrl_nav_next";   /* "Next" button <DIV>           */
    public $css_successiva_pag = "o2_ctrl_nav_nextpg"; /* "Next page" button <DIV>      */
    public $css_ultimo         = "o2_ctrl_nav_last";   /* "Last" button <DIV>           */
    /*     _____ OPTIONS ______________________________________________________________ */
    public $effect_on_over     = false; /* Enlargement effect for buttons on mouse over */


    /**
     * Costruttore
     *
     * @return o2_ctrl_navigator
     */
    function __construct() {

        $this->tipo_ctrl      = "navigator";
        $this->forza_submit   = true;
        $app                  = $_SESSION['o2_app'];
        $this->msg_del        = $this->msg2alert($app->msg_del);
        $this->msg_undo       = $this->msg2alert($app->msg_undo);
        // ______________________________________________ Effects on navigator buttons ___
        $this->effect_on_over = $app->option_get("nav_effects");

        }


    function vertical() {

        $this->direction = "V";
        foreach (get_object_vars($this) as $var_index => $var_value) {
            if (substr($var_index, 0, 3) == "css" &&
                substr($var_value, 0, 12) == "o2_ctrl_nav_") {
                $this->$var_index = "o2_ctrl_navv_".substr($var_value, 12);
                }
            }

        }


    function msg2alert($msg) {

        return addcslashes($msg, "\n\r'");

        }


    function act_new($action) {

        $this->act_new = $action;
        return true;

        }


    function act_del($action) {

        $this->act_del = $action;
        return true;

        }


    function act_save($action) {

        $this->act_save = $action;
        return true;

        }


    function act_undo($action) {

        $this->act_undo = $action;
        return true;

        }


    function act_select($action) {

        $this->act_select = $action;
        return true;

        }


    function act_detail($action) {

        $this->act_detail = $action;
        return true;

        }


    function confirm_new($msg = "") {

        $this->msg_new = $this->msg2alert($msg);
        return true;

        }


    function confirm_del($msg = "") {

        $this->msg_del = $this->msg2alert($msg);
        return true;

        }


    function confirm_save($msg = "") {

        $this->msg_save = $this->msg2alert($msg);
        return true;

        }


    function confirm_undo($msg = "") {

        $this->msg_undo = $this->msg2alert($msg);
        return true;

        }


    function confirm_select($msg = "") {

        $this->msg_select = $this->msg2alert($msg);
        return true;

        }


    function confirm_detail($msg = "") {

        $this->msg_detail = $this->msg2alert($msg);
        return true;

        }


    function visible_nav($visible = true) {

        $this->vis_nav = $visible;
        return true;

        }


    function visible_new($visible = true) {

        $this->vis_new = $visible;
        return true;

        }


    function visible_del($visible = true) {

        $this->vis_del = $visible;
        return true;

        }


    function visible_save($visible = true) {

        $this->vis_save = $visible;
        return true;

        }


    function visible_undo($visible = true) {

        $this->vis_undo = $visible;
        return true;

        }


    function visible_select($visible = true) {

        $this->vis_select = $visible;
        return true;

        }


    function visible_detail($visible = true) {

        $this->vis_detail = $visible;
        return true;

        }


    function enable_new($enabled = true) {

        $this->enable_new = $enabled;
        return true;

        }


    function enable_del($enabled = true) {

        $this->enable_del = $enabled;
        return true;

        }


    function enable_save($enabled = true) {

        $this->enable_save = $enabled;
        return true;

        }


    function enable_undo($enabled = true) {

        $this->enable_undo = $enabled;
        return true;

        }


    function enable_select($enabled = true) {

        $this->enable_select = $enabled;
        return true;

        }


    function enable_detail($enabled = true) {

        $this->enable_detail = $enabled;
        return true;

        }


    function css_space($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_space = ($this->direction != 'V' ?
                                    'o2_ctrl_nav_space' :
                                    'o2_ctrl_navv_space').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_space = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_space = ($this->direction != 'V' ?
                                'o2_ctrl_nav_space' :
                                'o2_ctrl_navv_space');
            }
        return true;

        }


    function css_i($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_i = ($this->direction != 'V' ?
                                'o2_ctrl_nav_i' :
                                'o2_ctrl_navv_i').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_i = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_i = ($this->direction != 'V' ? 'o2_ctrl_nav_i' : 'o2_ctrl_navv_i');
            }
        return true;

        }


    function css_e($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_e = ($this->direction != 'V' ?
                                'o2_ctrl_nav_e' :
                                'o2_ctrl_navv_e').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_e = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_e = ($this->direction != 'V' ? 'o2_ctrl_nav_e' : 'o2_ctrl_navv_e');
            }
        return true;

        }


    function css_new($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_new = ($this->direction != 'V' ?
                                'o2_ctrl_nav_new' :
                                'o2_ctrl_navv_new').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_new = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_new = ($this->direction != 'V' ?
                              'o2_ctrl_nav_new' :
                              'o2_ctrl_navv_new');
            }
        return true;

        }


    function css_del($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_del = ($this->direction != 'V' ?
                                'o2_ctrl_nav_del' :
                                'o2_ctrl_navv_del').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_del = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_del = ($this->direction != 'V' ?
                              'o2_ctrl_nav_del' :
                              'o2_ctrl_navv_del');
            }
        return true;

        }


    function css_save($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_save = ($this->direction != 'V' ?
                                'o2_ctrl_nav_save' :
                                'o2_ctrl_navv_save').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_save = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_save = ($this->direction != 'V' ?
                              'o2_ctrl_nav_save' :
                              'o2_ctrl_navv_save');
            }
        return true;

        }


    function css_undo($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_undo = ($this->direction != 'V' ?
                                   'o2_ctrl_nav_undo' :
                                   'o2_ctrl_navv_undo').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_undo = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_undo = ($this->direction != 'V' ?
                               'o2_ctrl_nav_undo' :
                               'o2_ctrl_navv_undo');
            }
        return true;

        }


    function css_select($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_select = ($this->direction != 'V' ?
                                     'o2_ctrl_nav_sele' :
                                     'o2_ctrl_navv_sele').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_select = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_select = ($this->direction != 'V' ?
                                 'o2_ctrl_nav_sele' :
                                 'o2_ctrl_navv_sele');
            }
        return true;

        }


    function css_detail($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_detail = ($this->direction != 'V' ?
                                     'o2_ctrl_nav_det' :
                                     'o2_ctrl_navv_det').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_detail = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_detail = ($this->direction != 'V' ?
                                 'o2_ctrl_nav_det' :
                                 'o2_ctrl_navv_det');
            }
        return true;

        }


    function css_first($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_primo = ($this->direction != 'V' ?
                                    'o2_ctrl_nav_first' :
                                    'o2_ctrl_navv_first').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_primo = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_primo = ($this->direction != 'V' ?
                                'o2_ctrl_nav_first' :
                                'o2_ctrl_navv_first');
            }
        return true;

        }


    function css_prevpg($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_precedente_pag = ($this->direction != 'V' ?
                                             'o2_ctrl_nav_prevpg' :
                                             'o2_ctrl_navv_prevpg').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_precedente_pag = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_precedente_pag = ($this->direction != 'V' ?
                                         'o2_ctrl_nav_prevpg' :
                                         'o2_ctrl_navv_prevpg');
            }
        return true;

        }


    function css_prev($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_precedente = ($this->direction != 'V' ?
                                         'o2_ctrl_nav_prev' :
                                         'o2_ctrl_navv_prev').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_precedente = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_precedente = ($this->direction != 'V' ?
                                     'o2_ctrl_nav_prev' :
                                     'o2_ctrl_navv_prev');
            }
        return true;

        }


    function css_next($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_successivo = ($this->direction != 'V' ?
                                         'o2_ctrl_nav_next' :
                                         'o2_ctrl_navv_next').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_successivo = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_successivo = ($this->direction != 'V' ?
                                     'o2_ctrl_nav_next' :
                                     'o2_ctrl_navv_next');
            }
        return true;

        }


    function css_nextpg($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_successiva_pag = ($this->direction != 'V' ?
                                             'o2_ctrl_nav_nextpg' :
                                             'o2_ctrl_navv_nextpg').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_successiva_pag = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_successiva_pag = ($this->direction != 'V' ?
                                         'o2_ctrl_nav_nextpg' :
                                         'o2_ctrl_navv_nextpg');
            }
        return true;

        }


    function css_last($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_ultimo = ($this->direction != 'V' ?
                                     'o2_ctrl_nav_last' :
                                     'o2_ctrl_navv_last').$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_ultimo = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_ultimo = ($this->direction != 'V' ?
                                 'o2_ctrl_nav_last' :
                                 'o2_ctrl_navv_last');
            }
        return true;

        }


    /**
     * Display navigator control
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_nav($this);
            }
        else {
            o2html::ctrl_nav($this);
            }

        }

    }

/**
 * Edit control
 *
 */
class o2_ctrl_edit extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $password = false; /* If password protected value ("******")                 */
    public $zoom_act = false; /* Activation (zoom) action on control                    */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css      = "o2_ctrl_edit"; /* Whole control <INPUT>                         */


    /** Costruttore
     *
     * @return o2_ctrl_edit
     */
    function __construct() {

        $this->tipo_ctrl = "edit";

        }


    /**
     * Imposta il css per il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_edit'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_edit';
            }
        return true;

        }


    /**
     * Imposta il mascheramento del valore del campo come password
     *
     * @param  boolean $abilitato
     * @return boolean
     */
    function pswd($abilitato) {

        $this->password = ($abilitato ? true : false);
        return true;

        }


    /**
     * Imposta l'azione $action_name as zoom-action for control
     *
     * @param  boolean $action_name   Name of program action to execute on zoom
     * @return boolean
     */
    function act_zoom($action_name) {

        $this->zoom_act = $action_name;
        return true;

        }


    /**
     * Forza il ritorno al server all'on-change del controllo
     *
     * @param  boolean $on_change
     * @return boolean
     */
    function on_change($on_change = false) {

        $this->forza_submit = ($on_change ? true : false);
        return true;

        }


    /**
     * Disegna il controllo edit
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_edit($this);
            }
        else {
            o2html::ctrl_edit($this);
            }

        }

    }


/**
 * Text area control
 *
 */
class o2_ctrl_text extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $html     = false; /* If text area is to be displayed as WYSIWYG editor      */
    public $zoom_act = false; /* Activation (zoom) action on control                    */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css = "o2_ctrl_text"; /* Whole control class <TEXTAREA>                     */


    /**
     * Costruttore
     *
     * @return o2_ctrl_text
     */
    function __construct() {

        $this->tipo_ctrl = "text";

        }


    /**
     * Imposta la classe di stile per il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_text'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_text';
            }
        return true;

        }


    /**
     * Imposta a true la specifica di html editor per il controllo
     *
     * @param boolean $vero
     */
    function htmlarea($vero = true) {

        $this->html = $vero;

        }


    /**
     * Imposta l'azione $action_name as zoom-action for control
     *
     * @param  boolean $action_name   Name of program action to execute on zoom
     * @return boolean
     */
    function act_zoom($action_name) {

        $this->zoom_act = $action_name;
        return true;

        }


    /**
     * Forza il ritorno al server all'on-change del controllo
     *
     * @param  boolean $on_change
     * @return boolean
     */
    function on_change($on_change = false) {

        $this->forza_submit = ($on_change ? true : false);
        return true;

        }


    /**
     * Disegna il controllo text-area
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_text($this);
            }
        else {
            o2html::ctrl_text($this);
            }

        }

    }


/**
 * Checkbox control
 *
 */
class o2_ctrl_check extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $css      = "o2_ctrl_check"; /* Whole control <INPUT>                        */
    public $selector = false;           /* If control is a view selector                */


    /**
     * Costruttore
     *
     * @return o2_ctrl_check
     */
    function __construct() {

        $this->tipo_ctrl = "checkbox";

        }


    /**
     * Imposta la classe di stile per il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_check'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_check';
            }
        return true;

        }


    /**
     * Forza il ritorno al server all'on-change del controllo
     *
     * @param  boolean $on_change
     * @return boolean
     */
    function on_change($on_change = false) {

        $this->forza_submit = ($on_change ? true : false);
        return true;

        }


    /**
     * Set confirm message text for action execution
     *
     * @param string $message
     */
    function confirm_message($message = "") {

        $this->confirm_message = $message;
        return true;

        }


   /**
     * Disegna il controllo checkbox
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_check($this);
            }
        else {
            o2html::ctrl_check($this);
            }

        }

    }


/**
 * Combobox and listbox controls
 *
 */
class o2_ctrl_listcombo extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $righe       = 1;     /* Rows number: [1]=combobox [>1]=listbox              */
    public $valori      = 0;     /* Values list in the form ->valori[cod]=[label]       */
    public $items_view  = null;  /* View name for items retrieving                      */
    public $items_code  = null;  /* Name of field to use as code in items list          */
    public $items_desc  = null;  /* Name of field to use as description in items list   */
    public $zoom_act    = false; /* Activation (zoom) action on control                 */
    public $untraslate  = false; /* Mark control values to be NOT translated            */
    public $preset_list = false; /* Preset items to prepend to list from view           */
    public $error       = false; /* If value in field is not in control values list     */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css         = "o2_ctrl_listcombo"; /* Whole control <SELECT>                */


    /**
     * Costruttore
     *
     * @return o2_ctrl_listcombo
     */
    function __construct() {

        $this->tipo_ctrl = "listcombo";
        $this->valori    = array();

        }


    /**
     * Imposta la classe di stile per il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_listcombo'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_listcombo';
            }
        return true;

        }


    /**
     * Imposta il numero di righe del listbox. Se $righe=1 il controllo sarà un combobox
     *
     * @param  integer $righe
     * @return boolean
     */
    function righe($righe = 1) {

        $this->righe = intval($righe);
        return true;

        }


    /**
     * Imposta l'array dei valori per il list-combo
     *
     * @param  integer $valori
     * @return boolean
     */
    function valori($valori = null) {

        $app = $_SESSION['o2_app'];
        // ___________________ Per catturare le informazioni di view su o2_view2list() ___
        if (isset($app->defining_combo)) {
            unset($app->defining_combo);
            }
        if (is_array($valori)) {
            $new_values = array();
            foreach ($valori as $key => $val) {
                $new_values[jx_encode($key)] = jx_encode($val);
                }
            $valori = $new_values;
            }
        if ($valori !== null) {
            $this->valori = $valori;
            }
        return true;

        }


    /**
     * Imposta l'azione $action_name as zoom-action for control
     *
     * @param  boolean $action_name   Name of program action to execute on zoom
     * @return boolean
     */
    function act_zoom($action_name) {

        $this->zoom_act = $action_name;
        return true;

        }


    /**
     * Forza il ritorno al server all'on-change del controllo
     *
     * @param  boolean $on_change
     * @return boolean
     */
    function on_change($on_change = false) {

        $this->forza_submit = ($on_change ? true : false);
        return true;

        }


    /**
     * Set confirm message text for action execution
     *
     * @param string $message
     */
    function confirm_message($message = "") {

        $this->confirm_message = $message;
        return true;

        }


    /**
     * Disegna il controllo listcombo
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_listcombo($this);
            }
        else {
            o2html::ctrl_listcombo($this);
            }

        }


    /**
     * Returns control description text
     *
     * Description may be saved in dataset, to avoid re-executing the query
     *
     */
    function get_desc() {

        $app          = $_SESSION['o2_app'];
        $prg          = $app->istanze_prg[$this->id_esecuzione];
        $view         = $prg->contesto[$this->task];
        $this->valore = $view->corrente[$this->campo_in_ctrl];
        $value        = trim($this->ctrl2sql());
        $ret          = "";
        $this->error  = false;
        // ______________________________________ If combo description has been cached ___
        if (isset($view->corrente[$this->campo_in_ctrl."_jxcbdesc"])) {
            $ret = $view->corrente[$this->campo_in_ctrl."_jxcbdesc"];
            }
        // ________________________________________________________ Decode description ___
        else {
            // __________________________________________________ Items list from view ___
            if ($this->items_view && $this->righe == 1) {
                // ______________________________________ Custom items (added to view) ___
                if ($this->preset_list && array_key_exists($value, $this->preset_list)) {
                    $ret = $this->preset_list[$value];
                    }
                // ___________________________________________________ Items from view ___
                else {
                    $items_view_name = $this->items_view;
                    if (!$prg->contesto[$items_view_name]) {
                        $items_view_name.= "_o2list";
                        $items_view_name = "v".substr(md5($items_view_name), -5);
                        define($this->items_view."_o2self", $items_view_name);
                        }
                    $items_view       = $prg->contesto[$items_view_name];
                    $filter_code_name = $this->items_desc."_O2LUDESC";
                    if ($items_view->campi[$this->items_code]) {
                        $items_code = $items_view->campi[$this->items_code];
                        $items_view->campi[$filter_code_name]       = clone $items_code;
                        $items_view->campi[$filter_code_name]->nome = $filter_code_name;
                        $items_view->campi[$filter_code_name]->min  = $value;
                        $items_view->campi[$filter_code_name]->max  = $value;
                        }
                    else {
                        $items_code = $items_view->formule[$this->items_code];
                        $items_view->formule[$filter_code_name]       = clone $items_code;
                        $items_view->formule[$filter_code_name]->nome = $filter_code_name;
                        $items_view->formule[$filter_code_name]->min  = "'".$value."'";
                        $items_view->formule[$filter_code_name]->max  = "'".$value."'";
                        }
                    // _______ If view has not a recordset yet or WHERE clause changed ___
                    if (!$items_view->vista ||
                        $items_view->range2where($items_view->file->indice) ||
                        $items_view->range2formule()) {
                        $items_view->record_primo();
                        }
                    // ________________________________________ Value decoded by query ___
                    $found = (count($items_view->recordset) > 0);
                    if ($found) {
                        $ret = $items_view->campo($this->items_desc);
                        }
                    elseif ($value) {
                        $this->error = true;
                        $ret         = '['.$value.'] Value not in list';
                        }
                    if ($items_view->campi[$this->items_code]) {
                        unset($items_view->campi[$filter_code_name]);
                        }
                    else {
                        unset($items_view->formule[$filter_code_name]);
                        }
                    }
                // ______________________________________ Cache description in dataset ___
                $view->corrente[$this->campo_in_ctrl."_jxcbdesc"] = $ret;
                }
            else {
                $mask = $this->maschera;
                // __________________________________________ Values list from picture ___
                if (!count($this->valori) && is_array($mask->valori)) {
                    $this->valori = $mask->valori;
                    }
                foreach ($this->valori as $item_code => $item_desc) {
                    if ((string) $value === trim($item_code)) {
                        if ($app->multilang) {
                            $ret = $app->translate($this->nome,
                                                   'item',
                                                   $item_code,
                                                   $item_desc);
                            }
                        else {
                            $ret = $item_desc;
                            }
                        break;
                        }
                    }
                if (!$ret && $value) {
                    $this->error = true;
                    $ret         = '['.$value.'] Value not in list';
                    }
                }
            }
        // ________________ Return description formatted according with execution mode ___
        if ($GLOBALS['jxjs']) {
            return addcslashes($ret, "'\\\n\r");
            }
        else {
            return htmlspecialchars($ret, ENT_QUOTES, $app->chr_encoding);
            }

        }


    /**
     * Decode description to get an unique code value.
     *
     * NOTE: This memthod is replication of client-side implicit selection on TAB!
     *
     */
    function code_from_desc($desc_value) {

        $app   = $_SESSION['o2_app'];
        $prg   = $app->istanze_prg[$this->id_esecuzione];
        $view  = $prg->contesto[$this->task];
        $list  = $this->xlist(false,
                              $_REQUEST[$this->nome.$this->id_esecuzione."_desc"],
                              true);
        $clist = array_keys($list);
        $vlist = array_values($list);
        $nlist = count($list);
        // ____________________________________________ Blanked value and lank in list ___
        return (($desc_value == '' && $nlist > 0 && $vlist[0] == '') ||
                // _________________________ ... or select element if just one in list ___
                ($nlist == 1) ?
                // ______________________________________ Return first element in list ___
                $clist[0] :
                // __________________________________________ Else return actual value ___
                $this->valore);

        }


    /**
     * Returns items list in AJAX style.
     * Returned object has the form:
     *
     *    ret_obj = { f : if_first,
     *                l : if_last,
     *                s : { code1 : desc1,
     *                      code2 : desc2,
     *                      ...
     *                      coden : descn } }
     *
     * @param string   $paging       [P]revious|[N]ext set of records
     * @param string   $filter_txt   Text used to filter records
     * @param boolean  $as_array     Return list as an array, for server-side uses
     */
    function xlist($paging = false, $filter_txt = '', $as_array = false) {

        $app             = $_SESSION['o2_app'];
        $prg             = $app->istanze_prg[$this->id_esecuzione];
        $items_view_name = $this->items_view;
        $xlist           = array();
        // ___________________________________________________ Save actual instance id ___
        $exeid_save = $app->progressivo_istanze;
        // ____________________________________ Set instance to requested field prg-id ___
        $app->progressivo_istanze = $this->id_esecuzione;
        if (!$prg->contesto[$items_view_name]) {
            $items_view_name.= "_o2list";
            $items_view_name = "v".substr(md5($items_view_name), -5);
            define($this->items_view."_o2self", $items_view_name);
            }
        $items_view       = $prg->contesto[$items_view_name];
        $filter_desc_name = $this->items_desc."_O2LUDESC";
        if ($filter_txt) {
            if ($items_view->campi[$this->items_desc]) {
                $items_desc = $items_view->campi[$this->items_desc];
                $items_view->campi[$filter_desc_name]       = clone $items_desc;
                $items_view->campi[$filter_desc_name]->nome = $filter_desc_name;
                $items_view->campi[$filter_desc_name]->like = $filter_txt;
                }
            else {
                $items_desc = $items_view->formule[$this->items_desc];
                $items_view->formule[$filter_desc_name]       = clone $items_desc;
                $items_view->formule[$filter_desc_name]->nome = $filter_desc_name;
                $items_view->formule[$filter_desc_name]->like = "'".
                                                                addslashes($filter_txt).
                                                                "'";
                }
            }
        if ($paging) {
            // ______________________________________________________________ Not used ___
            if ($paging == "A") {
                $items_view->righe_vis = 1000;
                $items_view->record_primo();
                }
            // _____________________________________________________________ Next page ___
            elseif ($paging == "N") {
                $items_view->record_pg_avanti();
                }
            // _________________________________________________________ Previous page ___
            else {
                $items_view->record_pg_indietro();
                }
            }
        elseif (!($filter_txt || $paging) || $items_view->range_mod()) {
            $items_view->record_primo();
            }
        // _________________________________________________ Insert default blank line ___
        if ($items_view->offset_ini == 0 && !$filter_txt) {
            // _______________________________________ Get description for blank value ___
            /*
             * NOT USED: If default value has a not blank description, then it's
             *           impossible to select default value blanking combo description!
             *
            $value_save       = $this->valore;
            $this->valore     = $items_view->campi[$this->items_code]->maschera->default;
            $blank            = $this->get_desc();
            $this->valore     = $value_save;
            */
            if ($as_array) {
                $xlist[$items_view->campi[$this->items_code]->maschera->default] = '';
                }
            else {
                $body_local = "['".
                              $items_view->campi[$this->items_code]->maschera->default.
                               "',";
                $desc_list  = "['',";
                }
            }
        else {
            $body_local = "[";
            $desc_list  = "[";
            }
        $unique_code = array();
        // ___________________________ Add items from preset, if any and if first page ___
        if ($this->preset_list && !$items_view->offset_ini) {
            foreach ($this->preset_list as $item_code => $item_desc) {
                // ___________________________ Filter preset items by description text ___
                if ($filter_txt && strpos($item_desc, $filter_txt) === false) {
                    continue;
                    }
                if (!$unique_code[$item_code]) {
                    $unique_code[$item_code] = true;
                    if ($as_array) {
                        $xlist[$item_code] = $item_desc;
                        }
                    else {
                        $body_local.= "'".addslashes($item_code)."',";
                        $desc_list .= "'".addslashes($item_desc)."',";
                        }
                    }
                }
            }
        // _______________________________________________________ Add items from view ___
        foreach ($items_view->recordset as $single_record) {
            $item_code = $single_record[$this->items_code];
            if (!$unique_code[$item_code]) {
                $unique_code[$item_code] = true;
                if ($as_array) {
                    $xlist[$item_code] = $single_record[$this->items_desc];
                    }
                else {
                    $body_local.= "'".addslashes($item_code)."',";
                    $desc_list .= "'".addslashes(strtr($single_record[$this->items_desc],
                                                       "\n\r\t", "   "))."',";
                    }
                }
            }
        unset($unique_code);
        if ($items_view->campi[$filter_desc_name]) {
            unset($items_view->campi[$filter_desc_name]);
            }
        elseif ($items_view->formule[$filter_desc_name]) {
            unset($items_view->formule[$filter_desc_name]);
            }
        if ($items_view->offset_fine >= ($items_view->totale_record - 1)) {
            $items_view->vista     = false;
            $items_view->direzione = 0;
            }
        // ________________________________________________________ Reset prg instance ___
        $app->progressivo_istanze = $exeid_save;
        if ($as_array) {
            return $xlist;
            }
        else {
            $body_local = substr($body_local, 0, -1);
            $desc_list  = substr($desc_list, 0, -1);
            return ($body_local ?
                     "{f:".($items_view->offset_ini ? "!1" : "!0").
                     ",l:".($items_view->offset_fine < ($items_view->totale_record - 1) ?
                            "!1" :
                            "!0").
                     ",v:".$body_local."],d:".$desc_list."]}" :
                    "{f:!0,l:!0}");
            }
        }

    }


/**
 * Button control
 *
 */
class o2_ctrl_button extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $label     = ""; /* Label                                                    */
    public $style     = 0;  /* Style: [0]=button, [1]=href                              */
    public $key_event = ""; /* Key accelerator binded to button                         */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css       = "o2_ctrl_button"; /* Whole button <INPUT> (btn) <SPAN> (href)   */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl    = "button";
        $this->forza_submit = true;

        }


    function style($style = 0) {

        switch ($style) {
            case 1:
                $this->style = 1;
                $this->css   = "o2_ctrl_ref";
                break;
            default:
                $this->style = 0;
                $this->css   = "o2_ctrl_button";
                break;
            }

        }


    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = ($this->style ? "o2_ctrl_ref" : "o2_ctrl_button").$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = ($this->style ? "o2_ctrl_ref" : "o2_ctrl_button");
            }
        return true;

        }


    function label($label = "") {

        $this->label = $label;
        return true;

        }


    /**
     * Set confirm message text for action execution
     *
     * @param string $message
     */
    function confirm_message($message = "") {

        $this->confirm_message = $message;
        return true;

        }


    /**
     * Set keyboard shortcut for activating button (only function keys enabled: F1..F20)
     *
     * @param string $key
     */
    function shortcut($key) {

        $this->key_event = $key;

        }


    /**
     * Return true if a keyboard accelerator is binded to control
     *
     */
    function has_key_event() {

        return ($this->key_event ? true : false);

        }


    /**
     * Return keyboard accelerator code if one is binded to control
     *
     */
    function get_key_code() {

        return intval(substr($this->key_event, 1));

        }


    /**
     * Disegna il controllo button
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_button($this);
            }
        else {
            o2html::ctrl_button($this);
            }

        }

    }


/**
 * File upload control
 *
 */
class o2_ctrl_file extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $salva_con_nome = '';   /* Name to assign to file in uploads folder (may
                                      contain relative paths                            */
    public $multiple       = false; /* If control can accept multiple files upload      */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css            = 'o2_ctrl_file';     /* Whole control                       */
    public $css_btn        = 'o2_ctrl_file_btn'; /* Whole control when s-o-c is enabled */


    /**
     * Costruttore
     *
     * @return o2_ctrl_file
     */
    function __construct() {

        $this->tipo_ctrl = 'file';

        }


    /**
     * Set control for multiple files upload
     *
     * @return boolean
     */
    function multiple() {

        $this->multiple = true;
        return true;

        }


    /**
     * Imposta la classe di stile per il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = '') {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $css = 'o2_ctrl_file'.$css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $css = 'o2_ctrl_file';
            }
        // _________________________________ Set button style class when s-o-c enabled ___
        if ($this->forza_submit) {
            $css.= '_btn';
            }
        $this->css = $css;
        return true;

        }


    /**
     * Force submit on control change.
     * When s-o-c is enabled control style (CSS) is changed to "<class>_btn".
     *
     * @param  boolean $on_change
     * @return boolean
     */
    function on_change($on_change = false) {

        $this->forza_submit = ($on_change ? true : false);
        if ($this->forza_submit && substr($this->css, -4) != '_btn') {
            $this->css.= '_btn';
            }
        return true;

        }


    /**
     * Imposta il nome con cui salvare il file
     *
     * @param  string $nome
     * @return boolean
     */
    function save_as($nome = '') {

        $this->salva_con_nome = $nome;
        return true;

        }


    /**
     * Disegna il controllo di file upload
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_file($this);
            }
        else {
            o2html::ctrl_file($this);
            }

        }

    }


/**
 * Image control
 *
 */
class o2_ctrl_img extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $label      = ""; /* Tooltip text                                            */
    public $immagine   = ""; /* Name of image file to display                           */
    public $imgexp     = ""; /* Expression of name of image file to display             */
    public $imgoverexp = ""; /* Expression of name of file to display on mouse over     */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css        = "o2_ctrl_img"; /* Whole control <IMG>                          */


    /**
    * @return void
    * @desc Costruttore
    */
    function __construct() {

        $this->tipo_ctrl    = "img";
        $this->forza_submit = true;

        }


    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_img'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_img';
            }
        return true;

        }


    function label($label = "") {

        $this->label = $label;
        return true;

        }


    /**
     * Set confirm message text for action execution
     *
     * @param string $message
     */
    function confirm_message($message = "") {

        $this->confirm_message = $message;
        return true;

        }


    function immagine($immagine = "") {

        $this->immagine = $immagine;
        return true;

        }


    function imgexp($imgexp = "") {

        $this->imgexp = $imgexp;
        return true;

        }


    function imgoverexp($imgoverexp = "") {

        $this->imgoverexp = $imgoverexp;
        return true;

        }


    function src() {

        $img_local = ($this->imgexp != "" ? $this->imgexp : $this->immagine);
        $rnt_alias = $GLOBALS['o2_runtime']->alias;
        list($special_folder, $img_name) = explode("/", $img_local, 2);
        switch (strtolower($special_folder)) {
            case "<o2>":
            case "<jx>":
                return $GLOBALS['o2_runtime']->alias.$img_name;
                break;
            default:
                if (substr($img_local, 0, 1) != "/" &&
                    strtolower(substr($img_local, 0, 5)) != "http:") {
                return "img/".$img_local;
                    }
                else {
                    return $img_local;
                    }
                break;
            }

        }


    function srcover() {

        if ($this->imgoverexp) {
            $img_local = $this->imgoverexp;
            list($special_folder, $img_name) = explode("/", $img_local, 2);
            switch (strtolower($special_folder)) {
                case "<o2>":
                case "<jx>":
                    return $GLOBALS['o2_runtime']->alias.$img_name;
                    break;
                default:
                    if (substr($img_local, 0, 1) != "/" &&
                        strtolower(substr($img_local, 0, 5)) != "http:") {
                        return "img/".$img_local;
                        }
                    else {
                        return $img_local;
                        }
                    break;
                }
            }
        else {
            return "";
            }

        }


    /**
     * Disegna il controllo image
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_image($this);
            }
        else {
            o2html::ctrl_image($this);
            }

        }

    }


/**
 * Multipage control
 *
 */
class o2_ctrl_multipage extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $posizione_ctrls = "T";     /* Buttons position:
                                          [T]op [R]ight [B]ottom [L]eft [...]none       */
    public $label           = array(); /* Pages labels list in the form:
                                          ->label[page_index] = [label_text]            */
    public $pagine          = 1;       /* Number of pages                               */
    public $sent_pages      = array(); /* List of pages sent at last once               */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css_spazio_out  = "o2_ctrl_multi_out";  /* Whole external space <TABLE>     */
    public $css_spazio_in   = "o2_ctrl_multi_in";   /* Page container space <DIV>       */
    public $css_labels      = "o2_ctrl_multi_head"; /* Selctoion buttons header <DIV>   */
    public $css_label_on    = "o2_ctrl_multi_on";   /* Selected button <INPUT>          */
    public $css_label_off   = "o2_ctrl_multi_off";  /* Unselected buttons <INPUT>       */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl    = "multipage";
        $this->label        = array("default");
        $this->forza_submit = true;

        }


    /**
     * Imposta la classe di stile per la <table> che contiene il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css_space_out($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_spazio_out = 'o2_ctrl_multi_out'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_spazio_out = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_spazio_out = 'o2_ctrl_multi_out';
            }
        return true;

        }


    /**
     * Imposta la classe di stile per il <div> contenuto nel controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css_space_in($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_spazio_in = 'o2_ctrl_multi_in'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_spazio_in = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_spazio_in = 'o2_ctrl_multi_in';
            }
        return true;

        }


    /**
    * @return boolean
    * @param string $css
    * @desc Imposta la classe di stile per il bottone selezionato
    */
    function css_label_on($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_label_on = 'o2_ctrl_multi_on'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_label_on = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_label_on = 'o2_ctrl_multi_on';
            }
        return true;

        }


    /**
     * Imposta la classe di stile per i bottoni deselezionati
     *
     * @param  string $css
     * @return boolean
     */
    function css_label_off($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css_label_off = 'o2_ctrl_multi_off'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css_label_off = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css_label_off = 'o2_ctrl_multi_off';
            }
        return true;

        }


    /**
     * Imposta l'array dei titoli delle pagine
     *
     * @param  array $label
     * @return boolean
     */
    function label($label = null) {

        if (!$label) {
            $label = array("Page1");
            }
        $this->label = $label;
        return true;

        }


    /**
     * Imposta la posizione dei bottoni di selezione: [T]op [R]ight [B]ottom [L]eft
     *
     * @param  string $tipo
     * @return boolean
     */
    function buttons($tipo = "T") {

        $this->posizione_ctrls = $tipo;
        return true;

        }


    /**
     * Forza il ritorno al server all'on-change del controllo
     *
     * TODO: method to be removed
     *
     * @param  boolean $on_change
     * @return boolean
     */
    function on_change($on_change = false) { }


    /**
     * Set confirm message text for action execution
     *
     * @param string $message
     */
    function confirm_message($message = "") {

        $this->confirm_message = $message;
        return true;

        }


    /**
     * Disegna il controllo multipage
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_multipage($this);
            }
        else {
            // _________________________________________________ Reset sent pages list ___
            $this->sent_pages = array();
            o2html::ctrl_multipage($this);
            }

        }

    }


/**
 * Flowbox control
 *
 */
class o2_ctrl_flowbox extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $label = array(); /* Pages labels list in the form:
                                 ->label[page_index] = [label_text]                     */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css   = "o2_ctrl_box_out"; /*                                               */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl    = "flowbox";
        $this->label        = array("default");

        }


    /**
     * Imposta la classe di stile per il <div> che contiene il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_box_out'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_box_out';
            }
        return true;

        }


    /**
     * Imposta l'array dei titoli delle pagine
     *
     * @param  array $label
     * @return boolean
     */
    function label($label = null) {

        if (!$label) {
            $label = array("Page1");
            }
        $this->label = $label;
        return true;

        }


    /**
     * Disegna il controllo flowbox
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_flowbox($this);
            }
        else {
            o2html::ctrl_flowbox($this);
            }

        }

    }


/**
 * Frame control
 *
 */
class o2_ctrl_frame extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css   = "o2_ctrl_box_in"; /*                                                */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl    = "frame";

        }


    /**
     * Imposta la classe di stile per il <div> che contiene il controllo
     *
     * @param  string $css
     * @return boolean
     */
    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_box_in'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_box_in';
            }
        return true;

        }


    /**
     * Disegna il controllo frame
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_frame($this);
            }
        else {
            o2html::ctrl_frame($this);
            }

        }

    }


/**
 * HTML coded area
 *
 */
class o2_ctrl_html extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $html  = "";             /* HTML code to write in control                    */
    public $no_id = false;          /* Used by tables for NOT current rows              */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css   = "o2_ctrl_html"; /* Whole control space <DIV>                        */


    /**
    * @return void
    * @desc Costruttore
    */
    function __construct() {

        $this->tipo_ctrl = "html";

        }


    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_html'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_html';
            }
        return true;

        }


    function html($codice = "") {

        $this->html = $codice;
        return true;

        }


    /**
     * Visualizza il controllo
     *
     * @return boolean
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_area($this);
            }
        else {
            o2html::ctrl_area($this);
            }

        }

    }


/**
 * Iframe control
 *
 */
class o2_ctrl_doc extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $path = ""; /* Document URL                                                  */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css  = "o2_ctrl_doc"; /* Whole control space <IFRAME>                       */


    /**
     * Costruttore
     *
     */
    function __construct() {

        $this->tipo_ctrl = "doc";

        }


    function css($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'o2_ctrl_doc'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'o2_ctrl_doc';
            }
        return true;

        }


    function path($indirizzo = "") {

        $this->path = $indirizzo;
        return true;

        }


    /**
     * Visualizza il controllo
     *
     * @return boolean
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_doc($this);
            }
        else {
            o2html::ctrl_doc($this);
            }

        }

    }


/**
 * DBgrid control
 *
 */
class o2_ctrl_tab extends o2_ctrl {

    /*     ===== PROPERTIES =========================================================== */
    public $colonne        = 0;       /* Actual number of colums                        */
    public $lines_per_row  = 1;       /* Number on HTML lines in each row               */
    public $norec_msg      = "";      /* Message to display on no record                */
    public $row_marker     = true;    /* If is row marker (1st left column) active      */
    public $footer         = false;   /* If tab has footer (at last one column has it)  */
    public $gp_vis_cols    = true;    /* grid-plus change visible columns               */
    public $gp_ord_cols    = true;    /* grid-plus change columns order                 */
    public $gp_size_cols   = true;    /* grid-plus change columns size                  */
    public $gp_filter_recs = true;    /* grid-plus filter records                       */
    public $gp_sort_recs   = true;    /* grid-plus sort data by columns                 */
    public $gp_rows_num    = true;    /* grid-plus set rows number                      */
    public $gp_export      = true;    /* grid-plus allow export data                    */
    public $gp_delete      = true;    /* grid-plus allow delete filtered records        */
    /*     _____ SERVICE VARIABLES ____________________________________________________ */
    public $prg_obj        = null;    /* Reference to obj o2_prg containing the table   */
    public $view_obj       = null;    /* Reference to obj o2_view used by table         */
    public $form_obj       = null;    /* Reference to obj o2_form containing the table  */
    public $riga_corrente  = false;   /* If rendering current record row                */
    public $view_shared    = false;   /* If used view is shared with other grids ctrls  */
    /**
     * @var o2_ctrl_navigator
     */
    public $internal_nav   = null;    /* Object o2_navigator internal the table         */
    public $views_sel      = array(); /* List of views selection status                 */
    public $views_trans    = array(); /* List of views transaction status               */
    public $hscroll        = 0;       /* Table body horizontal scrolling value          */
    public $vscroll        = 0;       /* Table body vertical scrolling value            */
    public $custom_colsord = false;   /* Custom columns order set                       */
    public $pinned_cols    = false;   /* Number of pinned columns                       */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $spazio_class   = "o2_tab_space";  /* Whole external assigned space <DIV>    */
    public $tab_class      = "o2_tab_tab";    /* Whole actually used space <TABLE>      */
    public $testa_class    = "o2_tab_testa";  /* Column title cell <TH>                 */
    public $corpo_class    = "o2_tab_corpo";  /* Body cell <TD>                         */
    public $riga_class     = "o2_tab_riga";   /* Standard body row <TR>                 */
    public $alt_class      = "o2_tab_alt";    /* Alternate body row <TR>                */
    public $footer_class   = "o2_tab_foot";   /* Column footer cell <TD>                */
    public $point_class    = "o2_tab_point";  /* On mouse over body row <TR>            */
    public $sele_class     = "o2_tab_sele";   /* Current record row <TR>                */
    public $sort_class     = "o2_tab_sort";   /* Sortable column button <SPAN>          */
    public $sorta_class    = "o2_tab_sorta";  /* Ascending sorted column button <SPAN>  */
    public $sortd_class    = "o2_tab_sortd";  /* Descending sorted column button <SPAN> */
    public $marker_class   = "o2_tab_marker"; /* Row marker object <BUTTON>             */
    public $fcol_class     = "o2_tab_fcol";   /* Row marker cell <TD>                   */


    /**
     * Costruttore
     *
     * @return o2_ctrl_tab
     */
    function __construct() {

        $this->tipo_ctrl    = "tab";
        $this->norec_msg    = $_SESSION['o2_app']->msg_norec;
        $this->row_marker   = true;
        $this->forza_submit = true;

        }


    function hide_indicator() {

        $this->row_marker = false;

        }


    function css_spazio($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->spazio_class = 'o2_tab_space'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->spazio_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->spazio_class = 'o2_tab_space';
            }
        return true;

        }


    function css_tab($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->tab_class = 'o2_tab_tab'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->tab_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->tab_class = 'o2_tab_tab';
            }
        return true;

        }


    function css_testa($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->testa_class = 'o2_tab_testa'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->testa_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->testa_class = 'o2_tab_testa';
            }
        return true;

        }


    function css_footer($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->footer_class = 'o2_tab_foot'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->footer_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->footer_class = 'o2_tab_foot';
            }
        return true;

        }


    function css_corpo($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->corpo_class = 'o2_tab_corpo'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->corpo_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->corpo_class = 'o2_tab_corpo';
            }
        return true;

        }


    function css_riga($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->riga_class = 'o2_tab_riga'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->riga_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->riga_class = 'o2_tab_riga';
            }
        return true;

        }


    function css_alt($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->alt_class = 'o2_tab_alt'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->alt_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->alt_class = 'o2_tab_alt';
            }
        return true;

        }


    function css_point($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->point_class = 'o2_tab_point'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->point_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->point_class = 'o2_tab_point';
            }
        return true;

        }


    function css_sele($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->sele_class = 'o2_tab_sele'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->sele_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->sele_class = 'o2_tab_sele';
            }
        return true;

        }


    function css_sort($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->sort_class = 'o2_tab_sort'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->sort_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->sort_class = 'o2_tab_sort';
            }
        return true;

        }


    function css_sorta($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->sorta_class = 'o2_tab_sorta'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->sorta_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->sorta_class = 'o2_tab_sorta';
            }
        return true;

        }


    function css_sortd($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->sortd_class = 'o2_tab_sortd'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->sortd_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->sortd_class = 'o2_tab_sortd';
            }
        return true;

        }


    function css_marker($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->marker_class = 'o2_tab_marker'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->marker_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->marker_class = 'o2_tab_marker';
            }
        return true;

        }


    function css_fcol($css = "") {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->fcol_class = 'o2_tab_fcol'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->fcol_class = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->fcol_class = 'o2_tab_fcol';
            }
        return true;

        }


    function pinned_cols($num = 0) {

        $this->pinned_cols = ($num ? $num : false);
        return true;

        }


    /**
     * Set grid-plus functions abilitation
     *
     * @param array $list
     */
    function grid_plus($list = array()) {

        if (is_array($list)) {
            $this->gp_vis_cols    = ($list[0] ? true : false);
            $this->gp_ord_cols    = ($list[1] ? true : false);
            $this->gp_size_cols   = ($list[2] ? true : false);
            $this->gp_filter_recs = ($list[3] ? true : false);
            $this->gp_sort_recs   = ($list[4] ? true : false);
            $this->gp_rows_num    = ($list[5] ? true : false);
            $this->gp_export      = ($list[6] ? true : false);
            $this->gp_delete      = ($list[7] ? true : false);
            }

        }


    /**
     * Imposta il messaggio da visualizzare quando nessun record è presente
     *
     * @param  string $message
     * @return boolean
     */
    function norec_msg($message = "") {

        $this->norec_msg = $message;
        return true;

        }


    /**
     * Display table control
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_tab($this);
            }
        else {
            o2html::ctrl_tab($this);
            }

        }


    /**
     * Stores context before table rendering and sets needed flags
     *
     */
    function status_open() {

        $this->views_sel   = array();
        $this->views_trans = array();
        foreach ($this->prg_obj->contesto as &$single_view) {
            // __________________________________________________ Exclude virtual view ___
            if ($single_view->dbdata) {
                $this->views_sel[$single_view->nome]   = $single_view->selezione;
                $this->views_trans[$single_view->nome] = $single_view->computing;
                if ($single_view->vista) {
                    $single_view->computing = true;
                    }
                }
            }

        }


    /**
     * Resets context after table rendering
     *
     */
    function status_close() {

        foreach ($this->prg_obj->contesto as &$single_view) {
            // __________________________________________________ Exclude virtual view ___
            if ($single_view->dbdata) {
                $single_view->selezione = $this->views_sel[$single_view->nome];
                $single_view->computing = $this->views_trans[$single_view->nome];
                }
            }
        $this->view_obj->corrente = &$this->view_obj->
                                      recordset[$this->view_obj->selezione];
        /* Eseguo il def del form per rimettere a posto i controlli fuori della tabella
           ancora da renderizzare                                                       */
        $this->form_obj->read_def();
        $this->modificabile = $this->save_modify;

        }


    /**
     * Set user personal settings for columns size
     *
     * @param string  $col    Column name (by control) to resize
     * @param integer $size   Size to set column to
     */
    function user_set_size($col, $size) {

        $app  = $_SESSION['o2_app'];
        $prg  = $app->istanze_prg[$this->id_esecuzione];
        $save = $app->option_get("tab_settings_save") === "1";
        // _________________________________________________________ Local persistance ___
        $prg->custom_grids[$this->nome]["S"][$col]+= $size;
        $size = $prg->custom_grids[$this->nome]["S"][$col];
        // ________________________________________________________ Global persistence ___
        if ($save) {
            $this->user_save();
            }

        }


    /**
     * Set user personal settings for columns visibility
     *
     * @param array $hide_list   Array of hidden columns
     */
    function user_set_vis($hide_list) {

        $app  = $_SESSION['o2_app'];
        $prg  = $app->istanze_prg[$this->id_esecuzione];
        $save = $app->option_get("tab_settings_save") === "1";
        // _________________________________________________________ Local persistance ___
        unset($prg->custom_grids[$this->nome]['V']);
        foreach ($hide_list as $c_vis) {
            if ($c_vis) {
                $prg->custom_grids[$this->nome]['V'][$c_vis] = true;
                }
            }
        // ________________________________________________________ Global persistence ___
        if ($save) {
            $this->user_save();
            }

        }


    /**
     * Set user personal settings for columns sorting (by context menu)
     *
     * @param array $hide_list   Array of order columns
     */
    function user_set_sort($sort_list) {

        $app  = $_SESSION['o2_app'];
        $prg  = $app->istanze_prg[$this->id_esecuzione];
        $form = $prg->form[$this->form];
        $view = $this->view_obj;
        eval("\$sort_list=".$sort_list.";");
        ksort($sort_list);
        $view_obj->sort = array();
        foreach ($sort_list as $sort_field) {
            $segment              = $form->controlli[$sort_field[1]]->campo_in_ctrl;
            $view->sort[$segment] = $sort_field[0];
            }
        $view->record_primo();

        }


    /**
     * Set user personal settings for rows number
     *
     * @param integer $rows   Number of rows to show in table (righe-vis)
     */
    function user_set_rows($rows) {

        $app  = $_SESSION['o2_app'];
        $prg  = $app->istanze_prg[$this->id_esecuzione];
        $save = $app->option_get("tab_settings_save") === "1";
        $view = $this->view_obj;
        // _________________________________________________________ Local persistance ___
        $view->user_rows = $rows;
        $view->struttura();
        $view->record_primo();
        $prg->custom_grids[$view->nome]["R"][""] = $rows;
        // ________________________________________________________ Global persistence ___
        if ($save) {
            $this->user_save();
            }

        }


    /**
     * Set user personal settings for columns order
     *
     * @param array $order_list   List of columns moved from original order
     */
    function user_set_order($order_list) {

        $app    = $_SESSION['o2_app'];
        $prg    = $app->istanze_prg[$app->progressivo_istanze];
        $form   = $prg->form[$this->form];
        $save   = $app->option_get("tab_settings_save") === "1";
        $rm_col = ($this->row_marker ? 0 : 1);
        eval("\$order_list=".$order_list.";");
        // _______________________________________ Local persistance and column moving ___
        foreach ($order_list as $ctrl => $pos) {
            // __________________________________ Get moved column actual column order ___
            $col_index = 0;
            foreach ($form->controlli as $ctrl_obj) {
                // __________ If control is inside of table, and it is not a navigator ___
                if ($ctrl_obj->padre == $this->nome &&
                    $ctrl_obj->tipo_ctrl != 'navigator') {
                    $col_index = $ctrl_obj->info_padre[2];
                    // ________________________________________ Existing control found ___
                    if ($ctrl_obj->nome == $ctrl) {
                        $col_index-= $rm_col;
                        break;
                        }
                    }
                }
            if (is_array($prg->custom_grids[$this->nome]['O'])) {
                foreach ($prg->custom_grids[$this->nome]['O'] as $ex_ctrl => $ex_pos) {
                    if ($ex_pos >= $pos && $ex_pos < $col_index) {
                        $prg->custom_grids[$this->nome]['O'][$ex_ctrl] = $ex_pos + 1;
                        }
                    elseif ($ex_pos <= $pos && $ex_pos > $col_index) {
                        $prg->custom_grids[$this->nome]['O'][$ex_ctrl] = $ex_pos - 1;
                        }
                    }
                }
            $prg->custom_grids[$this->nome]['O'][$ctrl] = $pos;
            $this->move_column($ctrl, $pos);
            }
        // ________________________________________________________ Global persistence ___
        if ($save) {
            $this->user_save();
            }

        }


    /**
     * Move a column from its original position to offset $position.
     * ATTENTION: $position index is relative to table children collection.
     *
     * @param string  $column     Column control name
     * @param integer $position   Final position to move column to (1 based)
     */
    function move_column($column, $position) {

        $app        = $_SESSION['o2_app'];
        $prg        = $app->istanze_prg[$app->progressivo_istanze];
        $form       = $prg->form[$this->form];
        // ______________________________________________________ Source column offset ___
        $from       = 0;
        // ____________________ Destination absolute offset (relative to all controls) ___
        $abs_pos    = 0;
        $tot_offset = 0;
        $tab_offset = 1;
        foreach ($form->controlli as $ctrl_idx => $ctrl_obj) {
            if ($ctrl_obj->padre == $this->nome && $ctrl_obj->tipo_ctrl != 'navigator') {
                if ($ctrl_idx == $column) {
                    $from = $tot_offset;
                    }
                // ______________________ Convert relative position to absolute offset ___
                if ($tab_offset == $position) {
                    $abs_pos = $tot_offset;
                    }
                $tab_offset++;
                }
           $tot_offset++;
           }
        unset($ctrl_idx);
        unset($ctrl_obj);
        // ______________________________________________________________ Move column ____
        $item            = array_splice($form->controlli, $from, 1);
        $form->controlli = array_slice($form->controlli, 0, $abs_pos, true) +
                           $item +
                           array_slice($form->controlli, $abs_pos, null, true);
        // _________ Manage parent-info: column index must match actual controls order ___
        $offset = 1;
        foreach ($form->controlli as $ctrl_idx => $ctrl_obj) {
            if ($ctrl_obj->padre == $this->nome && $ctrl_obj->tipo_ctrl != 'navigator') {
                $ctrl_obj->info_padre[0] = $ctrl_obj->info_padre[0];
                $ctrl_obj->info_padre[1] = $offset;
                $ctrl_obj->info_padre[2] = $offset;
                $offset++;
                }
            }
        // _______________ Set a flag on table to skip info-padre from form definition ___
        $this->custom_colsord = true;

        }


    /**
     * Save all user personal settings
     *
     */
    function user_save() {

        $app   = $_SESSION['o2_app'];
        $prg   = $app->istanze_prg[$this->id_esecuzione];
        $prg_n = $prg->nome;
        $view  = $this->view_obj;
        $user  = ($app->user_is_admin ? "" : $app->user);
        // ________________________________________________________ Global persistence ___
        $cs_tab    = $app->get_table("o2_custom_grids");
        $cs_db     = $cs_tab->db;
        $cs_server = $cs_db->server;
        $csf_user  = $cs_tab->campi['o2user']->nome_fisico;
        $csf_prg   = $cs_tab->campi['prg']->nome_fisico;
        $csf_obj   = $cs_tab->campi['obj']->nome_fisico;
        $csf_type  = $cs_tab->campi['p_type']->nome_fisico;
        $csf_col   = $cs_tab->campi['p_col']->nome_fisico;
        $csf_value = $cs_tab->campi['p_value']->nome_fisico;
        // ______________________________________________________ Delete view settings ___
        $where = $csf_user."='".$user."' AND ".
                 $csf_prg."='".$prg_n."' AND ".
                 $csf_obj."='".$view->nome."'";
        o2_gateway::deleterec($cs_server->type,
                              $cs_server->server,
                              $cs_server->user,
                              $cs_server->password,
                              $cs_db->nome,
                              $cs_db->proprietario,
                              $cs_tab->nome,
                              $cs_tab->nome,
                              $where);
        // ___________________________________________________ Save view rows settings ___
        if ($prg->custom_grids[$view->nome]["R"][""]) {
            $fields = array($csf_user,
                            $csf_prg,
                            $csf_obj,
                            $csf_type,
                            $csf_col,
                            $csf_value);
            $values = array("'".$user."'",
                            "'".$prg_n."'",
                            "'".$view->nome."'",
                            "'R'",
                            "''",
                            $view->user_rows);
            o2_gateway::insertrec($cs_server->type,
                                  $cs_server->server,
                                  $cs_server->user,
                                  $cs_server->password,
                                  $cs_db->nome,
                                  $cs_db->proprietario,
                                  $cs_tab->nome,
                                  $cs_tab->nome,
                                  $fields,
                                  $values);
            }
        // ________________________________ Delete previous size & visibility settings ___
        $where = $csf_user."='".$user."' AND ".
                 $csf_prg."='".$prg->nome."' AND ".
                 $csf_obj."='".$this->nome."'";
        o2_gateway::deleterec($cs_server->type,
                              $cs_server->server,
                              $cs_server->user,
                              $cs_server->password,
                              $cs_db->nome,
                              $cs_db->proprietario,
                              $cs_tab->nome,
                              $cs_tab->nome,
                              $where);
        // ____________________________ Set flag for size & visibility custom settings ___
        if ($user) {
            $fields = array($csf_user, $csf_prg, $csf_obj, $csf_type);
            $values = array("'".$user."'", "'".$prg_n."'", "'".$this->nome."'", "'X'");
            o2_gateway::insertrec($cs_server->type,
                                  $cs_server->server,
                                  $cs_server->user,
                                  $cs_server->password,
                                  $cs_db->nome,
                                  $cs_db->proprietario,
                                  $cs_tab->nome,
                                  $cs_tab->nome,
                                  $fields,
                                  $values);
            }
        // _______________________________________________ Columns visibility and size ___
        if (is_array($prg->custom_grids[$this->nome])) {
            foreach ($prg->custom_grids[$this->nome] as $s_v => $uset) {
                // ________________________________ Skip custom flag, already inserted ___
                if ($s_v == "X") {
                    unset($prg->custom_grids[$this->nome]["X"]);
                    continue;
                    }
                foreach ($uset as $column => $c_value) {
                    $fields = array($csf_user,
                                    $csf_prg,
                                    $csf_obj,
                                    $csf_type,
                                    $csf_col,
                                    $csf_value);
                    $values = array("'".$user."'",
                                    "'".$prg_n."'",
                                    "'".$this->nome."'",
                                    "'".$s_v."'",
                                    "'".$column."'",
                                    $c_value);
                    o2_gateway::insertrec($cs_server->type,
                                          $cs_server->server,
                                          $cs_server->user,
                                          $cs_server->password,
                                          $cs_db->nome,
                                          $cs_db->proprietario,
                                          $cs_tab->nome,
                                          $cs_tab->nome,
                                          $fields,
                                          $values);
                    }
                }
            }
        if ($app->option_get("tab_settings_save") !== "1") {
            o2act::alert("Table settings saved", 2);
            }

        }


    /**
     * Reset all user personal settings
     *
     */
    function user_reset() {

        $app  = $_SESSION['o2_app'];
        $prg  = $app->istanze_prg[$this->id_esecuzione];
        $form = $this->form_obj;
        $view = $this->view_obj;
        $user = ($app->user_is_admin ? "" : $app->user);
        // _________________________________________________________ Local persistance ___
        if (isset($prg->custom_grids[$view->nome]["R"])) {
            unset($prg->custom_grids[$view->nome]["R"]);
            }
        if (isset($prg->custom_grids[$this->nome])) {
            unset($prg->custom_grids[$this->nome]);
            }
        // ________________________________________________________ Global persistence ___
        $cs_tab    = $app->get_table("o2_custom_grids");
        $cs_db     = $cs_tab->db;
        $cs_server = $cs_db->server;
        $csf_user  = $cs_tab->campi['o2user']->nome_fisico;
        $csf_prg   = $cs_tab->campi['prg']->nome_fisico;
        $csf_obj   = $cs_tab->campi['obj']->nome_fisico;
        // _____________________________________________ Delete previous rows settings ___
        $where = $csf_user."='".$user."' AND ".
                 $csf_prg."='".$prg->nome."' AND ".
                 $csf_obj."='".$view->nome."'";
        o2_gateway::deleterec($cs_server->type,
                              $cs_server->server,
                              $cs_server->user,
                              $cs_server->password,
                              $cs_db->nome,
                              $cs_db->proprietario,
                              $cs_tab->nome,
                              $cs_tab->nome,
                              $where);
        // ________________________________ Delete previous size & visibility settings ___
        $where = $csf_user."='".$user."' AND ".
                 $csf_prg."='".$prg->nome."' AND ".
                 $csf_obj."='".$this->nome."'";
        o2_gateway::deleterec($cs_server->type,
                              $cs_server->server,
                              $cs_server->user,
                              $cs_server->password,
                              $cs_db->nome,
                              $cs_db->proprietario,
                              $cs_tab->nome,
                              $cs_tab->nome,
                              $where);
        // _______________________________________________________________ Redraw grid ___
        $refresh    = ($view->sort || $view->user_rows ? true : false);
        // ______________________________________________________ Reset custom sorting ___
        $view->sort = array();
        // ________________________________________________ Reset custom columns order ___
        if ($this->custom_colsord) {
            $this->custom_colsord = false;
            $form->defined        = false;
            $form->controlli      = array();
            $refresh              = true;
            }
        // ________________________________________________________________ Reset grid ___
        if ($refresh) {
            $view->righe_vis = $view->righe_vis_ori;
            $view->user_rows = 0;
            $view->struttura();
            $view->record_primo();
            // _____________________ Execute refresh action for controls custom sizing ___
            if ($form->refresh_action) {
                $prg->esegui_azione($form->refresh_action);
                }
            }

        }

    }


/**
 * Treeview object based on array
 *
 */
class o2_ctrl_tree extends o2_ctrl {

    public $list       = array();  /* Array of items                                    */
    public $open_nodes = array();  /* Array of open nodes                               */
    public $indent     = "";       /* Code to get one indentation level                 */
    public $sele_node  = false;    /* Last selected node ID                             */
    public $focus_node = false;    /* Focus node when moving by keyboard                */
    public $act_level  = 0;        /* Active nodes: 0=Nothing|1=All|2=Leaves|3=Folders  */
    public $sorted     = false;    /* If TRUE tree elements are sorted by label         */
    public $filter_txt = false;    /* Text to use for filtering elements labels by      */
    public $scripting  = false;    /* If control is defined in old scripting style      */
    public $vscroll    = 0;        /* Control vertical srolling value                   */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css        = "jxtree"; /* Whole control space <DIV>                   */


    /**
     * Return a treeview object
     *
     * @param  string $name  Unique object name (old scripting style definition only)
     * @param  array $list   Sub-items structure as array
     * @return o2_ctrl_tree
     */
    function __construct($name = false, $list = false) {

        $this->tipo_ctrl     = "tree";
        $this->indent        = "<span class='jx_tree_indent'></span>";
        // _______________________________________________________ Old scripting style ___
        if ($name) {
            $this->nome      = $name;
            $this->scripting = true;
            if ($list) {
                $this->list = $list;
                }
            $app                    = $_SESSION['o2_app'];
            $this->id_esecuzione    = $app->progressivo_istanze;
            $prg                    = $app->istanze_prg[$this->id_esecuzione];
            $prg->tree_ctrls[$name] = $this;
            }

        }


    /**
     * Define a new istanze of a treeview control in current program
     *
     * @param  string  $name         Control unique name
     * @param  array   $stru         Tree view structure
     * @param  integer $activation   Active nodes 0=None|1=All|2=Leavs|3=Folders
     * @param  string  $action       Action to be executed on nodes activation
     * @return o2_ctrl_tree
     */
    static function def($name, $stru = false, $activation = 0, $action = false) {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$app->progressivo_istanze];
        if (!isset($prg->tree_ctrls[$name])) {
            $prg->tree_ctrls[$name] = new o2_ctrl_tree($name);
            }
        if ($stru) {
            $prg->tree_ctrls[$name]->list = $stru;
            }
        if ($action) {
            $prg->tree_ctrls[$name]->azione_submit = $action;
            }
        $prg->tree_ctrls[$name]->act_level = $activation;
        return $prg->tree_ctrls[$name];

        }


    /**
     * Add a node to the treeview items list
     *
     * @param  string $node_id     Unique ID for the item
     * @param  string $parent_id   Parent node ID, if none node is top level
     * @param  string $node_code   HTML code to be displayed for item
     * @return string
     */
    function node_add($node_id, $parent_id, $node_code, $node_icon = "") {

        if (!$node_id) {
            $node_id = count($this->list) + 1;
            }
        $this->list[$node_id] = array($parent_id, $node_code, $node_icon);
        return $node_id;

        }


    /**
     * Remove a node and all its sub-nodes from the treeview items list and return all
     * removed items.
     *
     * @param  string $node_id     Unique ID for the item
     * @return array
     */
    function node_remove($node_id) {

        $nodes = array();
        if (isset($this->list[$node_id])) {
            $nodes[$node_id] = $this->list[$node_id];
            unset($this->list[$node_id]);
            // ______________________________________________________ Remove sub-nodes ___
            foreach ($this->list as $index => $subnode) {
                if ($subnode[0] && ($subnode[0] == $node_id)) {
                    $nodes+= $this->node_remove($index);
                    }
                }
            }
        return $nodes;

        }


    /**
     * Move a node from the old parent to a new parent
     *
     * @param  string $node_id      Unique ID for the item
     * @param  string $new_parent   New parent ID
     * @return boolean
     */
    function node_reparent($node_id, $new_parent) {

        if (isset($this->list[$node_id]) && isset($this->list[$new_parent])) {
            $this->list[$node_id][0] = $new_parent;
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Make a node visibile showing all its ancestors
     *
     * @param  string $node_id      Unique ID for the item
     * @return boolean
     */
    function node_show($node_id) {

        if (isset($this->list[$node_id])) {
            do {
                $parent_id = $this->list[$node_id][0];
                if ($parent_id && isset($this->list[$parent_id])) {
                    $this->open_nodes[$parent_id] = true;
                    }
                else {
                    break;
                    }
                $node_id = $parent_id;
                } while (true);
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Select a node and make it visible
     *
     * @param  string  $node_id     Node to be selected
     * @param  boolean $no_action   Hinibits action execution
     * @return boolean
     */
    function node_select($node_id, $no_action = false) {

        if (!$node_id) {
            $this->sele_node  = false;
            $this->focus_node = false;
            }
        elseif (isset($this->list[$node_id])) {
            $this->sele_node  = $node_id;
            $this->focus_node = $node_id;
            $this->node_show($node_id);
            if (!$no_action && $this->azione_submit) {
                $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
                $prg->esegui_azione($this->azione_submit, true);
                }
            }
        else {
            return false;
            }

        }


    /**
     * Expand all nodes with items down to $level.
     * If $level is 0 all folders are expanded.
     *
     * @param integer $level   Level to fold down to
     */
    function fold_all($level = 0) {

        $this->open_nodes = array();
        foreach ($this->list as $node_id => $node) {
            if ($level) {
                $child = $node;
                $nest  = 1;
                while (true) {
                    if ($child[0]) {
                        $nest++;
                        if ($nest > $level) {
                            break;
                            }
                        $child = $this->list[$child[0]];
                        }
                    else {
                        break;
                        }
                    }
                if ($nest <= $level) {
                    $this->open_nodes[$node_id] = true;
                    }
                }
            else {
                $this->open_nodes[$node_id] = true;
                }
            }

        }


    /**
     * Collapse all nodes with items
     *
     */
    function unfold_all() {

        $this->open_nodes = array();

        }


    /**
     * Expand a node.
     *
     * @param mix $node_id   Folding node ID
     */
    function node_fold($node_id) {

        $this->open_nodes[$node_id] = true;

        }


    /**
     * Collapse a node items
     *
     * @param mix $node_id   Unfolding node ID
     */
    function node_unfold($node_id) {

        if (isset($this->open_nodes[$node_id])) {
            unset($this->open_nodes[$node_id]);
            }

        }


    /**
     * Switch sort/unsort treeview elements
     *
     */
    function switch_sort() {

        $this->sorted = !$this->sorted;

        }


    /**
     * Set text to filter by
     *
     * @param string $txt
     */
    function set_filter($txt = false) {

        if ($txt) {
            $this->filter_txt = $txt;
            }
        else {
            $this->filter_txt = false;
            }

        }


    /**
     * Return HTML code for a node and its children
     *
     * @return string
     */
    function get_node_code($node_id = 0, $level = 0) {

        $this_node     = $this->list[$node_id];
        // _____________________________________________________ Node 0 is always open ___
        $open          = !$node_id || $this->open_nodes[$node_id];
        $code          = "";
        $children_code = "";
        $has_child     = false;
        // ____________________________________________________________________ Filter ___
        if ($this->filter_txt) {
            $this->filter($this->filter_txt, true);
            }
        // ______________________________________________________________________ Sort ___
        if ($this->sorted) {
            uksort($this->list, 'strnatcasecmp');
            }
        // __________________________________________________________ Loop on children ___
        foreach ($this->list as $child_id => $child) {
            if ("".$node_id == "".$child[0]) {
                // _____________________________ If node is open include children code ___
                if ($open) {
                    $children_code.= $this->get_node_code($child_id, ($level + 1));
                    $has_child     = true;
                    }
                // ___________________ If node is close record it has a child and exit ___
                else {
                    $has_child = true;
                    break;
                    }
                }
            }
        // _______________________________________________________ Icon to fold/unfold ___
        $fold = "<div class='jx_treenode_".
                ($has_child ? ($open ? "open'" : "close'") : "leaf'").
                ($has_child ?
                 " onclick='o2jse.tv.c(this,\"".$this->nome.'","'.$node_id."\");'"
                 : "")."></div>";
        // ______________________________________________________________________ Icon ___
        if ($this_node[2]) {
            list($special_folder, $img_name) = explode("/", $this_node[2], 2);
            switch (strtolower($special_folder)) {
                case "<o2>":
                case "<jx>":
                    $icon = "<img class='jx_treenode_icon' src='".
                            $GLOBALS['o2_runtime']->alias.$img_name."'>";
                    break;
                default:
                    $icon = "<img class='jx_treenode_icon' src='img/".$this_node[2]."'>";
                    break;
                }
            }
        else {
            $icon = "<span></span>";
            }
        // _________________________________________ Nodes JS activation and selection ___
        $css_txt = "";
        $act     = "";
        if ($this->act_level == 1 ||
            ($this->act_level == 2 && !$has_child) ||
            ($this->act_level == 3 && $has_child)) {
            $sele_node  = $node_id && ($this->sele_node == $node_id);
            $focus_node = $node_id && ($this->focus_node == $node_id);
            // _______________________________________________ Selection or focus node ___
            if ($sele_node || $focus_node) {
                // ____________________________________________________ Selection node ___
                if ($sele_node) {
                    $css_txt = "class='jx_treenode_sele' ";
                    // ______________________________________ Selection and focus node ___
                    if ($focus_node) {
                        $tab_idx = 'id="'.$this->nome.'_jxFocusNode" tabindex="0" ';
                        }
                    // ________________________________________________ Selection node ___
                    else {
                        $tab_idx = 'tabindex="0" ';
                        }
                    }
                // ________________________________________________________ Focus node ___
                else {
                    $css_txt = "class='jx_treenode_focus' ";
                    $tab_idx = 'id="'.$this->nome.'_jxFocusNode" tabindex="0" ';
                    $act     = " onclick='o2jse.tv.a(this,\"".$node_id."\");'";
                    }
                $act    .= " onKeyDown='o2jse.tv.k(event);'";
                }
            // _____________________________ Standard node (no selection and no focus) ___
            else {
                $tab_idx = "";
                $act     = " onclick='o2jse.tv.a(this,\"".$node_id."\");'";
                }
            }
        $code.= ($node_id ?
                 "<div ".$css_txt."style='display:block;white-space:nowrap;'>".
                 str_repeat($this->indent, $level).$fold.$icon.
                 "<div class='jx_treenode_text'".$tab_idx.$act.">".$this_node[1].
                 "</div></div>" :
                 "").$children_code;
        return $code;

        }


    /**
     * Filter Treeview control elements using $text.
     * Only elements containing $text in their description, and their ancestors, are left
     * in.
     * If param $hide_down is passed as FALSE subnodes are added to filtered ones, else
     * they are excluded.
     *
     * @param  string  $needle      Text to filter for
     * @param  boolean $hide_down   If FALSE add subnodes, else hide them
     * @return boolean
     */
    function filter($needle, $hide_down) {

        if ($needle) {
            $this->open_nodes = array();
            $new_list         = array();
            $show_list        = array();
            foreach ($this->list as $node_id => $node) {
                $in_ancestor = false;
                if (!$hide_down) {
                    $child = $node;
                    while (true) {
                        if ($child) {
                            if (stripos($child[1], $needle) !== false) {
                                $in_ancestor = true;
                                break;
                                }
                            $child = $this->list[$child[0]];
                            }
                        else {
                            break;
                            }
                        }
                    }
                $in_text = stripos($node[1], $needle) !== false;
                if ($in_text || $in_ancestor) {
                    $new_list[$node_id] = $node;
                    $child = $node;
                    while (true) {
                        if ($child[0]) {
                            $new_list[$child[0]] = $this->list[$child[0]];
                            $child = $this->list[$child[0]];
                            }
                        else {
                            break;
                            }
                        }
                    if ($in_text) {
                        $show_list[$node_id] = $node_id;
                        }
                    }
                }
            $this->list = $new_list;
            foreach ($show_list as $node_id) {
                $this->node_show($node_id);
                }
            }

        }


    /**
     * Export TreeView content in CSV format.
     *
     * @param  string  $file_name           Target file name
     * @param  string  $fields_separator    Character use to separate fields
     * @param  string  $text_delimiter      Character used to delimiter text values
     * @return string
     */
    function export($file_name = "", $fields_separator = ",", $text_delimiter = '"') {

        $app  = $_SESSION['o2_app'];
        $code = "";
        // _____________________________________________________________ Set file name ___
        if (!$file_name) {
            $file_name = $this->nome."_".date("Ymd_His");
            }
        else {
            $file_name = pathinfo($file_name, PATHINFO_BASENAME);
            if (strtolower(pathinfo($file_name, PATHINFO_EXTENSION)) == "csv") {
                $file_name = pathinfo($file_name, PATHINFO_FILENAME);
                }
            }
        $csv_file = $app->dir_tmp.$file_name.".csv";
        $zip_file = $app->dir_tmp.$file_name.".zip";
        // ________________________________________________ Loop on 1st level elements ___
        foreach ($this->list as $child_id => $child) {
            if (!$child[0]) {
                // _____________________________________________ Include elements code ___
                $code.= $this->export_nest($child_id, $fields_separator, $text_delimiter);
                }
            }
        // _____________________________________________________________ Write to file ___
        file_put_contents($csv_file, $code);
        // ________________________________________________________ Create ZIP archive ___
        $zip = new ZipArchive();
        if ($zip->open($zip_file, ZIPARCHIVE::CREATE || ZIPARCHIVE::OVERWRITE) !== true) {
            throw new o2_exception("Error creating zip file <i>".$zip_file."</i>",
                                   o2error_IO);
            unset($zip);
            return false;
            }
        $zip->addFile($csv_file, $file_name.".csv");
        $zip->close();
        return $zip_file;

        }


    /**
     * Export TreeView content in CSV format.
     *
     * @param  mix $node_id
     * @return string
     */
    private function export_nest($node_id,
                                 $fields_separator = ",",
                                 $text_delimiter   = '"',
                                 $prefix           = "",
                                 $level            = 0) {

        $this_node = $this->list[$node_id];
        $this_code = $prefix.$text_delimiter.
                     html_entity_decode(strip_tags($this_node[1]),
                                        ENT_QUOTES,
                                        $_SESSION['o2_app']->chr_encoding).
                     $text_delimiter;
        $children  = false;
        $code      = "";
        // __________________________________________________________ Loop on children ___
        foreach ($this->list as $child_id => $child) {
            if ("".$node_id == "".$child[0]) {
                $children = true;
                // _____________________________________________ Include children code ___
                $code.= $this->export_nest($child_id,
                                           $fields_separator,
                                           $text_delimiter,
                                           $this_code.$fields_separator,
                                           $level++);
                }
            }
        if (!$children) {
            $code.= $this_code."\n";
            }
        return $code;

        }


    /**
     * Sets nodes list for treeview control
     *
     * @param array $nodes_list
     */
    function nodes($nodes_list) {

        $this->list = $nodes_list;

        }


    /**
     * Sets activation level for treeview control
     *
     * 0 = None
     * 1 = All
     * 2 = Leaves
     * 3 = Folders
     *
     * @param integer $nodes_list
     */
    function activation($act_level) {

        $this->act_level = $act_level;

        }


    function css($css = '') {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'jxtree'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'jxtree';
            }
        return true;

        }


    /**
     * Visualizza il controllo
     *
     * @return boolean
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_tree($this);
            }
        else {
            o2html::ctrl_tree($this);
            }

        }

    }


/**
 * Images lister object based on array
 *
 *
 */
class o2_ctrl_imglist extends o2_ctrl {

    public $list       = array(); /* Array of items                                     */
    public $last_item  = false;   /* Last selected item ID                              */
    public $focus_item = false;   /* Focus item when moving by keyboard                 */
    public $action_del = false;   /* Action to be executed on click on delete image btn */
    public $items_w    = false;   /* Items width (if FALSE width wides as needed)       */
    public $items_h    = false;   /* Items height (if FALSE height wides as needed)     */
    public $scripting  = false;   /* If control is defined in old scripting style       */
    public $vscroll    = 0;       /* Control vertical srolling value                    */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css        = "jximglist"; /* Whole control space <DIV>                      */


    /**
     * Return a imglist object
     *
     * @param  string  $name     Unique object name (old scripting style definition only)
     * @param  array   $list     Items as array($img_file, $text)
     * @param  integer $width    Items width
     * @param  integer $height   Items height
     * @param  string  $click    Action to be executed on click on image
     * @param  string  $delete   Action to be executed on click on image delete button
     * @return o2_ctrl_imglist
     */
    function __construct($name   = false,
                         $list   = false,
                         $width  = 0,
                         $height = 0,
                         $click  = false,
                         $delete = false) {

        $this->tipo_ctrl = "imglist";
        if ($name) {
            $this->nome      = $name;
            $this->scripting = true;
            if ($list) {
                $items = array();
                foreach ($list as $key => $value) {
                    // ___________________________ Add pseudo-parameter for refreshing ___
                    $img         = rtrim($value[0]);
                    $img        .= (strpos($img, "?") ? "&" :"?")."jxrs=".microtime(true);
                    $items[$key] = array($img, $value[1]);
                    }
                $this->list = $items;
                }
            if ($width) {
                $this->items_w = intval($width);
                }
            if ($height) {
                $this->items_h = intval($height);
                }
            if ($click) {
                $this->azione_submit = $click;
                }
            if ($delete) {
                $this->action_del = $delete;
                }
            $app                       = $_SESSION['o2_app'];
            $this->id_esecuzione       = $app->progressivo_istanze;
            $prg                       = $app->istanze_prg[$this->id_esecuzione];
            $prg->imglist_ctrls[$name] = $this;
            }

        }


    /**
     * Define a new istanze of an images-lister control in current program
     *
     * @param  string  $name     Control unique name
     * @param  array   $list     Items list as array
     * @param  integer $width    Items width
     * @param  integer $height   Items height
     * @param  string  $click    Action to be executed on click on image
     * @param  string  $delete   Action to be executed on click on image delete button
     * @return o2_ctrl_tree
     */
    static function def($name,
                        $list   = false,
                        $width  = false,
                        $height = false,
                        $click  = false,
                        $delete = false) {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$app->progressivo_istanze];
        if (!isset($prg->imglist_ctrls[$name])) {
            $prg->imglist_ctrls[$name] = new o2_ctrl_imglist($name);
            }
        if (is_array($list)) {
            $items = array();
            foreach ($list as $key => $value) {
                // _______________________________ Add pseudo-parameter for refreshing ___
                $img         = rtrim($value[0]);
                $img        .= (strpos($img, "?") ? "&" : "?")."jxrs=".microtime(true);
                $items[$key] = array($img, $value[1]);
                }
            $prg->imglist_ctrls[$name]->list = $items;
            }
        if ($width !== false) {
            $prg->imglist_ctrls[$name]->items_w = intval($width);
            }
        if ($height !== false) {
            $prg->imglist_ctrls[$name]->items_h = intval($height);
            }
        if ($click) {
            $prg->imglist_ctrls[$name]->azione_submit = $click;
            }
        if ($delete) {
            $prg->imglist_ctrls[$name]->action_del = $delete;
            }
        return $prg->imglist_ctrls[$name];

        }


    /**
     * Add an image with text to the images lister items list
     *
     * @param  string $img_id      Unique ID for the image
     * @param  string $img_file    URL to image file
     * @param  string $item_code   HTML code to be displayed for item
     * @return string
     */
    function img_add($img_id, $img_file, $item_code) {

        if (!$img_id) {
            $img_id = count($this->list) + 1;
            }
        // _______________________________________ Add pseudo-parameter for refreshing ___
        $img_file            = rtrim($img_file).(strpos($img_file, "?") ? "&" : "?").
                               "jxrs=".microtime(true);
        $this->list[$img_id] = array($img_file, $item_code);
        return $img_id;

        }


    /**
     * Remove an image from the images lister images list and return removed item.
     *
     * @param  string $image_id     Unique ID for the image
     * @return array
     */
    function img_remove($image_id) {

        $remimg = false;
        if (isset($this->list[$image_id])) {
            $remimg = $this->list[$image_id];
            unset($this->list[$image_id]);
            }
        return $remimg;

        }


    /**
     * Set width and height for the image lister items
     *
     * @param  integer $width    Items width
     * @param  integer $height   Items height
     * @return boolean
     */
    function set_items_size($width = false, $height = false) {

        if ($width !== false) {
            $this->items_w = intval($width);
            }
        if ($height !== false) {
            $this->items_h = intval($height);
            }

        }


    /**
     * Activate an item
     *
     * @param  string $item_id   Item to be activated
     * @return boolean
     */
    function item_activate($item_id) {

        if (isset($this->list[$item_id])) {
            $this->last_item = $item_id;
            if ($this->azione_submit) {
                $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
                $prg->esegui_azione($this->azione_submit, true);
                }
            }
        else {
            return false;
            }

        }


    /**
     * Execute delete action for an item
     *
     * @param  string $item_id   Item to be deleted
     * @return boolean
     */
    function item_delete($item_id) {

        if (isset($this->list[$item_id])) {
            $this->last_item = $item_id;
            if ($this->action_del) {
                $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
                $prg->esegui_azione($this->action_del, true);
                }
            }
        else {
            return false;
            }

        }


    /**
     * Make the image of the selected item to refresh
     *
     * @param  string $img_id      Unique ID for the image
     * @return boolean
     */
    function img_refresh($img_id) {

        if (isset($this->list[$img_id])) {
            $old = $this->list[$img_id][0];
            $old = substr($old, 0, strpos($old, "jxrs="));
            $this->list[$img_id][0] = $old."jxrs=".microtime(true);
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Return HTML code for the images lister
     *
     * @return string
     */
    function get_code() {

        $del_img = $_SESSION['o2_app']->runtime->alias.'img/ctrl_delete.png';
        $code    = '';
        $style   = '';
        $cc      = '';
        $cd      = '';
        if (!$this->scripting) {
            $val = $this->ctrl2sql();
            }
        if ($this->items_w) {
            $style.= 'width:'.$this->items_w.'px;';
            }
        if ($this->items_h) {
            $style.= 'height:'.$this->items_h.'px;';
            }
        if ($this->azione_submit) {
            $cc = ' onclick="o2jse.il.c(this,\'';
            }
        if ($this->action_del) {
            $cd = ' onclick="o2jse.il.d(this,\'';
            $ce = '\');e=o2jse.event.std(event);e.stop()"';
            }
        foreach ($this->list as $key => $item) {
            $si = '';
            if ($this->scripting) {
                if ($key == $this->last_item) {
                    $si = ' jximglist_select';
                    }
                }
            elseif ($key == $val) {
                $si = ' jximglist_select';
                }
            $code.= '<div class="jximglist_frame'.$si.'" style="'.$style.'"'.
                    ($cc ? $cc.$key.'\');"' : "").
                    ($cd ?
                     ' onmouseover="document.getElementById(\''.
                       $this->nome.'_del_btn_'.$key.'\').style.display=\'block\'"'.
                     ' onmouseout="document.getElementById(\''.
                       $this->nome.'_del_btn_'.$key.'\').style.display=\'none\'"' : "").
                    '><img src="'.$item[0].
                    '"><div class="jximglist_text">'.$item[1].'</div>'.
                    ($cd ?
                     '<div id="'.$this->nome.'_del_btn_'.$key.
                     '" class="jximglist_del" style="display:none"'.$cd.$key.$ce.
                     '><img src="'.$del_img.'"></div>' : "").
                    '</div>';
            }
        return $code;

        }


    /**
     * Sets items list for images-lister control
     *
     * @param array $items_list
     */
    function items($items_list) {

        $this->list = $items_list;

        }


    /**
     * Sets delete action for images-lister control
     *
     * @param string $action
     */
    function delete($action) {

        $this->action_del = $action;

        }


    /**
     * Sets items width for images-lister control (zero for dynamic width)
     *
     * @param integer $width
     */
    function items_w($width = 0) {

        $this->items_w = $width;

        }


    /**
     * Sets items height for images-lister control (zero for dynamic height)
     *
     * @param integer $height
     */
    function items_h($height = 0) {

        $this->items_h = $height;

        }


    function css($css = '') {

        if ($css) {
            // _______________________ If starts with space then add to standard class ___
            if ($css[0] === ' ') {
                $this->css = 'jximglist'.$css;
                }
            // ________________________________________________ Replace standard class ___
            else {
                $this->css = $css;
                }
            }
        // ____________________________________________________________ Standard class ___
        else {
            $this->css = 'jximglist';
            }
        return true;

        }


    /**
     * Visualizza il controllo
     *
     * @return boolean
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_imglist($this);
            }
        else {
            o2html::ctrl_imglist($this);
            }

        }

    }


/**
* Progress bar object with AJAX update logics
*
*/
class o2_ctrl_progress extends o2_ctrl {

    public $id_field        = "JXPBI"; /* Field containing bar-ctrl ID when view-based  */
    public $progress_field  = "JXPBV"; /* Field containing bar-ctrl val when view-based */
    public $caller          = "";      /* Caller-function for refreshing by expression  */
    public $completed       = array(); /* Completion flag                               */
    public $started         = array(); /* Start flag                                    */
    public $complete_action = "";      /* Action to be executed on complete             */
    public $start_action    = "";      /* Action to be executed on start (from 0 to >0) */
    public $scripting       = false;   /* If control is defined in old scripting style  */
    /*     _____ CSS CLASSES __________________________________________________________ */
    public $css_out         = "o2_ctrl_progress";     /* Whole control <DIV>            */
    public $css_bar         = "o2_ctrl_progress_bar"; /* Progress indicator bar <DIV>   */
    public $css_txt         = "o2_ctrl_progress_txt"; /* Percent value text <DIV>       */


    /**
     * Returns a bar-ctrl object and register control in program
     *
     * @param string $ctrl_name   Control unique name
     */
    function __construct($ctrl_name = false) {

        $this->tipo_ctrl = "progress";
        // ____________________________________________ Scripting definition (old way) ___
        if ($ctrl_name) {
            $app                  = $_SESSION['o2_app'];
            $this->id_esecuzione  = $app->progressivo_istanze;
            $this->nome           = "PB".$ctrl_name;
            $this->scripting      = true;
            $app->istanze_prg[$this->id_esecuzione]->progress_ctrls[$this->nome] = $this;
            }

        }


    /**
     * Sets value normalized to 0-100
     *
     * @param integer $value
     * @param string  $ctrl_id
     */
    function set_value($value, $ctrl_id) {

        $value        = min(100, max(0, intval($value)));
        $this->valore = $value;
        if ($value == 0) {
            $this->started[$ctrl_id] = false;
            }
        if ($value < 100) {
            $this->completed[$ctrl_id] = false;
            }
        return $value;

        }


    /**
     * DEPRECATED: Scripting defining control - Old way
     *
     * Sets control value from db, when progress bar is linked to a view.
     * Method expects to find its name in the "id-field" column and its value in the
     * "progress-field" column.
     *
     */
    function set_value_by_view() {

        $app  = $_SESSION['o2_app'];
        $prg  = $app->istanze_prg[$app->progressivo_istanze];
        $view = $prg->contesto[$this->task];
        foreach ($view->recordset as $rec) {
            if ($this->nome == "PB".$rec[$this->id_field]) {
                $this->set_value($rec[$this->progress_field], '');
                break;
                }
            }
        if ($this->valore > 0) {
            $this->started[''] = true;
            }
        if ($this->valore == 100) {
            $this->completed[''] = true;
            }

        }


    /**
     * DEPRECATED: Scripting defining control - Old way
     *
     * Returns HTML code for bar and activate JavaScript to refresh bar progress.
     * If more than a progress bar are defined in program then all them are refreshed by
     * the same AJAX request.
     *
     * @return string
     */
    function get_code() {

        $id  = $this->nome.$this->id_esecuzione;
        $val = intval($this->valore);
        $act = ($this->modificabile && $val < 100 ?
                "<script> o2jse.progress.start(); </script>\n" : "");
        return '<div id="'.$id.'" class="'.
               $this->css_out.($this->modificabile ? "" : "_dis").'"><div class="'.
               $this->css_bar.'" style="width:'.$val.'%">&nbsp;</div><div class="'.
               $this->css_txt.'">'.$val."%</div></div>".$act;

        }


    /**
     * Prepares view progress-bar control is linked to.
     * 2 formulas are added to view:
     *  - JXPBI: use current record PK to compose a record unique ID
     *  - JXPBV: use expression defined on progress control to get percent value
     */
    function prepare_view($view) {

        // ______________________________ Add formulas to view for ID and VALUE fields ___
        if (!isset($view->formule['JXPBV'])) {
            // _______________________________________ Formula to get record unique ID ___
            $view->calcola('JXPBI',
                           'o2sys_long_str',
                       '$thisview->chiave_corrente($thisview->file,!1,!1,!1,!1,!0,!0,!0)',
                           null, null, null, null);
            // _________________________________________ Formula to get progress value ___
            $view->calcola('JXPBV',
                           'o2sys_long_str',
                           $this->caller.'()',
                           null,
                           null,
                           null,
                           null);
            $view->record_primo();
            }

        }


    /**
     * Returns JavaScript code to set controls values.
     * Method is intended to be called by client update-refresh logics.
     * When all progress-bars are completed client refresh is stopped by response.
     * All bars defined in program are updated by the same response.
     * When a progress-bar goes to completion on-complete-action is executed.
     *
     * @return string   JavaSript code to update bars
     */
    static function get_values() {

        $app     = $_SESSION['o2_app'];
        $prg     = $app->istanze_prg[$app->progressivo_istanze];
        $code    = "";
        $ctrls   = array();
        $list    = array();
        $stop    = true;
        $act     = false;
        $act_id  = "";
        $callers = array();
        // ______________________________________ Scripting defined controls (old way) ___
        if (is_array($prg->progress_ctrls) && count($prg->progress_ctrls)) {
            $ctrls = $prg->progress_ctrls;
            }
        // _________________________________________________________ Standard controls ___
        foreach ($prg->form as $form) {
            foreach ($form->controlli as $ctrl_name => $ctrl_obj) {
                if ($ctrl_obj->tipo_ctrl == 'progress') {
                    $ctrls[$ctrl_name] = $ctrl_obj;
                    }
                }
            }
        // ____________________________________________________ Group controls by view ___
        foreach ($ctrls as $ctrl_name => $ctrl_obj) {
            if ($ctrl_obj->modificabile) {
                // ___________________________________________ Bar from view (old way) ___
                if ($ctrl_obj->task) {
                    if (!isset($list[$ctrl_obj->task])) {
                        $list[$ctrl_obj->task] = array();
                        }
                    $list[$ctrl_obj->task]['JXPBC'][] = $ctrl_obj;
                    $list[$ctrl_obj->task]['JXPBI']   = $ctrl_obj->id_field;
                    $list[$ctrl_obj->task]['JXPBV']   = $ctrl_obj->progress_field;
                    }
                // ____________________________ Bar from view (standard Janox control) ___
                elseif ($form->controlli[$ctrl_obj->padre]->tipo_ctrl == 'tab') {
                    $view_name = $form->controlli[$ctrl_obj->padre]->task;
                    $view_obj  = $prg->contesto[$view_name];
                    if (!isset($list[$view_name])) {
                        $list[$view_name] = array();
                        }
                    // __________________ Add formulas to view for ID and VALUE fields ___
                    $ctrl_obj->prepare_view($view_obj);
                    $list[$view_name]['JXPBC'] = $ctrl_obj;
                    $list[$view_name]['JXPBI'] = 'JXPBI';
                    $list[$view_name]['JXPBV'] = 'JXPBV';
                    }
                // ____________________________________________________ Bar from value ___
                elseif ($ctrl_obj->caller) {
                    // _______________________________ Scripting defined bar (Old way) ___
                    if ($ctrl_obj->scripting) {
                        // ____________ Recall function using get-bar, to update value ___
                        if (!in_array($ctrl_obj->caller, $callers)) {
                            $callers[] = $ctrl_obj->caller;
                            call_user_func($ctrl_obj->caller);
                            }
                        }
                    // ________________________________________ Standard Janox control ___
                    else {
                        // _________ ->caller contains expression name to be evaluated ___
                        $local_caller = $ctrl_obj->caller;
                        $ctrl_obj->set_value($local_caller(), '');
                        }
                    $code.= "o2jse.progress.set('".
                            $ctrl_name.$app->progressivo_istanze."',".
                            $ctrl_obj->valore.");\n";
                    // ______________________________________________ Run start action ___
                    if ($ctrl_obj->valore > 0 && !$ctrl_obj->started['']) {
                        $ctrl_obj->started[''] = true;
                        if ($ctrl_obj->start_action) {
                            $act    = $ctrl_obj->start_action;
                            $act_id = $ctrl_name;
                            }
                        }
                    // _______________________ Stop refreshing when all bars completed ___
                    if ($ctrl_obj->valore < 100) {
                        $stop = false;
                        }
                    // ___________________________________________ Run complete action ___
                    elseif (!$ctrl_obj->completed['']) {
                        $ctrl_obj->completed[''] = true;
                        if ($ctrl_obj->complete_action) {
                            $act    = $ctrl_obj->complete_action;
                            $act_id = substr($ctrl_obj->nome, 2);
                            }
                        }
                    }
                }
            }
        $callers = array();
        // _____________________________________________________________ Loop on views ___
        foreach ($list as $view_name => $view_ctrls) {
            if (is_a($prg->contesto[$view_name], "o2_dbview")) {
                $view  = clone $prg->contesto[$view_name];
                $f_id  = $view_ctrls['JXPBI'];
                $f_val = $view_ctrls['JXPBV'];
                $view->record_primo();
                // _____________________________________ Loop on view related controls ___
                foreach ($view->recordset as $rec) {
                    // __________________________ Scripting defined controls (old way) ___
                    if (is_array($view_ctrls['JXPBC'])) {
                        foreach ($view_ctrls['JXPBC'] as $old_ctrl) {
                            $ctrl_name = "PB".$rec[$f_id];
                            if ($old_ctrl->nome == $ctrl_name) {
                                $ctrl   = $old_ctrl;
                                $rec_id = '';
                                break;
                                }
                            }
                        }
                    else {
                        $ctrl      = $view_ctrls['JXPBC'];
                        $ctrl_name = $ctrl->nome.$rec[$f_id];
                        $rec_id    = $rec[$f_id];
                        }
                    $ctrl->set_value($rec[$f_val], $rec_id);
                    $code.= "o2jse.progress.set('".$ctrl_name.$ctrl->id_esecuzione.
                                              "',".$ctrl->valore.");\n";
                    // ______________________________________________ Run start action ___
                    if ($ctrl->valore > 0 && !$ctrl->started[$rec_id]) {
                        $ctrl->started[$rec_id] = true;
                        if ($ctrl->start_action) {
                            $act    = $ctrl->start_action;
                            $act_id = $rec[$f_id];
                            }
                        }
                    // _______________________ Stop refreshing when all bars completed ___
                    if ($ctrl->valore < 100) {
                        $stop = false;
                        }
                    // ___________________________________________ Run complete action ___
                    elseif (!$ctrl->completed[$rec_id]) {
                        $ctrl->completed[$rec_id] = true;
                        if ($ctrl->complete_action) {
                            $act    = $ctrl->complete_action;
                            $act_id = $rec[$f_id];
                            }
                        }

                    }
                unset($view);
                }
            }
        if ($stop) {
            $code.= "o2jse.progress.stop();\n";
            }
        // _________________________________ If $act is sent posticipate progress-stop ___
        if ($act) {
            $code = "o2jse.cmd.exe(null,".
                                   $app->progressivo_istanze.",'".
                                   $act."','".
                                   $act_id."')\n";
            }
        print $code;
        die();

        }


    /**
     * DEPRECATED: Scripting defining control - Old way
     *
     * Define a bar-control and returns its HTML code, to be displayed in a HTML-area
     * control.
     * Function calling this method (usually a Janox expression) is used each time to
     * update value.
     *
     * @param  string  $bar_name          Control unique name
     * @param  integer $value             Constrol value
     * @param  string  $complete_action   Action to be executed on completion
     * @param  string  $start_action      Action to be executed on start (from 0 to >0)
     * @param  boolean $active            If bar is active and it makes update requests
     * @return string
     */
    static function get_bar($bar_name,
                            $value,
                            $complete_action = "",
                            $start_action    = "",
                            $active          = true) {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$app->progressivo_istanze];
        if (isset($prg->progress_ctrls["PB".$bar_name])) {
            $bar = $prg->progress_ctrls["PB".$bar_name];
            }
        else {
            $bar   = new self($bar_name);
            // ______ Function calling this method is saved to be used on value update ___
            $trace = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 5);
            foreach ($trace as $idx => $stack) {
                $func = $stack["function"];
                if ($func != __function__ && $func != "o2progress_bar") {
                    $bar->caller = $func;
                    break;
                    }
                }
            $bar->complete_action = $complete_action;
            $bar->start_action    = $start_action;
            }
        $bar->modificabile($active);
        $bar->set_value($value, '');
        return $bar->get_code();

        }


    /**
     * DEPRECATED: Scripting defining control - Old way
     *
     * Defines a bar-ctrl and returns its HTML code, to be displayed in a HTML-area
     * control.
     * This method is primary intended to used in a grid.
     * Control name must be unique: in a grid a unique column value can be used as name.
     *
     * @param  string  $bar_name          Control unique name
     * @param  string  $view              View name
     * @param  string  $id_field          Name of view-field containing name
     * @param  string  $progress_field    Name of view-field containing value
     * @param  string  $complete_action   Action to be executed on progress complete
     * @param  string  $start_action      Action to be executed on start (from 0 to >0)
     * @param  boolean $active            If bar is active and it makes update requests
     * @return string
     */
    static function get_by_view($bar_name,
                                $view,
                                $id_field,
                                $progress_field,
                                $complete_action = "",
                                $start_action    = "",
                                $active          = true) {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$app->progressivo_istanze];
        if (isset($prg->progress_ctrls[$bar_name])) {
            $bar = $prg->progress_ctrls[$bar_name];
            }
        else {
            $bar                  = new self($bar_name);
            $bar->task            = $view;
            $bar->id_field        = strtoupper($id_field);
            $bar->progress_field  = strtoupper($progress_field);
            $bar->complete_action = $complete_action;
            $bar->start_action    = $start_action;
            }
        $bar->modificabile($active);
        $bar->set_value_by_view();
        return $bar->get_code();

        }


    /**
     * Sets expression to be avaluated to get control value
     *
     * @param string $exp   Expression function as a string
     */
    function value($exp) {

        $this->caller = trim($exp, '()');

        }


    /**
     * Sets action to be executed on progress start
     *
     * @param string $action   Action name
     */
    function start_action($action) {

        $this->start_action = $action;

        }


    /**
     * Sets action to be executed on progress end
     *
     * @param string $action   Action name
     */
    function end_action($action) {

        $this->complete_action = $action;

        }

    /**
     * Disegna il controllo progress-bar
     *
     */
    function disegna() {

        if ($GLOBALS['jxjs']) {
            jxjs::ctrl_progress($this);
            }
        else {
            o2html::ctrl_progress($this);
            }

        }

    }

?>
