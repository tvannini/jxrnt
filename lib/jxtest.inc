<?php

/**
 * Janox Test Functions Library Module
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
 * This script contains functions callable from within Janox scripts and expressions:
 *  .: Retrieving objects lists
 *  .: Retrieving objects properties & informations
 *  .: Achieving particular goals (like files delivering, JavaScript interactions or
 *     browser caching)
 *
 *
 * @name      jxtest
 * @package   janox/jxtest.inc
 * @version   2.9
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * Define DB constants for delimiters
 */
define('o2__o', '');
define('o2__c', '');
define("o2_mssql_o", "[");
define("o2_mssql_c", "]");
define('o2_postgres_o', '"');
define('o2_postgres_c', '"');
define('o2_sqlite3_o', '`');
define('o2_sqlite3_c', '`');

/**
 * Define runtime as standard object
 */
$rnt = new stdClass();
$rnt->id              = 0;
$rnt->release         = "9.2";
$rnt->built           = "20240531";
$rnt->alias           = "/janox";
$rnt->interface       = "BAT";
$rnt->mswin           = false;
$rnt->last_microtime  = 0;
$rnt->microtime_incr  = 1;
$rnt->conf_list       = array();
$rnt->read_sys_tabs   = false;
$rnt->multisession    = true;
$rnt->decimals_point  = ".";
$rnt->thousands_point = ",";
$rnt->def_datachunk   = 65536;
$rnt->developers      = array();
$rnt->developer       = false;
$rnt->dev_key         = "";
$rnt->php_engine      = "php";
$rnt->rnt_script      = "";
$rnt->root            = "";
$rnt->dir_dbms        = "";
$rnt->dir_threads     = "";
$rnt->dir_css         = "";

/**
 * Define application as standard object
 */
$app = new stdClass();
$app->runtime             = $rnt;
$app->nome                = '';
$app->alias               = '';
$app->referer             = '';
$app->versione            = '';
$app->titolo              = '';
$app->instance_id         = '';
$app->chr_encoding        = '';
$app->default             = '';
$app->no_login            = '';
$app->lock_page           = '';
$app->sess_error          = '';
$app->pagina_prima        = '';
$app->pagina_dopo         = '';
$app->css                 = '';
$app->client_width        = 0;
$app->client_height       = 0;
$app->vars                = array();
$app->page_mark           = 0;
$app->relocate_now        = false;
$app->caret_position      = array();
$app->js_file             = '';
$app->favicon             = '';
$app->rpc_server          = false;
$app->db_as_service       = false;
$app->dbas_readonly       = false;
$app->services            = array();
$app->service_redirect    = false;
$app->jobs_local_host     = false;
$app->validazione         = 0;
$app->session_name        = '';
$app->session_id          = '';
$app->terminal_id         = 0;
$app->user                = 'default';
$app->user_is_admin       = false;
$app->user_is_poweruser   = false;
$app->password            = '';
$app->password_strength   = 0;
$app->password_change     = false;
$app->force_pwd_change    = false;
$app->block_md5_pwd       = false;
$app->ldap_server         = false;
$app->ldap_search_domain  = false;
$app->ldap_base_dn        = false;
$app->timeout             = 10;
$app->keep_alive          = true;
$app->user_roles          = array();
$app->user_rights         = array();
$app->logout_url          = false;
$app->access_log          = 0;
$app->lock                = false;
$app->job_worker          = false;
$app->file_db             = 'db_repository.inc';
$app->file_tabelle        = 'file_repository.inc';
$app->file_modelli        = 'mod_repository.inc';
$app->file_menu           = 'menu_repository.inc';
$app->file_vars           = 'var_repository.inc';
$app->file_viewmodels     = 'view_repository.inc';
$app->engines             = array();
$app->server              = array();
$app->db                  = array();
$app->tab                 = array();
$app->maschere            = array();
$app->menu                = array();
$app->menu_attivo         = '';
$app->menu_height         = 0;
$app->menu_width          = 0;
$app->menu_cache          = false;
$app->menu_link           = false;
$app->menu_linkh          = false;
$app->menu_prgs           = array();
$app->menu_style          = 'T';
$app->tables_create       = true;
$app->split_tab_repos     = false;
$app->tables_log          = false;
$app->logged_tabs         = array();
$app->log_backup          = true;
$app->log_rebuild         = true;
$app->log_db              = false;
$app->record_trace        = false;
$app->fetch_loop          = false;
$app->snapshot_db         = null;
$app->bulk_limit          = 499;
$app->db_timeout_lock     = false;
$app->db_timeout_stm      = false;
$app->use_asp_table       = false;
$app->sess_tables         = array();
$app->exe_prg_start       = false;
$app->prg_start           = '';
$app->prg_request         = '';
$app->prg_unlock          = '';
$app->maint_time          = 100;
$app->prg_user_prof       = false;
$app->prg_user_area       = false;
$app->chiamate            = array();
$app->istanze_prg         = array();
$app->progressivo_istanze = 0;
$app->istanza_attiva      = 0;
$app->prgs_cache          = array();
$app->esecuzioni          = array();
$app->ritorno             = false;
$app->try_catch_nest      = array();
$app->extcall_level       = 0;
$app->extcall_ritorno     = false;
$app->info_submit         = false;
$app->block_exe           = false;
$app->isolated_trans      = false;
$app->produzioni          = array();
$app->reporter            = '';
$app->ext_resources       = array();
$app->xmlstream_head      = false;
$app->xmlstream_foot      = false;
$app->app_options         = array();
$app->user_options        = array();
$app->msg_del             = '';
$app->msg_undo            = '';
$app->msg_norec           = '';
$app->profiling           = false;
$app->fastmsg_time        = 5;
$app->prgs_path_inc       = false;
$app->templ_path_inc      = false;
$app->host                = '';
$app->zoom                = true;
$app->start_user          = false;
$app->max_req_n           = 5;
$app->open_wins           = array();
$app->js_cache            = '';
$app->error_mode          = false;
$app->errors_log          = false;
$app->errors_log_full     = false;
$app->warning_log         = false;
$app->errors_inc          = false;
$app->error_handler       = false;
$app->custom_response     = false;
$app->prepared_stmts      = true;
$app->defining_combo      = null;
$app->defining_check      = null;
$app->statusbar_text      = '';
$app->status_custom       = false;
$app->status_height       = 0;
$app->html_started        = false;
$app->mouse_point         = false;
$app->mouse_wait          = false;
$app->grid_plus           = false;
$app->grid_multidel       = true;
$app->refresh_prg         = false;
$app->refresh_time        = 0;
$app->refresh_last        = 0;
$app->jxhttp_extra        = false;
$app->otp_time            = 3600;
$app->bug_aggregate       = false;
$app->bug_error_flow      = false;
$app->file_server_inc     = '';
$app->file_client_inc     = '';
$app->extra_html_head     = false;
$app->dispatcher          = null;
$app->doc                 = false;
$app->doc_enabled         = false;
$app->multilang           = false;
$app->translation         = array();
$app->run_level           = 'EXE';
$app->developer           = '';
$app->dev_key             = '';
$app->log2file            = false;
$app->sqltrace            = false;
$app->mutelog             = false;
$app->dir_home            = '';
$app->dir_htdocs          = '';
$app->dir_progs           = '';
$app->dir_cvs_progs       = '';
$app->dir_data            = '';
$app->dir_tmp             = '';
$app->dir_css             = '';
$app->dir_user            = '';
$app->dir_logs            = '';
$app->dir_models          = '';
$app->dir_img             = '';
$app->dir_uploads         = '';

