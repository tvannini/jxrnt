<?php

/**
 * Janox Environment Module
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
 * This script contains all elements needed by Janox structure:
 *  .: Instructions methods (o2act)
 *  .: Data views (recordsets & variables)
 *  .: Execution structures (o2_esecuzione)
 *  .: Program structure (o2_prg, ...)
 *
 *
 * @name      jxenv
 * @package   janox/jxenv.inc
 * @version   2.5
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


// ________________________________________________________________ Janox runtime core ___
include_once 'jxcore.inc';


/**
 * Program object
 *
 */
class o2_prg {

    /*     ===== PROPERTIES =========================================================== */
    public $nome            = "";      /* Program name                                  */
    public $id              = "";      /* Program instance unique id (prg exe id)       */
    public $exe_path        = "";      /* Unique prg mark with ancestors                */
    public $script          = "";      /* Program definition file name (.prg/.prg)      */
    /*     _____ ACTIONS ______________________________________________________________ */
    public $esecutivo       = "";      /* Action to execute at program start            */
    public $exit_now        = false;   /* If close-prg requested: exit actions in loop  */
    /*     _____ LISTS ________________________________________________________________ */
    public $parametri       = array(); /* Input parameters masks list                   */
    public $contesto        = array(); /* Views list                                    */
    public $form            = array(); /* Forms list                                    */
    public $azioni          = array(); /* Actions list                                  */
    public $protocolli      = array(); /* Input/output protocols list                   */
    public $risorse         = array(); /* Input/output channels list                    */
    public $reference_vars  = array(); /* Referenced variables used by o2refvar_get/set */
    public $profiling       = array(); /* Profiling options for single controls         */
    public $custom_grids    = false;   /* Grids custom settings                         */
    public $custom_ctrls    = array(); /* Controls custom position and size             */
    public $tree_ctrls      = array(); /* List of defined treeview controls             */
    public $imglist_ctrls   = array(); /* List of defined images-lister controls        */
    /*     _____ VIEWS REFRESH ________________________________________________________ */
    public $variazioni      = array(); /* Modified views list                           */
    public $evaluating_view = "";      /* View evaluating dependence for                */
    /*     _____ INTERNAL TOOL FLAG ___________________________________________________ */
    public $internal_tool   = false;   /* Program is an internal tool ("tools", "doc")  */
    /*     _____ SQL JOIN on prg views ________________________________________________ */
    public $link_check      = array(); /* Checks for finding join criteria              */
    public $link_field      = array(); /* Fields for finding join criteria              */
    public $link_view       = array(); /* Views for finding join criteria               */


    /**
     * Costruttore
     *
     * @param  string $nome
     * @param  string $id_esecuzione
     * @param  string $prefisso
     * @param  string $suffisso        NOT USED
     * @param  string $script
     * @return o2_prg
     */
    function __construct($nome, $id_esecuzione, $prefisso, $suffisso, $script = null) {

        if ($nome) {
            $app             = $_SESSION['o2_app'];
            $this->nome      = trim($nome);
            $this->id        = $id_esecuzione;
            $this->esecutivo = $prefisso;
            $this->exit_now  = false;
            $prgs            = "";
            foreach ($app->istanze_prg as $single_prg) {
                $prgs.= $single_prg->nome."|";
                }
            $this->exe_path = $prgs.$this->nome;
            if ($script) {
                $this->script = $script;
                }
            else {
                $this->script = trim($nome).".prg";
                }
            if ($app->multilang) {
                $this->load_translation();
                }
            }
        else {
            throw new o2_exception("Missing program name in definition",
                                   o2error_UNKNOWNPRG);
            }

        }


    /**
     * Definisce un parametro in entrata per il prg
     *
     * @param integer $id
     * @param string  $nome_parametro
     * @param string  $maschera
     */
    function set_par($id, $nome_parametro = "", $maschera = "") {

        if (!$_SESSION['o2_app']->maschere[$maschera]) {
            throw new o2_exception("Unknown data model <i>".$maschera.
                                   "</i> requested for parameter <i>[".$id."] ".
                                   $nome_parametro."</i> ",
                                   o2error_UNKNOWNMODEL);
            }
        else {
            $this->parametri[$id] = $_SESSION['o2_app']->maschere[$maschera];
            }

        }


    /**
     * Returns value of the requested parameter
     *
     * @param  integer $parameter_id
     * @return mix
     */
    function get_par($parameter_id) {

        if (isset($this->parametri[$parameter_id])) {
            $parameter_local = $this->parametri[$parameter_id];
            }
        else {
            $parameter_local = null;
            }
        $call_local = $_SESSION['o2_app']->chiamate[$this->id];
        if (isset($call_local['parametri'][$parameter_id - 1])) {
            $par_value_local = $call_local['parametri'][$parameter_id - 1]['valore'];
            switch ($parameter_local->tipo) {
                case "S":
                    if (func_num_args() > 1) {
                        $array_keys = func_get_args();
                        array_shift($array_keys);
                        $keys = "[".implode("][", $array_keys)."]";
                        eval("\$par_value_local = \$par_value_local".$keys.";");
                        }
                    else {
                        $par_value_local = ($par_value_local ?
                                            $par_value_local :
                                            $parameter_local->default);
                        }
                    break;
                case "A":
                    $par_value_local = (trim($par_value_local) !== "" ?
                                        $par_value_local :
                                        $parameter_local->default);
                                        break;
                default:
                    $par_value_local = ($par_value_local ?
                                        $par_value_local :
                                        $parameter_local->default);
                    break;
                }
            return $par_value_local;
            }
        else {
            return $parameter_local->default;
            }

        }


    /**
     * Creazione di una vista nel contesto del prg
     *
     * @param string $nome
     * @param string $prefisso
     * @param string $suffisso
     * @param string $modello_vista
     */
    function create_view($nome, $prefisso = "", $suffisso = "", $auto_aggr = true) {

        if (!isset($this->contesto[$nome])) {
            if ($nome != 'prg§_§var') {
                $this->contesto[$nome] = new o2_dbview($this->nome, $this->id, $nome);
                $this->contesto[$nome]->prefisso  = $prefisso;
                $this->contesto[$nome]->suffisso  = $suffisso;
                $this->contesto[$nome]->auto_aggr = $auto_aggr;
                $this->contesto[$nome]->struttura();
                }
            else {
                $this->contesto[$nome] = new o2_virtualview($this->nome, $this->id);
                }
            }

        }


    /**
     * Creazione di una form per il prg
     *
     * @param string  $nome
     * @param string  $linkto
     * @param boolean $menu_behavior
     * @param string  $vis_cond
     * @param string  $url
     */
    function form($nome,
                  $linkto        = "",
                  $menu_behavior = false,
                  $vis_cond      = "true",
                  $url           = false) {

        $nome.= "_form";
        if (!isset($this->form[$nome])) {
            $this->form[$nome] = new o2_form($this->nome,
                                             $this->id,
                                             $nome,
                                             $linkto,
                                             $menu_behavior,
                                             $vis_cond,
                                             $url);
            }

        }


    /**
     * Creazione di una risorsa per il prg
     *
     * @param string $nome
     * @param string $tipo
     * @param string $risorsa
     * @param string $direzione
     */
    function risorsa($nome, $tipo, $risorsa = "", $direzione = "O") {

        if (!$this->risorse[$nome]) {
            $this->risorse[$nome] = new o2_risorsa($this->nome,
                                                   $this->id,
                                                   $nome,
                                                   $tipo,
                                                   $risorsa,
                                                   $direzione);
        }

    }


    /**
     * Creazione di un protocollo I/O per il prg
     *
     * @param string $nome
     * @param string $tipo
     */
    function protocollo($nome, $tipo = "XML") {

        if (!$this->protocolli[$nome]) {
            $this->protocolli[$nome] = new o2_protocollo($this->nome,
                                                         $this->id,
                                                         $nome,
                                                         $tipo);
            }

        }


    /**
     * Definisce un'azione e la aggiunge alle azioni abilitate per il prg. Il valore
     * $service definisce il servizio che prenderà in carico l'esecuzione dell'azione.
     *
     * @param string $azione
     * @param string $service
     */
    function azione($azione, $service = "") {

        $this->azioni[$azione] = $service;

        }


    /**
     * Passa l'esecuzione all'azione [$action] fino al verificarsi di [$end_cond].
     * Se [$loop_view] è il nome di una vista del prg allora l'azione viene eseguita
     * dall'inizio del main file fino alla fine o fino a che la valutazione
     * dell'espressione [$end_cond] non risulta vera
     *
     * @param  string $action
     * @param  string $end_cond
     * @param  string $loop_view
     * @return boolean
     */
    function esegui_azione($action, $end_cond = true, $loop_view = "") {

        $app              = $_SESSION['o2_app'];
        $service          = $this->azioni[$action];
        $task_local       = $this->contesto[$loop_view];
        $counter_local    = 0;
        $act_end          = false;
        $condizione_local = false;
        $prosegui_loop    = true;
        if (!$app->ritorno && !($task_local && $task_local->act_wait_next)) {
            // _______________________________________________ Presets looping on view ___
            if ($loop_view) {
                // ________________________________ Set flag on view we are looping on ___
                $task_local->looping_on     = true;
                $task_local->righe_vis_save = $task_local->righe_vis;
                $task_local->righe_vis      = $task_local->file->batch_chunk;
                if (!$task_local->record_primo()) {
                    return false;
                    }
                if (!$task_local->corrente) {
                    $task_local->righe_vis = $task_local->righe_vis_save;
                    return true;
                    }
                }
            }
        // _______ If end condition is a constant it is evaluated before starting loop ___
        if (!is_string($end_cond)) {
            $condizione_local = $end_cond;
            $end_cond         = false;
            }
        // _______________________________________________________________ Action loop ___
        do {
            // ____________________________ If "returning" end single action execution ___
            if ($app->ritorno) {
                $app->ritorno = false;
                $act_end      = true;
                }
            // _____________ If loop was waiting for an on-line record-suffix to close ___
            elseif ($task_local->act_wait_next) {
                $task_local->act_wait_next = false;
                $act_end                   = true;
                }
            // __________________________________ Else execute single action execution ___
            else {
                $counter_local++;
                // _____________________________________ If action goes to end (batch) ___
                $act_end = $app->esecuzione_inizio($this->nome,
                                                   $this->id,
                                                   $action,
                                                   $service,
                                                   $counter_local);
                }
            // __________________________________ Prevent view refresh while executing ___
            if ($loop_view) {
                $task_local->vista = true;
                }
            // ______________________________________________ End condition evaluation ___
            if ($act_end) {
                if ($end_cond) {
                    eval("\$condizione_local = (".$end_cond.");");
                    }
                $prosegui_loop = !$loop_view || $task_local->record_avanti();
                // ___________________ Verify if view record-suffix stopped on on-line ___
                if ($task_local->suffix_running) {
                    $task_local->act_wait_next = true;
                    $act_end                   = false;
                    }
                // _____________________________________ ... or if it ended as a batch ___
                elseif ($task_local) {
                    $task_local->act_wait_next = false;
                    }
                }
            } while (!$condizione_local &&
                     $act_end &&
                     $prosegui_loop &&
                     !$this->exit_now);
        // _________________________ If loop action ended (exit with $act_end == TRUE) ___
        if ($loop_view && $act_end) {
            // _________________________________ Unset flag on view we were looping on ___
            $task_local->looping_on = false;
            // _______ Looping on view always requires view refresh to reset righe_vis ___
            $task_local->vista      = false;
            $task_local->direzione  = 0;
            $task_local->righe_vis  = $task_local->righe_vis_save;
            }
        // _______________________________________________ Return execution end status ___
        return $act_end;

        }


    /**
     * Relocate all prg views to last modified record
     *
     */
    function relocate_views() {

        foreach (array_keys($this->contesto) as $view_index) {
            if ($view_index && $view_index != "prg§_§var") {
                $this->contesto[$view_index]->relocate();
                }
            }

        }


    /**
     * Visualizza le form del prg che hanno la condizione di visibilità vera.
     * Introduce i campi che contengono i valori di selezione per le diverse viste del
     * program.
     *
     */
    function visualizza() {

        $app            = $_SESSION['o2_app'];
        $app->block_exe = true;
        if (count($this->form)) {
            // ___________________________________________ Display all visible windows ___
            foreach ($this->form as &$single_form) {
                $single_form->vis();
                }
            // ____________________________________________ Set views selection values ___
            if ($app->runtime->interface == "HTML") {
                if ($GLOBALS['jxjs']) {
                    foreach ($this->contesto as $view) {
                        if ($view->nome != "prg§_§var" && !$view->sele_by_table) {
                            $name  = $view->nome."_vs".$this->id;
                            $value = $view->selezione + 1;
                            print "jxv('".$name."',".$value.");\n";
                            }
                        $view->sele_by_table = false;
                        }
                    }
                else {
                    foreach ($this->contesto as $view) {
                        if ($view->nome != "prg§_§var") {
                            print "<input type='hidden' name='".$view->nome."_vs".
                                  $this->id."' value='".($view->selezione + 1)."'>\n";
                            }
                        }
                    }
                }
            }
        $app->block_exe = false;

        }


    /**
     * Conclude l'esecuzione del prg corrente
     *
     */
    function esci() {

        $app = $_SESSION['o2_app'];
        foreach ($this->form as &$single_form) {
                $single_form->chiusa = true;
                }
        foreach ($app->esecuzioni as $exe_id => $singola_esecuzione) {
            if ($singola_esecuzione->istanza_prg == $this->id) {
                $app->esecuzioni[$exe_id]->skip  = true;
                $app->esecuzioni[$exe_id]->passo = 0;
                }
            else {
                break;
                }
            }
        // ____________________________________________ Needed to exit looping actions ___
        $this->exit_now = true;

        }


    /**
     * Returns program ending state: a program is ended if has no running actions and no
     * visible forms.
     *
     * @return boolean
     */
    function concluso() {

        $app = $_SESSION['o2_app'];
        foreach ($this->form as $singola_form) {
            $cond_local = false;
            if (!$singola_form->chiusa) {
                if ($app->progressivo_istanze != $this->id) {
                    return false;
                    }
                else {
                    eval("\$cond_local = (".$singola_form->condizione.");");
                    if ($cond_local) {
                        return false;
                        }
                    }
                }
            }
        foreach ($app->esecuzioni as $singola_esecuzione) {
            if ($singola_esecuzione->istanza_prg == $this->id) {
                return false;
                }
            }
        return true;

        }


    /**
     * Clone program for cache storing to reuse it in further calls
     *
     * @return o2_prg
     */
    function clone4cache() {

        $aggr_vars = array();
        // _________________________ Unset variables values if not aggregation targets ___
        foreach ($this->contesto as &$single_view) {
            // __________________________ List variables that are aggregations tergets ___
            if (count($single_view->aggregate)) {
                foreach ($single_view->aggregate as $single_aggr) {
                    $aggr_vars[$single_aggr['retfield']];
                    }
                }
            // ______________________________ Sets flag on views to re-verify datasets ___
            $this->variazioni[$single_view->nome] = array($single_view->file->indice =>
                                                          true);
            }
        // ___________________________________________________________ Unset variables ___
        if (isset($this->contesto["prg§_§var"]) &&
            count($this->contesto["prg§_§var"]->variabili)) {
            foreach ($this->contesto["prg§_§var"]->variabili as $single_var) {
                $var_name = $single_var->nome_fisico;
                $var_val  = $single_var->maschera->default;
                if (!isset($aggr_vars[$var_name])) {
                    $this->contesto["prg§_§var"]->imposta($var_name, $var_val);
                    }
                }
            }
        // __________________________________________________ Unset focus informations ___
        if (isset($_SESSION['o2_app']->caret_position[$this->id])) {
            unset($_SESSION['o2_app']->caret_position[$this->id]);
            }
        // _______________________________________________________ Reopen closed forms ___
        foreach ($this->form as &$single_form) {
            $single_form->chiusa = false;
            }
        // ___________________________________________________________ Reset exit flag ___
        $this->exit_now   = false;
        // ______________________________________________________ Blank SQL JOIN flags ___
        $this->link_check = array();
        $this->link_field = array();
        $this->link_view  = array();
        // ___________________________________ Return cloned program for cache storing ___
        return clone $this;

        }


    /**
     * Sets control to get focus
     *
     * @param string  $form
     * @param string  $ctrl
     * @param integer $sele_mode
     * @param boolean $byuser
     */
    function set_caret($form, $ctrl, $sele_mode = null, $by_user = false) {

        $app = $_SESSION['o2_app'];
        if ($ctrl) {
            $app->info_submit = array("form" => $form, "ctrl" => $ctrl);
            if ($by_user                                ||
                !isset($app->caret_position[$this->id]) ||
                $app->caret_position[$this->id]["int"]) {
                $app->caret_position[$this->id] = array('form' => $form,
                                                        'ctrl' => $ctrl,
                                                        'sele' => $sele_mode,
                                                        'int'  => !$by_user);
                }
            }
        else {
            if (isset($app->caret_position[$this->id])) {
                unset($app->caret_position[$this->id]);
                }
            }

        }


    /**
     * Export data from a prg view
     *
     * @param  string  $view_name           Name of the view to export data from
     * @param  boolean $only_visible        Export only data visible on prg forms
     * @param  string  $fields_separator    Character use to separate fields
     * @param  string  $text_delimiter      Character used to delimiter text values
     * @param  string  $decimal_separator   Character used as decimal point in numbers
     * @param  string  $date_format         Format used for data fields (PHP date() style)
     * @param  string  $time_format         Format used for time fields (PHP date() style)
     * @param  boolean $titles_from_view    TRUE fields aliases in view are used as titles
     * @param  array   $excluded_aliases    List of aliases excluded in export
     * @param  array   $used_aliases        List of aliases to use for export
     * @return string
     */
    function export_data($view_name,
                         $file_name         = "",
                         $only_visible      = false,
                         $fields_separator  = ",",
                         $text_delimiter    = '"',
                         $decimal_separator = ".",
                         $date_format       = "Ymd",
                         $time_format       = "His",
                         $titles_from_view  = false,
                         $excluded_aliases  = false,
                         $used_aliases      = false) {

        $app                      = $_SESSION['o2_app'];
        $save_id                  = $app->progressivo_istanze;
        $exp_view                 = $this->contesto[$view_name];
        $prefix_save              = $exp_view->prefisso;
        $exp_view->prefisso       = false;
        $app->progressivo_istanze = $this->id;
        // _____________________________________________________________ Set file name ___
        if (!$file_name) {
            $file_name = $view_name."_".date("Ymd_His");
            }
        else {
            $file_name = pathinfo($file_name, PATHINFO_BASENAME);
            if (strtolower(pathinfo($file_name, PATHINFO_EXTENSION)) == "csv") {
                $file_name = pathinfo($file_name, PATHINFO_FILENAME);
                }
            }
        $csv_file = $app->dir_tmp.$file_name.".csv";
        $zip_file = $app->dir_tmp.$file_name.".zip";
        if (!($fh_local = fopen($csv_file, "w"))) {
            throw new o2_exception("Error writing to file <i>".$csv_file."</i>",
                                   o2error_IO);
            unset($exp_view);
            return false;
            }
        // ______________________ If passed $used_aliases overrides all other settings ___
        if (!$used_aliases || !is_array($used_aliases)) {
            // _________________________________________________ Select visible fields ___
            if (!$excluded_aliases || !is_array($excluded_aliases)) {
                $excluded_aliases = array();
                }
            $vis_fields  = array();
            $grid_fields = array();
            $titles      = array();
            $combos      = array();
            foreach ($this->form as $single_form) {
                if ($single_form->visible) {
                    foreach ($single_form->controlli as $single_ctrl) {
                        if ($single_ctrl->campo_in_ctrl &&
                            $single_ctrl->task == $view_name &&
                            (!$only_visible || $single_ctrl->is_visible()) &&
                            !in_array($single_ctrl->campo_in_ctrl, $excluded_aliases)) {
                            // _________________________________ Check title from grid ___
                            if ($single_ctrl->padre &&
                                $single_form->controlli[$single_ctrl->padre]->tipo_ctrl ==
                                                                                  "tab") {
                                if ($single_ctrl->info_padre[3]) {
                                    if ($app->multilang) {
                                        $title = $app->translate($single_ctrl->nome,
                                                                 'grid_title',
                                                                 0,
                                                             $single_ctrl->info_padre[3]);
                                        }
                                    else {
                                        $title = $single_ctrl->info_padre[3];
                                        }
                                    $titles[$single_ctrl->campo_in_ctrl] = $title;
                                    }
                                // __________________________ Add field to export list ___
                                $grid_fields[] = $single_ctrl->campo_in_ctrl;
                                }
                            else {
                                // __________________________ Add field to export list ___
                                $vis_fields[] = $single_ctrl->campo_in_ctrl;
                                }
                            $vis_fields[] = $single_ctrl->campo_in_ctrl;
                            // ______________________________ Add field to combos list ___
                            if ($single_ctrl->tipo_ctrl == "listcombo") {
                                $combos[$single_ctrl->campo_in_ctrl] = $single_ctrl;
                                }
                            }
                        }
                    }
                }
            // _______________________ Combine fields list to set first ones from grid ___
            $vis_fields = array_merge($grid_fields, $vis_fields);
            // ______________________________________ Filter $vis_fields by ->corrente ___
            if ($only_visible) {
                $unique  = array();
                foreach ($vis_fields as $field_id => $field_name) {
                    // _____________________________________ Take each field only once ___
                    if (isset($unique[$field_name])) {
                        unset($vis_fields[$field_id]);
                        }
                    else {
                        $unique[$field_name] = true;
                        }
                    }
                }
            // ______________________________________ All fields from view are visible ___
            else {
                $vis_fields = array();
                foreach ($exp_view->campi as $field_name => $single_field) {
                    if (!in_array($field_name, $excluded_aliases)) {
                        $vis_fields[] = $field_name;
                        }
                    }
                }
            }
        else {
            $vis_fields = array_keys($used_aliases);
            $titles     = $used_aliases;
            }
        // ________________________________________________________________ Titles row ___
        $exp_str = "";
        $exp_len = 0;
        $str2add = "";
        foreach ($vis_fields as $field_name) {
            $title = "";
            // _____________________________________________ Title from control (grid) ___
            if (isset($titles[$field_name])) {
                $title = str_replace($text_delimiter,
                                     $text_delimiter.$text_delimiter,
                                     $titles[$field_name]);
                }
            // ____________________________________ Use field alias from view as title ___
            elseif ($titles_from_view) {
                $title = $field_name;
                }
            // _______________________________ If field is in combo add title for code ___
            if (isset($combos[$field_name])) {
                $str2add.= $text_delimiter.($title ? $title." [CODE]" : "").
                           $text_delimiter.$fields_separator;
                }
            // _______________________________________________________ Add field title ___
            $str2add.= ($title ? $text_delimiter.$title.$text_delimiter : "").
                       $fields_separator;
            }
        $exp_str = substr($str2add, 0, -1)."\n";
        $exp_len = strlen($str2add);
        // ___________________________________________________ Loops until end of file ___
        while ($row = o2view_fetch($view_name)) {
            $str2add = "";
            foreach ($vis_fields as $field_name) {
                $field_value = $row[$field_name];
                $mask        = $exp_view->campi[$field_name]->maschera;
                switch ($mask->tipo) {
                    case "N":
                    case "L":
                        $str2add.= ($decimal_separator != "." ?
                                    str_replace(".",
                                                $decimal_separator,
                                                $field_value) :
                                    $field_value).$fields_separator;
                        break;
                    case "D":
                        if (trim($field_value, '0')) {
                            $d       = DateTime::createFromFormat('Ymd', $field_value);
                            $str2add.= $d->format($date_format).$fields_separator;
                            }
                        else {
                            if ($mask->zeroff) {
                                $str2add.= $fields_separator;
                                }
                            else {
                                $zero_date = date($date_format,
                                                  mktime(0, 0, 0, 1, 1, 2001));
                                $str2add  .= str_replace("2", "0",
                                                         str_replace("1", "0",
                                                                     $zero_date)).
                                             $fields_separator;
                                }
                            }
                        break;
                    case "O":
                        if (!trim($field_value, '0')) {
                            $field_value = '000000';
                            }
                        $t = DateTime::createFromFormat('His', $field_value);
                        if ($t) {
                            $str2add.= $t->format($time_format).$fields_separator;
                            }
                        else {
                            $str2add.= '000000'.$fields_separator;
                            }
                        break;
                    default:
                        $str2add.= ($field_value ?
                                    $text_delimiter.
                                    str_replace("\n", "\r",
                                                str_replace($text_delimiter,
                                                            $text_delimiter.
                                                            $text_delimiter,
                                                            $field_value)).
                                                $text_delimiter : "").
                                    $fields_separator;
                        break;
                    }
                // _______________________________ If field is in combo set two values ___
                if (isset($combos[$field_name])) {
                    $combos[$field_name]->valore = $field_value;
                    $desc_value = $combos[$field_name]->get_desc();
                    $str2add   .= $text_delimiter.
                                  str_replace($text_delimiter,
                                              $text_delimiter.$text_delimiter,
                                              $desc_value).
                                  $text_delimiter.$fields_separator;
                    }
                }
            $str2add = substr($str2add, 0, -1)."\n";
            $exp_str.= $str2add;
            $exp_len+= strlen($str2add);
            if ($exp_len > 65536) {
                fwrite($fh_local, $exp_str);
                $exp_str = "";
                $exp_len = 0;
                }
            };
        fwrite($fh_local, $exp_str);
        fclose($fh_local);
        $exp_view->prefisso = $prefix_save;
        // _________________________ This needed for call-prg in record prefix actions ___
        unset($app->chiamate[$app->progressivo_istanze + 1]);
        $exp_view->record_primo();
        $app->progressivo_istanze = $save_id;
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
     * Load grids custom settings.
     *
     * This method is called after prg inclusion, to know if there are defined forms or
     * not.
     *
     */
    function load_custom_grids() {

        $app  = $_SESSION['o2_app'];
        $save = $app->option_get("tab_settings_save");
        if ($app->grid_plus &&
            ($this->custom_grids === false) &&
            count($this->form) &&
            ($save === "1" || $save === "2")) {
            $this->custom_grids = array();
            $cs_tab             = $app->get_table("o2_custom_grids");
            $cs_db              = $cs_tab->db;
            $cs_server          = $cs_db->server;
            $co                 = constant('o2_'.$cs_server->type.'_o');
            $cc                 = constant('o2_'.$cs_server->type.'_c');
            $csf_user           = $cs_tab->campi['o2user']->nome_fisico;
            $csf_prg            = $cs_tab->campi['prg']->nome_fisico;
            $csf_obj            = $cs_tab->campi['obj']->nome_fisico;
            $csf_type           = $cs_tab->campi['p_type']->nome_fisico;
            $csf_col            = $cs_tab->campi['p_col']->nome_fisico;
            $csf_value          = $cs_tab->campi['p_value']->nome_fisico;
            $csn_user           = strtoupper($cs_tab->campi['o2user']->phys_name);
            $csn_obj            = strtoupper($cs_tab->campi['obj']->phys_name);
            $csn_type           = strtoupper($cs_tab->campi['p_type']->phys_name);
            $csn_col            = strtoupper($cs_tab->campi['p_col']->phys_name);
            $csn_value          = strtoupper($cs_tab->campi['p_value']->phys_name);
            $select             = $csf_user.' '.$co.$csn_user.$cc.','.
                                  $csf_obj.' '.$co.$csn_obj.$cc.','.
                                  $csf_type.' '.$co.$csn_type.$cc.','.
                                  $csf_col.' '.$co.$csn_col.$cc.','.
                                  $csf_value.' '.$co.$csn_value.$cc;
            // ____________________________ Blank user records are default set by root ___
            if ($app->user == 'root') {
                $where = $csf_user."='' AND ".$csf_prg."='".$this->nome."'";
                }
            else {
                $where = "(".$csf_user."='".$app->user.
                         "' OR ".$csf_user."='') AND ".
                         $csf_prg."='".$this->nome."'";
                }
            $res        = o2_gateway::recordset($cs_server->type,
                                                $cs_server->server,
                                                $cs_server->user,
                                                $cs_server->password,
                                                $cs_db->nome,
                                                $cs_db->proprietario,
                                                $cs_tab->nome,
                                                $cs_tab->nome,
                                                $select,
                                                $where,
                                                false,
                                                1000);
            $defaults   = array();
            $custom_obj = array();
            foreach ($res as $prop) {
                // ___________________________________________________ Manage defaults ___
                if ($prop[$csn_user] && $prop[$csn_type] == 'X') {
                    $custom_obj[$prop[$csn_obj]] = true;
                    }
                elseif ($prop[$csn_user] == '') {
                    $defaults[$prop[$csn_obj]]
                             [$prop[$csn_type]]
                             [$prop[$csn_col]] = $prop[$csn_value];
                    }
                else {
                    $this->custom_grids[$prop[$csn_obj]]
                                       [$prop[$csn_type]]
                                       [$prop[$csn_col]] = $prop[$csn_value];
                    }
                }
            // ________________________ Add defaults if not custom settings for object ___
            foreach ($defaults as $obj_name => $obj_set) {
                if (!isset($custom_obj[$obj_name])) {
                    $this->custom_grids[$obj_name] = $obj_set;
                    }
                }
            }
        else {
            $this->custom_grids = array();
            }

        }


    /**
     * Return log history as an array.
     * History is a list of insert, update, delete events on field binding to control.
     *
     * @param  string  $ctrl_name     Name of the control to get history for
     * @param  boolean $with_record   If TRUE record log is returned too
     * @return array
     */
    function log_history($ctrl_name, $with_record = false) {

        $app   = $_SESSION['o2_app'];
        $ctrl  = false;
        $view  = false;
        $field = false;
        foreach ($this->form as $form) {
           if (isset($form->controlli[$ctrl_name])) {
              $ctrl  = $form->controlli[$ctrl_name];
              $view  = $this->contesto[$ctrl->task];
              $field = $view->campi[$ctrl->campo_in_ctrl];
              break;
              }
           }
        if (!$ctrl) {
            throw new o2_exception("Unknown control <i>".$ctrl_name."</i> in program <i>".
                                   $this->nome."</i>",
                                   o2error_UNKNOWNCTRL);
            return false;
            }
        elseif (!$field) {
            throw new o2_exception("Control <i>".$ctrl_name.
                                   "</i> has no field in program <i>".$this->nome."</i>",
                                   o2error_UNKNOWNFIELD);
            return false;
            }
        $tab    = $app->tab[$view->files[$field->file]->log_table];
        $db     = $tab->db;
        $server = $db->server;
        $key    = $tab->chiave;
        // _____________________________________________________ Compose select string ___
        $selstr = "log_id, log_act, log_user, ".$field->nome_fisico." jxlogfield";
        // ____________________ Compose where clause by pkey (from the original table) ___
        $where  = $view->chiave_corrente($view->files[$field->file],
                                         false,
                                         false,
                                         false,
                                         false,
                                         false,
                                         true);
        // ______________________________________________________ Compose order clause ___
        $order  = "";
        foreach ($tab->chiave->segmenti as $segm) {
           $order.= ($order ? ", " : "").$segm->campo->nome_fisico." ".
                    ($segm->direzione == "D" ? "DESC " : "ASC ");
           }
        // _________________________________________________________ Get log recordset ___
        $set  = o2_gateway::recordset($server->type,
                                      $server->server,
                                      $server->user,
                                      $server->password,
                                      $db->nome,
                                      $db->proprietario,
                                      $tab->nome,
                                      $field->file,
                                      $selstr,
                                      $where,
                                      $order,
                                      1000);
        // _____________________________________________________ Include current value ___
        $rset = array(array('id'    => 0,
                            'date'  => date('Ymd'),
                            'time'  => date('His'),
                            'act'   => 'A',
                            'user'  => '',
                            'value' => $view->precedente($field->nome)));
        // ________________________________________________________ Refine log dataset ___
        $last = "";
        foreach ($set as $prog => $rec) {
            $rec   = array_change_key_case($rec);
            $prec  = ($set[$prog + 1] ? array_change_key_case($set[$prog + 1]) : array());
            $id    = $rec["log_id"];
            $date  = date("Ymd", substr($id, 0, -6));
            $time  = date("His", substr($id, 0, -6));
            $value = $rec["jxlogfield"];
            // ________________________ "D" records of UPDATE pairs are changed to "P" ___
            if ($id == $prec["log_id"] &&
                $rec["log_act"] == "D" &&
                $prec["log_act"] == "U") {
                $act = "P";
                }
            else {
                $act = $rec["log_act"];
                }
            // _______________________________ Filter records where field is unchanged ___
            if ($value != $last || $act != "U") {
                $rset[] = array("id"    => $id,
                                "date"  => $date,
                                "time"  => $time,
                                "act"   => $act,
                                "user"  => $rec["log_user"],
                                "value" => $value);
                }
            $last = $value;
            }
        if ($with_record) {
            $t_name = $view->files[$field->file]->repos_index;
            $ret    = array('tab' => $t_name,
                            'fld' => $rset,
                            'rec' => $this->log_record($view->nome, $field->file));
            if ($app->record_trace && is_array($app->tab[$t_name]->record_trace) &&
                (!isset($GLOBALS['jxtracesuspend']) ||
                 !in_array($t_name, $GLOBALS['jxtracesuspend']))) {
                $ret['trace'] = array();
                // _____________________________ Remove delimiters from tracing fields ___
                $co = constant('o2_'.$server->type.'_o');
                $cc = constant('o2_'.$server->type.'_c');
                foreach ($app->tab[$t_name]->record_trace as $ttype => $tfield) {
                    $ret['trace'][$ttype] = rtrim(ltrim($tfield, $co), $cc);
                    }
                }
            return $ret;
            }
        else {
            return $rset;
            }

        }


    /**
     * Return log history as an array.
     * History is a list of insert, update, delete events on actual record of a table.
     *
     * @param  string $view_name     Name of the view to get history for
     * @param  string $table_alias   Alias of the table to get history for
     * @return array
     */
    function log_record($view_name, $table_alias) {

        $app  = $_SESSION['o2_app'];
        $view = false;
        $view = $this->contesto[$view_name];
        if (!$view) {
            throw new o2_exception("Unknown view <i>".$view_name."</i> in program <i>".
                                   $this->nome."</i>",
                                   o2error_UNKNOWNVIEW);
            return false;
            }
        elseif (!$view->files[$table_alias]) {
            throw new o2_exception("Unknown table <i>".$table_alias."</i> in view <i>".
                                   $view_name."</i> of program <i>".$this->nome."</i>",
                                   o2error_UNKNOWNTABLE);
            return false;
            }
        $src_tab  = $view->files[$table_alias];
        $src_name = $src_tab->write_name;
        $db       = $src_tab->db;
        $server   = $db->server;
        // _____________________________________________________ Compose select string ___
        $selstr   = '';
        foreach ($src_tab->campi as $f_obj) {
            $selstr.= ($selstr ? ',' : '').$f_obj->nome_fisico;
            }
        // ____________________ Compose where clause by pkey (from the original table) ___
        $where    = $view->chiave_corrente($src_tab,
                                           false,
                                           false,
                                           false,
                                           false,
                                           false,
                                           true);
        // ______________________________________________________ Compose order clause ___
        $order    = "";
        foreach ($src_tab->chiave->segmenti as $segm) {
           $order.= ($order ? ", " : "").$segm->campo->nome_fisico." ".
                    ($segm->direzione == "D" ? "DESC " : "ASC ");
           }
        // _____________________________________________________ Include current value ___
        $rec  = o2_gateway::recordset($server->type,
                                      $server->server,
                                      $server->user,
                                      $server->password,
                                      $db->nome,
                                      $db->proprietario,
                                      $src_name,
                                      $table_alias,
                                      $selstr,
                                      $where,
                                      '',
                                      1);
        $res  = ($rec[0] ? array_change_key_case($rec[0]) : array());
        $rset = array(array('jxlog_id'   => 0,
                            'jxlog_date' => date('Ymd'),
                            'jxlog_time' => date('His'),
                            'jxlog_act'  => 'A',
                            'jxlog_user' => '') + $res);
        // _________________________________________________________ Get log recordset ___
        $tab    = $app->tab[$view->files[$table_alias]->log_table];
        $db     = $tab->db;
        $server = $db->server;
        // ___________________________________________ Add log fields to select string ___
        $selstr = 'log_id,log_act,log_user,'.$selstr;
        // ______________________________________________________ Compose order clause ___
        $order  = "";
        foreach ($tab->chiave->segmenti as $segm) {
           $order.= ($order ? ", " : "").$segm->campo->nome_fisico." ".
                    ($segm->direzione == "D" ? "DESC " : "ASC ");
           }
        $set = o2_gateway::recordset($server->type,
                                     $server->server,
                                     $server->user,
                                     $server->password,
                                     $db->nome,
                                     $db->proprietario,
                                     $tab->nome,
                                     $table_alias,
                                     $selstr,
                                     $where,
                                     $order,
                                     1000);
        // ________________________________________________________ Refine log dataset ___
        foreach ($set as $prog => $rec) {
            $rec  = array_change_key_case($rec);
            $prec = ($set[$prog + 1] ? array_change_key_case($set[$prog + 1]) : array());
            $id   = $rec["log_id"];
            $user = $rec["log_user"];
            $date = date("Ymd", substr($id, 0, -6));
            $time = date("His", substr($id, 0, -6));
            // ________________________ "D" records of UPDATE pairs are changed to "C" ___
            if ($rec["log_act"] == "D" &&
                $prec["log_act"] == "U" &&
                $id == $prec["log_id"]) {
                $act = "C";
                }
            else {
                $act = $rec["log_act"];
                }
            unset($rec["log_id"]);
            unset($rec["log_user"]);
            unset($rec["log_act"]);
            if (isset($rec["o2aspid"])) {
                unset($rec["o2aspid"]);
                }
            // _________________________________________________ Filter update records ___
            $rset[] = array("jxlog_id"   => $id,
                            "jxlog_date" => $date,
                            "jxlog_time" => $time,
                            "jxlog_act"  => $act,
                            "jxlog_user" => $user) + $rec;
            }
        return $rset;

        }


    /**
     * Load translation table for current language set in options
     *
     */
    function load_translation() {

        $app = $_SESSION['o2_app'];
        if (!isset($app->translation[$this->nome])) {
            // ______________________________________________________ /app/lang/<lang> ___
            $lang_dir = new o2_dir($app->dir_home.'/lang/'.$app->multilang.'/');
            // _____________________________________________________ Translation table ___
            if ($lang_dir->exists($this->nome.'.lng')) {
                $app->translation[$this->nome] = jx_parse_conf($lang_dir.$this->nome.
                                                               '.lng');
                }
            }

        }

    }


/**
 * Execution object
 *
 */
class o2_esecuzione {

