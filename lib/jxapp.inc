<?php

/**
 * Janox Application Module
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
 * This script contains all elements needed by Janox application structure.
 *
 *
 * @name      jxapp
 * @package   janox/jxapp.inc
 * @version   2.8
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


// ________________________________________________________________ Janox runtime core ___
include_once 'jxcore.inc';
// _________________________________________________________________ Environment stuff ___
include_once 'jxenv.inc';
// ____________________________________________________________________ User interface ___
include_once 'jxui.inc';
// ________________________________________________________________ Callable functions ___
include_once 'jxfnx.inc';


/**
 * Application: it contains all Janox data, only one instance at a time is allowed and it
 * is referenced by $_SESSION['o2_app'].
 *
 */
class o2_app {

    /*     ===== PROPERTIES =========================================================== */
    /**
     * @var o2_runtime
     */
    public $runtime          = null;    /* Running o2_runtime object                    */
    public $nome             = '';      /* Application name                             */
    public $alias            = '';      /* Application alias (in the form .../[alias]/) */
    public $referer          = '';      /* <alias>/<name>.php ($_SERVER['PHP_SELF'])    */
    public $versione         = '';      /* Version (in the form v.s.r)                  */
    public $titolo           = '';      /* Text to write in the browser title bar       */
    public $instance_id      = '';      /* Application instance unique id               */
    public $chr_encoding     = '';      /* Character encoding (default windows-1252)    */
    public $default          = '';      /* Document to be included when nothing to do   */
    public $no_login         = '';      /* Failed login page                            */
    public $lock_page        = '';      /* Access denied for Under Maintenance page     */
    public $sess_error       = '';      /* Failed session validation page               */
    public $pagina_prima     = '';      /* Document to prepend to pages                 */
    public $pagina_dopo      = '';      /* Document to append to pages                  */
    public $css              = '';      /* CSS documents string (separated by ",")      */
    public $client_width     = 0;       /* Browser window width                         */
    public $client_height    = 0;       /* Browser window height                        */
    public $vars             = array(); /* Application variables (o2_appvar) list       */
    public $page_mark        = 0;       /* Last sent page mark for page expiring        */
    public $relocate_now     = false;   /* Views relocating immediate executing         */
    public $caret_position   = array(); /* Programs scripts for caret reposition list   */
    public $js_file          = '';      /* Javascript file to be included               */
    public $favicon          = '';      /* "Favorite icon" for application              */
    public $rpc_server       = false;   /* If app is used as an RPC Server              */
    public $db_as_service    = false;   /* DB name app is serving as service (RPC only) */
    public $dbas_readonly    = false;   /* Database exposed as a service read-only mode */
    public $services         = array(); /* List of defined services                     */
    public $service_redirect = false;   /* If set all jobs are assigned to this service */
    public $jobs_local_host  = false;   /* If set all jobs are assigned to local host   */
    /*     _____ Session ______________________________________________________________ */
    public $validazione        = 0;         /* Validation level (0=none,1=full,2=mixed))*/
    public $session_name       = '';        /* Session name (PHPSESSID)                 */
    public $session_id         = '';        /* Session unique id                        */
    public $terminal_id        = 0;         /* Terminal ID, common to user multisession */
    public $user               = 'default'; /* Logged user ID                           */
    public $user_is_admin      = false;     /* If logged user is Administrator          */
    public $password           = '';        /* Logged user password                     */
    public $password_strength  = 0;         /* Rules to enforce password strength       */
    public $password_change    = false;     /* Force password to be changed every N days*/
    public $force_pwd_change   = false;     /* Password must be changed to login        */
    public $block_md5_pwd      = false;     /* Block login with MD5 hashed passwords    */
    public $ldap_server        = false;     /* LDAP authority server for user validation*/
    public $ldap_search_domain = false;     /* LDAP search domain for user validation   */
    public $ldap_base_dn       = false;     /* LDAP base DN for user validation         */
    public $timeout            = 10;        /* Session timeout (in minutes)             */
    public $keep_alive         = true;      /* Set auto-refresh to keep session alive   */
    public $user_roles         = array();   /* List of roles for logged user            */
    public $user_rights        = array();   /* List of rights owned by logged user      */
    public $logout_url         = false;     /* URL to logout app to, by o2app_logout()  */
    public $access_log         = 0;         /* O=No log; 1=Login/logout; 2=attempts too */
    public $lock               = false;     /* Deny standard logins under maintenance   */
    public $job_worker         = false;     /* Job running in session, if any           */
    /*     _____ Repositories _________________________________________________________ */
    public $file_db         = 'db_repository.inc';   /* Servers and databases           */
    public $file_tabelle    = 'file_repository.inc'; /* Tables                          */
    public $file_modelli    = 'mod_repository.inc';  /* Data models                     */
    public $file_menu       = 'menu_repository.inc'; /* Menus                           */
    public $file_vars       = 'var_repository.inc';  /* Application (session) variables */
    public $file_viewmodels = 'view_repository.inc'; /* View models                     */
    /*     _____ Internal repositories ________________________________________________ */
    public $engines         = array(); /* Database engines list                         */
    public $server          = array(); /* Servers list                                  */
    public $db              = array(); /* Databases list                                */
    public $tab             = array(); /* Tables list                                   */
    public $maschere        = array(); /* Data models list                              */
    /*     _____ Menus ________________________________________________________________ */
    public $menu            = array(); /* Defined menus list                            */
    public $menu_attivo     = '';      /* Level 0 menu name                             */
    public $menu_height     = 0;       /* Menu bar height                               */
    public $menu_width      = 0;       /* Menu vertical bar width                       */
    public $menu_cache      = false;   /* Menu bar cached HTML code                     */
    public $menu_link       = false;   /* Menus definition javascript file (TRN)        */
    public $menu_prgs       = array(); /* List of prgs callable by menu (o2c in URL)    */
    public $menu_style      = 'T';     /* Menu style [T]op or [L]eft                    */
    /*     _____ Tables options _______________________________________________________ */
    public $tables_create   = true;    /* Automatic tables creation at login            */
    public $split_tab_repos = false;   /* Each table has its own definition file        */
    public $tables_log      = false;   /* Set ON/OFF tables changes log capabilities    */
    public $logged_tabs     = array(); /* List of tables log is enabled for             */
    public $log_backup      = true;    /* Enables log-tables backup on drop/rebuild     */
    public $log_rebuild     = true;    /* Enables log-tables rebuild on struct. changes */
    public $log_db          = false;   /* Name of the DB to be used for log tables      */
    public $record_trace    = false;   /* List of columns for auto tracing on record    */
    public $fetch_loop      = false;   /* Statement fetching instead of paging in loops */
    public $snapshot_db     = null;    /* Database used for snapshots (jxsess_db)       */
    public $bulk_limit      = 499;     /* Maximum number of records for single insert   */
    public $db_timeout_lock = false;   /* Set lock-timeout for db session               */
    public $db_timeout_stm  = false;   /* Set statement-timeout for db session          */
    public $use_asp_table   = false;   /* Use sys table for ASPIDs or DISTINCT on users */
    /*     _____ Triggers _____________________________________________________________ */
    public $exe_prg_start   = false;   /* Start program has to be executed              */
    public $prg_start       = '';      /* Start program name                            */
    public $prg_request     = '';      /* Request program name                          */
    public $prg_unlock      = '';      /* Program to be executed on application unlock  */
    public $maint_time      = 100;     /* Number of hours for maintenance frequency     */
    /* _____________________________________________________________________ Plugin ___ */
    public $prg_user_prof   = false;   /* Program to customize user profiling (details) */
    public $prg_user_area   = false;   /* Program to customize user ASP area (details)  */
    /*     _____ Executions ___________________________________________________________ */
    public $chiamate        = array(); /* Calls and responses list                      */
    public $istanze_prg     = array(); /* Running programs instances list               */
    public $progressivo_istanze = 0;   /* Evaluating program instance id                */
    public $istanza_attiva  = 0;       /* Active (for interaction) program instance id  */
    public $prgs_cache      = array(); /* Cache list of executed programs               */
    public $esecuzioni      = array(); /* Pending executing actions list                */
    public $ritorno         = false;   /* If getting back from action or call           */
    public $extcall_level   = 0;       /* Prg id at which "external execution" starts   */
    public $extcall_ritorno = false;   /* If getting back from external call            */
    public $info_submit     = false;   /* Last submit form and control informations     */
    public $block_exe       = false;   /* Block record suffixes while rendering forms   */
    public $isolated_trans  = false;   /* ExeID of prg starting an isolated transaction */
    /*     _____ Productions & delivering _____________________________________________ */
    public $produzioni      = array(); /* Productions list                              */
    public $reporter        = '';      /* Complete path to o2reporter executable        */
    public $ext_resources   = array(); /* List of external resources to serve as STREAM */
    public $xmlstream_head  = false;   /* For XML stream (RPC) if doc head has been sent*/
    public $xmlstream_foot  = false;   /* For XML stream (RPC) if doc foot has been sent*/
    /*     _____ Options ______________________________________________________________ */
    public $app_options     = array(); /* Application runtime options list              */
    public $user_options    = array(); /* Logged user runtime options list              */
    public $msg_del         = '';      /* Message to be displayed on record delete      */
    public $msg_undo        = '';      /* Message to be displayed on record undo        */
    public $msg_norec       = '';      /* Message to be displayed in empty tables       */
    public $profiling       = false;   /* Enables runtime profiling capabilities        */
    public $fastmsg_time    = 5;       /* Fast messages time-out to fade away           */
    public $prgs_path_inc   = false;   /* Extra paths to look for prgs in               */
    public $templ_path_inc  = false;   /* Extra paths to look for report templates in   */
    /*     _____ Miscellaneous ________________________________________________________ */
    public $host            = '';      /* Machine registered name in a Janox network    */
    public $zoom            = true;    /* Enables activation for zoomable fields        */
    public $start_user      = false;   /* Usually FALSE or start user after o2app_user()*/
    public $max_req_n       = 5;       /* Max allowed number of unresponsed requests    */
    public $open_wins       = array(); /* List of open windows for full-ajax mode       */
    public $js_cache        = '';      /* JS commands to be fired at response end       */
    public $error_mode      = false;   /* Errors management: FALSE | DEV | EXE          */
    public $errors_log      = false;   /* Enables errors logging on file                */
    public $errors_inc      = false;   /* File name to be included for custom error msgs*/
    public $custom_response = false;   /* FALSE or buffer length for custom output      */
    public $jxsql           = false;   /* Enables internal JXSQL parser                 */
    public $prepared_stmts  = true;    /* Enables use of prepared statements in views   */
    /**
     * @var o2_ctrl_listcombo
     */
    public $defining_combo  = null;    /* o2_ctrl_listcombo ctrl to set as lookup ctrl  */
    public $defining_check  = null;    /* o2_ctrl_check ctrl to use as view-selection   */
    public $statusbar_text  = '';      /* Text to print in statusbar                    */
    public $status_custom   = false;    /* Custom HTML to display on status-bar          */
    public $status_height   = 0;       /* Status bar height                             */
    public $html_started    = false;   /* If HTML document has been opened              */
    public $mouse_point     = false;   /* Customized GdkCursor for pointer (default)    */
    public $mouse_wait      = false;   /* Customized GdkCursor for wait cursor          */
    public $grid_plus       = false;   /* Enables extra functions for db-grid control   */
    public $grid_multidel   = true;    /* Enables multi row deletion in grid (cont menu)*/
    public $refresh_prg     = false;   /* Program to be executed on refresh             */
    public $refresh_time    = 0;       /* Number of seconds to wait before refreshing   */
    public $refresh_last    = 0;       /* Last refresh execution time-out               */
    public $jxhttp_extra    = false;   /* Extra data for HTTP requests (see o2_post())  */
    public $otp_time        = 3600;    /* Time (in seconds) One Time Passwords expire   */
    /*     _____ BUGS compatibility ___________________________________________________ */
    public $bug_aggregate   = false;   /* If TRUE uncountable aggregations are skipped  */
    /*     _____ HTML extras __________________________________________________________ */
    public $file_server_inc = '';      /* Server side extra script                      */
    public $file_client_inc = '';      /* Client side extra script                      */
    public $extra_html_head = false;   /* Extra HTML HEAD content (META, SCRIPTS, ...)  */
    /*     _____ Modules ______________________________________________________________ */
    /**
     * @var o2_dispatcher
     */
    public $dispatcher      = null;    /* o2_dispatcher object                          */
    /*     _____ Documentor o2::doc ___________________________________________________ */
    public $doc             = false;   /* Application o2_doc object                     */
    public $doc_enabled     = false;   /* Application documentor module is active       */
    /*     _____ Internationalization - Multilang _____________________________________ */
    public $multilang       = false;   /* If labels internationalization is enabled:
                                          when enabled it is set to lang code (eng...)  */
    public $translation     = array(); /* Translation words list                        */
    /*     _____ Development __________________________________________________________ */
    public $run_level       = 'EXE';   /* Runtime execution level [DEV]/[EXE]           */
    public $developer       = '';      /* Developer name (got from runtime)             */
    public $dev_key         = '';      /* Developer password (MD5)                      */
    public $log2file        = false;   /* If logs from o2log() must be written to file  */
    public $sqltrace        = false;   /* If SQL tracing is sent to o2log()             */
    public $mutelog         = false;   /* Log only to file - no console                 */
    /*     _____ Filesystem ___________________________________________________________ */
    /**
     * @var o2_dir
     */
    public $dir_home        = '';      /* Application home (o2_dir)                     */
    /**
     * @var o2_dir
     */
    public $dir_htdocs      = '';      /* Application web root (o2_dir)                 */
    /**
     * @var o2_dir
     */
    public $dir_progs       = '';      /* Programs and repositories folder (o2_dir)     */
    /**
     * @var o2_dir
     */
    public $dir_cvs_progs   = '';      /* Developer personal folder (o2_dir)            */
    /**
     * @var o2_dir
     */
    public $dir_data        = '';      /* Internal db folder (o2_dir)                   */
    /**
     * @var o2_dir
     */
    public $dir_tmp         = '';      /* User TMP folder (o2_dir)                      */
    /**
     * @var o2_dir
     */
    public $dir_css         = '';      /* Application CSS folder (o2_dir)               */
    /**
     * @var o2_dir
     */
    public $dir_user        = '';      /* Logged user home folder (o2_dir)              */
    /**
     * @var o2_dir
     */
    public $dir_logs        = '';      /* Log files folder (o2_dir)                     */
    /**
     * @var o2_dir
     */
    public $dir_models      = '';      /* Application printout models folder (o2_dir)   */
    /**
     * @var o2_dir
     */
    public $dir_img         = '';      /* Applications images folder (o2_dir)           */
    /**
     * @var o2_dir
     */
    public $dir_uploads     = '';      /* User uploads folder (o2_dir)                  */


    /**
     * Costruttore
     *
     * @param  string $versione
     * @param  string $nome
     * @return o2_app
     */
    function __construct($versione = "", $nome = "") {

        // _________________________________________ Link to Janox runtime environment ___
        $this->runtime = $GLOBALS['o2_runtime'];
        $this->versione($versione);
        // ____________________________________________________________ PHP executable ___
        if (!$this->runtime->php_engine || !file_exists($this->runtime->php_engine)) {
            $this->runtime->find_php_exe();
            }
        // _______________________________________________________________ Development ___
        if ($this->runtime->developer) {
            $this->developer = $this->runtime->developer;
            $this->dev_key   = $this->runtime->dev_key;
            $this->run_level = "DEV";
            $this->sqltrace  = ($_REQUEST['jxtrace'] ? true : false);
            $this->log2file  = $this->sqltrace;
            $this->mutelog   = $this->sqltrace;
            }
        switch ($this->runtime->interface) {
            case "HTML":
                $this->session_name = session_name();
                $this->session_id   = session_id();
                if ($nome) {
                    $this->nome    = $nome;
                    $this->alias   = "";
                    $this->referer = "";
                    }
                else {
                    $this->referer = $_SERVER['PHP_SELF'];
                    $this->nome    = preg_replace('/\..*$/i', '',
                                                  basename($this->referer));
                    $this->alias   = dirname($this->referer)."/";
                    }
                $_SESSION['o2_app'] = $this;
                $this->crea_cartelle();
                $js_dir_local  = new o2_dir($this->runtime->root."htdocs".
                                            DIRECTORY_SEPARATOR."js".DIRECTORY_SEPARATOR,
                                            "env*.js");
                $files_list    = $js_dir_local->all_elements();
                $file_js_local = array_pop($files_list);
                $this->js_file = $file_js_local->nome.".".$file_js_local->ext;
                $this->favicon = ($this->dir_img->exists($this->nome.".ico") ?
                                  $this->alias."img/".$this->nome.".ico"     :
                                  $this->runtime->alias."img/jxapp.ico");
                // ________________________________________________ Client screen size ___
                if ($_REQUEST['jxcsw']) {
                    $this->client_width  = intval($_REQUEST['jxcsw']);
                    $this->client_height = intval($_REQUEST['jxcsh']);
                    }
                break;
            default:
                // ________________________________________ Generate pseudo session id ___
                $this->session_id = "";
                $chrs = "1234567890abcdefghijklmnopqrstuvwxyz";
                for($i = 0; $i < 32; $i++) {
                    $this->session_id.= $chrs[rand(0, 35)];
                    }
                $this->alias = "";
                // ______________________________________________ Set application name ___
                if ($nome) {
                    $this->nome = $nome;
                    }
                else {
                    $this->nome = preg_replace('/\..*$/i', '',
                                               basename($_SERVER['PHP_SELF']));
                    }
                $_SESSION['o2_app'] = $this;
                $this->crea_cartelle();
                break;
            }
        $this->host        = strtoupper($this->nome.'@'.gethostname());
        // ________________________________ Set application instance unique identifier ___
        $this->instance_id = hash('crc32b', $this->dir_home);
        if (!$this->titolo) {
            $this->titolo = $this->nome;
            }
        // __________________________________________________________ Default messages ___
        $this->msg_norec = "No records within selection";
        $this->msg_del   = "ATTENTION: current record will be deleted.\nProceed?";
        $this->msg_undo  = "ATTENTION: current record updates will be discarded.\n".
                           "Proceed?";
        // _________________________________________________ Application configuration ___
        $this->settings();
        // ______________________________________________________ Set error management ___
        if ($this->error_mode) {
            $this->set_error_mode();
            }
        // ___________________ Use OB to avoid blank lines from repositories to output ___
        ob_start();
        // ___________________________________ Repositories definition files inclusion ___
        $this->runtime->tools_repository();
        $this->doc_activate();
        $this->includi_repository($this->file_modelli);
        $this->includi_repository($this->file_db);
        $this->includi_repository($this->file_tabelle);
        $this->includi_repository($this->file_menu);
        $this->includi_repository($this->file_vars);
        ob_end_clean();
        $this->load_gateways();
        // ______________________________________________ Execution for internal tools ___
        if (isset($_REQUEST['jxtool'])) {
            $this->user            = 'jxtool';
            $this->split_tab_repos = false;
            }
        // _______________________________________________________ Scheduler execution ___
        elseif (isset($_REQUEST[o2_scheduler::$sched_word])) {
            $this->user = 'jxsys';
            }
        // ________________________________________________________ Standard execution ___
        else {
            $this->get_params();
            $this->create_tables();
            // ______________________________ Login program execution and user options ___
            $this->set_user($_REQUEST['user'], $_REQUEST['password']);
            $this->exe_prg_start = ($this->prg_start ? true : false);
            // ___________________________________________________ Maintenance Service ___
            if ($this->maint_time) {
                $this->check_maintenance();
                }
            }
        // __________________________________________________ Commit open transactions ___
        $this->commit_all(false);

        }


    /**
     * Define a batch application
     *
     * @param string $release
     */
    static function bat_app($release = "") {

        // ___________________________________________________ Provide HTML interfaces ___
        include_once $GLOBALS['o2_runtime']->root."lib/jxhtml.inc";
        // _______________________________________________________________ NEW SESSION ___
        $_SESSION['o2_app'] = new o2_app($release);
        $app                = $_SESSION['o2_app'];
        // _____________________________________________________________________ JXSQL ___
        if ($app->jxsql) {
            include_once $app->runtime->dir_dbms.'jxsql.inc';
            }
        // ____________________________ Server side extra script inclusion (extra.inc) ___
        if ($app->file_server_inc) {
            $app->include_extra();
            }
        // ______________________________ Run jobs if session is a worker (job-runner) ___
        if (o2_run_job::run_session()) { }
        // ____________________________ Run scheduler if session is a scheduler engine ___
        elseif (o2_scheduler::run_session()) { }
        // __________________________________________________ Application system tools ___
        elseif(isset($_REQUEST['jxtool'])) {
            switch (strtolower($_REQUEST['jxtool'])) {
                // _________________________________________________________ DB-export ___
                case 'dbexport':
                    $app->dbcopy('export',
                                 $_REQUEST['int_db'],
                                 $_REQUEST['ext_type'],
                                 $_REQUEST['ext_host'],
                                 $_REQUEST['ext_user'],
                                 $_REQUEST['ext_password'],
                                 $_REQUEST['ext_schema'],
                                 $_REQUEST['ext_db'],
                                 $_REQUEST['aspid']);
                    break;
                // _________________________________________________________ DB-import ___
                case 'dbimport':
                    $app->dbcopy('import',
                                 $_REQUEST['int_db'],
                                 $_REQUEST['ext_type'],
                                 $_REQUEST['ext_host'],
                                 $_REQUEST['ext_user'],
                                 $_REQUEST['ext_password'],
                                 $_REQUEST['ext_schema'],
                                 $_REQUEST['ext_db'],
                                 $_REQUEST['aspid']);
                    break;
                default:
                    throw new o2_exception("Tool \"".$_REQUEST['jxtool']."\" unknown.\n",
                                           o2error_EXECUTION);
                    die();
                    break;
                }
            }
        // _____________________________________________________ Application execution ___
        else {
            try {
                $app->esecutivo();
                }
            catch (o2_exception $o2e) {
                $o2e->send();
                return false;
                }
            }
        // __________________________________________________ Commit open transactions ___
        $app->commit_all(true);

        }


    /**
     * Sets error mode. Possible modes are:
     *  none   PHP settings are used from php.ini
     *  DEV    Development mode
     *  EXE    Production mode
     *
     */
    function set_error_mode() {

        switch ($this->error_mode) {
            case 'DEV':
                error_reporting(E_ALL & ~E_STRICT & ~E_NOTICE);
                ini_set('display_errors', false);
                ini_set('log_errors', true);
                break;
            case 'EXE':
                error_reporting(E_ALL &
                                ~E_WARNING &
                                ~E_DEPRECATED &
                                ~E_STRICT &
                                ~E_NOTICE);
                ini_set('display_errors', false);
                ini_set('log_errors', true);
                break;
            }

        }


    /**
     * Execute session logout: delete session record and destroy WEB session related
     * context.
     * If it is passed the client will be redirected to the $exit_url.
     * A list of parameters to be POSTed to the exit-url can be passaed as associtative
     * array, this way: ['par1'=>'val1', 'par2'=>'val2', ...]
     *
     * @param  string $exit_url
     * @param  array  $params
     * @return boolean
     */
    function logout($exit_url = "", $params = false) {

        // __________________________________________________________ Log logout event ___
        if ($this->access_log) {
            $this->log_access('O');
            }
        // _____________________________________________________ Remove session record ___
        $sess_tab = $this->get_table('o2_sessions');
        o2_gateway::deleterec($sess_tab->db->server->type,
                              $sess_tab->db->server->server,
                              $sess_tab->db->server->user,
                              $sess_tab->db->server->password,
                              $sess_tab->db->nome,
                              $sess_tab->db->proprietario,
                              $sess_tab->nome,
                              $sess_tab->indice,
                              $sess_tab->campi['sid']->nome_fisico." = '".
                                                                   $this->session_id."'");
        $this->commit_all(true);
        // ____________________________________________________ Remove user tmp-folder ___
        $this->dir_tmp->remove();
        // _____________________________________ Remove keepalive file from system TMP ___
        if ($this->keep_alive) {
            $tmp_d = new o2_dir(rtrim(sys_get_temp_dir(), '\\/').DIRECTORY_SEPARATOR);
            $tmp_f = $tmp_d->element('jx_'.$this->instance_id.'_'.$this->session_id);
            if ($tmp_d->exists($tmp_f->nome_completo)) {
                unlink($tmp_f->nome_completo);
                }
            }
        // ____________________________________________________ Remove session cookies ___
        if (session_id()) {
            if (ini_get("session.use_cookies")) {
                $cookies = session_get_cookie_params();
                setcookie(session_name(), '', 1, $cookies["path"], $cookies["domain"],
                                                 $cookies["secure"],$cookies["httponly"]);
                }
            session_destroy();
            }
        if ($exit_url) {
            $url = strtolower($exit_url);
            }
        elseif ($this->logout_url) {
            $url = strtolower($this->logout_url);
            if ((stripos($url, 'http://') === false) &&
                (stripos($url, 'https://') === false)) {
                $url = $this->alias.$url;
                }
            }
        else {
            $url = false;
            }
        if ($url) {
            if (!is_array($params) || (count($params) < 1)) {
                $params = '';
                }
            else {
                $params = ', '.json_encode($params);
                }
            if ($GLOBALS['jxjs']) {
                print "o2jse.cmd.post('".$url."'".$params.");\n}";
                }
            else {
                if (!$this->html_started) {
                    o2html::page_prefix();
                    }
                print "<script> setTimeout( function() { o2jse.cmd.post('".$url."'".
                                                                           $params.
                                                                         "); }, 300); ".
                      "</script></html>";
                }
            }
        die();

        }


    /**
     * Logs login, logout and login attempt events to database
     *
     * @param string  $type         Out, Failed, Expired or In
     * @param integer $timestamp    Timestamp (Expired only)
     * @param string  $sess_id      Session ID (Expired only)
     * @param string  $sess_user    User (Expired only)
     * @param string  $sess_app     Application name (Expired only)
     * @param string  $sess_runm    Run mode (Expired only)
     * @param string  $sess_term    Terminal ID (Expired only)
     * @param string  $sess_ip      IP address of remote client (Expired only)
     */
    function log_access($type       = '',
                        $timestamp  = 0,
                        $sess_id    = '',
                        $sess_user  = '',
                        $sess_app   = '',
                        $sess_host  = '',
                        $sess_runm  = '',
                        $sess_term  = 0,
                        $sess_ip    = '') {

        $tab = $this->get_table('jx_access_log');
        $f   = $tab->campi;
        switch (strtoupper($type)) {
            case 'O':
                $type     = 'O';
                $time     = time();
                $sessid   = $this->session_id;
                $user     = $this->user;
                $app      = $this->nome;
                $host     = $this->host;
                $run_mode = o2app_runmode();
                $terminal = $this->terminal_id;
                $ip       = $_SERVER['REMOTE_ADDR'];
                break;
            case 'F':
                $type     = 'F';
                $time     = time();
                $sessid   = $this->session_id;
                $user     = (isset($_REQUEST['user']) ? $_REQUEST['user'] : $this->user);
                $app      = $this->nome;
                $host     = $this->host;
                $run_mode = o2app_runmode();
                $terminal = $this->terminal_id;
                $ip       = $_SERVER['REMOTE_ADDR'];
                break;
            case 'E':
                $type     = 'E';
                $time     = $timestamp;
                $sessid   = $sess_id;
                $user     = $sess_user;
                $app      = $sess_app;
                $host     = $sess_host;
                $run_mode = $sess_runm;
                $terminal = $sess_term;
                if ($sess_ip) {
                    $ip = $sess_ip;
                    }
                else {
                    // ______________________________ Get IP from related Login record ___
                    $co    = constant('o2_'.$tab->db->server->type.'_o');
                    $cc    = constant('o2_'.$tab->db->server->type.'_c');
                    $where = $f['session_id']."='".$sessid."' AND ".
                             $f['access_flag']."='I' AND ".
                             $f['app_name']."='".$app."' AND ".
                             $f['user']."='".$user."' AND ".
                             $f['terminal']."=".$terminal;
                    $rec   = o2_gateway::recordset($tab->db->server->type,
                                                   $tab->db->server->server,
                                                   $tab->db->server->user,
                                                   $tab->db->server->password,
                                                   $tab->db->nome,
                                                   $tab->db->proprietario,
                                                   $tab->nome,
                                                   $tab->nome,
                                                   $f['client_ip'].' '.$co.'CIP'.$cc,
                                                   $where,
                                                   false,
                                                   1);
                    $ip    = $rec[0]['CIP'];
                    }
                break;
            case 'I':
            default:
                $type     = 'I';
                $time     = time();
                $sessid   = $this->session_id;
                $user     = $this->user;
                $app      = $this->nome;
                $host     = $this->host;
                $run_mode = o2app_runmode();
                $terminal = $this->terminal_id;
                $ip       = $_SERVER['REMOTE_ADDR'];
                break;
            }
        $fields               = array($f['timestamp']->nome_fisico,
                                      $f['access_flag']->nome_fisico,
                                      $f['user']->nome_fisico,
                                      $f['app_name']->nome_fisico,
                                      $f['host']->nome_fisico,
                                      $f['run_mode']->nome_fisico,
                                      $f['client_ip']->nome_fisico,
                                      $f['session_id']->nome_fisico,
                                      $f['terminal']->nome_fisico,
                                      $f['developer']->nome_fisico);
        $values               = array("'".$time."'",
                                      "'".$type."'",
                                      "'".$user."'",
                                      "'".$app."'",
                                      "'".$host."'",
                                      "'".$run_mode."'",
                                      "'".$ip."'",
                                      "'".$sessid."'",
                                      $terminal,
                                      "'".$this->developer."'");
        $t                    = $this->isolated_trans;
        $this->isolated_trans = true;
        $o2insertrec          = 'o2_'.$tab->db->server->type.'_insertrec';
        $error                = false;
        try {
            $o2insertrec($tab->db->server->server,
                         $tab->db->server->user,
                         $tab->db->server->password,
                         $tab->db->nome,
                         $tab->db->proprietario,
                         $tab->nome,
                         $tab->nome,
                         $fields,
                         $values);
            }
        // __________________________________ Hide duplicated key errors on access-log ___
        catch (Exception $e) {
            o2_exception::warning('Access-log duplicated entry:<br>Log type: '.$type.
                                  '<br>Timestamp: '.$timestamp.
                                  '<br>Session ID: '.$sess_id.
                                  '<br>User :'.$sess_user.
                                  '<br>Application: '.$sess_app.
                                  '<br>Host: '.$sess_host.
                                  '<br>Run mode: '.$sess_runm.
                                  '<br>Terminal: '.$sess_term.
                                  '<br>IP: '.$sess_ip);
            $e->cancel($tab->db->server);
            $error = true;
            }
        if (!$error) {
            o2_gateway::commit($tab->db->server->type,
                               $tab->db->server->server,
                               $tab->db->server->user,
                               $tab->db->server->password,
                               true);
            }
        $this->isolated_trans = $t;

        }


    /**
     * Change databases definition for ASP mode Schema and Database
     *
     */
    function set_aspid() {

        $asp = strtolower($this->vars['_area']->valore);
        foreach ($this->db as $db_idx => $db) {
            // _______________________________________________________ Manage ASP mode ___
            switch ($db->asp) {
                case 'D': // _________________________________________ ASP by Database ___
                    $this->db[$db_idx]->nome = $db->base_name.($asp ? '_'.$asp : '');
                    break;
                case 'S': // ___________________________________________ ASP by Schema ___
                    $this->db[$db_idx]->proprietario = $db->base_schema.
                                                       ($asp ? '_'.$asp : '');
                    break;
                default: // __________________________________ ASP by Column or no ASP ___
                    break;
                }
            }

        }


    /**
     * Creates unexisting tables for the whole application
     *
     * @return array
     */
    function create_tables() {

        $app_tabs     = array();
        $missing_tabs = array();
        $tabs_list    = array();
        $custom_map   = false;
        // ___________________________________________________ Custom tables remapping ___
        $this->remap_tables_on_db();
        // ___________________________________________________________ Tables creation ___
        if ($this->tables_create) {
            $this->upgrade_systabs();
            // ____________________________________________________ Group tables by db ___
            foreach ($this->tab as $tab_obj) {
                if ($tab_obj->db->create_tabs) {
                    $app_tabs[$tab_obj->db->id][] = $tab_obj->nome;
                    }
                }
            // ___________________________________________ Get full tables list per db ___
            foreach (array_keys($app_tabs) as $single_db) {
                $tabs_list[$single_db] = $this->db[$single_db]->tabelle();
                }
            // _____________________________________ Create missing tables list per db ___
            foreach ($app_tabs as $single_db => $db_tabs) {
                $missing_tabs = array_merge($missing_tabs,
                                            array_diff($db_tabs, $tabs_list[$single_db]));
                }
            // __________________________________________ Create missing tables per db ___
            foreach ($this->tab as $tab_obj) {
                if (in_array($tab_obj->nome, $missing_tabs)) {
                    $tab_obj->crea(true);
                    }
                }
            }
        // ______________________________________ If splitted table repository is used ___
        if ($this->split_tab_repos) {
            // ____________________________ Splitted table repository auto maintenance ___
            $tab_rep = new o2_fsitem($this->dir_progs.$this->file_tabelle);
            $dir_rep = new o2_dir($this->dir_progs."jx_tables".DIRECTORY_SEPARATOR);
            if (!$dir_rep->exists() ||
                ($dir_rep->data_modifica < $tab_rep->data_modifica ||
                 ($dir_rep->data_modifica == $tab_rep->data_modifica &&
                  $dir_rep->ora_modifica < $tab_rep->ora_modifica)) ||
                ($custom_map &&
                 ($dir_rep->data_modifica < $map_rep->data_modifica ||
                  ($dir_rep->data_modifica == $map_rep->data_modifica &&
                   $dir_rep->ora_modifica < $map_rep->ora_modifica)))) {
                $this->intcall("tools/o2dev_split_rep");
                }
            $this->tab = array();
            // __________________________________________________ Reload system tables ___
            require($this->runtime->root."lib/prgs/tables.inc");
            // ___________________________ Reload sys tables customized in application ___
            foreach ($this->tab as $sys_tab) {
                $def_file = $this->dir_progs."jx_tables".DIRECTORY_SEPARATOR.
                            $sys_tab->indice.".tab";
                if (file_exists($def_file)) {
                    include_once($def_file);
                    }
                }
            }
        $this->commit_all(false);

        }


    /**
     * Remap tables on dbs as defined in tables_map.ini
     *
     */
    function remap_tables_on_db() {

        // __________________________________________ Custom tables remapping from CVS ___
        if ($this->run_level == "DEV" && $this->dir_cvs_progs->exists('tables_map.ini')) {
            $custom_map = $this->dir_cvs_progs.'tables_map.ini';
            }
        // ___________________________________________________ Custom tables remapping ___
        elseif ($this->dir_progs->exists('tables_map.ini')) {
            $custom_map = $this->dir_progs.'tables_map.ini';
            }
        // _____________________________ Custom tables remapping from extra prgs paths ___
        elseif ($this->prgs_path_inc) {
            foreach ($this->prgs_path_inc as $path) {
                $dir = new o2_dir($path);
                if ($dir->exists('tables_map.ini')) {
                    $custom_map = $dir.'tables_map.ini';
                    break;
                    }
                }
            }
        if ($custom_map && $map_conf = @parse_ini_file($custom_map)) {
            foreach ($map_conf as $map_tab => $map_db) {
                if (isset($this->tab[$map_tab]) && isset($this->db[$map_db])) {
                    $this->tab[$map_tab]->set_db($this->db[$map_db]);
                    }
                }
            }

        }


    /**
     * Creates unexisting log tables for the whole application
     *
     */
    function create_logtabs() {

        // _______________ Tables log - If tables logging is active creates log tables ___
        if ($this->tables_log && count($this->logged_tabs)) {
            foreach ($this->logged_tabs as $tab_name => $log_level) {
                if ($tab_name) {
                    try {
                        $tab_obj            = $this->get_table($tab_name);
                        $tab_obj->log_level = $log_level;
                        $tab_obj->create_logtable($this->tables_create);
                        }
                    catch (o2_exception $o2e) {
                        o2_exception::warning("Unknown table <i>".$tab_name.
                                              "</i> requested for log-table creation.");
                        }
                    }
                }
            }
        $this->commit_all(false);

        }


    /**
     * Check system tables compatibility.
     * If a REQUEST (HTML) or line command parameter named jxsys_dbupgrade is passed
     * and user is logged in as a developer, then it applies upgrade script for system
     * tables which changed structure, else it outs a release compatibility error.
     *
     */
    function upgrade_systabs() {

        $dbupgrade           = isset($_REQUEST['jxsys_dbupgrade']) &&
                               $this->run_level == "DEV";
        $sys_tabs            = $this->runtime->sys_tabs();
        $tabs_by_db          = array();
        $missing_tabs        = array();
        $exist_db            = array();
        list($rnt_v, $rnt_r) = explode(".", $this->runtime->release);
        $rnt_rel             = trim($rnt_v).".".trim($rnt_r);
        // ________________________________________________________ Group tables by db ___
        foreach ($sys_tabs as $single_tab_name) {
            $single_tab = $this->tab[$single_tab_name];
            if ($single_tab->exists()) {
                $exist_db[$single_tab->db->id]     = true;
                $tabs_by_db[$single_tab->db->id][] = $single_tab_name;
                }
            else {
                $missing_tabs[$single_tab->db->id][] = $single_tab_name;
                }
            }
        foreach ($tabs_by_db as $single_db => $db_tabs) {
            // __________________________________ Skip DBs without table-creation flag ___
            if (!$this->db[$single_db]->create_tabs) {
                continue;
                }
            $db_rel = trim($this->db[$single_db]->get_sysrel());
            // _________________________________________ If an OLDER database is found ___
            if ($db_rel && ($rnt_rel > $db_rel)) {
                // ____________________________________ Check if any upgrade is needed ___
                $needed = false;
                foreach ($db_tabs as $single_tab) {
                    if (o2_upgrade($this->tab[$single_tab], $db_rel)) {
                        $needed = true;
                        break;
                        }
                    else {
                        $this->db[$single_db]->set_sysrel($rnt_rel);
                        }
                    }
                // ______________________________________________ If upgrade is needed ___
                if ($needed || $dbupgrade) {
                    // ____________________________________ If upgrade function called ___
                    if ($dbupgrade) {
                        foreach ($db_tabs as $single_tab) {
                            o2_upgrade($this->tab[$single_tab], $db_rel);
                            }
                        $this->db[$single_db]->set_sysrel($rnt_rel);
                        }
                    // __________________________________ If user is developer or root ___
                    elseif (o2user_is_admin() || $this->run_level == "DEV") {
                        switch ($this->runtime->interface) {
                            case "HTML":
                                o2html::page_prefix();
                                print "</form>\n<form method='POST' action='".
                                      $this->referer.
                                      "'><table class='o2_error_report'><tr>".
                                      "<td class='o2_error_text'>Database <i>".$single_db.
                                      "</i> (<i>".$this->db[$single_db]->nome.
                                      "</i> on <i>".$this->db[$single_db]->server->server.
                                      "</i>) needs to be upgraded to a newer release.".
                                      "<br> Runtime release: <b>".$rnt_rel.
                                      "</b><br> Database release: <b>".$db_rel.
                                      "</b></td></tr><tr><td class='o2_error_info'>".
                                      "<center>Automatic conversion</center></td></tr>".
                                      "<tr><td class='o2_error_text'>Database can be ".
                                      "automatically converted to the current version ".
                                      "but provide to backup database data before ".
                                      "proceeding!<br><br>Press button to convert ".
                                      "database.</td></tr><tr><td><center><input ".
                                      "type='submit' value='Convert' ".
                                      "class='o2_ctrl_button'></center>".
                                      "<input type='hidden' name='user' value='".
                                      $this->user.
                                      ($this->block_md5_pwd ?
                                      "'><input type='hidden' name='jxotp' value='".
                                      $this->otp() :
                                      "'><input type='hidden' name='password' value='".
                                      $this->password).
                                      "'><input type='hidden' name='dev' value='".
                                      $this->developer.
                                      "'><input type='hidden' name='key' value='".
                                      $this->runtime->developers[$this->developer].
                                      "'><input type='hidden' name='jxsys_dbupgrade' ".
                                      "value='1'></tr></td></table></form>\n".
                                      "</body></html>";
                                die();
                                break;
                            default:
                                print "\n\n     *** VERSION MATCHING FAILURE ***\n\n".
                                      "Database ".$single_db." (".
                                      $this->db[$single_db]->nome." on ".
                                      $this->db[$single_db]->server->server.
                                      ") needs to be upgraded to a newer release.\n".
                                      " Runtime release: ".$rnt_rel.
                                      "\n Database release: ".$db_rel.
                                      "\n\n     *** Automatic conversion ***\n\n".
                                      "Run conversion tool for automatic conversion or ".
                                      "visit www.janox.it for further informations.\n\n";
                                die();
                                break;
                            }
                        }
                    // ___________________________________________ If a standard login ___
                    else {
                        throw new o2_exception("Database <i>".$single_db."</i> needs to ".
                                               "be upgraded to a newer release.<br>".
                                               " Runtime release: <b>".$rnt_rel.
                                               "</b><br> Database release: <b>".$db_rel.
                                               "</b><br>Please contact system ".
                                               "administrator for conversion.",
                                               o2error_RELEASE);
                        die();
                        }
                    }
                }
            // __________________________________________ If a NEWER database is found ___
            elseif ($rnt_rel < $db_rel) {
                throw new o2_exception("Database <i>".$single_db.
                                       "</i> has already been upgraded to a newer ".
                                       "release.<br> Runtime release: <b>".$rnt_rel.
                                       "</b><br> Database release: <b>".$db_rel.
                                       "</b><br>Please change database for system tables".
                                       " or upgrade runtime version.",
                                       o2error_RELEASE);
                die();
                }
            }
        foreach ($missing_tabs as $single_db => $db_tabs) {
            if (!isset($exist_db[$single_db])) {
                $this->db[$single_db]->set_sysrel($rnt_rel);
                }
            // _________________________________________ Creates missing system tables ___
            foreach ($db_tabs as $single_tab) {
                $this->tab[$single_tab]->crea(true);
                }
            }


        }

    /**
     * Check last maintenance execution and if needed creates and run maintenance batch
     * job
     *
     */
    function check_maintenance() {

        $app = $_SESSION['o2_app'];
        $job = o2_job::get_by_name('jxmaintenance');
        // __________________ Check maintenance job existance and create it if missing ___
        if (!$job) {
            $job     = new o2_job(0,
                                  'jxmaintenance',
                                  'tools/jxmaintenance',
                                  array(),
                                  'Janox Maintenance Service',
                                  ceil($app->maint_time / 24),
                                  1,
                                  'B');
            $job_id  = $job->create();
            $job->id = $job_id;
            }
        else {
            $job_id = $job->id;
            }
        // ______________________________ Get last maintenance execution for this host ___
        $last_exe = o2_run_job::get_last_job($job_id, '', '', 0, $app->host);
        if (!$last_exe || (($last_exe->start + ($app->maint_time * 3600)) < time())) {
            $job->run(false);
            }

        }


    /**
     * Executes maintenance service actions on application.
     *
     * TODO: Add execution/error logging
     *
     */
    function maintenance() {

        $sessions = $this->get_table('o2_sessions');
        $db       = $sessions->db;
        $server   = $db->server;
        $co       = constant("o2_".$server->type."_o");
        $cc       = constant("o2_".$server->type."_c");
        $f        = $sessions->campi;
        // ==================================================== Clear expired sessions ===
        $now      = time();
        $today    = "'".date('Ymd', $now)."'";
        $now      = "'".date('His', $now)."'";
        $where    = $f['e_date'].'<'.$today.' OR ('.
                    $f['e_date'].'='.$today.' AND '.$f['e_time'].'<'.$now.')';
        $recs     = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $sessions->nome,
                                          "jxsessions",
                                          $f['sid'].' '.$co.'SID'.$cc.', '.
                                          $f['o2user'].' '.$co.'USER'.$cc.', '.
                                          $f['app_name'].' '.$co.'APP'.$cc.', '.
                                          $f['host'].' '.$co.'HOST'.$cc.', '.
                                          $f['run_mode'].' '.$co.'RUNM'.$cc.', '.
                                          $f['terminal_id'].' '.$co.'TERM'.$cc.', '.
                                          $f['e_date'].' '.$co.'DATE'.$cc.', '.
                                          $f['e_time'].' '.$co.'TIME'.$cc,
                                          $where,
                                          false,
                                          10000);
        o2_gateway::deleterec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $sessions->nome,
                              "jxsessions",
                              $where);
        // ______________________________________________________ Log expired sessions ___
        if ($this->access_log) {
            foreach ($recs as $rec) {
                $dt = o2_date_obj($rec['DATE'], $rec['TIME']);
                $this->log_access('E',
                                  $dt->getTimestamp(),
                                  $rec['SID'],
                                  $rec['USER'],
                                  $rec['APP'],
                                  $rec['HOST'],
                                  $rec['RUNM'],
                                  $rec['TERM']);
                }
            }
        // ============================= Clear jobs sessions without process (run-job) ===
        $runjobs  = $this->get_table(o2_run_job::$run_table);
        $run_db   = $runjobs->db;
        $run_srv  = $run_db->server;
        $co       = constant("o2_".$run_srv->type."_o");
        $cc       = constant("o2_".$run_srv->type."_c");
        $recs     = o2_gateway::recordset($run_srv->type,
                                          $run_srv->server,
                                          $run_srv->user,
                                          $run_srv->password,
                                          $run_db->nome,
                                          $run_db->proprietario,
                                          $runjobs->nome,
                                          "jxrunjobs",
                                          $runjobs->campi['run_sessid'].' '.$co.'SID'.$cc,
                                          false,
                                          false,
                                          10000);
        $run_list = array();
        // __________________________________________ Get list of SessIDs on processes ___
        foreach ($recs as $rec) {
            $run_list[] = $rec['SID'];
            }
        $co    = constant("o2_".$server->type."_o");
        $cc    = constant("o2_".$server->type."_c");
        $where = $f['run_mode']."='JOB'";
        $recs  = o2_gateway::recordset($server->type,
                                       $server->server,
                                       $server->user,
                                       $server->password,
                                       $db->nome,
                                       $db->proprietario,
                                       $sessions->nome,
                                       "jxsessions",
                                       $f['sid'].' '.$co.'SID'.$cc,
                                       $where,
                                       false,
                                       10000);
        // ______________________________________ Get list of sessions created by jobs ___
        $job_sess = array();
        foreach ($recs as $rec) {
            $jobs_sess[] = $rec['SID'];
            }
        // ____________________________________ Delete session records without process ___
        foreach ($jobs_sess as $sid) {
            if (!in_array($sid, $run_list)) {
                $where = $f['sid']."='".$sid."'";
                o2_gateway::deleterec($server->type,
                                      $server->server,
                                      $server->user,
                                      $server->password,
                                      $db->nome,
                                      $db->proprietario,
                                      $sessions->nome,
                                      "jxsessions",
                                      $where);
                }
            }
        // ====================================== Clear obsolete users TMP directories ===
        $recs        = o2_gateway::recordset($server->type,
                                             $server->server,
                                             $server->user,
                                             $server->password,
                                             $db->nome,
                                             $db->proprietario,
                                             $sessions->nome,
                                             "jxsessions",
                                             $f['sid'].' '.$co.'SID'.$cc,
                                             false,
                                             false,
                                             10000);
        $active_sess = array();
        // _______________________________________________ Get list of active sessions ___
        foreach ($recs as $rec) {
            $active_sess[] = $rec['SID'];
            }
        // ____________________________________________________ Loop on users TMP dirs ___
        $users_dirs = new o2_dir($this->dir_home."/users/");
        foreach ($users_dirs->all_elements() as $users_dir) {
            if ($users_dir->tipo == "D") {
                $tmp_dirs = new o2_dir($users_dir);
                foreach ($tmp_dirs->all_elements() as $tmp_dir) {
                    // ____________________________________ Check if session is active ___
                    if ($tmp_dir->tipo == "D" &&
                        !in_array(substr($tmp_dir->nome, 3), $active_sess)) {
                        $tmp_dir->remove();
                        }
                    }
                }
            }
        // ___________________________________________ Delete tmp files for keep-alive ___
        $tmp_dir = new o2_dir(rtrim(sys_get_temp_dir(), '\\/').DIRECTORY_SEPARATOR);
        foreach ($tmp_dir->all_elements('jx_'.$this->instance_id.'_*') as $tmp_f) {
            // ____________________________________________ Check if session is active ___
            if (!in_array(substr($tmp_f->nome, 12), $active_sess)) {
                unlink($tmp_f->nome_completo);
                }
            }
        // ======================================================== Clear expired OTPs ===
        $otps   = $this->get_table('jx_otp');
        $db     = $otps->db;
        $server = $db->server;
        $f      = $otps->campi;
        $now    = time();
        $where  = $f['create_time'].'<'.($now - $this->otp_time);
        o2_gateway::deleterec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $otps->nome,
                              "jx_otp",
                              $where);

        }


    /**
     * Generates a One Time Password for application user.
     * OTP is a time-based access code provided to user to override his standard password
     * and can be used only once.
     * Returned OTP is a string of $otp_len chars, default length is 6.
     * If $user is blank then OTP is generated for the currently logged user.
     *
     * @param  string $user       User to generate the OTP for
     * @param  integer $otp_len   Length of the OTP token
     * @return string
     */
    function otp($user = '', $otp_len = 6) {

        if (!$user) {
            $user = $this->user;
            }
        $code   = substr(str_shuffle(str_repeat('0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                                                '1'.substr($this->runtime->microtime(),
                                                           -1) + 0)), -$otp_len);
        $tab    = $this->get_table('jx_otp');
        $db     = $tab->db;
        $server = $db->server;
        $fields = array($tab->campi['code']->nome_fisico,
                        $tab->campi['user']->nome_fisico,
                        $tab->campi['create_time']->nome_fisico);
        $values = array("'".md5($code)."'", "'".$user."'", time());
        $trans  = $this->isolated_trans;
        $this->isolated_trans = $this->progressivo_istanze;
        o2_gateway::insertrec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              "jx_otp",
                              $fields,
                              $values);
        o2_gateway::commit($server->type,
                           $server->server,
                           $server->user,
                           $server->password);
        $this->isolated_trans = $trans;
        return $code;

        }


    /**
     * Retrieves parameters from http query and loads values in application variables, so
     * that URL (or path for cli) parameters can be used to fill application variables
     * values.
     *
     */
    function get_params() {

        foreach ($_REQUEST as $par_name => $par_value) {
            if (isset($this->vars[$par_name])) {
                $this->vars[$par_name]->valore = $par_value;
                }
            }

        }


    /**
     * Returns decimals separator point
     *
     * @return string
     */
    function decimals_point() {

        $dec_local = $this->option_get('decimals');
        return ($dec_local !== false ? $dec_local : $this->runtime->decimals_point);

        }


    /**
     * Returns thousands separator point
     *
     * @return string
     */
    function thousands_point() {

        $tho_local = $this->option_get('thousands');
        return ($tho_local !== false ? $tho_local : $this->runtime->thousands_point);

        }


    /**
     * Returns the list of ASP-IDs.
     *
     * ASP-IDs can be gotten in 2 ways, depending on app parameter "asp_areas_table":
     *  - from system table "jx_asp_areas";
     *  - as a DISTINCT on defined users.
     *
     * @return array
     */
    function asp_codes() {

        // _____________________________ Get the list from the system table for ASPIDs ___
        if ($this->use_asp_table) {
            $users  = $this->get_table('o2_users');
            $db     = $users->db;
            $server = $db->server;
            $co     = constant("o2_".$server->type."_o");
            $cc     = constant("o2_".$server->type."_c");
            $area   = $users->campi['area']->nome_fisico;
            $list   = o2_gateway::recordset($server->type,
                                            $server->server,
                                            $server->user,
                                            $server->password,
                                            $db->nome,
                                            $db->proprietario,
                                            $users->nome,
                                            $users->nome,
                                            "DISTINCT ".$area." ".$co."AREA".$cc,
                                            false,
                                            false,
                                            1000000);
            }
        // ____________________________ Get the list of ASP-IDs defined in users table ___
        else {
            $asps   = $this->get_table('jx_asp_areas');
            $db     = $asps->db;
            $server = $db->server;
            $co     = constant("o2_".$server->type."_o");
            $cc     = constant("o2_".$server->type."_c");
            $area   = $asps->campi['asp_area']->nome_fisico;
            $list   = o2_gateway::recordset($server->type,
                                            $server->server,
                                            $server->user,
                                            $server->password,
                                            $db->nome,
                                            $db->proprietario,
                                            $asps->nome,
                                            $asps->nome,
                                            $area." ".$co."AREA".$cc,
                                            false,
                                            false,
                                            1000000);
            }
        $ret = array();
        foreach ($list as $rec) {
            if ($rec["AREA"]) {
                $ret[] = $rec["AREA"];
                }
            }
        sort($ret);
        return $ret;

        }


    /**
     * Gestisce la compatibilit� tra la versione dell'applicazione e quella delle classi
     *
     *                              GESTIONE DELLE VERSIONI
     *
     *   La versione viene indicata con una stringa nella forma [ver].[sub].[rel]
     *   Il tipo di dato versione � un array composto da tre interi numerici:
     *      [ver] = Macro-versione commerciale
     *              Non compatibilit� nelle due direzioni
     *      [sub] = Versione sintattica
     *              Non compatibilit� nelle due direzioni
     *      [rel] = Release
     *              indica una diversa release delle classi ma sintatticamente
     *              compatibile nelle due direzioni
     *
     *
     * @param string $versione
     */
    function versione($versione = "") {

        $release_type             = array('ver' => 0, 'sub' => 0, 'rel' => 0);
        $rnt_release              = $release_type;
        $app_release              = $release_type;
        list($rnt_release['ver'],
             $rnt_release['sub'],
             $rnt_release['rel']) = array_pad(explode(".", $this->runtime->release, 3),
                                              3, 0);
        list($app_release['ver'],
             $app_release['sub'],
             $app_release['rel']) = array_pad(explode(".", $versione, 3), 3, 0);
        $rnt_release['ver']       = intval($rnt_release['ver']);
        $rnt_release['sub']       = intval($rnt_release['sub']);
        $rnt_release['rel']       = intval($rnt_release['rel']);
        $app_release['ver']       = intval($app_release['ver']);
        $app_release['sub']       = intval($app_release['sub']);
        $app_release['rel']       = intval($app_release['rel']);
        if ($rnt_release['ver'] > $app_release['ver'] ||
            ($rnt_release['ver'] == $app_release['ver'] &&
             $rnt_release['sub'] > $app_release['sub'])) {
            throw new o2_exception("<b>NO MATCHING VERSION!</b><br />Application ".
                                   "must be upgraded to version <b>".
                                   $rnt_release['ver'].".".$rnt_release['sub'].
                                   "</b><br />Visit <a href='www.janox.it'>".
                                   "www.janox.it</a> for further informations.",
                                   o2error_RELEASE);
            }
        elseif ($rnt_release['ver'] < $app_release['ver'] ||
                ($rnt_release['ver'] == $app_release['ver'] &&
                 $rnt_release['sub'] < $app_release['sub'])) {
            throw new o2_exception("<b>NO MATCHING VERSION!</b><br />Application has ".
                                   "been developed for a newer runtime version.".
                                   "<br />Visit <a href='www.janox.it'>".
                                   "www.janox.it</a> for further informations.",
                                   o2error_RELEASE);
            }
        else {
            $this->versione = $versione;
            }

        }


    /**
     * Use provided list of parameters, else reads application INI file, and sets
     * application settings, overridding runtime settings, where needed.
     *
     *  @param array $settings   List of key/value pairs for providing settings
     */
    function settings($settings = false) {

        if ($settings && is_array($settings)) {
            $conf = $settings;
            }
        else {
            // ___________________________________ Read application configuration file ___
            if ($this->dir_home->exists($this->nome.".ini")) {
                $conf = parse_ini_file($this->dir_home->element($this->nome.".ini"));
                // _________________________________________ Override runtime settings ___
                $conf = array_merge($this->runtime->conf_list, $conf);
                }
            // __________________________ If application configuration file is missing ___
            else {
                $buffer_local = "[app]\n".
                                "models       = \"mod_repository.inc\"\n".
                                "dbs          = \"db_repository.inc\"\n".
                                "tables       = \"file_repository.inc\"\n".
                                "menus        = \"menu_repository.inc\"\n".
                                "vars         = \"vars_repository.inc\"\n".
                                "viewmodels   = \"viewmodels_repository.inc\"\n".
                                "encoding     = \"windows-1252\"\n".
                                "validate     = 0\n".
                                "timeout      = 10\n".
                                "combo2lookup = on\n".
                                "zoom         = on\n".
                                "log_backup   = on\n".
                                "log_rebuild  = on\n".
                                "maintprg     = \n".
                                "maint_time   = 100\n";
                file_put_contents($this->dir_home."/".$this->nome.".ini.std",
                                  $buffer_local);
                throw new o2_exception("Cannot find setting file <i>".$this->nome.
                                       ".ini</i><br />Default one created in <i>".
                                       $this->dir_home."</i>",
                                       o2error_MISSINGFILE);
                return false;
                }
            }
        // ___________________________________________________ Set application options ___
        if (isset($conf['models'])) {
            $this->file_modelli = $conf['models'];
            }
        if (isset($conf['dbs'])) {
            $this->file_db = $conf['dbs'];
            }
        if (isset($conf['tables'])) {
            $this->file_tabelle = $conf['tables'];
            }
        if (isset($conf['menus'])) {
            $this->file_menu = $conf['menus'];
            }
        if (isset($conf['vars'])) {
            $this->file_vars = $conf['vars'];
            }
        if (isset($conf['viewmodels'])) {
            $this->file_viewmodels = $conf['viewmodels'];
            }
        if (isset($conf['split_rep'])) {
            $this->split_tab_repos = $this->run_level == "EXE" && $conf['split_rep'];
            }
        if (isset($conf['asp_areas_table'])) {
            $this->use_asp_table = ($conf['asp_areas_table'] ? true : false);
            }
        if (isset($conf['tables_create'])) {
            $this->tables_create = ($conf['tables_create'] ? true : false);
            }
        if (isset($conf['tables_log'])) {
            $this->tables_log = ($conf['tables_log'] ? true : false);
            }
        if (isset($conf['fetch_loop'])) {
            $this->fetch_loop = ($conf['fetch_loop'] ? true : false);
            }
        if (isset($conf['log_db'])) {
            $this->log_db = $conf['log_db'];
            }
        if (isset($conf['log_rebuild'])) {
            $this->log_rebuild = $conf['log_rebuild'];
            }
        if (isset($conf['log_backup'])) {
            $this->log_backup = $conf['log_backup'];
            }
        if (isset($conf['db_timeout_lock'])) {
            $this->db_timeout_lock = $conf['db_timeout_lock'];
            }
        if (isset($conf['db_timeout_stm'])) {
            $this->db_timeout_stm = $conf['db_timeout_stm'];
            }
        if (isset($conf['timeout'])) {
            $this->timeout = intval($conf['timeout']);
            }
        if (isset($conf['keepalive'])) {
            $this->keep_alive = $conf['keepalive'];
            }
        if (isset($conf['validate'])) {
            $this->validazione = intval($conf['validate']);
            }
        if (isset($conf['password_strength'])) {
            $this->password_strength = intval($conf['password_strength']);
            }
        if (isset($conf['password_change'])) {
            $this->password_change = intval($conf['password_change']);
            }
        if (isset($conf['block_md5_password'])) {
            $this->block_md5_pwd = ($conf['block_md5_password'] ? true : false);
            }
        if (isset($conf['ldap_server'])) {
            $this->ldap_server = $conf['ldap_server'];
            }
        if (isset($conf['ldap_search_domain'])) {
            $this->ldap_search_domain = $conf['ldap_search_domain'];
            }
        if (isset($conf['ldap_base_dn'])) {
            $this->ldap_base_dn = $conf['ldap_base_dn'];
            }
        if (isset($conf['encoding'])) {
            $this->chr_encoding = $conf['encoding'];
            }
        if (isset($conf['alias']) &&
            $conf['alias'] &&
            $this->runtime->interface == 'HTML') {
            $this->alias = $conf['alias'];
            }
        if (isset($conf['title'])) {
            $this->titolo = $conf['title'];
            }
        if (isset($conf['activemenu'])) {
            $this->menu_attivo = $conf['activemenu'];
            }
        if (isset($conf['menu_style'])) {
            $this->menu_style = strtoupper(substr($conf['menu_style'], 0, 1));
            }
        if (isset($conf['css'])) {
            $this->css = $conf['css'];
            }
        if (isset($conf['nologin'])) {
            $this->no_login = $conf['nologin'];
            }
        if (isset($conf['lockpage'])) {
            $this->lock_page = $conf['lockpage'];
            }
        if (isset($conf['logout_url'])) {
            $this->logout_url = $conf['logout_url'];
            }
        if (isset($conf['access_log'])) {
            $this->access_log = intval($conf['access_log']);
            }
        if (isset($conf['sesserror'])) {
            $this->sess_error = $conf['sesserror'];
            }
        if (isset($conf['header'])) {
            $this->pagina_prima = $conf['header'];
            }
        if (isset($conf['footer'])) {
            $this->pagina_dopo = $conf['footer'];
            }
        if (isset($conf['jxsql'])) {
            $this->jxsql = ($conf['jxsql'] ? true : false);
            }
        if (isset($conf['prepared_statements'])) {
            $this->prepared_stmts = ($conf['prepared_statements'] ? true : false);
            }
        if (isset($conf['snapshot_db'])) {
            $this->snapshot_db = $conf['snapshot_db'];
            }
        if (isset($conf['extrainc'])) {
            $this->file_server_inc = $conf['extrainc'];
            }
        if (isset($conf['htmlhead'])) {
            $this->file_client_inc = $conf['htmlhead'];
            }
        if (isset($conf['reporter'])) {
            $this->reporter = $conf['reporter'];
            }
        if (isset($conf['startprg'])) {
            $this->prg_start = $conf['startprg'];
            }
        if (isset($conf['reqprg'])) {
            $this->prg_request = $conf['reqprg'];
            }
        if (isset($conf['unlockprg'])) {
            $this->prg_unlock = $conf['unlockprg'];
            }
        if (isset($conf['prgs_path_inc'])) {
            $this->prgs_path_inc = array();
            foreach(explode(';', $conf['prgs_path_inc']) as $path) {
                if (strcspn($path, '\\/', 0, 1) &&
                    !preg_match('/\w\:/', $path[0].$path[1])) {
                    if ($path[0] == '.') {
                        $path = $this->dir_home.ltrim(substr($path, 1), '\\/');
                        }
                    else {
                        $path = $this->dir_home.$path;
                        }
                    }
                if (is_dir($path)) {
                    $this->prgs_path_inc[] = rtrim(realpath($path), '\\/').
                                             DIRECTORY_SEPARATOR;
                    }
                }
            }
        if (isset($conf['templates_path_inc'])) {
            $this->templ_path_inc = array();
            foreach(explode(';', $conf['templates_path_inc']) as $path) {
                if (strcspn($path, '\\/', 0, 1) &&
                    !preg_match('/\w\:/', $path[0].$path[1])) {
                    if ($path[0] == '.') {
                        $path = $this->dir_home.ltrim(substr($path, 1), '\\/');
                        }
                    else {
                        $path = $this->dir_home.$path;
                        }
                    }
                if (is_dir($path)) {
                    $this->templ_path_inc[] = rtrim(realpath($path), '\\/').
                                              DIRECTORY_SEPARATOR;
                    }
                }
            }
        if (isset($conf['msg_del'])) {
            $this->msg_del = $conf['msg_del'];
            }
        if (isset($conf['msg_undo'])) {
            $this->msg_undo = $conf['msg_undo'];
            }
        if (isset($conf['msg_norec'])) {
            $this->msg_norec = $conf['msg_norec'];
            }
        if (isset($conf['fastmsg_time'])) {
            $this->fastmsg_time = max(3, intval($conf['fastmsg_time']));
            }
        if (isset($conf['zoom'])) {
            $this->zoom = ($conf['zoom'] ? true : false);
            }
        if (isset($conf['error_mode'])) {
            switch (strtoupper($conf['error_mode'])) {
                case 'DEV':
                    $this->error_mode = 'DEV';
                    break;
                case 'EXE':
                    $this->error_mode = 'EXE';
                    break;
                default:
                    $this->error_mode = false;
                    break;
                }
            }
        if (isset($conf['errors_log'])) {
            $this->errors_log = ($conf['errors_log'] ? true : false);
            }
        if (isset($conf['warning_log'])) {
            $this->warning_log = ($conf['warning_log'] ? true : false);
            }
        if (isset($conf['errorinc'])) {
            $this->errors_inc = $conf['errorinc'];
            }
        if (isset($conf['max_req_n'])) {
            $this->max_req_n = max($conf['max_req_n'], 1);
            }
        if (isset($conf['maint_time'])) {
            $this->maint_time = intval($conf['maint_time']);
            }
        if (isset($conf['doc'])) {
            $this->doc_enabled = ($conf['doc'] ? true : false);
            }
        if (isset($conf['refresh_prg'])) {
            $this->refresh_prg = $conf['refresh_prg'];
            }
        if (isset($conf['refresh_time'])) {
            $this->refresh_time = max($conf['refresh_time'], 3);
            }
        if (isset($conf['grid_plus'])) {
            $this->grid_plus = ($conf['grid_plus'] ? true : false);
            }
        if (isset($conf['grid_multidel'])) {
            $this->grid_multidel = ($conf['grid_multidel'] ? true : false);
            }
        if (isset($conf['rpc_server'])) {
            $this->rpc_server = ($conf['rpc_server'] ? true : false);
            }
        if (isset($conf['user_profile_prg'])) {
            $this->prg_user_prof = ($conf['user_profile_prg'] ?
                                    $conf['user_profile_prg'] : false);
            }
        if (isset($conf['user_area_prg'])) {
            $this->prg_user_area = ($conf['user_area_prg'] ?
                                    $conf['user_area_prg'] : false);
            }
        if (isset($conf['db_as_service'])) {
            $this->db_as_service = $conf['db_as_service'];
            }
        if (isset($conf['dbas_readonly'])) {
            $this->dbas_readonly = ($conf['dbas_readonly'] ? true : false);
            }
        if (isset($conf['bug_aggregate'])) {
            $this->bug_aggregate = ($conf['bug_aggregate'] ? true : false);
            }
        if (isset($conf['host'])) {
            $this->host = strtoupper($conf['host']);
            }
        if (isset($conf['jobs_use_local_host'])) {
            $this->jobs_local_host = ($conf['jobs_use_local_host'] ? true : false);
            }
        if (isset($conf['service_redirect'])) {
            $this->service_redirect = strtoupper($conf['service_redirect']);
            }
        if (isset($conf['services'])) {
            $this->services = array();
            foreach (preg_split('/[,; ]+/', $conf['services']) as $service) {
                $service = strtoupper($service);
                if ($service != 'DEFAULT') {
                    $this->services[] = $service;
                    }
                }
            }
        if (isset($conf['profiling'])) {
            switch (strtolower($conf['profiling'])) {
                case "menu":
                    $this->profiling = "M";
                    break;
                case "ctrl":
                    $this->profiling = "C";
                    break;
                case "both":
                case "1":
                    $this->profiling = "B";
                    break;
                case "none":
                case "0":
                default:
                    $this->profiling = false;
                    break;
                }
            }
        if (isset($conf['record_trace'])) {
            $this->record_trace = array();
            list($this->record_trace['cu'],
                 $this->record_trace['cd'],
                 $this->record_trace['ct'],
                 $this->record_trace['uu'],
                 $this->record_trace['ud'],
                 $this->record_trace['ut']) =
                                      explode(",", strtoupper($conf['record_trace']));
            if (trim(implode("", $this->record_trace)) == "") {
                $this->record_trace = false;
                }
            }
        $this->runtime->read_options($conf);

        }


    function load_gateways() {

        if ($this->engines) {
            foreach ($this->engines as $engine) {
                $this->runtime->load_gateway($engine);
                }
            }
        else {
            $loaded = array();
            foreach ($this->tab as $single_tab) {
                $server_type = $single_tab->db->server->type;
                if (!in_array($server_type, $loaded)) {
                    $this->runtime->load_gateway($server_type);
                    $loaded[] = $server_type;
                    }
                }
            $this->engines = $loaded;
            }

        }


    /**
     * Ritorna il valore di una opzione per l'utente loggato o, se non impostata
     * per l'utente, quella per l'applicazione
     *
     * @param  string $option_key
     * @return mix
     */
    function option_get($option_key) {

        if (isset($this->user_options[$option_key])) {
            return $this->user_options[$option_key];
            }
        elseif (isset($this->app_options[$option_key])) {
            return $this->app_options[$option_key];
            }
        else {
            return false;
            }

        }


    /**
     * Returns TRUE if current user is in the requested role
     *
     * @param  string $role_name
     * @return boolean
     */
    function is_user($role_name) {

        return ($this->user_is_admin || in_array($role_name, $this->user_roles));

        }


    /**
     * Returns TRUE if current user has got requested right
     *
     * @param  string $right_code
     * @return boolean
     */
    function has_right($right_code) {

        return ($this->user_is_admin || in_array($right_code, $this->user_rights));

        }


    /**
     * Set the current user using passed user and password
     *
     * @param  string $user       User name to set up
     * @param  string $password   Password (or MD5) for the user
     * @return boolean
     */
    function set_user($user = false, $password = false) {

        // _____________________________ If no user is passed reset session start user ___
        if (!$user) {
            $user     = $this->start_user;
            $password = $this->get_user_password($user);
            }
        // ____________________ This to allow clear password as command line parameter ___
        $this->password = (strlen($password) < 32 || $this->block_md5_pwd ?
                           md5($password) : $password);
        $this->user     = $user;
        // __________________________________ Login program execution and user options ___
        list($this->user_roles,
             $this->user_rights,
             $this->app_options,
             $this->user_options,
             $this->logged_tabs,
             $this->force_pwd_change) = $this->intcall("tools/o2sys_start");
        // ________________________________________________ Lock for Under Maintenance ___
        if ($this->option_get("lock")) {
            if (!$this->user_is_admin && $this->run_level != "DEV") {
                if ($this->lock_page) {
                    o2act::gotourl($this->alias.$this->lock_page);
                    }
                else {
                    ob_end_clean();
                    die('<!DOCTYPE HTML><html><body><center><h1>Access denied</h1><br>'.
                        '<h4>Application is currently under maintenance.<br>'.
                        'Please try again later.</h4></center></body></html>');
                    }
                }
            else {
                $this->lock = true;
                $this->intcall("tools/jxlock");
                }
            }
        // ________________________________________________________________ Log tables ___
        $this->create_logtabs();
        // __________________________ Override runtime settings with user/app settings ___
        $this->runtime->decimals_point  = $this->decimals_point();
        $this->runtime->thousands_point = $this->thousands_point();
        // ________________________________________________________________ Admin menu ___
        if ($this->user_is_admin || $this->run_level == "DEV") {
            $this->runtime->menu();
            if (!$this->doc_activate() && isset($this->menu['jxdoc_folder'])) {
                unset($this->menu['jxdoc_folder']);
                unset($this->menu['o2admin']->voci['jxdoc_folder']);
                }
            }
        // ___________________________________________________________ Menus profiling ___
        if ($this->profiling == "M" || $this->profiling == "B") {
            $this->intcall("tools/o2sys_filter_menu");
            }
        // ______________________________________________ Filesystem profiled settings ___
        $this->crea_cartelle_pers();
        // _________________________________________________ Multilanguage translation ___
        if ($this->multilang = $this->option_get("lang")) {
            // _________________________________________ Load data-models translations ___
            $lang_dir = new o2_dir($this->dir_home.'/lang/'.$this->multilang.'/');
            // ______________________ MODELS repository: translate values from picture ___
            if (!isset($this->translation['#MODELS'])) {
                // _________________________________________________ Translation table ___
                if ($lang_dir->exists('#MODELS.lng')) {
                    $this->translation['#MODELS'] = jx_parse_conf($lang_dir.
                                                                  '#MODELS.lng');
                    foreach ($this->maschere as $mod_name => $model) {
                        if (count($model->valori)) {
                            foreach ($model->valori as $code => $desc) {
                                $this->maschere[$mod_name]->valori[$code] =
                                                          $this->translate($mod_name,
                                                                           'item',
                                                                           $code,
                                                                           $desc,
                                                                           '#MODELS');
                                }
                            }
                        }
                    }
                unset($this->translation['#MODELS']);
                }
            }
        $this->commit_all(false);
        return true;

        }


    /**
     * Return active password for a given user. If called with no parameters return the
     * current user password.
     *
     * @param  string $user   User password is requested for
     * @return string
     */
    function get_user_password($user = false) {

        if (!$user) {
            $user = $this->user;
            }
        $users  = $this->get_table('o2_users');
        $db     = $users->db;
        $server = $db->server;
        $co     = constant("o2_".$server->type."_o");
        $cc     = constant("o2_".$server->type."_c");
        $select = $users->campi['o2password']->nome_fisico.' '.$co."PASSWORD".$cc;
        $where  = $users->campi['o2user']->nome_fisico."='".$user."'";
        $list   = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $users->nome,
                                        $users->indice,
                                        $select,
                                        $where,
                                        false,
                                        1);
        return $list[0]["PASSWORD"];

        }


    /**
     * Activate doc if enabled. To be used in this way:
     * if ($this->doc_activate()) {
     *    $this->doc->method1;
     *    $this->doc->method2;
     *    ...
     *    }
     *
     * @return boolean
     */
    function doc_activate() {

        if ($this->doc_enabled) {
            if (!($this->doc instanceof o2_doc)) {
                $this->doc = new o2_doc();
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Set menu [$nome] as main application menu (level "0" menu bar)
     *
     */
    function attiva_menu($menu_name) {

        // ________________________________________________________ Recreate HTML menu ___
        if ($this->runtime->interface == "HTML") {
            $this->runtime->menu();
            $this->includi_repository($this->file_menu);
            $this->menu_cache = false;
            $this->menu_link  = false;
            }
        if (trim($menu_name) == "" || $this->menu[$menu_name]) {
            $this->menu_attivo = $menu_name;
            // _______________________________________________________ Menus profiling ___
            if ($this->profiling == "M" || $this->profiling == "B") {
                $this->intcall("tools/o2sys_filter_menu");
                }
            }
        else {
            throw new o2_exception("Unknown menu <i>".$menu_name.
                                   "</i> requested as main manu",
                                   o2error_UNKNOWNMENU);
            }

        }


    /**
     * Prints out menu. If user is root admin menu is displaied. If development context is
     * active dev_bar is displaied.
     *
     */
    function menu() {

        $this->attiva_menu($this->menu_attivo);

        }


    /**
     * Retrieve translated string for current language set in options
     *
     * @param string $ctrl
     * @param string $prop
     * @param string $prop_value
     * @param string $string
     */
    function translate($ctrl, $prop, $prop_value, $string, $rep = false) {

        if ($rep) {
            $prg = $rep;
            }
        else {
            $prg = $this->istanze_prg[$this->progressivo_istanze]->nome;
            }
        $res = $this->translation[$prg][$ctrl.'#'.$prop.
                                        ($prop == 'item' || $prop == 'page' ?
                                         '_'.$prop_value : '')];
        // ____________________________________ Look for translation in prg dictionary ___
        if (trim($res)) {
            return $res;
            }
        else {
            $res = $this->translation[''][$ctrl.'#'.$prop.
                                          ($prop == 'item' || $prop == 'page' ?
                                           '_'.$prop_value : '')];
            // _____________________________ Look for translation in global dictionary ___
            if (trim($res)) {
                return $res;
                }
            // _______________________________________________ Fallback to base string ___
            else {
                return $string;
                }
            }

        }


    /**
     * Translate current menu and its sub-menus
     *
     * @param string $start_menu   Used by recursive call
     * @return array
     */
    function translate_menu($start_menu = false) {

        if (!$start_menu || !isset($this->menu[$start_menu])) {
            $start_menu = $this->menu_attivo;
            }
        if ($start_menu && isset($this->menu[$start_menu])) {
            foreach ($this->menu[$start_menu]->voci as $item) {
                $item->label = $this->translate($item->nome,
                                                'label',
                                                0,
                                                $item->label,
                                                '#MENU');
                if ($item->tipo == "M") {
                    $this->translate_menu($item->nome);
                    }
                }
            }

        }


    /**
     * Returns execution informations (running programs and actions) as a javascript
     * object (js definition code).
     *
     * @return string
     */
    function exe_tree() {

        if ($this->progressivo_istanze) {
            $calls   = $_SESSION['o2_app']->chiamate;
            $js_conf = "o2jse.exeTree = [";
            $exes    = array_reverse($this->esecuzioni);
            foreach ($this->istanze_prg as $single_id => $single_prg) {
                $pars = '';
                foreach ($single_prg->parametri as $par_id => $par) {
                    if ($calls[$single_id]['parametri'][$par_id - 1]) {
                        $par_val = $calls[$single_id]['parametri'][$par_id - 1]['valore'];
                        $par_val = addcslashes((strlen($par_val) > 50 ?
                                                substr($par_val, 0, 44).' [...]' :
                                                $par_val), "'\\\r\n\"");
                        }
                    else {
                        $par_val = $par->default;
                        }
                    $pars.= ($pars ? ',' : '').
                            '"'.$par_id.' '.$single_prg->par_names[$par_id].
                            ': '.$par_val.'"';
                    }
                $js_conf.= "{type:'P',prg:".$single_id.",name:'".$single_prg->nome.
                           "',pars:[".$pars."]},";
                foreach ($exes as $single_exe) {
                    if ($single_exe->istanza_prg == $single_id) {
                        $js_conf.= "{type:'A',act:'".$single_exe->azione
                                          ."',step:".$single_exe->passo."},";
                        }
                    }
                }
            return substr($js_conf, 0, -1)."]; ";
            }
        else {
            return "";
            }

        }


    function roles_list() {

        if (count($this->user_roles)) {
            $js_conf = "o2jse.dev.roles = {";
            foreach ($this->user_roles as $idx => $role) {
                $js_conf.= $idx.":'".htmlentities($role,
                                                  ENT_QUOTES,
                                                  $_SESSION['o2_app']->chr_encoding)."',";
                }
            return substr($js_conf, 0, - 1)."};\n";
            }
        else {
            return "";
            }

        }


    function rights_list() {

        if (count($this->user_rights)) {
            $js_conf = "o2jse.dev.rights = {";
            foreach ($this->user_rights as $idx => $right) {
                $js_conf.= $idx.":'".htmlentities($right,
                                                  ENT_QUOTES,
                                                  $_SESSION['o2_app']->chr_encoding)."',";
                }
            return substr($js_conf, 0, - 1)."};\n";
            }
        else {
            return "";
            }

        }


    function reqdata_list() {

        if (count($_REQUEST)) {
            $js_conf = "o2jse.reqData = {";
            foreach ($_REQUEST as $single_req => $single_val) {
                $js_conf.= "'".$single_req."':'".addcslashes($single_val, "'\\\r\n")."',";
                }
            return substr($js_conf, 0, strlen($js_conf) - 1)."};\n";
            }
        else {
            return "";
            }

        }


    /**
     * Include un file di definizione di repository (models, dbs, tables, menus) per
     * l'applicazione
     *
     * @return boolean
     */
    function includi_repository($repository = "") {

        $file2inc = "";
        if ($repository) {
            if ($this->run_level == "DEV" && $this->dir_cvs_progs->exists($repository)) {
                $file2inc = $this->dir_cvs_progs->element($repository);
                }
            else {
                $file2inc = $this->dir_progs->element($repository);
                }
            if (!include_once($file2inc)) {
                if ($this->prgs_path_inc) {
                    foreach ($this->prgs_path_inc as $path) {
                        if (@include_once($path.$repository)) {
                            return true;
                            }
                        }
                    }
                throw new o2_exception("Cannot find file <i>".$repository.
                                        "</i> requested as repository.",
                                        o2error_MISSINGFILE);
                return false;
                }
            }
        return true;

        }


    /**
     * Include server side extra script in application.
     *
     */
    function include_extra() {

        if ($file = $this->file_server_inc) {
            if (!include_once $this->dir_progs.$file) {
                if ($this->prgs_path_inc) {
                    foreach ($this->prgs_path_inc as $path) {
                        if (@include_once($path.$file)) {
                            return true;
                            }
                        }
                    }
                throw new o2_exception("Cannot find file <i>".$file.
                                        "</i> requested as server side extra script.",
                                        o2error_MISSINGFILE);
                return false;
                }
            return true;
            }
        return false;

        }


    /**
     * Include lo script del modulo [prg] di un programma per la sua messa in esecuzione
     *
     * @param string $prg_name
     */
    function includi_prg($prg_name = '') {

        if (!$prg_name) {
            return;
            }
        $prg_name = trim($prg_name);
        if (strpos($prg_name, '/')) {
            list($prg_folder, $prg_name) = explode('/', $prg_name);
            switch ($prg_folder) {
                case 'tools':
                    $this->runtime->prg($prg_name);
                    if (!$this->extcall_level) {
                        $this->extcall_level   = $this->progressivo_istanze;
                        $this->extcall_ritorno = $this->ritorno;
                        }
                    $this->istanze_prg[$this->progressivo_istanze]->internal_tool = true;
                    return;
                    break;
                case 'doc':
                    if ($this->doc_activate()) {
                        $this->doc->prg($prg_name);
                        if (!$this->extcall_level) {
                            $this->extcall_level   = $this->progressivo_istanze;
                            $this->extcall_ritorno = $this->ritorno;
                            }
                        }
                    $this->istanze_prg[$this->progressivo_istanze]->internal_tool = true;
                    return;
                    break;
                case 'app':
                    $this->istanze_prg[$this->progressivo_istanze]->script = '';
                    break;
                }
            }
        $script_local = '';
        if ($this->run_level == 'DEV' && $this->dir_cvs_progs->exists($prg_name.'.prg')) {
            $script_local = $this->dir_cvs_progs.$prg_name.'.prg';
            }
        else {
            $script_local = $this->dir_progs.$prg_name.'.prg';
            }
        if (include($script_local)) {
            $this->istanze_prg[$this->progressivo_istanze]->load_custom_grids();
            }
        else {
            if ($this->prgs_path_inc) {
                foreach ($this->prgs_path_inc as $path) {
                    if (@include($path.$prg_name.'.prg')) {
                        $this->istanze_prg[$this->progressivo_istanze]
                                                                    ->load_custom_grids();
                        return;
                        }
                    }
                }
            throw new o2_exception('Cannot find file <i>'.$script_local.
                                    '</i> requested as PRG module for program <i>'.
                                    $prg_name.'</i>.',
                                    o2error_MISSINGFILE);
            }
        }


    /**
     * Include lo script del modulo [prf] di un programma per la definizione delle sue
     * funzioni
     *
     * @param string $prg_name
     * @param string $script
     */
    function includi_prf($prg_name = '', $script = '') {

        if (!$prg_name) {
            return;
            }
        $prg_name = trim($prg_name);
        if (strpos($prg_name, '/')) {
            list($prg_folder, $prg_name) = explode('/', $prg_name);
            switch ($prg_folder) {
                case 'tools':
                    return $this->runtime->prf($prg_name);
                    break;
                case 'doc':
                    if ($this->doc_activate()) {
                        return $this->doc->prf($prg_name);
                        }
                    return;
                    break;
                case 'app':
                    $this->istanze_prg[$this->progressivo_istanze]->script = '';
                    break;
                }
            }
        $script_local = '';
        if ($script) {
            $script_local = str_replace('.prg', '.prf', $script);
            }
        elseif ($this->run_level == 'DEV' &&
                $this->dir_cvs_progs->exists($prg_name.'.prf')) {
            $script_local = $this->dir_cvs_progs.$prg_name.'.prf';
            }
        else {
            $script_local = $this->dir_progs.$prg_name.'.prf';
            }
        if (!@include_once($script_local)) {
            if ($this->prgs_path_inc) {
                foreach ($this->prgs_path_inc as $path) {
                    if (@include_once($path.$prg_name.'.prf')) {
                        return;
                        }
                    }
                }
            throw new o2_exception('Cannot find file <i>'.$script_local.
                                   '</i> requested as PRF module for program <i>'.
                                   $prg_name.'</i>.',
                                   o2error_MISSINGFILE);
            }

        }


    /**
     * Return an application table object. If table is not defined the definition script
     * will be loaded before.
     * If parameter $clone is passed as TRUE a clone of the table is returned, else a
     * reference to the object is returned.
     *
     * @param  string $table_name
     * @return o2_file
     */
    function get_table($table_name, $clone = false) {

        // ___________________________________________________ Table is already loaded ___
        if (isset($this->tab[$table_name])) {
            // _________________________________________________________ Set log level ___
            if (isset($this->logged_tabs[$table_name])) {
                $this->tab[$table_name]->log_level = $this->logged_tabs[$table_name];
                }
            // _________________________________________________ Return table as clone ___
            if ($clone) {
                return clone $this->tab[$table_name];
                }
            // _____________________________________________ Return table as reference ___
            else {
                return $this->tab[$table_name];
                }
            }
        // ________________________________________________ Splitted tables repository ___
        elseif ($this->split_tab_repos) {
            $def_file = $this->dir_progs."jx_tables".DIRECTORY_SEPARATOR.
                        $table_name.".tab";
            // _____________________ Table definition is in the splitted rep directory ___
            if (@include_once $def_file) {
                $this->runtime->load_gateway($this->tab[$table_name]->db->server->type);
                return $this->get_table($table_name, $clone);
                }
            elseif ($this->doc_enabled) {
                $this->doc->repository();
                // _____________________________________________ Return table as clone ___
                if ($clone) {
                    return clone $this->tab[$table_name];
                    }
                // _________________________________________ Return table as reference ___
                else {
                    return $this->tab[$table_name];
                    }
                }
            else {
                throw new o2_exception("Missing definition file <i>".$def_file.
                                       "</i> for table <i>".$table_name."</i>",
                                       o2error_MISSINGFILE);
                return false;
                }
            }
        // ________________________________________________________ Unknown table name ___
        else {
            throw new o2_exception("Unknown table <i>".$table_name."</i>",
                                   o2error_MISSINGDEF);
            return false;
            }

        }


    /**
     * Include nella pagina PHP l'esecuzione di un'altra pagina PHP.
     * Accetta infiniti parametri.
     *
     * @param  string $programma
     * @return array
     */
    function intcall($programma) {

        $parametri = array();
        $next_id   = ($this->progressivo_istanze + 1);
        // ____________________________________________ Sets call in active calls list ___
        if (isset($this->chiamate[$next_id]) && $this->chiamate[$next_id]['fine']) {
            unset($this->chiamate[$next_id]);
            }
        if (!isset($this->chiamate[$next_id])) {
            // _______________________________________________________ Gets parameters ___
            $parametri   = func_get_args();
            $parametri_n = func_num_args() - 1;
            array_shift($parametri);
            $parametri_formattati = array();
            foreach ($parametri as $i => $singolo_parametro) {
                $parametri_formattati[$i]['tipo']   = "E";
                $parametri_formattati[$i]['task']   = "";
                $parametri_formattati[$i]['campo']  = "";
                $parametri_formattati[$i]['valore'] = $singolo_parametro;
                }
            $new_call = array('id'          => $next_id,
                              'prg'         => $programma,
                              'parametri_n' => $parametri_n,
                              'parametri'   => $parametri_formattati,
                              'fine'        => false);
            $this->chiamate[$next_id] = $new_call;
            }
        $chiamata_local = &$this->chiamate[$next_id];
        // __________________________________________________________ Executes program ___
        $this->includi_prf($programma);
        $this->includi_prg($programma);
        $prg_local = $this->istanze_prg[$next_id];
        if ($prg_local->esecutivo) {
            $prg_local->esegui_azione($prg_local->esecutivo, true);
            }
        $prg_local = $this->istanze_prg[$this->progressivo_istanze];
        if ($prg_local->concluso()) {
            $this->termina_prg();
            foreach ($chiamata_local['parametri'] as $i => $singolo_parametro) {
                $parametri[$i] = $singolo_parametro['valore'];
                }
            unset($this->chiamate[$chiamata_local['id']]);
            return $parametri;
            }

        }


    /**
     * Toglie il prg in esecuzione dall'elenco dei prg attivi
     *
     * @param integer $prg_id
     */
    function termina_prg($prg_id = 0) {

        if (!$prg_id) {
            $prg_id = $this->progressivo_istanze;
            }
        $prg_local = $this->istanze_prg[$prg_id];
        // ____________________________________________________ Clone program to cache ___
        if (isset($this->prgs_cache[$prg_local->nome])) {
            $this->prgs_cache[$prg_local->nome] = $prg_local->clone4cache();
            }
        // ____________________________________________ Unsets program pending actions ___
        foreach ($this->esecuzioni as $esec_index => $singola_esecuzione) {
            if ($singola_esecuzione->istanza_prg == $prg_id) {
                unset($this->esecuzioni[$esec_index]);
                }
            }
        // ______________________________________________ Unset program focus settings ___
        unset($this->caret_position[$prg_id]);
        // _____________________________________________________ Closes open resources ___
        foreach ($prg_local->risorse as &$singola_risorsa) {
            $singola_risorsa->chiudi();
            }
        // _____________________________________________ Set transaction isolation off ___
        if ($this->isolated_trans == $prg_id) {
            $this->commit_all(false);
            $this->isolated_trans = false;
            }
        // ____________________________________________________________ Unsets program ___
        unset($this->istanze_prg[$prg_id]);
        //  ____________________________________________________ Ends call if existing ___
        if ($prg_id == $this->extcall_level) {
            $this->extcall_level = 0;
            $this->ritorno       = $this->extcall_ritorno;
            }
        if ($this->chiamate[$prg_id]) {
            $this->chiamate[$prg_id]['fine'] = true;
            }
        $this->progressivo_istanze = $prg_id;
        unset($this->caret_position[$this->progressivo_istanze]);
        $this->progressivo_istanze--;

        }


    /**
     * Esecuzione dell'applicazione come RPC-Server in contesto WEB HTML
     *
     */
    function rpc_exe() {

        // ________________________________ If RPC is used to expose a DB as a service ___
        if ($this->db_as_service) {
            // ____________________________________________ Get database to be exposed ___
            if ($dbas = $this->db[$this->db_as_service]) {
                header("Content-type: application/json; charset=".$this->chr_encoding);
                // _____________________________________________ Unrem to output a log ___
                /*
                print json_encode(array("Janox Database As A Service" => $_SERVER,
                                        "Database"                    => $dbas,
                                        "Request"                     => $_REQUEST));
                return;
                */
                // _________________________________________ Execute start-prg, if any ___
                if ($this->exe_prg_start) {
                    $this->goto_prg($this->prg_start, false);
                    // ________________________________________ Actions list execution ___
                    $this->actions_loop();
                    }
                $this->runtime->load_gateway($dbas->server->type);
                // ____________________ Server side extra script inclusion (extra.inc) ___
                if ($this->file_server_inc) {
                    $this->include_extra();
                    }
                // _____________________________________________________________ JXSQL ___
                if ($this->jxsql) {
                    include_once $this->runtime->dir_dbms.'jxsql.inc';
                    }
                $verb = $_REQUEST['c'];
                if ($verb) {
                    if ($this->dbas_readonly &&
                        in_array($verb, array('insertfrom',
                                              'droptable',
                                              'renametable',
                                              'createtable',
                                              'modifyrec',
                                              'insertrec',
                                              'deleterec',
                                              'fkeyadd',
                                              'fkeyremove',
                                              'commit'))) {
                        print json_encode(array('!#ERROR' => 'Database <i>'.$dbas->id.
                                                             '</i> is in Read-only '.
                                                             'mode: modifications are '.
                                                             'not allowed.'));
                        return;
                        }
                    }
                else {
                    print json_encode(array("Janox Database As A Service" => "Enabled",
                                            "Database"                    => $dbas->id,
                                            "Mode" => ($this->dbas_readonly ?
                                                       'Read-only' : 'Read and write')));
                    return;
                    }
                print o2_gateway::jxsdb($dbas, $verb);
                }
            else {
                header($_SERVER['SERVER_PROTOCOL'].' 500 Internal Server Error',
                       true, 500);
                print "<h1>Error</h1>";
                throw new o2_exception("Unknown database <i>".$this->db_as_service."</i>".
                                       " set as Database As a Service",
                                       o2error_UNKNOWNDBSERVER);
                }
            }
        // __________________________________________________________ Standard RPC exe ___
        else {
            // _________________________________________________________________ JXSQL ___
            if ($this->jxsql) {
                include_once $this->runtime->dir_dbms.'jxsql.inc';
                }
            // ________________________ Server side extra script inclusion (extra.inc) ___
            if ($this->file_server_inc) {
                $this->include_extra();
                }
            // _________________________________________ Including view models scripts ___
            if ($this->file_viewmodels) {
                $this->includi_repository($this->file_viewmodels);
                }
            // _____________________________________________________ Database gateways ___
            $this->load_gateways();
            // _________________________________________ ESECUZIONE DEL PRG DI REQUEST ___
            $this->intcall("tools/o2sys_request");
            // ______________________________________ STARTING NEW EXECUTION HIERARCHY ___
            $this->istanze_prg         = array();
            $this->progressivo_istanze = 0;
            $this->prgs_cache          = array();
            $this->translation         = array('' => $this->translation['']);
            $this->goto_prg($this->prg_start, false);
            // ________________________________________________ Actions list execution ___
            $this->actions_loop();
            // ______________________________________________ Commit open transactions ___
            $this->commit_all(true);
            }
        // ________________________________________________ Custom response by o2out() ___
        if ($this->custom_response) {
            $resp = ob_get_clean();
            header("Content-type: text/plain; charset=".$this->chr_encoding);
            print substr($resp, 0, $this->custom_response);
            $this->custom_response = false;
            }
        // __________________________________________________ Close open XML streaming ___
        elseif($this->xmlstream_head && !$this->xmlstream_foot) {
            print "</DATAPACKET>\n";
            }
        ob_end_flush();

        }


    /**
     * Esecuzione dell'applicazione e dei prg caricati in contesto WEB HTML
     *
     * @param boolean $only_output
     */
    function esecutivo($only_output = false) {

        // ________________________________________ Reset transaction isolation levels ___
        $this->isolated_trans = false;
        // _____________________________________________ Including view models scripts ___
        if ($this->file_viewmodels) {
            $this->includi_repository($this->file_viewmodels);
            }
        // _________________________________________________________ Database gateways ___
        $this->load_gateways();
        try {
            // _________________________________________ ESECUZIONE DEL PRG DI REQUEST ___
            $this->intcall("tools/o2sys_request");
            // ______________________________________ STARTING NEW EXECUTION HIERARCHY ___
            if ($_REQUEST['o2c'] || $_SESSION['jxredirect'] || $this->exe_prg_start) {
                // ____________________________________ Check callable programs by URL ___
                if ($_REQUEST['o2c']) {
                    if (($this->profiling == "M" || $this->profiling == "B") &&
                        !isset($this->menu_prgs[$_REQUEST['o2c']]) &&
                        $this->run_level != "DEV" &&
                        (!$this->user_is_admin ||
                         substr($_REQUEST['o2c'], 0, 6) != "tools/")) {
                        throw new o2_exception("Can't call program <i>".$_REQUEST['o2c'].
                                               "</i>",
                                               o2error_UNKNOWNPRG);
                        return;
                        }
                    }
                $this->prgs_cache       = array();
                $this->translation      = array('' => $this->translation['']);
                $redirect               = $_SESSION['jxredirect'];
                $_SESSION['jxredirect'] = false;
                // _______________________________________ Reset range control filters ___
                unset($_SESSION['jxview_lastfilter']);
                $this->goto_prg(($redirect ? false : $_REQUEST['o2c']), $redirect);
                }
            else {
                // _________________________________ Including active programs scripts ___
                foreach ($this->istanze_prg as $singola_istanza) {
                    $this->includi_prf($singola_istanza->nome, $singola_istanza->script);
                    }
                // _______________________ CONTINUING EXECUTION AFTER USER INTERACTION ___
                if (!$only_output) {
                    o2html::receive();
                    }
                }
            // ________________________________________________ Actions list execution ___
            if (!$only_output) {
                $this->actions_loop();
                }
            // ___________________________ Reset original user if playing as other one ___
            if ($this->start_user) {
                $this->set_user();
                }
            // _______________________________________________________ Forms rendering ___
            $this->display_all();
            // ______________________________________________ Commit open transactions ___
            $this->commit_all(true);
            // _________________________________________________________ SQL Satistics ___
            if ($this->sqltrace) {
                $this->sql_stat();
                }
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            }

        }


    /**
     * Commits open transactions
     *
     * @param boolean $end   Close open statement for script ending
     */
    function commit_all($end = true) {

        foreach ($this->server as $server) {
            o2_gateway::commit($server->type,
                               $server->server,
                               $server->user,
                               $server->password,
                               $end);
            }

        }


    /**
     * Displays all visible form of all active programs
     *
     */
    function display_all() {


        $this->istanza_attiva = $this->progressivo_istanze;
        $this->relocate_now   = true;
        $singola_istanza      = false;
        // _____________________________________________________ Loop on prg instances ___
        foreach ($this->istanze_prg as &$singola_istanza) {
            $this->progressivo_istanze = $singola_istanza->id;
            $singola_istanza->relocate_views();
            $singola_istanza->visualizza();
            }
        $this->relocate_now = false;
        // _____________________________________________ JXJS new windows Insert Point ___
        if ($this->runtime->interface == "HTML" && !$GLOBALS['jxjs']) {
            print "<div id='jxjsinsert'></div>\n";
            }
        // ________________________________________________________ Display report-msg ___
        if ($singola_istanza) {
            $singola_istanza->display_report_msg();
            }

        }


    /**
     * Loops on actions list until the end or until an action hangs up
     *
     */
    function actions_loop() {

        // __________________________________________ While actions in list get to end ___
        do {
            // ______________________ Programs ending their own actions will be closed ___
            if ($this->progressivo_istanze &&
                $this->istanze_prg[$this->progressivo_istanze]->concluso()) {
                $this->termina_prg();
                }
            // _______________________ If a program action is waiting will be executed ___
            if (isset($this->esecuzioni[0]) &&
                    $this->esecuzioni[0]->istanza_prg == $this->progressivo_istanze) {
                $this->esecuzioni[0]->esegui();
                }
            } while ($this->esecuzione_fine());
        // __________________________________ Closing open resources (for productions) ___
        foreach ($this->istanze_prg as $singola_istanza) {
            foreach ($singola_istanza->risorse as $singola_risorsa) {
                $singola_risorsa->chiudi();
                }
            }
        $this->produzioni = array();
        /**
         * TOVERIFY: probably unuseful code
         */
        while ($this->progressivo_istanze &&
            $this->istanze_prg[$this->progressivo_istanze]->concluso()) {
            $this->termina_prg();
            }

        }


    /**
     * Starts executing program from menu or from "goto" action
     *
     * @param string $prg_togo
     * @param string $redirect
     */
    function goto_prg($prg_togo, $redirect) {

        // ____________________________________________ Reset dei valori di esecuzione ___
        $this->istanze_prg         = array();
        $this->progressivo_istanze = 0;
        $this->istanza_attiva      = 0;
        $this->esecuzioni          = array();
        $this->ritorno             = false;
        $this->caret_position      = array();
        $this->produzioni          = array();
        $this->open_wins           = array();
        // ____________________ If coming from a 'goto' istruction call already exists ___
        // __________________ If coming from a menu (o2c) I create call and parameters ___
        if ($prg_togo) {
            $contatore_local = -1;
            $parametri       = $_REQUEST;
            unset($parametri['o2c']);
            unset($parametri['JXSESSNAME']);
            if (isset($parametri[$this->session_name])) {
                unset($parametri[$this->session_name]);
                }
            $o2pars = array();
            foreach ($parametri as $nome_parametro => $singolo_parametro) {
                $contatore_local++;
                if (substr($nome_parametro, 0, 5) == 'extp_') {
                    $o2pars[$contatore_local]['tipo']   = "E";
                    $o2pars[$contatore_local]['task']   = "";
                    $o2pars[$contatore_local]['campo']  = "";
                    $o2pars[$contatore_local]['valore'] = $singolo_parametro;
                    }
                }
            // _________________ I drop calls list and create 1st call with parameters ___
            $this->chiamate = array(1 => array('id'          => 1,
                                               'prg'         => trim($_REQUEST['o2c']),
                                               'parametri_n' => count($o2pars),
                                               'parametri'   => $o2pars));
            }
        $this->get_params();
        if ($redirect) {
            $prg_to_exe = $redirect;
            }
        else {
            $prg_to_exe = ($this->exe_prg_start ? $this->prg_start : $prg_togo);
            }
        $this->extcall_level   = 0;
        $this->extcall_ritorno = 0;
        $this->exe_prg_start   = false;
        $this->intcall($prg_to_exe);

        }


    /**
     * Adds program action to application pending actions list, tries to execute it and
     * returns action ending status.
     *
     * @param  string  $prg
     * @param  string  $id_istanza_prg
     * @param  string  $azione
     * @param  integer $counter
     * @return boolean
     */
    function esecuzione_inizio($prg, $id_istanza_prg, $azione, $counter) {

        // ________________ New execution is inserted at the top of executions list... ___
        array_unshift($this->esecuzioni, new o2_esecuzione($prg,
                                                           $id_istanza_prg,
                                                           $azione));
        // ___________________________________________________ ...and then is executed ___
        $this->esecuzioni[0]->loop = $counter;
        $this->esecuzioni[0]->esegui();
        return $this->esecuzione_fine();

        }


    /**
     * Toglie un'azione dall'elenco delle azioni in esecuzione se si � conclusa in ogni
     * suo passo
     *
     * @return boolean
     */
    function esecuzione_fine() {

        if (isset($this->esecuzioni[0])) {
            $running_exe = $this->esecuzioni[0];
            if (!$running_exe->passo || $running_exe->skip) {
                array_shift($this->esecuzioni);
                if ($running_exe->suffix_of) {
                    $prg = $this->istanze_prg[$running_exe->istanza_prg];
                    $prg->contesto[$running_exe->suffix_of]->suffix_running = false;
                    $prg->contesto[$running_exe->suffix_of]->suffix_waiting = false;
                    $running_exe->suffix_of                                 = false;
                    }
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
     * Deletes a registered user and all related context (roles, rights, options, ...)
     *
     * @param  string $user   User to be deleted
     * @return boolean
     */
    function delete_user($user) {

        $tab    = $this->get_table("o2_users");
        $field  = $tab->campi["o2user"]->nome_fisico;
        $db     = $tab->db;
        $server = $db->server;
        $ret    = true;
        // _____________________________________________________ Verify user existance ___
        $res    = o2_gateway::verifyrec($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        $tab->indice,
                                        "*",
                                        $field."='".$user."'",
                                        "");
        if (!$res) {
            $ret = false;
            }
        else {
            // __________________________________________ Delete user from users table ___
            o2_gateway::deleterec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  $tab->indice,
                                  $field."='".$user."'");
            o2_gateway::commit($server->type,
                               $server->server,
                               $server->user,
                               $server->password);
            }
        // ____________________________________________________ Verify roles existance ___
        $tab    = $this->get_table("o2_roles_per_user");
        $field  = $tab->campi["o2user"]->nome_fisico;
        $db     = $tab->db;
        $server = $db->server;
        $res    = o2_gateway::verifyrec($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        $tab->indice,
                                        "*",
                                        $field."='".$user."'",
                                        "");
        if ($res) {
            // _____________________________________________ Delete user related roles ___
            o2_gateway::deleterec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  $tab->indice,
                                  $field."='".$user."'");
            o2_gateway::commit($server->type,
                               $server->server,
                               $server->user,
                               $server->password);
            }
        // __________________________________________________ Verify options existance ___
        $tab    = $this->get_table("o2_options");
        $field  = $tab->campi["o2user"]->nome_fisico;
        $db     = $tab->db;
        $server = $db->server;
        $res    = o2_gateway::verifyrec($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        $tab->indice,
                                        "*",
                                        $field."='".$user."'",
                                        "");
        if ($res) {
            // ___________________________________________ Delete user related options ___
            o2_gateway::deleterec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  $tab->indice,
                                  $field."='".$user."'");
            o2_gateway::commit($server->type,
                               $server->server,
                               $server->user,
                               $server->password);
            }
        // __________________________________________________ Verify options existance ___
        $tab    = $this->get_table("o2_custom_grids");
        $field  = $tab->campi["o2user"]->nome_fisico;
        $db     = $tab->db;
        $server = $db->server;
        $res    = o2_gateway::verifyrec($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        $tab->indice,
                                        "*",
                                        $field."='".$user."'",
                                        "");
        if ($res) {
            // ____________________________________ Delete user related grids settings ___
            o2_gateway::deleterec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  $tab->indice,
                                  $field."='".$user."'");
            o2_gateway::commit($server->type,
                               $server->server,
                               $server->user,
                               $server->password);
            }
        // ____________________________________________ Delete user's personal folders ___
        if ($user) {
            $udir = new o2_dir($this->dir_home."users".DIRECTORY_SEPARATOR.
                               $user.DIRECTORY_SEPARATOR);
            $udir->remove();
            }
        return $ret;

        }


    /**
     * Returns loop counter for a running action
     *
     * @param  string $action_name
     * @return integer
     */
    function action_counter($action_name = "") {

        if ($action_name) {
            foreach ($this->esecuzioni as $single_exe) {
                if ($single_exe->azione == $action_name) {
                    return $single_exe->loop;
                    }
                }
            return 0;
            }
        elseif ($this->esecuzioni[0]) {
            return $this->esecuzioni[0]->loop;
            }
        else {
            return 0;
            }

        }


    /**
     * Prepare pdf file from xml sources or print it to printer or print out as stream
     * (RPC XML stream)
     *
     * @param string $modello
     * @param string $file_pdf
     * @param array  $info_print
     * @param string $printer_name
     */
    function stampa($modello, $file_pdf, $info_print, $printer_name) {

        // ____________________________________ Chiudo eventuali risorse ancora aperte ___
        foreach ($this->istanze_prg as &$singola_istanza) {
            foreach ($singola_istanza->risorse as &$singola_risorsa) {
                $singola_risorsa->chiudi();
                }
            }
        // ________________________________________________ XML stream for WEB service ___
        if ($this->rpc_server) {
            print "</DATAPACKET>\n";
            $this->xmlstream_foot = true;
            return;
            }
        clearstatcache();
        // ___________________ If file to produce already exists it is deleted before  ___
        if ($file_pdf && $this->dir_tmp->exists(basename($file_pdf))) {
            if (!unlink($this->dir_tmp.basename($file_pdf))) {
                throw new o2_exception("<b>TARGET FILE LOCK:</b> ".
                                       "cannot overwrite existing file <i>".
                                       $this->dir_tmp.basename($file_pdf).
                                       "</i> for production.",
                                       o2error_IO);
                return;
                }
            }
        // ______________________________________________________________ WEB Printing ___
        if (strtolower(substr($this->reporter, 0, 7)) == "http://" ) {
            return $this->web_print($modello, $file_pdf, $info_print);
            }
        // ___________________________________________________ Check model is provided ___
        if (!$modello) {
            throw new o2_exception("<b>PRINT ERROR:</b> no model provided for printing ".
                                   "<i>".$file_pdf."</i>",
                                   o2error_MISSINGFILE);
            return;
            }
        $model_local  = $this->file_in_uso("MODELS", $modello);
        $target_file  = new o2_fsitem($file_pdf);
        $model_folder = new o2_dir(dirname($model_local));
        $rep_folder   = new o2_dir(is_dir($this->reporter) ?
                                   $this->reporter.DIRECTORY_SEPARATOR :
                                   dirname($this->reporter));
        // ________________________________________________________ Check model exists ___
        if (!$model_folder->exists(basename($model_local))) {
            throw new o2_exception("<b>MISSING MODEL FILE:</b> cannot find file <i>".
                                   basename($model_local).
                                   "</i> requested as model for printing.",
                                   o2error_MISSINGFILE);
            return;
            }
        // ________________________________________ Check Report Manager folder exists ___
        if (!$rep_folder->exists()) {
            throw new o2_exception("<b>MISSING REPORTER:</b> cannot find path to <i>".
                                   $rep_folder."</i>.",
                                   o2error_MISSINGFILE);
            return;
            }
        $model_local_txt   = file_get_contents($model_local);
        $model_local_txt   = str_replace("' +".chr(13).chr(10)."        '",
                                         "",
                                         $model_local_txt);
        $model_local_txt   = str_replace(" = ".chr(13).chr(10)."        '",
                                         " = '",
                                         $model_local_txt);
        $section_local     = false;
        $resource_local    = "";
        $file_local        = "";
        $model_local_array = explode(chr(13).chr(10), $model_local_txt);
        foreach ($model_local_array as $row_index => $single_row) {
            if (substr($single_row, 0, 14) == "  DataInfo = <") {
                $section_local = true;
                }
            if ($section_local && substr($single_row, 0, 8) == "    end>") {
                break;
                }
            if (substr($single_row, 0, 15) == "      Alias = '") {
                $resource_local = explode("'", $single_row);
                $resource_local = $resource_local[1];
                }
            if (substr($single_row, 0, 22) == "      MyBaseFilename =") {
                foreach ($info_print as $singola_produzione) {
                    if (array_key_exists($singola_produzione['prg'],
                                         $this->produzioni) &&
                        array_key_exists($singola_produzione['risorsa'],
                                         $this->
                                          produzioni[$singola_produzione['prg']])) {
                        foreach ($this->produzioni[$singola_produzione['prg']]
                                 [$singola_produzione['risorsa']] as $singolo_file) {
                            if (strtoupper(trim($resource_local)) ==
                                strtoupper(trim($singola_produzione['risorsa']))) {
                                $file_local = $singolo_file;
                                }
                            }
                        }
                    }
                if ($file_local) {
                    $model_local_array[$row_index] = "      MyBaseFilename = '".
                                                     $file_local."'";
                    $file_local = "";
                    }
                }
            }
        // __________________________________________ Creates reporter source file ___
        $nome_xml_local = tempnam($this->dir_tmp, "src_");
        file_put_contents($nome_xml_local,
                          implode(chr(13).chr(10), $model_local_array));
        // ____________________________________________________________ Prints out ___
        switch (strtolower($target_file->ext)) {
            case "xls":
                $prod_type = "-excel -onesheet";
                break;
            case "csv":
                $prod_type = "-csv -csvseparator \";\"";
                break;
            case "html":
                $prod_type = "-htmlsingle";
                break;
            case "rpm":
                $prod_type = "-m";
                break;
            default:
                $prod_type = "-pdf";
                break;
            }
        // _______________________________________ Check for OS and existing files ___
        $win     = $this->runtime->mswin;
        $printer = $win && $printer_name && $rep_folder->exists('printrepxp.exe');
        $repman  = $rep_folder.($win ?
                                ($printer ? 'printrepxp.exe' :
                                  ($rep_folder->exists('printreptopdf.cmd') ?
                                   'printreptopdf.cmd' : 'printreptopdf.exe')) :
                                ($rep_folder->exists('printreptopdf.sh') ?
                                 'printreptopdf.sh' : 'printreptopdf'));
        // ___________________________________________________ Check executable exists ___
        if (!file_exists($repman)) {
            throw new o2_exception("<b>MISSING REPORTER:</b> cannot find file <i>".
                                   $repman."</i>.",
                                   o2error_MISSINGFILE);
            return;
            }
        // ________________________________ Check can read and execute executable file ___
        if (!is_readable($repman) || !is_executable($repman)) {
            throw new o2_exception("<b>REPORTER ERROR:</b> cannot execute Reporter file ".
                                   "<i>".$repman."</i>.",
                                   o2error_IO);
            return;
            }
        // _____________________________________________________ Compose print command ___
        $rep_cmd = '"'.$repman.
                   ($printer ?
                    '" -q -printer "'.$printer_name.'" "'.$nome_xml_local.'"' :
                    '" -q '.$prod_type.' "'.$nome_xml_local.'" "'.
                    $this->dir_tmp.$file_pdf.'"');
        // ____________________________________________ Log command and output to file ___
        $log = $this->dir_tmp.$file_pdf.'.log';
        file_put_contents($log, "Print command:\n".$rep_cmd."\n\nCommand output:\n");
        $ret_val = 0;
        system($rep_cmd.'>>'.$log.' 2>&1', $ret_val);
        if (!file_exists($this->dir_tmp.$file_pdf)) {
            $log = file_get_contents($log);
            if (strpos($log, "No data available to print") === false) {
                throw new o2_exception("<b>PRINT ERROR:</b> cannot generate file <i>".
                                       $file_pdf."</i>.<hr><code>".
                                       nl2br($log)."</code>",
                                       o2error_IO);
                }
            }

        }


    /**
     * Prepare XML source files for WEB printing and request result to WEB Print Server
     *
     * @param string $modello
     * @param string $file_pdf
     * @param array  $info_print
     */
    function web_print($modello, $file_pdf, $info_print) {

        $model_local  = $this->file_in_uso("MODELS", $modello);
        $target_file  = new o2_fsitem($file_pdf);
        $model_folder = new o2_dir(dirname($model_local));
        $hm           = "_jxwp".substr(md5(rand(0, 32000)), 0, 10)."_/";
        $postdata     = array();
        if ($model_folder->exists(basename($model_local))) {
            $model_local_txt = file_get_contents($model_local);
            $model_local_txt = str_replace("' +".chr(13).chr(10)."        '",
                                           "",
                                           $model_local_txt);
            $model_local_txt = str_replace(" = ".chr(13).chr(10)."        '",
                                           " = '",
                                           $model_local_txt);
            $section_local   = false;
            $resource_local  = "";
            $file_local      = "";
            $model_local_array = explode(chr(13).chr(10), $model_local_txt);
            foreach ($model_local_array as $row_index => $single_row) {
                if (substr($single_row, 0, 14) == "  DataInfo = <") {
                    $section_local = true;
                    }
                if ($section_local && substr($single_row, 0, 8) == "    end>") {
                    break;
                    }
                if (substr($single_row, 0, 15) == "      Alias = '") {
                    $resource_local = explode("'", $single_row);
                    $resource_local = $resource_local[1];
                    }
                if (substr($single_row, 0, 22) == "      MyBaseFilename =") {
                    foreach ($info_print as $singola_produzione) {
                        if (array_key_exists($singola_produzione['prg'],
                                             $this->produzioni) &&
                            array_key_exists($singola_produzione['risorsa'],
                                             $this->
                                              produzioni[$singola_produzione['prg']])) {
                            foreach ($this->produzioni[$singola_produzione['prg']]
                                     [$singola_produzione['risorsa']] as $singolo_file) {
                                if (strtoupper(trim($resource_local)) ==
                                    strtoupper(trim($singola_produzione['risorsa']))) {
                                    $file_local = $singolo_file;
                                    }
                                }
                            }
                        }
                    if ($file_local) {
                        $bn                            = basename($file_local);
                        $sn                            = str_replace(".", "|", $bn);
                        $postdata[$sn]                 = file_get_contents($file_local);
                        $model_local_array[$row_index] = "      MyBaseFilename = '".
                                                         $hm.$bn."'";
                        $file_local = "";
                        }
                    }
                }
            // __________________________________________ Creates reporter source file ___
            $nome_xml_local = tempnam($this->dir_tmp, "src_");
            file_put_contents($nome_xml_local,
                              implode(chr(13).chr(10), $model_local_array));
            // ____________________________________________________________ Prints out ___
            switch (strtolower($target_file->ext)) {
                case "xls":
                    $prod_type = "-excel -onesheet";
                    break;
                case "csv":
                    $prod_type = "-csv -csvseparator \";\"";
                    break;
                case "html":
                    $prod_type = "-htmlsingle";
                    break;
                case "rpm":
                    $prod_type = "-m";
                    break;
                default:
                    $prod_type = "-pdf";
                    break;
                }
            // _____________________________________________________ Source model file ___
            $postdata["__jxsrc"] = file_get_contents($nome_xml_local);
            // ______________________________________________________ Result file name ___
            $postdata["__jxres"] = $file_pdf;
            // _________________________________________ Hash marker for relative path ___
            $postdata["__jxhm"]  = $hm;
            // _________________________________________________________ Print command ___
            $postdata["__jxpc"]  = "-q ".$prod_type." \"".$hm."jxwp_src\" \"".
                                   $hm.$file_pdf."\"";
            // __________________________________________ Make request and save result ___
            file_put_contents($this->dir_tmp.$file_pdf,
                              jx_post($this->reporter, $postdata));
            }

        }

    /**
     * Executes dbcopy tool from within application.
     * Mostly used from command line - see o2_app::bat_app()
     *
     * @param string $mode              Export or import
     * @param string $int_db            Internal db source for export or target for import
     * @param string $ext_type          External connection type
     * @param string $ext_host          External connection host
     * @param string $ext_user
     * @param string $ext_password
     * @param string $ext_schema
     * @param string $ext_db
     */
    function dbcopy($mode,
                    $int_db,
                    $ext_type,
                    $ext_host,
                    $ext_user     = '',
                    $ext_password = '',
                    $ext_schema   = '',
                    $ext_db       = '',
                    $aspid        = '') {

        // _______________________________________________ Run program with parameters ___
        $this->intcall('tools/jxdbcopy',
                       $mode,
                       $int_db,
                       $ext_type,
                       $ext_host,
                       $ext_user,
                       $ext_password,
                       $ext_schema,
                       $ext_db,
                       $aspid);

        }


    /**
     * Returns URL where WEB Server will dispose the requested file.
     * Cachable parameter is for internal use: if TRUE resource can be recovered from
     * client cache.
     *
     * @param  string  $file_path
     * @param  integer $force_download
     * @param  boolean $cachable         If resource is client-cachable
     * @return string
     */
    function file2http($file_path, $force_download = false, $cachable = false) {

        // ______________________________________________________ Check file existance ___
        if (!file_exists($file_path)) {
            throw new o2_exception("File <i>".$file_path."</i> NOT FOUND",
                                   o2error_MISSINGFILE);
            }
        // ____________________________________________________ Check file is readable ___
        elseif (!is_readable($file_path)) {
            throw new o2_exception("File <i>".$file_path."</i> NOT READABLE", o2error_IO);
            }
        // _____________________________________________________________ If file is OK ___
        else {
            // ___________________________________ Insert file in list, if not present ___
            $add = true;
            foreach ($this->ext_resources as $index => $sing_res) {
                if ($sing_res[0] == $file_path) {
                    $add  = false;
                    $r_id = $index;
                    break;
                    }
                }
            if ($add) {
                $r_id = count($this->ext_resources) + 1;
                }
            // _________________ Add or refresh force_download/cache-mode for resource ___
            $this->ext_resources[$r_id] = array($file_path,
                                                $force_download,
                                                $cachable);
            // _____________________________________ Check session ID transport method ___
            if (!ini_get("session.use_cookies")) {
                $sessid = "&".$this->session_name."=".$this->session_id;
                }
            return $this->runtime->alias."jxf.php?RID=".$r_id.
                                         "&JXSESSNAME=".$this->session_name.$sessid;
            }
        return "";

        }


    /**
     * Defines application level o2_dir objects and ensures folders existence.
     *
     * Filesystem structure
     *
     *    /app                  home
     *      |-/data             internal database (sqlite3) objects folder
     *      |-/doc              documentation files
     *      |-/htdocs           document root (www root)
     *      |    |-/css         CSS files
     *      |    |-/img         images files
     *      |    '-/trn         tmp files for download exposure
     *      |-/logs             log files
     *      |-/models           frf models for reports production
     *      |-/prgs             o2 repositories and programs definition scripts
     *      |    '-/[developer] scripts checked out by developer
     *      '-/users            users personal folders
     *
     */
    function crea_cartelle() {

        // _______________________________________________________________ /app/htdocs ___
        $this->dir_htdocs = new o2_dir(($this->runtime->interface != "HTML" ?
                                        dirname(realpath($_SERVER['argv'][0])) :
                                        getcwd()).DIRECTORY_SEPARATOR);
        // ______________________________________________________________________ /app ___
        $this->dir_home   = new o2_dir((realpath($this->dir_htdocs."..".
                                                 DIRECTORY_SEPARATOR) ?
                                        realpath($this->dir_htdocs."..".
                                                 DIRECTORY_SEPARATOR) :
                                        $this->dir_htdocs."..").DIRECTORY_SEPARATOR);
        // _________________________________________________________________ /app/data ___
        $this->dir_data   = new o2_dir($this->dir_home."data".DIRECTORY_SEPARATOR);
        $this->dir_data->crea();
        // ___________________________________________________________ /app/htdocs/img ___
        $this->dir_img       = new o2_dir($this->dir_htdocs."img".DIRECTORY_SEPARATOR);
        // ___________________________________________________________ /app/htdocs/css ___
        $this->dir_css       = new o2_dir($this->dir_htdocs."css".DIRECTORY_SEPARATOR);
        // ________________________________________________________________ /app/progs ___
        $this->dir_progs     = new o2_dir($this->dir_home."prgs".DIRECTORY_SEPARATOR);
        // ____________________________________________________ /app/progs/[developer] ___
        $this->dir_cvs_progs = new o2_dir($this->dir_progs.$this->developer.
                                          DIRECTORY_SEPARATOR);
        // _______________________________________________________________ /app/models ___
        $this->dir_models    = new o2_dir($this->dir_home."models".DIRECTORY_SEPARATOR);
        $this->dir_models->crea();
        // _________________________________________________________________ /app/logs ___
        $this->dir_logs = new o2_dir($this->dir_home."logs".DIRECTORY_SEPARATOR);
        $this->dir_logs->crea();
        // ________________________________________________________________ /app/users ___
        $users_local = new o2_dir($this->dir_home."users".DIRECTORY_SEPARATOR);
        $users_local->crea();

        }


    /**
     * Defines single user level o2_dir objects and ensures folders existence.
     *
     * Filesystem structure
     *
     *   /app                         home
     *     '-/users                   users personal folders
     *         '-/<user-name>         user home
     *             '-/<sess-ID>       user TMP folder per session
     *                 '-/jxuploads   uploaded files tmp folder
     *
     */
    function crea_cartelle_pers() {

        // ______________________________________________________________________ /app ___
        $user_local = trim(strtolower($this->user));
        // ________________________________________________________ /app/users/default ___
        $this->dir_user = new o2_dir($this->dir_home."users".DIRECTORY_SEPARATOR.
                                     $user_local.DIRECTORY_SEPARATOR);
        $this->dir_user->crea();
        // _________________________________________________ /app/users/default/sessid ___
        $this->dir_tmp = new o2_dir($this->dir_user."tmp".$this->session_id.
                                    DIRECTORY_SEPARATOR);
        $this->dir_tmp->crea();
        // _______________________________________ /app/users/default/sessid/jxuploads ___
        $this->dir_uploads = new o2_dir($this->dir_tmp."jxuploads".DIRECTORY_SEPARATOR);
        $this->dir_uploads->crea();

        }


    /**
     * Ritorna l'URL o il path al file da usare
     *
     * @param  string $cartella
     * @param  string $nome_file
     * @return string
     */
    function file_in_uso($cartella, $nome_file) {

        switch (strtoupper(trim($cartella))) {

            /**
             * Returns WEB path (URL) to CSS file, looking in two folders:
             *  - application CSS folder (app_root/htdocs/css/file.css)
             *  - runtime CSS folder (jxrnt/htdocs/css/file.css)
             */
            case "CSS":
                if ($nome_file && $this->dir_css->exists(basename($nome_file))) {
                    return "css/".basename($nome_file);
                    }
                $runtime_css_dir = new o2_dir($this->runtime->root."htdocs/css/");
                if ($nome_file && $runtime_css_dir->exists(basename($nome_file))) {
                    return $this->runtime->alias."css/".$nome_file;
                    }
                return "";
                break;
            // _____________________ Returns path to file in application models folder ___
            case "MODELS":
                if ($this->dir_models->exists(basename($nome_file))) {
                    return $this->dir_models.basename($nome_file);
                    break;
                    }
                else {
                    if ($this->templ_path_inc) {
                        foreach ($this->templ_path_inc as $path) {
                            $dir = new o2_dir($path);
                            if ($dir->exists(basename($nome_file))) {
                                return $dir.basename($nome_file);
                                }
                            }
                        }
                    throw new o2_exception("Can't find templete <i>".$nome_file."</i>",
                                           o2error_MISSINGFILE);
                    }
                break;
            default:
                throw new o2_exception("Unknown folder <i>".$cartella.
                                       "<i> requested for file <i>".$nome_file."</i>",
                                       o2error_MISSINGFILE);
            }

        }


    /**
     * Reports SQL statistics to development console or to log file
     *
     */
    function sql_stat() {

        $app   = $_SESSION['o2_app'];
        $log   = $GLOBALS['jxsql_stat'];
        $total = $log['#TOTAL'];
        unset($log['#TOTAL']);
        ksort($log);
        foreach($log as &$views) {
            ksort($views);
            foreach ($views as &$tables) {
                ksort($tables);
                foreach($tables as &$dbs) {
                    ksort($dbs);
                    }
                }
            }
        unset($views);
        unset($tables);
        unset($dbs);
        $prg_len  = 0;
        $view_len = 0;
        $tab_len  = 0;
        $db_len   = 0;
        foreach ($log as $prg => $views) {
            $prg_len = max($prg_len, strlen($prg));
            foreach ($views as $view => $tables) {
                $view_len = max($view_len, strlen($view));
                foreach ($tables as $table => $dbs) {
                    $tab_len = max($tab_len, strlen($table));
                    foreach ($dbs as $db => $modes) {
                        $db_len = max($db_len, strlen($db));
                        }
                    }
                }
            }
        // ______________________________________________________ Log out code to file ___
        if ($app->log2file || $app->mutelog || $app->rpc_server) {
            $title = '[SQL STATISTICS] Read: '.intval($total['R']).
                                     ' Write: '.intval($total['W'])."\n";
            $code  = "\n ".str_pad('Program', $prg_len).'   '.str_pad('View', $view_len).
                     '   '.str_pad('Table', $tab_len).'   '.str_pad('DB', $db_len).
                     '   Read  Write'."\n ".
                     str_repeat('-', $prg_len + $view_len + $db_len + $tab_len + 23)."\n";
            foreach ($log as $prg => $views) {
                $prg_f = true;
                foreach ($views as $view => $tables) {
                    $view_f = true;
                    foreach ($tables as $table => $dbs) {
                        $tab_f = true;
                        foreach ($dbs as $db => $modes) {
                            $code.= ' '.($prg_f  ? str_pad($prg, $prg_len) :
                                                str_repeat(' ', $prg_len)).'   '.
                                        ($view_f ? str_pad($view, $view_len) :
                                                str_repeat(' ', $view_len)).'   '.
                                        ($tab_f ? str_pad($table, $tab_len) :
                                                str_repeat(' ', $tab_len)).'   '.
                                        str_pad($db, $db_len).' '.
                                        str_pad($modes['R'], 5, ' ', STR_PAD_LEFT).'  '.
                                        str_pad($modes['W'], 5, ' ', STR_PAD_LEFT)."\n";
                            $prg_f  = false;
                            $view_f = false;
                            $tab_f  = false;
                            }
                        }
                    }
                }
            $log_file = $app->dir_logs.$app->developer.".log";
            if (!$f_local = fopen($log_file, 'a')) {
                throw new o2_exception("Cannot access log file <i>".$log_file."</i>",
                                       o2error_IO);
                }
            if (fwrite($f_local, "\n".$title.$code) === FALSE) {
                throw new o2_exception("Cannot write to log file <i>".$log_file."</i>",
                                       o2error_IO);
                }
            fclose($f_local);
            }
        // ____________________________________________________________ Log to console ___
        if (!$app->mutelog && !$app->rpc_server) {
            $code = '<table cellspacing="0" cellpadding="0"><tr><th>Program</th>'.
                    '<th>View</th><th>Table</th><th>DB</th><th>Read</th>'.
                    '<th>Write</th></tr>';
            foreach ($log as $prg => $views) {
                $prg_f = true;
                foreach ($views as $view => $tables) {
                    $view_f = true;
                    foreach ($tables as $table => $dbs) {
                        $tab_f = true;
                        foreach ($dbs as $db => $modes) {
                            $code.= '<tr '.($prg_f ? 'class="dbgh"' : '').'><td>'.
                                            ($prg_f  ? $prg  : '&nbsp').'</td><td>'.
                                            ($view_f ? $view : '&nbsp').'</td><td>'.
                                            ($tab_f ? $table : '&nbsp').'</td><td>'.
                                            $db.'</td><td>'.$modes['R'].'</td><td>'.
                                                            $modes['W'].'</td></tr>';
                            $prg_f  = false;
                            $view_f = false;
                            $tab_f  = false;
                            }
                        }
                    }
                }
            $code .= '</table>';
            $uid   = "jxdbg".substr($app->runtime->microtime(), -10);
            $title = '<b>[SQL STATISTICS] Read: '.intval($total['R']).
                                        ' Write: '.intval($total['W']).'</b>';
            // _____________________________________________________ Create detail DIV ___
            $code  = "<div class='jxlogtrace' style='display:none;' id='".$uid."'>".$code.
                     "</div>";
            // _______________________________________________ Log out code on console ___
            $code  = "<span class='jxlogactive' style='cursor:pointer;' ".
                     "onclick='o2jse.lab.ocd(this,\"".$uid."\");'>[+]</span> ".
                     $title.$code;
            $code  = "o2jse.lab.getResult(null,'".addcslashes($code, "'\\\r\n")."');\n";
            if ($GLOBALS['jxjs']) {
                $app->js_cache.= $code."\n";
                }
            else {
                print "<script> ".$code."</script>\n";
                }
            }
        }

    }


/**
 * This class is a container for cron related methods
 *
 */
class o2_cron {

    /**
     * Registers and unregisters application scheduler on cron-system
     *
     * @param  boolean $on   ON to register, OFF to unregister
     * @return boolean
     */
    static function register($on = true) {

        $rnt = $_SESSION['o2_app']->runtime;
        if ($on) {
            // ____ Register local host for Default service to activate it for network ___
            o2_service::register_host();
            }
        if ($rnt->mswin) {
            return self::register_mswin($on);
            }
        else {
            return self::register_linux($on);
            }

        }


    /**
     * Returns TRUE if application scheduler is registered on cron-system, else FALSE
     *
     * @return boolean
     */
    static function is_registered() {

        $app = $_SESSION['o2_app'];
        $rnt = $app->runtime;
        if ($rnt->mswin) {
            return self::is_registered_mswin();
            }
        else {
            return self::is_registered_linux();
            }

        }


    static function register_linux($on = true) {

        $app      = $_SESSION['o2_app'];
        $rnt      = $app->runtime;
        $php      = $rnt->php_engine;
        // ____________________________ Part of script used to look for scheduled task ___
        $script   = $app->dir_htdocs.$app->nome.'.php jxrnt="'.$rnt->rnt_script.
                    '" user=jxsys '.o2_scheduler::$sched_word;
        // ________________________ Create a php script to be used as crontab "editor" ___
        $code     = "<?php\n".
                    '$tmp_cron = (isset($argv[1]) ? $argv[1] : "");'."\n".
                    'if (!is_writable($tmp_cron)) {'."\n".
                    '    exit("Can\'t write to ".$tmp_cron);'."\n    }\n".
                    '$crontab = file($tmp_cron);'."\n".
                    '$key     = false;'."\n".
                    'foreach($crontab as $k=>$l){'.
                    'if(strpos($l,\''.$script.'\')!==false){$key=$k;break;}}'."\n";
        // _________________________________ Set ON: only if job is already registered ___
        if ($on) {
            // _________________________________________ String to be added in crontab ___
            $cron_str = "* * * * * ".$php." ".$script." >/dev/null 2>&1 || true\n";
            $code    .= 'if ($key === false) {'."\n".
                        '    $crontab[] = \''.$cron_str.'\';'."\n";
            }
        // ________________________________ Set OFF: only if job is not registered yet ___
        else {
            $code.= 'if ($key !== false) {'."\n".
                    '    unset($crontab[$key]);'."\n";
            }
        // _______________________________________________ Save temporary crontab file ___
        $code  .= "    sleep(1);\n".
                  '    file_put_contents($tmp_cron, implode("", $crontab));'."\n".
                  "    }\n".
                  "exit();\n?>";
        // _______________________________________________ Save php script as "editor" ___
        $editor = tempnam($app->dir_tmp, "jxcron_");
        file_put_contents($editor, $code);
        // ____________________________ Execute crontab -e with php script as "editor" ___
        $cmd = 'export EDITOR="'.$php." ".$editor.'"; crontab -e';
        $rnt->batch_exec($cmd);

        }


    /**
     * Registers and unregisters application scheduler on Windows SchTasks
     *
     * @param  boolean $on   ON to register, OFF to unregister
     * @return boolean
     */
    static function register_mswin($on = true) {

        $app = $_SESSION['o2_app'];
        $rnt = $app->runtime;
        $id  = "jx".md5($app->dir_htdocs.$app->nome);
        if ($on) {
            $php    = '"""'.$rnt->php_engine.'"""';
            $script = $php." ".$app->dir_htdocs.$app->nome.
                        '.php jxrnt="'.$rnt->rnt_script.
                        '" user=jxsys '.o2_scheduler::$sched_word;
            $cmd    = 'SCHTASKS /Create /SC MINUTE /MO 1 /TN '.$id.' /RU "" /TR "'.
                      $script.'" /F';
            }
        else {
            $cmd = 'SCHTASKS /Delete /TN '.$id.' /F';
            }
        $rnt->batch_exec($cmd);

        }


    /**
     * Returns TRUE if application scheduler is registered on Linux crontab, else FALSE
     *
     * @return boolean
     */
    static function is_registered_linux() {

        $app    = $_SESSION['o2_app'];
        $script = $app->dir_htdocs.$app->nome.".php";
        $res    = shell_exec("crontab -l");
        $res    = explode("\n", $res);
        $ret    = false;
        foreach ($res as $res_line) {
            $res_line = trim($res_line);
            if ($res_line && substr($res_line, 0, 1) !== "#") {
                if (stripos($res_line, $script)) {
                    $ret = true;
                    break;
                    }
                }
            }
        return $ret;

        }


    /**
     * Returns TRUE if application scheduler is registered on Windows SchTasks, else FALSE
     *
     * @return boolean
     */
    static function is_registered_mswin() {

        $app    = $_SESSION['o2_app'];
        $id     = "jx".md5($app->dir_htdocs.$app->nome);
        $script = $app->dir_htdocs.$app->nome.".php";
        $res    = shell_exec("SCHTASKS /Query /FO CSV");
        $res    = explode("\n", $res);
        $ret    = false;
        unset($res[0]);
        foreach ($res as $res_line) {
            if ($res_line) {
                $pars = str_getcsv($res_line);
                if (trim($pars[0]) == "\\".$id) {
                    $ret = true;
                    break;
                    }
                }
            }
        return $ret;

        }

    }


/**
 * A job is an execution model, consisting of a program name and a set of parameters to
 * call with.
 *
 */
class o2_job {

    static $jobs_table = 'jx_jobs'; /* System table for jobs storage                    */

    public $id            = 0;       /* Job unique ID                                   */
    public $name          = '';      /* Job name                                        */
    public $desc          = '';      /* Job description                                 */
    public $prg           = '';      /* Program to be executed                          */
    public $service       = '';      /* Service                                         */
    public $params        = array(); /* Parameters list to be passed to program         */
    public $keep_days     = 0;       /* Number of days run instances are conserved      */
    public $max_instances = 0;       /* Maximum of job instances that can run at a time */
    public $block_mode    = 'B';     /* Strategy for blocked job run: [B]lock|[Q]ueue   */
    public $disabled      = false;   /* If job is disabled it can't run                 */


    public function __construct($id,
                                $name,
                                $prg,
                                $params        = array(),
                                $desc          = '',
                                $keep_days     = 0,
                                $max_instances = false,
                                $block_mode    = 'B',
                                $disabled      = false,
                                $service       = false) {

        $app                 = $_SESSION['o2_app'];
        $this->id            = intval($id);
        $this->name          = $name;
        $this->prg           = $prg;
        $this->params        = $params;
        $this->desc          = $desc;
        $this->keep_days     = intval($keep_days);
        $this->max_instances = intval($max_instances);
        $block_mode          = strtoupper($block_mode);
        $this->block_mode    = ($block_mode == 'B' || $block_mode == 'Q' ?
                                $block_mode : 'B');
        $this->disabled      = ($disabled ? true : false);
        // _______________________________________________________________ Set service ___
        $service = strtoupper($service);
        if ($service && in_array($service, $app->services)) {
            $this->service = $service;
            }
        else {
            $this->service = '';
            }

        }


    /**
     * Returns a job object from db data with ID = $job_id
     *
     * @param  integer $job_id   Job ID to look for
     * @return o2_job
     */
    static function get_by_id($job_id) {

        $app     = $_SESSION['o2_app'];
        $tab     = $app->get_table(self::$jobs_table);
        $db      = $tab->db;
        $server  = $db->server;
        $where   = $tab->campi['job_id']->nome_fisico.'='.$job_id;
        $co      = constant('o2_'.$server->type.'_o');
        $cc      = constant('o2_'.$server->type.'_c');
        $select  = $tab->campi['job_id']->nome_fisico.' '.$co.'JOB_ID'.$cc.','.
                   $tab->campi['job_name']->nome_fisico.' '.$co.'JOB_NAME'.$cc.','.
                   $tab->campi['job_prg']->nome_fisico.' '.$co.'JOB_PRG'.$cc.','.
                   $tab->campi['job_service']->nome_fisico.' '.$co.'JOB_SERVICE'.$cc.','.
                   $tab->campi['job_params']->nome_fisico.' '.$co.'JOB_PARAMS'.$cc.','.
                   $tab->campi['job_keep_days']->nome_fisico.' '.$co.'JOB_KEEP_DAYS'.
                                                                 $cc.','.
                   $tab->campi['job_max_instances']->nome_fisico.' '.
                                                          $co.'JOB_MAX_INSTANCES'.$cc.','.
                   $tab->campi['job_block_mode']->nome_fisico.' '.$co.'JOB_BLOCK_MODE'.
                                                                  $cc.','.
                   $tab->campi['job_disabled']->nome_fisico.' '.$co.'JOB_DISABLED'.
                                                                $cc.','.
                   $tab->campi['job_desc']->nome_fisico.' '.$co.'JOB_DESC'.$cc;
        $job_rec = o2_gateway::recordset($server->type,
                                         $server->server,
                                         $server->user,
                                         $server->password,
                                         $db->nome,
                                         $db->proprietario,
                                         $tab->nome,
                                         'jx_jobs',
                                         $select,
                                         $where,
                                         '',
                                         1);
        $job_rec = $job_rec[0];
        if ($job_rec['JOB_ID']) {
            return new o2_job($job_rec['JOB_ID'],
                              $job_rec['JOB_NAME'],
                              $job_rec['JOB_PRG'],
                              unserialize($job_rec['JOB_PARAMS']),
                              $job_rec['JOB_DESC'],
                              $job_rec['JOB_KEEP_DAYS'],
                              $job_rec['JOB_MAX_INSTANCES'],
                              $job_rec['JOB_BLOCK_MODE'],
                              $job_rec['JOB_DISABLED'],
                              $job_rec['JOB_SERVICE']);
            }
        else {
            return false;
            }

        }


    /**
     * Returns a job object from db data with Name = $job_name if it exists, else FALSE
     *
     * @param  string $job_name   Job Name to look for
     * @return o2_job
     */
    static function get_by_name($job_name) {

        $app     = $_SESSION['o2_app'];
        $tab     = $app->get_table(self::$jobs_table);
        $db      = $tab->db;
        $server  = $db->server;
        $where   = $tab->campi["job_name"]->nome_fisico."='".$job_name."'";
        $co      = constant("o2_".$server->type."_o");
        $cc      = constant("o2_".$server->type."_c");
        $select  = $tab->campi["job_id"]->nome_fisico." ".$co."JOB_ID".$cc.",".
                   $tab->campi["job_name"]->nome_fisico." ".$co."JOB_NAME".$cc.",".
                   $tab->campi["job_prg"]->nome_fisico." ".$co."JOB_PRG".$cc.",".
                   $tab->campi["job_service"]->nome_fisico." ".$co."JOB_SERVICE".$cc.",".
                   $tab->campi["job_params"]->nome_fisico." ".$co."JOB_PARAMS".$cc.",".
                   $tab->campi["job_keep_days"]->nome_fisico." ".$co."JOB_KEEP_DAYS".
                                                                   $cc.",".
                   $tab->campi["job_max_instances"]->nome_fisico." ".
                                                          $co."JOB_MAX_INSTANCES".$cc.",".
                   $tab->campi["job_block_mode"]->nome_fisico." ".$co."JOB_BLOCK_MODE".
                                                                  $cc.",".
                   $tab->campi["job_disabled"]->nome_fisico." ".$co."JOB_DISABLED".
                                                                $cc.",".
                   $tab->campi["job_desc"]->nome_fisico." ".$co."JOB_DESC".$cc;
        $job_rec = o2_gateway::recordset($server->type,
                                         $server->server,
                                         $server->user,
                                         $server->password,
                                         $db->nome,
                                         $db->proprietario,
                                         $tab->nome,
                                         "jx_jobs",
                                         $select,
                                         $where,
                                         "",
                                         1);
        if (count($job_rec)) {
            $job_rec = $job_rec[0];
            $job     = new o2_job($job_rec['JOB_ID'],
                                  $job_rec['JOB_NAME'],
                                  $job_rec['JOB_PRG'],
                                  unserialize($job_rec['JOB_PARAMS']),
                                  $job_rec['JOB_DESC'],
                                  $job_rec['JOB_KEEP_DAYS'],
                                  $job_rec['JOB_MAX_INSTANCES'],
                                  $job_rec['JOB_BLOCK_MODE'],
                                  $job_rec['JOB_DISABLED'],
                                  $job_rec['JOB_SERVICE']);
            return $job;
            }
        else {
            return false;
            }

        }


    /**
     * Returns a nodes list to be used in a tree-view control, to display jobs and
     * related informations
     *
     * @param  mix     $root           Root node ID
     * @param  integer $desc_len       Max number of chars for nodes descritions
     * @param  boolean $group_by_srv   Group jobs by service
     * @param  boolean $simple_list    Skip infos about last and next scheduled executions
     * @return array
     */
    static function get_for_tree($root          = 0,
                                 $desc_len      = 50,
                                 $group_by_srv  = false,
                                 $order_by_name = false,
                                 $simple_list   = false) {

        $app      = $_SESSION['o2_app'];
        $aspid    = $app->vars['_area']->valore;
        $tab      = $app->get_table(self::$jobs_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $f        = $tab->campi;
        $select   = $f['job_id'].' '.$co.'JOB_ID'.$cc.','.
                    $f['job_name'].' '.$co.'JOB_NAME'.$cc.','.
                    $f['job_prg'].' '.$co.'JOB_PRG'.$cc.','.
                    $f['job_service'].' '.$co.'JOB_SERVICE'.$cc.','.
                    $f['job_params'].' '.$co.'JOB_PARAMS'.$cc.','.
                    $f['job_keep_days'].' '.$co.'JOB_KEEP_DAYS'.$cc.','.
                    $f['job_disabled'].' '.$co.'JOB_DISABLED'.$cc.','.
                    $f['job_desc'].' '.$co.'JOB_DESC'.$cc;
        $order_by = $f['job_name'].' ASC';
        $job_recs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_jobs',
                                          $select,
                                          '',
                                          ($order_by_name ? $order_by : ''),
                                          1000000);
        $list     = array();
        $img_path = '<jx>/img/jobs/tree/';
        foreach ($job_recs as $job_rec) {
            $srv_id = $job_rec['JOB_SERVICE'].'_srv';
            // __________________________________________ Create services folder nodes ___
            if ($group_by_srv) {
                if (!isset($list[$srv_id])) {
                    $list[$srv_id] = array($root,
                                           ($job_rec['JOB_SERVICE'] ?
                                            o2_str_nice($job_rec['JOB_SERVICE']) :
                                            'Default'),
                                           $img_path.'service.png',
                                           'srv',
                                           0);
                    }
                }
            // _______________________________________________________________ Add job ___
            $job_id           = ($job_rec['JOB_ID'] ? $job_rec['JOB_ID'] : 0);
            $id               = 'jxjob_'.$job_id;
            $list[$id]        = array(($group_by_srv ? $srv_id : $root),
                                      $job_rec['JOB_NAME'],
                                      $img_path.($job_rec['JOB_DISABLED'] ?
                                                 'job_dis.png' : 'job.png'),
                                      'job',
                                      $job_id);
            if ($job_rec['JOB_DESC']) {
                $desc              = $job_rec['JOB_DESC'];
                $desc              = (strlen($desc) < $desc_len ?
                                      $desc : substr($desc, 0, $desc_len - 3).'...');
                $list[$id.'_desc'] = array($id,
                                           '<i>&nbsp;'.$desc.'</i>',
                                           null,
                                           'desc',
                                           $job_id);
                }
            // ________________________________________________________ Add prg module ___
            $list[$id.'_prg'] = array($id,
                                      'Module: '.$job_rec['JOB_PRG'],
                                      $img_path.'prg.png',
                                      'prg',
                                      $job_id);
            // _____________________________________________________ Add module params ___
            if ($job_rec['JOB_PARAMS']) {
                foreach (unserialize($job_rec['JOB_PARAMS']) as $par_id => $par) {
                    $desc                      = $par_id.': '.$par;
                    $desc                      = (strlen($desc) < $desc_len ? $desc :
                                                  substr($desc, 0, $desc_len - 3).'...');
                    $list[$id.'_par_'.$par_id] = array($id.'_prg',
                                                       $desc,
                                                       $img_path.'par.png',
                                                       'param',
                                                       $job_id);
                    }
                }
            // ___________________________________________________________ Add service ___
            if (!$group_by_srv) {
                $list[$id.'_srv'] = array($id,
                                          'Service: '.
                                          ($job_rec['JOB_SERVICE'] ?
                                           o2_str_nice($job_rec['JOB_SERVICE']) :
                                           'Default'),
                                          $img_path.'service.png',
                                          'srv',
                                          $job_id);
                }
            if (!$simple_list) {
                // ________________________________________________ Add last execution ___
                $last = o2_run_job::get_last_job($job_id, $aspid);
                if ($last) {
                    $last_img          = $img_path.
                                         ($last->error || $last->progress < 100 ?
                                          'ko.png' : 'ok.png');
                    $list[$id.'_last'] = array($id,
                                             'Last run: '.date('d/m/Y H:i', $last->start),
                                             $last_img,
                                             'last',
                                             $job_id);
                    }
                // ______________________________________ Add next scheduled execution ___
                $next = o2_scheduler::get_next_job($job_id);
                if ($next['sched_id']) {
                    $sched             = o2_scheduler::get_task_by_id($next['sched_id']);
                    $sched_desc        = $sched->get_description();
                    $list[$id.'_next'] = array($id,
                                               'Next run: '.
                                                ($next['next'] === false ? 'Never' :
                                                    ($next['next'] === 0 ? 'Waiting...' :
                                                    $next['next']->format('d/m/Y H:i'))),
                                                $img_path.'next.png',
                                                'next',
                                                $job_id);
                    $list[$id.'_nextdesc'] = array($id.'_next',
                                                   '<i>'.$sched_desc.'</i>',
                                                   $img_path.'desc.png',
                                                   'nextdesc',
                                                   $job_id);
                    }
                }
            // _____________________________________________ Add run-jobs keeping time ___
            $list[$id.'_keep'] = array($id,
                                       'Keeping time: '.
                                       ($job_rec['JOB_KEEP_DAYS'] ?
                                        $job_rec['JOB_KEEP_DAYS'].' days' :
                                        'forever'),
                                       $img_path.'trash.png',
                                       'keep',
                                       $job_id);
            }
        return $list;

        }


    /**
     * Creates a job on db data.
     * If job ID is zero then next free ID is assigned to job.
     *
     */
    public function create() {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$jobs_table);
        $db     = $tab->db;
        $server = $db->server;
        // ________________________________________________ Check for one existing job ___
        $exists = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_jobs",
                                        "*",
                                        "",
                                        "",
                                        1);
        $fields = array($tab->campi["job_id"]->nome_fisico,
                        $tab->campi["job_name"]->nome_fisico,
                        $tab->campi["job_desc"]->nome_fisico,
                        $tab->campi["job_prg"]->nome_fisico,
                        $tab->campi["job_service"]->nome_fisico,
                        $tab->campi["job_params"]->nome_fisico,
                        $tab->campi["job_max_instances"]->nome_fisico,
                        $tab->campi["job_block_mode"]->nome_fisico,
                        $tab->campi["job_disabled"]->nome_fisico,
                        $tab->campi["job_keep_days"]->nome_fisico);
        $id_val = ($this->id ? $this->id :
                   (count($exists) ?
                    "(SELECT MAX(".$tab->campi["job_id"]->nome_fisico.")+1 FROM ".
                    o2tab_qname(self::$jobs_table).")" :
                    "1"));
        $values = array($id_val,
                        "'".$this->name."'",
                        "'".$this->desc."'",
                        "'".$this->prg."'",
                        "'".$this->service."'",
                   "'".o2_gateway::normalize($server->type, serialize($this->params))."'",
                        $this->max_instances,
                        "'".$this->block_mode."'",
                        "'".($this->disabled ? "1" : "0")."'",
                        $this->keep_days);
        o2_gateway::insertrec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              "jx_jobs",
                              $fields,
                              $values);
        // _____________________________________________________ Return new created ID ___
        $select = " MAX(".$tab->campi["job_id"]->nome_fisico.") AS NEW_ID";
        $new_id = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_jobs",
                                        $select,
                                        "",
                                        "",
                                        1);
        return $new_id[0]["NEW_ID"];

        }


    /**
     * Saves existing job on db data
     *
     */
    public function save() {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$jobs_table);
        $db     = $tab->db;
        $server = $db->server;
        $dis    = ($this->disabled ? "1" : "0");
        $sets   = array($tab->campi["job_name"]->nome_fisico    => "'".$this->name."'",
                        $tab->campi["job_desc"]->nome_fisico    => "'".$this->desc."'",
                        $tab->campi["job_prg"]->nome_fisico     => "'".$this->prg."'",
                        $tab->campi["job_service"]->nome_fisico => "'".$this->service."'",
                        $tab->campi["job_params"]->nome_fisico  =>
                   "'".o2_gateway::normalize($server->type, serialize($this->params))."'",
                        $tab->campi["job_max_instances"]->nome_fisico =>
                                                                     $this->max_instances,
                        $tab->campi["job_block_mode"]->nome_fisico =>
                                                                "'".$this->block_mode."'",
                        $tab->campi["job_disabled"]->nome_fisico => "'".$dis."'",
                        $tab->campi["job_keep_days"]->nome_fisico => $this->keep_days);
        $where  = $tab->campi["job_id"]->nome_fisico."=".$this->id;
        $res    = o2_gateway::modifyrec($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_jobs",
                                        $sets,
                                        $where);

        }


    /**
     * Deletes existing job on db data
     *
     */
    public function delete($del_run_jobs = false) {

        $app    = $_SESSION['o2_app'];
        // ________________________________________________________________ Delete job ___
        $tab    = $app->get_table(self::$jobs_table);
        $db     = $tab->db;
        $server = $db->server;
        $where  = $tab->campi["job_id"]->nome_fisico."=".$this->id;
        o2_gateway::deleterec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              "jx_jobs",
                              $where);
        // ______________________________________________________ Delete job schedules ___
        $tab    = $app->get_table(o2_scheduler::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $where  = $tab->campi["sched_job"]->nome_fisico."=".$this->id;
        o2_gateway::deleterec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              "jx_scheduler",
                              $where);
        // ______________________________________________________ Delete job instances ___
        if ($del_run_jobs) {
            $tab    = $app->get_table(o2_run_job::$run_table);
            $db     = $tab->db;
            $server = $db->server;
            $where  = $tab->campi["job_id"]->nome_fisico."=".$this->id;
            o2_gateway::deleterec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  "jx_run_jobs",
                                  $where);
            }

        }


    /**
     * Runs job: job is queued on db data, then a separated php process is run to execute
     * queued job.
     * (When no ASP-ID is set on instance, job module is executed once for every defined
     * ASP-ID.)
     * If a $sched_id value is passed then run instance is linked to scheduler task.
     * Function returns new run-job instance ID if queued else FALSE, if job is disabled
     * or maximum number of allowed instances in reached with full block-mode.
     *
     * @param  string  $aspid      ASPI-ID to run job for
     * @param  array   $params     List of overriding parameters to call program with
     * @param  integer $sched_id   Scheduler ID which requested execution
     * @return integer
     */
    public function run($aspid, $params = array(), $sched_id = false) {

        // ______________________________________________________________ Job disabled ___
        if ($this->disabled) {
            return false;
            }
        // _______________________________________________________________ Job enabled ___
        else {
            $app = $_SESSION['o2_app'];
            $rnt = $app->runtime;
            $cnt = 0;
            // _____________________________________________ Check max instances limit ___
            if ($this->max_instances) {
                // ___________________________________________ Count running instances ___
                foreach (o2_run_job::get_by_job($this->id) as $run_id => $run_job) {
                    if ($run_job->is_running()) {
                        $cnt++;
                        }
                    }
                // _______________________________________ Max instances limit reached ___
                if ($cnt >= $this->max_instances) {
                    // ___________________________________________ Check blocking-mode ___
                    if ($this->block_mode == "Q") {
                        $run_job = o2_run_job::queue($this,
                                                     $params,
                                                     $aspid,
                                                     false,
                                                     false,
                                                     false,
                                                     $sched_id,
                                                     $app->host);
                        return $run_job->id;
                        }
                    else {
                        return false;
                        }
                    }
                }
            $run_job = o2_run_job::queue($this,
                                         $params,
                                         $aspid,
                                         false,
                                         false,
                                         false,
                                         $sched_id,
                                         $app->host);
            // ____________________________ Set run-job status from Queued to Starting ___
            $run_job->update_status('S');
            $cmd = '"'.$rnt->php_engine.'" '.$app->dir_htdocs.$app->nome.
                                     '.php jxrnt="'.$rnt->rnt_script.
                                        '" user='.$app->user.
                                        ($app->block_md5_pwd ?
                                         ' jxotp='.$app->otp() :
                                         ' password='.$app->password).
                                        ($app->run_level == "DEV" ?
                                         ' dev='.$app->developer.' key='.$app->dev_key :
                                         '').
                                        ' '.o2_run_job::$run_word.'='.$run_job->id;
            $rnt->batch_exec($cmd);
            return $run_job->id;
            }

        }


    /**
     * Clear executed instances for job, according with "keep_days" parameter
     *
     */
    public function clear() {

        o2_run_job::clear_by_job($this->id);

        }

    }


/**
* A running job is an execution istance, created when a job is run.
*
*/
class o2_run_job {

    static $run_table = 'jx_running_jobs'; /* System table for run-jobs storage         */
    static $run_word  = 'jxrun';           /* Incoming parameter with queued run-job ID */
    static $job_word  = 'jxjob';           /* Incoming parameter with job ID to run     */

    public $id        = 0;                 /* Instance unique ID                        */
    public $job       = 0;                 /* Job ID running within the instance        */
    public $prg       = '';                /* Program called by instance                */
    public $service   = '';                /* Service                                   */
    public $host      = '';                /* Host name running job process             */
    public $params    = array();           /* Parameters passed to called program       */
    public $user      = '';                /* Instance running user                     */
    public $developer = false;             /* Developer running instance                */
    public $start     = 0;                 /* Start time-stamp                          */
    public $end       = 0;                 /* End time-stamp                            */
    public $aspid     = '';                /* Instance running ASP-ID                   */
    public $status    = '';                /* Status [Queued|Running|Completed|Killed]  */
    public $error     = false;             /* Error message, if any                     */
    public $pid       = 0;                 /* System process ID for running instance    */
    public $sessid    = false;             /* Session ID of run-job instance            */
    public $sched_id  = 0;                 /* Scheduler task ID requesting execution    */
    public $progress  = 0;                 /* Running instance percentage progress      */
    public $files     = array();           /* List of run-job produced files            */
    public $exe_time  = 0;                 /* Minimum execution date-time               */


    /**
     * Returns a run-job object from db data with Run-ID = $run_id
     *
     * @param  integer $run_id   Run-job ID to look for
     * @return o2_run_job
     */
    static function get_by_id($run_id) {

        $app                = $_SESSION['o2_app'];
        $tab                = $app->get_table(self::$run_table);
        $db                 = $tab->db;
        $server             = $db->server;
        $where              = $tab->campi['run_id']->nome_fisico."='".$run_id."'";
        $co                 = constant('o2_'.$server->type.'_o');
        $cc                 = constant('o2_'.$server->type.'_c');
        $f                  = $tab->campi;
        $select             = $f['run_id'].' '.$co.'RUN_ID'.$cc.','.
                              $f['job_id'].' '.$co.'JOB_ID'.$cc.','.
                              $f['run_prg'].' '.$co.'RUN_PRG'.$cc.','.
                              $f['run_service'].' '.$co.'RUN_SERVICE'.$cc.','.
                              $f['run_host'].' '.$co.'RUN_HOST'.$cc.','.
                              $f['run_user'].' '.$co.'RUN_USER'.$cc.','.
                              $f['run_developer'].' '.$co.'RUN_DEVELOPER'.$cc.','.
                              $f['run_params'].' '.$co.'RUN_PARAMS'.$cc.','.
                              $f['run_aspid'].' '.$co.'RUN_ASPID'.$cc.','.
                              $f['run_status'].' '.$co.'RUN_STATUS'.$cc.','.
                              $f['run_pid'].' '.$co.'RUN_PID'.$cc.','.
                              $f['run_sessid'].' '.$co.'RUN_SESSID'.$cc.','.
                              $f['run_sched_id'].' '.$co.'RUN_SCHED_ID'.$cc.','.
                              $f['run_progress'].' '.$co.'RUN_PROGRESS'.$cc.','.
                              $f['run_error'].' '.$co.'RUN_ERROR'.$cc.','.
                              $f['run_files'].' '.$co.'RUN_FILES'.$cc.','.
                              $f['run_start_date'].' '.$co.'RUN_START_DATE'.$cc.','.
                              $f['run_start_time'].' '.$co.'RUN_START_TIME'.$cc.','.
                              $f['run_end_date'].' '.$co.'RUN_END_DATE'.$cc.','.
                              $f['run_end_time'].' '.$co.'RUN_END_TIME'.$cc.','.
                              $f['run_at_date'].' '.$co.'RUN_AT_DATE'.$cc.','.
                              $f['run_at_time'].' '.$co.'RUN_AT_TIME'.$cc;
        $run_rec            = o2_gateway::recordset($server->type,
                                                    $server->server,
                                                    $server->user,
                                                    $server->password,
                                                    $db->nome,
                                                    $db->proprietario,
                                                    $tab->nome,
                                                    'jx_running_jobs',
                                                    $select,
                                                    $where,
                                                    '',
                                                    1);
        $run_rec            = $run_rec[0];
        $run_job            = new o2_run_job();
        $run_job->id        = $run_rec['RUN_ID'];
        $run_job->job       = $run_rec['JOB_ID'];
        $run_job->prg       = $run_rec['RUN_PRG'];
        $run_job->service   = $run_rec['RUN_SERVICE'];
        $run_job->host      = $run_rec['RUN_HOST'];
        $run_job->params    = unserialize($run_rec['RUN_PARAMS']);
        $run_job->user      = $run_rec['RUN_USER'];
        $run_job->developer = $run_rec['RUN_DEVELOPER'];
        $run_job->aspid     = $run_rec['RUN_ASPID'];
        $run_job->status    = $run_rec['RUN_STATUS'];
        $run_job->pid       = $run_rec['RUN_PID'];
        $run_job->sessid    = $run_rec['RUN_SESSID'];
        $run_job->sched_id  = $run_rec['RUN_SCHED_ID'];
        $run_job->progress  = $run_rec['RUN_PROGRESS'];
        $run_job->error     = $run_rec['RUN_ERROR'];
        $run_job->files     = unserialize($run_rec['RUN_FILES']);
        if ($run_rec['RUN_START_DATE']) {
            $sd             = $run_rec['RUN_START_DATE'];
            $st             = $run_rec['RUN_START_TIME'];
            $start          = new DateTime($sd[0].$sd[1].$sd[2].$sd[3].'-'.
                                           $sd[4].$sd[5].'-'.
                                           $sd[6].$sd[7].' '.
                                           $st[0].$st[1].':'.
                                           $st[2].$st[3].':'.
                                           $st[4].$st[5]);
            $run_job->start = $start->getTimestamp();
            unset($start);
            }
        if ($run_rec['RUN_END_DATE']) {
            $ed           = $run_rec['RUN_END_DATE'];
            $et           = $run_rec['RUN_END_TIME'];
            $end          = new DateTime($ed[0].$ed[1].$ed[2].$ed[3].'-'.
                                         $ed[4].$ed[5].'-'.
                                         $ed[6].$ed[7].' '.
                                         $et[0].$et[1].':'.
                                         $et[2].$et[3].':'.
                                         $et[4].$et[5]);
            $run_job->end = $end->getTimestamp();
            unset($end);
            }
        if ($run_rec['RUN_AT_DATE']) {
            $ad           = $run_rec['RUN_AT_DATE'];
            $at           = $run_rec['RUN_AT_TIME'];
            $at_time      = new DateTime($ad[0].$ad[1].$ad[2].$ad[3].'-'.
                                         $ad[4].$ad[5].'-'.
                                         $ad[6].$ad[7].' '.
                                         $at[0].$at[1].':'.
                                         $at[2].$at[3].':'.
                                         $at[4].$at[5]);
            $run_job->exe_time = $at_time->getTimestamp();
            unset($at_time);
            }
        return $run_job;

        }


    /**
     * Queues a job on db data, ready for execution, and returns new created run-job
     *
     * @param  o2_job  $job          Job to be queued
     * @param  array   $run_params   List of overriding parameters to call program with
     * @param  string  $aspid        ASP-ID to run process for (blank = all ASP-IDs)
     * @param  string  $service      Service to override job service for execution
     * @param  string  $at_date      Posticipate execution to date
     * @param  string  $at_time      Posticipate execution to time
     * @param  integer $sched_id     Scheduler task ID which requested execution
     * @param  string  $host         Host to override assigned host for execution
     * @return o2_run_job
     */
    static function queue($job,
                          $run_params = array(),
                          $aspid      = false,
                          $service    = false,
                          $at_date    = false,
                          $at_time    = false,
                          $sched_id   = 0,
                          $host       = false) {

        // _________________________________________________ Queue on all active hosts ___
        if ($host == '*') {
            $list = array();
            foreach (o2_service::get_hosts() as $host) {
                $list[] = self::queue($job,
                                      $run_params,
                                      $aspid,
                                      $service,
                                      $at_date,
                                      $at_time,
                                      $sched_id,
                                      $host);
                }
            return $list;
            }
        // ___________________________________________ Queue on single host or service ___
        $app                = $_SESSION['o2_app'];
        $run_job            = new o2_run_job();
        $run_job->id        = $app->runtime->microtime();
        $run_job->job       = $job->id;
        $run_job->prg       = $job->prg;
        $run_job->params    = array_merge($job->params, (is_array($run_params) ?
                                                         $run_params : array()));
        $run_job->user      = $app->user;
        $run_job->developer = $app->developer;
        $run_job->aspid     = ($aspid ? $aspid : '');
        $run_job->status    = 'Q';
        $service            = strtoupper($service);
        $run_job->service   = ($app->service_redirect ? $app->service_redirect :
                               ($service && in_array($service, $app->services) ?
                                $service : $job->service));
        $run_job->sched_id  = $sched_id;
        if ($at_date || $at_time) {
            $ad                = ($at_date ? $at_date : date('Ymd'));
            $at                = ($at_time ? $at_time : date('His'));
            $at_time           = new DateTime($ad[0].$ad[1].$ad[2].$ad[3]."-".
                                              $ad[4].$ad[5]."-".
                                              $ad[6].$ad[7]." ".
                                              $at[0].$at[1].":".
                                              $at[2].$at[3].":".
                                              $at[4].$at[5]);
            $run_job->exe_time = $at_time->getTimestamp();
            $at_date           = date('Ymd', $run_job->exe_time);
            $at_time           = date('His', $run_job->exe_time);
            }
        else {
            $run_job->exe_time = 0;
            $at_date           = '00000000';
            $at_time           = '000000';
            }
        $run_job->start      = time();
        $tab                 = $app->get_table(self::$run_table);
        $db                  = $tab->db;
        $server              = $db->server;
        $f                   = $tab->campi;
        $fields              = array($f['run_id']->nome_fisico,
                                     $f['job_id']->nome_fisico,
                                     $f['run_prg']->nome_fisico,
                                     $f['run_params']->nome_fisico,
                                     $f['run_user']->nome_fisico,
                                     $f['run_developer']->nome_fisico,
                                     $f['run_start_date']->nome_fisico,
                                     $f['run_start_time']->nome_fisico,
                                     $f['run_aspid']->nome_fisico,
                                     $f['run_status']->nome_fisico,
                                     $f['run_service']->nome_fisico,
                                     $f['run_at_date']->nome_fisico,
                                     $f['run_at_time']->nome_fisico,
                                     $f['run_sched_id']->nome_fisico,
                                     $f['run_host']->nome_fisico);
        $values              = array("'".$run_job->id."'",
                                     ($run_job->job ? $run_job->job : '0'),
                                     "'".$run_job->prg."'",
                "'".o2_gateway::normalize($server->type, serialize($run_job->params))."'",
                                     "'".$run_job->user."'",
                                     "'".$run_job->developer."'",
                                     "'".date('Ymd', $run_job->start)."'",
                                     "'".date('His', $run_job->start)."'",
                                     "'".$run_job->aspid."'",
                                     "'".$run_job->status."'",
                                     "'".$run_job->service."'",
                                     "'".$at_date."'",
                                     "'".$at_time."'",
                                     ($run_job->sched_id ? $run_job->sched_id : '0'),
                                     "'".($host ? $host :
                                          ($app->jobs_local_host ? $app->host : ''))."'");
        $trans               = $app->isolated_trans;
        $app->isolated_trans = true;
        $res                 = o2_gateway::insertrec($server->type,
                                                     $server->server,
                                                     $server->user,
                                                     $server->password,
                                                     $db->nome,
                                                     $db->proprietario,
                                                     $tab->nome,
                                                     'jx_running_jobs',
                                                     $fields,
                                                     $values);
        o2dbs_commit($server->nome);
        $app->isolated_trans = $trans;
        return $run_job;

        }


    /**
     * Check if session is a job-worker.
     * A job-worker can be used to run both a queued run-job and a direct job.
     * If session is a job-worker executes job and returns TRUE, else FALSE.
     *
     * @return boolean
     */
    static function run_session() {

        // _________________________________________________ Run a queued job instance ___
        if (isset($_REQUEST[self::$run_word])) {
            $app                        = $_SESSION['o2_app'];
            $run_job                    = self::get_by_id($_REQUEST[self::$run_word]);
            $sessid_save                = $app->session_id;
            $app->session_id            = $run_job->id;
            $app->vars['_area']->valore = $run_job->aspid;
            $app->set_aspid();
            // _________________________________________ Set log to file for developer ___
            $app->log2file = true;
            $app->mutelog  = true;
            // ______________ Run job instance (sess-ID used to update run-job record) ___
            $run_job->run($sessid_save);
            // _____________________________________________ Reset original session-ID ___
            $app->session_id     = $sessid_save;
            // ________________________ Preserve session record removing from rollback ___
            $app->isolated_trans = true;
            return true;
            }
        // ________________________ Run a job directly creating its instance (NOT USED) __
        elseif (isset($_REQUEST[self::$job_word])) {
            $app                        = $_SESSION['o2_app'];
            $job                        = o2_job::get_by_id($_REQUEST[self::$job_word]);
            $run_job                    = self::queue($job, array(), $_REQUEST['aspid']);
            $sessid_save                = $app->session_id;
            $app->session_id            = $run_job->id;
            $app->vars['_area']->valore = $run_job->aspid;
            $app->set_aspid();
            // _________________________________________ Set log to file for developer ___
            $app->log2file = true;
            $app->mutelog  = true;
            // ______________ Run job instance (sess-ID used to update run-job record) ___
            $run_job->run($sessid_save);
            // _____________________________________________ Reset original session-ID ___
            $app->session_id     = $sessid_save;
            // ________________________ Preserve session record removing from rollback ___
            $app->isolated_trans = true;
            return true;
            }
        // ___________________________________________________ Session is not a worker ___
        else {
            return false;
            }

        }


    /**
     * If session is a job-worker returns run-job running in session, else FALSE
     *
     * @return o2_run_job
     */
    static function get_by_session() {

        $app = $_SESSION['o2_app'];
        if ($app->job_worker) {
            return $app->job_worker;
            }
        else {
            return false;
            }

        }


    /**
     * Run queued processes assigned to local host.
     *
     * Processes can be assigned to host in 2 ways:
     *  1. process is assigned to a service and host is registered for that service;
     *  2. process is assigned directly to host.
     *
     * Returns number of executed processes.
     *
     * @return integer
     */
    static function run_queued() {

        $app  = $_SESSION['o2_app'];
        $rnt  = $app->runtime;
        $res  = 0;
        // ________________________________________________ Update local host services ___
        foreach (o2_service::get_host_services($app->host) as $srv => $srv_nice) {
            o2_service::update_host_service($srv);
            }
        // _________________________ Get services for local host and queued executions ___
        $host_srvs = o2_service::get_all_host_services();
        // ________ Check for something to do before locking table (SELECT FOR UPDATE) ___
        if (self::check_list()) {
            $processes = self::get_list('', true);
            }
        else {
            $processes = array();
            }
        $running = array();
        $maxs    = array();
        $now     = time();
        // ___________________________________________________ Count running instances ___
        foreach ($processes as $run_job) {
            if ($run_job->is_running()) {
                $running[$run_job->job] = $running[$run_job->job] + 1;
                if (!isset($maxs[$run_job->job])) {
                    $job                 = o2_job::get_by_id($run_job->job);
                    $maxs[$run_job->job] = $job->max_instances;
                    }
                }
            }
        // ________________________________________________ Loop on pending executions ___
        foreach ($processes as $run_job) {
            if ($run_job->status == 'Q') {
                // ____________________________________ Process is assigned by service ___
                $by_service = !$run_job->host && isset($host_srvs[$run_job->service]);
                // _______________________________________ Process is assigned by host ___
                $by_host    = $run_job->host && ($run_job->host == $app->host);
                if (($by_service || $by_host) &&
                    $run_job->exe_time <= $now &&
                    ($maxs[$run_job->job] == 0 ||
                     $maxs[$run_job->job] > $running[$run_job->job])) {
                    $res++;
                    // ____________________ Set run-job status from Queued to Starting ___
                    $run_job->update_status('S');
                    // ______________________________ Get developer password if needed ___
                    $dev = false;
                    if ($run_job->developer) {
                        // ___________________________ Read list of allowed developers ___
                        include_once($rnt->root."developers");
                        $dev    = true;
                        $devkey = $rnt->developers[strtolower($run_job->developer)];
                        }
                    $cmd = '"'.$rnt->php_engine.'" '.
                           $app->dir_htdocs.$app->nome.'.php jxrnt="'.$rnt->rnt_script.
                           '" user='.$run_job->user.
                           ($app->block_md5_pwd ?
                            ' jxotp='.$app->otp($run_job->user) :
                            ' password='.$app->get_user_password($run_job->user)).
                           ($dev ? ' dev='.$run_job->developer. ' key='.$devkey : '').' '.
                           self::$run_word.'='.$run_job->id;
                    $rnt->batch_exec($cmd);
                    $running[$run_job->job] = $running[$run_job->job] + 1;
                    if (!isset($maxs[$run_job->job])) {
                        $job                 = o2_job::get_by_id($run_job->job);
                        $maxs[$run_job->job] = $job->max_instances;
                        }
                    }
                }
            }
        if ($res) {
            $tab = $app->get_table(self::$run_table);
            o2dbs_commit($tab->db->server->nome);
            }
        // __________________________________________________________ Update PIDs list ___
        o2_service::update_host_pids();
        return $res;

        }


    /**
     * Kills instances marked as "killed" and running on this host
     *
     * @return boolean
     */
    static function kill_by_host() {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$run_table);
        $db       = $tab->db;
        $server   = $db->server;
        $where    = $tab->campi['run_status']."='K' AND ".
                    $tab->campi['run_host']."='".$app->host."'";
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $select   = $tab->campi['run_id'].' '.$co.'RUN_ID'.$cc;
        $run_recs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_running_jobs',
                                          $select,
                                          $where,
                                          '',
                                          100);
        $ret      = 0;
        if (count($run_recs)) {
            foreach ($run_recs as $run_rec) {
                $run_job = self::get_by_id($run_rec['RUN_ID']);
                $run_job->kill();
                $ret++;
                }
            }
        return $ret;

        }


    /**
     * Executes run-job
     *
     * @param $sessid   Session-ID running instance in sessions table
     */
    public function run($sessid = false) {

        $app             = $_SESSION['o2_app'];
        $app->job_worker = $this;
        try {
            $this->start($sessid);
            // ______________________________________________ Commit open transactions ___
            $app->commit_all(true);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            }
        // ___________________________ If execution stopped (on-line) rise an exception___
        if ($app->progressivo_istanze) {
            $prg = $app->istanze_prg[$app->progressivo_istanze]->nome;
            o2_exception::warning("Batch execution stopped in prg <i>".$prg."</i>.");
            /**
             * TODO: Rirpristinare l'eccezione dopo corretti i job con on-line
             *
            $e   = new o2_exception("Batch execution stopped in prg <i>".$prg."</i>.",
                                    o2error_EXECUTION);
            $e->send();
             */
            }
        $this->end();

        }


    /**
     * Starts job execution, calling requested program with defined parameters
     *
     * @param $sessid   Session-ID running instance in sessions table
     */
    public function start($sessid = false) {

        $app         = $_SESSION['o2_app'];
        $tab         = $app->get_table(self::$run_table);
        $db          = $tab->db;
        $server      = $db->server;
        $this->start = time();
        $start_d     = date('Ymd', $this->start);
        $start_t     = date('His', $this->start);
        $f           = $tab->campi;
        $sets        = array($f['run_start_date']->nome_fisico => "'".$start_d."'",
                             $f['run_start_time']->nome_fisico => "'".$start_t."'",
                             $f['run_host']->nome_fisico       => "'".$app->host."'",
                             $f['run_pid']->nome_fisico        => ''.getmypid(),
                             $f['run_sessid']->nome_fisico     => "'".$sessid."'",
                             $f['run_progress']->nome_fisico   => '1',
                             $f['run_status']->nome_fisico     => "'R'");
        $where       = $f["run_id"]."='".$this->id."'";
        // ______________________________ Execute update query in isolated transaction ___
        $trans               = $app->isolated_trans;
        $app->isolated_trans = true;
        $res                 = o2_gateway::modifyrec($server->type,
                                                     $server->server,
                                                     $server->user,
                                                     $server->password,
                                                     $db->nome,
                                                     $db->proprietario,
                                                     $tab->nome,
                                                     'jx_running_jobs',
                                                     $sets,
                                                     $where);
        o2dbs_commit($server->nome);
        $app->isolated_trans = $trans;
        // ____________________________ Run start-prg (because not run in ->esecutivo) ___
        if ($app->prg_start) {
            $app->goto_prg($app->prg_start, false);
            }
        // _______________________________ Get prg code, considering CVS & Janox tools ___
        $prg_folder          = false;
        if (strpos($this->prg, "/")) {
            list($prg_folder, $prg_name) = explode("/", $this->prg);
            }
        if ($prg_folder) {
            $prg_txt = file_get_contents($app->runtime->root.'lib'.DIRECTORY_SEPARATOR.
                                         'prgs'.DIRECTORY_SEPARATOR.$prg_name.'.prg');
            }
        elseif ($app->dir_cvs_progs->exists($this->prg.'.prg')) {
            $prg_txt = file_get_contents($app->dir_cvs_progs.$this->prg.'.prg');
            }
        else {
            $prg_txt = file_get_contents($app->dir_progs.$this->prg.'.prg');
            }
        // _______________ Set parameters defined by names to position expected by prg ___
        $params = array($this->prg);
        $parts  = array();
        preg_match_all('/o2def::par\((\d+),\s*\"([^"]*)\",\s*\"([^"]*)\"/',
                       $prg_txt,
                       $parts);
        foreach ($parts[1] as $par_id) {
           $par_id--;
           $p_name   = $parts[2][$par_id];
           $model    = $parts[3][$par_id];
           $params[] = (isset($this->params[$p_name]) ?
                        $this->params[$p_name] :
                        $app->maschere[$model]->default);
           }
        // _______________________________________________ Run program with parameters ___
        call_user_func_array(array($app, 'intcall'), $params);

        }


    /**
     * Ends job execution and writes down completion db data on run-job record
     *
     */
    public function end() {

        $app                 = $_SESSION['o2_app'];
        $tab                 = $app->get_table(self::$run_table);
        $db                  = $tab->db;
        $server              = $db->server;
        $error               = ($GLOBALS["jxlasterror"] ?
                                ltrim($GLOBALS["jxlasterror"]->to_raw_text()) : "");
        $error               = o2_gateway::normalize($server->type, $error);
        $this->end           = time();
        $end_d               = date("Ymd", $this->end);
        $end_t               = date("His", $this->end);
        $pars                = serialize($this->files);
        $f                   = $tab->campi;
        $sets                = array($f["run_error"]->nome_fisico    => "'".$error."'",
                                     $f["run_end_date"]->nome_fisico => "'".$end_d."'",
                                     $f["run_end_time"]->nome_fisico => "'".$end_t."'",
                                     $f["run_progress"]->nome_fisico => "100",
                                     $f["run_files"]->nome_fisico    => "'".$pars."'",
                                     $f["run_status"]->nome_fisico   => "'C'");
        $where               = $f["run_id"]."='".$this->id."'";
        // ______________________________ Execute update query in isolated transaction ___
        $trans               = $app->isolated_trans;
        $app->isolated_trans = true;
        $res                 = o2_gateway::modifyrec($server->type,
                                                     $server->server,
                                                     $server->user,
                                                     $server->password,
                                                     $db->nome,
                                                     $db->proprietario,
                                                     $tab->nome,
                                                     "jx_running_jobs",
                                                     $sets,
                                                     $where);
        o2dbs_commit($server->nome);
        $app->isolated_trans = $trans;

        }


    /**
     * Updates table record of running-job with $new_status
     *
     * @param string $new_status
     */
    public function update_status($new_status) {

        $this->status = strtoupper($new_status);
        $app          = $_SESSION['o2_app'];
        $tab          = $app->get_table(self::$run_table);
        $db           = $tab->db;
        $server       = $db->server;
        $sets         = array($tab->campi['run_status']->nome_fisico =>
                                                                   "'".$this->status."'");
        $where        = $tab->campi["run_id"]."='".$this->id."'";
        // ______________________________ Execute update query in isolated transaction ___
        $res          = o2_gateway::modifyrec($server->type,
                                              $server->server,
                                              $server->user,
                                              $server->password,
                                              $db->nome,
                                              $db->proprietario,
                                              $tab->nome,
                                              'jx_running_jobs',
                                              $sets,
                                              $where);
        return true;

        }


    /**
     * Kills instance if it is running on this host, else it marks run-job record for
     * killing
     *
     * @return boolean
     */
    public function kill() {

        $app = $_SESSION['o2_app'];
        // __________ This host: physically stop process and set status to [C]ompleted ___
        if (!$this->host || !$this->pid) {
            $this->status = 'C';
            }
        elseif ($this->host == $app->host) {
            $app->runtime->kill($this->pid);
            $this->status = 'C';
            }
        // ____________________________ Not this host: mark run-job record as [K]illed ___
        else {
            $this->status = 'K';
            }
        // ___________________________________________________ Save run-job new status ___
        $tab    = $app->get_table(self::$run_table);
        $db     = $tab->db;
        $server = $db->server;
        $sets   = array($tab->campi['run_status']->nome_fisico => "'".$this->status."'");
        $where  = $tab->campi['run_id']."='".$this->id."'";
        // ______________________________ Execute update query in isolated transaction ___
        $trans               = $app->isolated_trans;
        $app->isolated_trans = true;
        $res                 = o2_gateway::modifyrec($server->type,
                                                     $server->server,
                                                     $server->user,
                                                     $server->password,
                                                     $db->nome,
                                                     $db->proprietario,
                                                     $tab->nome,
                                                     'jx_running_jobs',
                                                     $sets,
                                                     $where);
        o2dbs_commit($server->nome);
        $app->isolated_trans = $trans;

        }


    /**
     * Returns TRUE if run-job process is still running
     * If run-job has been run by an other host check is performed against PIDs list
     * written in jx-hosts table at last update
     *
     * @return boolean
     */
    public function is_running() {

        if ($this->status == 'R' && (!$this->end || $this->end < 0)) {
            $app = $_SESSION['o2_app'];
            // ________ GLOBAL var is used as cache to exe query only once in a script ___
            if (isset($GLOBALS['jxprocpids'])) {
                $pids = $GLOBALS['jxprocpids'];
                }
            else {
                if ($this->host == $app->host) {
                    $pids = $app->runtime->proc_list(true);
                    }
                else {
                    $pids = o2_service::get_host_pids($this->host);
                    }
                $GLOBALS['jxprocpids'] = $pids;
                }
            return isset($pids[$this->pid]);
            }
        else {
            return false;
            }

        }


    /**
     * Returns the run-job decoded status
     *
     * @return boolean
     */
    public function get_status() {


        $run    = $this->is_running();
        $queued = ($this->status == 'Q' && $this->progress < 1);
        $error  = ($this->error ||
                  (!$queued && !$run && ($this->progress < 100 || $this->status == 'R')) ?
                   true : false);
        return ($queued ? 'Queued' : ($error ? 'Error' :
                o2format($this->status, 'jxjob_status')));

        }


    /**
     * Sets execution progress to be shown as job completed percentage
     *
     * @param integer $perc
     */
    public function set_progress($perc) {

        $perc = min(100, intval($perc));
        if ($this->progress != $perc) {
            $app    = $_SESSION['o2_app'];
            $tab    = $app->get_table(self::$run_table);
            $db     = $tab->db;
            $server = $db->server;
            $perc   = ($perc ? $perc : '0');
            $sets   = array($tab->campi['run_progress']->nome_fisico => $perc);
            $where  = $tab->campi['run_id']."='".$this->id."'";
            // __________________________ Execute update query in isolated transaction ___
            $trans               = $app->isolated_trans;
            $app->isolated_trans = true;
            $res                 = o2_gateway::modifyrec($server->type,
                                                         $server->server,
                                                         $server->user,
                                                         $server->password,
                                                         $db->nome,
                                                         $db->proprietario,
                                                         $tab->nome,
                                                         'jx_running_jobs',
                                                         $sets,
                                                         $where);
            o2dbs_commit($server->nome);
            $app->isolated_trans = $trans;
            $this->progress      = $perc;
            }

        }


    /**
     * Returns HTML code to show a progress bar related to run-job progress
     *
     * @param  string $complete_action
     * @param  string $start_action
     * @return string
     */
    public function get_bar($complete_action = false, $start_action = false) {

        $app    = $_SESSION['o2_app'];
        $prg    = $app->istanze_prg[$app->progressivo_istanze];
        $run    = $this->is_running();
        $queued = ($this->status == 'Q' && $this->progress < 1);
        $wait   = ($this->start > (time() - 3)) && ($this->progress < 1);
        if (isset($prg->progress_ctrls['PB'.$this->id])) {
            $bar = $prg->progress_ctrls['PB'.$this->id];
            }
        else {
            $bar = new o2_ctrl_progress($this->id);
            // ______ Function calling this method is saved to be used on value update ___
            $trace = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 5);
            foreach ($trace as $idx => $stack) {
                $func = $stack['function'];
                if ($func != __function__ &&
                    ($stack['class'] != __class__ || $func != 'get_monitor') &&
                    $func != 'o2job_get_bar') {
                    $bar->caller = $func;
                    break;
                    }
                }
            $bar->complete_action = $complete_action;
            $bar->start_action    = $start_action;
            }
        $bar->modificabile($this->progress == 100 || $run || $queued || $wait);
        $bar->set_value($this->progress, '');
        return $bar->get_code();

        }


    /**
     * Adds a file as production result for running run-job
     *
     * @param string $file_path   Path to file to be added
     */
    public function add_file($file_path) {

        $this->files[] = $file_path;

        }


    /**
     * Returns last job execution
     *
     * @param  integer $job_id
     * @param  string  $aspid
     * @param  string  $user
     * @param  integer $sched_id
     * @param  string  $host
     * @return o2_run_job
     */
    static function get_last_job($job_id,
                                 $aspid    = '',
                                 $user     = '',
                                 $sched_id = 0,
                                 $host     = '') {

        $app     = $_SESSION['o2_app'];
        $tab     = $app->get_table(self::$run_table);
        $db      = $tab->db;
        $server  = $db->server;
        $co      = constant('o2_'.$server->type.'_o');
        $cc      = constant('o2_'.$server->type.'_c');
        $f       = $tab->campi;
        $select  = $f['run_id'].' '.$co.'RUN_ID'.$cc;
        $aspfld  = $f['run_aspid']->nome_fisico;
        $where   = $f['job_id'].'='.$job_id.
                   ($aspid ? ' AND ('.$aspfld."='' OR ".
                                      $aspfld." IS NULL OR ".
                                      $aspfld."='NOASP' OR ".
                                      $aspfld."='".$aspid."')" :
                   '').
                   ($user ? ' AND '.$f['run_user']."='".$user."'" : '').
                   ($sched_id ? ' AND '.$f['run_sched_id']."=".$sched_id : '').
                   ($host ? ' AND '.$f['run_host']."='".$host."'" : '');
        $orderby = $f['run_start_date'].' DESC,'.$f['run_start_time'].' DESC';
        $run_job = o2_gateway::recordset($server->type,
                                         $server->server,
                                         $server->user,
                                         $server->password,
                                         $db->nome,
                                         $db->proprietario,
                                         $tab->nome,
                                         'jx_run_jobs',
                                         $select,
                                         $where,
                                         $orderby,
                                         1);
        if (count($run_job)) {
            return o2_run_job::get_by_id($run_job[0]['RUN_ID']);
            }
        else {
            return false;
            }

        }


    /**
     * Returns list of job executions
     *
     * @param  integer $job_id
     * @return array
     */
    static function get_by_job($job_id) {

        $app      = $_SESSION['o2_app'];
        $aspid    = $app->vars['_area']->valore;
        $tab      = $app->get_table(self::$run_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $f        = $tab->campi;
        $select   = $f['run_id'].' '.$co.'RUN_ID'.$cc;
        $aspfld   = $f['run_aspid']->nome_fisico;
        $where    = $f['job_id'].'='.$job_id.($aspid ?
                                              ' AND ('.$aspfld."='' OR ".
                                                       $aspfld." IS NULL OR ".
                                                       $aspfld."='".$aspid."' OR ".
                                                       $aspfld."='NOASP')" : '');
        $orderby  = $f['run_start_date'].' DESC,'.$f['run_start_time'].' DESC';
        $run_jobs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_run_jobs',
                                          $select,
                                          $where,
                                          $orderby,
                                          1000000);
        $list = array();
        if (count($run_jobs)) {
            foreach ($run_jobs as $run_rec) {
                $run_id        = $run_rec['RUN_ID'];
                $list[$run_id] = o2_run_job::get_by_id($run_id);
                }
            }
        return $list;

        }


    /**
     * Checks list of job executions.
     * This method is intended to be used to check queued list before calling ::get_list()
     * method, to avoid SELECT FOR UPDATE when nothing to do.
     *
     * @param  string  $user
     * @param  boolean $with_lock
     * @return array
     */
    static function check_list() {

        $app      = $_SESSION['o2_app'];
        $aspid    = $app->vars['_area']->valore;
        $tab      = $app->get_table(self::$run_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $f        = $tab->campi;
        $aspfld   = $f['run_aspid']->nome_fisico;
        // ______________________________________________________ Filter by HOST or... ___
        $where    = $f['run_status']."='Q' AND (".
                    $f['run_host']."='".$app->host."'";
        // ______________________________________________________ ...filter by SERVICE ___
        $services = array_keys(o2_service::get_all_host_services());
        if (count($services)) {
            $where.= " OR ".$f['run_service']." IN (";
            $comma = false;
            foreach ($services as $service) {
                $where.= ($comma ? ',' : '')."'".$service."'";
                $comma = true;
                }
            $where.= ')';
            }
        // ___________________________________________________________ Filter by ASPID ___
        $where   .= ')'.($aspid ? ' AND '.$aspfld."='".$aspid."'" : '');
        $run_jobs = o2_gateway::count($server->type,
                                      $server->server,
                                      $server->user,
                                      $server->password,
                                      $db->nome,
                                      $db->proprietario,
                                      $tab->nome,
                                      'jx_run_jobs',
                                      $where);
        return ($run_jobs ? true : false);

        }


    /**
     * Returns list of job executions
     *
     * @param  string  $user
     * @param  boolean $with_lock
     * @return array
     */
     static function get_list($user = "", $with_lock = false) {

        $app      = $_SESSION['o2_app'];
        $aspid    = $app->vars['_area']->valore;
        $tab      = $app->get_table(self::$run_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $f        = $tab->campi;
        $select   = $f['run_id'].' '.$co.'RUN_ID'.$cc.','.
                    $f['job_id'].' '.$co.'JOB_ID'.$cc.','.
                    $f['run_prg'].' '.$co.'RUN_PRG'.$cc.','.
                    $f['run_service'].' '.$co.'RUN_SERVICE'.$cc.','.
                    $f['run_host'].' '.$co.'RUN_HOST'.$cc.','.
                    $f['run_user'].' '.$co.'RUN_USER'.$cc.','.
                    $f['run_developer'].' '.$co.'RUN_DEVELOPER'.$cc.','.
                    $f['run_params'].' '.$co.'RUN_PARAMS'.$cc.','.
                    $f['run_aspid'].' '.$co.'RUN_ASPID'.$cc.','.
                    $f['run_status'].' '.$co.'RUN_STATUS'.$cc.','.
                    $f['run_pid'].' '.$co.'RUN_PID'.$cc.','.
                    $f['run_sessid'].' '.$co.'RUN_SESSID'.$cc.','.
                    $f['run_progress'].' '.$co.'RUN_PROGRESS'.$cc.','.
                    $f['run_error'].' '.$co.'RUN_ERROR'.$cc.','.
                    $f['run_files'].' '.$co.'RUN_FILES'.$cc.','.
                    $f['run_start_date'].' '.$co.'RUN_START_DATE'.$cc.','.
                    $f['run_start_time'].' '.$co.'RUN_START_TIME'.$cc.','.
                    $f['run_end_date'].' '.$co.'RUN_END_DATE'.$cc.','.
                    $f['run_end_time'].' '.$co.'RUN_END_TIME'.$cc.','.
                    $f['run_at_date'].' '.$co.'RUN_AT_DATE'.$cc.','.
                    $f['run_at_time'].' '.$co.'RUN_AT_TIME'.$cc;
        $aspfld   = $f['run_aspid']->nome_fisico;
        // ___ Recs from scheduler already have their own process, so can't be locked! ___
        $where    = ($user ? ' AND '.$f['run_user']." ='".$user."'".
                    ($aspid ? ' AND '.$aspfld."='".$aspid."'" : '') : '');
        $ord      = ($with_lock ? ' ASC' : ' DESC');
        $orderby  = $f['run_start_date'].$ord.','.$f['run_start_time'].$ord;
        $run_jobs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_run_jobs',
                                          $select,
                                          $where,
                                          $orderby,
                                          1000000,
                                          null,
                                          $with_lock);
        if (count($run_jobs)) {
            $list = array();
            foreach ($run_jobs as $run_rec) {
                $run_job            = new o2_run_job();
                $run_job->id        = $run_rec['RUN_ID'];
                $run_job->job       = $run_rec['JOB_ID'];
                $run_job->prg       = $run_rec['RUN_PRG'];
                $run_job->service   = $run_rec['RUN_SERVICE'];
                $run_job->host      = $run_rec['RUN_HOST'];
                $run_job->params    = unserialize($run_rec['RUN_PARAMS']);
                $run_job->user      = $run_rec['RUN_USER'];
                $run_job->developer = $run_rec['RUN_DEVELOPER'];
                $run_job->aspid     = $run_rec['RUN_ASPID'];
                $run_job->status    = $run_rec['RUN_STATUS'];
                $run_job->pid       = $run_rec['RUN_PID'];
                $run_job->sessid    = $run_rec['RUN_SESSID'];
                $run_job->progress  = $run_rec['RUN_PROGRESS'];
                $run_job->error     = $run_rec['RUN_ERROR'];
                $run_job->files     = unserialize($run_rec['RUN_FILES']);
                if ($run_rec['RUN_START_DATE']) {
                    $sd             = $run_rec['RUN_START_DATE'];
                    $st             = $run_rec['RUN_START_TIME'];
                    $start          = new DateTime($sd[0].$sd[1].$sd[2].$sd[3].'-'.
                                                   $sd[4].$sd[5].'-'.
                                                   $sd[6].$sd[7].' '.
                                                   $st[0].$st[1].':'.
                                                   $st[2].$st[3].':'.
                                                   $st[4].$st[5]);
                    $run_job->start = $start->getTimestamp();
                    unset($start);
                    }
                if ($run_rec['RUN_END_DATE']) {
                    $ed           = $run_rec['RUN_END_DATE'];
                    $et           = $run_rec['RUN_END_TIME'];
                    $end          = new DateTime($ed[0].$ed[1].$ed[2].$ed[3].'-'.
                                                 $ed[4].$ed[5].'-'.
                                                 $ed[6].$ed[7].' '.
                                                 $et[0].$et[1].':'.
                                                 $et[2].$et[3].':'.
                                                 $et[4].$et[5]);
                    $run_job->end = $end->getTimestamp();
                    unset($end);
                    }
                if ($run_rec['RUN_AT_DATE']) {
                    $ad           = $run_rec['RUN_AT_DATE'];
                    $at           = $run_rec['RUN_AT_TIME'];
                    $at_time      = new DateTime($ad[0].$ad[1].$ad[2].$ad[3].'-'.
                                                 $ad[4].$ad[5].'-'.
                                                 $ad[6].$ad[7].' '.
                                                 $at[0].$at[1].':'.
                                                 $at[2].$at[3].':'.
                                                 $at[4].$at[5]);
                    $run_job->exe_time = $at_time->getTimestamp();
                    unset($at_time);
                    }
                $list[$run_job->id] = $run_job;
                }
            return $list;
            }
        else {
            return false;
            }

        }


    /**
     * Delete a job instances, according with job "keep days" parameter
     *
     * @param integer $job_id
     */
    static function clear_by_job($job_id) {

        $job       = o2_job::get_by_id($job_id);
        $keep_days = $job->keep_days;
        if ($keep_days) {
            $app    = $_SESSION['o2_app'];
            $tab    = $app->get_table(self::$run_table);
            $db     = $tab->db;
            $server = $db->server;
            $co     = constant('o2_'.$server->type.'_o');
            $cc     = constant('o2_'.$server->type.'_c');
            $start  = new DateTime;
            $start->modify('-'.$keep_days.' days');
            $s_date = $start->format('Ymd');
            $s_time = $start->format('His');
            $f      = $tab->campi;
            $select = $f['run_id'].' '.$co.'RUN_ID'.$cc.','.
                      $f['run_sessid'].' '.$co.'RUN_SESSID'.$cc.','.
                      $f['run_host'].' '.$co.'RUN_HOST'.$cc;
            $where  = $f['job_id'].'='.$job_id.' AND ('.
                      $f['run_start_date']."<'".$s_date."' OR (".
                      $f['run_start_date']."='".$s_date."' AND ".
                      $f['run_start_time']."<'".$s_time."'))";
            // _________________________ Get all obsolete run instances for passed job ___
            $recs   = o2_gateway::recordset($server->type,
                                            $server->server,
                                            $server->user,
                                            $server->password,
                                            $db->nome,
                                            $db->proprietario,
                                            $tab->nome,
                                            'jx_run_jobs',
                                            $select,
                                            $where,
                                            '',
                                            1000);
            $host    = $app->host;
            $stab    = $app->get_table('o2_sessions');
            $sdb     = $stab->db;
            $sserver = $sdb->server;
            $sf      = $stab->campi;
            $now     = time();
            $sets    = array($sf['e_date']->nome_fisico => "'".date('Ymd', $now)."'",
                             $sf['e_time']->nome_fisico => "'".date('His', $now)."'");
            $d_sess  = array();
            foreach ($recs as $rec) {
                // _____________________________________________ Delete session record ___
                o2_gateway::deleterec($sserver->type,
                                      $sserver->server,
                                      $sserver->user,
                                      $sserver->password,
                                      $sdb->nome,
                                      $sdb->proprietario,
                                      $stab->nome,
                                      'o2_sessions',
                                      $sf['sid']."='".$rec['RUN_SESSID']."'");
                // _______________________________________________ Delete run instance ___
                o2_gateway::deleterec($server->type,
                                      $server->server,
                                      $server->user,
                                      $server->password,
                                      $db->nome,
                                      $db->proprietario,
                                      $tab->nome,
                                      'jx_run_jobs',
                                      $f['run_id']."='".$rec['RUN_ID']."'");
                }

            }

        }


    /**
     * Clear executed instances for all jobs, according with each "keep_days" parameter
     *
     */
    static function clear_all() {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(o2_job::$jobs_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $select   = $tab->campi['job_id'].' '.$co.'JOB_ID'.$cc;
        $job_recs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_jobs',
                                          $select,
                                          '',
                                          '',
                                          1000000);
        foreach ($job_recs as $job_rec) {
            self::clear_by_job($job_rec['JOB_ID']);
            }

        }


    /**
     * Returns HTML code to monitor running jobs
     * $complete_action is the local program action to be executed when a process goes to
     *                  completion
     * $start_action    is the local program action to be executed when a process starts
     *                  (changing from 0 to >0)
     * $buttons_action  is the local program action to be executed when a monitor button
     *                  is pressed. Req-params passed to action are:
     *                   1. show|attach:
     *                       -show:     Show process details icon
     *                       -attach:   Get attached files
     *                   2. process-id
     * If $job_name and/or $status parameters are valued then they are used to filter the
     * request.
     *
     * @param  string  $user
     * @param  string  $complete_action   Action to be executed on progress completion
     * @param  string  $start_action      Action to be executed on progress start (>0)
     * @param  string  $buttons_action    Action to be executed on button press
     * @param  string  $job_name          Job name to filter request on
     * @param  string  $status            Process status to filter request on
     * @param  integer $last_days         Filter run-jobs for last N days (0 = always)
     * @param  string  $asp_id            Filter run-jobs for execution ASP-Id
     * @return string
     */
    static function get_monitor($user            = '',
                                $complete_action = '',
                                $start_action    = '',
                                $buttons_action  = '',
                                $job_name        = '',
                                $status          = '',
                                $last_days       = 0,
                                $asp_id          = '') {

        $app       = $_SESSION['o2_app'];
        $rnt       = $GLOBALS['o2_runtime'];
        $aspid     = $app->vars['_area']->valore;
        $tab       = $app->get_table(self::$run_table);
        $db        = $tab->db;
        $server    = $db->server;
        $jtab      = $app->get_table(o2_job::$jobs_table);
        $co        = constant('o2_'.$server->type.'_o');
        $cc        = constant('o2_'.$server->type.'_c');
        $link      = array();
        $f         = $tab->campi;
        $jf        = $jtab->campi;
        $select    = $f['run_id'].' '.$co.'RUN_ID'.$cc.','.
                     $co.'jx_run_jobs'.$cc.'.'.$f['job_id'].' '.$co.'JOB_ID'.$cc.','.
                     $f['run_prg'].' '.$co.'RUN_PRG'.$cc.','.
                     $f['run_service'].' '.$co.'RUN_SERVICE'.$cc.','.
                     $f['run_host'].' '.$co.'RUN_HOST'.$cc.','.
                     $f['run_user'].' '.$co.'RUN_USER'.$cc.','.
                     $f['run_developer'].' '.$co.'RUN_DEVELOPER'.$cc.','.
                     $f['run_params'].' '.$co.'RUN_PARAMS'.$cc.','.
                     $f['run_aspid'].' '.$co.'RUN_ASPID'.$cc.','.
                     $f['run_status'].' '.$co.'RUN_STATUS'.$cc.','.
                     $f['run_pid'].' '.$co.'RUN_PID'.$cc.','.
                     $f['run_progress'].' '.$co.'RUN_PROGRESS'.$cc.','.
                     $f['run_error'].' '.$co.'RUN_ERROR'.$cc.','.
                     $f['run_files'].' '.$co.'RUN_FILES'.$cc.','.
                     $f['run_start_date'].' '.$co.'RUN_START_DATE'.$cc.','.
                     $f['run_start_time'].' '.$co.'RUN_START_TIME'.$cc.','.
                     $f['run_end_date'].' '.$co.'RUN_END_DATE'.$cc.','.
                     $f['run_end_time'].' '.$co.'RUN_END_TIME'.$cc.','.
                     $f['run_at_date'].' '.$co.'RUN_AT_DATE'.$cc.','.
                     $f['run_at_time'].' '.$co.'RUN_AT_TIME'.$cc.','.
                     $jf['job_name'].' '.$co.'JOB_NAME'.$cc.','.
                     $jf['job_prg'].' '.$co.'JOB_PRG'.$cc.','.
                     $jf['job_service'].' '.$co.'JOB_SERVICE'.$cc.','.
                     $jf['job_params'].' '.$co.'JOB_PARAMS'.$cc.','.
                     $jf['job_keep_days'].' '.$co.'JOB_KEEP_DAYS'.$cc.','.
                     $jf['job_max_instances'].' '.$co.'JOB_MAX_INSTANCES'.$cc.','.
                     $jf['job_block_mode'].' '.$co.'JOB_BLOCK_MODE'.$cc.','.
                     $jf['job_disabled'].' '.$co.'JOB_DISABLED'.$cc.','.
                     $jf['job_desc'].' '.$co.'JOB_DESC'.$cc;
        $jl        = o2_gateway::qualify($server->type,
                                         $db->nome,
                                         $db->proprietario,
                                         $jtab->nome).' '.$co.$jtab->nome.$cc;
        $link[$jl] = $co.'jx_run_jobs'.$cc.'.'.$f['job_id'].'='.
                                     $co.$jtab->nome.$cc.'.'.$jf['job_id'];
        $where     = ($user ?
                      $f['run_user']." ='".$user."'".
                      ($aspid ? ' AND '.$f['run_aspid']."='".$aspid."'" : '') :
                      ($asp_id ? $f['run_aspid']."='".$asp_id."'" : ''));
        $orderby   = $f['run_start_date'].' DESC,'.$f['run_start_time'].' DESC';
        $run_jobs  = o2_gateway::recordset($server->type,
                                           $server->server,
                                           $server->user,
                                           $server->password,
                                           $db->nome,
                                           $db->proprietario,
                                           $tab->nome,
                                           'jx_run_jobs',
                                           $select,
                                           $where,
                                           $orderby,
                                           1000000,
                                           $link);
        $img       = $rnt->alias.'img/jobs/';
        $code      = '<table style="width:100%">';
        $curd      = '';
        $label     = '<tr><td colspan="6" style="color:#666666;padding-top:5px;'.
                     'border-bottom:1px solid #bbbbbb;">';
        $sp        = '&nbsp;&nbsp;';
        if (count($run_jobs)) {
            $last_date = false;
            foreach ($run_jobs as $r) {
                // ____________________________________________________ Run-job object ___
                $run_job            = new o2_run_job();
                $run_job->id        = $r['RUN_ID'];
                $run_job->job       = $r['JOB_ID'];
                $run_job->prg       = $r['RUN_PRG'];
                $run_job->service   = $r['RUN_SERVICE'];
                $run_job->host      = $r['RUN_HOST'];
                $run_job->params    = unserialize($r['RUN_PARAMS']);
                $run_job->user      = $r['RUN_USER'];
                $run_job->developer = $r['RUN_DEVELOPER'];
                $run_job->aspid     = $r['RUN_ASPID'];
                $run_job->status    = $r['RUN_STATUS'];
                $run_job->pid       = $r['RUN_PID'];
                $run_job->progress  = $r['RUN_PROGRESS'];
                $run_job->error     = $r['RUN_ERROR'];
                $run_job->files     = unserialize($r['RUN_FILES']);
                if ($r['RUN_START_DATE']) {
                    $sd             = $r['RUN_START_DATE'];
                    $st             = $r['RUN_START_TIME'];
                    $start          = new DateTime($sd[0].$sd[1].$sd[2].$sd[3].'-'.
                                                   $sd[4].$sd[5].'-'.
                                                   $sd[6].$sd[7].' '.
                                                   $st[0].$st[1].':'.
                                                   $st[2].$st[3].':'.
                                                   $st[4].$st[5]);
                    $run_job->start = $start->getTimestamp();
                    unset($start);
                    }
                if ($r['RUN_END_DATE']) {
                    $ed           = $r['RUN_END_DATE'];
                    $et           = $r['RUN_END_TIME'];
                    $end          = new DateTime($ed[0].$ed[1].$ed[2].$ed[3].'-'.
                                                 $ed[4].$ed[5].'-'.
                                                 $ed[6].$ed[7].' '.
                                                 $et[0].$et[1].':'.
                                                 $et[2].$et[3].':'.
                                                 $et[4].$et[5]);
                    $run_job->end = $end->getTimestamp();
                    unset($end);
                    }
                if ($r['RUN_AT_DATE']) {
                    $ad           = $r['RUN_AT_DATE'];
                    $at           = $r['RUN_AT_TIME'];
                    $at_time      = new DateTime($ad[0].$ad[1].$ad[2].$ad[3].'-'.
                                                 $ad[4].$ad[5].'-'.
                                                 $ad[6].$ad[7].' '.
                                                 $at[0].$at[1].':'.
                                                 $at[2].$at[3].':'.
                                                 $at[4].$at[5]);
                    $run_job->exe_time = $at_time->getTimestamp();
                    unset($at_time);
                    }
                // ________________________________________________ Related job object ___
                $job = new o2_job($r['JOB_ID'],
                                  $r['JOB_NAME'],
                                  $r['JOB_PRG'],
                                  unserialize($r['JOB_PARAMS']),
                                  $r['JOB_DESC'],
                                  $r['JOB_KEEP_DAYS'],
                                  $r['JOB_MAX_INSTANCES'],
                                  $r['JOB_BLOCK_MODE'],
                                  $r['JOB_DISABLED'],
                                  $r['JOB_SERVICE']);
                // _______________________________________ Filter by job name & status ___
                if ((!$job_name || $job->name == $job_name) &&
                    (!$status || (substr($run_job->get_status(), 0, 1) ==
                                  strtoupper(substr($status, 0, 1))))) {
                    $curd = date('Ymd', $run_job->start);
                    // ______________________________________ Filter by number of days ___
                    if ($last_days &&
                        ($curd < date('Ymd',strtotime('-'.$last_days.' days')))) {
                        continue;
                        }
                    elseif ($last_date != $curd) {
                        if ($curd == date('Ymd')) {
                            $code.= $label.'Today</td></tr>';
                            }
                        elseif ($curd == date('Ymd',strtotime('-1 days'))) {
                            $code.= $label.'Yesterday</td></tr>';
                            }
                        else {
                            $code.= $label.date('d/m/Y', $run_job->start).'</td></tr>';
                            }
                        $last_date = $curd;
                        }
                    $run = $run_job->is_running();
                    $ico = $img;
                    if ($run_job->error) {
                        $ico.= 'failed.png';
                        }
                    elseif ($run_job->status == "Q") {
                        $ico.= 'scheduled.png';
                        }
                    elseif ($run_job->status == "S" &&
                            ((time() - $run_job->start) <= 3)) {
                        $ico.= 'scheduled.png';
                        }
                    elseif ($run_job->progress < 100) {
                        if ($run) {
                            $ico.= 'running.png';
                            }
                        else {
                            $ico.= 'failed.png';
                            }
                        }
                    else {
                        $ico.= 'executed.png';
                        }
                    $bar    = $run_job->get_bar($complete_action, $start_action);
                    $desc   = '<td style="white-space:nowrap">'.$job->name.
                              '</td><td style="white-space:nowrap">'.$sp.
                              date("H:i", $run_job->start).
                              '</td><td style="white-space:nowrap">'.$sp.
                              ($run_job->end > 0 ? date("H:i", $run_job->end) : "---").
                              $sp.'</td>';
                    $tr     = '<tr title="Module '.
                              $job->prg.(count($job->params) ? " with parameters" : "").
                              '">';
                    $attach = $buttons_action &&
                              is_array($run_job->files) && (count($run_job->files) > 0);
                    $code  .= $tr.'<td><img src="'.$ico.
                              ($buttons_action ?
                               '" class="jximgbtn" onclick="'.
                               o2_act4js(0, $buttons_action, "show", $run_job->id) : "").
                              '"></td>'.$desc.
                              '<td style="width:100%;height:18px">'.$bar.
                              '</td><td><img class="'.
                              ($attach ? "jximgbtn" : "").'" '.
                              ($attach ?
                               'onclick="'.o2_act4js(0,
                                                     $buttons_action,
                                                     "attach",
                                                     $run_job->id).'"' :
                               "").
                              ' src="'.$img.($attach ? "attach.png" : "attach_dis.png").
                              '"></td></tr>';
                    }
                }
            }
        $code.= '</table>';
        return $code;
        }

    }


/**
 * Services are used to group jobs and executions.
 * Jobs are queued on a service. When a network machine (host) is running for a service it
 * will execute all pending (queued) run instances linked to that service.
 * Services are defined in application INI file. Hosts are linked to services in system
 * table "jx_services".
 * This class is a container for methods related to services management.
 *
 */
class o2_service {

    static $srv_table   = 'jx_services'; /* System table for services/host matching     */
    static $hosts_table = 'jx_hosts';    /* System table for host PIDs list             */


    /**
     * Returns a list of services with assigned hosts
     *
     * @return array
     */
    static function get_list() {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$srv_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $f        = $tab->campi;
        $select   = $f['srv_service'].' '.$co.'SRV_SERVICE'.$cc.','.
                    $f['srv_host'].' '.$co.'SRV_HOST'.$cc.','.
                    $f['srv_start_date'].' '.$co.'SRV_START_DATE'.$cc.','.
                    $f['srv_start_time'].' '.$co.'SRV_START_TIME'.$cc.','.
                    $f['srv_last_date'].' '.$co.'SRV_LAST_DATE'.$cc.','.
                    $f['srv_last_time'].' '.$co.'SRV_LAST_TIME'.$cc;
        $srv_recs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_services',
                                          $select,
                                          '',
                                          '',
                                          1000000);
        $list     = array();
        foreach ($srv_recs as $srv_rec) {
            $host_id        = $srv_rec['SRV_HOST'].'h'.$srv_rec['SRV_SERVICE'].'s';
            $last_time      = $srv_rec['SRV_LAST_DATE'].$srv_rec['SRV_LAST_TIME'];
            $list[]         = array('host'   => $srv_rec['SRV_HOST'],
                                    'srv'    => $srv_rec['SRV_SERVICE'],
                                    's_date' => $srv_rec['SRV_START_DATE'],
                                    's_time' => $srv_rec['SRV_START_TIME'],
                                    'l_date' => $srv_rec['SRV_LAST_DATE'],
                                    'l_time' => $srv_rec['SRV_LAST_TIME']);
            }
        return $list;

        }


    /**
     * Returns a list of registered (active) hosts
     *
     * @return array
     */
    static function get_hosts() {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$srv_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $f        = $tab->campi;
        $select   = 'DISTINCT '.$f['srv_host'].' '.$co.'SRV_HOST'.$cc;
        $srv_recs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_services',
                                          $select,
                                          '',
                                          '',
                                          1000);
        $list     = array();
        foreach ($srv_recs as $srv_rec) {
            $list[] = $srv_rec['SRV_HOST'];
            }
        return $list;

        }


    /**
     * Returns a list of matching services and hosts ready to be used as a TreeView
     * control items
     *
     * @return array
     */
    static function get_for_tree($root = 0) {

        $app      = $_SESSION['o2_app'];
        $services = self::get_list();
        $img_path = '<jx>/img/jobs/tree/';
        $list     = array('_srv' => array($root,
                                          'Default',
                                          $img_path.'service.png',
                                          'srv',
                                          ''));
        foreach ($app->services as $service) {
            $srv_id = $service.'_srv';
            $list[$srv_id] = array($root,
                                   o2_str_nice($service),
                                   $img_path.'service.png',
                                   'srv',
                                   $service,
                                   '');
            }
        $now = (new DateTime('-3 minutes'))->format('YmdHis');
        foreach ($services as $srv) {
            $host_id        = $srv['host'].'h'.$srv['srv'].'s';
            $last_time      = $srv['l_date'].$srv['l_time'];
            $list[$host_id] = array($srv['srv'].'_srv',
                                    o2_str_nice($srv['host']),
                                    $img_path.($last_time < $now ?
                                               'host_dis.png' : 'host.png'),
                                    'host',
                                    $srv['srv'],
                                    $srv['host']);
            }
        return $list;

        }


    /**
     * Returns a list of services assigned to host
     *
     * @param  string $host
     * @return array
     */
    static function get_host_services($host) {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$srv_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $select   = $tab->campi['srv_service']->nome_fisico.' '.$co.'SRV_SERVICE'.$cc;
        $where    = $tab->campi['srv_host']->nome_fisico."='".$host."'";
        $srv_recs = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_services',
                                          $select,
                                          $where,
                                          '',
                                          1000000);
        $list     = array();
        foreach ($srv_recs as $srv_rec) {
            $srv        = $srv_rec['SRV_SERVICE'];
            $list[$srv] = ($srv ? o2_str_nice($srv) : 'Deafult');
            }
        return $list;

        }


    /**
     * Returns list of services enabled for local host.
     * Host can match a service in two ways:
     *  1. host is registered for service;
     *  2. host is registered as Default and service has no other registered hosts.
     *
     * @param  string $host
     * @return boolean
     */
    static function get_all_host_services() {

        $app       = $_SESSION['o2_app'];
        $host_srvs = o2_service::get_host_services($app->host);
        // ___________________________ Check if host is registered for Default service ___
        $default   = false;
        if (isset($host_srvs[''])) {
            $default = true;
            }
        // _____________________ Check services to run as Default (no host registered) ___
        $srvs2default = array();
        if ($default) {
            foreach ($app->services as $srv) {
                $srvs2default[$srv] = o2_str_nice($srv);
                }
            foreach (o2_service::get_list() as $srv) {
                if (isset($srvs2default[$srv['srv']])) {
                    unset($srvs2default[$srv['srv']]);
                    }
                }
            }
        // _____________________________________ Combine assigned and default services ___
        return ($host_srvs + $srvs2default);

        }


    /**
     * Removes a registered host from a service
     *
     * @param  string $host
     * @param  string $service
     * @return boolean
     */
    static function remove_host_service($host, $service) {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$srv_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $select   = $tab->campi['srv_host']->nome_fisico.' '.$co.'SRV_HOST'.$cc;
        $where    = $tab->campi['srv_service']->nome_fisico."='".$service."' AND ".
                    $tab->campi['srv_host']->nome_fisico."='".$host."'";
        $srv_recs = o2_gateway::verifyrec($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_services',
                                          $select,
                                          $where,
                                          '');
        if (count($srv_recs)) {
            o2_gateway::deleterec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  'jx_services',
                                  $where);
            }
        return true;

        }


    /**
     * Register host to a service
     *
     * @param  string $host
     * @param  string $service
     * @return boolean
     */
    static function add_host_service($host, $service) {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$srv_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $select   = $tab->campi['srv_host']->nome_fisico.' '.$co.'SRV_HOST'.$cc;
        $where    = $tab->campi['srv_service']->nome_fisico."='".$service."' AND ".
                    $tab->campi['srv_host']->nome_fisico."='".$host."'";
        $srv_recs = o2_gateway::verifyrec($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_services',
                                          $select,
                                          $where,
                                          '');
        if (!$srv_recs) {
            $fields = array($tab->campi["srv_host"]->nome_fisico,
                            $tab->campi["srv_service"]->nome_fisico,
                            $tab->campi['srv_start_date']->nome_fisico,
                            $tab->campi['srv_start_time']->nome_fisico,
                            $tab->campi['srv_last_date']->nome_fisico,
                            $tab->campi['srv_last_time']->nome_fisico);
            $now = time();
            $values = array("'".$host."'",
                            "'".$service."'",
                            "'".date('Ymd', $now)."'",
                            "'".date('His', $now)."'",
                            "'".date('Ymd', $now)."'",
                            "'".date('His', $now)."'");
            o2_gateway::insertrec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  "jx_services",
                                  $fields,
                                  $values);
            }
        return true;

        }


    /**
     * Updates host-service record (for local host) with last execution date and time
     *
     * @param  string $service
     * @return boolean
     */
    static function update_host_service($service) {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$srv_table);
        $db       = $tab->db;
        $server   = $db->server;
        $where    = $tab->campi['srv_service']->nome_fisico."='".$service."' AND ".
                    $tab->campi['srv_host']->nome_fisico."='".$app->host."'";
        $srv_recs = o2_gateway::verifyrec($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_services',
                                          '*',
                                          $where,
                                          '');
        if ($srv_recs) {
            $now  = time();
            $sets = array($tab->campi['srv_last_date']->nome_fisico =>
                                                                "'".date('Ymd', $now)."'",
                          $tab->campi['srv_last_time']->nome_fisico =>
                                                               "'".date('His', $now)."'");
            $res  = o2_gateway::modifyrec($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          "jx_services",
                                          $sets,
                                          $where);
            }
        return true;

        }


    /**
     * Updates host PIDs list, to check against processes PID in the network.
     *
     * @return boolean
     */
    static function update_host_pids() {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$hosts_table);
        $db       = $tab->db;
        $server   = $db->server;
        $where    = $tab->campi['host_host']->nome_fisico."='".$app->host."'";
        $host_rec = o2_gateway::verifyrec($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_hosts',
                                          '*',
                                          $where,
                                          '');
        $pids = serialize($app->runtime->proc_list(true));
        if ($host_rec) {
            $set = array($tab->campi['host_pids']->nome_fisico => "'".$pids."'");
            $res = o2_gateway::modifyrec($server->type,
                                         $server->server,
                                         $server->user,
                                         $server->password,
                                         $db->nome,
                                         $db->proprietario,
                                         $tab->nome,
                                         "jx_hosts",
                                         $set,
                                         $where);
            }
        else {
            $fields = array($tab->campi["host_host"]->nome_fisico,
                            $tab->campi["host_pids"]->nome_fisico);
            $values = array("'".$app->host."'", "'".$pids."'");
            o2_gateway::insertrec($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  "jx_hosts",
                                  $fields,
                                  $values);
            }
        return true;

        }


    /**
     * Returns PIDs list for the requested host
     *
     * @param  string $host
     * @return array
     */
    static function get_host_pids($host) {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$hosts_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant('o2_'.$server->type.'_o');
        $cc       = constant('o2_'.$server->type.'_c');
        $select   = $tab->campi['host_pids']->nome_fisico.' '.$co.'HOST_PIDS'.$cc;
        $where    = $tab->campi['host_host']->nome_fisico."='".$host."'";
        $pids_rec = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_hosts',
                                          $select,
                                          $where,
                                          '',
                                          1);
        $list     = array();
        if ($pids_rec[0]) {
            $list = unserialize($pids_rec[0]['HOST_PIDS']);
            }
        return $list;

        }


    /**
     * Register local host to a network system.
     * When a host is first registered it is assigned to Default (blank) service at least.
     * Host updates its own PIDs list too, to check against processes PID in the network.
     *
     * @return boolean
     */
    static function register_host() {

        $host = $_SESSION['o2_app']->host;
        $srvs = self::get_host_services($host);
        // ______________ Host has not assigned services: register for Default service ___
        if (!count($srvs)) {
            self::add_host_service($host, '');
            }
        // ______________________ Update local host for registered services activation ___
        else {
            foreach ($srvs as $srv => $srv_nice) {
                self::update_host_service($srv);
                }
            }
        // _______________________________________________ Update local host PIDs list ___
        self::update_host_pids();

        }

    }


/**
 * A task is a scheduled job with scheduling parameters.
 * Tasks are managed by Janox Scheduler to provide planned executions.
 *
 */
class o2_task {

    public $id          = 0;
    public $job         = 0;
    public $user        = "";
    public $type        = "";
    public $interval    = 0;
    public $date        = "";
    public $time        = "";
    public $recurrence  = "";
    public $week_day    = "";
    public $month_day   = 0;
    public $description = "";
    public $aspid       = false;
    public $all_hosts   = false;
    public $paused      = false;


    public function __construct($id         = 0,
                                $job        = 0,
                                $user       = "",
                                $type       = "",
                                $interval   = 0,
                                $date       = "",
                                $time       = "",
                                $recurrence = "",
                                $week_day   = "",
                                $month_day  = 0,
                                $aspid      = false,
                                $paused     = false,
                                $all_hosts  = false) {

        $this->id         = intval($id);
        $this->job        = intval($job);
        $this->user       = $user;
        $this->type       = $type;
        $this->interval   = intval($interval);
        $this->date       = $date;
        $this->time       = $time;
        $this->recurrence = $recurrence;
        $this->week_day   = $week_day;
        $this->month_day  = intval($month_day);
        $this->aspid      = $aspid;
        $this->all_hosts  = $all_hosts;
        $this->paused     = $paused;

        }


    /**
     * Creates a task on db data.
     * If task ID is zero then next free ID is assigned to task.
     *
     */
    public function create() {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(o2_scheduler::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        // _______________________________________________ Check for one existing task ___
        $exists = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_sched",
                                        "*",
                                        "",
                                        "",
                                        1);
        $fields = array($tab->campi["sched_id"]->nome_fisico,
                        $tab->campi["sched_job"]->nome_fisico,
                        $tab->campi["sched_user"]->nome_fisico,
                        $tab->campi["sched_type"]->nome_fisico,
                        $tab->campi["sched_interval"]->nome_fisico,
                        $tab->campi["sched_date"]->nome_fisico,
                        $tab->campi["sched_time"]->nome_fisico,
                        $tab->campi["sched_recurrence"]->nome_fisico,
                        $tab->campi["sched_week_day"]->nome_fisico,
                        $tab->campi["sched_month_day"]->nome_fisico,
                        $tab->campi["sched_aspid"]->nome_fisico,
                        $tab->campi["sched_all_hosts"]->nome_fisico,
                        $tab->campi["sched_paused"]->nome_fisico);
        $id_val = ($this->id ? $this->id :
                   (count($exists) ?
                    "(SELECT MAX(".$tab->campi["sched_id"]->nome_fisico.")+1 FROM ".
                    o2tab_qname(o2_scheduler::$sched_table).")" :
                    "1"));
        $values = array($id_val,
                        $this->job,
                        "'".$this->user."'",
                        "'".$this->type."'",
                        $this->interval,
                        "'".$this->date."'",
                        "'".$this->time."'",
                        "'".$this->recurrence."'",
                        "'".$this->week_day."'",
                        $this->month_day,
                        "'".$this->aspid."'",
                        "'".($this->all_hosts ? '1' : '0')."'",
                        "'".($this->paused ? '1' : '0')."'");
        o2_gateway::insertrec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              "jx_sched",
                              $fields,
                              $values);
        // _____________________________________________________ Return new created ID ___
        $select = " MAX(".$tab->campi["sched_id"]->nome_fisico.") AS NEW_ID";
        $new_id = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_sched",
                                        $select,
                                        "",
                                        "",
                                        1);
        return $new_id[0]["NEW_ID"];

        }


    /**
     * Saves existing task on db data
     *
     */
    public function save() {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(o2_scheduler::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $sets   = array(
                $tab->campi["sched_job"]->nome_fisico        => $this->job,
                $tab->campi["sched_user"]->nome_fisico       => "'".$this->user."'",
                $tab->campi["sched_type"]->nome_fisico       => "'".$this->type."'",
                $tab->campi["sched_interval"]->nome_fisico   => $this->interval,
                $tab->campi["sched_date"]->nome_fisico       => "'".$this->date."'",
                $tab->campi["sched_time"]->nome_fisico       => "'".$this->time."'",
                $tab->campi["sched_recurrence"]->nome_fisico => "'".$this->recurrence."'",
                $tab->campi["sched_week_day"]->nome_fisico   => "'".$this->week_day."'",
                $tab->campi["sched_month_day"]->nome_fisico  => $this->month_day,
                $tab->campi["sched_aspid"]->nome_fisico      => "'".$this->aspid."'",
                $tab->campi["sched_all_hosts"]->nome_fisico  => "'".($this->all_hosts ?
                                                                     '1' : '0')."'",
                $tab->campi["sched_paused"]->nome_fisico     => "'".($this->paused ?
                                                                     '1' : '0')."'"
                );
        $where  = $tab->campi["sched_id"]->nome_fisico."=".$this->id;
        $res    = o2_gateway::modifyrec($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_sched",
                                        $sets,
                                        $where);

        }


    /**
     * Deletes existing task on db data
     *
     */
    public function delete() {

        $app    = $_SESSION['o2_app'];
        // _______________________________________________________________ Delete task ___
        $tab    = $app->get_table(o2_scheduler::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $where  = $tab->campi["sched_id"]->nome_fisico."=".$this->id;
        o2_gateway::deleterec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              "jx_scheduler",
                              $where);

        }


    /**
     * Sets and returns task scheduling description.
     *
     * @return string
     */
    public function get_description() {

        $sd    = $this->date;
        $st    = $this->time;
        $sched = new DateTime($sd[0].$sd[1].$sd[2].$sd[3]."-".
                              $sd[4].$sd[5]."-".
                              $sd[6].$sd[7]." ".
                              $st[0].$st[1].":".
                              $st[2].$st[3].":".
                              $st[4].$st[5]);
        $date  = $sched->format("d/m/Y");
        $time  = $sched->format("H:i");
        $recur = "";
        $txt   = "";
        switch ($this->recurrence) {
            case "D":
                $recur = "Every day at ".$time;
                break;
            case "W":
                $recur = "Every ".$this->week_day." at ".$time;
                break;
            case "M":
                if ($this->month_day == 11 ||
                    $this->month_day == 12 ||
                    $this->month_day == 13) {
                    $th = "th";
                    }
                else {
                    switch (substr(trim($this->month_day), -1)) {
                        case "1":
                            $th = "st";
                            break;
                        case "2":
                            $th = "nd";
                            break;
                        case "3":
                            $th = "rd";
                            break;
                        default:
                            $th = "th";
                            break;
                        }
                    }
                if ($this->month_day) {
                    $recur = "Day ".$this->month_day.$th." of every month at ".$time;
                    }
                else {
                    $recur = "...";
                    }
                break;
            }
        switch ($this->type) {
            case "O":
                if ($sched->format("Y") > 0) {
                    $txt = "On ".$date." at ".$time;
                    }
                else {
                    $txt = "...";
                    }
                break;
            case "I":
                if ($this->interval) {
                    $txt = "Every ".$this->interval." minutes";
                    }
                else {
                    $txt = "...";
                    }
                break;
            case "R":
                $txt = $recur;
                break;
            }
        $this->description = $txt;
        return $txt;

        }


    /**
     * Returns next scheduled task execution.
     * Return value can be:
     *  FALSE      task will never run
     *  0 (zero)   task is waiting to run
     *  DateTime   next run date-time object
     *
     * @return DateTime
     */
    public function get_next_run() {

        $next     = false;
        $now      = new DateTime();
        $sd       = $this->date;
        $st       = $this->time;
        $sched    = new DateTime($sd[0].$sd[1].$sd[2].$sd[3]."-".
                                 $sd[4].$sd[5]."-".
                                 $sd[6].$sd[7]." ".
                                 $st[0].$st[1].":".
                                 $st[2].$st[3].":".
                                 $st[4].$st[5]);
        $last_run = o2_run_job::get_last_job($this->job, $this->aspid, '', $this->id);
        $last     = new DateTime();
        $last->setTimestamp($last_run ? $last_run->start : 0);
        switch ($this->type) {
            // ______________________________________________________________ Run once ___
            case "O":
                // _________________________________________ Sched date is in the past ___
                if ($now->format("Ymd") > $sched->format("Ymd")) {
                    $next = false;
                    }
                // ________________________________________________ Sched day is today ___
                elseif ($now->format("Ymd") == $sched->format("Ymd")) {
                    // _____________________________________________ Already run today ___
                    if ($now->format("Ymd") == $last->format("Ymd") &&
                        $last->format("His") >= $sched->format("His")) {
                        $next = false;
                        }
                    // _______________________________________ Waiting to be run today ___
                    elseif ($now->format("His") >= $sched->format("His")) {
                        $next = 0;
                        }
                    // _________________________________________ To be run later today ___
                    else {
                        $next = clone $sched; // ________________________ Today at ... ___
                        }
                    }
                // ____________________________________________ Sched is in the future ___
                else {
                    $next = clone $sched; // ________________________ Sched day at ... ___
                    }
                break;
            // _______________________________________________________ Run by interval ___
            case "I":
                // ___________________________________________ Already run in the past ___
                if ($last) {
                    // ______________________________________ Add interval to last run ___
                    $last->modify("+".intval($this->interval)." min");
                    // ____________________________________ Sched interval has elapsed ___
                    if ($now->getTimestamp() > $last->getTimestamp()) {
                        $next = 0;
                        }
                    // _______________________ Last run is earlier than sched interval ___
                    else {
                        $next = clone $last;
                        }
                    }
                // _____________________________________ Never run before, ready to go ___
                else {
                    $next = 0;
                    }
                break;
            // _____________________________________________________ Run by recurrence ___
            case "R":
                switch ($this->recurrence) {
                    // ______________________________________________ Recurrence daily ___
                    case "D":
                        // _________________________________________ Already run today ___
                        if ($last->format("Ymd") == $now->format("Ymd")) {
                            // ________ Run at or after sched time: run again tomorrow ___
                            if ($last->format("His") >= $sched->format("His")) {
                                $next = new DateTime("tomorrow");
                                $next->setTime($sched->format("H"),
                                               $sched->format("i"),
                                               $sched->format("s"));
                                }
                            // ________________________________ Sched time has elapsed ___
                            elseif ($now->format("His") >= $sched->format("His")) {
                                $next = 0;
                                }
                            // _________________________________ To be run later today ___
                            else {
                                $next = clone $now;
                                $next->setTime($sched->format("H"),
                                               $sched->format("i"),
                                               $sched->format("s"));
                                }
                            }
                        // ___________________________________________ To be run today ___
                        else {
                            // ________________________________ Sched time has elapsed ___
                            if ($now->format("His") >= $sched->format("His")) {
                                $next = 0;
                                }
                            // _________________________________ To be run later today ___
                            else {
                                $next = clone $now;
                                $next->setTime($sched->format("H"),
                                               $sched->format("i"),
                                               $sched->format("s"));
                                }
                            }
                        break;
                    // _____________________________________________ Recurrence weekly ___
                    case "W":
                        $wday = $this->week_day;
                        // ___________________________________ Today is sched week day ___
                        if ($now->format("D") == $wday) {
                            // ________________________________________ Was run before ___
                            if ($last &&
                                // _________________________________ AND was run today ___
                                $last->format("Ymd") == $now->format("Ymd") &&
                                // ________________ AND was run at or after sched time ___
                                $last->format("His") >= $sched->format("His")) {
                                // _______________________________ Next week sched day ___
                                $next = clone $now;
                                $next->modify("+1 week");
                                $next->setTime($sched->format("H"),
                                               $sched->format("i"),
                                               $sched->format("s"));
                                }
                            // _______________________________________ To be run today ___
                            else {
                                // ____________________________ Sched time has elapsed ___
                                if ($now->format("His") >= $sched->format("His")) {
                                    $next = 0;
                                    }
                                // _____________________________ To be run later today ___
                                else {
                                    $next = clone $now;
                                    $next->setTime($sched->format("H"),
                                                   $sched->format("i"),
                                                   $sched->format("s"));
                                    }
                                }
                            }
                        // _____________________________ To be run next week sched day ___
                        else {
                            $next = new DateTime($wday);
                            $next->setTime($sched->format("H"),
                                           $sched->format("i"),
                                           $sched->format("s"));
                            }
                        break;
                    // ____________________________________________ Recurrence monthly ___
                    case "M":
                        $mday = $this->month_day;
                        // __________________________________ Today is sched month day ___
                        if ($now->format("j") == $mday) {
                            // ________________________________________ Was run before ___
                            if ($last &&
                                // _________________________________ AND was run today ___
                                $last->format("Ymd") == $now->format("Ymd") &&
                                // ________________ AND was run at or after sched time ___
                                $last->format("His") >= $sched->format("His")) {
                                // ________________________________________ Next month ___
                                if ($now->format("j") > $mday) {
                                    $next = new DateTime("first day of next month");
                                    // ______________________ Manage last day of month ___
                                    $next->setDate($next->format("Y"),
                                                   $next->format("m"),
                                                   ($mday > $next->format("t") ?
                                                    $next->format("t") :
                                                    $mday));
                                    $next->setTime($sched->format("H"),
                                                   $sched->format("i"),
                                                   $sched->format("s"));
                                    }
                                // ________________________________________ This month ___
                                else {
                                    $next = new DateTime();
                                    // ______________________ Manage last day of month ___
                                    $next->setDate($now->format("Y"),
                                                   $now->format("m"),
                                                   ($mday > $now->format("t") ?
                                                    $now->format("t") :
                                                    $mday));
                                    $next->setTime($sched->format("H"),
                                                   $sched->format("i"),
                                                   $sched->format("s"));
                                    }
                                }
                            // _______________________________________ To be run today ___
                            else {
                                // ____________________________ Sched time has elapsed ___
                                if ($now->format("His") >= $sched->format("His")) {
                                    $next = 0;
                                    }
                                // _____________________________ To be run later today ___
                                else {
                                    $next = clone $now;
                                    $next->setTime($sched->format("H"),
                                                   $sched->format("i"),
                                                   $sched->format("s"));
                                    }
                                }
                            }
                        // ____________________________ To be run next sched month day ___
                        else {
                            // ____________________________________________ Next month ___
                            if ($now->format("j") > $mday) {
                                $first = new DateTime("first day of next month");
                                $next  = new DateTime();
                                // __________________________ Manage last day of month ___
                                $next->setDate($first->format("Y"),
                                               $first->format("m"),
                                               ($mday > $first->format("t") ?
                                                $first->format("t") :
                                                $mday));
                                $next->setTime($sched->format("H"),
                                               $sched->format("i"),
                                               $sched->format("s"));
                                }
                            // ____________________________________________ This month ___
                            else {
                                $next = new DateTime();
                                // __________________________ Manage last day of month ___
                                $next->setDate($now->format("Y"),
                                               $now->format("m"),
                                               ($mday > $now->format("t") ?
                                                $now->format("t") :
                                                $mday));
                                $next->setTime($sched->format("H"),
                                               $sched->format("i"),
                                               $sched->format("s"));
                                }
                            }
                        break;
                    }
                break;
            }
        return $next;

        }


    /**
     * Returns next scheduled task execution as a human readable description.
     *
     * @return string
     */
    public function get_next_run_desc() {

        $next = $this->get_next_run();
        $desc = "Never";
        if (is_a($next, "DateTime")) {
            $now = new DateTime();
            if ($next->format("Ymd") == $now->format("Ymd")) {
                $desc = "Today ".$next->format("H:i");
                }
            else {
                $now->modify("+1 day");
                if ($next->format("Ymd") == $now->format("Ymd")) {
                    $desc = "Tomorrow ".$next->format("H:i");
                    }
                else {
                    $desc = $next->format("d/m/Y H:i");
                    }
                }
            }
        elseif (is_numeric($next)) {
            $desc = "Waiting...";
            }
        return $desc;

        }

    }


/**
 * This class is a container for Janox Scheduler related methods.
 * Janox Scheduler is an operation mode of a Janox Application, intended to be called by
 * a cron-system, to execute scheduled tasks.
 * Janox Scheduler, other than scheduled tasks, runs queued processes, according with
 * Janox Nework Services system.
 *
 */
class o2_scheduler {

    static $sched_table = "jx_scheduler"; /* Table for scheduled tasks storage          */
    static $sched_word  = "jxsched";      /* Incoming parameter to run as scheduler     */


    /**
     * Run current session as a scheduler, running pending tasks.
     * If run as scheduler returns number of run tasks, else returns FALSE.
     *
     * @param  boolean $force   Force session to run as scheduler
     * @return boolean
     */
    static function run_session($force = false) {

        // __________________________________________________________ Run as scheduler ___
        if ($force || isset($_REQUEST[self::$sched_word])) {
            print 'JXScheduler: '.date("d-m-Y H:i:s")."\n";
            // __________________________________________ Delete old run-job instances ___
            print "Clear obsolete instances.\n";
            o2_run_job::clear_all();
            // _________________________________________________ Check scheduled tasks ___
            $app = $_SESSION['o2_app'];
            $ret = 0;
            //  If a job is pending from more than one task it's executed only once(?) ___
            print "Read pending tasks.\n";
            foreach (self::get_list2run(false) as $sched_id => $job) {
                // ________________________________________________ Run without ASP-ID ___
                if ($job['aspid'] == 'NOASP') {
                    print "   Run job \"".$job['job']."\"\n";
                    o2_job::get_by_id($job['job'])->run('', array(), $sched_id);
                    }
                // ___________________________________________ Run only for set ASP-ID ___
                elseif ($job['aspid']) {
                    print '   Run job "'.$job['job'].
                          '" for ASP-ID "'.$job['aspid']."\"\n";
                    o2_run_job::queue(o2_job::get_by_id($job['job']),
                                      array(),
                                      $job['aspid'],
                                      false,
                                      false,
                                      false,
                                      $sched_id);
/*
                    o2_job::get_by_id($job['job'])->run($job['aspid'],
                                                        array(),
                                                        $sched_id);
*/
                    }
                // __________________________________________ Run once for each ASP-ID ___
                else {
                    $aspids = $app->asp_codes();
                    if (!count($aspids) || (count($aspids) === 1 && !$aspids[0])) {
                        print "   Run job \"".$job['job']."\" (without ASP-ID)\n";
                        o2_job::get_by_id($job['job'])->run('', array(), $sched_id);
                        }
                    else {
                        print "   Run job \"".$job['job']."\" for every ASP-IDs\n";
                        foreach ($aspids as $aspid) {
                            if ($aspid) {
                                print "    - ASP-ID \"".$aspid."\"\n";
                                o2_run_job::queue(o2_job::get_by_id($job['job']),
                                                  array(),
                                                  $aspid,
                                                  false,
                                                  false,
                                                  false,
                                                  $sched_id);
/*
                                o2_job::get_by_id($job['job'])->run($aspid,
                                                                    array(),
                                                                    $sched_id);
*/
                                }
                            }
                        }
                    }
                $ret++;
                }
            print 'Tasks executed: '.$ret."\n";
            // _____________________________ Check pending processes (queued run-jobs) ___
            $ps = o2_run_job::run_queued();
            print 'Queued jobs run: '.$ps."\n";
            // ____________________ Check killed processes (run-jobs marked as killed) ___
            $ks = o2_run_job::kill_by_host();
            print 'Jobs killed: '.$ks."\n";
            return true;
            }
        // ________________________________________________ Session is not a scheduler ___
        else {
            return false;
            }

        }


    /**
     * Returns a task object from db data with Sched-ID = $sched_id
     *
     * @param  integer $sched_id   Task ID to look for
     * @return o2_task
     */
    static function get_task_by_id($sched_id) {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $co     = constant("o2_".$server->type."_o");
        $cc     = constant("o2_".$server->type."_c");
        $f      = $tab->campi;
        $select = $f["sched_job"]." ".$co."SCHED_JOB".$cc.",".
                  $f["sched_user"]." ".$co."SCHED_USER".$cc.",".
                  $f["sched_type"]." ".$co."SCHED_TYPE".$cc.",".
                  $f["sched_interval"]." ".$co."SCHED_INTERVAL".$cc.",".
                  $f["sched_date"]." ".$co."SCHED_DATE".$cc.",".
                  $f["sched_time"]." ".$co."SCHED_TIME".$cc.",".
                  $f["sched_recurrence"]." ".$co."SCHED_RECURRENCE".$cc.",".
                  $f["sched_week_day"]." ".$co."SCHED_WEEK_DAY".$cc.",".
                  $f["sched_month_day"]." ".$co."SCHED_MONTH_DAY".$cc.",".
                  $f["sched_aspid"]." ".$co."SCHED_ASPID".$cc.",".
                  $f["sched_all_hosts"]." ".$co."SCHED_ALL_HOSTS".$cc.",".
                  $f["sched_paused"]." ".$co."SCHED_PAUSED".$cc;
        $where  = $f["sched_id"]."='".$sched_id."'";
        $rec    = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_sched",
                                        $select,
                                        $where,
                                        "",
                                        1);
        if (count($rec)) {
            $rec = $rec[0];
            return new o2_task($sched_id,
                               $rec["SCHED_JOB"],
                               $rec["SCHED_USER"],
                               $rec["SCHED_TYPE"],
                               $rec["SCHED_INTERVAL"],
                               $rec['SCHED_DATE'],
                               $rec['SCHED_TIME'],
                               $rec["SCHED_RECURRENCE"],
                               $rec["SCHED_WEEK_DAY"],
                               $rec["SCHED_MONTH_DAY"],
                               $rec["SCHED_ASPID"],
                               $rec["SCHED_PAUSED"] === '1',
                               $rec["SCHED_ALL_HOSTS"] === '1');
            }
        else {
            return false;
            }

        }


    /**
     * Returns next job execution
     *
     * @param  integer $job_id
     * @return o2_run_job
     */
    static function get_next_job($job_id) {

        $app      = $_SESSION['o2_app'];
        $tab      = $app->get_table(self::$sched_table);
        $db       = $tab->db;
        $server   = $db->server;
        $co       = constant("o2_".$server->type."_o");
        $cc       = constant("o2_".$server->type."_c");
        $select   = $tab->campi["sched_id"]->nome_fisico." ".$co."SCHED_ID".$cc;
        $aspid    = $app->vars['_area']->valore;
        $aspfld   = $tab->campi["sched_aspid"]->nome_fisico;
        $where    = $tab->campi["sched_job"]->nome_fisico."=".$job_id." AND ".
                    $tab->campi["sched_paused"]->nome_fisico."<>'1' ".
                    ($aspid ?
                    "AND (".$aspfld."='' OR ".
                            $aspfld." IS NULL OR ".
                            $aspfld."='".$aspid."' OR ".
                            $aspfld."='NOASP')" :
                    "");
        $recs     = o2_gateway::recordset($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          "jx_sched",
                                          $select,
                                          $where,
                                          "",
                                          100);
        $ret_id   = 0;
        $ret_date = false;
        $next     = false;
        foreach ($recs as $rec) {
            $id   = $rec["SCHED_ID"];
            $task = self::get_task_by_id($id);
            $next = $task->get_next_run();
            // ____________________________________________________________ Waiting... ___
            if (is_numeric($next)) {
                $ret_date = $next;
                $ret_id   = $id;
                }
            // _________________________________________________________________ Never ___
            elseif ($next === false) {
                if ($ret_date === false) {
                    $ret_id = $id;
                    }
                }
            // __________________________________________________________________ Date ___
            elseif ($ret_date === false ||
                    (is_a($ret_date, "DateTime") &&
                    ($next < $ret_date))) {
                $ret_date = clone $next;
                $ret_id   = $id;
                }
            }
        return array('next' => $ret_date, 'sched_id' => $ret_id);

        }


    /**
     * Returns list of pending task, in the form: array(task-id => job-id)
     * If $effective is FALSE tasks list is recovered with a standard query and, if
     * something to do is found, then a new execution is started with $effective as TRUE.
     * When $effective is TRUE tasks list is recovered with a query with lock and result
     * list is returned.
     *
     * @param  string $effective   If query has to lock tasks list
     * @return array
     */
    static function get_list2run($effective = true) {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $co     = constant('o2_'.$server->type.'_o');
        $cc     = constant('o2_'.$server->type.'_c');
        $f      = $tab->campi;
        $select = $f['sched_id'].' '.$co.'SCHED_ID'.$cc.','.
                  $f['sched_job'].' '.$co.'SCHED_JOB'.$cc.','.
                  $f['sched_type'].' '.$co.'SCHED_TYPE'.$cc.','.
                  $f['sched_interval'].' '.$co.'SCHED_INTERVAL'.$cc.','.
                  $f['sched_date'].' '.$co.'SCHED_DATE'.$cc.','.
                  $f['sched_time'].' '.$co.'SCHED_TIME'.$cc.','.
                  $f['sched_recurrence'].' '.$co.'SCHED_RECURRENCE'.$cc.','.
                  $f['sched_week_day'].' '.$co.'SCHED_WEEK_DAY'.$cc.','.
                  $f['sched_month_day'].' '.$co.'SCHED_MONTH_DAY'.$cc.','.
                  $f['sched_aspid'].' '.$co.'SCHED_ASPID'.$cc.','.
                  $f['sched_all_hosts'].' '.$co.'SCHED_ALL_HOSTS'.$cc.','.
                  $f['sched_paused'].' '.$co.'SCHED_PAUSED'.$cc;
        $where  = $f['sched_paused']."<>'1'";
        $s_list = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        'jx_sched',
                                        $select,
                                        $where,
                                        '',
                                        1000000,
                                        null,
                                        $effective);
        $now    = new DateTime();
        $ret    = array();
        $srvs   = o2_service::get_all_host_services();
        foreach ($s_list as $s_idx => $sched_rec) {
            if ($sched_rec['SCHED_ALL_HOSTS'] != '1') {
                if ($app->service_redirect) {
                    $service = $app->service_redirect;
                    }
                else {
                    $job     = o2_job::get_by_id($sched_rec['SCHED_JOB']);
                    $service = $job->service;
                    }
                if (!isset($srvs[$service])) {
                    continue;
                    }
                $host = '';
                }
            else {
                $host = $app->host;
                }
            $todo     = false;
            $last_run = o2_run_job::get_last_job($sched_rec['SCHED_JOB'],
                                                 $sched_rec['SCHED_ASPID'],
                                                 '',
                                                 $sched_rec['SCHED_ID'],
                                                 $host);
            $last     = new DateTime();
            $last->setTimestamp($last_run ? $last_run->start : 0);
            switch ($sched_rec['SCHED_TYPE']) {
                // __________________________________________________________ Run Once ___
                case "O":
                    $sd    = $sched_rec['SCHED_DATE'];
                    $st    = $sched_rec['SCHED_TIME'];
                    $sched = new DateTime($sd[0].$sd[1].$sd[2].$sd[3]."-".
                                          $sd[4].$sd[5]."-".
                                          $sd[6].$sd[7]." ".
                                          $st[0].$st[1].":".
                                          $st[2].$st[3].":".
                                          $st[4].$st[5]);
                             // ___________ IF: last run is before scheduled date/time ___
                    $todo  = ($last < $sched) &&
                             // __________ AND: today is the day scheduled for running ___
                             ($sched->format("Ymd") == $now->format("Ymd")) &&
                             // __ AND: today time is the same or after scheduled time ___
                             ($sched->format("His") <= $now->format("His"));
                    break;
                // ___________________________________________________ Run by Interval ___
                case "I":
                    // ___ Set 1 minute as "safe" time to avoid executions overlapping ___
                    $last->modify("+".$sched_rec["SCHED_INTERVAL"]." min");
                            // _____________ IF: last run + $int minutes is before now ___
                    $todo = ($last->getTimestamp() < $now->getTimestamp());
                    break;
                // _________________________________________________ Run by Recurrence ___
                case "R":
                    // ____________________________________________ Manage "safe-time" ___
                    /**
                     * "safe-time" is the time after which the recurrence is obsolete.
                     * For example, if safe-time is 10 mins (600 secs) and scheduled time
                     * is 1:00, after 1:10 task will not start until next recurrence.
                     */
                    // _________________________________________ Last execution exists ___
                    $s_time = clone $now;
                    $s_time->sub(new DateInterval('PT10M'));
                    $safe_time = $sched_rec["SCHED_TIME"] > $s_time->format("His") ||
                                 $s_time->format('Ymd') < $now->format('Ymd') ;
                    // ________________________ Last execution before today (or never) ___
                    $exp_date  = $last->format("Ymd") < $now->format("Ymd");
                    // _________________________ Scheduled time before or equal to now ___
                    $exp_time  = $sched_rec["SCHED_TIME"] <= $now->format("His");
                    // _____________________________________ Switch type of recurrence ___
                    switch ($sched_rec["SCHED_RECURRENCE"]) {
                        case "D":
                                    // __________________ IF: last run is before today ___
                            $todo = $exp_date &&
                                    // _____________________ AND time is now or before ___
                                    $exp_time && $safe_time;
                            break;
                        case "W":
                                    // __________________ IF: last run is before today ___
                            $todo = $exp_date &&
                                    // __ AND: today week-day is the same as scheduled ___
                                    ($now->format("D") == $sched_rec["SCHED_WEEK_DAY"]) &&
                                    // _____________________ AND time is now or before ___
                                    $exp_time && $safe_time;
                            break;
                        case "M":
                                    // __________________ IF: last run is before today ___
                            $todo = $exp_date &&
                                    // _ AND: today month-day is the same as scheduled ___
                                    ($now->format("j") ==$sched_rec["SCHED_MONTH_DAY"]) &&
                                    // _____________________ AND time is now or before ___
                                    $exp_time && $safe_time;
                            break;
                        }
                    break;
                }
            if ($todo) {
                if ($effective) {
                    $ret[$sched_rec["SCHED_ID"]] =
                                               array("job"  => $sched_rec["SCHED_JOB"],
                                                     "aspid"=> $sched_rec['SCHED_ASPID']);
                    }
                else {
                    return self::get_list2run();
                    }
                }
            }
        return $ret;

        }


    /**
     * Returns list of scheduled tasks for the requested job
     * NOTE: if session ASPID is set then only scheduled items for current ASPID are
     *       returned (ASP blank, 'NOASP' or equal to ASPID).
     *
     * @param  integer $job_id
     * @return array
     */
    static function get_by_job($job_id) {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $co     = constant("o2_".$server->type."_o");
        $cc     = constant("o2_".$server->type."_c");
        $select = $tab->campi["sched_id"]->nome_fisico." ".$co."SCHED_ID".$cc.",".
                  $tab->campi["sched_paused"]->nome_fisico." ".$co."SCHED_PAUSED".$cc;
        $aspid  = $app->vars['_area']->valore;
        $aspfld = $tab->campi["sched_aspid"]->nome_fisico;
        $where  = $tab->campi["sched_job"]->nome_fisico."=".$job_id.
                  ($aspid ?
                   " AND (".$aspfld."='' OR ".
                            $aspfld." IS NULL OR ".
                            $aspfld."='".$aspid."' OR ".
                            $aspfld."='NOASP')" :
                   "");
        $recs   = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_sched",
                                        $select,
                                        $where,
                                        "",
                                        100);
        $list   = array();
        if (count($recs)) {
            foreach ($recs as $rec_id => $rec) {
                $sched_id        = $rec["SCHED_ID"];
                $sched_obj       = o2_scheduler::get_task_by_id($sched_id);
                $list[$sched_id] = array('desc'   => $sched_obj->get_description(),
                                         'active' => $rec["SCHED_PAUSED"] !== '1');
                }
            return $list;
            }
        else {
            return false;
            }

        }


    /**
     * Returns HTML code for scheduler admin console.
     * $buttons_action  is the local program action to be executed when a monitor button
     *                  is pressed. Req-params passed to action are:
     *                   1. show:
     *                       -show:  Show task details icon
     *                   2. task-id
     * If $job_id is passed it is used to filter scheduled tasks by.
     * If $job_id is not passed and $job_name is passed, then $job_name is used to filter
     * scheduled tasks by.
     *
     * @param  string  $buttons_action
     * @param  string  $job_name
     * @param  integer $job_id
     * @return string
     */
    static function get_monitor($buttons_action = "", $job_name = "", $job_id = 0) {

        $app    = $_SESSION['o2_app'];
        $rnt    = $app->runtime;
        $tab    = $app->get_table(self::$sched_table);
        $db     = $tab->db;
        $server = $db->server;
        $co     = constant("o2_".$server->type."_o");
        $cc     = constant("o2_".$server->type."_c");
        $select = $tab->campi["sched_job"]->nome_fisico." ".$co."SCHED_JOB".$cc.",".
                  $tab->campi["sched_id"]->nome_fisico." ".$co."SCHED_ID".$cc;
        $aspid  = $app->vars['_area']->valore;
        $aspfld = $tab->campi["sched_aspid"]->nome_fisico;
        $where  = ($aspid ?
                   $aspfld."='' OR ".
                   $aspfld." IS NULL OR ".
                   $aspfld."='".$aspid."' OR ".
                   $aspfld."='NOASP'" :
                   "");
        $recs   = o2_gateway::recordset($server->type,
                                        $server->server,
                                        $server->user,
                                        $server->password,
                                        $db->nome,
                                        $db->proprietario,
                                        $tab->nome,
                                        "jx_sched",
                                        $select,
                                        $where,
                                        "",
                                        1000000);
        $code   = '<table style="width:100%"><tr><th colspan="2">Job</th><th>Module</th>'.
                  '<th>ASP-ID</th><th>Scheduled</th><th>Next run</th></tr>';
        $img    = $rnt->alias."img/jobs/";
        if (count($recs)) {
            $job_name = strtolower($job_name);
            foreach ($recs as $rec_id => $rec) {
                $job = o2_job::get_by_id($rec["SCHED_JOB"]);
                if ((!$job_id && !$job_name) ||
                    ($job_id && ($job->id == $job_id)) ||
                    ($job_name && (strtolower($job->name) == $job_name))) {
                    $task = self::get_task_by_id($rec["SCHED_ID"]);
                    $desc = $task->get_description();
                    $next = $task->get_next_run_desc();
                    $ico  = $img.($task->paused ? "paused.png" : "scheduled.png");
                    $row  = '<tr'.($task->paused ?
                                   ' style="color:#A8A8A8"' :
                                   ($next == "Never" ?
                                    ' style="color:#993300"' :
                                    ($next == "Waiting..." ?
                                     ' style="color:#009900"' : ""))).'>';
                    $code.= $row.'<td><img src="'.$ico.
                            ($buttons_action ?
                             '" class="jximgbtn" title="See task details" onclick="'.
                             o2_act4js(0, $buttons_action, "show", $task->id).'"' : '"').
                            '></td><td>'.$job->name.
                            '</td><td>'.$job->prg.
                            '</td><td>'.($task->aspid ?
                                         ($task->aspid == "NOASP" ?
                                          "[NOASP]" :
                                          $task->aspid) :
                                         "[All]").
                            '</td><td>'.$desc.
                            '</td><td>'.$next.
                            '</td></tr>';
                    }
                }
            $code.= '</table>';
            return $code;
            }
        else {
            return false;
            }

        }

    }


/**
 * This class is an application analyzer, to look for elements usage in application
 * structure
 *
 */
class o2_inspector {

    static $mlt_table     = 'jx_translations'; /* System table for translations storage */
    /*     ===== ELEMENT IDENTIFIER WORDS ============================================= */
    static $w_table       = "table";
    static $w_link_table  = "link_table";
    static $w_program     = "prg";
    static $w_par         = "par";
    static $w_var         = "var";
    static $w_menu        = "menu";
    static $w_view        = "view";
    static $w_field       = "field";
    static $w_link_field  = "link_field";
    static $w_formula     = "formula";
    static $w_sql         = "sql_formula";
    static $w_aggregate   = "aggregate";
    static $w_action      = "act";
    static $w_step        = "step";
    static $w_expression  = "exp";
    static $w_form        = "form";
    static $w_protocol    = "protocol";
    static $w_ctrl        = "ctrl";
    static $w_io          = "io";
    /*     ===== PROPERTIES =========================================================== */
    public $app_name      = "";
    public $developer     = false;
    public $dir_home      = null;
    public $dir_prgs      = null;
    public $dir_prgs_dev  = null;
    public $dir_tmp       = null;
    public $prgs_list     = array();
    public $tabs_repos    = "";
    public $menu_repos    = "";
    public $vars_repos    = "";
    /*     ===== SERVICE PARAMETERS =================================================== */
    public $img_path      = "<jx>/img/tools/tree/";


    public function __construct($app_script = "", $developer = false) {

        // ____________________________________________ Called with application script ___
        if ($app_script && file_exists($app_script)) {
            $this->app_name = basename($app_script, ".php");
            $this->dir_home = new o2_dir(dirname($app_script).
                                         DIRECTORY_SEPARATOR."..".DIRECTORY_SEPARATOR);
            $this->dir_prgs = new o2_dir($this->dir_home->nome_completo.
                                         "prgs".DIRECTORY_SEPARATOR);
            if (file_exists($this->dir_home.'users'.DIRECTORY_SEPARATOR.'root')) {
                $this->dir_tmp = new o2_dir($this->dir_home.'users'.DIRECTORY_SEPARATOR.
                                            'root'.DIRECTORY_SEPARATOR);
                }
            elseif (file_exists($this->dir_home.'logs')) {
                $this->dir_tmp = new o2_dir($this->dir_home.'logs');
                }
            else {
                $this->dir_tmp = $this->dir_home;
                }
            // ___ Called as developer: set dev prgs folder ______________________________
            if ($developer && $this->dir_prgs->exists($developer)) {
                $this->developer    = $developer;
                $this->dir_prgs_dev = new o2_dir($this->dir_home->nome_completo.
                                                 "prgs".DIRECTORY_SEPARATOR.
                                                 $developer.DIRECTORY_SEPARATOR);
                }
            // _________________________ If not exists dev-folder then reset developer ___
            else {
                $this->developer = false;
                }
            // ___________________________________ Read application configuration file ___
            if (!$this->dir_home->exists($this->app_name.".ini")) {
                throw new o2_exception('Inspector: missing INI settings for script "'.
                                       $app_script.'"',
                                       o2error_MISSINGFILE);
                }
            $ini_file = $this->dir_home->element($this->app_name.".ini");
            $conf     = parse_ini_file($ini_file);
            $tabs     = (isset($conf['tables']) ?
                         $conf['tables'] :
                         "file_repository.inc");
            $menus    = (isset($conf['menus']) ?
                         $conf['menus'] :
                         "menu_repository.inc");
            $vars     = (isset($conf['vars']) ?
                         $conf['vars'] :
                         "var_repository.inc");
            // _________________________________________ Load custom tables repository ___
            if ($this->developer && $this->dir_prgs_dev->exists($tabs)) {
                $this->tabs_repos = $this->dir_prgs_dev->nome_completo.$tabs;
                }
            // _______________________________________ Load standard tables repository ___
            else {
                $this->tabs_repos = $this->dir_prgs->nome_completo.$tabs;
                }
            // __________________________________________ Load custom menus repository ___
            if ($this->developer && $this->dir_prgs_dev->exists($menus)) {
                $this->menu_repos = $this->dir_prgs_dev->nome_completo.$menus;
                }
            // ________________________________________ Load standard menus repository ___
            else {
                $this->menu_repos = $this->dir_prgs->nome_completo.$menus;
                }
            // __________________________________________ Load custom menus repository ___
            if ($this->developer && $this->dir_prgs_dev->exists($vars)) {
                $this->vars_repos = $this->dir_prgs_dev->nome_completo.$vars;
                }
            // ________________________________________ Load standard menus repository ___
            else {
                $this->vars_repos = $this->dir_prgs->nome_completo.$vars;
                }
            }
        // ____________________________________________ Called from inside application ___
        elseif (isset($_SESSION['o2_app'])) {
            $app                = $_SESSION['o2_app'];
            $this->developer    = $app->developer;
            $this->dir_home     = new o2_dir($app->dir_home);
            $this->dir_prgs     = new o2_dir($app->dir_progs);
            $this->dir_prgs_dev = new o2_dir($app->dir_cvs_progs);
            $this->dir_tmp      = new o2_dir($app->dir_tmp);
            // _________________________________________ Load custom tables repository ___
            if ($this->developer && $this->dir_prgs_dev->exists($app->file_tabelle)) {
                $this->tabs_repos = $this->dir_prgs_dev->nome_completo.$app->file_tabelle;
                }
            // _______________________________________ Load standard tables repository ___
            else {
                $this->tabs_repos = $this->dir_prgs->nome_completo.$app->file_tabelle;
                }
            // __________________________________________ Load custom menus repository ___
            if ($this->developer && $this->dir_prgs_dev->exists($app->file_menu)) {
                $this->menu_repos = $this->dir_prgs_dev->nome_completo.$app->file_menu;
                }
            // ________________________________________ Load standard menus repository ___
            else {
                $this->menu_repos = $this->dir_prgs->nome_completo.$app->file_menu;
                }
            // __________________________ Load custom application variables repository ___
            if ($this->developer && $this->dir_prgs_dev->exists($app->file_vars)) {
                $this->vars_repos = $this->dir_prgs_dev->nome_completo.$app->file_vars;
                }
            // ________________________ Load standard application variables repository ___
            else {
                $this->vars_repos = $this->dir_prgs->nome_completo.$app->file_vars;
                }
            }
        else {
            throw new o2_exception(($app_script ?
                                    'Inspector: unknown application from script "'.
                                     $app_script.'"' :
                                    'Inspector: missing application'),
                                   o2error_MISSINGFILE);
            }
        // _______________________________________________ Load standard programs list ___
        $this->prgs_list = array();
        foreach ($this->dir_prgs->all_elements("*.prf") as $prf) {
            $this->prgs_list[$prf->nome] = $prf->nome_completo;
            }
        // _________________________________________________ Load custom programs list ___
        if ($this->developer) {
            foreach ($this->dir_prgs_dev->all_elements("*.prf") as $prf) {
                $this->prgs_list[$prf->nome] = $prf->nome_completo;
                }
            }

        }


    // ======================== Methods used from inside app to get repositories lists ===

    static function get_app_models() {

        $app  = $_SESSION['o2_app'];
        $mods = array();
        foreach (array_keys($app->maschere) as $mod_name) {
            if (substr($mod_name, 0, 3) != "_o2" &&
                substr($mod_name, 0, 2) != "o2"  &&
                substr($mod_name, 0, 2) != "jx") {
                $mods[$mod_name] = $mod_name;
                }
            }
        return $mods;

        }


    static function get_app_vars() {

        $app  = $_SESSION['o2_app'];
        $vars = array();
        foreach ($app->vars as $var_name => $var_obj) {
            $vars[$var_name] = $var_obj->maschera->nome;
            }
        return $vars;

        }


    static function get_app_prgs() {

        $app       = $_SESSION['o2_app'];
        $prgs_list = array();
        // _______________________________________________ Load standard programs list ___
        foreach ($app->dir_progs->all_elements("*.prf") as $prf) {
            $prgs_list[$prf->nome] = $prf->nome_completo;
            }
        // ____________________________________________ Load development programs list ___
        if ($app->developer) {
            foreach ($app->dir_cvs_progs->all_elements("*.prf") as $prf) {
                $prgs_list[$prf->nome] = $prf->nome_completo;
                }
            }
        // _________________________________________________ Load custom programs list ___
        if ($app->prgs_path_inc) {
            foreach ($app->prgs_path_inc as $path) {
                if ($dh = new o2_dir($path)) {
                    foreach ($dh->all_elements("*.prf") as $prf) {
                        if (!isset($prgs_list[$prf->nome])) {
                            $prgs_list[$prf->nome] = $prf->nome_completo;
                            }
                        }
                    unset($dh);
                    }
                }
            }
        return $prgs_list;

        }


    static function get_app_menus($first_level_only = true) {

        $app   = $_SESSION['o2_app'];
        $bar   = $app->menu_attivo;
        $list  = array();
        $code  = file_get_contents($app->dir_progs->element($app->file_menu));
        $parts = array();
        $res   = preg_match_all('/o2def::menu_item\("([^"]*)",\s*"([^"]*)",\s*"([^"]*)",'.
                                '\s*"([^"]*)",\s*"([^"]*)"/',
                                $code, $parts);
        /*
         1 = padre
         2 = tipo [M|P|...]
         3 = menu name
         4 = Menu label
         5 = prg-name
        */
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                // _________________________________________________ Only first level! ___
                if (($parts[2][$i] != 'S') &&
                    (!$first_level_only || $parts[1][$i] == $bar)) {
                    $list[$parts[3][$i]] = $parts[4][$i];
                    }
                }
            }
        return $list;

        }


    static function get_app_tabs() {

        $app  = $_SESSION['o2_app'];
        $tabs = array();
        foreach ($app->tab as $tab) {
            $tab_name = $tab->indice;
            if (!$tab->sys_tab && substr($tab_name, -4) != "_log") {
                $tabs[$tab_name] = $tab_name;
                }
            }
        return $tabs;

        }


    static function get_tab_fields($tab_name) {

        $app    = $_SESSION['o2_app'];
        $fields = array();
        if ($tab_name) {
            foreach (array_keys($app->tab[$tab_name]->campi) as $field_name) {
                $fields[$field_name] = $field_name;
                }
            }
        return $fields;

        }


    static function get_tab_keys($tab_name) {

        $app  = $_SESSION['o2_app'];
        $keys = array();
        if ($tab_name) {
            foreach (array_keys($app->tab[$tab_name]->chiavi) as $key_name) {
                $keys[$key_name] = $key_name;
                }
            }
        return $keys;

        }


    static function get_prg_exps_list($prg_name) {

        $app  = $_SESSION['o2_app'];
        $list = array();
        if ($prg_name) {
            $code = file_get_contents(($app->developer &&
                                       $app->dir_cvs_progs->exists($prg_name.".prf") ?
                                       $app->dir_cvs_progs :
                                       $app->dir_progs).$prg_name.".prf");
            $exps = array_keys(self::get_prg_exps($code, $prg_name));
            $list = array_combine($exps, $exps);
            }
        return $list;

        }


    static function get_prg_pars_list($prg_name) {

        $insp = new self;
        $prgs = $insp->get_app_prgs();
        return self::get_prg_params($prgs[$prg_name]);

        }


    static function get_prg_views_list($prg_name) {

        $insp  = new self;
        $prgs  = $insp->get_app_prgs();
        $views = self::get_prg_views($prgs[$prg_name]);
        $views = array_keys($views);
        $list  = array();
        foreach ($views as $view_name) {
            if ($view_name == 'prg�_�var') {
                $list['prg�_�var'] = "[Local variables]";
                }
            else {
                $list[$view_name] = $view_name;
                }
            }
        return $list;

        }


    static function get_prg_acts_list($prg_name) {

        $insp = new self;
        $prgs = $insp->get_app_prgs();
        $acts = self::get_prg_acts($prgs[$prg_name]);
        $acts = array_keys($acts);
        $list = array();
        foreach ($acts as $act_name) {
            $list[$act_name] = $act_name;
            }
        return $list;

        }


    static function get_prg_view_fields_list($prg_name, $view_name) {

        $insp = new self;
        $prgs = $insp->get_app_prgs();
        $list = array();
        if ($view_name == 'prg�_�var') {
            $fields = self::get_prg_variables(file_get_contents($prgs[$prg_name]));
            foreach ($fields as $field_name => $field_model) {
                $list[$field_name] = $field_name;
                }
            }
        else {
            $fields = self::get_prg_selects(file_get_contents($prgs[$prg_name]));
            foreach ($fields[$view_name] as $view_obj) {
                foreach ($view_obj as $field_obj) {
                    $field        = $field_obj['alias'];
                    $list[$field] = $field;
                    }
                }
            }
        return $list;

        }


    // =============================================================== Service methods ===

    /**
     * Find last defined ACTION before passed offset and returns action name
     *
     * @param  string  $prf_code
     * @param  integer $offset
     * @return string
     */
    static function get_action_before($prf_code, $offset) {

        // ________________________________________________________ Search action name ___
        $parts  = array();
        preg_match_all('/function \w*��(\w*)_act\(/',
                       substr($prf_code, 0, $offset), $parts);
        // _____________________________________ Get last action defined before string ___
        return array_pop($parts[1]);

        }


    /**
     * Reads all menus and returns an array in the form:
     *
     * (prg =>
     *    (menu-label))
     *
     * @return array
     */
    public function get_menu_prgs() {

        $code  = file_get_contents($this->menu_repos);
        $ret   = array();
        $parts = array();
        $res   = preg_match_all('/o2def::menu_item\("([^"]*)",\s*"([^"]*)",\s*"([^"]*)",'.
                                '\s*"([^"]*)",\s*"([^"]*)"/',
                                $code, $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                if ($parts[2][$i] == "P") {
                    $prg = $parts[5][$i];
                    if (!isset($ret[$prg])) {
                        $ret[$prg] = array();
                        }
                    $ret[$prg][] = $parts[4][$i];
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads all descentant menus from a given one and returns all called programs:
     *
     * @return array
     */
    public function get_prgs_by_menu($menu = '', $menus = false, $result = array()) {

        if (!$menus) {
            $menus = array();
            $code  = file_get_contents($this->menu_repos);
            preg_match_all('/o2def::menu_item\("([^"]*)",\s*"([^"]*)",'.
                           '\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)"/',
                           $code, $menus);
            /*
             1 = padre
             2 = tipo [M|P|...]
             3 = menu name
             4 = Menu label
             5 = prg-name
            */
            }
        $ms = count($menus[0]);
        if ($ms) {
            for ($i = 0; $i < $ms; $i++) {
                if ($menus[1][$i] == $menu) {
                    if ($menus[2][$i] == 'P') {
                        if (!in_array($menus[5][$i], $result)) {
                            $result[] = $menus[5][$i];
                            }
                        }
                    elseif ($menus[2][$i] == 'M') {
                        $result+= $this->get_prgs_by_menu($menus[3][$i],
                                                          $menus,
                                                          $result);
                        }
                    }
                }
            }
        return $result;

        }


    /**
     * Reads all descentant programs called from a given one
     *
     * @return array
     */
    public function get_prgs_by_prg($prg    = '',
                                    $depth  = 0,
                                    $result = array(),
                                    $level  = 0) {

        if (($prg != '[exp]') && !in_array($prg, $result)) {
            $result[] = $prg;
            }
        $level++;
        // _______________________________________ Look for CALLs and GOTOs in program ___
        if ((!$depth || ($level <= $depth)) && $this->prgs_list[$prg]) {
            $code  = file_get_contents($this->prgs_list[$prg]);
            $calls = self::get_prg_acts_call($code);
            foreach ($calls as $prg_name => $action) {
                if (($prg_name != '[exp]') && !in_array($prg_name, $result)) {
                    $result+= $this->get_prgs_by_prg($prg_name, $depth, $result, $level);
                    }
                }
            }
        return $result;

        }


    /**
     * Reads all tables and fields to look for data-types and returns an array in the
     * form:
     *
     * (model =>
     *    (table =>
     *       (field)))
     *
     * @param  string $prf_code
     * @return array
     */
    public function get_tabs_models() {

        $code  = file_get_contents($this->tabs_repos);
        $ret   = array();
        $parts = array();
        $res   = preg_match_all('/o2def::tab\("([^"]*)",[^;]*\);\s+(o2def::field\('.
                                '[^;]+\);\s+)++o2def::index/',
                                $code, $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                // _________________________________________________ Search table name ___
                $table     = $parts[1][$i];
                $match     = '/o2def::field\("([^"]*)",\s*"[^"]*",\s*"([^"]*)"\);/';
                $partsflds = array();
                $resflds   = preg_match_all($match, $parts[0][$i], $partsflds);
                if ($resflds) {
                    for ($j = 0; $j < $resflds; $j++) {
                        $field = $partsflds[1][$j];
                        $model = $partsflds[2][$j];
                        if (!isset($ret[$model])) {
                            $ret[$model] = array();
                            }
                        if (!isset($ret[$model][$table])) {
                            $ret[$model][$table] = array();
                            }
                        $ret[$model][$table][] = $field;
                        }
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads all defined application variables to look for data-types and returns an array
     * in the form:
     *
     * (variable => model)
     *
     * @param  string $prf_code
     * @return array
     */
    public function get_variables() {

        $code  = file_get_contents($this->vars_repos);
        $ret   = array();
        $parts = array();
        $res   = preg_match_all('/o2def::appvar\(\s*"(\w*)"\s*,\s*"(\w*)"\s*\);/',
                                $code, $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                // ______________________________________________ Search variable name ___
                $var       = $parts[1][$i];
                $model     = $parts[2][$i];
                $ret[$var] = $model;
                }
            }
        return $ret;

        }


    /**
     * Reads program variables and returns a list in the form:
     *
     * (variable => model)
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_variables($prf_code) {

        $parts = array();
        $res  = preg_match_all('/\$task_prg�_�var\-\>definisci\("([^"]*)",'.
                               '\s*"([^"]*)"\);/',
                               $prf_code, $parts);
        $ret = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $variable       = $parts[1][$i];
                $model          = $parts[2][$i];
                $ret[$variable] = $model;
                }
            }
        return $ret;

        }


    /**
     * Reads program definition and returns start-action name
     *
     * @param  string $prf_path   Full path to PRF file
     * @return string
     */
    static function get_prg_start_act($prf_path) {

        $prg = substr($prf_path, 0, -1)."g";
        $ret = '';
        if (file_exists($prg)) {
            $parts = array();
            $res   = preg_match('/o2def::prg\("[^"]*",\s*"([^"]*)",\s*"",\s*__FILE__\);/',
                                file_get_contents($prg), $parts);
            if ($res) {
                $ret = $parts[1];
                }
            }
        return $ret;

        }


    /**
     * Reads program parameters and returns a list in the form:
     *
     * (parameter => model)
     *
     * @param  string $prf_path   Full path to PRF file
     * @return array
     */
    static function get_prg_params($prf_path) {

        $prg = substr($prf_path, 0, -1)."g";
        $ret = array();
        if (file_exists($prg)) {
            $parts = array();
            $res   = preg_match_all('/o2def::par\(\d*,\s*"([^"]*)",\s*"([^"]*)"\);/',
                                    file_get_contents($prg), $parts);
            if ($res) {
                for ($i = 0; $i < $res; $i++) {
                    $param       = $parts[1][$i];
                    $model       = $parts[2][$i];
                    $ret[$param] = $model;
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads program defined views and returns a list of them:
     *
     * @param  string $prf_path   Full path to PRF file
     * @return array
     */
    static function get_prg_views($prf_path) {

        $prg = substr($prf_path, 0, -1)."g";
        $ret = array();
        if (file_exists($prg)) {
            $prg_code = file_get_contents($prg);
            // __________________________________________________ Check prg�_�var view ___
            if (strpos($prg_code, 'o2def::view(\'prg�_�var\');')) {
                $ret['prg�_�var'] = array('prefix' => '', 'suffix' => '');
                }
            $parts = array();
            $res   = preg_match_all('/o2def::view\("(\w*)",\s*[^,]+,\s*'.
                                    '"(\w*)",\s*"(\w*)"/', $prg_code, $parts);
            if ($res) {
                for ($i = 0; $i < $res; $i++) {
                    $ret[$parts[1][$i]] = array('prefix' => $parts[2][$i],
                                                'suffix' => $parts[3][$i]);
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads all tables uses in program and returns a list in the form:
     *
     * (tab_name =>
     *    (view =>
     *       (tab_alias =>
     *          (main:   TRUE is main-table, FALSE is link,
     *           key:    Used key name, if not by-exp, else FALSE,
     *           keyexp: TRUE if key name by-exp, else FALSE,
     *           tabexp: TRUE if table name by-exp, else FALSE,
     *           dbexp:  TRUE if database name by-exp, else FALSE))))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_tabs($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$task_(\w*)\-\>usa_file(_link)*\("(\w*)","(\w*)",'.
                                '"([^"]*)",?([^,;]*),?([^;]*)\);/',
                                $prf_code, $parts);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $tab_view     = $parts[1][$i];
                $tab_name     = $parts[3][$i];
                $tab_main     = !$parts[2][$i] ;
                $tab_alias    = $parts[4][$i];
                $tab_key      = $parts[5][$i];
                $tab_key_exp  = (strpos($tab_key, "_exp_") !== false);
                $tab_name_exp = (strpos($parts[7][$i], "_exp_") !== false);
                $tab_db_exp   = (strpos($parts[6][$i], "_exp_") !== false);
                if (!$ret[$tab_name]) {
                    $ret[$tab_name] = array();
                    }
                if (!$ret[$tab_name][$tab_view]) {
                    $ret[$tab_name][$tab_view] = array();
                    }
                $tab = array("main"   => $tab_main,
                             "key"    => ($tab_key_exp ? false : $tab_key),
                             "keyexp" => $tab_key_exp,
                             "tabexp" => $tab_name_exp,
                             "dbexp"  => $tab_db_exp);
                $ret[$tab_name][$tab_view][$tab_alias] = $tab;
                }
            }
        return $ret;

        }


    /**
     * Reads all selected fields in program and returns a list in the form:
     *
     * (view =>
     *    (tab_alias =>
     *       (field_name =>
     *          (alias: field selection alias,
     *           min:   TRUE if a MIN expression is set, else FALSE,
     *           max:   TRUE if a MAX expression is set, else FALSE,
     *           not:   TRUE if a NOT expression is set, else FALSE,
     *           like:  TRUE if a LIKE expression is set, else FALSE))))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_selects($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$task_(\w*)\-\>usa\("(\w*)","(\w*)","(\w*)",'.
                                '([^,]*),([^,]*),([^,]*),([^;]*)\);/',
                                $prf_code, $parts);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $tab_view     = $parts[1][$i];
                $tab_alias    = $parts[3][$i];
                $field_name   = $parts[4][$i];
                $field_alias  = $parts[2][$i];
                $field_min    = (strpos($parts[5][$i], "_exp_") !== false);
                $field_max    = (strpos($parts[6][$i], "_exp_") !== false);
                $field_not    = (strpos($parts[7][$i], "_exp_") !== false);
                $field_like   = (strpos($parts[8][$i], "_exp_") !== false);
                if (!$ret[$tab_view]) {
                    $ret[$tab_view] = array();
                    }
                if (!$ret[$tab_view][$tab_alias]) {
                    $ret[$tab_view][$tab_alias] = array();
                    }
                $field = array("alias" => $field_alias,
                               "min"   => $field_min,
                               "max"   => $field_max,
                               "not"   => $field_not,
                               "like"  => $field_like);
                $ret[$tab_view][$tab_alias][$field_name] = $field;
                }
            }
        return $ret;

        }


    /**
     * Reads all aggragates defined in program views and returns a list in the form:
     *
     * (view =>
     *    (field_name =>
     *       (type => variable)))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_aggregates($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$task_(\w*)->aggregate\("(\w*)",\s*"(\w*)",\s*'.
                                '"prg�_�var",\s*"(\w*)"\);/',
                                $prf_code, $parts);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $view  = $parts[1][$i];
                $type  = $parts[2][$i];
                $field = $parts[3][$i];
                $var   = $parts[4][$i];
                if (!$ret[$view]) {
                    $ret[$view] = array();
                    }
                if (!$ret[$view][$field]) {
                    $ret[$view][$field] = array();
                    }
                $ret[$view][$field][$type] = $var;
                }
            }
        return $ret;

        }


    /**
     * Reads program defined actions and returns a list of them:
     *
     * @param  string $prf_path   Full path to PRF file
     * @return array
     */
    static function get_prg_acts($prf_path) {

        $prg = substr($prf_path, 0, -1)."g";
        $ret = array();
        if (file_exists($prg)) {
            $prg_code = file_get_contents($prg);
            $parts = array();
            $res   = preg_match_all('/o2def::act\("(\w*)"(,\s*"(\w*)")?\);/',
                                    $prg_code, $parts);
           if ($res) {
                for ($i = 0; $i < $res; $i++) {
                    $ret[$parts[1][$i]] = $parts[3][$i];
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads all used fields in program protocols and returns a list in the form:
     *
     * (protocol =>
     *    (view =>
     *       (alias => (field => mask))))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_protocols($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$protocollo_(\w*)\-\>usa\("(\w*)",\s*"(\w*)",\s*'.
                                '"(\w*)",\s*"(\w*)"\);/',
                                $prf_code, $parts);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $protocol = $parts[1][$i];
                $alias    = $parts[2][$i];
                $view     = $parts[3][$i];
                $field    = $parts[4][$i];
                $mask     = $parts[5][$i];
                if (!$ret[$protocol]) {
                    $ret[$protocol] = array();
                    }
                if (!$ret[$protocol][$view]) {
                    $ret[$protocol][$view] = array();
                    }
                $ret[$protocol][$view][$alias] = array($field => $mask);
                }
            }
        return $ret;

        }


    /**
     * Look for program forms actions (close & refresh) and returns an array in the form:
     *
     * (form => ('act-refresh' => <refresh-act>,
     *           'act-close'   => <close-act>))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_form_acts($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$form_(\w*)\-\>(refresh|close)_action\("(\w*)"\);/',
                                $prf_code, $parts);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $form   = $parts[1][$i];
                $type   = $parts[2][$i];
                $action = $parts[3][$i];
                if (!$ret[$form]) {
                    $ret[$form] = array();
                    }
                $ret[$form][$type.'-act'] = $action;
                }
            }
        return $ret;

        }


    /**
     * Reads all program expressions and returns an array in the form:
     *
     * (exp-ID => exp-body)
     *
     * @param  string $prf_code
     * @param  string $prg_name
     * @return array
     */
    static function get_prg_exps($prf_code, $prg_name) {

        $ret = array();
        $res = preg_split('/function '.$prg_name.'_exp_(\d+)\(\) \{/', $prf_code, -1,
                          PREG_SPLIT_DELIM_CAPTURE);
        if ($res) {
            array_shift($res);
            $yorn = true;
            foreach ($res as $id => $part) {
                if ($yorn) {
                    $expnum       = $part;
                    $expbody      = substr($res[$id + 1], 0, strrpos($res[$id + 1], "}"));
                    $ret[$expnum] = $expbody;
                    }
                $yorn = !$yorn;
                }
            }
        return $ret;

        }


    /**
     * Find all uses of an expression in a program script
     *
     * @param  string  $prf_code
     * @param  integer $exp
     * @return array
     */
    public function get_prg_exp($prf_code, $prg_name, $exp) {

        $ret      = array();
        $lines    = array();
        $prg_len  = strlen($prg_name);
        $prg_path = substr($this->prgs_list[$prg_name], 0, -1)."g";
        $prg_code = (file_exists($prg_path) ? file_get_contents($prg_path) : "");
        // _________________________________________________ Look for uses in PRG file ___
        $res      = explode($prg_name.'_exp_'.$exp.'()', $prg_code);
        if (count($res)) {
            array_pop($res);
            foreach ($res as $piece) {
                $in_form  = false;
                $in_io    = false;
                // __________________________________________________ Split into lines ___
                $lines    = array_merge($lines, explode("\n", $piece));
                // ___________________________________ Get last line before occurrence ___
                $last     = end($lines);
                // ______________________________________ Visibility condition in form ___
                $form_pos = strpos($last, 'o2def::form("');
                if ($form_pos !== false) {
                    $form_pos+= 13;
                    $form     = substr($last,
                                       $form_pos,
                                       strpos($last, '"', $form_pos) - $form_pos);
                    $in_form = true;
                    }
                // ___________________________________ Used in I/O resource definition ___
                $io_pos = strpos($last, 'o2def::io("');
                if ($io_pos !== false) {
                    $io_pos+= 11;
                    $io     = substr($last,
                                     $io_pos,
                                     strpos($last, '"', $io_pos) - $io_pos);
                    $in_io = true;
                    }
                if ($in_form) {
                    if (!isset($ret['F'])) {
                        $ret['F'] = array();
                        }
                    // ______________________________________________________ Add form ___
                    if (!isset($ret['F'][$form])) {
                        $ret['F'][$form] = array();
                        }
                    $ret['F'][$form]['F'] = $form;
                    }
                elseif ($in_io) {
                    if (!isset($ret['I'])) {
                        $ret['I'] = array();
                        }
                    // _______________________________________________________ Add I/O ___
                    $ret['I'][] = $io;
                    }
                }
            }
        // _________________________________________________ Look for uses in PRF file ___
        $res = explode($prg_name.'_exp_'.$exp.'()', $prf_code);
        if (count($res)) {
            array_pop($res);
            foreach ($res as $piece) {
                $in_view    = false;
                $in_act     = false;
                $in_form    = false;
                $table      = false;
                $link_tab   = false;
                $field      = false;
                $link_field = false;
                $formula    = false;
                $sql        = false;
                $form       = false;
                $rows       = false;
                $ctrl       = false;
                // __________________________________________________ Split into lines ___
                $lines      = array_merge($lines, explode("\n", $piece));
                // ________________________________________ Skip expression definition ___
                if (substr($piece, -9) != "function ") {
                    // _______________________________ Get last line before occurrence ___
                    $last     = end($lines);
                    // _____________________________________________ Righe-vis in view ___
                    $rows_pos = strpos($last, '->righe_vis = ');
                    if ($rows_pos) {
                        $rows_pos+= 14;
                        $rows     = substr($last,
                                           $rows_pos,
                                           strpos($last, '"', $rows_pos) - $rows_pos);
                        $in_view = true;
                        }
                    // _________________________________________________ Table in view ___
                    $tab_pos = strpos($last, '->usa_file("');
                    if ($tab_pos) {
                        $tab_pos+= 12;
                        $table   = substr($last,
                                          $tab_pos,
                                          strpos($last, '"', $tab_pos) - $tab_pos);
                        $in_view = true;
                        }
                    // __________________________________________________ Link in view ___
                    $link_pos = strpos($last, '->usa_file_link("');
                    if ($link_pos) {
                        $link_pos+= 17;
                        $link_tab = substr($last,
                                           $link_pos,
                                           strpos($last, '"', $link_pos) - $link_pos);
                        $in_view = true;
                        }
                    // _______________________________________________ Field selection ___
                    $field_pos = strpos($last, '->usa("');
                    if ($field_pos) {
                        $field_pos+= 7;
                        $field     = substr($last,
                                            $field_pos,
                                            strpos($last, '"', $field_pos) - $field_pos);
                        $in_view = true;
                        }
                    // _________________________________________________ Link criteria ___
                    $link_cri_pos = strpos($last, '->unisci("');
                    if ($link_cri_pos) {
                        $link_cri_pos+= 10;
                        $link_field   = substr($last,
                                               $link_cri_pos,
                                               strpos($last, '"', $link_cri_pos) -
                                               $link_cri_pos - 6);
                        $in_view = true;
                        }
                    // ____________________________________________ Formula definition ___
                    $formula_pos = strpos($last, '->calcola("');
                    if ($formula_pos) {
                        $formula_pos+= 11;
                        $formula     = substr($last,
                                              $formula_pos,
                                              strpos($last, '"', $formula_pos) -
                                              $formula_pos);
                        $in_view = true;
                        }
                    // ___________________________________________________ SQL-formula ___
                    $sql_pos = strpos($last, '->sql_formula("');
                    if ($sql_pos) {
                        $sql_pos+= 15;
                        $sql     = substr($last,
                                          $sql_pos,
                                          strpos($last, '"', $sql_pos) -
                                          $sql_pos);
                        $in_view = true;
                        }
                    // ________________________________________ Action step definition ___
                    $step_pos = strpos($last, '$o2exe->s(');
                    // _______________________________________________ Form definition ___
                    $form_pos = strpos($last, '$form_');
                    if ($form_pos !== false) {
                        $form_pos+= 6;
                        $form     = substr($last,
                                           $form_pos,
                                           strpos($last, '->', $form_pos) - $form_pos);
                        $in_form  = true;
                        }
                    // ____________________________________ Control definition in form ___
                    $ctrl_pos = strpos($last, '$ctrl_');
                    if ($ctrl_pos !== false) {
                        $ctrl_pos+= 6;
                        $ctrl     = substr($last,
                                           $ctrl_pos,
                                           strpos($last, '->', $ctrl_pos) - $ctrl_pos);
                        $in_form  = true;
                        }
                    // __________________________________ If needed look for view name ___
                    if ($in_view) {
                        // _______________________________ Look backward for view name ___
                        for ($i = count($lines); $i > 0; $i--) {
                            $line     = $lines[$i - 1];
                            $view_pos = strpos($line, 'function '.$prg_name.'��');
                            if ($view_pos !== false) {
                                $view = substr($line,
                                               $view_pos + $prg_len + 11,
                                               strpos($line,
                                                      '_def(',
                                                      $view_pos + $prg_len + 11) -
                                               $view_pos - $prg_len - 11);
                                break;
                                }
                            }
                        if (!isset($ret['V'])) {
                            $ret['V'] = array();
                            }
                        if (!isset($ret['V'][$view])) {
                            $ret['V'][$view] = array();
                            }
                        if ($rows_pos) {
                            $ret['V'][$view]['R'] = true;
                            }
                        elseif ($tab_pos) {
                            if (!isset($ret['V'][$view]['T'])) {
                                $ret['V'][$view]['T'] = array();
                                }
                            $ret['V'][$view]['T'][] = $table;
                            }
                        elseif ($link_pos) {
                            if (!isset($ret['V'][$view]['L'])) {
                                $ret['V'][$view]['L'] = array();
                                }
                            $ret['V'][$view]['L'][] = $link_tab;
                            }
                        elseif ($field_pos) {
                            if (!isset($ret['V'][$view]['S'])) {
                                $ret['V'][$view]['S'] = array();
                                }
                            $ret['V'][$view]['S'][] = $field;
                            }
                        elseif ($link_cri_pos) {
                            if (!isset($ret['V'][$view]['C'])) {
                                $ret['V'][$view]['C'] = array();
                                }
                            $ret['V'][$view]['C'][] = $link_field;
                            }
                        elseif ($formula_pos) {
                            if (!isset($ret['V'][$view]['F'])) {
                                $ret['V'][$view]['F'] = array();
                                }
                            $ret['V'][$view]['F'][] = $formula;
                            }
                        elseif ($sql_pos) {
                            if (!isset($ret['V'][$view]['Q'])) {
                                $ret['V'][$view]['Q'] = array();
                                }
                            $ret['V'][$view]['Q'][] = $sql;
                            }
                        }
                    // __________________ Check if expression is used in a action step ___
                    elseif ($step_pos) {
                        $step_pos+= 10;
                        $step     = substr($last,
                                           $step_pos,
                                           strpos($last, ')') - $step_pos);
                        // _____________________________ Look backward for action name ___
                        for ($i = count($lines); $i > 0; $i--) {
                            $line    = $lines[$i - 1];
                            $act_pos = strpos($line, 'function '.$prg_name.'��');
                            if ($act_pos !== false) {
                                $act = substr($line, $act_pos + $prg_len + 11, -16);
                                break;
                                }
                            }
                        if (!isset($ret['A'])) {
                            $ret['A'] = array();
                            }
                        if (!isset($ret['A'][$act])) {
                            $ret['A'][$act] = array();
                            }
                        // _________________________________ Add action step condition ___
                        $ret['A'][$act][] = $step;
                        }
                    // ___ Check if expression is used in a form or control definition ___
                    elseif ($in_form) {
                        if (!isset($ret['F'])) {
                            $ret['F'] = array();
                            }
                        if (!$form) {
                            // ___________________________ Look backward for form name ___
                            for ($i = count($lines); $i > 0; $i--) {
                                $line     = $lines[$i - 1];
                                $form_pos = strpos($line, 'function '.$prg_name.'��');
                                if ($form_pos !== false) {
                                    $form = substr($line,
                                                   $form_pos + $prg_len + 11,
                                                   strpos($line,
                                                          '_form_def(',
                                                          $form_pos + $prg_len + 11) -
                                                   $form_pos - $prg_len - 11);
                                    // ____________________________ Add form & control ___
                                    if (!isset($ret['F'][$form])) {
                                        $ret['F'][$form] = array();
                                        }
                                    $ret['F'][$form][] = $ctrl;
                                    break;
                                    }
                                }
                            }
                        else {
                            // ______________________________________________ Add form ___
                            if (!isset($ret['F'][$form])) {
                                $ret['F'][$form] = array();
                                }
                            $ret['F'][$form]['F'] = $form;
                            }
                        }
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads all actions on views (recordset::) and returns a list in the form:
     *
     * (view =>
     *    (<event> =>
     *       (act_name =>
     *          (step => <event>))))
     *
     * where <event> is INSERT|DELETE|SAVE|AGGREGATE|SNAPSHOT|BULK-INSERT|...
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_acts_view($prf_code) {

        $parts = array();
        $res = preg_match_all('/\$o2exe->s\((\d*)\)[^:]*o2act::view\("(\w*)",\s*"(\w*)",'.
                              '\s*([^&]*)\)\s*&&\s*\$o2exe->e\(\);/',
                              $prf_code, $parts, PREG_OFFSET_CAPTURE);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $offset    = $parts[0][$i][1];
                $step      = $parts[1][$i][0];
                $view_name = $parts[2][$i][0];
                $cmd       = $parts[3][$i][0];
                $pars      = $parts[4][$i][0];
                $pars_list = false;
                switch ($cmd) {
                    case "1": // ___________________________________ Goto first record ___
                    case "2": // ________________________________ Goto previous record ___
                    case "3": // ____________________________________ Goto next record ___
                    case "4": // ____________________________________ Goto last record ___
                    case "5": // __________________________________ Goto previous page ___
                    case "6": // ______________________________________ Goto next page ___
                        $event = 'MOVE';
                        break;
                    case "S": // ____________________________________ Action: POST ROW ___
                        $event = 'SAVE';
                        break;
                    case "A": // ________________________________________ Action: UNDO ___
                        $event = 'UNDO';
                        break;
                    case "I": // ______________________________ Action: PREPARE INSERT ___
                        $event = 'INSERT';
                        break;
                    case "C": // ______________________________________ Action: DELETE ___
                    case "D": // ____________________________________ Delete recordset ___
                        $event = 'DELETE';
                        break;
                    case "L": // ______________________________________ Action: LOCATE ___
                        $event     = 'LOCATE';
                        $pars_list = array();
                        $pars      = substr($pars, 6, -1);
                        foreach (explode(',', $pars) as $single_par) {
                            list($par_field, $par_exp) = explode('=>', $single_par);
                            $par_field = substr($par_field, 1, -1);
                            $par_exp   = substr($par_exp,
                                                strpos($par_exp, '_exp_') + 5,
                                                -2);
                            $pars_list[$par_field] = $par_exp;
                            }
                        break;
                    case "G": // _______________________________ Evaluate AGGREGATIONS ___
                        $event = 'AGGREGATE';
                        break;
                    case "K": // ______________________________________ Lock recordset ___
                        $event = 'LOCK';
                        break;
                    case "B": // _________________________ Action: BULK INSERT PREPARE ___
                    case "E": // ____________________________________ Action: BULK END ___
                        $event = 'BULK-INSERT';
                        break;
                    case "SS": // _____________________________________ SNAPSHOT Start ___
                    case "SM": // ______________________________________ SNAPSHOT Sync ___
                    case "SC": // _____________________________________ SNAPSHOT Clear ___
                        $event = 'SNAPSHOT';
                        break;
                    default: // ______________________________________ Action: REFRESH ___
                        $event = 'REFRESH';
                        break;
                    }
                // ________________________________________________ Search action name ___
                $action = self::get_action_before($prf_code, $offset);
                if (!$ret[$view_name]) {
                    $ret[$view_name] = array();
                    }
                if (!$ret[$view_name][$event]) {
                    $ret[$view_name][$event] = array();
                    }
                if (!$ret[$view_name][$event][$action]) {
                    $ret[$view_name][$event][$action] = array();
                    }
                // ____________________________________________ LOCATE with parameters ___
                if ($pars_list) {
                    $ret[$view_name][$event][$action][$step] = $pars_list;
                    }
                // ______________________________________________________ Other events ___
                else {
                    $ret[$view_name][$event][$action][$step] = $event;
                    }
                }
            }
        return $ret;

        }


    /**
     * Reads all UPDATE actions and returns a list in the form:
     *
     * (view =>
     *    (field_alias =>
     *       (act_name =>
     *          (step => <event>))))
     *
     * where <event> is UPDATE.
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_acts_update($prf_code) {

        $parts = array();
        $res = preg_match_all('/\$o2exe->s\((\d*)\)[^:]*o2act::set\("([^"]*)", "(\w*)",/',
                              $prf_code, $parts, PREG_OFFSET_CAPTURE);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $offset      = $parts[0][$i][1];
                $step        = $parts[1][$i][0];
                $view_name   = $parts[2][$i][0];
                $field_alias = $parts[3][$i][0];
                // ________________________________________________ Search action name ___
                $action      = self::get_action_before($prf_code, $offset);
                if (!$ret[$view_name]) {
                    $ret[$view_name] = array();
                    }
                if (!$ret[$view_name][$field_alias]) {
                    $ret[$view_name][$field_alias] = array();
                    }
                if (!$ret[$view_name][$field_alias][$action]) {
                    $ret[$view_name][$field_alias][$action] = array();
                    }
                $ret[$view_name][$field_alias][$action][$step] = 'UPDATE';
                }
            }
        return $ret;

        }


    /**
     * Reads all EXECUTE actions and returns a list in the form:
     *
     * (executed_action =>
     *    (action =>
     *       (step => ('C' => <end-cond>,
     *                 'V' => <loop-view>))))
     *
     * where <event> is UPDATE.
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_acts_exe($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$o2exe->s\((\d*)\)[^:]*o2act::exe\("(\w*)",\s*([^,]*)'.
                                ',\s*"(\w*)"/',
                                $prf_code, $parts, PREG_OFFSET_CAPTURE);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $offset    = $parts[0][$i][1];
                $step      = $parts[1][$i][0];
                $exe_act   = $parts[2][$i][0];
                $end_cond  = $parts[3][$i][0];
                $loop_view = $parts[4][$i][0];
                // ________________________________________________ Search action name ___
                $action    = self::get_action_before($prf_code, $offset);
                // ______________________________________________ Decode end-condition ___
                if (strpos($end_cond, '_exp_')) {
                    $end_cond = substr($end_cond, strrpos($end_cond, '_') + 1, -3);
                    }
                else {
                    $end_cond = strtolower($end_cond);
                    }
                if (!$ret[$exe_act]) {
                    $ret[$exe_act] = array();
                    }
                if (!$ret[$exe_act][$action]) {
                    $ret[$exe_act][$action] = array();
                    }
                $ret[$exe_act][$action][$step] = array('C' => $end_cond,
                                                       'V' => $loop_view);
                }
            }
        return $ret;

        }


    /**
     * Reads all CALL and GOTO actions and returns a list in the form:
     *
     * (prg_name =>
     *    (act_name =>
     *       (step =>
     *          ("call"|"goto" => (par1 => [exp],
     *                             par2 => <field>@<view>,
     *                             ... )))))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_acts_call($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$o2exe->s\((\d*)\)[^:]*o2act::(call|gotoprg)'.
                                '\("?([^,]*)("|\(\)),(.*)$/m',
                                $prf_code, $parts, PREG_OFFSET_CAPTURE);
        $ret = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $offset   = $parts[0][$i][1];
                $step     = $parts[1][$i][0];
                $type     = $parts[2][$i][0];
                $prg_name = ($parts[4][$i][0] == "()" ? "[exp]" : $parts[3][$i][0]);
                $action   = self::get_action_before($prf_code, $offset);
                if (!isset($ret[$prg_name])) {
                    $ret[$prg_name] = array();
                    }
                if (!isset($ret[$prg_name][$action])) {
                    $ret[$prg_name][$action] = array();
                    }
                if (!isset($ret[$prg_name][$action][$step])) {
                    $ret[$prg_name][$action][$step] = array();
                    }
                $pars     = array();
                $raw_pars = explode(",", substr($parts[5][$i][0], 0, -17));
                array_shift($raw_pars);
                foreach ($raw_pars as $par_pos => $raw_par) {
                    // _______________________________________ Parameter by expression ___
                    if (strpos($raw_par, "()")) {
                        $pars[$par_pos] = "[exp]";
                        }
                    // ________________________________________ Parameter by reference ___
                    else {
                        $raw_par = trim(trim(trim($raw_par), ")"), '"');
                        list($view, $field) = explode("��", $raw_par);
                        $pars[$par_pos] = $field."@".$view;
                        }
                    }
                $ret[$prg_name][$action][$step][$type] = $pars;
                }
            }
        return $ret;

        }


    /**
     * Reads all Return Parameter actions and returns a list in the form:
     *
     * (par_id =>
     *    (act_name =>
     *       (step => <event>)))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_acts_retpar($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$o2exe->s\((\d*)\)[^:]*o2act::ret\((\d*)/',
                                $prf_code, $parts, PREG_OFFSET_CAPTURE);
        $ret = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $offset = $parts[0][$i][1];
                $step   = $parts[1][$i][0];
                $par_id = $parts[2][$i][0];
                $action = self::get_action_before($prf_code, $offset);
                if (!isset($ret[$par_id])) {
                    $ret[$par_id] = array();
                    }
                if (!isset($ret[$par_id][$action])) {
                    $ret[$par_id][$action] = array();
                    }
                $ret[$par_id][$action][] = $step;
                }
            }
        return $ret;

        }


    /**
     * Reads all CONFIRM actions and returns a list in the form:
     *
     * (view =>
     *    (field_alias =>
     *       (act_name =>
     *          (step => <event>))))
     *
     * where <event> is CONFIRM.
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_acts_confirm($prf_code) {

        $parts = array();
        $res   = preg_match_all('/\$o2exe->s\((\d*)\)[^:]*o2act::confirm\("([^"]*)",\s*'.
                                '"(\w*)",\s*\w*_exp_(\d*)\(\)/',
                                $prf_code, $parts, PREG_OFFSET_CAPTURE);
        $ret   = array();
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $offset      = $parts[0][$i][1];
                $step        = $parts[1][$i][0];
                $view_name   = $parts[2][$i][0];
                $field_alias = $parts[3][$i][0];
                $msg_exp     = $parts[4][$i][0];
                // ________________________________________________ Search action name ___
                $action      = self::get_action_before($prf_code, $offset);
                if (!$ret[$view_name]) {
                    $ret[$view_name] = array();
                    }
                if (!$ret[$view_name][$field_alias]) {
                    $ret[$view_name][$field_alias] = array();
                    }
                if (!$ret[$view_name][$field_alias][$action]) {
                    $ret[$view_name][$field_alias][$action] = array();
                    }
                $ret[$view_name][$field_alias][$action][$step] = $msg_exp;
                }
            }
        return $ret;

        }


    /**
     * Reads program controls and returns a list in the form:
     *
     * (form =>
     *  (ctrl => (
     *   'T' => type,
     *   'P' => parent,
     *   'V' => view,
     *   'F' => field,
     *   'L' => list_view,
     *   'C' => list_code,
     *   'D' => list_desc)))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_ctrls($prf_code) {

        $ret   = array();
        // __________________________________________ Used as base field for a control ___
        $parts = array();
        $res   = preg_match_all('/\$ctrl_\w*\s*=\s*&\$form_(\w*)->ctrldef\("([^"]*)",\s*'.
                                '"([^"]*)",\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)"\);/',
                                $prf_code, $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $form   = $parts[1][$i];
                $ctrl   = $parts[2][$i];
                $type   = $parts[3][$i];
                $parent = $parts[4][$i];
                $view   = $parts[5][$i];
                $field  = $parts[6][$i];
                if (!isset($ret[$form])) {
                    $ret[$form] = array();
                    }
                $ret[$form][$ctrl] = array('T' => $type,
                                           'P' => $parent,
                                           'V' => $view,
                                           'F' => $field,
                                           'L' => false,
                                           'C' => false,
                                           'D' => false);
                }
            }
        // ______________________ Used in view2list() function (combo items from view) ___
        $parts = array();
        $res   = preg_match_all('/\$ctrl_(\w*)->valori\(o2_view2list\("([^"]*)",\s*'.
                                '"([^"]*)",\s*"([^"]*)",?\s*([^;]*)\)\);/',
                                $prf_code, $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $ctrl   = $parts[1][$i];
                $l_view = $parts[2][$i];
                $l_code = $parts[3][$i];
                $l_desc = $parts[4][$i];
                $l_exp  = $parts[5][$i];
                $parts2 = array();
                $res2   = preg_match('/\$ctrl_'.$ctrl.'\s*=\s*&\$form_(\w*)->ctrldef\("'.
                                     $ctrl.'",\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)",'.
                                     '\s*"([^"]*)"\);/', $prf_code, $parts2);
                $form   = $parts2[1];
                $type   = $parts2[2];
                $parent = $parts2[3];
                $view   = $parts2[4];
                $field  = $parts2[5];
                if (!isset($ret[$form])) {
                    $ret[$form] = array();
                    }
                $ret[$form][$ctrl] = array('T' => $type,
                                           'P' => $parent,
                                           'V' => $view,
                                           'F' => $field,
                                           'L' => $l_view,
                                           'C' => $l_code,
                                           'D' => $l_desc);
                }
            }
        return $ret;

        }


    /**
     * Reads program controls and returns a list used actions in the form:
     *
     * (action =>
     *  (ctrl => (type)))
     *
     * @param  string $prf_code
     * @return array
     */
    static function get_prg_ctrl_acts($prf_code) {

        $ret   = array();
        // _____________________________________________________ Used as submit-action ___
        $parts = array();
        $res   = preg_match_all('/\$ctrl_(\w*)->(azione|act_\w*)\("(\w*)"\);/',
                                $prf_code, $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $ctrl   = $parts[1][$i];
                $type   = $parts[2][$i];
                $action = $parts[3][$i];
                // _______________________________________________________ Decode type ___
                if ($type == 'azione') {
                    $type = 'main';
                    }
                else {
                    $type = substr($type, 4);
                    }
                // ________________________________________________ Look for ctrl form ___
                $parts2 = array();
                $res2   = preg_match('/\$ctrl_'.$ctrl.'\s*=\s*&\$form_(\w*)->ctrldef\("'.
                                     $ctrl.'",/', $prf_code, $parts2);
                $form   = $parts2[1];
                if (!isset($ret[$action])) {
                    $ret[$action] = array();
                    }
                if (!isset($ret[$action][$form])) {
                    $ret[$action][$form] = array();
                    }
                if (!isset($ret[$action][$form][$ctrl])) {
                    $ret[$action][$form][$ctrl] = array();
                    }
                $ret[$action][$form][$ctrl][] = $type;
                }
            }
        return $ret;

        }


    public function find_model($data_type) {

        $root     = "[root]";
        $ret      = array();
        $id_var   = 0;
        $id_tab   = 0;
        $id_fld   = 0;
        $id_prg   = 0;
        $id_view  = 0;
        $id_alias = 0;
        // _____________________________ Loop on application variables from repository ___
        foreach ($this->get_variables() as $var => $model) {
            $id_var++;
            if ($model == $data_type) {
                $ret["v".$id_var] = array($root,
                                          $var,
                                          $this->img_path."var.png",
                                          self::$w_var);
                }
            }
        // ______________________________ Loop on tables fields models from repository ___
        $models   = $this->get_tabs_models();
        if (isset($models[$data_type])) {
            foreach ($models[$data_type] as $table => $fields) {
                $id_tab++;
                $ret["t".$id_tab] = array($root,
                                          $table,
                                          $this->img_path."tab.png",
                                          self::$w_table);
                foreach ($fields as $field) {
                    $id_fld++;
                    $ret["t".$id_tab."f".$id_fld] = array("t".$id_tab,
                                                          $field,
                                                          $this->img_path."field.png",
                                                          self::$w_field);
                    }
                }
            }
        // __________________________________________________________ Loop on programs ___
        foreach ($this->prgs_list as $prg => $prf) {
            $id_prg++;
            $code = file_get_contents($prf);
            // _______________________________________ Add program parameters by model ___
            foreach (self::get_prg_params($prf) as $param => $model) {
                if ($model == $data_type) {
                    if (!isset($ret["p".$id_prg])) {
                        $ret["p".$id_prg] = array($root,
                                                  $prg,
                                                  $this->img_path."prg.png",
                                                  self::$w_program);
                        }
                    $ret["p".$id_prg."r".$param] = array("p".$id_prg,
                                                         $param,
                                                         $this->img_path."par.png",
                                                         self::$w_par);
                    }
                }
            // ________________________________________ Add program variables by model ___
            foreach (self::get_prg_variables($code) as $variable => $model) {
                if ($model == $data_type) {
                    if (!isset($ret["p".$id_prg])) {
                        $ret["p".$id_prg] = array($root,
                                                  $prg,
                                                  $this->img_path."prg.png",
                                                  self::$w_program);
                        }
                    $ret["p".$id_prg."l".$variable] = array("p".$id_prg,
                                                            $variable,
                                                            $this->img_path."var.png",
                                                            self::$w_var);
                    }
                }
            // _________________________________ Add program expressions by o2format() ___
            $exps = self::get_prg_exps($code, $prg);
            $res  = preg_grep('/o2format\(.*["\']'.$data_type.'["\']/', $exps);
            if (count($res)) {
                if (!isset($ret["p".$id_prg])) {
                    $ret["p".$id_prg] = array($root,
                                              $prg,
                                              $this->img_path."prg.png",
                                              self::$w_program);
                    }
                foreach ($res as $exp_id => $exp_body) {
                    $ret["p".$id_prg."e".$exp_id] = array("p".$id_prg,
                                                          $exp_id,
                                                          $this->img_path."exp.png",
                                                          self::$w_expression);
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find uses for model "'.$data_type.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find uses for model "'.$data_type.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_app_var($var_name) {

        $root     = "[root]";
        $ret      = array();
        $id_tab   = 0;
        $id_fld   = 0;
        $id_prg   = 0;
        $id_view  = 0;
        $id_alias = 0;
        // __________________________________________________________ Loop on programs ___
        foreach ($this->prgs_list as $prg => $prf) {
            $id_prg++;
            $code = file_get_contents($prf);
            // _______________________________________ Add program actions by variable ___
            foreach (self::get_prg_acts_update($code) as $view => $variable) {
                if ($view == '_o2SESSION') {
                    foreach ($variable as $name => $action) {
                        if ($name == $var_name) {
                            foreach ($action as $act_name => $step) {
                                if (!isset($ret["p".$id_prg])) {
                                    $ret["p".$id_prg] = array($root,
                                                              $prg,
                                                              $this->img_path."prg.png",
                                                              self::$w_program);
                                    }
                                if (!isset($ret["p".$id_prg."a".$act_name])) {
                                    $ret["p".$id_prg."a".$act_name] = array('p'.$id_prg,
                                                                         $act_name,
                                                                         $this->img_path.
                                                                         "action.png",
                                                                         self::$w_action);
                                    }
                                foreach ($step as $step_n => $step_act) {
                                    $key       = "p".$id_prg."a".$act_name.'s'.$step;
                                    $ret[$key] = array('p'.$id_prg.'a'.$act_name,
                                                       $step_n.' '.$step_act,
                                                       $this->img_path."step.png",
                                                       self::$w_step);
                                    }
                                }
                            }
                        }
                    }
                }
            // _________________ Find uses in expressions (o2val(), o2zero(), o2pre()) ___
            $exps = self::get_prg_exps($code, $prg);
            foreach ($exps as $exp_id => $body) {
                $parts = array();
                $res   = preg_match('/o2(\w+)\s*\(\s*["\']_o2SESSION["\']\s*,\s*["\']'.
                                    $var_name.'["\']\s*\)/',
                                    $body, $parts);
                if (count($parts[0])) {
                    if (strpos($parts[1], "_") === false) {
                        if (!isset($ret['p'.$id_prg])) {
                            $ret['p'.$id_prg] = array($root,
                                                      $prg,
                                                      $this->img_path."prg.png",
                                                      self::$w_program);
                            }
                        $ret['p'.$id_prg."e".$exp_id] = array('p'.$id_prg,
                                                              $exp_id,
                                                              $this->img_path."exp.png",
                                                              self::$w_expression);
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find uses for variable "'.$var_name.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find uses for variable "'.$var_name.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_table($table) {

        $root     = "[root]";
        $ret      = array();
        $id_prg   = 0;
        $id_view  = 0;
        $id_alias = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code = file_get_contents($prf);
            $tabs = self::get_prg_tabs($code);
            if (isset($tabs[$table])) {
                $id_prg++;
                foreach ($tabs[$table] as $view_name => $tab_aliases) {
                    $id_view++;
                    foreach ($tab_aliases as $tab_alias => $tab_info) {
                        $id_alias++;
                        $ret["p".$id_prg]              = array($root,
                                                               $prg,
                                                               $this->img_path."prg.png",
                                                               self::$w_program);
                        $ret["p".$id_prg."v".$id_view] = array("p".$id_prg,
                                                               $view_name,
                                                               $this->img_path."view.png",
                                                               self::$w_view);
                        $ret["p".$id_prg."v".$id_view."f".$id_alias] =
                                                           array("p".$id_prg."v".$id_view,
                                                                 $tab_alias,
                                                                $this->img_path."tab.png",
                                                                 self::$w_table);
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find uses for table "'.$table.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find uses for table "'.$table.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_table_event($table, $event) {

        $root    = "[root]";
        $ret     = array();
        $id_prg  = 0;
        $id_view = 0;
        $id_act  = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code   = file_get_contents($prf);
            $tabs   = self::get_prg_tabs($code);
            $events = self::get_prg_acts_view($code);
            if (isset($tabs[$table])) {
                $id_prg++;
                foreach ($tabs[$table] as $view_name => $tab_aliases) {
                    foreach ($tab_aliases as $tab_alias => $tab_info) {
                        if ($tab_info["main"]) {
                            $id_view++;
                            if ($events[$view_name]) {
                                foreach ($events[$view_name] as $type => $actions) {
                                    if ($type == $event) {
                                        foreach ($actions as $action => $steps) {
                                            $id_act++;
                                            foreach ($steps as $step => $event) {
                                                $i_prg = array($root,
                                                               $prg,
                                                               $this->img_path."prg.png",
                                                               self::$w_program);
                                                $ret["p".$id_prg] = $i_prg;
                                                $i_view = array("p".$id_prg,
                                                                $view_name,
                                                               $this->img_path."view.png",
                                                                self::$w_view);
                                                $ret["p".$id_prg."v".$id_view] = $i_view;
                                                $i_act = array("p".$id_prg."v".$id_view,
                                                               $action,
                                                             $this->img_path."action.png",
                                                               self::$w_action);
                                                $ret["p".$id_prg."v".$id_view.
                                                     "a".$id_act] = $i_act;
                                                $i_step = array("p".$id_prg."v".$id_view.
                                                                "a".$id_act,
                                                                $step,
                                                               $this->img_path."step.png",
                                                                self::$w_step);
                                                $ret["p".$id_prg."v".$id_view."a".$id_act.
                                                     "s".$step] = $i_step;

                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find '.$event.'s for table "'.$table.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find '.$event.'s for table "'.$table.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_table_field($table, $field) {

        $root     = "[root]";
        $ret      = array();
        $id_prg   = 0;
        $id_view  = 0;
        $id_alias = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code = file_get_contents($prf);
            $tabs = self::get_prg_tabs($code);
            if (isset($tabs[$table])) {
                $id_prg++;
                $sels = self::get_prg_selects($code);
                foreach ($tabs[$table] as $view_name => $tab_aliases) {
                    $id_view++;
                    foreach ($tab_aliases as $tab_alias => $tab_info) {
                        if (isset($sels[$view_name][$tab_alias][$field])) {
                            $id_alias++;
                            $ret["p".$id_prg]              = array($root,
                                                                   $prg,
                                                                $this->img_path."prg.png",
                                                                   self::$w_program);
                            $ret["p".$id_prg."v".$id_view] = array("p".$id_prg,
                                                                   $view_name,
                                                               $this->img_path."view.png",
                                                                   self::$w_view);
                            $ret["p".$id_prg."v".$id_view."f".$id_alias] =
                                                           array("p".$id_prg."v".$id_view,
                                                                 $tab_alias,
                                                                $this->img_path."tab.png",
                                                                 self::$w_table);
                            }
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find uses for field "'.$field.
                                     '" from table "'.$table.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find uses for field "'.$field.
                                           '" from table "'.$table.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_table_key($table, $key_name) {

        $root     = "[root]";
        $ret      = array();
        $id_prg   = 0;
        $id_view  = 0;
        $id_alias = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code = file_get_contents($prf);
            $tabs = self::get_prg_tabs($code);
            if (isset($tabs[$table])) {
                $id_prg++;
                foreach ($tabs[$table] as $view_name => $tab_aliases) {
                    $id_view++;
                    foreach ($tab_aliases as $tab_alias => $tab_info) {
                        if ($tab_info["key"] == $key_name ||
                            ($key_name == "[exp]" &&  $tab_info["keyexp"])) {
                            $id_alias++;
                            $ret["p".$id_prg]              = array($root,
                                                                   $prg,
                                                                $this->img_path."prg.png",
                                                                   self::$w_program);
                            $ret["p".$id_prg."v".$id_view] = array("p".$id_prg,
                                                                   $view_name,
                                                               $this->img_path."view.png",
                                                                   self::$w_view);
                            $ret["p".$id_prg."v".$id_view."f".$id_alias] =
                                                           array("p".$id_prg."v".$id_view,
                                                                 $tab_alias,
                                                                $this->img_path."tab.png",
                                                                 self::$w_table);
                            }
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find uses for key "'.$key_name.
                                     '" from table "'.$table.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find uses for key "'.$key_name.
                                           '" from table "'.$table.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_act_field_update($table, $field) {

        $root    = "[root]";
        $ret     = array();
        $id_prg  = 0;
        $id_view = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code = file_get_contents($prf);
            $tabs = self::get_prg_tabs($code);
            if (isset($tabs[$table])) {
                $id_prg++;
                $sels = self::get_prg_selects($code);
                $upds = self::get_prg_acts_update($code);
                // ________________________________ Loop on tables uses in prg by view ___
                foreach ($tabs[$table] as $view_name => $tab_aliases) {
                    $id_view++;
                    // ____________________________ Loop on table-aliases in each view ___
                    foreach ($tab_aliases as $tab_alias => $tab_info) {
                        // _______________ If SELECT exists in prg for requested field ___
                        if (isset($sels[$view_name][$tab_alias][$field])) {
                            $fld_sel = $sels[$view_name][$tab_alias][$field];
                            // _____ If UPDATE exists in prg for req. field (by alias) ___
                            if (isset($upds[$view_name][$fld_sel["alias"]])) {
                                $ret["p".$id_prg] = array($root,
                                                          $prg,
                                                          $this->img_path."prg.png",
                                                          self::$w_program);
                                $ret["p".$id_prg."v".$id_view] = array("p".$id_prg,
                                                                       $view_name,
                                                                       $this->img_path.
                                                                       "view.png",
                                                                       self::$w_view);
                                $fld_upd = $upds[$view_name][$fld_sel["alias"]];
                                // ________ Loop on actions with UPDATEs on req. field ___
                                foreach ($fld_upd as $action => $steps_list) {
                                    $ret["p".$id_prg."v".$id_view."a".$action] =
                                                         array("p".$id_prg."v".$id_view,
                                                               $action,
                                                             $this->img_path."action.png",
                                                               self::$w_action);
                                    // __________ Loop on UPDATEs steps of each action ___
                                    foreach ($steps_list as $step => $event) {
                                        $idx       = "p".$id_prg."v".$id_view.
                                                     "a".$action."s".$step;
                                        $ret[$idx] = array("p".$id_prg."v".$id_view.
                                                           "a".$action,
                                                           $step,
                                                           $this->img_path."step.png",
                                                           self::$w_step);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find UPDATEs for field "'.$field.
                                     '" from table "'.$table.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find UPDATEs for field "'.$field.
                                           '" from table "'.$table.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_prg($prg_name) {

        $root = "[root]";
        $ret  = array();
        // _________________________________________________ Look for program in menus ___
        $res = $this->get_menu_prgs();
        if (count($res)) {
            foreach ($res as $prg => $menu_list) {
                foreach ($menu_list as $menu_id => $menu) {
                    if ($prg == $prg_name) {
                        $ret["m".$menu_id] = array($root,
                                                   $menu,
                                                   $this->img_path."menu.png",
                                                   self::$w_menu);
                        }
                    }
                }
            }
        // ______________________________________ Look for CALLs and GOTOs in programs ___
        $id_prg   = 0;
        $id_act   = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code  = file_get_contents($prf);
            $calls = self::get_prg_acts_call($code);
            if (isset($calls[$prg_name])) {
                $id_prg++;
                // ___________________________________________ Loop on program actions ___
                foreach ($calls[$prg_name] as $action => $steps) {
                    $id_act++;
                    $ret["p".$id_prg]             = array($root,
                                                          $prg,
                                                          $this->img_path."prg.png",
                                                          self::$w_program);
                    $ret["p".$id_prg."a".$id_act] = array("p".$id_prg,
                                                          $action,
                                                          $this->img_path."action.png",
                                                          self::$w_action);
                    foreach ($steps as $step => $type) {
                        $instr = array("p".$id_prg."a".$id_act,
                                       $step,
                                       $this->img_path."step.png",
                                       self::$w_step);
                        $ret["p".$id_prg."a".$id_act."s".$step] = $instr;
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find uses for program "'.$prg_name.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find uses for program "'.$prg_name.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_field_as_param($table, $field) {

        $root    = "[root]";
        $ret     = array();
        $id_prg  = 0;
        $id_view = 0;
        $id_act  = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code = file_get_contents($prf);
            $tabs = self::get_prg_tabs($code);
            if (isset($tabs[$table])) {
                $id_prg++;
                $sels  = self::get_prg_selects($code);
                $calls = self::get_prg_acts_call($code);
                // ________________________________ Loop on tables uses in prg by view ___
                foreach ($tabs[$table] as $view_name => $tab_aliases) {
                    $id_view++;
                    // ____________________________ Loop on table-aliases in each view ___
                    foreach ($tab_aliases as $tab_alias => $tab_info) {
                        // _________________________ If SELECT exists from a main table___
                        if ($tab_info["main"] &&
                            isset($sels[$view_name][$tab_alias][$field])) {
                            $fld_param = $sels[$view_name][$tab_alias][$field]["alias"].
                                         "@".$view_name;
                            foreach ($calls as $called_prg => $actions) {
                                foreach ($actions as $action => $steps) {
                                    $id_act++;
                                    // __________________________ $types are CALL|GOTO ___
                                    foreach ($steps as $step => $types) {
                                        foreach ($types as $type => $params) {
                                            foreach ($params as $par_id => $param) {
                                                if ($param == $fld_param) {
                                                    // _______________________ Program ___
                                                    $l1_id = "p".$id_prg;
                                                    $l1    = array($root,
                                                                   $prg,
                                                                $this->img_path."prg.png",
                                                                   self::$w_program);
                                                    $ret[$l1_id] = $l1;
                                                    // __________________________ View ___
                                                    $l2_id = $l1_id."v".$id_view;
                                                    $l2    = array($l1_id,
                                                                   $view_name,
                                                               $this->img_path."view.png",
                                                                   self::$w_view);
                                                    $ret[$l2_id] = $l2;
                                                    // ________________________ Action ___
                                                    $l3_id = $l2_id."a".$id_act;
                                                    $l3    = array($l2_id,
                                                                   $action,
                                                             $this->img_path."action.png",
                                                                   self::$w_action);
                                                    $ret[$l3_id] = $l3;
                                                    // __________________________ Step ___
                                                    $l4_id = $l3_id."s".$step;
                                                    $l4    = array($l3_id,
                                                                   $step." ".$called_prg,
                                                               $this->img_path."step.png",
                                                                   self::$w_step);
                                                    $ret[$l4_id] = $l4;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0,
                                'Find PARAM-BY-REFs for field "'.$field.
                                     '" from table "'.$table.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find PARAM-BY-REFs for field "'.$field.
                                           '" from table "'.$table.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_string($search) {

        $root   = "[root]";
        $ret    = array();
        $id_prg = 0;
        $cnt    = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $code = file_get_contents($prf);
            $exps = self::get_prg_exps($code, $prg);
            $res  = preg_grep('/'.preg_quote($search, '/').'/', $exps);
            if (count($res)) {
                $id_prg++;
                $ret["p".$id_prg] = array($root,
                                          $prg,
                                          $this->img_path."prg.png",
                                          self::$w_program);
                foreach ($res as $exp_id => $exp_body) {
                    $ret["p".$id_prg."e".$exp_id] = array("p".$id_prg,
                                                          $exp_id,
                                                          $this->img_path."exp.png",
                                                          self::$w_expression);
                    $cnt++;
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find occurences of string "'.$search.'" ('.$cnt.')',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find occurences of string "'.$search.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_duplicated_exps() {

        $root   = "[root]";
        $ret    = array();
        $id_prg = 0;
        foreach ($this->prgs_list as $prg => $prf) {
            $id_prg++;
            $code = file_get_contents($prf);
            $exps = self::get_prg_exps($code, $prg);
            foreach ($exps as $exp_id => $exp_body) {
                $exps_copy = $exps;
                unset($exps_copy[$exp_id]);
                if (array_search($exp_body, $exps_copy)) {
                    $ret["p".$id_prg] = array($root,
                                              $prg,
                                              $this->img_path."prg.png",
                                              self::$w_program);
                    $ret["p".$id_prg."e".$exp_id] = array("p".$id_prg,
                                                          $exp_id,
                                                          $this->img_path."exp.png",
                                                          self::$w_expression);
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find duplicated expressions',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find duplicated expressions',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_prg_duplicated_exps($prg) {

        $root = "[root]";
        $ret  = array();
        $code = file_get_contents($this->prgs_list[$prg]);
        $exps = self::get_prg_exps($code, $prg);
        foreach ($exps as $exp_id => $exp_body) {
            $exps_copy = $exps;
            unset($exps_copy[$exp_id]);
            if (array_search($exp_body, $exps_copy)) {
                $ret["prg"] = array($root,
                                    $prg,
                                    $this->img_path."prg.png",
                                    self::$w_program);
                $ret["prg".$exp_id] = array("prg",
                                            $exp_id,
                                            $this->img_path."exp.png",
                                            self::$w_expression);
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find duplicated expressions in program "'.$prg.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find duplicated expressions in program "'.
                                           $prg.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root, "No matches found"));
            }
        return $ret;

        }


    public function find_prg_par($prg, $par_id) {

        $root = "[root]";
        $ret  = array();
        $prf  = $this->prgs_list[$prg];
        $code = file_get_contents($prf);
        $uses = self::get_prg_acts_retpar($code);
        // ________________________________________ Uses in actions (return parameter) ___
        if (count($uses[$par_id])) {
            $ret["p"] = array($root,
                              $prg,
                              $this->img_path."prg.png",
                              self::$w_program);
            foreach ($uses[$par_id] as $act => $in_act) {
                $ret["a".$act] = array("p",
                                       $act,
                                       $this->img_path."action.png",
                                       self::$w_action);
                foreach ($in_act as $step) {
                    $ret["a".$act."s".$step] = array("a".$act,
                                                     $step,
                                                     $this->img_path."step.png",
                                                     self::$w_step);
                    }
                }
            }
        // ________________________________________ Find uses in expressions (o2par()) ___
        $exps = self::get_prg_exps($code, $prg);
        foreach ($exps as $exp_id => $body) {
            if (strpos($body, "o2par(".$par_id.")")) {
                if (!isset($ret['p'])) {
                    $ret["p"] = array($root,
                                      $prg,
                                      $this->img_path."prg.png",
                                      self::$w_program);
                    }
                $ret["e".$exp_id] = array("p",
                                          $exp_id,
                                          $this->img_path."exp.png",
                                          self::$w_expression);
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find occurences of parameter ('.$par_id.
                                   ') in program "'.$prg.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find occurences of parameter ('.$par_id.
                                           ') in program "'.$prg.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root,
                                           "No matches found: parameter is not used"));
            }
        return $ret;

        }


    public function find_prg_act($prg, $action) {

        $root = "[root]";
        $ret  = array();
        $prf  = $this->prgs_list[$prg];
        $code = file_get_contents($prf);
        // _________________________________________ Used as prefix or suffix in views ___
        $views = self::get_prg_views($prf);
        if (count($views)) {
            foreach ($views as $view => $triggers) {
                if ($triggers['prefix'] == $action || $triggers['suffix'] == $action) {
                    if (!isset($ret['p'])) {
                        $ret['p'] = array($root,
                                          $prg,
                                          $this->img_path.'prg.png',
                                          self::$w_program);
                        }
                    $ret['v'.$view] = array('p',
                                            $view,
                                            $this->img_path.'view.png',
                                            self::$w_view);
                    }
                }
            }
        // ______________________________________________ Uses as program start action ___
        $start_act = self::get_prg_start_act($prf);
        if ($start_act && $start_act == $action) {
            if (!isset($ret['p'])) {
                $ret['p'] = array($root,
                                  $prg,
                                  $this->img_path.'prg.png',
                                  self::$w_program);
                }
            $ret['astart'] = array('p',
                                   '[Start action]',
                                   $this->img_path.'action.png',
                                   self::$w_action);
            }
        // ____________________________________________________ Uses in execute action ___
        $exe_acts = self::get_prg_acts_exe($code);
        if ($exe_acts[$action]) {
            foreach ($exe_acts[$action] as $act => $steps) {
                foreach ($steps as $step => $obj) {
                    if (!isset($ret['p'])) {
                        $ret['p'] = array($root,
                                          $prg,
                                          $this->img_path.'prg.png',
                                          self::$w_program);
                        }
                    if (!isset($ret['a'.$act])) {
                        $ret['a'.$act] = array('p',
                                               $act,
                                               $this->img_path.'action.png',
                                               self::$w_action);
                        }
                    $ret['a'.$act.'s'.$step] = array('a'.$act,
                                                     $step.' EXECUTE',
                                                     $this->img_path.'step.png',
                                                     self::$w_step);
                    }
                }
            }
        // ___________________________________ Used as refresh or close action on form ___
        $forms = self::get_prg_form_acts($code);
        if (count($forms)) {
            foreach ($forms as $form => $acts) {
                if ($acts['refresh-act'] == $action) {
                    if (!isset($ret['p'])) {
                        $ret['p'] = array($root,
                                          $prg,
                                          $this->img_path.'prg.png',
                                          self::$w_program);
                        }
                    if (!isset($ret['f'.$form])) {
                        $ret['f'.$form] = array('p',
                                                $form,
                                                $this->img_path.'form.png',
                                                self::$w_form);
                        }
                    $ret['f'.$form.'arefresh'] = array('f'.$form,
                                                       '[Refresh action]',
                                                       $this->img_path.'action.png',
                                                       self::$w_action);
                    }
                if ($acts['close-act'] == $action) {
                    if (!isset($ret['p'])) {
                        $ret['p'] = array($root,
                                          $prg,
                                          $this->img_path.'prg.png',
                                          self::$w_program);
                        }
                    if (!isset($ret['f'.$form])) {
                        $ret['f'.$form] = array('p',
                                                $form,
                                                $this->img_path.'form.png',
                                                self::$w_form);
                        }
                    $ret['f'.$form.'aclose'] = array('f'.$form,
                                                     '[Close action]',
                                                     $this->img_path.'action.png',
                                                     self::$w_action);
                    }
                }
            }
        // ________________________________________________________ Used on form ctrls ___
        $ctrls = self::get_prg_ctrl_acts($code);
        if ($ctrls[$action]) {
            if (!isset($ret['p'])) {
                $ret['p'] = array($root,
                                  $prg,
                                  $this->img_path.'prg.png',
                                  self::$w_program);
                }
            foreach ($ctrls[$action] as $form => $obj) {
                if (!isset($ret['f'.$form])) {
                    $ret['f'.$form] = array('p',
                                            $form,
                                            $this->img_path.'form.png',
                                            self::$w_form);
                    }
                foreach ($obj as $ctrl => $acts) {
                    if (!isset($ret['f'.$form.'c'.$ctrl])) {
                        $ret['f'.$form.'c'.$ctrl] = array('f'.$form,
                                                          $ctrl,
                                                          $this->img_path.'ctrl.png',
                                                          self::$w_ctrl);
                        }
                    foreach ($acts as $type) {
                        $ret['f'.$form.'c'.$ctrl.'t'.$type] = array('f'.$form.'c'.$ctrl,
                                                             strtoupper($type).' action',
                                                             $this->img_path.'action.png',
                                                             self::$w_action);
                        }
                    }
                }
            }
        // _______________________________________________________ Used in o2_act4js() ___
        $exps = self::get_prg_exps($code, $prg);
        foreach ($exps as $exp => $body) {
            if (strpos($body, 'o2_act4js(')) {
                $parts = array();
                $funcs = preg_match_all('/o2_act4js\([^,]*\s*,\s*["\'](\w*)["\']/',
                                        $body, $parts);
                foreach ($parts[1] as $act) {
                    if ($act == $action) {
                        if (!isset($ret['p'])) {
                            $ret['p'] = array($root,
                                              $prg,
                                              $this->img_path.'prg.png',
                                              self::$w_program);
                            }
                        $ret['e'.$exp] = array('p',
                                               $exp.' [o2_act4js()]',
                                               $this->img_path.'exp.png',
                                               self::$w_expression);
                        }
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find occurences of action ('.$action.
                                   ') in program "'.$prg.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find occurences of action ('.$action.
                                           ') in program "'.$prg.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root,
                                           "No matches found: action is not used"));
            }
        return $ret;

        }


    public function find_prg_field($prg, $view, $field) {

        $root = "[root]";
        $ret  = array();
        $prf  = $this->prgs_list[$prg];
        $code = file_get_contents($prf);
        // ============================================================= Uses in views ===
        // _____________________________ Used as aggregation (field or target) in view ___
        $uses = self::get_prg_aggregates($code);
        foreach ($uses as $aggr_view => $aggr_fields) {
            foreach ($aggr_fields as $aggr_field => $aggr_types) {
                foreach ($aggr_types as $aggr_type => $aggr_var) {
                    // ______________________________________ View field is aggregated ___
                    if ($aggr_view == $view && $aggr_field == $field) {
                        if (!isset($ret['p'])) {
                            $ret['p'] = array($root,
                                              $prg,
                                              $this->img_path.'prg.png',
                                              self::$w_program);
                            }
                        if (!isset($ret['v'.$view])) {
                            $ret['v'.$view] = array('p',
                                                    $view,
                                                    $this->img_path.'view.png',
                                                    self::$w_view);
                            }
                        $key = 'v'.$view.'f'.$field.'t'.$aggr_type;
                        $ret[$key] = array('v'.$view,
                                           $aggr_type,
                                           $this->img_path.'aggr.png',
                                           self::$w_aggregate);
                        }
                    // ________________________ Variable is aggregation target in view ___
                    if ($view == 'prg�_�var' && $aggr_var == $field) {
                        if (!isset($ret['p'])) {
                            $ret['p'] = array($root,
                                              $prg,
                                              $this->img_path.'prg.png',
                                              self::$w_program);
                            }
                        if (!isset($ret['v'.$aggr_view])) {
                            $ret['v'.$aggr_view] = array('p',
                                                         $aggr_view,
                                                         $this->img_path.'view.png',
                                                         self::$w_view);
                            }
                        $ret['v'.$aggr_view.'f'.$field] = array('v'.$aggr_view,
                                                                $aggr_type.' ('.
                                                                $aggr_field.')',
                                                               $this->img_path.'aggr.png',
                                                                self::$w_aggregate);
                        }
                    }
                }
            }
        // _____________________________________________ Used in a SQL-formula in view ___
        if ($view != 'prg�_�var') {
            $parts = array();
            $res   = preg_match_all('/\$task_'.$view.
                                    '->sql_formula\(["\'](\w+)["\'][^;]*\[["\']'.$field.
                                    '["\']\][^;]*\);/',
                                    $code, $parts);
            if (isset($parts[0])) {
                if (!isset($ret['v'.$view])) {
                    $ret['v'.$view] = array('p',
                                            $view,
                                            $this->img_path.'view.png',
                                            self::$w_view);
                    }
                foreach ($parts[1] as $sql_name) {
                    $ret['v'.$view.'f'.$sql_name] = array('v'.$view,
                                                          $sql_name.' (SQL formula)',
                                                          $this->img_path.'field.png',
                                                          self::$w_field);
                    }
                }
            }
        // =========================================================== Uses in actions ===
        // _________________________________ Used as parameter in CALL or GOTO actions ___
        $uses = self::get_prg_acts_call($code);
        foreach ($uses as $acts) {
            foreach ($acts as $act => $steps) {
                foreach ($steps as $step => $call) {
                    foreach ($call as $call_goto => $pars) {
                        foreach ($pars as $par_id => $par) {
                            if ($par == $field.'@'.$view) {
                                if (!isset($ret['p'])) {
                                    $ret['p'] = array($root,
                                                      $prg,
                                                      $this->img_path.'prg.png',
                                                      self::$w_program);
                                    }
                                if (!isset($ret['a'.$act])) {
                                    $ret['a'.$act] = array('p',
                                                           $act,
                                                           $this->img_path.'action.png',
                                                           self::$w_action);
                                    }
                                $ret['a'.$act.'s'.$step] = array('a'.$act,
                                                                 $step.' '.
                                                                 strtoupper($call_goto).
                                                                 ' (par '.($par_id + 1).
                                                                 ')',
                                                                 $this->img_path.
                                                                 'step.png',
                                                                 self::$w_step);
                                }
                            }
                        }
                    }
                }
            }
        // ____________________________________________________ Used in UPDATE actions ___
        $uses = self::get_prg_acts_update($code);
        if (isset($uses[$view]) && isset($uses[$view][$field])) {
            foreach ($uses[$view][$field] as $act => $steps) {
                foreach ($steps as $step => $update) {
                    if (!isset($ret['p'])) {
                        $ret['p'] = array($root,
                                          $prg,
                                          $this->img_path.'prg.png',
                                          self::$w_program);
                        }
                    if (!isset($ret['a'.$act])) {
                        $ret['a'.$act] = array('p',
                                               $act,
                                               $this->img_path.'action.png',
                                               self::$w_action);
                        }
                    $ret['a'.$act.'s'.$step] = array('a'.$act,
                                                     $step.' UPDATE',
                                                     $this->img_path.'step.png',
                                                     self::$w_step);
                    }
                }
            }
        // ____________________________________________________ Used in LOCATE actions ___
        $uses = self::get_prg_acts_view($code);
        if (isset($uses[$view]) && isset($uses[$view]['LOCATE'])) {
            foreach ($uses[$view]['LOCATE'] as $act => $steps) {
                foreach ($steps as $step => $pars) {
                    foreach ($pars as $par_field => $par_exp) {
                        if ($par_field == $field) {
                            if (!isset($ret['p'])) {
                                $ret['p'] = array($root,
                                                  $prg,
                                                  $this->img_path.'prg.png',
                                                  self::$w_program);
                                }
                            if (!isset($ret['a'.$act])) {
                                $ret['a'.$act] = array('p',
                                                       $act,
                                                       $this->img_path.'action.png',
                                                       self::$w_action);
                                }
                            $ret['a'.$act.'s'.$step] = array('a'.$act,
                                                             $step.' LOCATE',
                                                             $this->img_path.'step.png',
                                                             self::$w_step);
                            }
                        }
                    }
                }
            }
        // ___________________________________________________ Used in CONFIRM actions ___
        $uses = self::get_prg_acts_confirm($code);
        if (isset($uses[$view]) && isset($uses[$view][$field])) {
            foreach ($uses[$view][$field] as $act => $steps) {
                foreach ($steps as $step => $msg_exp) {
                    if (!isset($ret['p'])) {
                        $ret['p'] = array($root,
                                          $prg,
                                          $this->img_path.'prg.png',
                                          self::$w_program);
                        }
                    if (!isset($ret['a'.$act])) {
                        $ret['a'.$act] = array('p',
                                               $act,
                                               $this->img_path.'action.png',
                                               self::$w_action);
                        }
                    $ret['a'.$act.'s'.$step] = array('a'.$act,
                                                     $step.' CONFIRM',
                                                     $this->img_path.'step.png',
                                                     self::$w_step);
                    }
                }
            }
        // _____________________________________________________ Find uses in controls ___
        $ctrls = self::get_prg_ctrls($code);
        if (count($ctrls)) {
            foreach ($ctrls as $form => $form_ctrls) {
                foreach ($form_ctrls as $ctrl => $obj) {
                    if (($obj['V'] == $view && $obj['F'] == $field) ||
                        ($obj['L'] == $view &&
                         ($obj['C'] == $field || $obj['D'] == $field))) {
                        if (!isset($ret['p'])) {
                            $ret['p'] = array($root,
                                              $prg,
                                              $this->img_path."prg.png",
                                              self::$w_program);
                            }
                        if (!isset($ret['f'.$form])) {
                            $ret['f'.$form] = array('p',
                                                    $form,
                                                    $this->img_path."form.png",
                                                    self::$w_form);
                            }
                        $ret['c'.$ctrl] = array('f'.$form,
                                                $ctrl,
                                                $this->img_path."ctrl.png",
                                                self::$w_ctrl);
                        }
                    }
                }
            }
        // ____________________________________________________ Find uses in protocols ___
        $prot = self::get_prg_protocols($code);
        foreach ($prot as $protocol => $views) {
            if (isset($views[$view])) {
                foreach ($views[$view] as $alias => $obj) {
                    if (isset($obj[$field])) {
                        if (!isset($ret['p'])) {
                            $ret["p"] = array($root,
                                              $prg,
                                              $this->img_path."prg.png",
                                              self::$w_program);
                            }
                        if (!isset($ret['pr'.$protocol])) {
                            $ret['pr'.$protocol] = array('p',
                                                         $protocol,
                                                         $this->img_path."io.png",
                                                         self::$w_protocol);
                            }

                        $ret['pr'.$protocol.'f'.$field] = array('pr'.$protocol,
                                                                $alias,
                                                              $this->img_path."field.png",
                                                                self::$w_field);
                        }
                    }
                }
            }
        // _____________________ Find uses in expressions (o2val(), o2zero(), o2pre()) ___
        $exps = self::get_prg_exps($code, $prg);
        foreach ($exps as $exp_id => $body) {
            $parts = array();
            $res   = preg_match('/o2(\w+)\s*\(\s*["\']'.$view.
                                '["\']\s*,\s*["\']'.$field.'["\']\s*\)/',
                                $body, $parts);
            if (isset($parts[0])) {
                if (strpos($parts[1], "_") === false) {
                    if (!isset($ret['p'])) {
                        $ret["p"] = array($root,
                                          $prg,
                                          $this->img_path."prg.png",
                                          self::$w_program);
                        }
                    $ret["e".$exp_id] = array("p",
                                              $exp_id,
                                              $this->img_path."exp.png",
                                              self::$w_expression);
                    }
                }
            }
        if (count($ret)) {
            if ($view == 'prg�_�var') {
                $ret[$root] = array(0, 'Find occurences of variable ('.$field.
                                       ') in program "'.$prg.'"',
                                    $this->img_path."folder.png");
                }
            else {
                $ret[$root] = array(0, 'Find occurences of field ('.$field."@".$view.
                                       ') in program "'.$prg.'"',
                                    $this->img_path."folder.png");
                }
            }
        else {
            if ($view == 'prg�_�var') {
                $ret = array($root    => array(0,
                                               'Find occurences of variable ('.$field.
                                               ') in program "'.$prg.'"',
                                               $this->img_path."folder_error.png"),
                             "[info]" => array($root,
                                               "No matches found: variable is not used"));
                }
            else {
                $ret = array($root    => array(0,
                                               'Find occurences of field ('.$field."@".
                                               $view.') in program "'.$prg.'"',
                                               $this->img_path."folder_error.png"),
                             "[info]" => array($root,
                                               "No matches found: field is not used"));
                }
            }
        return $ret;

        }


    public function find_prg_view($prg, $view) {

        $root = "[root]";
        $ret  = array();
        $prf  = $this->prgs_list[$prg];
        $code = file_get_contents($prf);
        // ___________________________________________________________ Uses in actions ___
        $actions = array();
        $uses    = self::get_prg_acts_view($code);
        if (isset($uses[$view])) {
            foreach ($uses[$view] as $event => $acts) {
                foreach ($acts as $act_name => $act) {
                    foreach ($act as $step => $dummy_event) {
                        if (!isset($ret['p'])) {
                            $ret['p'] = array($root,
                                              $prg,
                                              $this->img_path.'prg.png',
                                              self::$w_program);
                            }
                        if (!isset($ret['a'.$act])) {
                            $ret['a'.$act_name] = array('p',
                                                        $act_name,
                                                        $this->img_path.'action.png',
                                                        self::$w_action);
                            }
                        if (!isset($actions[$act_name])) {
                            $actions[$act_name] = array();
                            }
                        $actions[$act_name][$step] = array('a'.$act_name,
                                                           $step.' ('.
                                                           strtoupper($event).')',
                                                           $this->img_path.
                                                           'step.png',
                                                           self::$w_step);
                        }
                    }
                }
            // _____________________________ Used to resort steps in the correct order ___
            foreach ($actions as $act_name => $steps) {
                ksort($steps);
                foreach ($steps as $step => $item) {
                    $ret['a'.$act_name.'s'.$step] = $item;
                    }
                }
            }
        // _____________________________________________________ Find uses in controls ___
        $ctrls = self::get_prg_ctrls($code);
        if (count($ctrls)) {
            foreach ($ctrls as $form => $form_ctrls) {
                foreach ($form_ctrls as $ctrl => $obj) {
                    if ($obj['V'] == $view && $obj['F'] == '') {
                        if (!isset($ret['p'])) {
                            $ret['p'] = array($root,
                                              $prg,
                                              $this->img_path."prg.png",
                                              self::$w_program);
                            }
                        if (!isset($ret['f'.$form])) {
                            $ret['f'.$form] = array('p',
                                                    $form,
                                                    $this->img_path."form.png",
                                                    self::$w_form);
                            }
                        $ret['c'.$ctrl] = array('f'.$form,
                                                $ctrl,
                                                $this->img_path."ctrl.png",
                                                self::$w_ctrl);
                        }
                    }
                }
            }
        // _____________________________________ Find uses in expressions (o2view_*()) ___
        $exps = self::get_prg_exps($code, $prg);
        foreach ($exps as $exp_id => $body) {
            $parts = array();
            $res   = preg_match('/o2view_(\w+)\s*\(\s*["\']'.$view.'["\']/',
                                $body, $parts);
            if (isset($parts[0])) {
                if (!isset($ret['p'])) {
                    $ret["p"] = array($root,
                                      $prg,
                                      $this->img_path."prg.png",
                                      self::$w_program);
                    }
                $ret["e".$exp_id] = array("p",
                                          $exp_id,
                                          $this->img_path."exp.png",
                                          self::$w_expression);
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find occurences of view ('.$view.
                                   ') in program "'.$prg.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find occurences of view ('.$view.
                                           ') in program "'.$prg.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root,
                                           "No matches found: view is not used"));
            }
        return $ret;

        }


    public function find_prg_exp($prg, $exp) {

        $root   = "[root]";
        $ret    = array();
        $prf    = $this->prgs_list[$prg];
        $code   = file_get_contents($prf);
        $uses   = $this->get_prg_exp($code, $prg, $exp);
        $id_prg = 0;
        if (count($uses)) {
            $id_prg++;
            $ret["p".$id_prg] = array($root,
                                      $prg,
                                      $this->img_path."prg.png",
                                      self::$w_program);
            // _________________________________________________________ Uses in views ___
            if (isset($uses['V'])) {
                foreach ($uses['V'] as $view => $in_view) {
                    $ret["p".$id_prg."v".$view] = array("p".$id_prg,
                                                        $view,
                                                        $this->img_path."view.png",
                                                        self::$w_view);
                    if (isset($in_view['R'])) {
                        $id       = "p".$id_prg."v".$view."r";
                        $ret[$id] = array("p".$id_prg."v".$view,
                                          'Paging records',
                                          $this->img_path."view.png",
                                          self::$w_view);
                        }
                    if (isset($in_view['T'])) {
                        foreach ($in_view['T'] as $table) {
                            $id       = "p".$id_prg."v".$view."t".$table;
                            $ret[$id] = array("p".$id_prg."v".$view,
                                              $table.' (main)',
                                              $this->img_path."tab.png",
                                              self::$w_table);
                            }
                        }
                    if (isset($in_view['L'])) {
                        foreach ($in_view['L'] as $link_tab) {
                            $id       = "p".$id_prg."v".$view."l".$link_tab;
                            $ret[$id] = array("p".$id_prg."v".$view,
                                              $link_tab.' (link)',
                                              $this->img_path."tab.png",
                                              self::$w_link_table);
                            }
                        }
                    if (isset($in_view['S'])) {
                        foreach ($in_view['S'] as $field) {
                            $id       = "p".$id_prg."v".$view."s".$field;
                            $ret[$id] = array("p".$id_prg."v".$view,
                                              $field.' (selected field)',
                                              $this->img_path."field.png",
                                              self::$w_field);
                            }
                        }
                    if (isset($in_view['C'])) {
                        foreach ($in_view['C'] as $link_field) {
                            $id       = "p".$id_prg."v".$view."c".$link_field;
                            $ret[$id] = array("p".$id_prg."v".$view,
                                              $link_field.' (link criteria)',
                                              $this->img_path."field.png",
                                              self::$w_link_field);
                            }
                        }
                    if (isset($in_view['F'])) {
                        foreach ($in_view['F'] as $formula) {
                            $id       = "p".$id_prg."v".$view."f".$formula;
                            $ret[$id] = array("p".$id_prg."v".$view,
                                              $formula.' (formula)',
                                              $this->img_path."field.png",
                                              self::$w_formula);
                            }
                        }
                    if (isset($in_view['Q'])) {
                        foreach ($in_view['Q'] as $sql) {
                            $id       = "p".$id_prg."v".$view."q".$sql;
                            $ret[$id] = array("p".$id_prg."v".$view,
                                              $sql.' (SQL formula)',
                                              $this->img_path."field.png",
                                              self::$w_sql);
                            }
                        }
                    }
                }
            // _______________________________________________________ Uses in actions ___
            if (isset($uses['A'])) {
                foreach ($uses['A'] as $act => $in_act) {
                    $ret["p".$id_prg."a".$act] = array("p".$id_prg,
                                                       $act,
                                                       $this->img_path."action.png",
                                                       self::$w_action);
                    foreach ($in_act as $step) {
                        $ret["p".$id_prg."a".$act."s".$step] = array("p".$id_prg."a".$act,
                                                                     $step,
                                                               $this->img_path."step.png",
                                                                     self::$w_step);
                        }
                    }
                }
            // _________________________________________________________ Uses in forms ___
            if (isset($uses['F'])) {
                foreach ($uses['F'] as $form => $ctrls) {
                    $ret["p".$id_prg."f".$form] = array("p".$id_prg,
                                                        $form,
                                                        $this->img_path."form.png",
                                                        self::$w_form);
                    foreach ($ctrls as $ctrl_name => $ctrl) {
                        if ($ctrl_name === "F") {
                            $id       = "p".$id_prg."f".$form."f".$form;
                            $ret[$id] = array("p".$id_prg."f".$form,
                                              $form,
                                              $this->img_path."form.png",
                                              self::$w_form);
                            }
                        else {
                            $id       = "p".$id_prg."f".$form."c".$ctrl;
                            $ret[$id] = array("p".$id_prg."f".$form,
                                              $ctrl,
                                              $this->img_path."ctrl.png",
                                              self::$w_ctrl);
                            }
                        }
                    }
                }
            // __________________________________________________ Uses in I/O resource ___
            if (isset($uses['I'])) {
                foreach ($uses['I'] as $io_id => $io) {
                    $ret["p".$id_prg."i".$io_id] = array("p".$id_prg,
                                                         $io,
                                                         $this->img_path."io.png",
                                                         self::$w_io);
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find occurences of expression ('.$exp.
                                   ') in program "'.$prg.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find occurences of expression ('.$exp.
                                           ') in program "'.$prg.'"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root,
                                           "No matches found: expression is not used"));
            }
        return $ret;

        }


    public function find_prg_unused_exps($prg) {

        $root     = "[root]";
        $ret      = array();
        $prf      = $this->prgs_list[$prg];
        $code     = file_get_contents($prf);
        $prg_path = substr($prf, 0, -1)."g";
        $prg_code = (file_exists($prg_path) ? file_get_contents($prg_path) : "");
        $exps     = self::get_prg_exps($code, $prg);
        $id_prg   = 0;
        if (count($exps)) {
            $id_prg++;
            $ret["p".$id_prg] = array($root,
                                      $prg,
                                      $this->img_path."prg.png",
                                      self::$w_program);
            foreach ($exps as $exp => $body) {
                $parts = explode($prg.'_exp_'.$exp.'()', $code);
                array_pop($parts);
                if ((count($parts) < 2 && substr($parts[0], -9) == "function ") &&
                    strpos($prg_code, $prg.'_exp_'.$exp.'()') === false) {
                    $ret["p".$id_prg."e".$exp] = array("p".$id_prg,
                                                       $exp,
                                                       $this->img_path."exp.png",
                                                       self::$w_expression);
                    }
                }
            }
        if (count($ret)) {
            $ret[$root] = array(0, 'Find unused expressions in program "'.$prg.'"',
                                $this->img_path."folder.png");
            }
        else {
            $ret = array($root    => array(0,
                                           'Find unused expressions in program "'.$prg.
                                           '"',
                                           $this->img_path."folder_error.png"),
                         "[info]" => array($root,
                                           "No matches found: all expressions are used"));
            }
        return $ret;

        }


    public function obscure($prg_name, $conf = array()) {

        $level         = (isset($conf['level']) ? $conf['level'] : 2);
        $header        = (isset($conf['header']) ? $conf['header'] : false);
        $acts_excluded = (isset($conf['ACTIONS_EXCLUDE']) ?
                          $conf['ACTIONS_EXCLUDE'] : array());
        $vars_excluded = (isset($conf['VARIABLES_EXCLUDE']) ?
                          $conf['VARIABLES_EXCLUDE'] : array());
        $custom_levels = (isset($conf['PRG_CUSTOM_LEVEL']) ?
                          $conf['PRG_CUSTOM_LEVEL'] : array());
        $ok            = true;
        $skip_msg      = "";
        if ($this->dir_prgs->exists($prg_name.'.prg')) {
            $prf_file = $this->dir_prgs.$prg_name.'.prf';
            $prg_file = $this->dir_prgs.$prg_name.'.prg';
            $prg_code = file_get_contents($prg_file);
            $prf_code = file_get_contents($prf_file);
            // __________________________________________________ Program custom level ___
            if (isset($custom_levels[$prg_name])) {
                $level    = $custom_levels[$prg_name];
                $skip_msg.= '  Custom level set to '.$level."\n";
                }
            // ===================================================== REPLACE VARIABLES ===
            if ($level > 1) {
                // __________________________________________________ Get actions list ___
                $vars = array_keys(self::get_prg_variables($prf_code));
                $idx  = 0;
                foreach ($vars as $var) {
                    $idx++;
                    $new_var = 'v'.$idx;
                    // ________________________________ Replace in variable definition ___
                    $prf_code = preg_replace('/(\$task_prg�_�var->definisci\(")'.$var.
                                             '(",\s*"\w*"\);)/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    // ______________________________ Replace variable in aggregations ___
                    $prf_code = preg_replace('/(\$task_\w*->aggregate\("\w*",\s*"\w*",'.
                                             '\s*"prg�_�var",\s*")'.$var.'("\);)/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    // _____________________ Replace parameter in CALL or GOTO actions ___
                    $prf_code = str_replace('"prg�_�var��'.$var.'"',
                                            '"prg�_�var��'.$new_var.'"',
                                            $prf_code);
                    // _____________________________________ Replace in UPDATE actions ___
                    $prf_code = preg_replace('/(\$o2exe->s\(\d*\)[^:]*o2act::set\('.
                                             '"prg�_�var",\s*")'.$var.'(",)/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    // ____________________________________ Replace in CONFIRM actions ___
                    $prf_code = preg_replace('/(\$o2exe->s\(\d*\)[^:]*o2act::confirm\('.
                                             '"prg�_�var",\s*")'.$var.'(",)/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    // ______________________________________ Replace uses in controls ___
                    $prf_code = preg_replace('/(\$ctrl_\w*\s*=\s*&\$form_\w*->ctrldef\('.
                                             '"[^"]*",\s*"[^"]*",\s*"[^"]*",\s*'.
                                             '"prg�_�var",\s*")'.$var.'("\);)/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    // _____________________________________ Replace uses in protocols ___
                    $prf_code = preg_replace('/(\$protocollo_\w*\-\>usa\('.
                                             '"\w*",\s*"prg�_�var",\s*")'.$var.'(",)/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    // ______ Replace uses in expressions (o2val(), o2zero(), o2pre()) ___
                    $prf_code = preg_replace('/(o2\w+\s*\(\s*["\']prg�_�var["\']\s*,'.
                                             '\s*["\'])'.$var.'(["\'])/',
                                             '$1'.$new_var.'$2',
                                             $prf_code);
                    }
                }
            // ======================================================= REPLACE ACTIONS ===
            if ($level == 3) {
                $func_replace = array();
                // _________________________________________ Check uses in o2_act4js() ___
                if (strpos($prf_code, 'o2_act4js(')) {
                    foreach ($this->function_explain($prf_code, 'o2_act4js') as $match) {
                        list($func_body, $params, $offset) = $match;
                        // ___ If action name is not passed clear it can't be replaced ___
                        if ($params[1] === '0') {
                            $parts = array();
                            preg_match('/["\'](\w*)["\']/', $params[2], $parts);
                            // ______________________________________ Not clear action ___
                            if ($parts[0] !== $params[2]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2_act4js() at offset "
                                               .$offset."\n";
                                break;
                                }
                            // __________________________________________ Clear action ___
                            else {
                                if (!isset($func_replace['o2_act4js'])) {
                                    $func_replace['o2_act4js'] = array();
                                    }
                                $func_replace['o2_act4js'][] = array($parts[1],
                                                                     $func_body,
                                                                     $params);
                                }
                            }
                        else {
                            $skip_msg.= "  WARNING: function o2_act4js() refers to ".
                                        "external program\n";
                            }
                        }
                    }
                 // _________________________________________________ o2progress_bar() ___
                if ($ok && strpos($prf_code, 'o2progress_bar(')) {
                    foreach ($this->function_explain($prf_code,
                                                     'o2progress_bar') as $match) {
                        list($func_body, $params, $offset) = $match;
                        // ______________________________________ Check valued actions ___
                        $act1 = strtolower($params[3]);
                        $act2 = strtolower($params[4]);
                        if ($act1 == 'false' ||
                            $act1 == 'null' ||
                            $act1 == "''" ||
                            $act1 == '""') {
                            $act1 = false;
                            }
                        if ($act2 == 'false' ||
                            $act2 == 'null' ||
                            $act2 == "''" ||
                            $act2 == '""') {
                            $act2 = false;
                            }
                        // ___ If action name is not passed clear it can't be replaced ___
                        if ($act1) {
                            $parts1 = array();
                            preg_match('/["\'](\w*)["\']/', $params[3], $parts1);
                            if ($parts1[0] !== $params[3]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2progress_bar() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        if ($act2) {
                            $parts2 = array();
                            preg_match('/["\'](\w*)["\']/', $params[4], $parts2);
                            if ($parts2[0] !== $params[4]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2progress_bar() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        // ______________________________________________ Clear action ___
                        if ($ok && ($act1 || $act2)) {
                            if (!isset($func_replace['o2progress_bar'])) {
                                $func_replace['o2progress_bar'] = array();
                                }
                            $func_replace['o2progress_bar'][] = array(($act1 ?
                                                                       $parts1[1] :
                                                                       false),
                                                                      ($act2 ?
                                                                       $parts2[1] :
                                                                       false),
                                                                      $func_body,
                                                                      $params);
                            }
                        }
                    }
                // _____________________________________________ o2progress_view_bar() ___
                if ($ok && strpos($prf_code, 'o2progress_view_bar(')) {
                    foreach ($this->function_explain($prf_code,
                                                     'o2progress_view_bar') as $match) {
                        list($func_body, $params, $offset) = $match;
                        // ______________________________________ Check valued actions ___
                        $act1 = strtolower($params[5]);
                        $act2 = strtolower($params[6]);
                        if ($act1 == 'false' ||
                            $act1 == 'null' ||
                            $act1 == "''" ||
                            $act1 == '""') {
                            $act1 = false;
                            }
                        if ($act2 == 'false' ||
                            $act2 == 'null' ||
                            $act2 == "''" ||
                            $act2 == '""') {
                            $act2 = false;
                            }
                        // ___ If action name is not passed clear it can't be replaced ___
                        if ($act1) {
                            $parts1 = array();
                            preg_match('/["\'](\w*)["\']/', $params[5], $parts1);
                            if ($parts1[0] !== $params[5]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: Unclear ".
                                                "syntax in o2progress_view_bar() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        if ($act2) {
                            $parts2 = array();
                            preg_match('/["\'](\w*)["\']/', $params[6], $parts2);
                            if ($parts2[0] !== $params[6]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: Unclear ".
                                                "syntax in o2progress_view_bar() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        // ______________________________________________ Clear action ___
                        if ($ok && ($act1 || $act2)) {
                            if (!isset($func_replace['o2progress_view_bar'])) {
                                $func_replace['o2progress_view_bar'] = array();
                                }
                            $func_replace['o2progress_view_bar'][] = array(($act1 ?
                                                                            $parts1[1] :
                                                                            false),
                                                                           ($act2 ?
                                                                            $parts2[1] :
                                                                            false),
                                                                           $func_body,
                                                                           $params);
                            }
                        }
                    }
                // ___________________________________________________ o2job_get_bar() ___
                if ($ok && strpos($prf_code, 'o2job_get_bar(')) {
                    foreach ($this->function_explain($prf_code,
                                                     'o2job_get_bar') as $match) {
                        list($func_body, $params, $offset) = $match;
                        // ______________________________________ Check valued actions ___
                        $act1 = strtolower($params[2]);
                        $act2 = strtolower($params[3]);
                        if ($act1 == 'false' ||
                            $act1 == 'null' ||
                            $act1 == "''" ||
                            $act1 == '""') {
                            $act1 = false;
                            }
                        if ($act2 == 'false' ||
                            $act2 == 'null' ||
                            $act2 == "''" ||
                            $act2 == '""') {
                            $act2 = false;
                            }
                        // ___ If action name is not passed clear it can't be replaced ___
                        if ($act1) {
                            $parts1 = array();
                            preg_match('/["\'](\w*)["\']/', $params[2], $parts1);
                            if ($parts1[0] !== $params[2]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2job_get_bar() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        if ($act2) {
                            $parts2 = array();
                            preg_match('/["\'](\w*)["\']/', $params[3], $parts2);
                            if ($parts2[0] !== $params[3]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2job_get_bar() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        // ______________________________________________ Clear action ___
                        if ($ok && ($act1 || $act2)) {
                            if (!isset($func_replace['o2job_get_bar'])) {
                                $func_replace['o2job_get_bar'] = array();
                                }
                            $func_replace['o2job_get_bar'][] = array(($act1 ?
                                                                      $parts1[1] :
                                                                      false),
                                                                     ($act2 ?
                                                                      $parts2[1] :
                                                                      false),
                                                                     $func_body,
                                                                     $params);
                            }
                        }
                    }
                // ______________________________________________________ o2tree_def() ___
                if ($ok && strpos($prf_code, 'o2tree_def(')) {
                    foreach ($this->function_explain($prf_code, 'o2tree_def') as $match) {
                        list($func_body, $params, $offset) = $match;
                        // ___ If action name is not passed clear it can't be replaced ___
                        $act1 = strtolower($params[4]);
                        if ($act1 &&
                            $act1 != 'false' &&
                            $act1 != 'null' &&
                            $act1 != '""' &&
                            $act1 != "''") {
                            $parts = array();
                            preg_match('/["\'](\w*)["\']/', $params[4], $parts);
                            // ______________________________________ Not clear action ___
                            if ($parts[0] !== $params[4]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2tree_def() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            // __________________________________________ Clear action ___
                            else {
                                if (!isset($func_replace['o2tree_def'])) {
                                    $func_replace['o2tree_def'] = array();
                                    }
                                $func_replace['o2tree_def'][] = array($parts[1],
                                                                      $func_body,
                                                                      $params);
                                }
                            }
                        }
                    }
                // ___________________________________________________ o2imglist_def() ___
                if ($ok && strpos($prf_code, 'o2imglist_def(')) {
                    foreach ($this->function_explain($prf_code,
                                                     'o2imglist_def') as $match) {
                        list($func_body, $params, $offset) = $match;
                        // ______________________________________ Check valued actions ___
                        $act1 = strtolower($params[5]);
                        $act2 = strtolower($params[6]);
                        if ($act1 == 'false' ||
                            $act1 == 'null' ||
                            $act1 == "''" ||
                            $act1 == '""') {
                            $act1 = false;
                            }
                        if ($act2 == 'false' ||
                            $act2 == 'null' ||
                            $act2 == "''" ||
                            $act2 == '""') {
                            $act2 = false;
                            }
                        // ___ If action name is not passed clear it can't be replaced ___
                        if ($act1) {
                            $parts1 = array();
                            preg_match('/["\'](\w*)["\']/', $params[5], $parts1);
                            if ($parts1[0] !== $params[5]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2imglist_def() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        if ($act2) {
                            $parts2 = array();
                            preg_match('/["\'](\w*)["\']/', $params[6], $parts2);
                            if ($parts2[0] !== $params[6]) {
                                $func_replace = array();
                                $ok           = false;
                                $skip_msg    .= "  Skip actions replacing: ".
                                                "Unclear syntax in o2imglist_def() at ".
                                                "offset ".$offset."\n";
                                break;
                                }
                            }
                        // ______________________________________________ Clear action ___
                        if ($ok && ($act1 || $act2)) {
                            if (!isset($func_replace['o2imglist_def'])) {
                                $func_replace['o2imglist_def'] = array();
                                }
                            $func_replace['o2imglist_def'][] = array(($act1 ?
                                                                      $parts1[1] :
                                                                      false),
                                                                     ($act2 ?
                                                                      $parts2[1] :
                                                                      false),
                                                                     $func_body,
                                                                     $params);
                            }
                        }
                    }
                if ($ok) {
                    // ______________________________________________ Get actions list ___
                    $acts = self::get_prg_acts($prf_file);
                    $idx  = 0;
                    // ___________________ Check each action against its possible uses ___
                    foreach (array_keys($acts) as $act) {
                        // _____________________________ Check excluded actions prefix ___
                        if (isset($acts_excluded[$prg_name])) {
                            $excl = preg_split('/\W/', $acts_excluded[$prg_name]);
                            if (in_array($act, $excl)) {
                                $skip_msg.= '  Skip action "'.$act."\"\n";
                                continue;
                                }
                            }
                        $idx++;
                        $new_act = 'a'.$idx;
                        // _____________________ Replace in Janox scriptable functions ___
                        if (count($func_replace)) {
                            // ________________________________ Replace in o2_act4js() ___
                            if (isset($func_replace['o2_act4js'])) {
                                foreach ($func_replace['o2_act4js'] as $item) {
                                    if ($item[0] == $act) {
                                        $end_pars = array_slice($item[2], 2);
                                        $replace  = 'o2_act4js('.$item[2][1].','.
                                                                 "'".$new_act."'".
                                                    (count($end_pars) ?
                                                     ','.implode(',', $end_pars) : '').
                                                    ')';
                                        $prf_code = str_replace($item[1],
                                                                $replace,
                                                                $prf_code);
                                        }
                                    }
                                }
                            // ___________________________ Replace in o2progress_bar() ___
                            if (isset($func_replace['o2progress_bar'])) {
                                foreach ($func_replace['o2progress_bar'] as $i => $item) {
                                    // ________ Replace first action (complete-action) ___
                                    if ($item[0] && $item[0] == $act) {
                                        $replace  = str_replace($item[3][3],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2progress_bar'][$i][2] = $replace;
                                        }
                                    // __________ Replace second action (start-action) ___
                                    if ($item[1] && $item[1] == $act) {
                                        $replace  = str_replace($item[3][4],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2progress_bar'][$i][2] = $replace;
                                        }
                                    }
                                }
                            // ______________________ Replace in o2progress_view_bar() ___
                            if (isset($func_replace['o2progress_view_bar'])) {
                                foreach ($func_replace['o2progress_view_bar'] as
                                                                            $i => $item) {
                                    // ________ Replace first action (complete-action) ___
                                    if ($item[0] && $item[0] == $act) {
                                        $replace  = str_replace($item[3][5],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2progress_view_bar']
                                                     [$i][2] = $replace;
                                        }
                                    // __________ Replace second action (start-action) ___
                                    if ($item[1] && $item[1] == $act) {
                                        $replace  = str_replace($item[3][6],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2progress_view_bar']
                                                     [$i][2] = $replace;
                                        }
                                    }
                                }
                            // ______________________ Replace in o2progress_view_bar() ___
                            if (isset($func_replace['o2job_get_bar'])) {
                                foreach ($func_replace['o2job_get_bar'] as $i => $item) {
                                    // ________ Replace first action (complete-action) ___
                                    if ($item[0] && $item[0] == $act) {
                                        $replace  = str_replace($item[3][2],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2job_get_bar'][$i][2] = $replace;
                                        }
                                    // __________ Replace second action (start-action) ___
                                    if ($item[1] && $item[1] == $act) {
                                        $replace  = str_replace($item[3][3],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2job_get_bar'][$i][2] = $replace;
                                        }
                                    }
                                }
                            // _______________________________ Replace in o2tree_def() ___
                            if (isset($func_replace['o2tree_def'])) {
                                foreach ($func_replace['o2tree_def'] as $item) {
                                    if ($item[0] == $act) {
                                        $replace  = 'o2tree_def('.$item[2][1].','.
                                                                  $item[2][2].','.
                                                                  $item[2][3].','.
                                                                  "'".$new_act.'\')';
                                        $prf_code = str_replace($item[1],
                                                                $replace,
                                                                $prf_code);
                                        }
                                    }
                                }
                            // ___________________________ Replace in o2progress_bar() ___
                            if (isset($func_replace['o2imglist_def'])) {
                                foreach ($func_replace['o2imglist_def'] as $i => $item) {
                                    // ________ Replace first action (complete-action) ___
                                    if ($item[0] && $item[0] == $act) {
                                        $replace  = str_replace($item[3][5],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2imglist_def'][$i][2] = $replace;
                                        }
                                    // __________ Replace second action (start-action) ___
                                    if ($item[1] && $item[1] == $act) {
                                        $replace  = str_replace($item[3][6],
                                                                "'".$new_act."'",
                                                                $item[2]);
                                        $prf_code = str_replace($item[2],
                                                                $replace,
                                                                $prf_code);
                                        $func_replace['o2imglist_def'][$i][2] = $replace;
                                        }
                                    }
                                }
                            }
                        // _____________________________ Replace in action declaration ___
                        $prg_code = preg_replace('/(o2def::act\(")'.$act.
                                                 '("[^)]*\);)/',
                                                 '$1'.$new_act.'$2',
                                                 $prg_code);
                        // ___________________________________ Replace prefix in views ___
                        $prg_code = preg_replace('/(o2def::view\("\w*",\s*[^,]+,\s*")'.
                                                 $act.'(",\s*"\w*"[^;]*\);)/',
                                                 '$1'.$new_act.'$2',
                                                 $prg_code);
                        // ___________________________________ Replace suffix in views ___
                        $prg_code = preg_replace('/(o2def::view\("\w*",\s*[^,]+,'.
                                                 '\s*"\w*",\s*")'.$act.'("[^;]*\);)/',
                                                 '$1'.$new_act.'$2',
                                                 $prg_code);
                        // ______________________________ Replace program start action ___
                        $prg_code = preg_replace('/(o2def::prg\("\w*",\s*")'.$act.
                                                 '(",\s*"",\s*__FILE__\);)/',
                                                 '$1'.$new_act.'$2',
                                                 $prg_code);
                        // _________________________________ Replace action definition ___
                        $prf_code = preg_replace('/(function \w*��)'.$act.
                                                 '(_act\(&\$o2exe\) {)/',
                                                 '$1'.$new_act.'$2',
                                                 $prf_code);
                        // ____________________________________ Replace execute action ___
                        $prf_code = preg_replace('/(\$o2exe->s\(\d*\)[^:]*o2act::'.
                                                 'exe\(")'.$act.'(",\s*[^,]*,)/',
                                                 '$1'.$new_act.'$2',
                                                 $prf_code);
                        // ___________________ Replace refresh or close action on form ___
                        $prf_code = preg_replace('/(\$form_\w*\-\>(refresh|close)'.
                                                 '_action\(")'.$act.'("\);)/',
                                                 '$1'.$new_act.'$3',
                                                 $prf_code);
                        // _____________________________________ Replace in form ctrls ___
                        $prf_code = preg_replace('/(\$ctrl_\w*->(azione|act_\w*)\(")'.
                                                 $act.'("\);)/',
                                                 '$1'.$new_act.'$3',
                                                 $prf_code);
                        // ____________________________________ Replace in o2_act4js() ___
                        $prf_code = preg_replace('/(o2_act4js\([^,]*\s*,\s*["\'])'.
                                                 $act.'(["\'])/',
                                                 '$1'.$new_act.'$2',
                                                 $prf_code);
                        }
                    }
                }
            // ________________________________________________________ Prepare header ___
            if ($header) {
                $header = preg_split('/\r\n|\n|\r/', $header);
                $header = "<?"."php\n\n/**\n * ".implode("\n * ", $header)."\n */\n";
                }
            else {
                $header = "<?"."php\n\n";
                }
            // _______________________________________ Strip white spaces and comments ___
            $tmp = $this->dir_tmp.'jxobscure';
            file_put_contents($tmp, $prg_code);
            $prg_code = $header.substr(php_strip_whitespace($tmp), 6);
            file_put_contents($tmp, $prf_code);
            $prf_code = $header.substr(php_strip_whitespace($tmp), 6);
            unlink($tmp);
            // ______________________________________________ Return obscured contents ___
            return array('prg' => $prg_code, 'prf' => $prf_code, 'log' => $skip_msg);
            }
        else {
            return array('prg' => '', 'prf' => '', 'log' => "  Program not found\n");
            }

        }


    function function_explain($prf_code, $function) {

        $res          = array();
        $inside_func  = false;
        $func_body    = '';
        $nest_level   = 0;
        $inside_nest  = false;
        $param_idx    = 0;
        $params       = array();
        $offset       = 0;
        $offset_start = 0;
        foreach (token_get_all($prf_code) as $token) {
            if (is_array($token)) {
                list($token_type, $token_text) = $token;
                // ______________________________________________ Found function start ___
                if ($token_type == T_STRING && $token_text == $function) {
                    $offset_start = $offset;
                    $inside_func  = true;
                    $func_body    = '';
                    $nest_level   = 0;
                    $inside_nest  = false;
                    $param_idx    = 0;
                    $params       = array();
                    }
                // _________________________________________ If inside function syntax ___
                if ($inside_func) {
                    $func_body.= $token_text;
                    // ______________ If reading 1st level parameter (no sub function) ___
                    if ($param_idx) {
                        // _____________________________________ Add text to parameter ___
                        $params[$param_idx].= trim($token_text);
                        }
                    }
                $offset+= strlen($token_text);
                }
            else {
                // _________________________________________ If inside function syntax ___
                if ($inside_func) {
                    $func_body.= $token;
                    switch ($token) {
                        case '(':
                            // _______________________________ Increment nesting level ___
                            $inside_nest = true;
                            $nest_level++;
                            // ____________________________ Start 1st level parameters ___
                            if ($nest_level == 1) {
                                $param_idx = 1;
                                }
                            // ______ If reading 1st level parameter (no sub function) ___
                            elseif ($param_idx) {
                                // ______________________________ Add text to paramter ___
                                $params[$param_idx].= '(';
                                }
                            break;
                        case ')':
                            // ______________________________ End 1st level parameters ___
                            if ($nest_level == 1) {
                                $param_idx = 0;
                                }
                            // ______ If reading 1st level parameter (no sub function) ___
                            elseif ($param_idx) {
                                // ______________________________ Add text to paramter ___
                                $params[$param_idx].= ')';
                                }
                            $nest_level--;
                            break;
                        case ',':
                            // _____________________________________ Move on parameter ___
                            if ($nest_level == 1) {
                                $param_idx++;
                                }
                            // ______ If reading 1st level parameter (no sub function) ___
                            elseif ($param_idx) {
                                // ______________________________ Add text to paramter ___
                                $params[$param_idx].= ',';
                                }
                            break;
                        default:
                            // ______ If reading 1st level parameter (no sub function) ___
                            if ($param_idx) {
                                // ______________________________ Add text to paramter ___
                                $params[$param_idx].= trim($token);
                                }
                            break;
                        }
                    // ______________________________________________ Function closure ___
                    if ($inside_nest && $nest_level == 0) {
                        $res[]       = array($func_body,
                                             $params,
                                             $offset_start,
                                             $offset - $offset_start + 1);
                        $inside_func = false;
                        }
                    }
                $offset+= strlen($token);
                }
            }
        return $res;

        }


    // ==================================================== MULTI LANGUAGE TRANSLATION ===
    /**
     * Executes an harvest of all used strings from base version (development language) of
     * the requested program
     *
     * @param string $prg_name   Name of program to process
     */
    function mlt_harvest($prg_name, $check_expressions = true) {

        if (!file_exists($this->dir_prgs.$prg_name.'.prf')) {
            return;
            }
        $code  = file_get_contents($this->dir_prgs.$prg_name.'.prf');
        $list  = array();
        $exps  = array();
        $parts = array();
        // ____________________________________ Windows title (can contain expression) ___
        preg_match_all(
                 '/^\s*\$form_(\w+)->titolo\((["\'](.+)["\']|\w+_exp_(\d+)\(\))\);\s*$/m',
                       $code, $parts);
        foreach ($parts[1] as $idx => $form_name) {
            // ___________________________________________________ Title by expression ___
            if ($parts[4][$idx]) {
                if ($check_expressions) {
                    $exps[$parts[4][$idx]] = 'Title of form '.$form_name;
                    }
                }
            // _______________________________________________________ Title by string ___
            else {
                if (!isset($list[$form_name])) {
                    $list[$form_name] = array();
                    }
                $list[$form_name]['title'][0] = $parts[3][$idx];
                }
            }
        // ___________________ Labels, buttons and imgs label (can contain expression) ___
        preg_match_all(
                  '/^\s*\$ctrl_(\w+)->label\((["\'](.+)["\']|\w+_exp_(\d+)\(\))\);\s*$/m',
                       $code, $parts);
        foreach ($parts[1] as $idx => $ctrl_name) {
            // ___________________________________________________ Label by expression ___
            if ($parts[4][$idx]) {
                if ($check_expressions) {
                    $exps[$parts[4][$idx]] = 'Label of control '.$ctrl_name;
                    }
                }
            // _______________________________________________________ Label by string ___
            else {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array();
                    }
                $list[$ctrl_name]['label'][0] = $parts[3][$idx];
                }
            }
        // __________________ Confirm and navigators messages (can contain expression) ___
        preg_match_all(
          '/^\s*\$ctrl_(\w+)->confirm_(\w*)\((["\'](.+)["\']|\w+_exp_(\d+)\(\))\);\s*$/m',
                       $code, $parts);
        foreach ($parts[1] as $idx => $ctrl_name) {
            // _________________________________________________ Message by expression ___
            if ($parts[5][$idx]) {
                if ($check_expressions) {
                    $exps[$parts[5][$idx]] = 'Message ('.$parts[2][$idx].
                                             ') of control '.$ctrl_name;
                    }
                }
            // _____________________________________________________ Message by string ___
            else {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array();
                    }
                $list[$ctrl_name]['msg_'.$parts[2][$idx]][0] = $parts[4][$idx];
                }
            }
        // _____________________________________________________ List/combo-box values ___
        preg_match_all('/^\s*\$ctrl_(\w+)->valori\(array\(\s*(.+)\s*\)\);\s*$/m',
                       $code, $parts);
        foreach ($parts[1] as $idx => $ctrl_name) {
            $parts2 = array();
            preg_match_all('/["\']?(.*?)["\']?\s*=>\s*(["\'])(.*?)\2\s*,?\s*/',
                           $parts[2][$idx], $parts2);
            foreach ($parts2[3] as $valid => $value) {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array('item' => array());
                    }
                $list[$ctrl_name]['item'][$parts2[1][$valid]] = $value;
                }
            }
        // ____________________________________________________ Multipage pages labels ___
        preg_match_all('/^\s*\$ctrl_(\w+)->label\(array\(\s*(.+)\s*\)\);\s*$/m',
                       $code, $parts);
        foreach ($parts[1] as $idx => $ctrl_name) {
            $parts2 = array();
            preg_match_all('/["\'](.*?)["\']/', $parts[2][$idx], $parts2);
            foreach ($parts2[1] as $valid => $value) {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array('page' => array());
                    }
                $list[$ctrl_name]['page'][$valid] = $value;
                }
            }
        // ____________________ Grid column title and tooltip (can contain expression) ___
        preg_match_all('/^\s*\$ctrl_(\w+)->info_padre\(array\((.+)\)\);\s*$/m',
                       $code, $parts);
        foreach ($parts[1] as $idx => $ctrl_name) {
            $parts2 = array();
            preg_match_all('/["\'](.*?)["\']|\w+_exp_(\d+)\(\)|\d+|False|True/i',
                           $parts[2][$idx], $parts2);
            // ____________________________________________ Column title by expression ___
            if ($parts2[2][3]) {
                if ($check_expressions) {
                    $exps[$parts2[2][3]] = 'Column title of control '.$ctrl_name;
                    }
                }
            // ________________________________________________ Column title by string ___
            elseif ($parts2[1][3]) {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array();
                    }
                $list[$ctrl_name]['grid_title'][0] = $parts2[1][3];
                }
            // ___________________________________________ Column footer by expression ___
            if ($parts2[2][8]) {
                if ($check_expressions) {
                    $exps[$parts2[2][8]] = 'Column footer of control '.$ctrl_name;
                    }
                }
            // _______________________________________________ Column footer by string ___
            elseif ($parts2[1][8]) {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array();
                    }
                $list[$ctrl_name]['grid_foot'][0] = $parts2[1][8];
                }
            // __________________________________________ Column tooltip by expression ___
            if ($parts2[2][10]) {
                if ($check_expressions) {
                    $exps[$parts2[2][10]] = 'Column tooltip of control '.$ctrl_name;
                    }
                }
            // ______________________________________________ Column tooltip by string ___
            elseif ($parts2[1][8]) {
                if (!isset($list[$ctrl_name])) {
                    $list[$ctrl_name] = array();
                    }
                $list[$ctrl_name]['grid_tip'][0] = $parts2[1][10];
                }
            }
        // ___________________________________________________ Function o2_translate() ___
        preg_match_all(
                '/o2_translate\s*\(\s*["\'](.+?)["\']\s*,\s*(["\'])(.+?)\2\s*([^)]*)\)/',
                       $code, $parts);
        foreach ($parts[3] as $idx => $base_string) {
            $list[$parts[1][$idx]]['string'][0] = $base_string;
            }
        $this->mlt_process_list($prg_name, $list);
        // _________________________________________________________ CHECK EXPRESSIONS ___
        if ($check_expressions) {
            $app       = $_SESSION['o2_app'];
            $log       = array();
            $file      = $app->dir_home.'/lang/'.$prg_name.'.check';
            $exps_code = self::get_prg_exps($code, $prg_name);
            foreach ($exps_code as $exp => $desc) {
                // _______________________________ Remove expressions with translation ___
                if (!preg_match('/o2_translate\s*\(/', $desc)) {
                    foreach (token_get_all("<?php\n".$desc."\n?>") as $token) {
                        if (is_array($token)) {
                            // ___________________________ Look for "language" strings ___
                            if (($token[0] == T_CONSTANT_ENCAPSED_STRING) &&
                                (strpos($token[1], ' ') !== false) &&
                                (preg_match('/\w+/', $token[1]))) {
                                $log[] = $exp.' : '.
                                         (strlen($token[1]) <= 70 ?
                                          $token[1] :
                                          substr($token[1], 0, 65).'[...]')."\n";
                                break;
                                }
                            }
                        }

                    }
                }
            $tot = count($log);
            if ($tot) {
                file_put_contents($file,
                                  "Janox translation - support to development\n".
                                  str_repeat('~', 42)."\n\n".
                                  'Application:   '.$app->nome."\n".
                                  'Program:       '.$prg_name."\n".
                                  'Date:          '.date('d.m.Y')."\n\n".$tot.
                                  " expressions need to be checked for translation:\n".
                                  str_repeat('-', strlen($tot) + 48)."\n\n".
                                  implode('', $log));
                }
            else {
                if (file_exists($file)) {
                    unlink($file);
                    }
                }
            }

        }


    /**
     * Executes an harvest of the models repository strings
     *
     */
    function mlt_harvest_models() {

        $app  = $_SESSION['o2_app'];
        $list = array();
        foreach ($app->maschere as $mod_name => $model) {
            if (count($model->valori) &&
                substr($mod_name, 0, 3) != "_o2" &&
                substr($mod_name, 0, 2) != "o2"  &&
                substr($mod_name, 0, 2) != "jx") {
                foreach ($model->valori as $code => $desc) {
                    if (trim($desc)) {
                        $list[$mod_name]['item'][$code] = $desc;
                        }
                    }
                }
            }
        $this->mlt_process_list('#MODELS', $list);

        }


    /**
     * Executes an harvest of the menu repository labels
     *
     */
    function mlt_harvest_menu() {

        $list = array();
        foreach (o2_inspector::get_app_menus(false) as $menu => $label) {
            $list[$menu]['label'][0] = $label;
            }
        $this->mlt_process_list('#MENU', $list);

        }


    /**
     * Removes control properties missing in list (removed from program) from all
     * dictionaries (all languages)
     *
     * @param string $prg_name   Name of programm to check
     * @param array  $list       List of harvested strings
     */
    function mlt_delete_removed_controls($prg_name, $list) {

        $app  = $_SESSION['o2_app'];
        $tab  = $app->get_table(self::$mlt_table);
        $db   = $tab->db;
        $srv  = $db->server;
        $co   = constant('o2_'.$srv->type.'_o');
        $cc   = constant('o2_'.$srv->type.'_c');
        $f    = $tab->campi;
        // ________________________________________________________________ Connection ___
        $conn = o2_gateway::connect($srv->type,
                                    $srv->server,
                                    $srv->user,
                                    $srv->password);
        // _____________________________________________________________________ Query ___
        $qtab  = o2_gateway::qualify($srv->type,
                                     $db->nome,
                                     $db->proprietario,
                                     $tab->nome);
        $query = "SELECT ".$f['mlt_ctrl'].' '.$co.'CTRL'.$cc.','.
                           $f['mlt_property'].' '.$co.'PROPERTY'.$cc.','.
                           $f['mlt_prop_value'].' '.$co.'PROP_VALUE'.$cc.
                 " FROM ".$qtab.
                 " WHERE ".$f['mlt_lang']."='' AND ".$f['mlt_prg']."='".$prg_name."'";
        // _________________________________________________________________ Statement ___
        if (!$GLOBALS["jxlasterror"]) {
            $stm = false;
            // ________________________________________________ Create query statement ___
            if (!($stm = $conn->query($query))) {
               $err = $conn->errorInfo();
               throw new o2_exception($err[2], o2error_DBDATAQUERY);
               return false;
               }
            while ($row = $stm->fetch(PDO::FETCH_ASSOC)) {
                $ctrl = $row['CTRL'];
                $prop = $row['PROPERTY'];
                $val  = $row['PROP_VALUE'];
                // ___________________________ If missing remove from all dictionaries ___
                if (!isset($list[$ctrl][$prop][$val])) {
                    $where = $f['mlt_prg']."='".$prg_name."' AND ".
                             $f['mlt_ctrl']."='".$ctrl."' AND ".
                             $f['mlt_property']."='".$prop."' AND ".
                             $f['mlt_prop_value']."='".o2_gateway::normalize($srv->type,
                                                                             $val)."'";
                    o2_gateway::deleterec($srv->type,
                                          $srv->server,
                                          $srv->user,
                                          $srv->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_translations',
                                          $where);
                    }
                }

            }

        }


    /**
     * Processes harvested strings list and write it into system translation table for
     * base-dictionary (lang='')
     *
     * @param string $prg_name   Name of program to process
     * @param array  $list       List of harvested strings
     */
    function mlt_process_list($prg_name, $list) {

        if (!is_array($list)) {
            throw new o2_exception("Multilanguage Translation: Cannot process items list",
                                   o2error_EXECUTION);
            return false;
            }
        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$mlt_table);
        $db     = $tab->db;
        $server = $db->server;
        $co     = constant('o2_'.$server->type.'_o');
        $cc     = constant('o2_'.$server->type.'_c');
        $f      = $tab->campi;
        // ___________________ Remove missing control properties from all dictionaries ___
        $this->mlt_delete_removed_controls($prg_name, $list);
        // ___________________________________________ Clear base-dictionary (lang='') ___
        $where = $f['mlt_lang']."='' AND ".$f['mlt_prg']."='".$prg_name."'";
        o2_gateway::deleterec($server->type,
                              $server->server,
                              $server->user,
                              $server->password,
                              $db->nome,
                              $db->proprietario,
                              $tab->nome,
                              'jx_translations',
                              $where);

        // ______________________________________________________________ Process list ___
        $select = $f['mlt_base_str'].' '.$co.'BASE_STR'.$cc.','.
                  $f['mlt_lang_str'].' '.$co.'LANG_STR'.$cc;
        foreach ($list as $ctrl => $properties) {
            foreach ($properties as $prop => $prop_values) {
                foreach ($prop_values as $prop_val => $string) {
                    if ($string) {
                        $fields = array($f['mlt_lang'],
                                        $f['mlt_prg'],
                                        $f['mlt_ctrl'],
                                        $f['mlt_property'],
                                        $f['mlt_prop_value'],
                                        $f['mlt_lang_str']);
                        $values = array("''",
                                        "'".$prg_name."'",
                                        "'".$ctrl."'",
                                        "'".$prop."'",
                                        "'".o2_gateway::normalize($server->type,
                                                                  $prop_val)."'",
                                        "'".o2_gateway::normalize($server->type,
                                                                  $string)."'");
                        o2_gateway::insertrec($server->type,
                                              $server->server,
                                              $server->user,
                                              $server->password,
                                              $db->nome,
                                              $db->proprietario,
                                              $tab->nome,
                                              'jx_translations',
                                              $fields,
                                              $values);
                        }
                    }
                }
            }

        }


    /**
     * Executes "receive" action for requested language.
     * Reads .lng files from lang directory and writes translations in system translation
     * table.
     *
     * @param string  $lang          Lang code to receive
     * @param string  $base_lang     Base language to match changes against
     * @param integer $tot_progress  Total progress to reach (when in multiple jobs chain)
     * @param integer $base_progress Base progress to add (when in multiple jobs chain)
     */
    function mlt_get_lang($lang,
                          $base_lang     = '',
                          $base_progress = 0,
                          $tot_progress  = 100) {

        $app    = $_SESSION['o2_app'];
        $tab    = $app->get_table(self::$mlt_table);
        $db     = $tab->db;
        $server = $db->server;
        $co     = constant('o2_'.$server->type.'_o');
        $cc     = constant('o2_'.$server->type.'_c');
        $f      = $tab->campi;
        $select = $f['mlt_ctrl'].' '.$co.'CTRL'.$cc.','.
                  $f['mlt_property'].' '.$co.'PROPERTY'.$cc.','.
                  $f['mlt_prop_value'].' '.$co.'PROP_VALUE'.$cc.','.
                  $f['mlt_base_str'].' '.$co.'BASE_STR'.$cc.','.
                  $f['mlt_lang_str'].' '.$co.'LANG_STR'.$cc;
        // __________________________________________________________ /app/lang/<lang> ___
        $lang_dir = new o2_dir($app->dir_home.'/lang/'.$lang.'/');
        $prgs     = $lang_dir->all_elements('*.lng');
        $tot      = count($prgs);
        $idx      = 0;
        // ___________________________________________ Loop on all translated programs ___
        foreach ($prgs as $prg_dict) {
            // _________________________ Read translations from dictionary file (.lng) ___
            $dict  = jx_parse_conf($prg_dict);
            // ________________________________________ Retrieve base language records ___
            $where = $f['mlt_lang']."='".$base_lang."' AND ".
                     $f['mlt_prg']."='".$prg_dict->nome."'";
            $base  = o2_gateway::recordset($server->type,
                                           $server->server,
                                           $server->user,
                                           $server->password,
                                           $db->nome,
                                           $db->proprietario,
                                           $tab->nome,
                                           'jx_translations',
                                           $select,
                                           $where,
                                           '',
                                           100000);
            // __________________________________________ Retrieve translation records ___
            $where = $f['mlt_lang']."='".$lang."' AND ".
                     $f['mlt_prg']."='".$prg_dict->nome."'";
            $recs  = o2_gateway::recordset($server->type,
                                           $server->server,
                                           $server->user,
                                           $server->password,
                                           $db->nome,
                                           $db->proprietario,
                                           $tab->nome,
                                           'jx_translations',
                                           $select,
                                           $where,
                                           '',
                                           100000);
            foreach ($recs as $rec) {
                $item = $rec['CTRL'].'#'.$rec['PROPERTY'].
                        ($rec['PROPERTY'] == 'item' || $rec['PROPERTY'] == 'page' ?
                         '_'.$rec['PROP_VALUE'] : '');
                if (trim($dict[$item]) &&
                    (trim($rec['LANG_STR']) != trim($dict[$item]))) {
                    $where_i = $where.' AND '.
                               $f['mlt_ctrl']."='".$rec['CTRL']."' AND ".
                               $f['mlt_property']."='".$rec['PROPERTY']."' AND ".
                               $f['mlt_prop_value']."='".
                                            o2_gateway::normalize($server->type,
                                                                  $rec['PROP_VALUE'])."'";
                    // _____________________________________ Retrieve base-lang string ___
                    $base_str = '';
                    foreach ($base as $b_rec) {
                        if ($b_rec['CTRL']       == $rec['CTRL'] &&
                            $b_rec['PROPERTY']   == $rec['PROPERTY'] &&
                            $b_rec['PROP_VALUE'] == $rec['PROP_VALUE']) {
                            $base_str = $b_rec['LANG_STR'];
                            break;
                            }
                        }
                    // _______________ Remove "changed" status from translation record ___
                    if ($base_str != $rec['BASE_STR']) {
                        $sets = array($f['mlt_lang_str'].'' => "'".o2_gateway::normalize(
                                                                            $server->type,
                                                                            $dict[$item]).
                                                            "'",
                                      $f['mlt_base_str'].'' => "'".o2_gateway::normalize(
                                                                            $server->type,
                                                                            $base_str).
                                                            "'");
                        }
                    else {
                        $sets = array($f['mlt_lang_str'].'' => "'".o2_gateway::normalize(
                                                                            $server->type,
                                                                            $dict[$item]).
                                                            "'");
                        }
                    o2_gateway::modifyrec($server->type,
                                          $server->server,
                                          $server->user,
                                          $server->password,
                                          $db->nome,
                                          $db->proprietario,
                                          $tab->nome,
                                          'jx_translations',
                                          $sets,
                                          $where_i);

                    }
                }
            $idx++;
            if (o2session_is_job()) {
                o2job_set_progress(($idx * $tot_progress / $tot) + $base_progress);
                }
            }

        }

    }

?>