/**
 * Define globals variables for runtime and application objjects
 */
$GLOBALS['o2_runtime'] = $rnt;
$_SESSION['o2_app']    = $app;


function o2val($view_name, $field_name) {

    $jxctx = $GLOBALS['jxt-context'];
    $type  = 'A';
    // _________________________________________________________ Application variables ___
    if ($view_name == '_o2SESSION') {
        $app_vars = $jxctx['A'];
        $type     = (isset($app_vars[$field_name]) ? $app_vars[$field_name] : 'A');
        }
    // _____________________________________________________________ Program variables ___
    elseif ($view_name == 'prg§_§var' || $view_name == 'prg_var') {
        $prg_vars = $jxctx['V'];
        $type     = (isset($prg_vars[$field_name]) ? $prg_vars[$field_name] : 'A');
        }
    // ______________________________ Program selects (fields from views and formulas) ___
    else {
        $prg_sels = $jxctx['S'];
        $type     = (isset($prg_sels[$view_name][$field_name]) ?
                     $prg_sels[$view_name][$field_name] : 'A');
        }
    switch ($type) {
        case 'A':
            $ret = '';
            break;
        case 'D':
            $ret = '20010101';
            break;
        case 'O':
            $ret = '120000';
            break;
        case 'N':
            $ret = 999;
            break;
        case 'L':
            $ret = 1;
            break;
        case 'S':
            if (func_num_args() > 2) {
                $ret = '';
                }
            else {
                $ret = array();
                }
            break;
        }
    return $ret;

    }


function o2pre($view_name, $field_name) {

    return o2val($view_name, $field_name);

    }


function o2zero($view_name, $field_name) {

    return false;

    }


function o2isnull($view_name, $field_name) {

    return false;

    }


function o2nullify($view_name, $field_name) {

    return true;

    }


function o2exp($exp_n = 0) {

    return true;

    }