    /*     ===== PROPERTIES =========================================================== */
    public $azione      = "";    /* Action name                                         */
    public $prg         = "";    /* Action owning program name                          */
    public $istanza_prg = "";    /* Action owning program id (PRG_EXE_ID)               */
    public $passo       = 0;     /* Executing instruction id                            */
    public $loop        = 0;     /* Loop counter index                                  */
    public $definition  = "";    /* Definition function name                            */
    public $def_script  = "";    /* Definition script for internal actions              */
    public $skip        = false; /* If program ended rest of execution is skipped       */
    public $suffix_of   = false; /* View name action is record-suffix of (if any)       */


    /**
     * Costruttore
     *
     * @param  string $prg
     * @param  string $id_istanza_prg
     * @param  string $azione
     * @param  string $service
     * @return o2_esecuzione
     */
    function __construct($prg, $id_istanza_prg, $azione, $service) {

        $this->azione      = $azione;
        $this->prg         = $prg;
        $this->istanza_prg = $id_istanza_prg;
        $this->definition  = $prg."§§".$azione.'_act';

        }


    /**
     * ToString: used only by error messages in o2_exception
     *
     * @return string
     */
    function __toString() {

        return '{'.$this->prg.'|'.$this->azione.'}';

        }


    /**
     * Esegue l'azione definita per l'istanza di esecuzione
     *
     */
    function esegui() {

        if ($this->def_script) {
            eval($this->def_script);
            }
        $action_local = $this->definition;
        $action_local($this);

        }


    /**
     * Se un passo è da eseguire ritorna TRUE e se non è in esecuzione ce lo mette
     *
     * @param  integer $exe_id
     * @return boolean
     */
    function s($exe_id) {

        // ________________________________ Prg ended and rest of execution is skipped ___
        if ($this->skip) {
            return false;
            }
        // _____________________________________ A step is already running and waiting ___
        elseif ($this->passo) {
            // __________________ This same step is running so it's a return from call ___
            if ($this->passo == $exe_id) {
                $_SESSION['o2_app']->ritorno = true;
                return true;
                }
            // _________________________________________________ Other step is waiting ___
            else {
                return false;
                }
            }
        // ________________________________________________ Step is free for execution ___
        else {
            $this->passo                 = $exe_id;
            $_SESSION['o2_app']->ritorno = false;
            return true;
            }

        }


    /**
     * Termina l'esecuzione di un passo
     *
     * @return boolean
     */
    function e() {

        $this->passo = 0;
        $_SESSION['o2_app']->ritorno = false;
        return false;

        }

    }


/**
 * Application variable object
 *
 */
class o2_appvar extends o2_campo {

    public $valore = ""; /* Variable value                                              */


    function __construct($nome, $maschera) {

        $this->file        = "";
        $this->nome_fisico = $nome;
        $this->phys_name   = $nome;
        if ($maschera && $_SESSION['o2_app']->maschere[$maschera]) {
            $this->maschera = $_SESSION['o2_app']->maschere[$maschera];
            $this->tipo     = $this->maschera->tipo;
            $this->valore   = $this->maschera->default;
            }
        else {
            throw new o2_exception("Unknown data model <i>".$maschera.
                                   "</i> requested for application variable <i>".
                                   $nome."</i>",
                                   o2error_UNKNOWNMODEL);
            }

        }

    }


/**
 * View field
 *
 */
class o2_campo_task extends o2_campo {

    /*     ===== PROPERTIES =========================================================== */
    public $nome       = "";      /* Unique field name                                  */
    public $min        = "";      /* MINIMUM RANGE expression                           */
    public $max        = "";      /* MAXIMUM RANGE expression                           */
    public $not        = "";      /* NOT RANGE expression                               */
    public $like       = "";      /* LIKE RANGE expression                              */
    public $condizione = "";      /* Formulas range condition                           */
    public $formula    = "";      /* Formulas value expression                          */
    public $order_by   = array(); /* Formulas ORDER_BY equivalent                       */
    public $valore     = "";      /* Variables (prg§_§var) value                        */
    public $selector   = false;   /* If formula is a check selector for view            */


    /**
     * Costruttore
     *
     * @param  string   $con_nome
     * @param  string   $alias_tabella
     * @param  o2_campo $campo
     * @param  mix      $min
     * @param  mix      $max
     * @param  mix      $not
     * @param  string   $like
     * @return o2_campo_task
     */
    function __construct($con_nome, $alias_tabella, $campo, $min, $max, $not, $like) {

        $this->nome        = $con_nome;
        $this->file        = $alias_tabella;
        $this->maschera    = $campo->maschera;
        $this->nome_fisico = $campo->nome_fisico;
        $this->phys_name   = $campo->phys_name;
        $this->tipo        = $campo->tipo;
        $this->min         = $min;
        $this->max         = $max;
        $this->not         = $not;
        $this->like        = $like;
        $this->condizione  = "";
        $this->valore      = "";
        $this->formula     = "";

        }

    }


/**
 * View
 *
 */
class o2_view {

    /*     ===== PROPERTIES =========================================================== */
    public $dbdata        = false;   /* If view content is from DB, FALSE=virtual_view  */
    public $nome          = "";      /* View name                                       */
    public $prg           = "";      /* Name of program containing view                 */
    public $id_esecuzione = 0;       /* Prg execution id (PRG_EXE_ID)                   */
    public $def_function  = "";      /* Definition function in program script           */
    public $righe_vis     = 10;      /* Number of rows displayed for interaction        */
    /*     _____ INTERNAL LISTS _______________________________________________________ */
    public $recordset     = array(); /* Array of records of current recordset           */
    public $corrente      = array(); /* Array of values of current record               */
    public $precedente    = array(); /* Array of values before mod of current record    */
    public $verified      = array(); /* Last verified (links and formulas) record       */
    public $dipendenze    = array(); /* Dependent views list                            */
    /*     _____ INTERNAL FLAGS _______________________________________________________ */
    public $status        = "M";     /* Behaviour status: [I]nsert [D]elete [M]odify    */
    public $modificato    = false;   /* If current record has been modified             */


    /**
     * Comunica, a seguito di una variazione per questa vista, alle viste dipendenti di
     * aggiornare i propri contesti
     *
     */
    function comunica_variazioni() {

        $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
        foreach (array_keys($this->dipendenze) as $pending_view) {
            $view = $prg->contesto[$pending_view];
            if ($view instanceof o2_dbview) {
                foreach ($view->files as $single_file) {
                    $prg->variazioni[$pending_view][$single_file->indice] = true;
                    }
                $view->comunica_variazioni();
                }
            else {
                unset($this->dipendenze[$pending_view]);
                }
            }

        }

    }


class o2_dbview extends o2_view {

    /*     ===== PROPERTIES =========================================================== */
    /**
     * @var o2_file
     */
    public $file            = "";    /* Object o2_file of view main file                */
    public $chunk           = 1;     /* Records chunk in request to db                  */
    public $prefisso        = "";    /* Action to be executed as "view prefix"          */
    public $suffisso        = "";    /* Action to be executed as "view suffix"          */
    public $prefisso_exe    = true;  /* If prefix is to be executed                     */
    public $internal        = false; /* If table has not _def function                  */
    public $auto_aggr       = true;  /* If aggregation are valued automatically         */
    /*     _____ DEFINITION LISTS _____________________________________________________ */
    public $files           = array(); /* o2_file objects of used files                 */
    public $select_str      = array(); /* SELECT string of used fields for each file    */
    public $campi           = array(); /* o2_campo objects of used fields               */
    public $formule         = array(); /* Calculated fields                             */
    public $sql_formulas    = array(); /* SQL calculated fields                         */
    public $aggregate       = array(); /* Aggregate expressions to eval on view         */
    public $wheres          = array(); /* WHERE clauses for each file                   */
    public $esclusivi       = array(); /* Fields with strict ranges (no link criteria)  */
    public $chiavi_in_uso   = array(); /* Used index for each file                      */
    public $chiavi_exps     = array(); /* Expressions evaluating to used index          */
    public $indici          = array(); /* ORDER BY clauses for each file                */
    public $indici_inversi  = array(); /* Inverse ORDER BY clauses for each file        */
    public $sort            = array(); /* Temporary SORT fields                         */
    /*     _____ INTERNAL LISTS _______________________________________________________ */
    public $link_cache      = array(); /* Links returned datasets cache for reusing     */
    public $ultima_where    = array(); /* Last WHERE clause used for each file          */
    public $selections      = array(); /* List of selected items for selection-view     */
    /*     _____ SQL-LINK _____________________________________________________________ */
    public $link_on         = array(); /* SQL join on criteria for single tables        */
    public $link_wheres     = array(); /* Range (where) criteria for linked tables      */
    /*     _____ INTERNAL FLAGS _______________________________________________________ */
    public $selezione       = 0;     /* Index of current record in recordset            */
    public $totale_record   = 101;   /* Number of records filtered in whole main file   */
    public $righe_vis_ori   = 0;     /* N. of rows per page used by custom rows reset   */
    public $righe_vis_save  = 0;     /* Saved righe-vis to reset after looping on view  */
    public $user_rows       = 0;     /* N. of rows set by user in custom settings       */
    public $fine            = false; /* If reached end condition in recordset building  */
    public $structured      = false; /* If ->struttura() method has been called before  */
    public $vista           = false; /* If view has an actual recordset                 */
    public $computing       = false; /* If we already are within recordset computing    */
    public $suffix_running  = false; /* If we already are within suffix execution       */
    public $suffix_waiting  = false; /* If executing suffix action is waiting on-line   */
    public $relocate_to_key = false; /* Key segment values to relocate to               */
    public $relocated       = false; /* If view has been relocated after last drawn     */
    public $sele_by_table   = false; /* If view selection in driven by a grid (HTML-FA) */
    public $act_wait_next   = false; /* If loop on view is waiting for next row         */
    public $looping_on      = false; /* If an action is looping on view                 */
    public $custom_from     = false; /* If FROM clausole is defined by expression       */
    public $cached_insert   = false; /* Multiple inserts cached in a single statement   */
    public $insert_cache    = '';    /* Cached records for cached-insert mode           */
    /*     _____ RECORDSET PAGING _____________________________________________________ */
    public $statement       = null;  /* PDO Statement opened on view (if any)           */
    public $offset_ini      = 0;     /* First recordset row index in whole file         */
    public $offset_fine     = 0;     /* Last recordset row index in whole file          */
    public $direzione       = 0;     /* Records retrieving direction:
                                        0   = first n records
                                        1   = previous n records
                                        2   = next n records
                                        3   = last n records
                                        100 = current n records                         */
    public $eof             = true;  /* End of file reached                             */
    public $bof             = true;  /* Beginning of file reached                       */
    public $key_filter_same = "";    /* WHERE clause to actual recordset                */
    public $key_filter_prev = "";    /* WHERE clause to previous recordset              */
    public $key_filter_next = "";    /* WHERE clause to next recordset                  */


    /**
     * Costruttore
     *
     * @param  string $prg
     * @param  string $id_esecuzione_prg
     * @param  string $nome
     * @return o2_view
     */
    function __construct($prg, $id_esecuzione_prg, $nome = "") {

        if ($nome == "") {
            throw new o2_exception("Missing view name in definition",
                                   o2error_UNKNOWNVIEW);
            }
        else {
            $this->dbdata        = true;
            $this->nome          = $nome;
            $this->prg           = $prg;
            $this->id_esecuzione = $id_esecuzione_prg;
            $this->def_function  = $this->prg."§§".$this->nome."_def";
            }

        }


    /**
     * Add, for each table in view, fields needed by used key and ansures presence of pkey
     * segments
     *
     */
    function add_key_fields() {

        foreach ($this->files as &$single_file) {
            $file_name = $single_file->indice;
            // ____________________ Skip first time (from prg def) to avoid executions ___
            if ($this->structured) {
                // ____________________________________________ View key by expression ___
                if ($this->chiavi_exps[$file_name]) {
                    $key_by_exp = $this->chiavi_exps[$file_name]();
                    if ($key_by_exp != $this->chiavi_in_uso[$file_name]) {
                        $this->chiavi_in_uso[$file_name] = $key_by_exp;
                        unset($this->indici[$file_name]);
                        unset($this->indici_inversi[$file_name]);
                        }
                    }
                }
            $pk_segms = $single_file->pk_segments;
            $used_key = $single_file->chiavi[$this->chiavi_in_uso[$file_name]];
            foreach ($used_key->segmenti as &$single_seg) {
                $fieldname     = strtoupper($single_seg->campo->phys_name);
                $fieldname_kf  = $fieldname."_JXKF";
                $field_present = false;
                foreach ($this->campi as &$single_field) {
                    if ($single_field->file == $file_name &&
                        strtoupper($single_field->phys_name) == $fieldname) {
                        $phys_name = $single_field->nome_fisico;
                        if (isset($pk_segms[$phys_name])) {
                            unset($pk_segms[$phys_name]);
                            }
                        $field_present = true;
                        break;
                        }
                    }
                if (!$field_present) {
                    $this->campi[$fieldname_kf] = new o2_campo_task($fieldname_kf,
                                                                    $file_name,
                                                                    $single_seg->campo,
                                                                    null,
                                                                    null,
                                                                    null,
                                                                    null);
                    if (isset($pk_segms[$single_seg->campo->nome_fisico])) {
                        unset($pk_segms[$single_seg->campo->nome_fisico]);
                        }
                    }
                }
            // _ Main file used key is not primary key: check if missing pkey segments ___
            if ($file_name == $this->file->indice &&
                $used_key->nome != $single_file->chiave->nome &&
                count($pk_segms)) {
                foreach ($pk_segms as $seg_field) {
                    $seg_name = $seg_field.'_JXKF';
                    $this->campi[$seg_name] = new o2_campo_task($seg_name,
                                                                $file_name,
                                                      $single_file->campi[$seg_field],
                                                                null,
                                                                null,
                                                                null,
                                                                null);
                    }
                }
            }

        }


    /**
     * Legge la struttura del task basata su corrente e compone le stringhe di SELECT per
     * i singoli file
     *
     */
    function struttura() {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$app->progressivo_istanze];
        // _______________________________ Eseguo il main per avere tutti i file usati ___
        $this->aggregate = array();
        $def_function    = $this->def_function;
        if ($this->internal ||
           (is_callable($def_function) && ($def_function($this) || true))) {
            // ____ Aggiungo i campi della chiave in uso per il file ai campi del task ___
            $this->add_key_fields();
            if (!$this->file) {
                $this->righe_vis = 0;
                }
            // ____________________________________________ User temporaneous settings ___
            elseif ($this->user_rows) {
                $this->righe_vis_ori = $this->righe_vis;
                $this->righe_vis     = $this->user_rows;
                }
            // __________________________________________________ User global settings ___
           elseif ($prg->custom_grids[$this->nome]["R"][""]) {
                $this->righe_vis_ori = $this->righe_vis;
                $this->user_rows     = $prg->custom_grids[$this->nome]["R"][""];
                $this->righe_vis     = $this->user_rows;
                }
            // ___________________ Composizione delle SELECT e calcolo del batch chunk ___
            $this->select_str = array();
            $record_size      = 0;
            $file_type        = $this->file->db->server->type;
            $c_o              = constant("o2_".$file_type."_o");
            $c_c              = constant("o2_".$file_type."_c");
            // ___________________________ Add O2ASPID field for main table, if needed ___
            if ($this->files[$this->file->indice]->asp == 'C') {
                $this->select_str[$this->file->indice] = $c_o.$this->file->indice.$c_c.
                                                         ".O2ASPID";
                }
            foreach ($this->campi as &$singolo_campo) {
                if (($singolo_campo->file == $this->file->indice) &&
                    !$singolo_campo->formula) {
                    $record_size+= min(256, $singolo_campo->maschera->dimensione);
                    }
                $file_type = $this->files[$singolo_campo->file]->db->server->type;
                $c_o       = constant("o2_".$file_type."_o");
                $c_c       = constant("o2_".$file_type."_c");
                $comma     = ($this->select_str[$singolo_campo->file] ? "," : "");
                if ($singolo_campo->formula) {
                    $this->select_str[$singolo_campo->file].= $comma.
                                                              $singolo_campo->formula." ".
                                                              $c_o.$singolo_campo->nome.
                                                              $c_c;
                    }
                else {
                    $this->select_str[$singolo_campo->file].= $comma.
                                                              $c_o.$singolo_campo->file.
                                                              $c_c.".".
                                                              $singolo_campo->nome_fisico.
                                                              " ".$c_o.
                                                              $singolo_campo->nome.$c_c;
                    }
                }
            $this->file->batch_chunk = max(128, intval($this->file->db->chunk_size /
                                                       max($record_size, 1)));
            // _______________________________ Page records number for looping on view ___
            if ($this->looping_on) {
                $this->righe_vis = $this->file->batch_chunk;
                }
            $this->structured = true;
            }
        else {
            throw new o2_exception("Cannot find definition for view <i>".
                                   $this->nome."</i>",
                                   o2error_MISSINGDEF);
            }

        }


    /**
     * Imposta il main file per il task
     *
     * @param string $tabella    Table name from the tables-repository
     * @param string $con_nome   Table alias used in the view
     * @param string $chiave     Index name to order by
     * @param string $db         Alternative DB name for the table
     * @param string $phys_tab   Alternative physical name for the table
     */
    function usa_file($tabella, $con_nome, $chiave, $db = null, $phys_tab = null) {

        $this->usa_file_link($tabella, $con_nome, $chiave, $db, $phys_tab);
        $this->file = $this->files[$con_nome];

        }


    /**
     * Imposta l'indice per i file non primari
     *
     * @param string $tabella    Table name from the tables-repository
     * @param string $con_nome   Table alias used in the view
     * @param string $chiave     Index name to order by
     * @param string $db         Alternative DB name for the table
     * @param string $phys_tab   Alternative physical name for the table
     */
    function usa_file_link($tabella, $con_nome, $chiave, $db = null, $phys_tab = null) {

        $app = $_SESSION['o2_app'];
        // ___________________________________________ Only first time view is defined ___
        if (!$this->structured) {
            // ___________________________________________ Clone table from repository ___
            $this->files[$con_nome]             = $app->get_table($tabella, true);
            $this->files[$con_nome]->write_name = $this->files[$con_nome]->nome;
            $this->files[$con_nome]->indice     = $con_nome;
            // ___________________________________________________ Index by expression ___
            if (strpos($chiave, "()")) {
                $this->chiavi_in_uso[$con_nome] = $this->files[$con_nome]->chiave->nome;
                $this->chiavi_exps[$con_nome]   = str_replace("()", "", $chiave);
                }
            // _________________________________________________________ Index by name ___
            else {
                $this->chiavi_in_uso[$con_nome] = $chiave;
                $this->chiavi_exps[$con_nome]   = false;
                }
            }
        // ______________________________________________________ Alternative database ___
        if ($db !== null) {
            // ___________________________________ Set flag to restructure view on use ___
            $this->custom_from = true;
            if ($db) {
                if (isset($app->db[$db])) {
                    $this->files[$con_nome]->set_db($app->db[$db]);
                    $type = $app->db[$db]->server->type;
                    $app->runtime->load_gateway($type);
                    if (!in_array($type, $app->engines)) {
                        $app->engines[] = $type;
                        }
                    }
                else {
                    throw new o2_exception("Unknown database <i>".$db.
                                           "</i> requested for table <i>".$con_nome.
                                           " (".$tabella.")</i>",
                                           o2error_UNKNOWNDBSERVER);
                    }
                }
            }
        // _________________________________________________ Alternative physical name ___
        if ($phys_tab !== null) {
            // ___________________________________ Set flag to restructure view on use ___
            $this->custom_from = true;
            $ut                = $this->files[$con_nome];
            if ($phys_tab) {
                // ________________________________________ Table is provided by query ___
                if (strpos($phys_tab, " ")) {
                    // ___ Write name is used for write operations instead of subquery ___
                    if (!$ut->write_name) {
                        $ut->write_name = $ut->nome;
                        }
                    $ut->nome     = "(".$phys_tab.")";
                    $ut->subquery = true;
                    }
                // ________________________________ Table is provided by physical name ___
                else {
                    $ut->nome       = $phys_tab;
                    $ut->write_name = $phys_tab;
                    $ut->subquery   = false;
                    }
                }
            else {
                $t              = $app->get_table($tabella);
                $ut->nome       = $t->nome;
                $ut->write_name = $t->nome;
                $ut->subquery   = false;
                }
            }

        }