function o2par($parameter_id) {

    $pars = $GLOBALS['jxt-context']['P'];
    $type = (isset($pars[$parameter_id]) ? $pars[$parameter_id] : 'A');
    switch ($type) {
        case 'A':
            $ret = '';
            break;
        case 'D':
            $ret = '00000000';
            break;
        case 'O':
            $ret = '000000';
            break;
        case 'N':
            $ret = 0;
            break;
        case 'L':
            $ret = 0;
            break;
        case 'S':
            $ret = array();
            break;
        }
    return $ret;

    }


function o2par_zero($parameter_id) {

    return false;

    }


function o2format($value, $model) {

    return '';

    }


function o2uid() {

    return '1000';

    }


function o2_date_obj($date_str = '', $time_str = '000000') {

    $d = ($date_str ? $date_str : '00000000');
    $t = ($time_str ? $time_str : '000000');
    return new DateTime($d[0].$d[1].$d[2].$d[3].'-'.
                        $d[4].$d[5].'-'.
                        $d[6].$d[7].' '.
                        $t[0].$t[1].':'.
                        $t[2].$t[3].':'.
                        $t[4].$t[5]);

    }


function o2_date_diff($date1, $time1, $date2, $time2 = '000000', $format = false) {

    $d1 = ($date1 ? $date1 : '00000000');
    $t1 = ($time1 ? $time1 : '000000');
    $o1 = new DateTime($d1[0].$d1[1].$d1[2].$d1[3].'-'.
                       $d1[4].$d1[5].'-'.
                       $d1[6].$d1[7].' '.
                       $t1[0].$t1[1].':'.
                       $t1[2].$t1[3].':'.
                       $t1[4].$t1[5]);
    $d2 = ($date2 ? $date2 : '00000000');
    $t2 = ($time2 ? $time2 : '000000');
    $o2 = new DateTime($d2[0].$d2[1].$d2[2].$d2[3].'-'.
                       $d2[4].$d2[5].'-'.
                       $d2[6].$d2[7].' '.
                       $t2[0].$t2[1].':'.
                       $t2[2].$t2[3].':'.
                       $t2[4].$t2[5]);
    $do = $o1->diff($o2);
    if ($format) {
        return $do->format($format);
        }
    else {
        return $do->days;
        }

    }


function o2_mkdate($date, $years = 0, $months = 0, $days = 0) {

    $d  = intval(substr($date, 6, 2));
    $m  = intval(substr($date, 4, 2));
    $y  = intval(substr($date, 0, 4));
    // _______________________________________ Create a date object from starting date ___
    $do = (new DateTime())->setDate($y, $m, $d);
    $ts = $do->getTimestamp();
    // _________________ In case of 0 days added to end-of-month preserve end-of-month ___
    if (($days == 0) && $ts == (new DateTime())->setDate($y, $m + 1, 0)->getTimestamp()) {
        $do = (new DateTime())->setDate($y + $years, $m + $months + 1, 0);
        }
    else {
        // _______________________________________ Add positive years, months and days ___
        if ($years > 0 || $months > 0 || $days > 0) {
            $do->add(new DateInterval("P".($years > 0 ? $years."Y" : "").
                                          ($months > 0 ? $months."M" : "").
                                          ($days > 0 ? $days."D" : "")));
            }
        // _________________________________ Substract negative years, months and days ___
        if ($years < 0 || $months < 0 || $days < 0) {
            $do->sub(new DateInterval("P".($years < 0 ? abs($years)."Y" : "").
                                          ($months < 0 ? abs($months)."M" : "").
                                          ($days < 0 ? abs($days)."D" : "")));
            }
        }
    return ($do ? $do->format("Ymd") : "00000000");

    }


function o2_mktime($time, $hours = 0, $minutes = 0, $seconds = 0) {

    $ts = @mktime(substr($time, 0, 2) + $hours,
                  substr($time, 2, 2) + $minutes,
                  substr($time, 4, 2) + $seconds,
                  1,
                  1,
                  1);
    return ($ts ? date("His", $ts) : "000000");

    }


function o2_callprg($prg) {

    return true;

    }


function o2_prg4http($prg_name = "") {

    return $prg_name;

    }


function o2_act4js($prg_id, $action_name) {

    return $action_name;

    }


function o2_reqpar($par_id) {

    return '';

    }


function o2_path($local_file, $force_download = false) {

    return '';

    }


function o2_encode($string) {

    return $string;

    }


function o2_send($file_url) {

    return true;

    }


function o2out($text) {

    return true;

    }


function o2mail_send($from,
                     $to,
                     $cc      = "",
                     $bcc     = "",
                     $subject = "No subject",
                     $text    = "",
                     $as_html = false,
                     $attach  = false) {

    return true;

    }


function o2_translate($code, $string) {

    return $string;

    }


function o2_play($sound_file = false) {

    return true;

    }


function o2_status($code) {

    return true;

    }


//= ==================================== CONTROL =========================================

function o2ctrl_focus($form_name, $ctrl_name, $select_mode = null) {

    return true;

    }


function o2ctrl_activate($form_name, $ctrl_name) {

    return true;

    }


function o2ctrl_set_x($form_name, $ctrl_name, $x = 0) {

    return true;

    }


function o2ctrl_set_y($form_name, $ctrl_name, $y = 0) {

    return true;

    }


function o2ctrl_set_width($form_name, $ctrl_name, $width = 0) {

    return true;

    }


function o2ctrl_set_height($form_name, $ctrl_name, $height = 0) {

    return true;

    }


function o2ctrl_log($ctrl_name, $exe_id = 0) {

    return array();

    }


function o2record_log($view_name, $table_alias, $exe_id = 0) {

   return array();

    }


// ==================================== o2 RUNTIME =======================================

function o2rnt_alias() {

    return '';

    }


function o2rnt_root() {

    return '';

    }


function o2rnt_dirthreads() {

    return '';

    }


function o2rnt_dircss() {

    return '';

    }


function o2rnt_interface() {

    return 'HTML';

    }


// ===================================== SESSION =========================================

function o2session_is_job() {

    return false;

    }


function o2session_is_scheduler() {

    return false;

    }


function o2session_lock($on = true) {

    return true;

    }


// ======================================= JOBS ==========================================

function o2job_set_progress($perc) {

    return true;

    }


function o2job_add_file($file_path) {

    return true;

    }


function o2job_create($name,
                      $prg,
                      $params        = false,
                      $desc          = "",
                      $keep_days     = 0,
                      $max_instances = false,
                      $block_mode    = "B",
                      $disabled      = false) {

    return true;

    }


function o2job_get_by_name($job_name) {

    return 1;

    }


function o2job_is_enabled($job_name) {

    return true;

    }


function o2job_get_bar($run_job_id, $complete_action = false, $start_action = false) {

    return '';

    }


function o2job_enable($job_name, $on = true) {

    return true;

    }


function o2job_queue($job_name, $params = array(), $host = false) {

    return 1;

    }


function o2job_run($job_name, $params = array()) {

    return 1;

    }


function o2job_is_running($job_name) {

    return true;

    }


function o2job_proc_get_status($runjob_id) {

    return 'C';

    }


function o2job_proc_kill($runjob_id) {

    return true;

    }


function o2job_schedule($job_name,
                        $type,
                        $interval   = 0,
                        $date       = "00000000",
                        $time       = "000000",
                        $recurrence = "D",
                        $week_day   = "",
                        $month_day  = 0,
                        $aspid      = "",
                        $all_hosts  = false) {

    return 1;

    }


function o2job_is_scheduled($job_name) {

    return false;

    }


// ======================================= CRON ==========================================

function o2cron_is_registered() {

    return true;

    }


function o2cron_register($on = true) {

    return true;

    }


// ==================================== DEVELOPER ========================================

function o2log($logobj = null, $log2file = false) {

    return true;

    }


function o2sqltrace($start = true) {

    return true;

    }


// ======================================= USER ==========================================

function o2user_roles() {

    return array();

    }


function o2user_rights() {

    return array();

    }


function o2user_has($right_code) {

    return true;

    }


function o2user_is($role_name) {

    return true;

    }


function o2user_is_admin() {

    return true;

    }


function o2user_is_poweruser() {

    return true;

    }


function o2user_load_granting() {

    return true;

    }


function o2user_delete($user) {

    return true;

    }


function o2user_pwd_expires($user = false) {

    return 0;

    }


// ===================================== JAVASCRIPT =======================================

function o2js_exe($js_code) {

    return true;

    }


function o2js_doc_ready($js_code) {

    return true;

    }


function o2js_load_script($js_url) {

    return true;

    }


function o2js_load_css($css_url) {

    return true;

    }


// ====================================== BROWSER ========================================

function o2client_browser() {

    return '';

    }


function o2client_ip() {

    return '';

    }


function o2client_host() {

    return '';

    }


function o2client_platform() {

    return '';

    }


function o2client_os() {

    return '';

    }


function o2client_width() {

    return 1200;

    }


function o2client_height() {

    return 900;

    }


// ==================================== APPLICATION ======================================

function o2app_sid() {

    return '123456789';

    }


function o2app_user($new_user = false) {

    return 'root';

    }


function o2app_iniset($parameter, $value) {

    return true;

    }


function o2app_password() {

    return '';

    }


function o2app_css() {

    return '';

    }


function o2app_prg() {

    return array();

    }


function o2app_name() {

    return '';

    }