    /**
     * Applica al task il modello di view
     *
     * @param string $view_model
     * @param string $con_nome
     */
    function usa_view($view_model, $con_nome) {

        throw new o2_exception("Sorry, use of view-model <i>".$view_model.
                               "</i> in view <i>".$this->nome."</i> is not supported",
                               o2error_UNKNOWNVIEWMODEL);

        }


    /**
     * Aggiunge un campo al task con criteri esclusivi di WHERE
     *
     * @param string $con_nome
     * @param string $tabella
     * @param string $campo
     * @param string $range_min
     * @param string $range_max
     * @param string $not
     * @param string $like
     */
    function usa($con_nome,
                 $tabella,
                 $campo,
                 $range_min = null,
                 $range_max = null,
                 $not       = null,
                 $like      = null) {

        $con_nome = strtoupper($con_nome);
        if (!is_null($range_min) ||
            !is_null($range_max) ||
            !is_null($not)       ||
            !is_null($like)) {
            $this->esclusivi[$tabella][$con_nome] = true;
            }
        $this->unisci($con_nome,
                      $tabella,
                      $campo,
                      $range_min,
                      $range_max,
                      $not,
                      $like,
                      true);

        }


    /**
     * Aggiunge un campo al task con i criteri per una link
     *
     * @param string $con_nome
     * @param string $tabella
     * @param string $campo
     * @param string $range_min
     * @param string $range_max
     * @param string $not
     * @param string $like
     */
    function unisci($con_nome,
                    $tabella,
                    $campo,
                    $range_min   = null,
                    $range_max   = null,
                    $not         = null,
                    $like        = null,
                    $from_select = false) {

        $con_nome  = strtoupper($con_nome);
        if (!$from_select &&
            (strlen($con_nome) > 30) &&
            (substr($con_nome, -6) == "_JOIN_")) {
            $con_nome = "JLF_".substr(md5($con_nome), 0, 26);
            }
        $tab_local = $this->files[$tabella];
        if (isset($this->campi[$con_nome])) {
            unset($this->campi[$con_nome]);
            }
        $this->campi[$con_nome] = new o2_campo_task($con_nome,
                                                    $tabella,
                                                    $tab_local->campi[$campo],
                                                    $range_min,
                                                    $range_max,
                                                    $not,
                                                    $like);

        }


    /**
     * Aggiunge un'espressione calcolata alla vista del task
     *
     * @param string $nome_formula
     * @param string $formula
     * @param string $range_min
     * @param string $range_max
     * @param string $not
     * @param string $like
     */
    function calcola($nome_formula,
                     $formula   = null,
                     $range_min = null,
                     $range_max = null,
                     $not       = null,
                     $like      = null) {

        $nome_formula = strtoupper($nome_formula);
        $this->formule[$nome_formula] = new o2_campo_task($nome_formula,
                                                          "",
                                                          new o2_campo("", $nome_formula),
                                                          $range_min,
                                                          $range_max,
                                                          $not,
                                                          $like);
        $this->formule[$nome_formula]->condizione = "";
        if (!is_null($formula)) {
            $this->formule[$nome_formula]->formula = $formula;
            }

        }


    /**
     * Add a SQL formula to view
     *
     * @param string $name
     * @param string $formula
     * @param string $range_min
     * @param string $range_max
     * @param string $not
     * @param string $like
     */
    function sql_formula($name,
                         $formula   = null,
                         $range_min = null,
                         $range_max = null,
                         $not       = null,
                         $like      = null) {

        $name  = strtoupper($name);
        $table = $this->file->indice;
        $type  = $this->file->db->server->type;
        $c_o   = constant("o2_".$type."_o");
        $c_c   = constant("o2_".$type."_c");
        $sql   = '';
        // __________________________________________________ Compose formula SQL code ___
        if (is_array($formula) && (count($formula) > 0)) {
            switch (strtoupper($formula[0])) {
                case 'CONCAT':
                    $str = array();
                    for ($i = 1; $i < count($formula); $i++) {
                        if (is_array($formula[$i])) {
                            if ($field = $this->campi[strtoupper($formula[$i][0])]) {
                                $str[] = $c_o.$field->file.$c_c.".".$field->nome_fisico;
                                }
                            }
                        else {
                            $str[] = "'".o2_gateway::normalize($type, $formula[$i], true).
                                     "'";
                            }
                        }
                    $sql = o2_gateway::concat($type, $str);
                    break;
                default:
                    throw new o2_exception("Unknown SQL function ".$formula[0].
                                           "in view <i>".$this->nome."</i>",
                                           o2error_UNKNOWNFIELD);
                    break;
                }
            }
        if (isset($this->campi[$name])) {
            unset($this->campi[$name]);
            }
        if (!is_null($range_min) ||
            !is_null($range_max) ||
            !is_null($not)       ||
            !is_null($like)) {
            $this->esclusivi[$table][$name] = true;
            }
        $this->campi[$name] = new o2_campo_task($name,
                                                $table,
                                                new o2_campo($table,
                                                             $name,
                                                             $c_o.$name.$c_c),
                                                $range_min,
                                                $range_max,
                                                $not,
                                                $like);
        $this->campi[$name]->formula = $sql;

        }


    /**
     * Ritorna il riferimento all'oggetto o2_campo_task da associare ad un controllo
     *
     * @param  string $nome_campo
     * @return o2_campo_task
     */
    function campo_per_controllo($nome_campo) {

        $nome_campo = strtoupper($nome_campo);
        if (key_exists($nome_campo, $this->campi)) {
            return $this->campi[$nome_campo];
            }
        elseif (key_exists($nome_campo, $this->formule)) {
            return $this->formule[$nome_campo];
            }

        }


    /**
     * Creates or recreates data content if view has changed.
     * Returns TRUE if view is refreshed.
     *
     * @return boolean
     */
    function refresh() {

        $ret = false;
        // _____________ If view is not already computing and it is not in insert mode ___
        if (!$this->computing && $this->status != "I") {
            // _______________________________________________ If view has a recordset ___
            if ($this->vista) {
                if ($this->range_mod()) {
                    $this->record_primo();
                    $ret = true;
                    }
                }
            // ______________________________________________________ Create recordset ___
            else {
                $this->crea_recordset();
                $ret = true;
                }
            }
        return $ret;

        }


    /**
     * Calculate aggregation functions and set aggregated results to target fields.
     *
     */
    function calcola_aggregate() {

        if (count($this->aggregate)) {
            $app = $_SESSION['o2_app'];
            // ____________________________________________ Check if view is COUNTABLE ___
            $countable = true;
            // __________________________________________________ Check ranges on link ___
            foreach ($this->esclusivi as $file_name => $per_file) {
                if ($per_file[0] &&
                    $file_name != $this->file->indice &&
                    !$this->files[$file_name]->link_by_sql) {
                    $countable = false;
                    break;
                    }
                }
            // ______________________________________________ Check ranges on formulas ___
            if ($this->wheres["o2calc§_§exps"]) {
                $countable = false;
                }
            // ____________________________________ Use database AGGREGATE (COUNTABLE) ___
            if ($countable) {
                $arr_local = array();
                // _________________________________ Compose array for gateway command ___
                foreach ($this->aggregate as $i => $aggr) {
                    $c_o       = constant("o2_".$this->file->db->server->type."_o");
                    $c_c       = constant("o2_".$this->file->db->server->type."_c");
                    $field_obj = $this->campi[$aggr['field']];
                    $field     = $c_o.$field_obj->file.$c_c.'.'.$field_obj->nome_fisico;
                    $arr_local["JXFUNC_".$i] = array('func'  => $aggr['func'],
                                                     'field' => $field);
                    }
                // ________________________ Compose WHERE clause for SQL-linked tables ___
                foreach ($this->files as $linkidx => $linktab) {
                    if ($linktab->link_by_sql) {
                        $this->range2where($linkidx);
                        }
                    }
                // __________________ Add SQL-linked table WHERE clauses to main table ___
                $mainfile = $this->file->indice;
                if ($this->link_wheres) {
                    $this->wheres[$mainfile].= ($this->wheres[$mainfile] ? " AND " : "").
                                                implode(" AND ", $this->link_wheres);
                    $this->ultima_where[$mainfile][0] = $this->wheres[$mainfile];
                    }
                $links   = (is_array($this->link_on) ? $this->link_on : null);
                $ret_val = o2_gateway::aggregate($this->file->db->server->type,
                                                 $this->file->db->server->server,
                                                 $this->file->db->server->user,
                                                 $this->file->db->server->password,
                                                 $this->file->db->nome,
                                                 $this->file->db->proprietario,
                                                 $this->file->nome,
                                                 $this->file->indice,
                                                 $this->wheres[$this->file->indice],
                                                 $arr_local,
                                                 $links);
                // ________________________________ Get aggregated values from dataset ___
                foreach ($this->aggregate as $i => $aggr) {
                    $app->istanze_prg[$app->progressivo_istanze]->
                      contesto[$aggr["retview"]]->imposta($aggr["retfield"],
                                                          $ret_val["JXFUNC_".$i]);
                    }
                }
            // ___________________________ Aggregate loop for UNCOUNTABLE aggregations ___
            else {
                // ____ Bug compatibility tip: skip uncountable aggr and log a warning ___
                if ($app->bug_aggregate) {
                    o2_exception::warning("Can't aggregate view <i>".$this->nome.
                                          "</i> with unsolvable filters");
                    }
                else {
                    $this->aggregate_loop();
                    }
                }
            }
        return true;

        }


    /**
     * Calcola le aggregazioni per viste con filtri su LINK non risolte e/o formule.
     * Questo metodo esegue un loop di conteggio sull'intero recordset della vista e il
     * risultato è corretto per qualunque tipo di vista.
     *
     */
    function aggregate_loop() {

        $app   = $_SESSION['o2_app'];
        $prg   = $app->istanze_prg[$app->progressivo_istanze];
        $count = 0;
        // _____________________________________ Copy view properties to restore later ___
        $view_copy = array();
        foreach (get_object_vars($this) as $prop_id => $single_prop) {
            if (is_object($single_prop)) {
                $view_copy[$prop_id] = clone $single_prop;
                }
            else {
                $view_copy[$prop_id] = $single_prop;
                }
            }
        $this->righe_vis  = $this->file->batch_chunk;
        $this->suffisso   = false;
        $this->prefisso   = false;
        $aggregations     = $this->aggregate;
        $this->aggregate  = array();
        // _____________________________________________________________ Start dataset ___
        $this->looping_on = true;
        $this->record_primo();
        if (count($this->recordset)) {
            $res = array();
            // ___________________________________________________ Preset result array ___
            foreach ($aggregations as $i => $aggr) {
                switch ($aggr['func']) {
                    case 'COUNT':
                        $res[$i] = 0;
                        break;
                    case 'SUM':
                        $res[$i] = 0;
                        break;
                    case 'MAX':
                        $res[$i] = false;
                        break;
                    case 'MIN':
                        $res[$i] = false;
                        break;
                    case 'AVG':
                        $res[$i] = 0;
                        break;
                    }
                }
            do {
                foreach ($aggregations as $i => $aggr) {
                    $count++;
                    $value = $this->corrente[$aggr['field']];
                    switch ($aggr['func']) {
                        case 'COUNT':
                            $res[$i]++;
                            break;
                        case 'SUM':
                            $res[$i]+= $value;
                            break;
                        case 'MAX':
                            $res[$i] = ($res[$i] === false ?
                                        $value : max($res[$i], $value));
                            break;
                        case 'MIN':
                            $res[$i] = ($res[$i] === false ?
                                        $value : min($res[$i], $value));
                            break;
                        case 'AVG':
                            $res[$i]+= $value;
                            break;
                        }
                    }
                } while ($this->record_avanti());
            // ________________________________ Riporto i valori calcolati in contesto ___
            foreach ($aggregations as $i => $aggr) {
                if ($aggr['func'] == 'AVG') {
                    $value = $res[$i] / $count;
                    }
                else {
                    $value = $res[$i];
                    }
                $prg->contesto[$aggr["retview"]]->imposta($aggr["retfield"], $value);
                }
            }
        $this->looping_on = false;
        // ___________________________________________________ Restore view properties ___
        foreach (get_object_vars($this) as $prop_id => $single_prop) {
            // ________________________________________________ Save useful properties ___
            if ($prop_id != 'totale_record') {
                $this->$prop_id = $view_copy[$prop_id];
                }
            }
        $this->computing = false;
        return;

        }