function o2app_instance_id() {

    return '12345678';

    }


function o2app_alias() {

    return '';

    }


function o2app_host() {

    return '';

    }


function o2app_title($title = false) {

    return '';

    }


function o2app_validate() {

    return 1;

    }


function o2app_encoding() {

    return '';

    }


function o2app_timeout() {

    return 1000;

    }


function o2app_exetime() {

    return 1000000;

    }


function o2app_runlevel() {

    return 'DEV';

    }


function o2app_runmode() {

    return 'WEB';

    }


function o2app_developer() {

    return 'supervisor';

    }


function o2app_nologin() {

    return '';

    }


function o2app_sesserror() {

    return '';

    }


function o2app_menu() {

    return 'base';

    }


function o2app_lang() {

    return 'it';

    }


function o2app_vars() {

    return array();

    }


function o2app_menus() {

    return array();

    }


function o2app_models() {

    return array();

    }


function o2app_servers() {

    return array();

    }


function o2app_databases() {

    return array();

    }


function o2app_tables() {

    return array();

    }


function o2app_createtables() {

    return true;

    }


function o2app_prgs_list() {

    return array();

    }


function o2app_startprg() {

    return '';

    }


function o2app_reqprg() {

    return '';

    }


function o2app_submit($index = false) {

    $ret = array();
    switch (strtolower($index)) {
        case "form":
           $ret = 'form';
           break;
        case "ctrl":
           $ret = 'ctrl';
           break;
        }
    return $ret;

    }


function o2app_dir_home() {

    return '';

    }


function o2app_dir_htdocs() {

    return '';

    }


function o2app_dir_prgs() {

    return '';

    }


function o2app_dir_cvsprgs() {

    return '';

    }


function o2app_dir_tmp() {

    return '';

    }


function o2app_dir_data() {

    return '';

    }


function o2app_dir_doc() {

    return '';

    }


function o2app_dir_css() {

    return '';

    }


function o2app_dir_logs() {

    return '';

    }


function o2app_dir_templates() {

    return '';

    }


function o2app_dir_img() {

    return '';

    }


function o2app_dir_user() {

    return '';

    }


function o2app_dir_uploads() {

    return '';

    }


function o2app_logout($exit_url = false, $params = false) {

    return true;

    }


function o2app_otp($user = '', $otp_len = 6) {

    return '123456';

    }


// ======================================= MENU ==========================================

function o2menu_isactive($menu_item) {

    return true;

    }


function o2menu_type($menu_name = "") {

    return '';

    }


function o2menu_label($menu_name = "") {

    return '';

    }


function o2menu_data($menu_name = "") {

    return '';

    }


function o2menu_icon($menu_name = "") {

    return '';

    }


function o2menu_additem($menu_name  = "",
                        $item_type  = "S",
                        $item_name  = "",
                        $item_label = "",
                        $item_data  = "",
                        $visible    = true,
                        $active     = true,
                        $item_ico   = "",
                        $position   = 0) {

    return true;

    }


function o2menu_code($menu_name = '', $style = '') {

    return '';

    }


// ==================================== SQL ENGINE =======================================

function o2sql_delimiter_open($engine_type) {

    return '"';

    }


function o2sql_delimiter_close($engine_type) {

    return '"';

    }


// ====================================== SERVER =========================================

function o2dbs_name($server_name) {

    return '';

    }


function o2dbs_engine($server_name) {

    return '';

    }


function o2dbs_user($server_name) {

    return '';

    }


function o2dbs_password($server_name) {

    return '';

    }


function o2dbs_query($server_name, $command_str) {

    return array();

    }


function o2dbs_execute($server_name, $command_str) {

    return 1;

    }


function o2dbs_commit($server_name) {

    return true;

    }


function o2dbs_rollback($server_name) {

    return true;

    }


// ===================================== DATABASE ========================================

function o2db_name($db_name) {

    return '';

    }


function o2db_server($db_name) {

    return '';

    }


function o2db_engine($db_name) {

    return '';

    }


function o2db_qprefix($db_name) {

    return '';

    }


function o2db_schema($db_name) {

    return '';

    }


function o2db_tables($db_name) {

    return array();

    }


function o2db_asp($db_name) {

    return false;

    }


function o2db_createtab($db_name) {

    return '';

    }


function o2db_indexes_seton($db_name) {

    return 1;

    }


function o2db_indexes_setoff($db_name) {

    return 1;

    }


function o2db_fkeys_seton($db_name) {

    return 1;

    }


function o2db_fkeys_setoff($db_name) {

    return 1;

    }


// ====================================== MODEL ==========================================

function o2model_type($model_name) {

    return 'A';

    }


function o2model_mask($model_name) {

    return 'C50';

    }


function o2model_size($model_name) {

    return 50;

    }


function o2model_values($model_name) {

    return array();

    }


function o2model_values4combo($model_name, $add_blank = false, $blank_label = "") {

    return array();

    }


// ====================================== FIELD ==========================================

function o2field_name($table_name, $field_name) {

    return '';

    }


function o2field_fname($table_name, $field_name) {

    return '';

    }


function o2field_model($table_name, $field_name) {

    return '_o2alpha';

    }


// ====================================== TABLE ==========================================

function o2tab_is_defined($table_name) {

    return true;

    }


function o2tab_exists($table_name) {

    return true;

    }


function o2tab_data($table_name) {

    return true;

    }


function o2tab_create($table_name) {

    return true;

    }


function o2tab_drop($table_name) {

    return true;

    }


function o2tab_script($table_name) {

    return '';

    }


function o2tab_db($table_name) {

    return '';

    }


function o2tab_engine($table_name) {

    return '';

    }


function o2tab_name($table_name) {

    return $table_name;

    }


function o2tab_qname($table_name) {

    return $table_name;

    }


function o2tab_fields($table_name) {

    return array();

    }


function o2tab_indexes($table_name, $type_info = false) {

    return array();

    }


function o2tab_pkey($table_name) {

    return 'PK';

    }


function o2tab_log($table_name) {

    return true;

    }


function o2tab_log_level($table_name) {

    return 'M';

    }


function o2tab_set_log($table_name, $log_level = "R") {

    return true;

    }


function o2tab_unset_log($table_name) {

    return true;

    }


function o2tab_suspend_log($table_name, $suspend = true) {

    return true;

    }


function o2tab_trace($table_name) {

    return '';

    }


function o2tab_suspend_trace($table_name, $suspend = true) {

    return true;

    }


function o2tab_info($table_name) {

    return array();

    }


function o2tab_rebuild($table_name, $matches = null) {

    return true;

    }


function o2tab_rename($table_name, $new_name) {

    return true;

    }


function o2tab_column_add($table_name, $column_name, $type, $int = 0, $dec = 0) {

    return true;

    }


function o2tab_column_remove($table_name, $column_name) {

    return true;

    }


function o2tab_column_resize($table_name, $column_name, $int, $dec = 0) {

    return true;

    }


function o2tab_column_rename($table_name, $old_name, $new_name) {

    return true;

    }


function o2tab_index_add($table_name, $index_name, $index_segs, $unique = true) {

    return true;

    }


function o2tab_index_remove($table_name, $index_name) {

    return true;

    }


function o2tab_index_seton($table_name, $index_name) {

    return true;

    }


function o2tab_index_setoff($table_name, $fkey_name) {

    return true;

    }


function o2tab_indexes_seton($table_name) {

    return 1;

    }


function o2tab_indexes_setoff($table_name) {

    return 1;

    }


function o2tab_export($table_name, $file_name, $asp_mode = false) {

    return true;

    }


function o2tab_import($table_name, $file_name, $asp_mode = false) {

    return true;

    }


function o2tab_browse($table_name, $index = "") {

    return true;

    }


function o2tab_browselog($table_name) {

    return true;

    }


function o2tab_asp($table_name) {

    return false;

    }


function o2tab_aspclear($table_name) {

    return true;

    }


function o2tab_aspclone($table_name, $target_asp) {

    return true;

    }


function o2tab_fkeys_list($table_name) {

    return array();

    }


function o2tab_fkey_seton($table_name, $fkey_name) {

    return true;

    }


function o2tab_fkey_setoff($table_name, $fkey_name) {

    return true;

    }


function o2tab_fkeys_seton($table_name) {

    return 1;

    }


function o2tab_fkeys_setoff($table_name) {

    return 1;

    }


// ===================================== PROGRAM =========================================

function o2prg_params($exe_id = 0) {

    return array();

    }


// =================================== VIEW TABLE ========================================

function o2viewtab_exists($view_name, $table_alias) {

    return true;

    }


function o2viewtab_data($view_name, $table_alias) {

    return true;

    }


function o2viewtab_create($view_name, $table_alias) {

    return true;

    }


function o2viewtab_drop($view_name, $table_alias) {

    return true;

    }


function o2viewtab_info($view_name, $table_alias) {

    return array();

    }


function o2viewtab_rebuild($view_name, $table_alias, $matches = null) {

    return true;

    }


function o2viewtab_rename($view_name, $table_alias, $new_name) {

    return true;

    }


function o2viewtab_export($view_name, $table_alias, $file_name, $asp_mode = false) {

    return true;

    }


function o2viewtab_import($view_name, $table_alias, $file_name, $asp_mode = false) {

    return true;

    }


function o2viewtab_browse($view_name, $table_alias, $index = "") {

    return true;

    }


function o2viewtab_aspclear($view_name, $table_alias) {

    return true;

    }