    /**
     * Legge l'elenco dei campi del task e crea le clausole WHERE di ogni file per ogni
     * record della vista. Ritorna true se le clausole sono variate dall'ultima produzione
     * della vista
     *
     * @param  string  $file_in_esame
     * @param  integer $record
     * @return boolean
     */
    function range2where($file_in_esame, $record = -1) {

        $app              = $_SESSION['o2_app'];
        $prg              = $app->istanze_prg[$this->id_esecuzione];
        $evaluating_local = $prg->evaluating_view;
        $where_local      = array();
        $main_db          = $this->file->db;
        $link_db          = $this->files[$file_in_esame]->db;
        $file_type        = $link_db->server->type;
        $link_prefix      = o2_gateway::qualify($file_type,
                                                $link_db->nome,
                                                $link_db->proprietario);
        $c_o              = constant("o2_".$file_type."_o");
        $c_c              = constant("o2_".$file_type."_c");
        $full_tab_alias   = $c_o.$file_in_esame.$c_c;
        $file_idx         = o2_gateway::qualify($file_type,
                                                $link_db->nome,
                                                $link_db->proprietario,
                                                $this->files[$file_in_esame]->nome).
                            " ".$c_o.$file_in_esame.$c_c;
        unset($this->link_on[$file_idx]);
        $prg->evaluating_view               = $this->nome;
        $this->esclusivi[$file_in_esame][0] = false;
        // ___________________________ Condition to use SQL join instead of Janox link ___
        /*
         * SQL-JOIN criteria:
         *  1. file is not main one (link table)
         *  2. main table and link table share the same physical SQL server
         *  3. Postgres tables share the same physical db, too
         *
         *  NOTE: SQL-JOIN behaviour is disabled in INSERT mode
         *        See ->verifica_record() method too
         */
        $sql_join_app = $file_in_esame         != $this->file->indice &&
                        $main_db->server->type == $file_type &&
                        $main_db->server->nome == $link_db->server->nome &&
                        ($main_db->nome        == $link_db->nome ||
                         $file_type            != 'postgres');
        if (!$sql_join_app && $file_in_esame != $this->file->indice) {
            $this->files[$file_in_esame]->not_sql_join = true;
            }
        // ___________________________ Get Primary Key segments to check link criteria ___
        $pk_segms = $this->files[$file_in_esame]->pk_segments;
        foreach ($this->campi as &$campo) {
            if ($campo->file == $file_in_esame) {
                $single_cond         = false;
                $nome_completo_campo = $campo->nome_fisico;
                // ________________ Property ->not_sql_join may change at every field! ___
                $sql_join = $sql_join_app && !$this->files[$file_in_esame]->not_sql_join;
                // _________________________________________ MIN == MAX - Strict range ___
                if (!is_null($campo->min) && ($campo->min == $campo->max)) {
                    // ____________________________________________________ Expression ___
                    if (strpos($campo->min, "_exp_") !== false) {
                        // __________________________________________________ SQL-LINK ___
                        if ($sql_join &&
                            !isset($this->esclusivi[$file_in_esame][$campo->nome])) {
                            $valore_local = null;
                            $pk_segms     = $this->check_join_field($file_in_esame,
                                                                    $campo,
                                                                    $pk_segms);
                            // _ Save link where clausole, used by ->verifica_record() ___
                            if ($this->status == "I" || $this->modificato) {
                                eval("\$valore_local = ".$campo->min.";");
                                }
                            }
                        else {
                            eval("\$valore_local = ".$campo->min.";");
                            }
                        }
                    // _________________________________________________________ Value ___
                    else {
                        $valore_local = $campo->min;
                        }
                    if (!is_null($valore_local)) {
                        if ($campo->tipo == "N") {
                            if ($valore_local) {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."=".
                                               number_format($valore_local,
                                                             $campo->maschera->decimali,
                                                             ".",
                                                             "").")";
                                }
                            // _________________________________ Manage zero (0) value ___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."=0 OR ".
                                                   $full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                                }
                            }
                        elseif ($campo->tipo == "L") {
                            if ($valore_local) {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."='1')";
                                }
                            // ____________________________________ Manage FALSE value ___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."<'1' OR ".
                                                   $full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                                }
                            }
                        elseif ($campo->tipo == "D") {
                            if ($valore_local && $valore_local != "00000000") {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "='".$valore_local."')";
                                }
                            // ________________________ Manage zero ('00000000') value ___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "='00000000' OR ".$full_tab_alias.".".
                                                         $nome_completo_campo." IS NULL)";
                                }
                            }
                        elseif ($campo->tipo == "O") {
                            if ($valore_local && $valore_local != "000000") {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "='".$valore_local."')";
                                }
                            // __________________________ Manage zero ('000000') value ___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."='000000' OR ".
                                                   $full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                                }
                            }
                        elseif ($campo->tipo == "F") {
                            if ($valore_local !== "") {
                                $single_cond = "(".$campo->formula."='".
                                               o2_gateway::normalize($file_type,
                                                                     $valore_local)."')";
                                }
                            // _______________________________ Manage blank ('') value ___
                            else {
                                $single_cond = "(RTRIM(".$campo->formula.")='' OR RTRIM(".
                                               $campo->formula.") IS NULL)";
                                }
                            }
                        elseif ($valore_local !== "") {
                            $single_cond = "(".$full_tab_alias.".".$nome_completo_campo.
                                           "='".o2_gateway::normalize($file_type,
                                                                      $valore_local)."')";
                            }
                        // ___________________________________ Manage blank ('') value ___
                        else {
                            $single_cond = "(RTRIM(".$full_tab_alias.".".
                                                     $nome_completo_campo.
                                           ")='' OR RTRIM(".$full_tab_alias.".".
                                                            $nome_completo_campo.
                                           ") IS NULL)";
                            }
                        if (isset($this->esclusivi[$file_in_esame][$campo->nome])) {
                            $this->esclusivi[$file_in_esame][0] =
                            $this->esclusivi[$file_in_esame][$campo->nome];
                            $main_where = $this->link_wheres[$file_in_esame];
                            if ($sql_join &&
                                strpos($main_where, $single_cond) === false) {
                                $this->link_wheres[$file_in_esame].= ($main_where ?
                                                                      " AND " : "").
                                                                      $single_cond;
                                }
                            }
                        $where_local[] = $single_cond;
                        $single_cond   = false;
                        }
                    }
                else {
                    // _________________________________________________ MINIMUM Range ___
                    if (!is_null($campo->min)) {
                        // ________________________________________________ Expression ___
                        if (strpos($campo->min, "_exp_") !== false) {
                            eval("\$valore_local = ".$campo->min.";");
                            }
                        // _____________________________________________________ Value ___
                        else {
                            $valore_local = $campo->min;
                            }
                        if (!is_null($valore_local)) {
                            if ($campo->tipo == "N") {
                                if ($valore_local) {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo.">=".
                                                   number_format($valore_local,
                                                               $campo->maschera->decimali,
                                                                 ".",
                                                                 "").")";
                                    }
                                else {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo.">=0 OR ".
                                                       $full_tab_alias.".".
                                                       $nome_completo_campo." IS NULL)";
                                    }
                                }
                            elseif ($campo->tipo == "L") {
                                if ($valore_local) {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo."='1')";
                                    }
                                }
                            elseif ($campo->tipo == "D") {
                                if ($valore_local && $valore_local != "00000000") {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo.
                                                   ">='".$valore_local."')";
                                    }
                                }
                            elseif ($campo->tipo == "O") {
                                if ($valore_local && $valore_local != "000000") {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo.
                                                   ">='".$valore_local."')";
                                    }
                                }
                            elseif ($campo->tipo == "F") {
                                if ($valore_local !== "") {
                                    $single_cond = "(".$campo->formula.">='".
                                                   o2_gateway::normalize($file_type,
                                                                         $valore_local).
                                                   "')";
                                    }
                                }
                            elseif ($valore_local !== "") {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.">='".
                                                o2_gateway::normalize($file_type,
                                                                      $valore_local)."')";
                                }
                            if ($single_cond) {
                                if (isset($this->esclusivi[$file_in_esame]
                                                          [$campo->nome])) {
                                    $this->esclusivi[$file_in_esame][0] =
                                    $this->esclusivi[$file_in_esame][$campo->nome];
                                    $main_where = $this->link_wheres[$file_in_esame];
                                    if ($sql_join &&
                                        strpos($main_where, $single_cond) === false) {
                                        $this->link_wheres[$file_in_esame].=($main_where ?
                                                                            " AND " : "").
                                                                             $single_cond;
                                        }
                                    }
                                // _____ Remove from SQL-JOIN if already added to list ___
                                elseif ($sql_join) {
                                    $this->files[$file_in_esame]->not_sql_join = true;
                                    unset($this->link_on[$file_idx]);
                                    unset($this->link_wheres[$file_in_esame]);
                                    }
                                $where_local[] = $single_cond;
                                $single_cond   = false;
                                }
                            }
                        }
                    // _________________________________________________ MAXIMUM Range ___
                    if (!is_null($campo->max)) {
                        // ________________________________________________ Expression ___
                        if (strpos($campo->max, "_exp_") !== false) {
                            eval("\$valore_local = ".$campo->max.";");
                            }
                        // _____________________________________________________ Value ___
                        else {
                            $valore_local = $campo->max;
                            }
                        if (!is_null($valore_local)) {
                            if ($campo->tipo == "N") {
                                if ($valore_local) {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo.
                                                   "<=".number_format($valore_local,
                                                               $campo->maschera->decimali,
                                                                      ".",
                                                                      "").")";
                                    }
                                else {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo."<=0 OR ".
                                                       $full_tab_alias.".".
                                                       $nome_completo_campo." IS NULL)";
                                    }
                                }
                            elseif ($campo->tipo == "L") {
                                if (!$valore_local) {
                                    $single_cond = "(".$full_tab_alias.".".
                                                       $nome_completo_campo."<'1' OR ".
                                                       $full_tab_alias.".".
                                                       $nome_completo_campo." IS NULL)";
                                    }
                                }
                            elseif ($campo->tipo == "D") {
                                if (!$valore_local) {
                                    $valore_local = "00000000";
                                    }
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "<='".$valore_local."' OR ".
                                                     $full_tab_alias.".".
                                                     $nome_completo_campo." IS NULL)";
                                }
                            elseif ($campo->tipo == "O") {
                                if (!$valore_local) {
                                    $valore_local = "000000";
                                    }
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "<='".$valore_local."' OR ".
                                                     $full_tab_alias.".".
                                                     $nome_completo_campo." IS NULL)";
                                }
                            elseif ($campo->tipo == "F") {
                                // ____________________ Manage blank ('') value & NULLs___
                                $single_cond = "(".$campo->formula."<='".
                                                   o2_gateway::normalize($file_type,
                                                                         $valore_local).
                                               "' OR RTRIM(".$campo->formula.") IS NULL)";
                                }
                            // ________________________ Manage blank ('') value & NULLs___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."<='".
                                                   o2_gateway::normalize($file_type,
                                                                         $valore_local).
                                               "' OR RTRIM(".$full_tab_alias.".".
                                                             $nome_completo_campo.
                                               ") IS NULL)";
                                }
                            if ($single_cond) {
                                if (isset($this->esclusivi[$file_in_esame]
                                                          [$campo->nome])) {
                                    $this->esclusivi[$file_in_esame][0] =
                                    $this->esclusivi[$file_in_esame][$campo->nome];
                                    $main_where = $this->link_wheres[$file_in_esame];
                                    if ($sql_join &&
                                        strpos($main_where, $single_cond) === false) {
                                        $this->link_wheres[$file_in_esame].=($main_where ?
                                                                            " AND " : "").
                                                                            $single_cond;
                                        }
                                    }
                                // _____ Remove from SQL-JOIN if already added to list ___
                                elseif ($sql_join) {
                                    $this->files[$file_in_esame]->not_sql_join = true;
                                    unset($this->link_on[$file_idx]);
                                    unset($this->link_wheres[$file_in_esame]);
                                    }
                                $where_local[] = $single_cond;
                                $single_cond   = false;
                                }
                            }
                        }
                    }
                // _________________________________________________________ NOT Range ___
                if (!is_null($campo->not)) {
                    // ____________________________________________________ Expression ___
                    if (strpos($campo->not, "_exp_") !== false) {
                        eval("\$valore_local = ".$campo->not.";");
                        }
                    // _________________________________________________________ Value ___
                    else {
                        $valore_local = $campo->not;
                        }
                    if (!is_null($valore_local)) {
                        if ($campo->tipo == "N") {
                            if ($valore_local) {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "<>".number_format($valore_local,
                                                               $campo->maschera->decimali,
                                                                  ".",
                                                                  "")." OR ".
                                                   $full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                                }
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."<>0)";
                                }
                            }
                        elseif ($campo->tipo == "L") {
                            if ($valore_local) {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."<'1' OR ".
                                                   $full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                                }
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."='1')";
                                }
                            }
                        elseif ($campo->tipo == "D") {
                            if ($valore_local && $valore_local != "00000000") {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "<>'".$valore_local."' OR ".
                                                   $full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                                }
                            // ________________________ Manage zero ('00000000') value ___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."<>'00000000')";
                                }
                            }
                        elseif ($campo->tipo == "O") {
                            if ($valore_local && $valore_local != "000000") {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo.
                                               "<>'".$valore_local."' OR ".
                                                     $full_tab_alias.".".
                                                     $nome_completo_campo." IS NULL)";
                                }
                            // __________________________ Manage zero ('000000') value ___
                            else {
                                $single_cond = "(".$full_tab_alias.".".
                                                   $nome_completo_campo."<>'000000')";
                                }
                            }
                        elseif ($campo->tipo == "F") {
                            if ($valore_local !== "") {
                                $single_cond = "(".$campo->formula."<>'".
                                               o2_gateway::normalize($file_type,
                                                                    $valore_local).
                                               "' OR ".$campo->formula." IS NULL)";
                                }
                            // _______________________________ Manage blank ('') value ___
                            else {
                                $single_cond = "(RTRIM(".$camp->formula.")>'')";
                                }
                            }
                        elseif ($valore_local !== "") {
                            $single_cond = "(".$full_tab_alias.".".$nome_completo_campo.
                                           "<>'".o2_gateway::normalize($file_type,
                                                                       $valore_local).
                                           "' OR ".$full_tab_alias.".".
                                                   $nome_completo_campo." IS NULL)";
                            }
                        // ___________________________________ Manage blank ('') value ___
                        else {
                            $single_cond = "(RTRIM(".$full_tab_alias.".".
                                                     $nome_completo_campo.")>'')";
                            }
                        if ($single_cond) {
                            if (isset($this->esclusivi[$file_in_esame][$campo->nome])) {
                                $this->esclusivi[$file_in_esame][0] =
                                $this->esclusivi[$file_in_esame][$campo->nome];
                                $main_where = $this->link_wheres[$file_in_esame];
                                if ($sql_join &&
                                    strpos($main_where, $single_cond) === false) {
                                    $this->link_wheres[$file_in_esame].= ($main_where ?
                                                                          " AND " : "").
                                                                          $single_cond;
                                    }
                                }
                            // _________ Remove from SQL-JOIN if already added to list ___
                            elseif ($sql_join) {
                                $this->files[$file_in_esame]->not_sql_join = true;
                                unset($this->link_on[$file_idx]);
                                unset($this->link_wheres[$file_in_esame]);
                                }
                            $where_local[] = $single_cond;
                            $single_cond   = false;
                            }
                        }
                    }
                // ________________________________________________________ LIKE Range ___
                if (!is_null($campo->like)) {
                    // ____________________________________________________ Expression ___
                    if (strpos($campo->like, "_exp_") !== false) {
                        eval("\$valore_local = rtrim(".$campo->like.");");
                        }
                    // _________________________________________________________ Value ___
                    else {
                        $valore_local = rtrim($campo->like);
                        }
                    if ($valore_local) {
                        if (substr($valore_local, -1) == "*") {
                            $valore_local = substr($valore_local, 0, -1);
                            $sf           = "";
                            }
                        else {
                            $sf = "%";
                            }
                        // _______ Double slash needed in LIKE for Postgres and Oracle ___
                        if ($file_type == "postgres" || $file_type == "oracle") {
                            $valore_local = strtr($valore_local, array('\\' => '\\\\'));
                            }
                        $valore_local = o2_gateway::normalize($file_type, $valore_local);
                        if ($campo->tipo == "F") {
                            // ______________________ SQL UPPER for key-insensitive search ___
                            $single_cond = "UPPER(".$campo->formula.
                                           ") LIKE '".$sf.strtoupper($valore_local)."%'";
                            }
                        else {
                            // __________________ SQL UPPER for key-insensitive search ___
                            $single_cond = "UPPER(".$full_tab_alias.".".
                                                    $nome_completo_campo.
                                           ") LIKE '".$sf.strtoupper($valore_local)."%'";
                            }
                        if (isset($this->esclusivi[$file_in_esame][$campo->nome])) {
                            $this->esclusivi[$file_in_esame][0] =
                            $this->esclusivi[$file_in_esame][$campo->nome];
                            $main_where = $this->link_wheres[$file_in_esame];
                            if ($sql_join &&
                                strpos($main_where, $single_cond) === false) {
                                $this->link_wheres[$file_in_esame].= ($main_where ?
                                                                      " AND " : "").
                                                                      $single_cond;
                                }
                            }
                        // _________ Remove from SQL-JOIN if already added to list ___
                        elseif ($sql_join) {
                            $this->files[$file_in_esame]->not_sql_join = true;
                            unset($this->link_on[$file_idx]);
                            unset($this->link_wheres[$file_in_esame]);
                            }
                        $where_local[] = $single_cond;
                        $single_cond   = false;
                        }
                    }
                }
            }
        if ($this->files[$file_in_esame]->link_by_sql) {
            // _______________ Check for SQL joins with wrong criteria (incomplete PK) ___
            if (count($pk_segms)) {
                $this->files[$file_in_esame]->not_sql_join = true;
                }
            // ___________________ Check for SQL joins with wrong criteria (by values) ___
            if ($this->files[$file_in_esame]->not_sql_join) {
                $this->files[$file_in_esame]->link_by_sql = false;
                $prg->evaluating_view                     = $evaluating_local;
                unset($this->link_on[$file_idx]);
                unset($this->link_wheres[$file_in_esame]);
                return $this->range2where($file_in_esame, $record);
                }
            elseif ($this->status != "I" && !$this->modificato) {
                $record = -1;
                }
            }
        // _______________________________________ Add ASPID filter if anabled on file ___
        if ($this->files[$file_in_esame]->asp == 'C' && $app->vars['_area']->valore) {
            array_unshift($where_local, $c_o.$file_in_esame.$c_c.".O2ASPID='".
                          $app->vars['_area']->valore."'");
            }
        // ______________________________________________________ Compose WHERE clause ___
        $this->wheres[$file_in_esame] = implode(" AND ", $where_local);
        $prg->evaluating_view         = $evaluating_local;
        // __________________________________________ Merge WHERE clause from SQL-LINK ___
        if ($file_in_esame == $this->file->indice && count($this->link_wheres)) {
            $new_where = $this->wheres[$file_in_esame].
                         ($this->wheres[$file_in_esame] ? " AND " : "").
                         implode(" AND ", $this->link_wheres);
            }
        else {
            $new_where = $this->wheres[$file_in_esame];
            }
        // ______________________________________________________________ Handle ASPID ___
        if ($file_in_esame != $this->file->indice &&
            $this->files[$file_in_esame]->asp == 'C' &&
            $this->files[$file_in_esame]->link_by_sql) {
            // _______________________________________________ Linked table with ASPID ___
            if (!$app->vars['_area']->valore) {
                if (strpos($this->link_on[$file_idx], "O2ASPID") === false) {
                    $this->link_on[$file_idx] = $full_tab_alias.".O2ASPID=".
                                                $c_o.$this->file->indice.$c_c.
                                                ".O2ASPID AND ".$this->link_on[$file_idx];
                    }
                }
            // ____________________________________________ Linked table without ASPID ___
            elseif (strpos($this->link_on[$file_idx], "O2ASPID") === false) {
                $this->link_on[$file_idx] = $full_tab_alias.".O2ASPID='".
                                           $app->vars['_area']->valore."' AND ".
                                           $this->link_on[$file_idx];
                }
            }


        /**
        * If WHERE clause has changed return TRUE else FALSE (link-on criteria are
        * checked too)
        */
        if (!isset($this->ultima_where[$file_in_esame][$record]) ||
            $this->ultima_where[$file_in_esame][$record] != $new_where.
                                                            $this->link_on[$file_idx]) {
            $this->ultima_where[$file_in_esame][$record] = $new_where.
                                                           $this->link_on[$file_idx];
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Check fields for join criteria
     *
     * @param  string        $file          Checked link file name
     * @param  o2_campo_task $field         Checked link field object
     * @param  array         $pk_segments   Missing PKey segments to be linked
     * @return array
     */
    function check_join_field($file, $field, $pk_segments) {

        $prg        = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
        $link_db    = $this->files[$file]->db;
        $file_type  = $link_db->server->type;
        $c_o        = constant("o2_".$file_type."_o");
        $c_c        = constant("o2_".$file_type."_c");
        $qlfd_field = $c_o.$file.$c_c.".".$field->nome_fisico;
        $exp        = $field->min;
        // _____________________________________________________ Expression evaluation ___
        $idx = count($prg->link_check);
        $prg->link_check[$idx] = true;
        $valore_local          = false;
        eval("\$valore_local = ".$exp.";");
        // _______________ Constant JOIN criteria (no o2val() involved but "_SESSION") ___
        if ($prg->link_check[$idx]) {
            $link_by_const = true;
            if ($field->tipo == "N") {
                if ($valore_local) {
                    $value = $qlfd_field."=".number_format($valore_local,
                                                           $field->maschera->decimali,
                                                           ".", "");
                    }
                else {
                    $value = "(".$qlfd_field."=0 OR ".$qlfd_field." IS NULL)";
                    }
                }
            elseif ($field->tipo == "L") {
                if ($valore_local) {
                    $value = $qlfd_field."='1'";
                    }
                else {
                    $value = "(".$qlfd_field."<'1' OR ".$qlfd_field." IS NULL)";
                    }
                }
            elseif ($field->tipo == "D") {
                if ($valore_local && $valore_local != "00000000") {
                    $value = $qlfd_field."='".$valore_local."'";
                    }
                else {
                    $value = "(".$qlfd_field."='00000000' OR ".$qlfd_field." IS NULL)";
                    }
                }
            elseif ($field->tipo == "O") {
                if ($valore_local && $valore_local != "000000") {
                    $value = $qlfd_field."='".$valore_local."'";
                    }
                else {
                    $value = "(".$qlfd_field."='000000' OR ".$qlfd_field." IS NULL)";
                    }
                }
            elseif ($valore_local !== "") {
                $value = $qlfd_field."='".
                         o2_gateway::normalize($file_type, $valore_local)."'";
                }
            else {
                $value = "(RTRIM(".$qlfd_field.")='' OR RTRIM(".$qlfd_field.") IS NULL)";
                }
            }
        else {
            $link_by_const = false;
            $linkfld       = strtoupper($prg->link_field[$idx]);
            $linkview      = $prg->link_view[$idx];
            $linkfile      = $this->campi[$linkfld]->file;
            $value         = $qlfd_field."=".$c_o.$this->campi[$linkfld]->file.$c_c.".".
                             $this->campi[$linkfld]->nome_fisico;
            }
        unset($prg->link_field[$idx]);
        unset($prg->link_view[$idx]);
        unset($prg->link_check[$idx]);
        /* ===============================================================================
         * Check conditions to identify link criteria:
         *  - link criteria is a constant (or session variable) or:
         *     - a link field is identified and
         *        - field is not a formula
         *        - field belongs to this view
         *        - field file is main-table or it is NOT excluded from join
         *        - link value is equal to field value (no modification is made in
         *          expression)
         */
        $file_idx = o2_gateway::qualify($file_type,
                                        $link_db->nome,
                                        $link_db->proprietario,
                                        $this->files[$file]->nome)." ".$c_o.$file.$c_c;
        if ($link_by_const ||
            ($linkfld && !isset($this->formule[$linkfld]) &&
             $linkview == $this->nome &&
             ($linkfile == $this->file || !$this->files[$linkfile]->not_sql_join) &&
             $valore_local === o2val($linkview, $linkfld))) {
            if (strpos($this->link_on[$file_idx], $qlfd_field) === false) {
                $and = ($this->link_on[$file_idx] ? " AND " : "");
                $this->files[$file]->link_by_sql = true;
                $this->link_on[$file_idx].= $and.$value;
                }
            // _________________________________________________ Check out PK-segments ___
            if (isset($pk_segments[$field->nome_fisico])) {
                unset($pk_segments[$field->nome_fisico]);
                }
            }
        else {
            $this->files[$file]->not_sql_join = true;
            unset($this->link_on[$file_idx]);
            unset($this->link_wheres[$file]);
            }
        return $pk_segments;

        }


    /**
     * Ricalcola le condizioni per le formule (campi calcolati)
     *
     * @param  integer $record
     * @return boolean
     */
    function range2formule($record = 0) {

        $cond_totale      = "";
        $app              = $_SESSION['o2_app'];
        $evaluating_local = $app->istanze_prg[$this->id_esecuzione]->evaluating_view;
        $app->istanze_prg[$this->id_esecuzione]->evaluating_view = $this->nome;
        foreach ($this->formule as &$formula) {
            $where_local  = array();
            $valore_local = null;
            // ______________________________________________ Exact value (MIN == MAX) ___
            if (!is_null($formula->min) && ($formula->min == $formula->max)) {
                eval("\$valore_local = ".$formula->min.";");
                if (!is_null($valore_local)) {
                    $where_local[] = "('§§o2§§' == '".addcslashes($valore_local, "\'").
                                     "')";
                    }
                }
            else {
                // _______________________________________________ MINIMUM range value ___
                if (!is_null($formula->min)) {
                    eval("\$valore_local = ".$formula->min.";");
                    if (!is_null($valore_local)) {
                        $where_local[] = "('§§o2§§' >= '".
                                        addcslashes($valore_local, "\'")."')";
                        }
                    }
                // _______________________________________________ MAXIMUM range value ___
                if (!is_null($formula->max)) {
                    eval("\$valore_local = ".$formula->max.";");
                    if (!is_null($valore_local)) {
                        $where_local[] = "('§§o2§§' <= '".
                                         addcslashes($valore_local, "\'")."')";
                        }
                    }
                }
            // _______________________________________________________ NOT range value ___
            if (!is_null($formula->not)) {
                eval("\$valore_local = ".$formula->not.";");
                if (!is_null($valore_local)) {
                    $where_local[] = "('§§o2§§' != '".addcslashes($valore_local, "\'").
                                     "')";
                    }
                }
            // ______________________________________________________ LIKE range value ___
            if (!is_null($formula->like)) {
                eval("\$valore_local = ".$formula->like.";");
                if (!is_null($valore_local)) {
                    $where_local[] = "(stripos('§§o2§§', '".
                                     addcslashes($valore_local, "\'")."') !== false)";
                    }
                }
            // _______________________________________________________ Whole condition ___
            if (count($where_local)) {
                $this->formule[$formula->nome]->condizione = implode(" && ",
                                                                     $where_local);
                $cond_totale.= $this->formule[$formula->nome]->condizione;
                }
            else {
                $this->formule[$formula->nome]->condizione = "";
                }
            }
        $app->istanze_prg[$this->id_esecuzione]->evaluating_view = $evaluating_local;
        // ______________________________________________ Check for changed conditions ___
        if ($cond_totale || isset($this->wheres["o2calc§_§exps"])) {
            $this->wheres["o2calc§_§exps"] = $cond_totale;
            if (!isset($this->ultima_where["o2calc§_§exps"][$record]) ||
                $this->wheres["o2calc§_§exps"] !=
                $this->ultima_where["o2calc§_§exps"][$record]) {
                $this->ultima_where["o2calc§_§exps"][$record] =
                $this->wheres["o2calc§_§exps"];
                return true;
                }
            else {
                return false;
                }
            }
        else {
            return false;
            }

        }


    /**
     * Check if ranges are chenged for the view.
     * Ranges are checked in main file, linked files and formulas.
     *
     * @return boolean
     */
    function range_mod() {

        if ($this->vista) {
            // __________________________________ If range criteria changed for a file ___
            foreach ($this->files as $single_file) {
                $file_name = $single_file->indice;
                // _________________________________________________________ Link file ___
                if ($file_name != $this->file->indice) {
                    if ($this->range2where($file_name, $this->selezione)) {
                        return true;
                        }
                    }
                // _________________________________________________________ Main file ___
                else {
                    if ($this->range2where($file_name)) {
                        return true;
                        }
                    }
                }
            // ______________________________________________ Recalc range on formulas ___
            if ($this->range2formule()) {
                return true;
                }
            return false;
            }
        else {
            return true;
            }

        }


    /**
     * Crea un recordset per la vista
     * If $lock parameter is passed as TRUE then a lock is acquired for each returned row
     *
     *
     * @param boolean $preserve_orderby   If order by clausole must be preserved or
     *                                    recreated. TRUE is used by locate() method.
     * @param boolean $lock               Lock returned rows
     */
    function crea_recordset($preserve_orderby = false, $lock = false) {

        $app                 = $_SESSION['o2_app'];
        $this->computing     = true;
        $this->chunk         = 1;
        $this->recordset     = array();
        $this->corrente      = array();
        $this->wheres        = array();
        $this->ultima_where  = array();
        $this->verified      = array();
        $this->link_cache    = array();
        $this->link_wheres   = array();
        $this->comunica_variazioni();
        $this->dipendenze = array();
        $mainfile         = $this->file->indice;
        $file_type        = $this->file->db->server->type;
        $c_o              = constant("o2_".$file_type."_o");
        $c_c              = constant("o2_".$file_type."_c");
        // _______________________________________ Where clause from range expressions ___
        foreach ($this->files as $single_file) {
            $this->range2where($single_file->indice);
            }
        // __________________________________________________ Recalc range on formulas ___
        $this->range2formule();
        // ____________________________ Reset links cache (save mainfile and formulas) ___
        if (isset($this->ultima_where['o2calc§_§exps'])) {
            $this->ultima_where = array($mainfile       => $this->ultima_where[$mainfile],
                                        'o2calc§_§exps' =>
                                         $this->ultima_where['o2calc§_§exps']);
            }
        else {
            $this->ultima_where = array($mainfile => $this->ultima_where[$mainfile]);
            }
        // _____________________________________ Custom FROM (table name or SQL query) ___
        if ($this->custom_from) {
            $this->struttura();
            }
        // ____________________________________________________ View key by expression ___
        elseif ($this->chiavi_exps[$mainfile]) {
            $key_by_exp = $this->chiavi_exps[$mainfile]();
            if ($key_by_exp != $this->chiavi_in_uso[$mainfile]) {
                $this->struttura();
                }
            }
        // ________________________________________ Auto aggregation functions on view ___
        if ($this->auto_aggr) {
            $this->calcola_aggregate();
            }
        $this->fine           = false;
        $selezione_originaria = $this->selezione;
        $ritornati_local      = 0;
        $key_for_prev         = "";
        $key_for_next         = "";
        $key_for_same         = "";
        $start_local          = true;
        // _______________________________________________ Main table ORDER BY clauses ___
        if (!$this->indici[$mainfile] || !$preserve_orderby) {
            $used_key                        = $this->chiavi_in_uso[$mainfile];
            $this->indici[$mainfile]         = $this->order_by($mainfile, $used_key);
            $this->indici_inversi[$mainfile] = $this->order_by($mainfile, $used_key,
                                                               true);
            }
        foreach ($this->files as $linkidx => $linktab) {
            // _____________________________ Compose WHERE clause for SQL-linked table ___
            $this->range2where($linkidx);
            }
        // __________________________ Add SQL-linked table WHERE clauses to main table ___
        if ($this->link_wheres) {
            $this->wheres[$mainfile].= ($this->wheres[$mainfile] ? " AND " : "").
                                       implode(" AND ", $this->link_wheres);
            $this->ultima_where[$mainfile][0] = $this->wheres[$mainfile];
            }
        // _____________________________________________ Loop for recordset retrieving ___
        do {
            // ____________________________ Only for the very first query sets offsets ___
            if ($start_local) {
                switch ($this->direzione) {
                    case 0:
                        $this->offset_ini  = 0;
                        $this->offset_fine = 0;
                        break;
                    case 1:
                        $this->offset_fine = ($this->offset_ini ?
                                              $this->offset_ini - 1 : 0);
                        $this->offset_ini  = $this->offset_fine;
                        break;
                    case 2:
                        $this->offset_ini  = ($this->offset_fine ?
                                              $this->offset_fine + 1 : 0);
                        $this->offset_fine = $this->offset_ini;
                        break;
                    case 3:
                        $this->offset_ini  = 0;
                        $this->offset_fine = $this->totale_record - 1;
                        break;
                    }
                }
            $recordset_local = $this->get_recordset($lock);
            if ($this->direzione == 0) {
                $recordset_local = array();
                $this->recordset = array();
                }
            elseif (!is_array($recordset_local)) {
                $recordset_local = array();
                }
            $ritornati_local    = count($recordset_local);
            $this->vista        = true;
            $recordcount_local  = 0;
            $record_block_local = array();
            $mancanti_local     = $this->righe_vis - count($this->recordset);
            $forward            = $this->direzione === 0 || $this->direzione === 2;
            // ============== GESTIONE DELLE LINK E VERIFICA DEL RECORD ==================
            foreach ($recordset_local as $indice_local => $singolo_record) {
                // _____________________________________________ If forward retrieving ___
                if ($forward) {
                    // _________________________ Set checking record as current record ___
                    $this->selezione = (count($this->recordset) + $recordcount_local);
                    }
                // ____________________________________________ If backward retrieving ___
                else {
                    // _________________________ Set checking record as current record ___
                    $this->selezione = $mancanti_local - $recordcount_local - 1;
                    }
                $this->imposta_corrente($singolo_record);
                // ____________________ Only for the very first query sets key-filters ___
                if ($start_local) {
                    $start_local = false;
                    // _________________________________________ If forward retrieving ___
                    if ($forward) {
                        $this->key_filter_same = "((".
                                                 $this->chiave_corrente($this->file,
                                                                        false,
                                                                        true).
                                                 ") OR (".
                                                 $this->chiave_corrente($this->file).
                                                 "))";
                        $this->key_filter_prev = $this->chiave_corrente($this->file,
                                                                        false,
                                                                        true,
                                                                        true);
                        }
                    // ________________________________________ If backward retrieving ___
                    else {
                        $this->key_filter_next = $this->chiave_corrente($this->file,
                                                                        false,
                                                                        true);
                        }
                    }
                // ____________________ Verify formulas and links conditions on record ___
                if ($this->verifica_record($this->selezione)) {
                    // __________________________________ If passed add record to view ___
                    $record_block_local[$this->selezione] = $this->corrente;
                    $recordcount_local++;
                    // ________________________ If requested records number is reached ___
                    if ($recordcount_local == $mancanti_local) {
                        $this->fine = true;
                        // _____________________________________ If forward retrieving ___
                        if ($forward) {
                            if ($ritornati_local > $recordcount_local) {
                                $this->eof = false;
                                }
                            }
                        break;
                        }
                    }
                }
            // _________________________________________________ If forward retrieving ___
            if ($forward) {
                $this->offset_fine     = $this->offset_fine + $indice_local;
                $this->totale_record   = max($this->totale_record, $this->offset_fine +5);
                $this->key_filter_next = $this->chiave_corrente($this->file, false, true);
                }
            // ________________________________________________ If backward retrieving ___
            else {
                $start_local = false;
                if ($this->direzione != 3) {
                    $this->offset_ini = max(0, $this->offset_ini - $indice_local);
                    }
                $this->key_filter_same = "((".
                                         $this->chiave_corrente($this->file,
                                                                false,
                                                                true).
                                         ") OR (".
                                         $this->chiave_corrente($this->file).
                                         "))";
                $this->key_filter_prev = $this->chiave_corrente($this->file,
                                                                false,
                                                                true,
                                                                true);
                }
            // __________________________ If at last one record has been added to view ___
            if ($recordcount_local) {
                $this->recordset = array_merge($this->recordset, $record_block_local);
                }
            // ____________________________________________________ Got the REAL count ___
            if ($this->eof) {
                // _______________________________________ No records within selection ___
                if (!$this->offset_fine) {
                    $this->fine = true;
                    }
                if ($forward) {
                    $this->totale_record = max($this->offset_fine + 1,
                                               count($this->recordset));
                    }
                }
            else {
                $this->totale_record = max($this->totale_record, $this->offset_fine + 1);
                }
            // _________________________________________________ Dynamic chunk setting ___
            $this->chunk = max($ritornati_local / max($recordcount_local, 1), 1);
            } while (count($this->recordset) < $this->righe_vis && !$this->fine);
        // ____________________________________________________ If backward retrieving ___
        if (!$forward) {
            $this->recordset = array_reverse($this->recordset);
            }
        $tot = count($this->recordset);
        if ($tot < $this->righe_vis) {
            // ___________________________________________ No records within selection ___
            if (($this->offset_ini + $tot) < 1) {
                $this->recordset     = array();
                $this->totale_record = 0;
                $this->direzione     = 100;
                $this->seleziona_riga();
                $this->computing     = false;
                $this->vista         = true;
                return true;
                }
            $this->totale_record = $this->offset_ini + $tot;
            }
        elseif ($forward) {
            $this->offset_fine = min($this->offset_fine + 1,
                                     $this->offset_ini + $tot - 1);
            }
        else {
            $this->offset_ini = max($this->offset_ini - 1, $this->offset_fine - $tot + 1);
            }
        // === COUNT PROBLEM! ============================================================
        if ($this->offset_fine > $this->totale_record) {
            $this->totale_record = $this->offset_fine - 1;
            }
        // ===============================================================================
        $this->direzione = 100;
        $this->seleziona_riga($selezione_originaria);
        $this->computing = false;
        $this->vista     = true;
        }


    /**
     * Ritorna il numero totale di record per il main file della vista.
     * Questo metodo equivale alla COUNT di database per il main file della vista: il suo
     * risultato è corretto per la vista solo se non sono presenti filtri su LINK NON
     * RISOLTE IN JOIN o su formule.
     *
     * @return integer
     */
    function totale_rec() {

        $file     = $this->file;
        $db       = $file->db;
        $server   = $db->server;
        $mainfile = $file->indice;
        $links    = null;
        $this->range2where($this->file->indice);
        foreach ($this->files as $linkidx => $linktab) {
            if ($linktab->link_by_sql) {
                // ________________________ Compose WHERE clause for SQL-linked tables ___
                $this->range2where($linkidx);
                }
            }
        // __________________________ Add SQL-linked table WHERE clauses to main table ___
        if ($this->link_wheres) {
            $this->wheres[$mainfile].= ($this->wheres[$mainfile] ? " AND " : "").
                                        implode(" AND ", $this->link_wheres);
            $this->ultima_where[$mainfile][0] = $this->wheres[$mainfile];
            }
        $links = (is_array($this->link_on) ? $this->link_on : null);
        return o2_gateway::count($server->type,
                                 $server->server,
                                 $server->user,
                                 $server->password,
                                 $db->nome,
                                 $db->proprietario,
                                 $file->nome,
                                 $mainfile,
                                 (isset($this->wheres[$mainfile]) ?
                                  $this->wheres[$mainfile] :
                                  ""),
                                 $links);

        }


    /**
     * Ritorna il numero totale di record per viste con filtri su LINK e/o formule.
     * Questo metodo esegue un loop di conteggio sull'intero recordset della vista e il
     * risultato è corretto per qualunque tipo di vista.
     *
     * @return integer
     */
    function count_loop() {

        $counter   = 0;
        // _____________________________________ Copy view properties to restore later ___
        $view_copy = array();
        foreach (get_object_vars($this) as $prop_id => $single_prop) {
            if (is_object($single_prop)) {
                $view_copy[$prop_id] = clone $single_prop;
                }
            else {
                $view_copy[$prop_id] = $single_prop;
                }
            }
        $this->righe_vis  = $this->file->batch_chunk;
        $this->suffisso   = false;
        $this->prefisso   = false;
        // _____________________________________________________________ Count records ___
        $this->looping_on = true;
        $this->record_primo();
        if (count($this->recordset)) {
            do {
                $counter++;
                } while ($this->record_avanti());
            }
        $this->looping_on = false;
        // ___________________________________________________ Restore view properties ___
        foreach (get_object_vars($this) as $prop_id => $single_prop) {
            $this->$prop_id = $view_copy[$prop_id];
            }
        if (isset($this->ultima_where["o2calc§_§exps"])) {
            $this->vista     = false;
            $this->direzione = 0;
            }
        return $counter;

        }


    /**
     * Estrae e restituisce il recorset del main file da un database
     *
     * @return array
     */
    function get_recordset($lock = false) {

        $ret_val    = array();
        $file_name  = $this->file->indice;
        $select_str = ($this->select_str[$file_name] ?
                       $this->select_str[$file_name] :
                       "*");
        $fetch_loop = $_SESSION['o2_app']->fetch_loop;
        // ______________________________________________________ Merge SELECT strings ___
        foreach ($this->files as $linkidx => $linktab) {
            if ($linkidx != $this->file->indice && $linktab->link_by_sql) {
                $select_str.= ",".$this->select_str[$linkidx];
                }
            }
        if (!$this->indici[$file_name]) {
            $used_key                         = $this->chiavi_in_uso[$file_name];
            $this->indici[$file_name]         = $this->order_by($file_name, $used_key);
            $this->indici_inversi[$file_name] = $this->order_by($file_name, $used_key,
                                                                true);
            }
        // ==================== Sistema ottimizzato usando righe_vis come chunk minimo ===
        $quanti = intval($this->righe_vis * $this->chunk) +
                  ($fetch_loop && $this->looping_on ? 0 : 1);
        // -------------------------------------------------------------------------------
        $where_exp = (isset($this->wheres[$file_name]) ? $this->wheres[$file_name] : "");
        // __________________________________________________________________ SQL-LINK ___
        $links     = (is_array($this->link_on) ? $this->link_on : null);
        // ______________________________________ View unique ID to refetch statements ___
        $view_id   = $this->nome.$this->id_esecuzione;
        // ___________________________________________________________ Compose dataset ___
        switch ($this->direzione) {
            // _____________________________________________________________ First set ___
            case 0:
                $ret_val = o2_gateway::recordset($this->file->db->server->type,
                                                 $this->file->db->server->server,
                                                 $this->file->db->server->user,
                                                 $this->file->db->server->password,
                                                 $this->file->db->nome,
                                                 $this->file->db->proprietario,
                                                 $this->file->nome,
                                                 $file_name,
                                                 $select_str,
                                                 $where_exp,
                                                 $this->indici[$file_name],
                                                 $quanti,
                                                 $links,
                                                 $lock,
                                                 false,
                                                 ($fetch_loop && $this->looping_on ?
                                                  $view_id : false));

                $ret_num               = count($ret_val);
                $this->bof             = true;
                $this->eof             = false;
                $this->key_filter_prev = "";
                $this->key_filter_same = "";
                $this->offset_ini      = 0;
                $this->offset_fine     = 0;
                $this->relocated       = true;
                if ($ret_num < $quanti) {
                    $this->fine = true;
                    $this->eof  = true;
                    }
//      print "<h2>|< I:".$this->offset_ini." F:".$this->offset_fine."</h2>\n";
                $this->direzione = 2;
                break;
            // __________________________________________________________ Previous set ___
            case 1:
                $page_filter = false;
                if ($this->key_filter_prev) {
                    $page_filter = $this->key_filter_prev.($where_exp ? ' AND ' : '');
                    }
                $ret_val = o2_gateway::recordset($this->file->db->server->type,
                                                 $this->file->db->server->server,
                                                 $this->file->db->server->user,
                                                 $this->file->db->server->password,
                                                 $this->file->db->nome,
                                                 $this->file->db->proprietario,
                                                 $this->file->nome,
                                                 $file_name,
                                                 $select_str,
                                                 $where_exp,
                                                 $this->indici_inversi[$file_name],
                                                 $quanti,
                                                 $links,
                                                 $lock,
                                                 $page_filter,
                                                 ($fetch_loop && $this->looping_on ?
                                                  $view_id : false));

                $ret_num = count($ret_val);
                if ($ret_num < $quanti) {
                    $this->direzione    = 0;
                    $this->recordset    = array();
                    $this->link_cache   = array();
                    $newlist            = array();
                    foreach ($this->ultima_where as $uw_key => $uw_value) {
                        if (isset($uw_value[-1])) {
                            $newlist[$uw_key][-1] = $uw_value[-1];
                            }
                        }
                    $this->ultima_where = $newlist;
                    return $this->get_recordset($lock);
                    }
                else {
                    $this->bof = false;
                    }
// o2log("I:".$this->offset_ini." F:".$this->offset_fine);
                $this->relocated = true;
                $this->direzione = 1;
                break;
            // ______________________________________________________________ Next set ___
            case 2:
                $page_filter = false;
                if ($this->key_filter_next) {
                    $page_filter = $this->key_filter_next.($where_exp ? ' AND ' : '');
                    }
                $ret_val = o2_gateway::recordset($this->file->db->server->type,
                                                 $this->file->db->server->server,
                                                 $this->file->db->server->user,
                                                 $this->file->db->server->password,
                                                 $this->file->db->nome,
                                                 $this->file->db->proprietario,
                                                 $this->file->nome,
                                                 $file_name,
                                                 $select_str,
                                                 $where_exp,
                                                 $this->indici[$file_name],
                                                 $quanti,
                                                 $links,
                                                 $lock,
                                                 $page_filter,
                                                 ($fetch_loop && $this->looping_on ?
                                                  $view_id : false));
                $ret_num           = count($ret_val);
                $this->bof         = false;
                if ($ret_num < $quanti) {
                    $this->fine = true;
                    $this->eof  = true;
                    }
                else {
                    $this->eof  = false;
                    }
// o2log("I:".$this->offset_ini." F:".$this->offset_fine);
                $this->relocated = true;
                $this->direzione = 2;
                break;
            // ______________________________________________________________ Last set ___
            case 3:
                $ret_val = o2_gateway::recordset($this->file->db->server->type,
                                                 $this->file->db->server->server,
                                                 $this->file->db->server->user,
                                                 $this->file->db->server->password,
                                                 $this->file->db->nome,
                                                 $this->file->db->proprietario,
                                                 $this->file->nome,
                                                 $file_name,
                                                 $select_str,
                                                 $where_exp,
                                                 $this->indici_inversi[$file_name],
                                                 $quanti,
                                                 $links,
                                                 $lock,
                                                 false,
                                                 ($fetch_loop && $this->looping_on ?
                                                  $view_id : false));
                $ret_num               = count($ret_val);
                $this->eof             = true;
                $this->bof             = false;
                $this->key_filter_next = "";
                if ($ret_num < $quanti) {
                    $this->direzione    = 0;
                    $this->recordset    = array();
                    $this->link_cache   = array();
                    $newlist            = array();
                    foreach ($this->ultima_where as $uw_key => $uw_value) {
                        if (isset($uw_value[-1])) {
                            $newlist[$uw_key][-1] = $uw_value[-1];
                            }
                        }
                    $this->ultima_where = $newlist;
                    return $this->get_recordset($lock);
                    }
                $this->offset_fine = $this->totale_record - 1;
                $this->offset_ini  = $this->offset_fine - $quanti;
                $this->relocated   = true;
 // o2log("I:".$this->offset_ini." F:".$this->offset_fine);
                $this->direzione = 1;
                break;
            // ______________________________________________________________ Same set ___
            default:
                if ($this->key_filter_same) {
                    $where_exp = $this->key_filter_same.
                                 ($where_exp ? " AND ".$where_exp : "");
                    }
                $ret_val = o2_gateway::recordset($this->file->db->server->type,
                                                 $this->file->db->server->server,
                                                 $this->file->db->server->user,
                                                 $this->file->db->server->password,
                                                 $this->file->db->nome,
                                                 $this->file->db->proprietario,
                                                 $this->file->nome,
                                                 $file_name,
                                                 $select_str,
                                                 $where_exp,
                                                 $this->indici[$file_name],
                                                 $quanti,
                                                 $links,
                                                 $lock,
                                                 false,
                                                 ($fetch_loop && $this->looping_on ?
                                                  $view_id : false));
                $ret_num           = count($ret_val);
                $this->bof         = false;
                $this->offset_ini  = $this->totale_record - $ret_num;
                $this->offset_fine = $this->offset_ini + $ret_num;
                if ($ret_num < $this->righe_vis) {
                    $this->fine = true;
                    $this->eof  = true;
                    }
                $this->direzione = 2;
                $this->relocated = "refresh";
// o2log("I:".$this->offset_ini." F:".$this->offset_fine);
                break;
            }
    return $ret_val;

    }


    /**
     * Verifica le condizioni, aggiunge i campi delle link per il record corrente e
     * restituisce TRUE se corrente deve essere inserito nella vista
     *
     * @return boolean
     */
    function verifica_record($record = 0) {

        $transazione_local = $this->computing;
        $this->computing   = true;
        // ____________________________________________________ Loop on standard links ___
        foreach ($this->files as &$singolo_file) {
            $ret_val   = false;
            $file_name = $singolo_file->indice;
            // ______________________________ Skip main table and tables linked by SQL ___
            if ($file_name != $this->file->indice &&
                (!$singolo_file->link_by_sql ||
                 $this->status == "I" ||
                 $this->modificato)) {
                // _____________________________ Custom FROM (table name or SQL query) ___
                if ($this->custom_from) {
                    $this->struttura();
                    }
                // ____________________________________________ File key by expression ___
                elseif ($this->chiavi_exps[$file_name]) {
                    $key_by_exp = $this->chiavi_exps[$file_name]();
                    if ($key_by_exp != $this->chiavi_in_uso[$file_name]) {
                        $this->struttura();
                        }
                    }
                if (!$this->indici[$file_name]) {
                    $this->indici[$file_name] = $this->order_by($file_name,
                                                        $this->chiavi_in_uso[$file_name]);
                    }
                $this->calcola_formule();
                // ___________________________________________ Ricalcolo le condizioni ___
                $cond_local = $this->range2where($file_name, $record);
                $this->range2formule($record);
                // Se le condizioni di WHERE sono variate e sono diverse da quelle
                // dell'ultima query eseguita allora eseguo di nuovo la query
                if ($cond_local &&
                    ($this->link_cache[$file_name."_where"] !==
                     $this->wheres[$file_name])) {
                    // ______________________________________ Composizione della query ___
                    $ret_val = o2_gateway::verifyrec($singolo_file->db->server->type,
                                                     $singolo_file->db->server->server,
                                                     $singolo_file->db->server->user,
                                                     $singolo_file->db->server->password,
                                                     $singolo_file->db->nome,
                                                     $singolo_file->db->proprietario,
                                                     $singolo_file->nome,
                                                     $singolo_file->indice,
                                                     $this->select_str[$file_name],
                                                     $this->wheres[$file_name],
                                                     $this->indici[$file_name]);
                    $this->link_cache[$file_name."_where"] = $this->wheres[$file_name];
                    if ($ret_val === false) { // ________________________ Link fallita ___
                        if ($this->esclusivi[$file_name][0]) { // _____ Link esclusiva ___
                            $this->computing              = $transazione_local;
                            $this->link_cache[$file_name] = false;
                            return false;
                            }
                        else { // _________________________________ Link non esclusiva ___
                            $ret_val = array();
                            }
                        }
                    $this->link_cache[$file_name] = $ret_val;
                    }
                elseif ($this->link_cache[$file_name."_where"] ===
                        $this->wheres[$file_name]) {
                    if ($this->link_cache[$file_name] !== false) {
                        $ret_val = $this->link_cache[$file_name];
                        }
                    else {
                        return false;
                        }
                    }
                if ($ret_val !== false) {
                    if (is_array($this->corrente)) {
                        foreach ($this->corrente as $singolo_campo => $singolo_valore)  {
                            if (isset($this->campi[$singolo_campo]) &&
                                $this->campi[$singolo_campo]->file == $file_name) {
                                unset($this->corrente[$singolo_campo]);
                                }
                            }
                        }
                    else {
                        $this->corrente = array();
                        }
                    $this->corrente+= $ret_val;
                    }
                }
            }
        $this->range2formule($record);
        $this->calcola_formule();
        // ______________________________________ Verifico le condizioni sulle formule ___
        foreach ($this->formule as &$singola_formula) {
            if (trim($singola_formula->condizione) != '') {
                // ATTENZIONE: nella composizione delle stringhe di 'condizione'
                // l'espressione '§§o2§§' è un segnaposto per il valore della variabile
                // che viene sostituito prima di valutare la condizione
                $f_value    = addslashes($this->campo($singola_formula->nome));
                $condizione = false;
                eval("\$condizione = ".str_replace("§§o2§§",
                                                   $f_value,
                                                   $singola_formula->condizione).";");
                if (!$condizione) {
                    $this->computing = $transazione_local;
                    return false;
                    }
                }
            }
        // ______________________ Se tutte le condizioni sono soddisfatte ritorno TRUE ___
        $this->computing = $transazione_local;
        return true;

        }


    /**
     * Imposta i valori dell'array ->corrente ai correnti valori dei campi calcolati
     *
     */
    function calcola_formule() {

        $app = $_SESSION['o2_app'];
        foreach ($this->formule as &$singola_formula) {
            $app->defining_check = null;
            $espressione_local   = false;
            // _________ To be used inside expression: used for progress-bar record ID ___
            $thisview            = $this;
            eval("\$espressione_local = ".$singola_formula->formula.";");
            if ($app->defining_check === $this->nome) {
                $singola_formula->selector = true;
                }
            $app->defining_check                    = null;
            $this->corrente[$singola_formula->nome] = $espressione_local;
            }
        unset ($thisview);

        }


    /**
     * Ritorna vero se è variata una delle viste da cui questa dipende
     *
     * @return boolean
     */
    function variato($file2verify = "") {

        $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
        if (array_key_exists($this->nome, $prg->variazioni)) {
            if (!$file2verify) {
                unset($prg->variazioni[$this->nome]);
                return true;
                }
            elseif (array_key_exists($file2verify, $prg->variazioni[$this->nome])) {
                unset($prg->variazioni[$this->nome][$file2verify]);
                return true;
                }
            else {
                return false;
                }
            }
        else {
            return false;
            }

        }


    /**
     * Imposta la riga [$riga] (con base 0) del recordset come record corrente del task
     *
     * @param integer $riga
     */
    function seleziona_riga($riga = 0) {

        $recs = count($this->recordset);
        if ($recs) {
            $this->selezione  = max(0, min($riga, $recs - 1));
            $this->corrente   = &$this->recordset[$this->selezione];
            $this->precedente = $this->corrente;
            $this->modificato = false;
            $this->record_prefix();
            }
        else {
            $this->selezione  = 0;
            $this->corrente   = array();
            $this->modificato = false;
            }
        $this->comunica_variazioni();

        }


    /**
     * Executes record prefix for view, if any
     *
     * @return boolean
     */
    function record_prefix() {

        $app         = $_SESSION['o2_app'];
        $ret_val     = true;
        $return_save = $app->ritorno;
        if ($this->prefisso && $this->prefisso_exe) {
            $insert_point = false;
            if ($this->id_esecuzione < count($app->istanze_prg)) {
                // __________________ New execution is inserted at the right prg level ___
                foreach ($app->esecuzioni as $exe_id => $single_exe) {
                    if ($single_exe->istanza_prg < $this->id_esecuzione) {
                        array_splice($app->esecuzioni,
                                     $exe_id,
                                     0,
                                     array(new o2_esecuzione($this->prg,
                                                             $this->id_esecuzione,
                                                             $this->prefisso,
                                                             "")));
                        $insert_point = $exe_id;
                        break;
                        }
                    }
                if (!$insert_point) {
                    $app->esecuzioni[] = new o2_esecuzione($this->prg,
                                                           $this->id_esecuzione,
                                                           $this->prefisso,
                                                           "");
                    }
                $ret_val = false;
                }
            else {
                $ret_val = $app->istanze_prg[$this->id_esecuzione]->
                            esegui_azione($this->prefisso, true);
                }
            }
        if ($ret_val) {
            $app->ritorno = $return_save;
            }
        return $ret_val;

        }


    /**
     * Executes action on row and updates
     *
     * @return boolean
     */
    function record_suffix() {

        $app = $_SESSION['o2_app'];
        if ($app->block_exe) {
            return true;
            }
        $ret_local = true;
        if ($this->modificato && $this->suffisso && !$this->suffix_running) {
            $prg_local = $app->istanze_prg[$this->id_esecuzione];
            if (!$this->suffix_waiting) {
                $this->suffix_running = true;
                $this->suffix_waiting = true;
                $exe_count            = count($app->esecuzioni);
                // _________________________________________ If suffix stops (on-line) ___
                if (!$prg_local->esegui_azione($this->suffisso, true)) {
                    /*
                     * If more than one action are added to execution list
                     * ($app->esecuzioni) we must get the very first action: that one is
                     * the real suffix, not its sub-actions.
                     */
                    $exe_count = (count($app->esecuzioni) - $exe_count - 1);
                    $ret_local = false;
                    $app->esecuzioni[$exe_count]->suffix_of = $this->nome;
                    }
                }
            }
        if ($ret_local) {
            if ($this->status == "I") {
                $this->direzione = 100;
                $this->crea_recordset();
                }
            $this->status         = "M";
            $this->modificato     = false;
            $this->corrente       = $this->precedente;
            $this->suffix_running = false;
            $this->suffix_waiting = false;
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Imposta il puntatore di selezione alla riga successiva del recordset oppure, se
     * siamo sull'ultimo record del recordset, alla prima riga del set di record
     * successivo. Ritorna FALSE se il record corrente è l'ultimo del recordset
     *
     * @return boolean
     */
    function record_avanti() {

        $ret_val = false;
        if ($this->record_suffix()) {
            if ($this->selezione < (count($this->recordset) - 1)) {
                $this->seleziona_riga($this->selezione + 1);
                $this->relocate_to_key = false;
                $ret_val               = true;
                }
            elseif (!$this->eof) {
                $this->relocate_to_key = false;
                $this->prefisso_exe    = false;
                $this->direzione       = 2;
                $this->crea_recordset();
                $this->prefisso_exe    = true;
                $this->seleziona_riga();
                $this->relocate_to_key = false;
                if (count($this->recordset)) {
                    $ret_val = true;
                    }
                }
            }
        return $ret_val;

        }


    /**
     * Imposta il puntatore di selezione alla riga precedente del recordset oppure, se
     * siamo sul primo record del recordset, all'ultima riga del set di record precedente.
     * Ritorna FALSE se il record corrente è il primo del recordset
     *
     * @return boolean
     */
    function record_indietro() {

        $chiave_local = $this->chiave_corrente($this->file, true);
        if ($this->record_suffix()) {
            if ($this->selezione > 0) {
                $this->seleziona_riga($this->selezione - 1);
                $this->relocate_to_key = false;
                $this->bof             = false;
                return true;
                }
            elseif (!$this->bof) {
                $this->relocate_to_key = false;
                $this->prefisso_exe    = false;
                $this->direzione       = 1;
                $this->crea_recordset();
                $this->prefisso_exe    = true;
                $this->seleziona_riga(count($this->recordset) - 1);
                $this->relocate_to_key = false;
                return true;
                }
            else {
                return false;
                }
            }
        else {
            return false;
            }

        }


    /**
     * Imposta il puntatore di selezione alla riga corrente del set di record precedente
     *
     */
    function record_pg_indietro() {

        if ($this->record_suffix()) {
            $this->relocate_to_key = false;
            $this->direzione       = 1;
            $this->crea_recordset();
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Imposta il puntatore di selezione alla riga corrente del set di record successivo
     *
     */
    function record_pg_avanti() {

        if ($this->record_suffix()) {
            $this->relocate_to_key = false;
            $this->direzione       = 2;
            $this->crea_recordset();
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Imposta il puntatore di selezione alla prima riga del primo set di record
     * If $lock parameter is passed as TRUE then a lock is acquired for each returned row
     *
     * @param boolean $lock
     */
    function record_primo($lock = false) {

        if ($this->record_suffix()) {
            $this->relocate_to_key = false;
            $this->prefisso_exe    = false;
            $this->direzione       = 0;
            $this->crea_recordset(false, $lock);
            $this->prefisso_exe    = true;
            $this->seleziona_riga();
            return true;
            }
        else {
            return false;
            }
        }


    /**
     * Imposta il puntatore di selezione all'ultima riga dell'ultimo set di record
     *
     */
    function record_ultimo() {

        if ($this->record_suffix()) {
            $this->relocate_to_key = false;
            $this->prefisso_exe    = false;
            $this->direzione       = 3;
            $this->crea_recordset();
            $this->prefisso_exe    = true;
            $this->seleziona_riga(count($this->recordset) - 1);
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Refreshes view content, mantaining data page.
     * If current record is not present in refreshed dataset then sends view to first
     * record.
     *
     */
    function smart_refresh() {

        if ($this->record_suffix() && !$this->relocate_to_key) {
            $old_key         = $this->chiave_corrente($this->file, true);
            $this->direzione = 100;
            $this->crea_recordset();
            for ($row_n = 0; $row_n < count($this->recordset); $row_n++) {
                $this->selezione  = $row_n;
                $this->corrente   = &$this->recordset[$this->selezione];
                $this->precedente = $this->corrente;
                $this->modificato = false;
                if ($this->chiave_corrente($this->file, true) == $old_key) {
                    return true;
                    }
                }
            o2_exception::warning("Record lost after refresh: view <i>".$this->nome.
                                  "</i> sent to first record");
            $this->record_primo();
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Compone la where per rintracciare il record con la chiave univoca corrente.
     * La forma normale per richiedere la chiave del main file è:
     *  $view->chiave_corrente($view->file)
     * L'oggetto $file deve essere la tabella presa dalla vista stessa, non dal repository
     * e quindi la forma standard per un file diverso dal main è:
     *  $view->chiave_corrente($view->files[$file_name])
     *
     * @param  o2_file $file
     * @param  boolean $precedente
     * @param  boolean $for_recordset
     * @param  boolean $backward
     * @param  boolean $no_custom_sort   Used by ->modifica_corrente & ->cancella_corrente
     * @param  boolean $hide_table       Used by ->inserisci/modifica/cancella_corrente
     * @param  boolean $use_pkey         Use Primary Key instead on current index
     * @param  boolean $for_record_id    Return key values as a record unique ID
     * @return string
     */
    function chiave_corrente($file,
                             $precedente     = false,
                             $for_recordset  = false,
                             $backward       = false,
                             $no_custom_sort = false,
                             $hide_table     = false,
                             $use_pkey       = false,
                             $for_record_id  = false) {

        $app              = $_SESSION['o2_app'];
        $condizione_local = array();
        $key_segments     = (!$use_pkey && $this->chiavi_in_uso[$file->indice] ?
                            $file->chiavi[$this->chiavi_in_uso[$file->indice]]->segmenti :
                            $file->chiave->segmenti);
        $file_type        = $this->file->db->server->type;
        $c_o              = constant("o2_".$file_type."_o");
        $c_c              = constant("o2_".$file_type."_c");
        // _____________________________________________ Include custom sorting fields ___
        if (count($this->sort) && !$no_custom_sort) {
            foreach (array_reverse($this->sort) as $sort_segment => $sort_segment_dir) {
                $new_segment = new o2_segmento_indice($this->campi[$sort_segment],
                                                      $sort_segment_dir);
                foreach ($key_segments as $segment_index => $key_segment) {
                    if ($key_segment->campo->phys_name ==
                        $this->campi[$sort_segment]->phys_name) {
                        unset($key_segments[$segment_index]);
                        break;
                        }
                    }
                // _______________________ Manage custom sorting for fields from joins ___
                if ($this->campi[$sort_segment]->file != $this->file->indice) {
                    $new_segment->custom_sorted = true;
                    }
                array_unshift($key_segments, $new_segment);
                }
            }
        // ________________________________________ Loop on key segments to get values ___
        $prev_segm_cond = "";
        foreach ($key_segments as &$segmento_chiave) {
            $trovato          = false;
            $nome_campo_local = $segmento_chiave->campo->phys_name;
            if ($segmento_chiave->custom_sorted) {
                $field_name = ($hide_table ? "" :
                               $c_o.$segmento_chiave->campo->file.$c_c.".").
                              $segmento_chiave->campo->nome_fisico;
                }
            else {
                $field_name = ($hide_table ? "" : $c_o.$file->indice.$c_c.".").
                              $segmento_chiave->campo->nome_fisico;
                }
            $key_field_name = strtoupper($nome_campo_local)."_JXKF";
            $valore_local   = $segmento_chiave->campo->maschera->default;
            $set_di_record  = ($precedente ? $this->precedente : $this->corrente);
            $type_local     = "";
            $dec_local      = 0;
            // __________________ Look for field in main and sorted join tables fields ___
            foreach ($this->campi as $singolo_campo) {
                if ($nome_campo_local == $singolo_campo->phys_name &&
                    ($singolo_campo->file == $file->indice ||
                     $segmento_chiave->custom_sorted) &&
                    $singolo_campo->nome != $key_field_name) {
                    $valore_local = ($precedente ?
                                     $this->precedente($singolo_campo->nome) :
                                     $this->campo($singolo_campo->nome));
                    $type_local   = $singolo_campo->maschera->tipo;
                    $dec_local    = $singolo_campo->maschera->decimali;
                    $trovato      = true;
                    break;
                    }
                }
            // ___________________________ Not found but field has been added by Janox ___
            if (!$trovato &&
                is_array($set_di_record) &&
                array_key_exists($key_field_name, $set_di_record)) {
                $valore_local = ($precedente ?
                                 $this->precedente($key_field_name) :
                                 $this->campo($key_field_name));
                $type_local   = $segmento_chiave->campo->maschera->tipo;
                $dec_local    = $segmento_chiave->campo->maschera->decimali;
                }
            // _______________________________________________ Get field current value ___
            if ($type_local != "N") {
                $valore_local = "'".o2_gateway::normalize($file->db->server->type,
                                                          $valore_local)."'";
                }
            else {
                $valore_local = number_format($valore_local, $dec_local, ".", "");
                }
            // _____________________________________ Add field and value to conditions ___
            if ($for_recordset) {
                // ___________________________________________ Manage blank ('') value ___
                if ($valore_local === "''") {
                    if (($backward && $segmento_chiave->direzione == "D") ||
                        (!$backward && $segmento_chiave->direzione != "D")) {
                        if ($file_type == "oracle") {
                            $text_local = "(".$prev_segm_cond."(RTRIM(".$field_name.
                                          ")>'' OR RTRIM(".$field_name.") IS NOT NULL))";
                            }
                        else {
                            $text_local = "(".$prev_segm_cond.
                                          "(RTRIM(".$field_name.")>''))";
                            }
                        }
                    // _____________________________________ ATTENTION: Nothing is <'' ___
                    else {
                        $text_local = "(0>1)";
                        }
                    $prev_segm_cond.= "(RTRIM(".$field_name.
                                      ")='' OR RTRIM(".$field_name.") IS NULL) AND ";
                    }
                else {
                    $text_local     = "(".$prev_segm_cond.$field_name.
                                          ($backward ?
                                           ($segmento_chiave->direzione != "D" ?
                                            " < " : " > ") :
                                           ($segmento_chiave->direzione != "D" ?
                                            " > " : " < ")).
                                          $valore_local.")";
                    $prev_segm_cond.= "(".$field_name."=".$valore_local.") AND ";
                    }
                }
            // ______________________________ Store values to compose record unique ID ___
            elseif ($for_record_id) {
                $text_local = trim($valore_local);
                }
            // _______________________________________________ Manage blank ('') value ___
            elseif ($valore_local === "''") {
                $text_local = "(RTRIM(".$field_name.
                              ")='' OR RTRIM(".$field_name.") IS NULL)";
                }
            else {
                $text_local = $field_name."=".$valore_local;
                }
            $condizione_local[] = $text_local;
            }
        // ______________________________________________________ Compose return value ___
        if ($for_recordset) {
            $ret_text_local = "(".implode(" OR ", $condizione_local).")";
            if ($file->asp == 'C') {
                $valore_local   = $app->vars['_area']->valore;
                $ret_text_local = ($hide_table ? "" : $c_o.$file->indice.$c_c.".").
                                  "O2ASPID='".$valore_local."' AND (".$ret_text_local.")";
                }
            return $ret_text_local;
            }
        elseif ($for_record_id) {
            return implode('|', $condizione_local);
            }
        else {
            if ($file->asp == 'C') {
                array_unshift($condizione_local,
                              ($hide_table ? "" :  $c_o.$file->indice.$c_c.".").
                              "O2ASPID='".$app->vars['_area']->valore."'");
                }
            return implode(" AND ", $condizione_local);
            }

        }


    /**
     * Ritorna vero se esiste, per $file, la chiave univoca in corrente. Se passato a TRUE
     * $precedente cerca la chiave con i valori precedenti alle eventuali modifiche
     *
     * @param  o2_file $file
     * @param  boolean $precedente
     * @return boolean
    */
    function esiste_chiave($file, $precedente = false) {


        $where_local = $this->chiave_corrente($file, $precedente);
        $ret_val     = o2_gateway::verifyrec($file->db->server->type,
                                             $file->db->server->server,
                                             $file->db->server->user,
                                             $file->db->server->password,
                                             $file->db->nome,
                                             $file->db->proprietario,
                                             $file->nome,
                                             $file->indice,
                                             "*",
                                             $where_local,
                                             "");
        if ($ret_val === false) {
            return false;
            }
        else {
            return true;
            }

        }


    /**
     * Calcola una espressione di aggregazione su un campo e ne restituisce il valore in
     * $target_field di $target_view.
     * Se la proprietà della vista $auto_aggr è TRUE la runtime si prenderà cura del loro
     * aggiornamento al variare del dataset, altrimenti il ricalcolo delle aggregazioni
     * deve essere eseguito tramite l'istruzione di recordset "aggregate".
     *
     * @param  string $function
     * @param  string $aggr_field
     * @param  string $target_view
     * @param  string $target_field
     * @param  string $auto
     * @return boolean
     */
    function aggregate($function, $aggr_field, $target_view, $target_field) {

        $target_field      = strtoupper($target_field);
        $aggr_field        = strtoupper($aggr_field);
        $this->aggregate[] = array('func'     => $function,
                                   'field'    => $aggr_field,
                                   'retview'  => $target_view,
                                   'retfield' => $target_field);
        return true;

        }


    /**
     * Ritorna la clausola di ORDER BY della tabella per la chiave [$chiave].
     * DESC se [$invertito] = true
     *
     * @param  string  $table
     * @param  string  $chiave
     * @param  boolean $invertito
     * @return string
     */
    function order_by($table, $chiave = "", $invertito = false) {

        // ______________________________________ If no table use this view main table ___
        if (!$table) {
            $table = $this->file->indice;
            }
        // ___________________________________________ If no key use table primary key ___
        if (!$chiave) {
            $chiave = $this->files[$table]->chiave->nome;
            }
        // _______________________________________________________________ Custom sort ___
        if ($table == $this->file->indice && count($this->sort)) {
            $key_obj = clone $this->files[$table]->chiavi[$chiave];
            foreach (array_reverse($this->sort) as $sort_segment => $sort_segment_dir) {
                $seg_field   = $this->campi[$sort_segment];
                $new_segment = new o2_segmento_indice($seg_field, $sort_segment_dir);
                $new_segment->custom_sort = true;
                // __________________________ Remove custom segments from standard key ___
                foreach ($key_obj->segmenti as $segment_index => $key_segment) {
                    if ($key_segment->campo->nome_fisico == $seg_field->nome_fisico) {
                        unset($key_obj->segmenti[$segment_index]);
                        }
                    }
                array_unshift($key_obj->segmenti, $new_segment);
                }
            return $key_obj->order_by($this, $table, $invertito);
            }
        // ______________________________________________________________ Standard key ___
        else {
            return $this->files[$table]->chiavi[$chiave]->order_by($this,
                                                                   $table,
                                                                   $invertito);
            }

        }


    /**
     * Position view on first record metching passed conditions, if any, or on first next
     * record.
     *
     * @param array $conditions   Array of locate conditions in the form field=>value
     */
    function locate($conditions = "") {

        // __________ NO LOCATE CONDITIONS: recreate recordset and locate first record ___
        if (!is_array($conditions)) {
            $this->vista     = false;
            $this->direzione = 0;
            return true;
            }
        $prefix_save    = $this->prefisso;
        $this->prefisso = false;
        $this->record_primo();
        $conditions = array_change_key_case($conditions, CASE_UPPER);
        $main_file  = $this->file->indice;
        $c_o        = constant("o2_".$this->file->db->server->type."_o");
        $c_c        = constant("o2_".$this->file->db->server->type."_c");
        // ___________________________________________________________ Get a view copy ___
        $view            = clone $this;
        $view->righe_vis = 1;
        $view->aggregate = array();
        $view->prefisso  = false;
        $view->suffisso  = false;
        // _____________________________________________ Clear cloned view dependences ___
        $view->dipendenze = array();
        unset($_SESSION['o2_app']->istanze_prg[$this->id_esecuzione]->
                                    variazioni[$view->nome]);
        $used_segments   = array();
        $order_by_used   = "";
        $order_by_unused = "";
        /* Determino l'ordinamento per il recupero del giusto record.
           I segmenti di chiave non valorizzati dalla locate li "sfilo" e li riaggiungo
           in fondo alla chiave                                                         */
        $used_key = $view->file->chiavi[$view->chiavi_in_uso[$main_file]];
        foreach ($used_key->segmenti as $singolo_segmento) {
            foreach ($view->campi as $single_field) {
                if ($single_field->nome_fisico == $singolo_segmento->campo->nome_fisico &&
                    $single_field->file == $main_file) {
                    $alias_campo = $single_field->nome;
                    break;
                    }
                }
            // _____ If field is NOT in locate criteria (or passed with default value) ___
            if (!in_array($alias_campo, array_keys($conditions)) ||
                $conditions[$alias_campo] == $view->campi[$alias_campo]->
                                              maschera->default) {
                $separatore      = ($order_by_unused ? ", " : "");
                $order_by_unused.= $separatore.$c_o.$alias_campo.$c_c." ASC";
                }
            // ________________________________________ If field is in locate criteria ___
            else {
                $separatore      = ($order_by_used ? ", " : "");
                $order_by_used  .= $separatore.$c_o.$alias_campo.$c_c." ASC";
                $used_segments[] = $alias_campo;
                }
            }
        /* Compongo la nuova chiave e aggiungo le select con le clausole per il recupero
           del giusto record                                                            */
        foreach ($conditions as $cnd_field => $cnd_value) {
            if ($cnd_value != $view->campi[$cnd_field]->maschera->default) {
                if (!in_array($cnd_field, $used_segments)) {
                    $order_by_used.= ($order_by_used ? ", " : "").
                                     $c_o.$cnd_field.$c_c." ASC";
                    }
                $name_in_tab = $view->campi[$cnd_field]->nome;
                foreach ($view->file->campi as $single_name_in_tab => $fieldobj) {
                    if ($fieldobj->nome_fisico == $view->campi[$cnd_field]->nome_fisico) {
                        $name_in_tab = $single_name_in_tab;
                        break;
                        }
                    }
                $view->usa($cnd_field."jxlct",
                           $view->campi[$cnd_field]->file,
                           $name_in_tab,
                           rtrim($cnd_value));
                }
            }
        // _________________________________________________ Set new ORDER BY clausole ___
        $view->indici[$main_file] = $order_by_used.
                                    ($order_by_used && $order_by_unused ? ", " : "").
                                    $order_by_unused;
        $view->prefisso_exe       = false;
        $view->direzione          = 0;
        $view->crea_recordset(true);
        $view->seleziona_riga();
        // _____________________________ Get record unique key to filter original view ___
        $this->key_filter_next = "((".$view->chiave_corrente($view->file, false, true).
                                  ") OR (".$view->chiave_corrente($view->file)."))";
        unset($view);
        $this->direzione = 2;
        $this->selezione = 0;
        $this->prefisso  = $prefix_save;
        $this->crea_recordset();
        $this->offset_ini  = -1;
        $this->offset_fine = -$this->righe_vis;

        }


    /**
     * Set relocation key for the view (after insert or modify)
     *
     * @param array $key_segments
     */
    function set_relocation($key_segments) {

        if (!$this->suffix_running) {
            $this->relocate_to_key = $key_segments;
            if ($_SESSION['o2_app']->relocate_now) {
                $this->relocate();
                }
            }

        }


    /**
     * Relocate view to last updated record ** RELOCATE AFTER MODIFY **
     *
     */
    function relocate() {

        if ($this->relocate_to_key) {
            $this->key_filter_same = $this->relocate_to_key;
            $this->direzione       = 100;
            $this->selezione       = 0;
            $this->crea_recordset();
            }
        $this->relocate_to_key = false;

        }


    /**
    * Attiva la costruzione della form del selettore evoluto per la vista
    *
    */
    function selettore() {

        $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
        if (isset($prg->form[$this->nome."_o2sel_form"])) {
            unset($prg->form[$this->nome."_o2sel_form"]);
            }
        $prg->form($this->nome."_o2sel");
        $form_local = $prg->form[$this->nome."_o2sel_form"];
        $form_local->x(200);
        $form_local->y(100);
        $form_local->larghezza(800);
        $form_local->altezza(500);
        $form_local->titolo("Selector [".$this->nome."]");
        $ctrl_local = $form_local->ctrldef('table1', "tab", "", $this->nome, "");
            $ctrl_local->x(10);
            $ctrl_local->y(10);
            $ctrl_local->larghezza(780);
            $ctrl_local->altezza(430);
            $ctrl_local->modificabile(false);
        $ctrl_local = $form_local->ctrldef('navigator1',
                                           "navigator",
                                           "",
                                           $this->nome,
                                           "");
            $ctrl_local->x(10);
            $ctrl_local->y(440);
            $ctrl_local->larghezza(780);
            $ctrl_local->altezza(20);
        $numero_campo = 0;
        foreach ($this->campi as $nome_local => &$campo_local) {
            $numero_campo++;
            $ctrl_local = $form_local->ctrldef($campo_local->nome."_selfield",
                                               "edit",
                                               "table1",
                                               $this->nome,
                                               $nome_local);
            $ctrl_local->info_padre(array(1 ,$numero_campo, $numero_campo, $nome_local));
            }

        }


    /**
     * Imposta i valori dell'array ->corrente ai correnti valori dei campi
     *
     * @param array $valori_riga
     */
    function imposta_corrente($valori_riga = array()) {

        $this->corrente = $valori_riga;
        $this->calcola_formule();
        $this->precedente = $this->corrente;
        $this->modificato = false;

        }


    /**
     * Esegue l'update della main table ($this->file) per i campi di ->corrente diversi da
     * ->precedente
     *
     * @return boolean
     */
    function modifica_corrente() {

        $app        = $_SESSION['o2_app'];
        $update_arr = array();
        $db_type    = $this->file->db->server->type;
        // ____________________________________________________ Insert trace on record ___
        if ($app->record_trace &&
            (!isset($GLOBALS['jxtracesuspend']) ||
             !in_array($this->file->repos_index, $GLOBALS['jxtracesuspend']))) {
            $trace_c_user = $this->file->record_trace["cu"];
            $trace_c_date = $this->file->record_trace["cd"];
            $trace_c_time = $this->file->record_trace["ct"];
            $trace_u_user = $this->file->record_trace["uu"];
            $trace_u_date = $this->file->record_trace["ud"];
            $trace_u_time = $this->file->record_trace["ut"];
            $app_c_user   = $app->record_trace["cu"];
            $app_c_date   = $app->record_trace["cd"];
            $app_c_time   = $app->record_trace["ct"];
            $app_u_user   = $app->record_trace["uu"];
            $app_u_date   = $app->record_trace["ud"];
            $app_u_time   = $app->record_trace["ut"];
            }
        foreach ($this->campi as $singolo_campo) {
            if ($singolo_campo->file == $this->file->indice) {
                $field_name = $singolo_campo->nome;
                if (isset($this->corrente[$field_name]) &&
                    ($this->corrente[$field_name] !== $this->precedente[$field_name])) {
                    $value = $this->corrente[$field_name];
                    // ___________________ Trace columns are removed if valued by user ___
                    if ($trace_c_user && $field_name == $app_c_user) {
                        $trace_c_user = false;
                        // _______________ Zero valued trace fields are left unchenged ___
                        if (!$value) {
                            continue;
                            }
                        }
                    elseif ($trace_c_date && $field_name == $app_c_date) {
                        $trace_c_date = false;
                        // _______________ Zero valued trace fields are left unchenged ___
                        if (!trim($value, '0')) {
                            continue;
                            }
                        }
                    elseif ($trace_c_time && $field_name == $app_c_time) {
                        $trace_c_time = false;
                        // _______________ Zero valued trace fields are left unchenged ___
                        if (!trim($value, '0')) {
                            continue;
                            }
                        }
                    elseif ($trace_u_user && $field_name == $app_u_user) {
                        $trace_u_user = false;
                        // _______________ Zero valued trace fields are left unchenged ___
                        if (!$value) {
                            continue;
                            }
                        }
                    elseif ($trace_u_date && $field_name == $app_u_date) {
                        $trace_u_date = false;
                        // _______________ Zero valued trace fields are left unchenged ___
                        if (!trim($value, '0')) {
                            continue;
                            }
                        }
                    elseif ($trace_u_time && $field_name == $app_u_time) {
                        $trace_u_time = false;
                        // _______________ Zero valued trace fields are left unchenged ___
                        if (!trim($value, '0')) {
                            continue;
                            }
                        }
                    $f_name = $singolo_campo->nome_fisico;
                    switch ($singolo_campo->maschera->tipo) {
                        case "A": // ___________________________________________ Alpha ___
                        case "S": // _______________________________________ Structure ___
                            $update_arr[$f_name] = "'".o2_gateway::normalize($db_type,
                                                                             $value)."'";
                            break;
                        case "D":
                        case "O":
                            $update_arr[$f_name] = "'".$value."'";
                            break;
                        case "L": // _________________________________________ Logical ___
                            $update_arr[$f_name] = ($value ? "'1'" : "'0'");
                            break;
                        default:
                            $update_arr[$f_name] = $value;
                            break;
                        }
                    }
                }
            }
        if (count($update_arr) != 0) {
            $prev_key = $this->chiave_corrente($this->file,
                                               true,
                                               false,
                                               false,
                                               true,
                                               true,
                                               true);
            // ___________________________________________________ Manage log by level ___
            if ($app->tables_log &&
                (!isset($GLOBALS['jxlogsuspend']) ||
                 !in_array($this->file->repos_index, $GLOBALS['jxlogsuspend'])) &&
                ($this->file->log_level == "C" || $this->file->log_level == "M")) {
                // ______________________ Skip multiple record changes for transaction ___
                if (o2_gateway::use_key_once($db_type,
                                             $this->file->db->server->server,
                                             $this->file->db->server->user,
                                             $this->file->write_name,
                                             $prev_key)) {
                    $this->file->log_write_full("C", $prev_key);
                    }
                }
            // _______________________________________ Update existing tracing columns ___
            if ($trace_u_user) {
                $update_arr[$trace_u_user] = "'".
                                             o2_gateway::normalize($db_type, $app->user).
                                             "'";
                }
            if ($trace_u_date) {
                $update_arr[$trace_u_date] = "'".date("Ymd")."'";
                }
            if ($trace_u_time) {
                $update_arr[$trace_u_time] = "'".date("His")."'";
                }
            $ret_val = o2_gateway::modifyrec($db_type,
                                             $this->file->db->server->server,
                                             $this->file->db->server->user,
                                             $this->file->db->server->password,
                                             $this->file->db->nome,
                                             $this->file->db->proprietario,
                                             $this->file->write_name,
                                             $this->file->indice,
                                             $update_arr,
                                             $prev_key);
            // ___________________________________________________ Manage log by level ___
            if ($app->tables_log &&
                (!isset($GLOBALS['jxlogsuspend']) ||
                 !in_array($this->file->repos_index, $GLOBALS['jxlogsuspend'])) &&
                $this->file->log_level == "R") {
                $this->log_write("R");
                }
            }
        $this->precedente = $this->corrente;
        $this->modificato = false;
        return true;

        }


    /**
     * Esegue l'insert into $file per i campi di ->corrente diversi da ->precedente
     *
     * @return boolean
     */
    function inserisci_corrente() {

        $app               = $_SESSION['o2_app'];
        $db_type           = $this->file->db->server->type;
        $insert_nomi_arr   = array();
        $insert_valori_arr = array();
        if ($this->file->asp == 'C') {
            $insert_nomi_arr[]         = 'O2ASPID';
            $this->corrente['O2ASPID'] = o2_gateway::normalize($db_type,
                                                             $app->vars['_area']->valore);
            $insert_valori_arr[]       = "'".$this->corrente['O2ASPID']."'";
            }
        $doit = false;
        // ____________________________________________________ Insert trace on record ___
        if ($app->record_trace &&
            (!isset($GLOBALS['jxtracesuspend']) ||
             !in_array($this->file->repos_index, $GLOBALS['jxtracesuspend']))) {
            $trace_c_user = $this->file->record_trace["cu"];
            $trace_c_date = $this->file->record_trace["cd"];
            $trace_c_time = $this->file->record_trace["ct"];
            $trace_u_user = $this->file->record_trace["uu"];
            $trace_u_date = $this->file->record_trace["ud"];
            $trace_u_time = $this->file->record_trace["ut"];
            $app_c_user   = $app->record_trace["cu"];
            $app_c_date   = $app->record_trace["cd"];
            $app_c_time   = $app->record_trace["ct"];
            $app_u_user   = $app->record_trace["uu"];
            $app_u_date   = $app->record_trace["ud"];
            $app_u_time   = $app->record_trace["ut"];
            }
        foreach ($this->campi as $singolo_campo) {
            if ($singolo_campo->file == $this->file->indice) {
                $field_name = $singolo_campo->nome;
                if ($this->corrente[$field_name] != $this->precedente[$field_name]) {
                    $insert_nomi_arr[] = $singolo_campo->nome_fisico;
                    // ___________________ Trace columns are removed if valued by user ___
                    if ($trace_c_user && $field_name == $app_c_user) {
                        $trace_c_user = false;
                        }
                    elseif ($trace_c_date && $field_name == $app_c_date) {
                        $trace_c_date = false;
                        }
                    elseif ($trace_c_time && $field_name == $app_c_time) {
                        $trace_c_time = false;
                        }
                    elseif ($trace_u_user && $field_name == $app_u_user) {
                        $trace_u_user = false;
                        }
                    elseif ($trace_u_date && $field_name == $app_u_date) {
                        $trace_u_date = false;
                        }
                    elseif ($trace_u_time && $field_name == $app_u_time) {
                        $trace_u_time = false;
                        }
                    switch ($singolo_campo->maschera->tipo) {
                        case "A": // ___________________________________________ Alpha ___
                        case "S": // _______________________________________ Structure ___
                            $insert_valori_arr[] = "'".o2_gateway::normalize($db_type,
                                                        $this->corrente[$field_name])."'";
                            break;
                        case "D": // ____________________________________________ Date ___
                        case "O": // ____________________________________________ Time ___
                            $insert_valori_arr[] = "'".$this->corrente[$field_name]."'";
                            break;
                        case "L": // _________________________________________ Logical ___
                            $insert_valori_arr[] = ($this->corrente[$field_name] ?
                                                    "'1'" : "'0'");
                            break;
                        default: // ___________________________________________ Number ___
                            $insert_valori_arr[] = $this->corrente[$field_name];
                            break;
                        }
                    $doit = true;
                    }
                }
            }
        if ($doit) {
            // _______________________________________ Update existing tracing columns ___
            $t = time();
            if ($trace_c_user) {
                $insert_nomi_arr[]   = $trace_c_user;
                $insert_valori_arr[] = "'".o2_gateway::normalize($db_type,$app->user)."'";
                }
            if ($trace_c_date) {
                $insert_nomi_arr[]   = $trace_c_date;
                $insert_valori_arr[] = "'".date("Ymd", $t)."'";
                }
            if ($trace_c_time) {
                $insert_nomi_arr[]   = $trace_c_time;
                $insert_valori_arr[] = "'".date("His", $t)."'";
                }
            if ($trace_u_user) {
                $insert_nomi_arr[]   = $trace_u_user;
                $insert_valori_arr[] = "'".o2_gateway::normalize($db_type,$app->user)."'";
                }
            if ($trace_u_date) {
                $insert_nomi_arr[]   = $trace_u_date;
                $insert_valori_arr[] = "'".date("Ymd", $t)."'";
                }
            if ($trace_u_time) {
                $insert_nomi_arr[]   = $trace_u_time;
                $insert_valori_arr[] = "'".date("His", $t)."'";
                }
            $ret_val = o2_gateway::insertrec($db_type,
                                             $this->file->db->server->server,
                                             $this->file->db->server->user,
                                             $this->file->db->server->password,
                                             $this->file->db->nome,
                                             $this->file->db->proprietario,
                                             $this->file->write_name,
                                             $this->file->indice,
                                             $insert_nomi_arr,
                                             $insert_valori_arr);
            }
        // _______________________________________________________ Manage log by level ___
        if ($app->tables_log &&
            (!isset($GLOBALS['jxlogsuspend']) ||
             !in_array($this->file->repos_index, $GLOBALS['jxlogsuspend']))) {
            // ____________________________ Save record key for in-transaction updates ___
            $rec_key = $this->chiave_corrente($this->file,
                                              false,
                                              false,
                                              false,
                                              true,
                                              true,
                                              true);
            o2_gateway::use_key_once($db_type,
                                     $this->file->db->server->server,
                                     $this->file->db->server->user,
                                     $this->file->write_name,
                                     $rec_key);
            if ($this->file->log_level == "R") {
                $this->log_write("I");
                }
            elseif ($this->file->log_level == "I" || $this->file->log_level == "E") {
                $this->file->log_write_full("I", $rec_key);
                }
            }
        $this->totale_record++;
        $this->precedente = $this->corrente;
        $this->modificato = false;
        return true;

        }


   /**
     * Esegue l'insert into $file in maniera cached
     *
     * @return boolean
     */
    function bulk_insert($fields, $values, $flush = false) {

        $app     = $_SESSION['o2_app'];
        $db_type = $this->file->db->server->type;
        $chunk   = $this->file->db->server->chunk_size;
        if ($this->file->asp == 'C') {
            $fields[]                  = 'O2ASPID';
            $this->corrente['O2ASPID'] = o2_gateway::normalize($db_type,
                                                             $app->vars['_area']->valore);
            $values[]                  = "'".$this->corrente['O2ASPID']."'";
            }
//        if ($this->cached_insert) {
        if ($values) {
            $this->insert_cache.= '('.implode(',', $values).'),';
            $this->totale_record++;
            }
        $cache = strlen($this->insert_cache);
        if (($cache > 0) && ($flush || ($cache > $chunk))) {
            o2_gateway::insertrec($db_type,
                                  $this->file->db->server->server,
                                  $this->file->db->server->user,
                                  $this->file->db->server->password,
                                  $this->file->db->nome,
                                  $this->file->db->proprietario,
                                  $this->file->write_name,
                                  $this->file->indice,
                                  $fields,
                                  array(substr($this->insert_cache, 1, -2)));
            $this->insert_cache = '';
            }
        $this->precedente = $this->corrente;
        $this->modificato = false;
        return true;

        }


    /**
     * Esegue la cancellazione del record corrente dal main file
     *
     * @return boolean
     */
    function cancella_corrente() {

        $app      = $_SESSION['o2_app'];
        $prev_key = $this->chiave_corrente($this->file,
                                           true,
                                           false,
                                           false,
                                           true,
                                           true,
                                           true);
        // _______________________________________________________ Manage log by level ___
        if ($app->tables_log &&
            (!isset($GLOBALS['jxlogsuspend']) ||
             !in_array($this->file->repos_index, $GLOBALS['jxlogsuspend'])) &&
            (strpos("DEM", $this->file->log_level) !== false)) {
            $this->file->log_write_full("D", $prev_key);
            }
        $ret_val = o2_gateway::deleterec($this->file->db->server->type,
                                         $this->file->db->server->server,
                                         $this->file->db->server->user,
                                         $this->file->db->server->password,
                                         $this->file->db->nome,
                                         $this->file->db->proprietario,
                                         $this->file->write_name,
                                         $this->file->indice,
                                         $prev_key);
        // _______________________________________________________ Manage log by level ___
        if ($app->tables_log &&
            (!isset($GLOBALS['jxlogsuspend']) ||
             !in_array($this->file->repos_index, $GLOBALS['jxlogsuspend'])) &&
            $this->file->log_level == "R") {
            $this->log_write("D");
            }
        $this->totale_record--;
        $this->offset_fine--;
        $this->relocate_to_key = false;
        return true;

        }


    /**
     * Log table data modifications to log table.
     *
     * Record insert
     *    Inserts one record with [log_action] = [I]nsert, containg new value for each
     *    field
     *
     * Record delete
     *    Inserts one record with [log_action] = [D]elete, containg last value for each
     *    field
     *
     * Record update
     *    Inserts two records, with same timestamp:
     *    one with [log_action] = [D]elete, containing old value for each field;
     *    one with [log_action] = [U]pdate, containing new value for each field.
     *
     *
     * @param  string $action
     * @param  string $timestamp
     * @return boolean
     */
    function log_write($action = "I", $timestamp = "") {

        $dele_rec = false;
        // _____________________________________________________ Log level "R" - U + D ___
        if ($action == "R") {
            $action   = "U";
            $dele_rec = true;
            }
        $app = $_SESSION['o2_app'];
        if (!$timestamp) {
            $timestamp = $app->runtime->microtime();
            }
        $file_local        = $this->file->logtable();
        $insert_nomi_arr   = array();
        $insert_valori_arr = array();
        $db_type           = $file_local->db->server->type;
        $c_o               = constant("o2_".$db_type."_o");
        $c_c               = constant("o2_".$db_type."_c");
        if ($this->file->asp == 'C') {
            $insert_nomi_arr[]   = 'O2ASPID';
            $insert_valori_arr[] = "'".
                                   o2_gateway::normalize($db_type,
                                                         $app->vars['_area']->valore).
                                   "'";
            }
        $insert_nomi_arr[]   = $c_o."log_id".$c_c;
        $insert_valori_arr[] = "'".$timestamp."'";
        $insert_nomi_arr[]   = $c_o."log_act".$c_c;
        $insert_valori_arr[] = "'".$action."'";
        $insert_nomi_arr[]   = $c_o."log_user".$c_c;
        $insert_valori_arr[] = "'".$app->user."'";
        foreach ($this->campi as $singolo_campo) {
            if ($singolo_campo->file == $this->file->indice &&
                !strpos($singolo_campo->nome, "_JXKF")      &&
                !in_array($singolo_campo->nome_fisico, $insert_nomi_arr)) {
                $value_local         = ($action != "D" && $action != "C" ?
                                        $this->corrente[$singolo_campo->nome] :
                                        $this->precedente[$singolo_campo->nome]);
                $insert_nomi_arr[]   = $singolo_campo->nome_fisico;
                $insert_valori_arr[] = ($singolo_campo->maschera->tipo != "N" ?
                                        "'".o2_gateway::normalize($db_type,
                                                                  $value_local)."'" :
                                        ($value_local ? $value_local : "0"));
                }
            }
        $ret_val    = o2_gateway::insertrec($db_type,
                                            $file_local->db->server->server,
                                            $file_local->db->server->user,
                                            $file_local->db->server->password,
                                            $file_local->db->nome,
                                            $file_local->db->proprietario,
                                            $file_local->nome,
                                            $file_local->indice,
                                            $insert_nomi_arr,
                                            $insert_valori_arr);
        // _____________________________________________________ Log level "R" - U + D ___
        if ($dele_rec) {
            return $this->log_write("D", $timestamp);
            }
        return true;

        }


    /**
     * Esegue la cancellazione dei tutti i record del main file, secondo i filtri correnti
     * della vista
     *
     * @return boolean
     */
    function delete_recordset() {

        $mfile      = $this->file;
        $mfile_name = $mfile->indice;
        $this->range2where($mfile_name);
        $this->range2formule();
        // _______________ If delete-log is active on main file: delete loop is needed ___
        if ($_SESSION['o2_app']->tables_log &&
            (!isset($GLOBALS['jxlogsuspend']) ||
             !in_array($this->file->repos_index, $GLOBALS['jxlogsuspend'])) &&
            strpos("RDEM", $mfile->log_level) !== false) {
            return $this->delete_loop();
            }
        // ___________________________ If ranges on link tables: delete loop is needed ___
        foreach ($this->esclusivi as $escl_tbl => $escl_fld) {
            if (count($escl_fld) && $escl_tbl != $mfile_name) {
                return $this->delete_loop();
                }
            }
        // _______________________ If delete is solved as a SQL "DELETE ... WHERE ..." ___
        $where = (isset($this->wheres[$mfile_name]) && $this->wheres[$mfile_name] ?
                  $this->wheres[$mfile_name] : false);
        o2_gateway::deleterec($mfile->db->server->type,
                              $mfile->db->server->server,
                              $mfile->db->server->user,
                              $mfile->db->server->password,
                              $mfile->db->nome,
                              $mfile->db->proprietario,
                              $mfile->write_name,
                              $mfile_name,
                              $where);
        $this->vista     = false;
        $this->direzione = 0;
        return true;

        }


    /**
     * Esegue la cancellazione dei tutti i record del main file, secondo i filtri correnti
     * della vista, con viste non risolvibili in SQL.
     *
     * @return boolean
     */
    function delete_loop() {

        $this->looping_on = true;
        $this->record_primo();
        // ___________________________________________________ Loops until end of file ___
        if (count($this->recordset) > 0) {
            do {
                $this->cancella_corrente();
                } while ($this->record_avanti());
            $this->relocate_to_key = false;
            }
        $this->looping_on = false;
        return true;

        }


    /**
     * Execute a SQL INSERT-INTO from this view to target table.
     * Fields are matched by name.
     *
     * @param  string $target_table
     * @return boolean
     */
    function insert_into($target_table) {

        $app   = $_SESSION['o2_app'];
        $mfile = $this->file->indice;
        $this->range2where($mfile);
        $this->range2formule();
        $where      = (isset($this->wheres[$mfile]) && $this->wheres[$mfile] ?
                       $this->wheres[$mfile] : false);
        $db_from    = $this->file->db->nome;
        $owner_from = $this->file->db->proprietario;
        $target_obj = $app->get_table($target_table);
        // ______________________________________ Error getting target table reference ___
        if ($target_obj == false) {
            return false;
            }
        // _______________________________________________ Error: different db from/to ___
        if ($this->file->db->nome != $target_obj->db->nome) {
            throw new o2_exception("Cannot insert from a db to a different db.<br>".
                                   "Database from: <i>".$this->file->db->nome."</i><br>".
                                   "Database to: <i>".$target_obj->db->nome."</i>.",
                                   o2error_DBTABLECOPY);
            return false;
            }
        $fields = array();
        foreach ($this->campi as $src_fld_name => $src_field) {
            $match_found = false;
            foreach ($target_obj->campi as $trg_fld_name => $trg_field) {
                // _________________________________ Match by logical or physical name ___
                if (strtoupper($trg_fld_name) == $src_fld_name ||
                    strtoupper($trg_field->phys_name) == $src_fld_name) {
                    $fields[$src_field->nome_fisico] = $trg_field->nome_fisico;
                    $match_found = true;
                    break;
                    }
                }
            // ____________________________________________ If no matching field found ___
            if (!$match_found) {
                throw new o2_exception("Cannot find a match for source field <i>".
                                       $src_fld_name."</i>, inserting view <i>".
                                       $this->nome."</i> into table <i>".
                                       $target_table."<i>.",
                                       o2error_DBTABLECOPY);
                return false;
                }
            // ____________________________________________ If no matching fields type ___
            elseif ($src_field->tipo != $trg_field->tipo) {
                throw new o2_exception("No matching type, inserting view <i>".
                                       $this->nome."</i> into table <i>".
                                       $target_table."</i>.<br>Source field <i>".
                                       $src_fld_name."</i>: ".$src_field->tipo.
                                       "<br>Target field <i>".$trg_fld_name."</i>: ".
                                       $trg_field->tipo.".",
                                       o2error_DBTABLECOPY);
                return false;
                }
            }
        $aff_rows = o2_gateway::insertfrom($this->file->db->server->type,
                                           $this->file->db->server->server,
                                           $this->file->db->server->user,
                                           $this->file->db->server->password,
                                           $this->file->db->nome,
                                           $this->file->db->proprietario,
                                           $this->file->nome,
                                           $target_obj->db->nome,
                                           $target_obj->db->proprietario,
                                           $target_obj->nome,
                                           $fields,
                                           $where);
        $this->vista     = false;
        $this->direzione = 0;
        return $aff_rows;

        }


   /**
    * Esegue la scrittura del file in accordo con la modalità del task
    *
    */
    function scrittore() {

        // _____________________ RETURN if view has NOT been MODIFIED and not DELETING ___
        if (!$this->modificato && $this->status != "D") {
            return;
            }
        // _____________________________________ Custom FROM (table name or SQL query) ___
        if ($this->custom_from) {
            $this->struttura();
            }
        // _____________________________________ Record WRITE --- N.B. Only main file! ___
        switch ($this->status) {
            case "I": // _______________________________________________ Insert record ___
                $this->inserisci_corrente();
                $this->status = "M";
                $this->set_relocation("((".$this->chiave_corrente($this->file,
                                                                  false,
                                                                  true).") OR (".
                                      $this->chiave_corrente($this->file)."))");
                break;
            case "M": // _______________________________________________ Modify record ___
                $variata_chiave = ($this->chiave_corrente($this->file) !==
                                   $this->chiave_corrente($this->file, true));
                $this->modifica_corrente();
                if ($variata_chiave) {
                    $this->set_relocation("((".$this->chiave_corrente($this->file,
                                                                      false,
                                                                      true).") OR (".
                                          $this->chiave_corrente($this->file)."))");
                    }
                break;
            case "D": // _______________________________________________ Delete record ___
                $this->cancella_corrente();
                break;
            }

        }


    /**
     * Imposta il valore di un campo del task
     *
     * @param  string  $campo
     * @param  mix     $valore
     * @param  boolean $init
     * @return boolean
     */
    function imposta($campo, $valore, $init = false) {

        $campo         = strtoupper($campo);
        $file_to_check = ($this->campi[$campo]->file ?
                          $this->campi[$campo]->file :
                          $this->file->indice);
        if (($this->variato($file_to_check) || !isset($this->corrente[$campo])) &&
             $this->status != "I") {
            if (!$this->vista ||
                 $this->range2where($this->file->indice) ||
                 $this->range2formule()) {
                $this->record_primo();
                }
            if (!isset($this->recordset[0]) && !$init) {
                $wheres = '';
                foreach ($this->wheres as $where) {
                    if ($where) {
                        $wheres.= ($wheres ? ' AND ' : '').$where;
                        }
                    }
                throw new o2_exception("Cannot update field <i>".
                                       $campo."</i> on view <i>".$this->nome.
                                       "</i>: view has no current record.<br><hr>".
                                       htmlspecialchars($wheres),
                                       o2error_EXECUTION);
                return false;
                }
            }
        $dummy_ctrl           = new o2_ctrl();
        $dummy_ctrl->maschera = $this->campi[$campo]->maschera;
        $dummy_ctrl->valore   = $valore;
        $valore               = $dummy_ctrl->ctrl2sql();
        if ($this->corrente[$campo] !== $valore && (!$init || $this->zero($campo))) {
            $this->corrente[$campo] = $valore;
            if ($init) {
                $this->precedente[$campo] = $this->corrente[$campo];
                }
            else {
                $this->modificato = true;
                }
            // __________________________________ Reset cached descriptions for combos ___
            if (isset($this->corrente[$campo."_jxcbdesc"])) {
                unset($this->corrente[$campo."_jxcbdesc"]);
                }
            $this->comunica_variazioni();
            }
        return true;

        }


    /**
     * Ritorna il valore di $field_name per il record corrente
     *
     * @param  string $field_name
     * @return mix
     */

    function campo($field_name) {

        $field_name = strtoupper($field_name);
        $field_file = $this->campi[$field_name]->file;
        $main_file  = $this->file->indice;
        // ___________________________________ If source view is not already computing ___
        if (!$this->computing) {
            // _________________________________________ If view dataset is not actual ___
            if (!$this->vista) {
                $this->crea_recordset();
                }
            // ________________________________________________ Check for view changes ___
            elseif ($this->variato($field_file && !$this->files[$field_file]->link_by_sql
                                   ? $field_file : $main_file)) {
                // _______________________________ If changed WHERE clause for dataset ___
                if ($this->range2where($main_file) || $this->range2formule()) {
                    $this->record_primo();
                    }
                elseif ($field_file && $field_file != $main_file &&
                        !$this->files[$field_file]->link_by_sql &&
                        $this->range2where($field_file, $this->selezione)) {
                    if ($this->status != "I" && !$this->modificato) {
                        $this->record_primo();
                        }
                    }
                // ____________ Verify if current record is in INSERT mode or modified ___
                elseif (($this->status == "I" || $this->modificato) &&
                         $this->corrente !== $this->verified[$main_file]) {
                    $this->verifica_record($this->selezione);
                    $this->verified[$main_file] = $this->corrente;
                    }
                }
            // ________________ Verify if current record is in INSERT mode or modified ___
            elseif (($this->status == "I" || $this->modificato) &&
                     $this->corrente !== $this->verified[$main_file]) {
                $this->verifica_record($this->selezione);
                $this->verified[$main_file] = $this->corrente;
                }
            }
        $evaluating_view = $_SESSION['o2_app']->
                            istanze_prg[$this->id_esecuzione]->evaluating_view;
        if ($evaluating_view && $evaluating_view != $this->nome) {
            $this->dipendenze[$evaluating_view] = false;
            }
        return (!is_null($this->corrente[$field_name]) ?
                $this->corrente[$field_name]           :
                (isset($this->campi[$field_name]->maschera->default) ?
                 $this->campi[$field_name]->maschera->default        :
                 ""));

        }


    /**
     * Ritorna il valore di $campo_corrente precedente alle modifiche
     *
     * @param  string $campo_corrente
     * @return mix
     */
    function precedente($campo_corrente) {

        $campo_corrente = strtoupper($campo_corrente);
        /* Se la vista sorgente non ha ancora prodotto una vista e non è già in
           produzione della vista la produco                                            */
        if (!$this->vista     &&
            !$this->computing &&
            $this->files[$this->campi[$campo_corrente]->file]) {
            $this->crea_recordset();
            }
        return (!is_null($this->precedente[$campo_corrente]) ?
                $this->precedente[$campo_corrente] :
                $this->campi[$campo_corrente]->maschera->default);

        }


    /**
     * Returns boolean value true/false if field value is equal/different from its own
     * default value
     *
     * @param  string $field_name
     * @return boolean
     */
    function zero($field_name) {

        $field_local   = $this->campi[$field_name];
        $default_local = $field_local->maschera->default;
        $val_local     = $this->campo($field_name);
        $ret_local     = false;
        switch ($field_local->maschera->tipo) {
            case "S":
                $ret_local = ($val_local === $default_local);
                break;
            case "N":
                $ret_local = (floatval($val_local) === floatval($default_local));
                break;
            case "L":
                $ret_local = (!($val_local) === !($default_local));
                break;
            default:
                $ret_local = (trim($val_local) === trim($default_local));
                break;
            }
        return $ret_local;

        }

    }


class o2_virtualview extends o2_view {

    /*     ===== PROPERTIES =========================================================== */
    /*     _____ DEFINITION LISTS _____________________________________________________ */
    public $variabili = array(); /* o2_campo_task objects of used variables             */


    /**
     * Costruttore
     *
     * @param  string $prg
     * @param  string $id_esecuzione_prg
     * @return o2_view
     */
    function __construct($prg, $id_esecuzione_prg) {

        $this->dbdata        = false;
        $this->nome          = "prg§_§var";
        $this->prg           = $prg;
        $this->id_esecuzione = $id_esecuzione_prg;
        $this->def_function  = $this->prg."§§".$this->nome."_def";
        $def_function        = $this->def_function;
        if (is_callable($def_function)) {
            $def_function($this);
            }

        }


    /**
     * Aggiunge una variabile virtual alla vista
     *
     * @param string $variabile
     * @param string $maschera
     */
    function definisci($variabile, $maschera) {

        if (!$_SESSION['o2_app']->maschere[$maschera]) {
            throw new o2_exception("Unknown data model <i>".$maschera.
            "</i> requested for variable <i>".$variabile.
            "</i> in view <i>".$this->nome."</i>",
            o2error_UNKNOWNMODEL);
            }
        else {
            $variabile                   = strtoupper($variabile);
            $maschera_local              = $_SESSION['o2_app']->maschere[$maschera];
            $this->variabili[$variabile] = new o2_campo_task($variabile,
                                                             "",
                                                             new o2_campo("",
                                                                          $variabile,
                                                                          $variabile,
                                                                          $maschera),
                                                             null,
                                                             null,
                                                             null,
                                                             null);
            $this->variabili[$variabile]->valore = $maschera_local->default;
            $this->corrente[$variabile]          = $maschera_local->default;
            }

        }


    /**
     * Ritorna il riferimento all'oggetto o2_campo dell'oggetto variabile da associare ad
     * un controllo
     *
     * @param  string $nome_campo
     * @return o2_campo
     */
    function &campo_per_controllo($nome_campo) {

        $nome_campo = strtoupper($nome_campo);
        if (key_exists($nome_campo, $this->variabili)) {
            return $this->variabili[$nome_campo];
            }

        }


    /**
     * Imposta il valore di un campo della vista
     *
     * @param string  $field
     * @param mix     $value
     * @param boolean $init
     */
    function imposta($field, $value, $init = false) {

        $field = strtoupper($field);
        if ($this->corrente[$field] !== $value && (!$init || $this->zero($field))) {
            $this->corrente[$field] = $value;
            if (!$init) {
                $this->modificato = true;
                }
            // __________________________________ Reset cached descriptions for combos ___
            if (isset($this->corrente[$field."_jxcbdesc"])) {
                unset($this->corrente[$field."_jxcbdesc"]);
                }
            $this->comunica_variazioni();
            }
        if ($init) {
            $this->precedente[$field] = $this->corrente[$field];
            }

        }


    /**
     * Ritorna il valore di $field per il record corrente
     *
     * @param  string $field
     * @return mix
     */
    function campo($field) {

        $app   = $_SESSION['o2_app'];
        $prg   = $app->istanze_prg[$this->id_esecuzione];
        $field = strtoupper($field);
        foreach ($prg->contesto as &$view) {
            if (is_a($view, 'o2_dbview') && count($view->aggregate)) {
                if ($view->auto_aggr) {
                    foreach ($view->aggregate as $aggr) {
                        // ___________ If field is an auto-aggragation target variable ___
                        if ($aggr['retfield'] == $field) {
                            if (!$view->vista) {
                                $view->record_primo();
                                }
                            // __ Bug compatibility tip: evaluate aggregations targets ___
                            elseif ($app->bug_aggregate) {
                                /**
                                 * This complex logic is needed to grant the right
                                 * behaviour for automatic aggregations in tables from
                                 * sub-views.
                                 * Change it only if you know what you're doing!
                                 */
                                $uw = $view->ultima_where[$view->file->indice];
                                $ws = $view->wheres[$view->file->indice];
                                if ($view->range2where($view->file->indice)) {
                                    $view->refresh();
                                    }
                                $view->calcola_aggregate();
                                $view->wheres[$view->file->indice]       = $ws;
                                $view->ultima_where[$view->file->indice] = $uw;
                                /********************************************************/
                                }
                            }
                        }
                    }
                }
            }
        if ($prg->evaluating_view) {
            $this->dipendenze[$prg->evaluating_view] = false;
            }
        return (!is_null($this->corrente[$field]) ? $this->corrente[$field] :
                (isset($this->variabili[$field]->maschera->default) ?
                 $this->variabili[$field]->maschera->default :
                 ""));

        }


    /**
     * Ritorna il valore di $field precedente alle modifiche
     *
     * @param  string $field
     * @return mix
     */
    function precedente($field) {

        $field = strtoupper($field);
        return (!is_null($this->precedente[$field]) ?
                $this->precedente[$field] :
                $this->campi[$field]->maschera->default);

        }


    /**
     * Returns boolean value true/false if field value is equal/different from its own
     * default value
     *
     * @param  string $field_name
     * @return boolean
     */
    function zero($field_name) {

        $field_local   = $this->variabili[$field_name];
        $default_local = $field_local->maschera->default;
        $val_local     = $this->campo($field_name);
        $ret_local     = false;
        switch ($field_local->maschera->tipo) {
            case "S":
                $ret_local = ($val_local === $default_local);
                break;
            case "N":
                $ret_local = (floatval($val_local) === floatval($default_local));
                break;
            case "L":
                $ret_local = (!($val_local) === !($default_local));
                break;
            default:
                $ret_local = (trim($val_local) === trim($default_local));
                break;
            }
        return $ret_local;

        }

    }


/**
 * Menu item object
 *
 */
class o2_voce_menu {

    /*     ===== PROPERTIES =========================================================== */
    public $nome     = "";   /* Stringa che rappresenta il nome univoco della voce      */
    public $label    = "";   /* Stringa che rappresenta la label della voce             */
    public $ico      = "";   /* Nome del file icona da visualizzare                     */
    public $tipo     = "S";  /* Un carattere che rappresenta la tipologia della voce:
                                   [M]enu: apre un sotto-menu
                                   [P]rogramma: chiama ('o2c') il programma in [data]
                                   [U]rl: rappresenta un link per l'url in [data]
                                   [A]zione: esegue l'azione specificata in [data]
                                   [S]eparatore: separatore o titolo ($label <> "")     */
    public $data     = "";   /* Stringa del parametro previsto dalla tipologia          */
    public $padre    = "";   /* Nome univoco della voce-menu che contiene l'elemento    */
    public $visibile = true; /* Booleano che indica la visibilità della voce di menu    */
    public $attivo   = true; /* Booleano che indica se la voce di menu è attiva         */


    /**
     * Costruttore
     *
     * @param  string  $padre
     * @param  string  $tipo
     * @param  string  $nome
     * @param  string  $label
     * @param  string  $data
     * @param  boolean $visibile
     * @param  boolean $attivo
     * @return o2_voce_menu
     */
    function __construct($padre,
                         $tipo     = "S",
                         $nome     = "",
                         $label    = "",
                         $data     = "",
                         $visibile = true,
                         $attivo   = true,
                         $ico      = "") {

        $tipo = $tipo[0];
        $tipo = strtoupper($tipo);
        if (strrpos("PUAMJ", $tipo) === false) {
            $tipo  = "S";
            $label = "";
            }
        $this->padre    = trim($padre);
        $this->tipo     = $tipo;
        $this->nome     = $nome;
        $this->label    = $label;
        $this->ico      = $ico;
        $this->data     = $data;
        $this->visibile = $visibile;
        $this->attivo   = $attivo;

        }


    /**
     * Search for a menu-item descending a menu recursively.
     * If item is found manu-item object is returned, FALSE otherwise.
     *
     * @param  string $item_name   Related o2_voce_menu object
     * @return o2_voce_menu
     */
    function get_subitem($item_name) {

        $menu = $_SESSION['o2_app']->menu;
        if ($this->nome == $item_name) {
            return $this;
            }
        elseif (isset($menu[$this->nome]) && count($menu[$this->nome]->voci)) {
            foreach ($menu[$this->nome]->voci as $sub_item) {
                if ($sub_item->nome == $item_name) {
                    return $sub_item;
                    }
                elseif ($sub_item->tipo == "M") {
                    if ($found = $sub_item->get_subitem($item_name)) {
                        return $found;
                        }
                    }
                }
            }
        return false;

        }

    }


/**
 * Menu bar object
 *
 */
class o2_menubar {

    /*     ===== PROPERTIES =========================================================== */
    public $nome = "";      /* Unique menu name                                         */
    public $ico  = "";      /* Icon file name                                           */
    public $voci = array(); /* Menu items list                                          */


    /**
     * Costruttore
     *
     * @param  string $nome
     * @param  string $ico
     * @return o2_menubar
     */
    function __construct($nome, $ico) {

        $this->nome = trim($nome);
        $this->ico  = $ico;
        $this->voci = array();

        }


    /**
     * Aggiunge una voce al menu
     *
     * @param string  $tipo
     * @param string  $nome
     * @param string  $label
     * @param string  $data
     * @param boolean $visibile
     * @param boolean $attivo
     * @param string  $ico
     * @param integer $position
     */
    function voce($tipo     = "S",
                  $nome     = "",
                  $label    = "",
                  $data     = "",
                  $visibile = true,
                  $attivo   = true,
                  $ico      = "",
                  $position = 0) {

        if ($position && $position < count($this->voci)) {
            array_splice($this->voci,
                         $position - 1,
                         0,
                         array(trim($nome) =>new o2_voce_menu($this->nome,
                                                              $tipo,
                                                              $nome,
                                                              $label,
                                                              $data,
                                                              $visibile,
                                                              $attivo,
                                                              $ico)));
            }
        else {
            $this->voci[trim($nome)] = new o2_voce_menu($this->nome,
                                                        $tipo,
                                                        $nome,
                                                        $label,
                                                        $data,
                                                        $visibile,
                                                        $attivo,
                                                        $ico);
            }

        }


    /**
     * Search for a menu-item descending a menu recursively.
     * If item is found manu-item object is returned, FALSE otherwise.
     *
     * @param  string $item_name   Related o2_voce_menu object
     * @return o2_voce_menu
     */
    function get_subitem($item_name) {

        $app = $_SESSION['o2_app'];
        if ($this->nome == $item_name) {
            return $this;
            }
        elseif (count($this->voci)) {
            foreach ($this->voci as $sub_item) {
                if ($sub_item->nome == $item_name) {
                    return $sub_item;
                    }
                elseif ($sub_item->tipo == "M") {
                    if ($found = $sub_item->get_subitem($item_name)) {
                        return $found;
                        }
                    }
                }
            }
        return false;

        }


}


/**
 * Resource reading/writing protocol object
 *
 */
class o2_protocollo {

    /*     ===== PROPERTIES =========================================================== */
    public $nome          = "";      /* Unique object name                              */
    public $prg           = "";      /* Name of program containing protocol             */
    public $id_esecuzione = 0;       /* Program execution id (PRG_EXE_ID)               */
    public $campi         = array(); /* Fields list                                     */
    public $tipo          = "XML";   /* Format (XML, FS, TEXT)                          */


    /**
     * Costruttore
     *
     * @param  string $prg
     * @param  string $id_esecuzione
     * @param  string $nome
     * @param  string $tipo
     * @return o2_protocollo
     */
    function __construct($prg, $id_esecuzione, $nome = "", $tipo = "XML") {

        $this->nome          = $nome."_protocollo";
        $this->prg           = $prg;
        $this->id_esecuzione = $id_esecuzione;
        $this->tipo          = $tipo;
        $def_local           = $prg."§§".$this->nome.'_def';
        // ________________________________ Execute report DEF to refresh all controls ___
        if (is_callable($def_local)) {
            $this->campi = array();
            $def_local($this);
            }

        }


    function usa($id, $view, $field, $mask = "") {

        $app        = $_SESSION['o2_app'];
        $field      = strtoupper($field);
        $view_field = $app->istanze_prg[$this->id_esecuzione]->contesto[$view]->
                       campo_per_controllo($field);
        if (!isset($this->campi[$id])) {
            $this->campi[$id] = new o2_ctrl();
            }
        $this->campi[$id]->nome          = $id;
        $this->campi[$id]->tipo_ctrl     = $view_field->tipo;
        $this->campi[$id]->prg           = $this->prg;
        $this->campi[$id]->id_esecuzione = $this->id_esecuzione;
        $this->campi[$id]->task          = $view;
        $this->campi[$id]->campo_in_ctrl = $field;
        $this->campi[$id]->form          = $this->nome;
        if (!$mask || !$app->maschere[$mask]) {
            $this->campi[$id]->maschera = $view_field->maschera;
            }
        else {
            $this->campi[$id]->maschera = $app->maschere[$mask];
            }
        $this->campi[$id]->larghezza = $this->campi[$id]->maschera->dimensione;
        $this->campi[$id]->valore    = $this->campi[$id]->maschera->default;

        }


    /**
     * Legge i valori del contesto corrente delle viste
     *
     */
    function leggi_valori_vista() {

        // ________________________________ Execute report DEF to refresh all controls ___
        $def_local = $this->prg."§§".$this->nome.'_def';
        if (is_callable($def_local)) {
            $def_local($this);
            }
        $prg = $_SESSION['o2_app']->istanze_prg[$this->id_esecuzione];
        foreach ($this->campi as &$field) {
            // ________________________________________________ If field (always TRUE) ___
            if ($field->campo_in_ctrl) {
                // __________________________________ Get report field value from view ___
                $field->valore = $prg->contesto[$field->task]->
                                                             campo($field->campo_in_ctrl);
                }
            }

        }


    /**
     * Crea le dichiarazioni (<metadata>) XML per il report
     *
     * @return string
     */
    function xml_metadata() {

        $rpc      = $_SESSION['o2_app']->rpc_server;
        $metadata.= ($rpc ? '' : "<METADATA>\n<FIELDS>\n");
        // _________________________________________________ Remove "_protocol" suffix ___
        $name     = substr($this->nome, 0, -11);
        foreach ($this->campi as &$field) {
            // _________________ If report field in binded to view field (always true) ___
            if ($field->campo_in_ctrl) {
                $fieldtype = "";
                $width     = "";
                // _______________________________________ Add single field definition ___
                switch ($field->maschera->tipo) {
                    case "N":
                        $fieldtype = "R8";
                        break;
                    case "L":
                        $fieldtype = "string";
                        $width     = "1";
                        break;
                    case "D":
                        $fieldtype = "date";
                        break;
                    case "O":
                        $fieldtype = "time";
                        break;
                    default:
                        $fieldtype = "string";
                        $width     = $field->maschera->dimensione;
                        break;
                    }
                if ($rpc) {
                    $metadata.= "<FIELD _src=\"".$name.
                                "\" attrname=\"".$field->nome.
                                "\" fieldtype=\"".$fieldtype."\"".
                                ($width ? " width=\"".$width."\"" : "")."/>\n";
                    }
                else {
                    $metadata.= "<FIELD attrname=\"".$field->nome.
                                "\" fieldtype=\"".$fieldtype."\"".
                                ($width ? " width=\"".$width."\"" : "")."/>\n";
                    }
                }
            }
        $metadata.= ($rpc ? '' : "</FIELDS>\n</METADATA>\n");
        return $metadata;

        }


    /**
     * Crea la riga di dati (<rowdata>) XML per il report
     *
     * @return string
     */
    function xml_rowdata() {

        $app     = $_SESSION['o2_app'];
        // _________________________________________________ Remove "_protocol" suffix ___
        $name    = substr($this->nome, 0, -11);
        $rowdata = "<ROW ".($app->rpc_server ? ' _src="'.$name.'" ' : '');
        $this->leggi_valori_vista();
        foreach ($this->campi as &$field) {
            // _________________ If report field in binded to view field (always true) ___
            if ($field->campo_in_ctrl) {
                $val = $field->ctrl2sql();
                // ________________________________ Add single field data for this row ___
                if ($field->maschera->tipo == "O") {
                    $rowdata.= $field->nome."=\"".$val."\" ";
                    }
                elseif ($field->maschera->tipo == "D") {
                    $rowdata.= $field->nome."=\"".($val != "00000000" ? $val : "")."\" ";
                    }
                else {
                    $rowdata.= $field->nome."=\"".
                               str_replace("\r", '',
                                str_replace("\n", '&#10;',
                                            htmlspecialchars(rtrim($val),
                                                             ENT_COMPAT | ENT_HTML5,
                                                             $app->chr_encoding))).
                               "\" ";


                    }
                }
            }
        $rowdata.= "/>\n";
        return $rowdata;

        }


    function leggi_fs($risorsa_fs) {

        $dir_local = new o2_dir($risorsa_fs);
        return $dir_local->all_elements();

        }


    /**
     * Comunica secondo il protocollo sulla risorsa indicata
     *
     * @param string  $risorsa
     * @param boolean $init
     */
    function io($risorsa, $init = false) {

        $app           = $_SESSION['o2_app'];
        $risorsa_local = $app->istanze_prg[$this->id_esecuzione]->
                          risorse[$risorsa];
        // ==================================================================== IMPORT ===
        if ($risorsa_local->direzione == "I") {
            switch ($this->tipo) {
                case "FS":
                    if (!$init) {
                        $this->leggi_fs($risorsa_local->risorsa);
                        }
                    break;
                default:
                    throw new o2_exception("<b>ILLEGAL PROTOCOL TYPE</b> for protocol ".
                                           "<i>".$this->nome."</i> ".
                                           "(type <i>".$this->tipo."</i>) ".
                                           "used on resource <i>".$risorsa."</i>",
                              o2error_IO);
                    break;
                }
            }
        // ==================================================================== EXPORT ===
        else {
            switch ($this->tipo) {
                case "XML":
                    // ____________________________________ Stream XML for WEB service ___
                    if ($_SESSION['o2_app']->rpc_server) {
                        $data_local                 = "";
                        $risorsa_local->terminatore = "</DATAPACKET>\n";
                        // ___________________________ If resource is NOT already open ___
                        if (!$risorsa_local->id) {
                            if (!$app->xmlstream_head) {
                                ob_clean();
                                ob_implicit_flush(true);
                                header("Content-Type: text/xml");
                                $data_local = '<?xml version="1.0" encoding="'.
                                              $app->chr_encoding."\"?>\n<DATAPACKET>\n";
                                $app->xmlstream_head = true;
                                $app->xmlstream_foot = false;
                                }
                            $data_local.= $this->xml_metadata();
                            $risorsa_local->apri();
                            }
                        if (!$init) {
                            $data_local.= $this->xml_rowdata();
                            }
                        }
                    else {
                        $data_local                 = "";
                        $risorsa_local->terminatore = "</ROWDATA>\n</DATAPACKET>\n";
                        // ___________________________ If resource is NOT already open ___
                        if (!$risorsa_local->id) {
                            $data_local = "<?xml version='1.0' standalone='yes' ?".
                                          ">\n<DATAPACKET version='2.0'>\n".
                                          $this->xml_metadata()."<ROWDATA>\n";
                            $risorsa_local->apri();
                            }
                        if (!$init) {
                            $data_local.= $this->xml_rowdata();
                            }
                        }
                    $risorsa_local->buffered_write($data_local);
                    break;
                default:
                    throw new o2_exception("<b>ILLEGAL PROTOCOL TYPE</b> for protocol ".
                                           "<i>".$this->nome."</i> ".
                                           "(type <i>".$this->tipo."</i>) ".
                                           "used on resource <i>".$risorsa."</i>",
                              o2error_IO);
                    break;
                }
            }

        }

    }


/**
 * Filesystem or other type resource object
 *
 */
class o2_risorsa {

    /*     ===== PROPERTIES =========================================================== */
    public $nome          = "";    /* Unique object id                                  */
    public $prg           = "";    /* Name of program containing resource               */
    public $id_esecuzione = 0;     /* Program execution id (PRG_EXE_ID)                 */
    public $tipo          = "F";   /* Type: [F]ilesystem [S]pool_for_print              */
    public $direzione     = "O";   /* Direction: [I]n (import) or [O]ut (export)        */
    public $risorsa       = "";    /* Path to resource file                             */
    public $id            = 0;     /* Handle to resource when opened                    */
    public $terminatore   = "";    /* Ending string (set by protocol)                   */
    public $inerited      = false; /* If resource is inerited from a previous prg       */
    /*     _____ Buffering ____________________________________________________________ */
    public $buffer_len    = 65535; /* Max buffer len, after which to write on disk      */
    public $buffer_txt    = "";    /* Buffer text                                       */


    /**
     * Costruttore
     *
     * @param  string $prg
     * @param  string $id_esecuzione
     * @param  string $nome
     * @param  string $risorsa
     * @param  string $direzione
     * @return o2_risorsa
     */
    function __construct($prg,
                         $id_esecuzione,
                         $nome      = "",
                         $tipo      = "F",
                         $risorsa   = "",
                         $direzione = "O") {


        $this->nome          = $nome;
        $this->prg           = $prg;
        $this->id_esecuzione = $id_esecuzione;
        $this->inerited      = false;
        // _____________________________ Look for an open resource in running programs ___
        $app = $_SESSION['o2_app'];
        for ($prg_id = 1; $prg_id < $this->id_esecuzione; $prg_id++) {
            if (isset($app->istanze_prg[$prg_id]->risorse[$nome])) {
                $resource         = $app->istanze_prg[$prg_id]->risorse[$nome];
                $this->risorsa    = $resource->risorsa;
                $this->id         = $resource->id;
                $this->tipo       = $resource->tipo;
                $this->direzione  = $resource->direzione;
                $this->buffer_txt = $resource->buffer_txt;
                $this->inerited   = $prg_id;
                }
            }
        if (!$this->inerited) {
            $this->tipo      = $tipo;
            $this->risorsa   = ($direzione == "O" ? $app->dir_tmp : "").$risorsa;
            $this->direzione = $direzione;
            }

        }


    /**
     * Apre il canale di importazione/esportazione da/verso la risorsa indicata in
     * this->risorsa
     */
    function apri() {

        $app = $_SESSION['o2_app'];
        if ($this->inerited) {
            $resource = $app->istanze_prg[$this->inerited]->risorse[$this->nome];
            if (!$resource->id) {
                $resource->apri();
                }
            $this->id              = $resource->id;
            $resource->terminatore = $this->terminatore;
            if ($this->direzione == "O") {
                $app->produzioni[$this->prg][$this->nome][] = $this->risorsa;
                }
            }
        // ________________________________________________ Stream XML for WEB service ___
        elseif ($app->rpc_server) {
            if ($this->direzione == "O") {
                $app->produzioni[$this->prg][$this->nome][] = $this->risorsa;
                }
            $this->id = $app->runtime->microtime();
            }
        else {
            $folder_local = new o2_dir(dirname($this->risorsa));
            if (!is_dir(dirname($this->risorsa))                 ||
                ($this->direzione == "O"                         &&
                 $folder_local->exists(basename($this->risorsa)) &&
                 !unlink($this->risorsa))                        ||
                ($this->direzione == "I"                         &&
                 !$folder_local->exists(basename($this->risorsa)))) {
                throw new o2_exception("Cannot open file <i>".$this->risorsa."</i>",
                                       o2error_IO);
                }
            if ($this->direzione == "O") {
                $dir_local = "w";
                $app->produzioni[$this->prg][$this->nome][] = $this->risorsa;
                }
            else {
                $dir_local = "r";
                }
            $this->id = fopen($this->risorsa, $dir_local);
            stream_set_write_buffer($this->id, 0);
            }

        }


    /**
     * Chiude la risorsa se aperta
     *
     */
    function chiudi() {

        $app = $_SESSION['o2_app'];
        if ($app->rpc_server) {
            $this->id = 0;
            }
        elseif ($this->id && !$this->inerited) {
            if ($this->direzione == "O") {
                $this->scrivi($this->buffer_txt.$this->terminatore);
                $this->buffer_txt = "";
                }
            fclose($this->id);
            $this->id = 0;
            }
        elseif ($this->inerited) {
            $app->istanze_prg[$this->inerited]->risorse[$this->nome]->buffer_txt =
                                                                        $this->buffer_txt;
            }

        }


    /**
     * Scrive $data sul canale della risorsa
     *
     */
    function scrivi($data = "") {

        if (fwrite($this->id, $data) === false) {
            throw new o2_exception("Error writing to file <i>".$this->risorsa."</i>",
                                   o2error_IO);
            }

        }


    /**
     * Writes $data in buffer linked to resource.
     * Buffer will be downloaded to disk when reaching ->buffer_len limit.
     *
     */
    function buffered_write($data = "") {

        $app = $_SESSION['o2_app'];
        // ________________________________________________ Stream XML per WEB service ___
        if ($app->rpc_server) {
            print $data;
            }
        else {
            $this->buffer_txt.= $data;
            if (strlen($this->buffer_txt) >= $this->buffer_len) {
                list($data2write, $this->buffer_txt) = str_split($this->buffer_txt,
                                                                 $this->buffer_len);
                $this->scrivi($data2write);
                }
            }

        }

    }


/**
 * Documentor object
 *
 */
class o2_doc {

    public $root  = "";
    public $alias = "";


    function __construct() {

        $rnt         = $GLOBALS['o2_runtime'];
        $this->root  = $rnt->root."doc".DIRECTORY_SEPARATOR;
        $this->alias = $rnt->alias."doc/";
        $this->repository();

        }


    function repository() {

        @include_once($this->root."prgs/doc.models.inc");
        @include_once($this->root."prgs/doc.dbs.inc");
        $GLOBALS['o2_runtime']->read_sys_tabs = true;
        @include_once($this->root."prgs/doc.tables.inc");
        $GLOBALS['o2_runtime']->read_sys_tabs = false;

        }


    function prg($prg_name) {

        @include($this->root."prgs/".$prg_name.".prg");

        }


    function prf($prg_name) {

        @include_once($this->root."prgs/".$prg_name.".prf");

        }


    /**
     * Shows documentation for requested node
     *
     * @param  string  $prg_name
     * @param  string  $form_name
     * @param  string  $ctrl_name
     * @param  integer $node_id
     * @return boolean
     */
    function show_node($prg_name  = "",
                       $form_name = "",
                       $ctrl_name = "",
                       $node_id   = 0) {

        $parent_name = "";
        $parent_info = "";
        // __________________________________________________ Check request parameters ___
        if (!$prg_name && !$node_id) {
            throw new o2_exception("Wrong documentation request: ".
                                   "missing Program or NodeID.",
                                   o2error_UNKNOWNPRG);
            }
        // ___________________________________________________ Check requested program ___
        elseif ($prg_name) {
            $app = $_SESSION['o2_app'];
            $prg = $app->istanze_prg[$app->progressivo_istanze];
            if ($prg->nome != $prg_name) {
                throw new o2_exception("Wrong documentation request: ".
                                       "unknown program <i>".$prg_name."</i>.",
                                       o2error_UNKNOWNPRG);
                }
            // ________________________________________________ Check requested window ___
            elseif ($form_name) {
                if ($prg->form[$form_name]) {
                    $form = $prg->form[$form_name];
                    // _______________________________________ Check requested control ___
                    if ($ctrl_name) {
                        if ($form->controlli[$ctrl_name]) {
                            $ctrl = $form->controlli[$ctrl_name];
                            // ___________________ Look for control parent (container) ___
                            $parent_name = $ctrl->padre;
                            if ($parent_name) {
                                $parent = $form->controlli[$parent_name];
                                if ($parent->tipo_ctrl == "multipage") {
                                    $parent_page = $ctrl->info_padre[0] + 1;
                                    $parent_info = $parent->label[$ctrl->info_padre[0]];
                                    }
                                // _ Look for control grand-parent (3^level container) ___
                                $gparent_name = $parent->padre;
                                if ($gparent_name &&
                                    $form->controlli[$gparent_name]->
                                     tipo_ctrl == "multipage") {
                                    $gparent      = $form->controlli[$gparent_name];
                                    $gparent_page = $parent->info_padre[0] + 1;
                                    $gparent_info = $gparent->
                                                     label[$parent->info_padre[0]];
                                    }

                                }
                            }
                        else {
                            throw new o2_exception("Wrong documentation request: ".
                                                   "unknown control <i>".$ctrl_name.
                                                   "</i> in window <i>".$form_name.
                                                   "</i>.",
                                                   o2error_UNKNOWNFORM);
                            }


                        }
                    }
                else {
                    throw new o2_exception("Wrong documentation request: ".
                                           "unknown window <i>".$form_name.
                                           "</i> in program <i>".$prg_name."</i>.",
                                           o2error_UNKNOWNFORM);
                    }
                }
            }
        $_SESSION['o2_app']->intcall("doc/jxdoc_start",
                                     $node_id,
                                     $prg_name,
                                     $form_name,
                                     $ctrl_name,
                                     $parent_name,
                                     $parent_info,
                                     $gparent_name,
                                     $gparent_info);
        return true;

        }

    }


// ========================= Janox istructions collection ================================
/**
 * Collects methods for instructions execution
 *
 */
class o2act {


    // ================================ DEBUGGING ========================================
    /**
     * Esegue la procedura di debugging
     *
     */
    static function debug() {

        return o2log();

        }


    // ================================ RIGHE DI COMMENTO ================================
    /**
     * Definisce una riga di commento. Questo metodo non fa niente ed esiste per supporto
     * all'interfaccia di sviluppo.
     *
     * @param string $commento
     */
    static function remark($commento = "") {

        return true;

        }

    // ============================= ESECUZIONE DI CODICE PHP ============================
    /**
     * Definisce una espressione PHP da valutare all'esecuzione del metodo. Questo metodo
     * non fa niente ed esiste per supporto all'interfaccia di sviluppo.
     *
     * @param mix $espressione
     */
    static function script($espressione = "") {

        return true;

        }


    // ============================= ESECUZIONE DELLE CHIAMATE ===========================
    /**
     * Reindirizza il browser su un altro url
     *
     * @param  string $url
     * @return boolean
     */
    static function gotourl($url) {

        $app = $_SESSION['o2_app'];
        $app->commit_all(true);
        // _________________________________________________________________ Full-AJAX ___
        if ($GLOBALS['jxjs']) {
            print "window.location = '".$url.
                  "';\no2jse.setExeId(".$app->progressivo_istanze.");\n}\n";
            }
        // _____________________________________________________________ Standard HTML ___
        else {
            print "<script> window.location = '".$url."';  </script>";
            }
        die();

        }


    /**
     * Passa il controllo ad un'altra pagina PHP. Accetta infiniti parametri.
     *
     * @param  string  $program
     * @param  boolean $deferred
     * @return boolean
     */
    static function gotoprg($program, $deferred = false) {

        $app = $_SESSION['o2_app'];
        // _________________________________ Goto is not allowed inside jobs execution ___
        if (isset($_REQUEST[o2_run_job::$run_word]) ||
            isset($_REQUEST[o2_run_job::$job_word]) ||
            isset($_REQUEST[o2_scheduler::$sched_word])) {
            throw new o2_exception("GOTO-PRG to <i>".$program.
                                   "</i> not allowed inside job execution.",
                                   o2error_EXECUTION);
            return;
            }
        $pars   = func_get_args();
        $pars_n = func_num_args() - 2;
        array_shift($pars);
        array_shift($pars);
        $o2pars = array();
        foreach ($pars as $i => $single_par) {
            $o2pars[$i]['tipo']   = 'E';
            $o2pars[$i]['task']   = '';
            $o2pars[$i]['campo']  = '';
            $o2pars[$i]['valore'] = $single_par;
            }
        if ($app->extcall_level) {
            for ($exe_id = $app->progressivo_istanze; $app->extcall_level; $exe_id--) {
                $app->termina_prg($exe_id);
                }
            array_unshift($pars, $program);
            call_user_func_array(array($app, 'intcall'), $pars);
            }
        else {
            // ________________________________ Reset calls list and create first call ___
            $app->chiamate = array(1 => array('id'          => 1,
                                              'prg'         => trim($program),
                                              'parametri_n' => $pars_n,
                                              'parametri'   => $o2pars));
            switch ($app->runtime->interface) {
                case "HTML":
                    ob_clean();
                    $_SESSION['jxredirect'] = $program;
                    // _____________________________________________________ Full-AJAX ___
                    if ($GLOBALS['jxjs'] || $GLOBALS['jxjsorigin']) {
                        jxjs::page();
                        }
                    // _________________________________________________ Standard HTML ___
                    else {
                        o2html::page(true);
                        die();
                        }
                    break;
                default:
                    $_SESSION['jxredirect'] = $program;
                    o2html::page(true);
                    die();
                    break;
                }
            }
        return true;

        }


    /**
     * Provides the CALL-PROGRAM command for Janox executions syntax
     *
     * @param  string  $programma
     * @param  boolean $cache_mode
     * @return boolean
     */
    static function call($programma, $cache_mode = false) {

        $app     = $_SESSION['o2_app'];
        $next_id = ($app->progressivo_istanze + 1);
        // ______________________________________ Se ritorno dalla fine della chiamata ___
        if ($app->ritorno) {
            /* Se esiste una chiamata conclusa la termino, aggiorno i parametri di
               ritorno e concludo il passo                                              */
            $chiamata_local = $app->chiamate[$next_id];
            if ($chiamata_local && $chiamata_local['fine']) {
                foreach ($chiamata_local['parametri'] as $singolo_parametro) {
                    if ($singolo_parametro['tipo'] == "V") {
                        $view_local = $app->istanze_prg[$app->progressivo_istanze]->
                                       contesto[$singolo_parametro['task']];
                        if ($view_local->campo($singolo_parametro['campo']) !==
                            $singolo_parametro['valore']) {
                            $view_local->imposta($singolo_parametro['campo'],
                                                 $singolo_parametro['valore']);
                            }
                        }
                    }
                unset($app->chiamate[$chiamata_local['id']]);
                return true;
                }
            else {
                return false;
                }
            }
        // ____________________________________________________________ Se da eseguire ___
        else {
            // ____________________ Creo la chiamata nell'elenco delle chiamate attive ___
            if (!isset($app->chiamate[$next_id])) {
                // ______________________________________________ Recupero i parametri ___
                $parametri   = func_get_args();
                $parametri_n = func_num_args() - 2;
                array_shift($parametri);
                array_shift($parametri);
                $format_pars = array();
                foreach ($parametri as $i => $single_par) {
                    if (!is_string($single_par) || strpos($single_par, "§§") === false) {
                        $format_pars[$i]['tipo']   = "E";
                        $format_pars[$i]['task']   = "";
                        $format_pars[$i]['campo']  = "";
                        $format_pars[$i]['valore'] = $single_par;
                        }
                    else {
                        list($view_name, $campo_local) = explode("§§", $single_par);
                        $format_pars[$i]['tipo']   = "V";
                        $format_pars[$i]['task']   = $view_name;
                        $format_pars[$i]['campo']  = strtoupper($campo_local);
                        $value_local = $app->istanze_prg[$app->progressivo_istanze]->
                                        contesto[$view_name]->campo($campo_local);
                        $format_pars[$i]['valore'] = $value_local;
                        }
                    }
                $app->chiamate[$next_id] = array('id'          => $next_id,
                                                 'prg'         => trim($programma),
                                                 'parametri_n' => $parametri_n,
                                                 'parametri'   => $format_pars,
                                                 'fine'        => false);
                }
            $chiamata_local = &$app->chiamate[$next_id];
            // ____________________________________________________ Esecuzione del prg ___
            if ($cache_mode                         &&
                isset($app->prgs_cache[$programma]) &&
                $app->prgs_cache[$programma]->id === $next_id) {
                $app->includi_prf($programma, $app->prgs_cache[$programma]->script);
                $app->istanze_prg[$next_id] = clone $app->prgs_cache[$programma];
                $app->progressivo_istanze++;
                }
            else {
                if ($cache_mode) {
                    $app->prgs_cache[$programma] = true;
                    }
                $app->includi_prf($programma);
                $app->includi_prg($programma);
                }
            $prg = $app->istanze_prg[$next_id];
            if ($prg->esecutivo) {
                $prg->esegui_azione($prg->esecutivo, true);
                }
            if ($prg && $prg->concluso()) {
                $app->termina_prg();
                foreach ($chiamata_local['parametri'] as $single_par) {
                    if ($single_par['tipo'] == "V") {
                        $view = $app->istanze_prg[$app->progressivo_istanze]->
                                 contesto[$single_par['task']];
                        if ($view->campo($single_par['campo']) != $single_par['valore']) {
                            $view->imposta($single_par['campo'], $single_par['valore']);
                            }
                        }
                    }
                unset($app->chiamate[$chiamata_local['id']]);
                return true;
                }
            else {
                return false;
                }
            }

        }


    /**
     * Conclude l'esecuzione del prg corrente
     *
     * @return boolean
     */
    static function close() {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->esci();
        return true;

        }


    // ========================= ESECUZIONE DELLE AZIONI ===============================

    /**
     * Passa l'esecuzione all'azione [$action] fino al verificarsi di [$end_cond].
     * Se [$loop_view] è il nome di una vista del prg allora l'azione viene eseguita
     * dall'inizio del main file fino alla fine (se [$direction] = "A") o viceversa (se
     * [$direction] != "A") o fino a che la valutazione dell'espressione [$end_cond] non
     * risulta vera
     *
     * @param  string $action
     * @param  string $end_cond
     * @param  string $loop_view
     * @param  string $direction
     * @return boolean
     */
    static function exe($action, $end_cond = true, $loop_view = "") {

        $app = $_SESSION['o2_app'];
        return $app->istanze_prg[$app->progressivo_istanze]->esegui_azione($action,
                                                                           $end_cond,
                                                                           $loop_view);

        }


    /**
     * Imposta il menu [$nome] come menu di livello 0 per la pagina
     *
     * @param  string $nome
     * @return boolean
     */
    static function menu($nome) {

        $app = $_SESSION['o2_app'];
        $app->attiva_menu($nome);
        return true;

        }


    /**
     * Visualizza [$text] come alert, come testo della barra di stato o come pupup
     *
     * @param  string  $text
     * @param  integer $media
     * @return boolean
     */
    static function alert($text, $media = 0) {

        $app = $_SESSION['o2_app'];
        // ______________________________________ CMD-line and batch executions (jobs) ___
        if ($app->runtime->interface != "HTML") {
            // __________________________________________ Jobs and scheduler processes ___
            if (isset($_REQUEST[o2_run_job::$run_word]) ||
                isset($_REQUEST[o2_run_job::$job_word]) ||
                isset($_REQUEST[o2_scheduler::$sched_word])) {
                if ($media === 1) {
                    $type = 'a status-bar text';
                    }
                elseif ($media === 2) {
                    $type = 'a popup';
                    }
                else {
                    $type = 'an alert-box';
                    }
                o2_exception::warning("Batch execution emitted ".$type.
                                      " with message:<br>".$text);
                }
            // _______________________________________ CMD-line (no jobs or scheduler) ___
            else {
                print "\n".$text."\n";
                }
            return true;
            }
        // ________________________________________________________________ Status bar ___
        elseif ($media === 1) {
            if ($GLOBALS['jxjs']) {
                jxjs::status_msg($text);
                }
            else {
                $app->statusbar_text = $text;
                }
            return true;
            }
        // _____________________________________________________________________ Popup ___
        elseif ($media === 2) {
            if ($GLOBALS['jxjs']) {
                jxjs::fast_msg($text);
                }
            else {
                o2html::fast_msg($text);
                }
            return true;
            }
        // _______________________________________________________________ Message box ___
        else {
            return o2act::call("tools/o2sys_alert", false, $text);
            }

        }


    /**
     * Richiede conferma al messaggio $testo e ritorna il valore nella variabile $field
     * della vista $view.
     *
     * @param  string $view
     * @param  string $field
     * @param  string $testo
     * @return boolean
     */
    static function confirm($view, $field, $testo) {

        $app = $_SESSION['o2_app'];
        return o2act::call("tools/o2sys_confirm", false, $testo, $view."§§".$field);

        }


    /**
     * Set $field in $view to $value
     *
     * @param  string $view
     * @param  string $field
     * @param  string $value
     * @return boolean
     */
    static function set($view, $field, $value) {

        $app = $_SESSION['o2_app'];
        if ($view == "_o2SESSION") {
            if (array_key_exists($field, $app->vars)) {
                $dummy_ctrl                = new o2_ctrl();
                $dummy_ctrl->maschera      = $app->vars[$field]->maschera;
                $dummy_ctrl->valore        = $value;
                $app->vars[$field]->valore = $dummy_ctrl->ctrl2sql();
                if ($field == '_area') {
                    $app->set_aspid();
                    }
                }
            else {
                throw new o2_exception("Unknown application variable <i>".$field.
                                       "</i> in assignment",
                                       o2error_UNKNOWNAPPVAR);
                }
            }
        else {
            $field     = strtoupper($field);
            $prg_local = $app->istanze_prg[$app->progressivo_istanze];
            $prg_local->contesto[$view]->imposta($field, $value);
            }
        return true;

        }


    /**
     * Imposta il parametro di indice [$id_parametro] a [$valore]
     *
     * @param  integer $id_parametro
     * @param  string  $valore
     * @return boolean
     */
    static function ret($id_parametro, $valore) {

        $call_local = $_SESSION['o2_app']->
                       chiamate[$_SESSION['o2_app']->progressivo_istanze]
                       ['parametri'][$id_parametro - 1]['valore'] = $valore;
        return true;

        }


    /**
     * Esegue il [comando] passato sul recordset di [task]
     *  ""  refresh (ricreo tutto il recordset)
     *  "1" primo record
     *  "2" record precedente
     *  "3" prossimo record
     *  "4" ultimo record
     *  "S" scrittura di corrente
     *  "M" attiva la modalità edit per i controlli del task
     *  "R" disattiva la modalità edit per i controlli del task
     *  "L" Posizionamento (locate) nel recordset per criteri di campo
     *  "A" annulla modifiche (ricarica corrente da precedente)
     *  "I" prepara inserimento (presenta record vuoto fuori vista. Necessita azione di
     *      scrittura)
     *  "C" cancellazione (esegue scrittura)
     *
     * @param  string $task
     * @param  string $comando
     * @param  array  $argomenti
     * @return boolean
     */
    static function view($task, $comando = "", $argomenti = "") {

        $app = $_SESSION['o2_app'];
        $prg = $app->istanze_prg[$app->progressivo_istanze];
        if (isset($prg->contesto[$task])) {
            $view = $prg->contesto[$task];
            }
        else {
            $view_by_model = "v".substr(md5($task."_o2list"), -5);
            if (isset($prg->contesto[$view_by_model])) {
                $view = $prg->contesto[$view_by_model];
                }
            }
        if ($view) {
            switch ($comando) {
                case "1": // _______________________________________ Goto first record ___
                    return $view->record_primo();
                    break;
                case "2": // ____________________________________ Goto previous record ___
                    return $view->record_indietro();
                    break;
                case "3": // ________________________________________ Goto next record ___
                    return $view->record_avanti();
                    break;
                case "4": // ________________________________________ Goto last record ___
                    return $view->record_ultimo();
                    break;
                case "5": // ______________________________________ Goto previous page ___
                    return $view->record_pg_indietro();
                    break;
                case "6": // __________________________________________ Goto next page ___
                    return $view->record_pg_avanti();
                    break;
                case "S": // ________________________________________ Action: POST ROW ___
                    $view->scrittore();
                    $view->direzione = 100;
                    $view->status    = "M";
                    break;
                case "M": // _____________ Action: EDIT (not used EDIT always enabled) ___
                    $view->status    = "M";
                    break;
                case "A": // ______ Action: UNDO (reload ->corrente from ->precedente) ___
                    if ($view->status == "I") {
                        $view->direzione = 100;
                        $view->crea_recordset();
                        $view->status = "M";
                        $view->seleziona_riga($view->selezione);
                        }
                    elseif ($view->modificato) {
                        $view->corrente   = $view->precedente;
                        $view->modificato = false;
                        }
                    break;
                case "I": // __________________________________ Action: PREPARE INSERT ___
                    if ($view->record_suffix()) {
                        $view->relocate_to_key = false;
                        if (!$view->vista ||
                            ($view->variato($view->file->indice) && $view->range_mod())) {
                            $view->record_primo();
                            }
                        if (!$view->recordset[0]) {
                            $view->recordset = array();
                            }
                        array_splice($view->recordset,
                                     $view->selezione + 1,
                                     0,
                                     array(array()));
                        if (count($view->recordset) > $view->righe_vis) {
                            if ($view->selezione == ($view->righe_vis - 1)) {
                                array_shift($view->recordset);
                                $view->status     = "I";
                                $view->modificato = false;
                                $view->seleziona_riga($view->selezione);
                                }
                            else {
                                array_pop($view->recordset);
                                $view->status     = "I";
                                $view->modificato = false;
                                $view->seleziona_riga($view->selezione + 1);
                                }
                            }
                        else {
                            $view->status     = "I";
                            $view->modificato = false;
                            $view->seleziona_riga($view->selezione + 1);
                            }
                        $view->relocated = true;
                        }
                    else {
                        return false;
                        }
                    break;
                case "C": // __________________________________________ Action: DELETE ___
                    if ($view->status == "I") {
                        $view->direzione = 100;
                        $view->crea_recordset();
                        $view->seleziona_riga($view->selezione);
                        }
                    else {
                        if ($view->variato($view->file->indice) && $view->range_mod()) {
                            $view->record_primo();
                            }
                        $view->status = "D";
                        $view->scrittore();
                        $view->vista = false;
                        }
                    $view->direzione  = 100;
                    $view->status     = "M";
                    $view->modificato = false;
                    break;
                case "L": // __________________________________________ Action: LOCATE ___
                    if ($view->record_suffix()) {
                        $view->locate($argomenti);
                        return true;
                        }
                    else {
                        return false;
                        }
                    break;
                case "D": // ________________________________________ Delete recordset ___
                    return $view->delete_recordset();
                    break;
                case "G": // ___________________________________ Evaluate aggregations ___
                    if ($view->range_mod()) {
                        $view->record_primo();
                        }
                    return $view->calcola_aggregate();
                    break;
                case "K": // __________________________________________ Lock recordset ___
                    return $view->record_primo(true);
                    break;
                case "X": // Action: RANGE-CTRL (not used see prg "tools/o2sys_tab_filter)
                    return $view->selettore();
                    break;
                default: // __________________________________________ Action: REFRESH ___
                    return $view->smart_refresh();
                    break;
                }
            }
        return true;

        }


    /**
     * Invia/riceve i dati del contesto corrente al/dal canale $risorsa secondo il
     * protocollo $protocollo
     *
     * @param  string  $protocol
     * @param  string  $risorsa
     * @param  boolean $init
     * @return boolean
     */
    static function io($protocol, $risorsa, $init = false) {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->protocolli[$protocol]->io($risorsa,
                                                                                $init);
        return true;

        }


    /**
     * "Impacchetta" i file prodotti in un processo di stampa in uno spool per la stampa
     * con printserver
     *
     * @param  string $modello
     * @param  string $file_pdf
     * @param  array  $info_print
     * @return boolean
     */
    static function report($modello, $file_pdf, $info_print, $printer_name = "") {

        $app = $_SESSION['o2_app'];
        $app->stampa($modello, $file_pdf, $info_print, $printer_name);
        return true;

        }

    }


/**
 * Returns a view recordset as an array in the form
 * array[$code_field] = $desc_field
 *
 * @param  string $view_name
 * @param  string $code_field
 * @param  string $desc_field
 * @param  array  $preset
 * @return array
 */
function o2_view2list($view_name, $code_field, $desc_field, $preset = array()) {

    $app          = $_SESSION['o2_app'];
    $prg_local    = $app->istanze_prg[$app->progressivo_istanze];
    $code_field   = strtoupper($code_field);
    $desc_field   = strtoupper($desc_field);
    $array_return = array();
    if (isset($prg_local->contesto[$view_name]) ||
        function_exists("_o2viewmodels§§".$view_name."_def")) {
        $new_view_name = $view_name."_o2list";
        $new_view_name = "v".substr(md5($new_view_name), -5);
        // __________________________________ If view has been created from view model ___
        if (isset($prg_local->contesto[$new_view_name])) {
            define($view_name."_o2self", $new_view_name);
            $view_local = $prg_local->contesto[$new_view_name];
            $code_field = strtoupper($new_view_name)."_".$code_field;
            $desc_field = strtoupper($new_view_name)."_".$desc_field;
            }
        // ________________________________ If view is actually defined in current prg ___
        elseif (isset($prg_local->contesto[$view_name])) {
            $view_local = $prg_local->contesto[$view_name];
            }
        // _______________________________________________ Else create view from model ___
        else {
            define($view_name."_o2self", $new_view_name);
            $view_local               = new o2_dbview($prg_local->nome,
                                                      $prg_local->id,
                                                      $new_view_name);
            $view_local->def_function = "_o2viewmodels§§".$view_name."_def";
            $prg_local->contesto[$new_view_name] = $view_local;
            $view_local->struttura();
            $code_field = strtoupper($new_view_name)."_".$code_field;
            $desc_field = strtoupper($new_view_name)."_".$desc_field;
            }
        $righe_vis_save = $view_local->righe_vis;
        if (!$app->defining_combo || $app->defining_combo->righe > 1) {
            $view_local->righe_vis = 100000;
            }
        $refreshed = false;
        // ________________________________________________ Redirect on lookup control ___
        if ($app->defining_combo) {
            $app->defining_combo->untraslate = true;
            // __________________________________________________________ Combo-lookup ___
            if ($app->defining_combo->righe < 2) {
                $app->defining_combo->items_view  = $view_name;
                $app->defining_combo->items_code  = $code_field;
                $app->defining_combo->items_desc  = $desc_field;
                $app->defining_combo->preset_list = $preset;
                unset($app->defining_combo);
                return array();
                }
            // _______________________________________________________________ Listbox ___
            else {
                unset($app->defining_combo);
                // _________________ If no dataset or WHERE clause changed for dataset ___
                if (!$view_local->vista ||
                    $view_local->range2where($view_local->file->indice) ||
                    $view_local->range2formule()) {
                    $view_local->record_primo();
                    $refreshed = true;
                    }
                }
            }
        else {
            $view_local->record_primo();
            $view_local->righe_vis = $righe_vis_save;
            }
        $array_return = array($view_local->campi[$code_field]->maschera->default => "") +
                        (is_array($preset) ? $preset : array());
        foreach ($view_local->recordset as $single_record) {
            $array_return[$single_record[$code_field]] = $single_record[$desc_field];
            }
        }
    else {
        throw new o2_exception("Unknown view <i>".$view_name.
                               "</i> for list loading in program <i>".$prg_local->nome.
                               "</i>",
                               o2error_UNKNOWNVIEW);
        }
    return $array_return;

    }


/**
 * Parses a file "ini style" to get a list of key-value pairs.
 * Use ";" or "#" to start a remark line.
 *
 * @param  string $conf_file
 * @return array
 */
function jx_parse_conf($conf_file) {

    $conf  = array();
    $lines = file($conf_file);
    foreach ($lines as $line) {
        $str_start = substr(trim($line), 0, 1);
        if ($str_start && $str_start != ";" && $str_start != "#") {
            $parts = array();
            preg_match_all('/\s*"?([^"]+)"?\s*=\s*"?([^"]+)"?/', $line, $parts);
            $conf[$parts[1][0]] = $parts[2][0];
            }
        }
    return $conf;

    }


/**
 * Execute a post request to provided URL.
 * $postdata is an array in the form array("key1" => "val1", "key2" => "val2", ...).
 *
 * @param  string $url        Address for request
 * @param  array  $postdata   Postdata fields to include in request
 * @return string             Response body
 */
function jx_post($url, $postdata) {

    $data = "";
    $bndr = "---------------------".substr(md5(rand(0, 32000)), 0, 10);
    foreach($postdata as $key => $val) {
        $data.= "--$bndr\n";
        $data.= "Content-Disposition: form-data; name=\"".$key."\"\n\n".$val."\n";
        }
    $data  .= "--$bndr\n";
    $params = array('http'    => array(
                    'method'  => 'POST',
                    'header'  => 'Content-Type: multipart/form-data; boundary='.$bndr,
                    'content' => $data));
    $ctx    = stream_context_create($params);
    $fp     = fopen($url, 'rb', false, $ctx);
    if (!$fp) {
        throw new o2_exception("Can't open stream ".$url."<br>".
                               htmlspecialchars($php_errormsg),
                               o2error_IO);
        }
    $response = @stream_get_contents($fp);
    if ($response === false) {
        throw new o2_exception("Can't read data from ".$url."<br>".
                               htmlspecialchars($php_errormsg),
                               o2error_IO);
        }
    return $response;

    }


/**
 * Logs out a dump of a passed object to the development console, with an execution stack
 * trace.
 *
 * @param mix $logobj   Object to dump out - can by any type (String, Obj, Array, ...)
 * @see   o2_timestart()
 * @see   o2_timeend()
 */
function jxlog($logobj = null) {

    $app = $_SESSION['o2_app'];
    if ($app->run_level != "DEV") {
        return;
        }
    // ________________________________________________________ Create micro-timestamp ___
    list($m, $s) = explode(' ', microtime());
    $tms = $s.str_pad($m * 1000000, 6, STR_PAD_LEFT);
    $tms = date("H:i:s.". substr($m, 2, -2));
    // __________________________________________________ Dump out parameter structure ___
    if ($logobj === null) {
        $logobj = $last_prg." -> ".$last_act;
        }
    $uid = "jxdbg".substr($tms, -10);
    // _____________________________________________________ Get programs/actions tree ___
    $last_prg = "";
    // ______________________________________________________________ Loop on programs ___
    foreach ($app->istanze_prg as $single_id => $single_prg) {
        $code.= " [".$single_id."] ".$single_prg->nome."\n";
        $last_prg = $single_prg->nome;
        // ____________________________________________ Loop on single program actions ___
        $last_act = "";
        $exe_code = "";
        foreach ($app->esecuzioni as $single_exe) {
            if ($single_exe->istanza_prg == $single_id) {
                $exe_code = ($last_act ? "  |- " : "  '- ").$single_exe->azione." : ".
                            $single_exe->passo."\n".$exe_code;
                $last_act = ($last_act ? $last_act : $single_exe->azione);
                }
            }
        $code.= $exe_code;
        }
    // ____________________________________________________________ Get functions tree ___
    foreach (debug_backtrace() as $func) {
        // ___________________________________________________ Stop on action function ___
        if ($func['function'] == $last_prg."§§".$last_act."_act") {
            break;
            }
        // ___________________________________________ Stop on internal execution call ___
        elseif ($func['class'] == "o2_esecuzione" && $func['function'] == "esegui") {
            break;
            }
        // ___________________________________________________ Last program expression ___
        elseif (substr($func['function'], 0, strlen($last_prg) + 5) ==
            $last_prg."_exp_") {
            $code.= "      '- EXP-".substr($func['function'],
                                             strlen($last_prg) + 5)."\n";
            }
        }
    // __________________________________________________________ Log out code to file ___
    if ($app->log2file || $app->mutelog || $app->rpc_server) {
        $log_file = $app->dir_logs.$app->developer.".log";
        if (!$f_local = fopen($log_file, 'a')) {
            throw new o2_exception("Cannot access log file <i>".$log_file."</i>",
                                   o2error_IO);
            }
        if (fwrite($f_local, $tms.": ".print_r($logobj, true)."\n".$code) === FALSE) {
            throw new o2_exception("Cannot write to log file <i>".$log_file."</i>",
                                   o2error_IO);
            }
        fclose($f_local);
        }
    if (!$app->mutelog && !$app->rpc_server) {
        if ($app->runtime->interface == "HTML") {
            // _____________________________________________________ Create detail DIV ___
            $code = "<div class='jxlogtrace' style='display:none;' id='".$uid."'>".$code.
                    "</div>";
            // _______________________________________________ Log out code on console ___
            $code = "<span class='jxlogactive' style='cursor:pointer;' ".
                    "onclick='o2jse.lab.ocd(this,\"".$uid."\");'>[+]</span> ".$tms.": ".
                    print_r($logobj, true).$code;
            $code = "o2jse.lab.getResult(null,'".addcslashes($code, "'\\\r\n")."');\n";
            if ($GLOBALS['jxjs']) {
                $_SESSION['o2_app']->js_cache.= $code."\n";
                }
            else {
                print "<script> ".$code."</script>\n";
                }
            }
        else {
            print $tms.": ".print_r($logobj, true)."\n";
            }
        }

    }

?>