function o2viewtab_aspclone($view_name, $table_alias, $target_asp) {

    return true;

    }


// ====================================== VIEW ===========================================

function o2view_mod($view_name) {

    return true;

    }


function o2view_status($view_name) {

    return 'M';

    }


function o2view_ins($view_name) {

    return true;

    }


function o2view_start($view_name) {

    return 1;

    }


function o2view_end($view_name) {

    return 10;

    }


function o2view_reqrows($view_name) {

    return 10;

    }


function o2view_retrows($view_name) {

    return 10;

    }


function o2view_total($view_name, $from_cache = false) {

    return 10;

    }


function o2view_rec($view_name) {

    return true;

    }


function o2view_row($view_name) {

    return array();

    }


function o2view_select($view_name) {

    return 1;

    }


function o2view_export($view_name,
                       $csv_file_name     = "",
                       $only_visible      = false,
                       $fields_separator  = ",",
                       $text_delimiter    = '"',
                       $decimal_separator = ".",
                       $date_format       = "Ymd",
                       $time_format       = "His",
                       $titles_from_view  = false,
                       $excluded_aliases  = false,
                       $used_aliases      = false) {

    return true;

    }


function o2view_get_query($view_name, $with_orderby = false, $with_limit = false) {

    return '';

    }


function o2view_fetch($view_name) {

    return array();

    }


function o2view_fetch_free($view_name) {

    return true;

    }


function o2view_delete($view_name) {

    return true;

    }


function o2view_insertinto($view_name, $target_table) {

    return 1;

    }


function o2view_selector($view_name) {

    return true;

    }


function o2view_selector_all($view_name) {

    return true;

    }


function o2view_selector_none($view_name) {

    return true;

    }


function o2view_selector_set($view_name, $on = true) {

    return true;

    }


function o2view_selector_get_selected($view_name) {

    return array();

    }


function o2view_snapshot($view_name) {

    return true;

    }


function o2view_snapshot_name($view_name, $snapshot_name = false) {

    return '';

    }


function o2view_snapshot_exists($view_name) {

    return true;

    }

// =============================== TRANSACTION ===========================================

function o2transaction_commit() {

    return true;

    }


function o2transaction_rollback() {

    return true;

    }


function o2transaction_isolate() {

    return true;

    }


// ================================ ACTION ===============================================

function o2act_counter($action_name = "") {

    return 1;

    }

// ================================= FORM ================================================

function o2form_x($form_name, $level = 0) {

    return 100;

    }


function o2form_y($form_name, $level = 0) {

    return 100;

    }


function o2form_width($form_name, $level = 0) {

    return 100;

    }


function o2form_height($form_name, $level = 0) {

    return 100;

    }


function o2form_maximize($form_name, $maximized = true) {

    return true;

    }


// ========================================= DIR =========================================

function o2dir_list($dir_path, $file_match = "*") {

    return array();

    }


function o2dir_create($dir_path) {

    return true;

    }


function o2dir_delete($dir_path) {

    return true;

    }


// ========================================= FILE ========================================

function o2file_fullname($file_path) {

    return $file_path;

    }


function o2file_basename($file_path) {

    return $file_path;

    }


function o2file_ext($file_path) {

    return $file_path;

    }


function o2file_dir($file_path) {

    return '';

    }


function o2file_type($file_path) {

    return 'F';

    }


function o2file_ico($file_path, $large = false, $internal_use = false) {

    return '';

    }


function o2file_size($file_path) {

    return 50;

    }


function o2file_date($file_path) {

    return '20000101';

    }


function o2file_time($file_path) {

    return '000000';

    }


function o2file_exists($file_path) {

    return true;

    }


function o2file_delete($file_path) {

    return true;

    }


function o2file_rename($file_path, $new_name) {

    return true;

    }


function o2file_copy($file_path, $new_file) {

    return true;

    }


function o2file_encode($file_path) {

    return $file_path;

    }


// ====================================== ASP ============================================

function o2asp_code_get() {

    return '';

    }


function o2asp_code_set($new_aspid) {

    return '';

    }


function o2asp_codes() {

    return array();

    }


// =================================== DISPATCHER ========================================

function o2dispatcher_get_list() {

    return array();

    }


function o2dispatcher_count() {

    return 1;

    }


function o2dispatcher_clear() {

    return true;

    }


function o2dispatcher_alert($msg = '') {

    return true;

    }


function o2dispatch_add($title, $body = "", $icon = false) {

    return 1;

    }


function o2dispatch_add_label($title, $body = "", $icon = false) {

    return '';

    }


function o2dispatch_set_prg($id, $prg_name) {

    return true;

    }


function o2dispatch_remove($id) {

    return true;

    }


function o2dispatch_get($id) {

    return array();

    }


function o2dispatch_can_activate($id) {

    return true;

    }


function o2dispatch_activate($id, $preserve = false) {

    return true;

    }


// ================================= TREEVIEW CONTROL ====================================

function o2tree_def($name, $structure = false, $activation = 0, $action = false) {

    return new stdClass();

    }


function o2tree_destroy($name) {

    return true;

    }


function o2tree_fold($name, $level = 0) {

    return true;

    }


function o2tree_unfold($name) {

    return true;

    }


function o2tree_expand($name, $level = 0) {

    return true;

    }


function o2tree_collapse($name) {

    return true;

    }


function o2tree_node_fold($name, $node_id) {

    return true;

    }


function o2tree_node_unfold($name, $node_id) {

    return true;

    }


function o2tree_node_expand($name, $node_id) {

    return true;

    }


function o2tree_node_collapse($name, $node_id) {

    return true;

    }


function o2tree_node_select($name, $node_id) {

    return true;

    }


function o2tree_node_add($name, $node_id, $parent_id, $node_code, $node_icon = "") {

    return true;

    }


function o2tree_node_remove($name, $node_id) {

    return true;

    }


function o2tree_node_show($name, $node_id) {

    return true;

    }


function o2tree_get_nodes($name) {

    return array();

    }


function o2tree_get_selection($name) {

    return '1';

    }


function o2tree_get_code($name) {

    return '';

    }



function o2tree_filter($name, $text, $hide_down = false) {

    return true;

    }


function o2tree_export($tree_name,
                       $csv_file_name    = '',
                       $fields_separator = ',',
                       $text_delimiter   = '"') {

    return '';

    }


// ================================= IMAGE FUNCTIONS =====================================

function o2img_thumbnail($source_file, $target_file, $width, $height, $ratio = true) {

    return true;

    }


// ============================== IMAGES LISTER CONTROL ==================================


function o2imglist_def($name,
                       $items        = false,
                       $width        = false,
                       $height       = false,
                       $act_activate = false,
                       $act_delete   = false) {

    return new stdClass();

    }


function o2imglist_destroy($name) {

    return true;

    }


function o2imglist_item_add($name, $item_id, $text_code) {

    return true;

    }


function o2imglist_item_remove($name, $item_id) {

    return true;

    }


function o2imglist_set_size($name, $width = false, $height = false) {

    return true;

    }


function o2imglist_get_items($name) {

    return array();

    }


function o2imglist_get_code($name) {

    return '';

    }


function o2imglist_get_selection($name) {

    return '';

    }


// =============================== PROGRESS BAR CONTROL ==================================

function o2progress_bar($bar_name,
                        $value,
                        $complete_action = "",
                        $start_action    = "",
                        $active          = true) {

    return '';

    }


function o2progress_view_bar($bar_name,
                             $view,
                             $id_field,
                             $progress_field,
                             $complete_action = "",
                             $start_action    = "",
                             $active          = true) {

    return '';

    }


// ============================ PROGRAM REFRENCED VARIABLES ==============================

function o2refvar_set($name, $value = false) {

    $name = strtolower($name);
    $GLOBALS['reference_vars'][$name] = $value;

    }


function &o2refvar_get($name) {

    $name = strtolower($name);
    return $GLOBALS['reference_vars'][$name];

    }


// ==================================== LAST ERROR =======================================

function o2error_lastclear() {

    return true;

    }


function o2error_getlastid($box_div = false) {

    return 0;

    }


function o2error_getlastcode() {

    return false;

    }


function o2error_getlastclass() {

    return false;

    }


function o2error_getlastmsg() {

    return '';

    }


function o2error_getlasttxt() {

    return false;

    }


function o2error_getlasthtml() {

    return '';

    }


// ================================== LAST EXCEPTION =====================================

function o2exception_get() {

    return new stdClass();

    }


// ================================= FORMATTING TEXT =====================================

function o2txt_justify($text = "", $width = 60, $indent = 0, $indent_chr = " ") {

    return $text;

    }


function o2txt_split ($text_left  = "",
                      $text_right = "",
                      $width      = 60,
                      $indent     = 0,
                      $indent_chr = " ",
                      $sep_chr    = ".") {

    return $text_left.' '.$text_right;

    }


function o2txt_center($text       = "",
                      $width      = 60,
                      $indent     = 0,
                      $indent_chr = " ",
                      $fill_chr   = " ") {

    return $text;

    }


function jxsql_parse($dbname, $sql, $fq_columns = false) {

    return '';

    }


function jxsql($dbname, $sql, $fq_columns = false) {

    return '';

    }


function o2_str_nice($str) {

    return '';

    }


function o2_view2list($view_name, $code_field, $desc_field, $preset = array()) {

    return array('' => 'Empty');

    }


?>
