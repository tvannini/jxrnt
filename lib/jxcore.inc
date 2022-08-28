<?php

/**
 * Janox Runtime Core
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
 * This script contains the whole runtime environment base elements:
 *  .:§ System functions
 *  .:§ Internal base objects
 *  .:§ Databases gateways interface
 *  .:§ Runtime object
 *  .:§ Definition methods (o2def)
 *  .:§ Exceptions handling
 *
 * @name      jxcore
 * @package   janox/jxcore.inc
 * @version   2.8
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


new o2_runtime();


/**
 * o2 exception class
 */
class o2_exception extends Exception {

    public $error_class = "UNKNOWN"; /* Class is assigned in constructor by $code       */
    public $caught      = true;      /* If exception has been caught                    */
    public $id          = 0;         /* Exception unique id                             */
    public $warning     = false;     /* Warning level: non breaking error, log to file  */


    /**
     * Constructor
     *
     * @param string  $message
     * @param integer $code
     */
    public function __construct($message, $code = 0) {

        $this->id     = mt_rand();
        $this->caught = true;
        $this->set_error_class($code);
        // _______________________________________________ Hide errors after first one ___
        if ($GLOBALS["jxlasterror"]) {
            $GLOBALS['jxerrorhide'] = true;
            }
        elseif ($code != o2error_DEPRECATED && $code != o2error_WARNING) {
            $GLOBALS["jxlasterror"] = $this;
            }
        // _____________________ Don't break transaction for Warnings and Deprecations ___
        if (isset($_SESSION['o2_app']) &&
            $code != o2error_DEPRECATED &&
            $code != o2error_WARNING) {
            $this->mark_transactions();
            }
        parent::__construct($message, $code);

        }


    /**
     * Sets constant named codes for possible exceptions
     *
     */
    public static function def_errors() {

        $seed_sys  = -1;
        $seed_db   = 49;
        $seed_objs = 99;
        // ________________________________________________ [0...49] system
        define('o2error_UNKNOWN',          ++$seed_sys);
        define('o2error_IO',               ++$seed_sys);
        define('o2error_XML',              ++$seed_sys);
        define('o2error_MISSINGDEF',       ++$seed_sys);
        define('o2error_MISSINGFILE',      ++$seed_sys);
        define('o2error_FILEUPLOAD',       ++$seed_sys);
        define('o2error_EXECUTION',        ++$seed_sys);
        define('o2error_RELEASE',          ++$seed_sys);
        define('o2error_DEPRECATED',       ++$seed_sys);
        define('o2error_WARNING',          ++$seed_sys);
        define('o2error_APP',              ++$seed_sys);
        define('o2error_SNAPSHOT',         ++$seed_sys);
        // ________________________________________________ [50...99] database gateway ___
        define('o2error_DBCONNECT',        ++$seed_db);
        define('o2error_DBTABLES',         ++$seed_db);
        define('o2error_DBTABEXISTS',      ++$seed_db);
        define('o2error_DBTABLECOPY',      ++$seed_db);
        define('o2error_DBTABLEDROP',      ++$seed_db);
        define('o2error_DBTABLECREATE',    ++$seed_db);
        define('o2error_DBTABLEREBUILD',   ++$seed_db);
        define('o2error_DBAGGREGATE',      ++$seed_db);
        define('o2error_DBCOUNT',          ++$seed_db);
        define('o2error_DBRECQUERY',       ++$seed_db);
        define('o2error_DBDATAQUERY',      ++$seed_db);
        define('o2error_DBRECINSERT',      ++$seed_db);
        define('o2error_DBRECUPDATE',      ++$seed_db);
        define('o2error_DBRECDELETE',      ++$seed_db);
        define('o2error_DBCOMMIT',         ++$seed_db);
        define('o2error_DBEXECUTE',        ++$seed_db);
        define('o2error_DBTABLEINFO',      ++$seed_db);
        define('o2error_DBTABLEALTER',     ++$seed_db);
        define('o2error_DBFKEY',           ++$seed_db);
        define('o2error_DBINDEX',          ++$seed_db);
        // _____________________________________________ [100...149] objects referring ___
        define('o2error_UNKNOWNMODEL',     ++$seed_objs);
        define('o2error_UNKNOWNDBSERVER',  ++$seed_objs);
        define('o2error_UNKNOWNTABLE',     ++$seed_objs);
        define('o2error_UNKNOWNVIEW',      ++$seed_objs);
        define('o2error_UNKNOWNVIEWMODEL', ++$seed_objs);
        define('o2error_UNKNOWNFIELD',     ++$seed_objs);
        define('o2error_UNKNOWNMENU',      ++$seed_objs);
        define('o2error_UNKNOWNAPPVAR',    ++$seed_objs);
        define('o2error_UNKNOWNEXP',       ++$seed_objs);
        define('o2error_UNKNOWNFORM',      ++$seed_objs);
        define('o2error_UNKNOWNPRG',       ++$seed_objs);
        define('o2error_UNKNOWNACTION',    ++$seed_objs);
        define('o2error_UNKNOWNCTRL',      ++$seed_objs);
        }


    public function set_error_class($code) {

        $error_class = "";
        switch ($code) {
            // ________________________________________________________________ System ___
            case o2error_IO:
                $error_class = "SYSTEM INPUT/OUTPUT";
                break;
            case o2error_XML:
                $error_class = "XML PARSING";
                break;
            case o2error_MISSINGDEF:
                $error_class = "DEFINITION MISSING";
                break;
            case o2error_MISSINGFILE:
                $error_class = "FILE MISSING";
                break;
            case o2error_FILEUPLOAD:
                $error_class = "FILE UPLOAD";
                break;
            case o2error_EXECUTION:
                $error_class = "EXECUTION";
                break;
            case o2error_RELEASE:
                $error_class = "VERSION MATCHING";
                break;
            case o2error_DEPRECATED:
                $error_class = "DEPRECATED TECHNIQUE";
                break;
            case o2error_APP:
                $error_class = "APPLICATION";
                break;
            case o2error_SNAPSHOT:
                $error_class = "VIEW SNAPSHOT";
                break;
                        // ____________________________________________ [50...99] database gateway ___
            case o2error_DBCONNECT:
                $error_class = "DATABASE CONNECTION";
                break;
            case o2error_DBTABLES:
                $error_class = "DATABASE TABLES RETRIEVE";
                break;
            case o2error_DBTABEXISTS:
                $error_class = "DATABASE TABLE EXISTANCE";
                break;
            case o2error_DBTABLECOPY:
                $error_class = "DATABASE TABLE COPY";
                break;
            case o2error_DBTABLEDROP:
                $error_class = "DATABASE TABLE DROP";
                break;
            case o2error_DBTABLECREATE:
                $error_class = "DATABASE TABLE CREATE";
                break;
            case o2error_DBTABLEREBUILD:
                $error_class = "DATABASE TABLE REBUILD";
                break;
            case o2error_DBAGGREGATE:
                $error_class = "DATABASE AGGREGATE";
                break;
            case o2error_DBCOUNT:
                $error_class = "DATABASE TABLE COUNT";
                break;
            case o2error_DBRECQUERY:
                $error_class = "DATABASE RECORD QUERY";
                break;
            case o2error_DBDATAQUERY:
                $error_class = "DATABASE DATASET QUERY";
                break;
            case o2error_DBRECINSERT:
                $error_class = "DATABASE RECORD INSERT";
                break;
            case o2error_DBRECUPDATE:
                $error_class = "DATABASE RECORD UPDATE";
                break;
            case o2error_DBRECDELETE:
                $error_class = "DATABASE RECORD DELETE";
                break;
            case o2error_DBCOMMIT:
                $error_class = "DATABASE COMMIT";
                break;
            case o2error_DBEXECUTE:
                $error_class = "DATABASE EXECUTE";
                break;
            case o2error_DBTABLEINFO:
                $error_class = "DATABASE TABLE STRUCTURE RETRIEVE";
                break;
            case o2error_DBTABLEALTER:
                $error_class = "DATABASE TABLE ALTER STRUCTURE";
                break;
            case o2error_DBFKEY:
                $error_class = "DATABASE FOREIGN KEYS";
                break;
            case o2error_DBINDEX:
                $error_class = "DATABASE TABLE INDEX";
                break;
            // _________________________________________ [100...149] objects referring ___
            case o2error_UNKNOWNDBSERVER:
                $error_class = "DATABASE SERVER REFERRING";
                break;
            case o2error_UNKNOWNTABLE:
                $error_class = "TABLE REFERRING";
                break;
            case o2error_UNKNOWNVIEW:
                $error_class = "VIEW REFERRING";
                break;
            case o2error_UNKNOWNVIEWMODEL:
                $error_class = "VIEW MODEL REFERRING";
                break;
            case o2error_UNKNOWNMENU:
                $error_class = "MENU REFERRING";
                break;
            case o2error_UNKNOWNAPPVAR:
                $error_class = "APPLICATION VARIABLE REFERRING";
                break;
            case o2error_UNKNOWNEXP:
                $error_class = "EXPRESSION REFERRING";
                break;
            case o2error_UNKNOWNFORM:
                $error_class = "FORM REFERRING";
                break;
            case o2error_UNKNOWNPRG:
                $error_class = "PROGRAM REFERRING";
                break;
            case o2error_UNKNOWNCTRL:
                $error_class = "CONTROL REFERRING";
                break;
            case o2error_UNKNOWNACTION:
                $error_class = "ACTION REFERRING";
                break;
            // ________________________________________________________________ Others ___
            case o2error_WARNING:
                $error_class = "WARNING";
                break;
            case o2error_UNKNOWN:
            default:
                $error_class = "UNKNOWN";
                break;
            }
        $this->code        = $code;
        $this->error_class = $error_class;

        }


    /**
     * Marks active transactions with error status, to avoid next commit
     *
     */
    private function mark_transactions() {

        $app = $_SESSION['o2_app'];
        if ($app) {
            foreach ($app->server as $server) {
                // _____________________ If an active transaction is found on a server ___
                $engine = $server->type;
                if (isset($GLOBALS['o2_'.$engine.'_trans'])) {
                    foreach ($GLOBALS['o2_'.$engine.'_trans'] as $key => $trans) {
                        if (!is_array($GLOBALS['o2_'.$engine.'_error'])) {
                            $GLOBALS['o2_'.$engine.'_error'] = array();
                            }
                        $GLOBALS['o2_'.$engine.'_error'][$key] = true;
                        }
                    }
                }
            }

        }


    /**
     * Cancel exception, avoid bubbling and sending.
     * This method is intended to be used in a CATCH part of a TRY/CATCH block, to unset
     * catched exception.
     * If exception is generated on a db connection pass $server object parameter to
     * identify server-key.
     * All other connections are restored to be correctly committed, while session-key
     * connection is sent to rollback.
     */
    public function cancel($server = false) {

        $app = $_SESSION['o2_app'];
        if (is_a($server, 'o2_server')) {
            $errorkey = $server->server.$server->user.
                       ($app->isolated_trans ? 'jxext' : '');
            }
        else {
            $errorkey = '';
            }
        // __________________________________________ Clear error on all transactions  ___
        foreach ($app->server as $app_server) {
            $engine = $app_server->type;
            if (isset($GLOBALS['o2_'.$engine.'_trans'])) {
                foreach ($GLOBALS['o2_'.$engine.'_trans'] as $key => $trans) {
                    // _____________________________ Skip passed $db connection if any ___
                    if (($key != $errorkey) &&
                        isset($GLOBALS['o2_'.$engine.'_error'][$key])) {
                        unset($GLOBALS['o2_'.$engine.'_error'][$key]);
                        }
                    }
                }
            }
        unset($GLOBALS["jxlasterror"]);
        // ________________________________________________ Rollback passed connection ___
        if ($errorkey && isset($GLOBALS['o2_'.$server->type.'_trans'][$errorkey])) {
            o2_gateway::commit($server->type,
                               $server->server,
                               $server->user,
                               $server->password,
                               true);
            }

        }


    /**
     * Handler for not-exception errors like warning, deprecated, ecc...
     *
     * @return boolean
     */
    public static function errors_handler($severity, $message, $file, $line) {

        if (isset($_SESSION['o2_app']) && $_SESSION['o2_app']->error_mode == 'DEV') {
            if ($severity === E_WARNING) {
                (new o2_exception($message.'<br>File: '.$file.' Line: '.$line,
                                  o2error_WARNING))->send();
                }
            elseif ($severity === E_DEPRECATED) {
                (new o2_exception($message.'<br>File: '.$file.' Line: '.$line,
                                  o2error_DEPRECATED))->send();
                }
            elseif ($severity != E_STRICT && $severity != E_NOTICE) {
                (new o2_exception($message.'<br>File: '.$file.' Line: '.$line))->send();
                }
            else {
                return false;
                }
            }
        return true;

        }


    /**
     * This is the public method to get private property ->code
     *
     * @return integer
     */
    public function get_code() {

        return $this->code;

        }


    /**
     * This is the public method to get private property ->class
     *
     * @return string
     */
    public function get_class() {

        return $this->error_class;

        }


    /**
     * This is the public method to get private property ->message
     *
     * @return string
     */
    public function get_message() {

        return $this->message;

        }


    /**
     * This is the method to use when returning exception as a string
     *
     * @return string
     */
    public function __toString() {

        switch ($GLOBALS['o2_runtime']->interface) {
            case "HTML":
                return $this->to_html();
            default:
                return $this->to_raw_text();
            }

        }


    /**
     * Returns error message in HTML context
     *
     * @return string
     */
    public function to_html() {

        $app = $_SESSION['o2_app'];
        if ($app->run_level == "DEV" || $GLOBALS['o2_runtime']->developer){
            return $this->to_html_dev();
            }
        elseif ($app->errors_inc && file_exists($app->dir_progs.$app->errors_inc)) {
            return $this->to_html_custom();
            }
        else {
            return $this->to_html_std();
            }

        }


    /**
     * Returns text formatted for color console
     *
     *    $foreg_color = 0 - Gray      1 - Red       2 - Green     3 - Yellow
     *                   4 - Blue      5 - Purple    6 - Cyan      7 - Light white
     *                   8 - White (default)
     *
     *    $backg_color = 0 - Gray      1 - Red       2 - Green     3 - Yellow
     *                   4 - Blue      5 - Purple    6 - Cyan      7 - White
     *                   8 - Black (default)
     *
     *    $effect      = 1 - Bold      2 - Lite      4 - Underlined
     *                   7 - Reverse   9 - Strocked
     *
     * @return string
     */
    public static function colorize($text2color,
                                    $foreg_color = 8,
                                    $backg_color = 8,
                                    $effect      = 0) {

        return "\033[".($foreg_color < 8 ? (30 + $foreg_color) : "").
                       ($backg_color < 8 ? ($foreg_color ? ";" : "").
                                           (40 + $backg_color).";" : "").
                       ($effect ? ($foreg_color.$backg_color ? ";" : "").
                                   $effect : "")."m".
               $text2color."\033[0m";

        }


    /**
     * Returns HTML error message for developers
     *
     * @return string
     */
    private function to_html_dev() {

        $prg_local = "";
        $act_local = "";
        $exp_local = "";
        $fun_local = "";
        $app       = $_SESSION['o2_app'];
        if (is_array($app->istanze_prg)) {
            foreach (array_reverse($app->istanze_prg) as $singola_istanza) {
                if ($prg_local == "") {
                    $prg_local = "<b>".$singola_istanza->nome.
                                 " [".$singola_istanza->id."]</b>";
                    }
                else {
                    $prg_local = $singola_istanza->nome.
                                 " [".$singola_istanza->id."]; ".$prg_local;
                    }
                }
            foreach ($this->getTrace() as $single_step) {
                if ($this->getCode() > 99 &&
                    substr($single_step['function'], 0, 2) == "o2") {
                    $args      = (is_array($single_step['args']) ?
                                  implode(", ", $single_step['args']) : "");
                    $fun_local = "<code>".$single_step['function']."(".$args.")</code>";
                    }
                list($prg_name, $exp_local) = explode("_exp_", $single_step['function']);
                if ($prg_name == $singola_istanza->nome) {
                    $exp_local = ($exp_local ?
                              "<tr>\n<td class='o2_error_info'><i>Expression:</i></td>\n".
                              "<td class='o2_error_info'>[".$exp_local."] ".$fun_local.
                              "</td>\n</tr>\n" :
                              "");
                    break;
                    }
                $exp_local = ($exp_local || !$fun_local ?
                              $exp_local :
                             "<tr>\n<td class='o2_error_info'><i>Instruction:</i></td>\n".
                             "<td class='o2_error_info'>".$fun_local."</td>\n</tr>\n");
                }
            $prg_local = ($prg_local ?
                          "<tr>\n<td class='o2_error_info'><i>Program:</i></td>\n".
                          "<td class='o2_error_info'>".$prg_local."</td>\n</tr>\n" :
                          "");
            }
        if ($app->esecuzioni[0]) {
            if ($app->esecuzioni[0]->passo) {
                $act_local = "<tr>\n<td class='o2_error_info'><i>Action:</i></td>\n".
                             "<td class='o2_error_info'>".
                             $app->esecuzioni[0]->azione." [step ".
                             $app->esecuzioni[0]->passo."]</td>\n</tr>\n";
                }
            }
        $msg_id     = "jxerror_".$this->id;
        $error_text = "<div id='".$msg_id."' style='position:relative;z-index:9998;'>\n".
                      "<table class='o2_error_report'>\n".
                      "<tr><td COLSPAN='2'><table>\n".
                      "<tr>\n<td class='o2_error_title'><center><b>".
                      $this->error_class." FAILURE</b></center></td>\n".
                      "<td class='o2_error_close' onclick='getElementById(\"".
                      $msg_id."\").style.display=\"none\";' ".
                      "title='Close error message'>X</td></tr></table>\n</tr>\n".
                      "<tr>\n<td COLSPAN='2'class='o2_error_text'>".
                      $this->getMessage()."</td>\n</tr>\n".
                      "<tr>\n <td class='o2_error_info'><i>Application:</i></td>\n".
                      " <td class='o2_error_info'>".$app->nome."</td>\n</tr>\n".
                      $prg_local.$act_local.$exp_local."<tr>\n".
                      " <td class='o2_error_info'><i>From:</i></td>\n".
                      " <td class='o2_error_info'>".$this->getFile()." (".
                      $this->getLine().")</td>\n</tr>\n".
                      "<tr>\n <td class='o2_error_info'><i>Time:</i></td>\n".
                      "<td class='o2_error_info'>".date("H:i:s d-m-Y")."</td>\n</tr>\n".
                      ($this->caught ? "" :
                      "<tr>\n<td COLSPAN='2' class='o2_error_info'><center>".
                       "<b>Execution terminated</b></center></td>\n</tr>\n").
                      "</table></div>\n";
        // ___________________________________________ Unrem to add full PHP backtrace ___
        // jxlog(debug_backtrace()); // Developers only
        // $error_text.= "<pre>".print_r(debug_backtrace(), 1)."</pre>"; // All users
        // file_put_contents($app->dir_logs.'bt.log', print_r(debug_backtrace(), 1)); //FS
        return $error_text;

        }


    /**
     * Returns HTML error message for standard users
     *
     * @return string
     */
    private function to_html_std() {

        $rnt = $GLOBALS['o2_runtime'];
        $app = $_SESSION['o2_app'];
        // ________________________________________________________ Data error from DB ___
        if ($this->code >= 50 && $this->code < 100) {
            $title = "DATA ERROR";
            $img   = $rnt->alias."img/data_error.png";
            $msg   = "System experienced a data error: check processed data ".
                     "or contact System Administrator.";
            }
        // __________________________________________________________ All other errors ___
        else {
            $title = "INTERNAL ERROR";
            $img   = $rnt->alias."img/error.png";
            $msg   = "System experienced an internal error: ".
                     "contact System Administrator.";
            }
        $err_id   = "jxerror_".$this->id;
        $err_html = "<div id='".$err_id."' class='winshadow' ".
                  "style='position:absolute;top:10%;left:5%;width:400px;z-index:9998;'>".
                    "<div class='o2_form_window'>\n".
                    "<table class='o2_form_body' style='padding:5px;'>\n".
                    "<tr><td ROWSPAN='2'><img src='".$img."'></td>\n".
                    "<td style='font-weight:bold;font-size:120%;text-align:center'>".
                    $title."</td>\n".
                    "<td style='width:10px;cursor:pointer' onclick='getElementById(\"".
                    $err_id."\").style.display=\"none\";' ".
                    "title='Close error message'><img src='".$rnt->alias.
                    "img/close.png'></td></tr>\n".
                    "<tr><td COLSPAN='2' style='padding:10px;'>".$msg."</td></tr>\n".
                    ($app->errors_log ?
                     "<tr><td COLSPAN='3'><i>Error will be reported.</i></td></tr>" : "").
                    "<tr><td COLSPAN='3'><i>Time:</i> ".date("H:i:s d-m-Y")."</td></tr>".
                    ($this->caught ? "" :
                    "<tr>\n<td COLSPAN='3' class='o2_ctrl_edit_dis'><center>".
                    "<b>Execution terminated</b></center></td>\n</tr>\n").
                    "</table></div></div>\n";
        return $err_html;

        }


    /**
     * Returns HTML error message for standard users
     *
     * @return string
     */
    private function to_html_custom() {

        $app      = $_SESSION['o2_app'];
        $err_id   = "jxerror_".mt_rand();
        ob_start();
        include $app->dir_progs.$app->errors_inc;
        $msg      = ob_get_clean();
        $err_id   = "jxerror_".$this->id;
        $err_html = "<div id='".$err_id."' class='winshadow' style='position:absolute;".
                    "top:10%;left:5%;width:400px;z-index:9998;'>".
                    "<div class='o2_form_window'>\n".$msg."</div></div>\n";
        return $err_html;

        }


    public function to_raw_text() {

        $prg = "";
        $act = "";
        $exp = "";
        $fun = "";
        $app = $_SESSION['o2_app'];
        if (is_array($app->istanze_prg)) {
            foreach (array_reverse($app->istanze_prg) as $singola_istanza) {
                if ($prg == "") {
                    $prg = $singola_istanza->nome." [".$singola_istanza->id."]";
                    }
                else {
                    $prg = $singola_istanza->nome." [".$singola_istanza->id."]; ".$prg;
                    }
                }
            foreach ($this->getTrace() as $single_step) {
                if ($this->getCode() > 99 &&
                    substr($single_step['function'], 0, 2) == "o2") {
                    $fun = $single_step['function'].
                           "(".implode(", ", $single_step['args']).")";
                    }
                list($prg_name, $exp) = explode("_exp_", $single_step['function']);
                if ($prg_name == $singola_istanza->nome) {
                    $exp = ($exp ? "         Expression: [".$exp."] ".$fun."\n" : "");
                    break;
                    }
                $exp = ($exp || !$fun ? $exp : "         Instruction: ".$fun."\n");
                }
            $prg = ($prg ? "         Program: ".$prg."\n" : "");
            }
        if ($app->esecuzioni[0]) {
            if ($app->esecuzioni[0]->passo) {
                $act = "         Action: ".$app->esecuzioni[0]->azione." [step ".
                                           $app->esecuzioni[0]->passo."]\n";
                }
            }
        $msg_text = $this->message;
        $parts = array();
        preg_match('/\<code\>(.*)\<\/code\>/s', $this->message, $parts);
        $query    = $parts[1];
        $msg_text = str_replace($query, htmlspecialchars($query), $msg_text);
        $msg_text = htmlspecialchars_decode(strip_tags(preg_replace(
                                                            '/<br\s*\/?>|<hr\s*\/?>|\n/i',
                                                                    "\n         ",
                                                                    $msg_text)));
        $error_text = "\n\n     *** ".$this->error_class." FAILURE ***\n\n".
                      "         ".$msg_text."\n\n".
                      "         Application: ".$app->nome."\n".
                      $prg.$act.$exp.
                      "         From: ".$this->getFile()." (".$this->getLine().")\n".
                      "         Time: ".date("H:i:s d-m-Y")."\n\n".
                      ($this->caught ? "" : "     *** Execution terminated ***\n\n");
        return $error_text;

        }


    private function to_color_text() {

        $prg = "";
        $act = "";
        $exp = "";
        $fun = "";
        $app = $_SESSION['o2_app'];
        if (is_array($app->istanze_prg)) {
            foreach (array_reverse($app->istanze_prg) as $singola_istanza) {
                if ($prg == "") {
                    $prg = o2_exception::colorize($singola_istanza->nome." [".
                                                  $singola_istanza->id."]", 3, 8, 1);
                    }
                else {
                    $prg = o2_exception::colorize($singola_istanza->nome." [".
                                                  $singola_istanza->id."];", 3)." ".$prg;
                    }
                }
            foreach ($this->getTrace() as $single_step) {
                if ($this->getCode() > 99 &&
                    substr($single_step['function'], 0, 2) == "o2") {
                    $fun = $single_step['function'].
                           "(".implode(", ", $single_step['args']).")";
                    }
                list($prg_name, $exp) = explode("_exp_", $single_step['function']);
                if ($prg_name == $singola_istanza->nome) {
                    $exp = ($exp ?
                            "         ".
                            o2_exception::colorize("Expression:", 7, 8, 2)." ".
                            o2_exception::colorize("[".$exp."] ".$fun, 3)."\n" :
                            "");
                    break;
                    }
                $exp = ($exp || !$fun ?
                        $exp :
                        "         ".
                        o2_exception::colorize("Instruction:", 7, 8, 2)." ".
                        o2_exception::colorize($fun, 3)."\n");
                }
            $prg = ($prg ?
                    "         ".
                    o2_exception::colorize("Program:", 7, 8, 2)." ".
                    o2_exception::colorize(wordwrap($prg, 63,
                                                    "\n".str_repeat(" ", 18), true),
                                           3)."\n" : "");
            }
        if ($app->esecuzioni[0]) {
            if ($app->esecuzioni[0]->passo) {
                $act_txt = $app->esecuzioni[0]->azione.
                           " [step ".$app->esecuzioni[0]->passo."]";
                $act_txt = wordwrap($act_txt, 64, "\n".str_repeat(" ", 17), true);
                $act     = "         ".o2_exception::colorize("Action:", 7, 8, 2)." ".
                                       o2_exception::colorize($act_txt, 3).
                           "\n";
                }
            }
        $end_color  = "\033[0m";
        $base_color = $end_color."\033[37;2m";
        $i_color    = $end_color."\033[33m";
        $b_color    = $end_color."\033[37;2;4m";
        $u_color    = $end_color."\033[33;4m";
        $code_color = $end_color."\033[33m";
        $msg_text   = wordwrap($this->message, 72, "\n         ", true);
        $msg_text   = preg_replace('/<b\s*>/i', $b_color,
                                   preg_replace('/<\/b>/i', $base_color, $msg_text));
        $msg_text   = preg_replace('/<i\s*>/i', $i_color,
                                   preg_replace('/<\/i>/i', $base_color, $msg_text));
        $msg_text   = preg_replace('/<u\s*>/i', $u_color,
                                   preg_replace('/<\/u>/i', $base_color, $msg_text));
        $msg_text   = preg_replace('/<code\s*>/i', $code_color,
                                   preg_replace('/<\/code>/i', $base_color, $msg_text));
        $msg_text   = preg_replace('/<hr\s*\/?>/i', "\n         ----------\n         ",
                                   preg_replace('/<\/code>/i', $base_color, $msg_text));
        $msg_text   = $base_color.strip_tags(preg_replace('/<br\s*\/?>/i',
                                                          "\n         ",
                                                          $msg_text)).$end_color;
        $error_text = "\n\n     ".o2_exception::colorize("*** ".$this->error_class.
                                                         " FAILURE ***", 1)."\n\n".
                      "         ".$msg_text."\n\n".
                      "         ".o2_exception::colorize("Application:", 7, 8, 2)." ".
                                  o2_exception::colorize(wordwrap($app->nome, 59,
                                                         "\n".str_repeat(" ", 22), true),
                                                         3)."\n".
                      $prg.$act.$exp.
                      "         ".o2_exception::colorize("From:", 7, 8, 2)." ".
                                  o2_exception::colorize(wordwrap($this->getFile()." (".
                                                                  $this->getLine().")",
                                                                  66,
                                                                  "\n".str_repeat(" ",
                                                                                  15),
                                                                  true),
                                                         3)."\n".
                      "         ".o2_exception::colorize("Time:", 7, 8, 2)." ".
                                  o2_exception::colorize(date("H:i:s d-m-Y"), 3)."\n\n".
                      ($this->caught ?
                       "" :
                       "     ".
                       o2_exception::colorize("*** Execution terminated ***", 1)."\n\n");
        return $error_text;

        }


    public function send() {

        if ($GLOBALS['jxerrorhide']) {
            return;
            }
        $app = $_SESSION['o2_app'];
        // _________________________________________________________ Log error to file ___
        if ($app && $app->errors_log) {
            file_put_contents($app->dir_logs."errors.log",
                             ($app->run_level == "DEV" ?
                              "DEVELOPER: ".$app->developer."\n" : "").
                             "USER: ".$app->user."\n".
                             "SESSID: ".$app->session_id."\n".
                             $this->to_raw_text(),
                             FILE_APPEND);
            }
        // __________________________________ WEB service mode used for XML stream out ___
        if ($app && $app->rpc_server && !$app->custom_response) {
            print "<!--\n".$this->to_raw_text()."\n-->";
            die();
            }
        $rnt = $GLOBALS['o2_runtime'];
        switch ($rnt->interface) {
            case "HTML":
                if ($GLOBALS['jxjs']) {
                    print "jxe('".addcslashes($this->to_html(), "'\\\n\r")."');\n";
                    }
                else {
                    print $this->to_html();
                    }
                break;
            default:
                if ($rnt->mswin) {
                    print $this->to_raw_text();
                    }
                else {
                    print $this->to_color_text();
                    }
                break;
            }

        }


    /**
     * Top level handler for uncaught exceptions
     *
     * @param o2_exception $myself
     */
    public static function autosend($myself) {

        if (!isset($_SESSION['o2_app'])) {
            // ___________________________________________________________ Default css ___
            print "<html><head><link rel='stylesheet' type='text/css' href='".
                  $GLOBALS['o2_runtime']->alias."css/janox.css'></head><body>\n";
            }
        // ______________________________________________________ o2 managed exception ___
        if ($myself instanceof o2_exception) {
            $myself->caught = false;
            $myself->send();
            }
        elseif ($myself instanceof PDOException) {
            $m = $myself->getMessage();
            if ($GLOBALS['o2_runtime']->developer) {
                $t = $myself->getTrace();
                // ______________________________________________ PDO Connection error ___
                if ($t[0]['class'] == "PDO" && $t[0]['function'] == "__construct") {
                    $m = $t[0]['args'][0]."<br><br>".$m;
                    }
                }
            $e = new o2_exception($m, o2error_DBCONNECT);
            $e->send();
            }
        // __________________________________________________________ Other exceptions ___
        else {
            $e = new o2_exception($myself->getMessage(), o2error_UNKNOWN);
            $e->send();
            }
        if ($GLOBALS['o2_runtime']->interface == "HTML") {
            if ($GLOBALS['jxjs']) {
                jxjs::end(true);
                }
            elseif (isset($_SESSION['o2_app'])) {
                $app = $_SESSION['o2_app'];
                if ($app->html_started) {
                    o2html::page_suffix();
                    }
                else {
                    o2html::page_prefix();
                    }
                }
            else {
                print "</body></html>";
                }
            }
        }


    /**
     * Method used to send a DEPRECATION message to developers.
     * Method behaviour changes on application parameter "error_mode":
     *  - "DEV": error is shown on interface, if user is developer, and in errors.log, if
     *           it is active;
     *  - "EXE" or "OFF": error is reported in warning.log, if activated, else it is
     *                    ignored.
     *
     * @param string $oldfunc   Old deprecated function name
     * @param string $newfunc   New suggested function name
     * @param string $object    Deprecated/suggested objects nature
     */
    public static function deprecate($oldfunc, $newfunc, $object = "function") {

        if ($app = $_SESSION['o2_app']) {
            $o2e = new o2_exception("WARNING! ".ucfirst($object)." <b>".$oldfunc.
                                    "</b> has been DEPRECATED.<br>".
                                    "Relying on this feature is highly discouraged.<br>".
                                    "Please use ".$object." <b>".$newfunc."</b> instead.",
                                    o2error_DEPRECATED);
            if ($app->error_mode == 'DEV') {
                if ($app->run_level == "DEV") {
                    $o2e->send();
                    }
                // _________________________________________________ Log error to file ___
                elseif ($app->errors_log) {
                    file_put_contents($app->dir_logs."errors.log",
                                      ($app->run_level == "DEV" ?
                                       "DEVELOPER: ".$app->developer."\n" : "").
                                      "USER: ".$app->user."\n".
                                      "SESSID: ".$app->session_id."\n".
                                      $o2e->to_raw_text(),
                                      FILE_APPEND);
                    }
                }
            // ___________________________________________________ Log warning to file ___
            elseif ($app->warning_log) {
                file_put_contents($app->dir_logs."warning.log",
                                  ($app->run_level == "DEV" ?
                                   "DEVELOPER: ".$app->developer."\n" : "").
                                  "USER: ".$app->user."\n".
                                  "SESSID: ".$app->session_id."\n".
                                  $o2e->to_raw_text(),
                                  FILE_APPEND);
                }
            }
        return false;

        }


    /**
     * Method used to log internal "critical" events: no user error is generated and event
     * is logged on "warning.log" file, in application logs directory
     *
     * @param string $warn_msg   Warning message
     */
    public static function warning($warn_msg) {

        if (($app = $_SESSION['o2_app']) && $app->warning_log) {
            $o2e = new o2_exception($warn_msg, o2error_WARNING);
            // ___________________________________________________ Log warning to file ___
            file_put_contents($app->dir_logs."warning.log",
                             ($app->run_level == "DEV" ?
                              "DEVELOPER: ".$app->developer."\n" : "").
                             "USER: ".$app->user."\n".
                             "SESSID: ".$app->session_id."\n".
                             $o2e->to_raw_text(),
                             FILE_APPEND);
            }
        return false;

        }
    }


/**
 * Filesystem element (file or folder)
 *
 */
class o2_fsitem {

    /*     ===== PROPERTIES =========================================================== */
    public $nome_completo = ""; /* Complete element name (path/name.extension)          */
    public $nome          = ""; /* Element base name without path and extension         */
    public $path          = ""; /* Path (for folder is equal to $nome_completo)         */
    public $ext           = ""; /* Extension                                            */
    public $tipo          = ""; /* Type [F] = file, [D] = directory                     */
    public $http_mime     = ""; /* Upload files mime type                               */
    public $data_modifica = ""; /* Last modification date                               */
    public $ora_modifica  = ""; /* Last modification time                               */
    public $dimensioni    = 0;  /* File size                                            */


    /**
     * Constructor
     *
     * @param string  $file
     */
    function __construct($file) {

        $this->nome_completo = (realpath($file) ? realpath($file) : $file);
        $parts               = pathinfo($this->nome_completo);
        $this->ext           = (isset($parts['extension']) ? $parts['extension'] : '');
        $this->nome          = $parts['filename'];
        $this->path          = $parts['dirname'].
                               ($parts['dirname'] != DIRECTORY_SEPARATOR ?
                                DIRECTORY_SEPARATOR : '');
        $exists              = file_exists($this->nome_completo);
        if ($exists) {
            $timestamp_local     = @filemtime($this->nome_completo);
            $this->data_modifica = date("Ymd", $timestamp_local);
            $this->ora_modifica  = date("His", $timestamp_local);
            }
        else {
            $this->data_modifica = '00000000';
            $this->ora_modifica  = '000000';
            }
        if (is_dir($this->nome_completo)) {
            $this->tipo = "D";
            }
        else {
            $this->tipo = "F";
            if ($exists) {
                $this->dimensioni = @filesize($this->nome_completo);
                }
            }

        }


    function set_mime() {

        $this->http_mime = o2_mime_content($this->nome_completo, $this->ext);

        }


    /**
     * Provide toString method for using object instead of full name property
     *
     * @return string   Element full path and name
     */
    function __toString() {

        return $this->nome_completo;

        }

    }


/**
 * Filesystem folder
 *
 */
class o2_dir extends o2_fsitem {

    /*     ===== PROPERTIES =========================================================== */
    public $match         = "*";     /* Filesystem match criteria                       */
    public $esistenza     = false;   /* If folder exists                                */


    /**
     * Costruttore
     *
     * @param  string $dir
     * @param  string $match
     * @return o2_dir
     */
    function __construct($dir, $match = "*") {

        $this->tipo          = "D";
        $this->match         = ($match != "" ? $match : "*");
        $dir                 = (realpath($dir) ? realpath($dir) : $dir);
        $parts               = pathinfo($dir);
        $this->ext           = (isset($parts['extension']) ? $parts['extension'] : "");
        $this->nome          = $parts['filename'];
        $this->path          = $parts['dirname'].
                               ($parts['dirname'] != DIRECTORY_SEPARATOR ?
                                DIRECTORY_SEPARATOR : '');
        $this->nome_completo = $dir.(substr($dir, -1) != DIRECTORY_SEPARATOR ?
                                     DIRECTORY_SEPARATOR : "");
        $this->esistenza     = file_exists($this->nome_completo);
        if ($this->esistenza) {
            $timestamp_local     = @filemtime($this->nome_completo);
            $this->data_modifica = date("Ymd", $timestamp_local);
            $this->ora_modifica  = date("His", $timestamp_local);
            }
        else {
            $this->data_modifica = '00000000';
            $this->ora_modifica  = '000000';
            }

        }


    /**
     * Crea la cartella e ritorna true se la creazione ha successo
     *
     * @return boolean
     */
    function crea() {

        if (!$this->esistenza && mkdir($this->nome_completo)) {
            $this->esistenza = true;
            }
        return $this->esistenza;

        }


    /**
     * Ritorna l'elenco di tutti gli elementi di filesystem contenuti nella folder e che
     * soddisfano la maschera $match
     *
     * @param  string $match
     * @return array
     */
    function all_elements($match = "") {

        if (!$match) {
            $match = $this->match;
            }
        // _________________________________________________ Match dot files (.myfile) ___
        if ($match == '*') {
            $match = '{.[!.],}*';
            $opt   = GLOB_BRACE;
            }
        else {
            $opt = null;
            }
        $elements       = array();
        $elements_names = glob($this->nome_completo.$match, $opt);
        if ($elements_names) {
            foreach ($elements_names as $element) {
                $fname = (realpath($element) ? realpath($element) : $element);
                if (is_dir($fname)) {
                    $elements[basename($fname)] = new o2_dir($fname.DIRECTORY_SEPARATOR);
                    }
                else {
                    $elements[basename($fname)] = new o2_fsitem($fname);
                    }
                }
            }
        return $elements;

        }


    /**
     * Removes folder and all its content
     *
     * @return boolean
     */
    function remove() {

        if (!$this->esistenza) {
            return true;
            }
        clearstatcache();
        $res_val  = true;
        $elements = array_diff(scandir($this->nome_completo), Array(".", ".."));
        if ($elements) {
            foreach ($elements as $element) {
                $element = new o2_fsitem($this->nome_completo.$element);
                if ($element->tipo == "D") {
                    $folder  = new o2_dir($element->nome_completo.DIRECTORY_SEPARATOR);
                    $res_val = $res_val && $folder->remove();
                    unset($folder);
                    }
                else {
                    $res_val = $res_val && unlink($element->nome_completo);
                    }
                }
            }
        if ($res_val) {
            $res_val         = rmdir($this->nome_completo);
            $this->esistenza = false;
            }
        return $res_val;

        }


    /**
     * Removes folder elements (files and folders) older than $hours.
     * Returns TRUE on success and FALSE on fails.
     *
     * @param  integer $hours   Number of hours after which elements are expired
     * @return boolean
     */
    function clear_expired($hours = 24) {

        $now = time();
        $ret = true;
        // ___________________________________________________ Clear obsolete elements ___
        foreach ($this->all_elements() as $single_element) {
            $mt  = $single_element->ora_modifica;
            $md  = $single_element->data_modifica;
            // _______________________________________________________ Expiration time ___
            $exp = mktime(intval(substr($mt,  0, 2)) + $hours,
                          intval(substr($mt,  2, 2)),
                          intval(substr($mt, -2   )),
                          intval(substr($md,  4, 2)),
                          intval(substr($md, -2   )),
                          intval(substr($md,  0, 4)));
            if ($exp < $now) {
                if ($single_element->tipo == "D") {
                    $ret = $ret && $single_element->remove();
                    }
                else {
                    $ret = $ret && unlink($single_element->nome_completo);
                    }
                }
            }
        return $ret;

        }


    /**
     * Ritorna l'oggetto o2_fsitem dell'elemento di filesystem richiesto
     *
     * @param  string $file
     * @return o2_elemento_fs
     */
    function element($file) {

        return new o2_fsitem($this->nome_completo.basename($file));

        }


    /**
     * Verifica l'esistenza della cartella (con $file == null) o di $file all'interno
     * della cartella
     *
     * @param  string $file
     * @return boolean
     */
    function exists($file = null) {

        if ($file) {
            $item = new o2_fsitem($this->nome_completo.basename($file));
            return file_exists($item);
            }
        else {
            return $this->esistenza;
            }

        }

    }


/**
 * Database server
 *
 */
class o2_server {

    /*     ===== PROPERTIES =========================================================== */
    public $nome       = ""; /* Name in application repository                          */
    public $type       = ""; /* Engine type                                             */
    public $server     = ""; /* Host name or IP address                                 */
    public $user       = ""; /* User id                                                 */
    public $password   = ""; /* User password                                           */
    public $chunk_size = 0;  /* Data chunk size                                         */


    /**
     * Costruttore
     *
     * @param string  $id
     * @param string  $type
     * @param string  $server
     * @param string  $user
     * @param string  $password
     * @param integer $chunk_size
     */
    function __construct($id,
                         $type       = '',
                         $server     = '',
                         $user       = '',
                         $password   = '',
                         $chunk_size = 0) {

        $this->nome       = $id;
        $this->type       = $type;
        $this->server     = $server;
        $this->user       = $user;
        $this->password   = $password;
        $this->chunk_size = ($chunk_size ?
                             $chunk_size :
                             $GLOBALS['o2_runtime']->def_datachunk);

        }


    /**
     * Esegue un comando sul server di dati
     *
     * @param  string $comando         SQL code to be executed
     * @param  string $for_statement   FALSE for queries, TRUE for statements
     * @return mix
     */
    function esegui($comando = '', $for_statement = false) {

        // _______________________ Execute a statement and return affected rows number ___
        if ($for_statement) {
            return o2_gateway::execute($this->type,
                                       $comando,
                                       $this->server,
                                       $this->user,
                                       $this->password,
                                       true,
                                       true);
            }
        // _________________________________ Execute a query and return result dataset ___
        else {
            return o2_gateway::execute($this->type,
                                       $comando,
                                       $this->server,
                                       $this->user,
                                       $this->password,
                                       false,
                                       false);
            }

        }

    }


/**
 * Database
 *
 */
class o2_db {

    /*     ===== PROPERTIES =========================================================== */
    public $id           = "";    /* Name in application repository                     */
    /**
     * @var o2_server
     */
    public $server       = 0;     /* o2_server object where database is hosted          */
    public $nome         = '';    /* Database physical name                             */
    public $proprietario = '';    /* Owner or schema                                    */
    /*     ===== ASP ================================================================== */
    public $asp          = false; /* Flag that indicates Application Service Provider
                                     support enabled. Allowed values are:
                                     FALSE, [C]olumn, [S]chema, [D]atabase              */
    public $base_name    = '';    /* Original base name to compose with ASPID           */
    public $base_schema  = '';    /* Original base schema to compose with ASPID         */
    /*     ===== OPTIONS ============================================================== */
    public $chunk_size   = 65536; /* Read data chunk size in bytes                      */
    public $create_tabs  = true;  /* If is anabled tables automatic creation            */
    public $sys_release  = false; /* Release of system tables stored on db              */


    /**
     * Costruttore per gli oggetti o2_db
     *
     * @param  string    $id
     * @param  o2_server $server
     * @param  string    $name
     * @param  string    $owner
     * @param  string    $asp
     * @param  boolean   $create_tabs
     * @return o2_db
     */
    function __construct($id,
                         $server,
                         $name,
                         $owner       = "",
                         $asp         = false,
                         $create_tabs = true) {

        if (is_a($server, 'o2_server')) {
            $this->id = $id;
            if ($server->type == "postgres" &&
                strpos($server->server, "host=") === false) {
                $server->server = "host=".$server->server." dbname=".$name;
                }
            $asp_name   = $name;
            $asp_schema = $owner;
            // _____________________________________________________________ Check ASP ___
            if ($asp) {
                $asp = strtoupper($asp);
                // ___________________________________________________ Manage ASP mode ___
                switch ($asp) {
                    case 'D': // _____________________________________ ASP by Database ___
                        $aspid = strtolower($_SESSION['o2_app']->vars['_area']->valore);
                        $asp_name.= ($aspid ? '_'.$aspid : '');
                        break;
                    case 'S': // _______________________________________ ASP by Schema ___
                        $aspid = strtolower($_SESSION['o2_app']->vars['_area']->valore);
                        $asp_schema.= ($aspid ? '_'.$aspid : '');
                        break;
                    default: // ________________________________________ ASP by Column ___
                        $asp = 'C';
                        break;
                    }
                }
            else {
                $asp = false;
                }
            $this->server       = $server;
            $this->nome         = $asp_name;
            $this->proprietario = $asp_schema;
            $this->base_name    = $name;
            $this->base_schema  = $owner;
            $this->asp          = $asp;
            $this->chunk_size   = $server->chunk_size;
            $this->create_tabs  = $create_tabs;
            }
        else {
            throw new o2_exception("Unknown server <i>".$server->nome.
                                   "</i> for database <i>".$name."</i>",
                                   o2error_UNKNOWNDBSERVER);
            }

        }


    /**
     * Ritorna l'array dell'elenco delle tabelle contenute nel database
     *
     * @return array
     */
    function tabelle() {

        $arr_local = o2_gateway::tables($this->server->type,
                                        $this->server->server,
                                        $this->server->user,
                                        $this->server->password,
                                        $this->nome,
                                        $this->proprietario);
        return $arr_local;

        }


    /**
     * Return current system release for database.
     * System release is the release code of system tables in database, if any.
     *
     * @return string
     */
    function get_sysrel() {

        if ($this->sys_release === false) {
            if (in_array("o2sys_dbrelease",
                         o2_gateway::tables($this->server->type,
                                            $this->server->server,
                                            $this->server->user,
                                            $this->server->password,
                                            $this->nome,
                                            $this->proprietario))) {
                $c_o = constant("o2_".$this->server->type."_o");
                $c_c = constant("o2_".$this->server->type."_c");
                $res = o2_gateway::verifyrec($this->server->type,
                                             $this->server->server,
                                             $this->server->user,
                                             $this->server->password,
                                             $this->nome,
                                             $this->proprietario,
                                             "o2sys_dbrelease",
                                             "o2_dbrelease",
                                             $c_o."release_code".$c_c,
                                             "",
                                             "");
                $rel_field = false;
                if (is_array($res)) {
                    if (in_array("release_code", array_keys($res))) {
                        $rel_field = "release_code";
                        }
                    elseif (in_array("RELEASE_CODE", array_keys($res))) {
                        $rel_field = "RELEASE_CODE";
                        }
                    }
                if ($rel_field) {
                    list($ver, $rel)   = explode(".", $res[$rel_field]);
                    $this->sys_release = $ver.".".$rel;
                    }
                else {
                    $this->sys_release = "";
                    }
                }
            else {
                $this->sys_release = "";
                }
            }
        return $this->sys_release;

        }


    /**
     * Sets current system release for database.
     * System release is the release code of system tables in database, if any.
     *
     */
    function set_sysrel($rel_code) {

        if ($this->create_tabs) {
            $rel_tab = $_SESSION['o2_app']->get_table('o2_dbrelease');
            $rel_tab->set_db($this);
            $rel_tab->elimina();
            $rel_tab->crea(true);
            o2_gateway::insertrec($this->server->type,
                                  $this->server->server,
                                  $this->server->user,
                                  $this->server->password,
                                  $this->nome,
                                  $this->proprietario,
                                  $rel_tab->nome,
                                  'o2_dbrelease',
                                  array($rel_tab->campi['release_code']->nome_fisico),
                                  array("'".$rel_code."'"));
            }
        $this->sys_release = $rel_code;

        }

    }


/**
 * System database object. Stores runtime informations.
 *
 */
class o2_sysdb extends o2_db {


    /**
     * Object constructor
     *
     */
    function __construct() {

        $type   = "sqlite3";
        $name   = "jx_sysdb";
        $phys   = $GLOBALS['o2_runtime']->dir_data."sys.db";
        $owner  = "";
        $dbname = "";
        parent::__construct($name,
                            new o2_server($name, $type, $phys),
                            $dbname,
                            $owner,
                            false,
                            true);
        $GLOBALS['o2_runtime']->load_gateway($type);

        }

    }


/**
 * Input/output mask object
 *
 */
class o2_maschera {

    /*     ===== PROPERTIES =========================================================== */
    public $nome         = "";      /* Mask name                                        */
    public $tipo         = "";      /* Base data type:
                                        [N] = Numeric
                                        [A] = Alfanumeric (string)
                                        [D] = Date
                                        [O] = Time
                                        [L] = Boolean
                                        [S] = Structure (object or array)               */
    public $maschera     = "";      /* Picture expression                               */
    public $dimensione   = 0;       /* Actual data maximum dimension in chars           */
    public $default      = "";      /* Default value                                    */
    public $separatore   = "";      /* Char for separating dates, times, and decimals   */
    public $negativo     = false;   /* Negative values allowed (only tipo = "N")        */
    public $prefisso     = "";      /* Prefix string (only tipo = "N")                  */
    public $interi       = 0;       /* Interger part digits (only tipo = "N")           */
    public $decimali     = 0;       /* Decimal part digits (only tipo = "N")            */
    public $migliaia     = "";      /* Thousands separator (only tipo = "N")            */
    public $allineamento = "Left";  /* Alignment (same as HTML align)                   */
    public $valori       = array(); /* Allowed data values                              */
    public $shortyear    = false;   /* Short (2 digits) year (only tipo = "D")          */
    public $zeroff       = false;   /* Shows zero (0) as blank ("") (tipo = "N|D|O")    */
    public $expression   = "";      /* Tagged expression as evaluated from mask         */
    public $zoom_prg     = "";      /* Activation program for the data model            */


    /**
     * Crea un oggetto o2_maschera di tipo [$tipo] e con maschera [$maschera] oppure
     * quella di default per il tipo
     *
     * @param  string $nome
     * @param  string $tipo
     * @param  string $maschera
     * @param  string $zoom_prg
     * @return o2_maschera
     */
    function __construct($nome, $tipo, $maschera = "", $zoom_prg = "") {

        $rnt            = $GLOBALS['o2_runtime'];
        $this->nome     = $nome;
        $this->tipo     = $tipo;
        $this->zoom_prg = $zoom_prg;
        switch ($this->tipo) {
            case "N": // ______________________________________________________ Number ___
                $this->default  = 0;
                $this->maschera = ($maschera ? $maschera : "6");
                $ris_local      = array();
                preg_match('/(\D*\s*)(\d+)([\.,]?)(\d*)(\D*)/',
                           $this->maschera,
                           $ris_local);
                $this->prefisso     = $ris_local[1];
                $this->interi       = intval($ris_local[2]);
                $this->decimali     = intval($ris_local[4]);
                $this->separatore   = $rnt->decimals_point;
                $opt_str            = strtolower($ris_local[5]);
                $this->migliaia     = (strpos($opt_str, "t") !== false ?
                                       $rnt->thousands_point : false);
                $this->zeroff       = (strpos($opt_str, "z") !== false);
                $this->negativo     = (strpos($opt_str, "n") !== false);
                $this->dimensione   = strlen($this->prefisso) +
                                      $this->interi +
                                      $this->decimali +
                                      ($this->decimali && $this->separatore ? 1 : 0) +
                                      ($this->negativo ? 1 : 0) +
                                      ($this->migliaia !== false ?
                                       intval($this->interi / 3) :
                                       0);
                $this->allineamento = (strpos($opt_str, "l") !== false ?
                                       "Left" : "Right");
                break;
            case "D": // ________________________________________________________ Date ___
                $this->maschera     = trim($this->maschera);
                $this->default      = "00000000";
                $this->maschera     = ($maschera ? $maschera : "/");
                $this->prefisso     = "";
                $this->interi       = 0;
                $this->decimali     = 0;
                $this->dimensione   = 10;
                $this->separatore   = substr($this->maschera, 0 ,1);
                $this->shortyear    = strpos($this->maschera, "2") !== false;
                $opt_str            = strtolower($this->maschera);
                $this->zeroff       = strpos($opt_str, "z") !== false;
                $e1                 = (substr($opt_str, 1, 1));
                $e2                 = (substr($opt_str, 2, 1));
                $e2                 = ($e2 == "2" || $e2 == "z" ? "" : $e2);
                $e3                 = (substr($opt_str, 3, 1));
                $e3                 = ($e3 == "2" || $e3 == "z" ? "" : $e3);
                if (($e1.$e2.$e3 == '')) {
                    $e1 = 'd';
                    $e2 = 'm';
                    $e3 = 'y';
                    }
                $this->expression   = ($e1 ? $e1.($e2.$e3 ? $this->separatore : '') : '').
                                      ($e2 ? $e2.($e3 ? $this->separatore : '') : '').$e3;
                $this->allineamento = (strpos($opt_str, "r") !== false ?
                                       "Right" : "Left");
                break;
            case "O": // ________________________________________________________ Time ___
                $this->maschera     = trim($this->maschera);
                $this->default      = "000000";
                $this->maschera     = ($maschera ? $maschera : ":");
                $this->prefisso     = "";
                $this->interi       = 0;
                $this->decimali     = 0;
                $this->dimensione   = 8;
                $this->separatore   = substr($this->maschera, 0 ,1);
                $opt_str            = strtolower($this->maschera);
                $this->zeroff       = (strpos($opt_str, "z") !== false);
                $e1                 = (substr($opt_str, 1, 1));
                $e2                 = (substr($opt_str, 2, 1));
                $e2                 = ($e2 == "z" ? "" : $e2);
                $e3                 = (substr($opt_str, 3, 1));
                $e3                 = ($e3 == "z" ? "" : $e3);
                if (($e1.$e2.$e3 == '')) {
                    $e1 = 'h';
                    $e2 = 'm';
                    $e3 = 's';
                    }
                $this->expression   = ($e1 ? $e1.($e2.$e3 ? $this->separatore : '') : '').
                                      ($e2 ? $e2.($e3 ? $this->separatore : '') : '').$e3;
                $this->allineamento = (strpos($opt_str, "r") !== false ?
                                       "Right" : "Left");
                break;
            case "L": // _____________________________________________________ Boolean ___
                $this->default      = "0";
                $this->maschera     = ($maschera  ? $maschera : "");
                $this->prefisso     = "";
                $this->interi       = 1;
                $this->decimali     = 0;
                $this->dimensione   = 1;
                $this->separatore   = "";
                $this->allineamento = "Right";
                $ris_local          = array();
                preg_match_all('/\[(.*)\]/i', $this->maschera, $ris_local);
                if (isset($ris_local[1][0])) {
                    $rep_str            = str_replace($ris_local[1][0],
                                                      "",
                                                      $this->maschera);
                    $this->allineamento = (stripos($rep_str, "l") !== false ?
                                           "Left" : "Right");
                    $this->valori       = preg_split ('/\|/', $ris_local[1][0]);
                    }
                else {
                    $this->allineamento = (stripos($rep_str, "l") !== false ?
                                           "Left" : "Right");
                    $this->valori       = array();
                    }
                break;
            case "A": // _______________________________________________________ Alpha ___
                $this->default  = "";
                $this->maschera = ($maschera ? $maschera : "C10");
                $ris_local      = array();
                preg_match_all('/([ULANC])\s*(\d+)/i', $this->maschera, $ris_local);
                $this->prefisso     = "";
                $this->interi       = 0;
                $this->decimali     = 0;
                $this->dimensione   = array_sum($ris_local[2]);
                $this->separatore   = "";
                $this->allineamento = "Left";
                $ris_local          = array();
                preg_match_all('/\[(.*)\]/i', $this->maschera, $ris_local);
                if (isset($ris_local[1][0])) {
                    $rep_str            = str_replace($ris_local[1][0],
                                                      "",
                                                      $this->maschera);
                    $this->allineamento = (stripos($rep_str, "r") !== false ?
                                           "Right" : "Left");
                    $this->valori       = array();
                    foreach (preg_split ('/\|/', $ris_local[1][0]) as $value) {
                        $key = substr($value, 0, $this->dimensione);
                        if ($key === false) {
                            $key = '';
                            }
                        $this->valori[$key] = ($value === '' ? ' ' : $value);
                        }
                    }
                else {
                    $this->allineamento = (stripos($this->maschera, "r") !== false ?
                                           "Right" : "Left");
                    $this->valori       = array();
                    }
                break;
            case "S": // ___________________________________________________ Structure ___
                $this->default      = array();
                $this->maschera     = "";
                $this->prefisso     = "";
                $this->interi       = 0;
                $this->decimali     = 0;
                $this->dimensione   = 0;
                $this->separatore   = "";
                $this->allineamento = "Left";
                break;
            default: // ________________________________________________ Other formats ___
                $this->default      = "";
                $this->maschera     = "";
                $this->prefisso     = "";
                $this->interi       = 0;
                $this->decimali     = 0;
                $this->dimensione   = 0;
                $this->separatore   = "";
                $this->allineamento = "Left";
                break;
            }

        }

    }


/**
 * Field of db table
 * ancestor class for o2_campo_task and o2_appvar
 *
 */
class o2_campo {

    /*     ===== PROPERTIES =========================================================== */
    public $file        = ""; /* Table name where field is defined                      */
    public $nome_fisico = ""; /* Field physical name with delimiters                    */
    public $phys_name   = ""; /* Field physical name without delimiters                 */
    public $tipo        = ""; /* Data type: [N|A|L|D|O|S] from mask                     */
    /**
     * @var o2_maschera
     */
    public $maschera    = ""; /* o2_maschera object defining field                      */


    /**
     * Costruttore
     *
     * @param  string $tabella
     * @param  string $nome_fisico
     * @param  string $maschera
     * @return o2_campo
     */
    function __construct($tabella,
                         $nome_fisico    = "",
                         $delimited_name = "",
                         $maschera       = "") {

        $app               = $_SESSION['o2_app'];
        $this->file        = $tabella;
        $this->nome_fisico = $delimited_name;
        $this->phys_name   = $nome_fisico;
        if (!$maschera) {
            $this->tipo = "F";
            return ;
            }
        if (isset($app->maschere[$maschera])) {
            $this->maschera = $app->maschere[$maschera];
            $this->tipo     = $this->maschera->tipo;
            }
        else {
            throw new o2_exception("Unknown data model <i>".$maschera.
                                   "</i> requested for field <i>".$nome_fisico.
                                   "</i> in table <i>".$tabella."</i>",
                                   o2error_UNKNOWNMODEL);
            }

        }


    function __toString() {

        return $this->nome_fisico;

        }

    }


/**
 * Table index segment
 *
 */
class o2_segmento_indice {

    /*     ===== PROPERTIES =========================================================== */
    public $campo       = "";    /* Field being the segment                             */
    public $direzione   = "A";   /* Direction: [A] = ASCENDING, [D] = DESCENDING        */
    public $custom_sort = false; /* If segment is part of a user custom sorting         */


    function __construct($campo, $direzione) {

        $this->campo     = $campo;
        $this->direzione = strtoupper(trim($direzione));
        $this->direzione = ($this->direzione{0} == "D" ? "D" : "A");

        }
    }


/**
 * Table index
 *
 */
class o2_chiave {

    /*     ===== PROPERTIES =========================================================== */
    public $nome     = "";      /* Index name                                           */
    public $segmenti = array(); /* Segments list                                        */
    public $unique   = true;    /* Unique: only unique indexes are usable in views      */


    /**
     * Costruttore
     *
     * @param  $con_nome string
     * @param  $segmenti array
     * @return o2_chiave
     */
    function __construct($con_nome, $segmenti, $unique = true) {

        $this->nome     = $con_nome;
        $this->segmenti = $segmenti;
        $this->unique   = $unique;

        }


    /**
     * Ritorna la clausola di ORDER BY SQL per la tabella con alias [$tabella].
     * Con il parametro [$invertito] passato a TRUE ritorna la chiave inversa.
     *
     * @param  o2_view $view
     * @param  string  $table
     * @param  boolean $invertito
     * @return string
     */
    function order_by($view, $table = "", $invertito = false) {

        $asp_var = $_SESSION['o2_app']->vars['_area'];
        if (!$table) {
            $table = $view->file->indice;
            }
        $table_obj      = $view->files[$table];
        $file_type      = $table_obj->db->server->type;
        $c_o            = constant("o2_".$file_type."_o");
        $c_c            = constant("o2_".$file_type."_c");
        $full_tab_alias = $c_o.$table.$c_c;
        foreach ($this->segmenti as $singolo_segmento) {
            $str_local.= ($str_local ? "," : "");
            if ($singolo_segmento->custom_sort) {
                $nulls_first = $file_type == "postgres" || $file_type == "oracle";
                $asc         = " ASC".($nulls_first ? " NULLS FIRST" : "");
                $desc        = " DESC".($nulls_first ? " NULLS LAST" : "");
                $str_local  .= $c_o.$singolo_segmento->campo->file.$c_c.".".
                               $singolo_segmento->campo->nome_fisico.
                               ($singolo_segmento->direzione == "D" ?
                                ($invertito ? $asc : $desc) :
                                ($invertito ? $desc : $asc));
                }
            else {
                $nulls_first = $file_type == "oracle";
                $asc         = " ASC".($nulls_first ? " NULLS FIRST" : "");
                $desc        = " DESC".($nulls_first ? " NULLS LAST" : "");
                $str_local.= $full_tab_alias.".".
                             $singolo_segmento->campo->nome_fisico.
                             ($singolo_segmento->direzione == "D" ?
                              ($invertito ? $asc : $desc) :
                              ($invertito ? $desc : $asc));
                }
            }
        return $str_local;

        }

    }


/**
 * Database table
 *
 */
class o2_file {

    /*     ===== PROPERTIES =========================================================== */
    public $indice       = "";      /* Nome univoco della tabella nel repository        */
    public $repos_index  = "";      /* Nome univoco in repository conservato nelle view */
    public $id           = 0;       /* Indice numerico univoco per la tabella           */
    /**
     * @var o2_db
     */
    public $db           = "";      /* Oggetto o2_db del database della tabella         */
    public $nome         = "";      /* Stringa del nome fisico del file                 */
    public $log_table    = "";      /* Nome dell'o2_file object della tabella di log    */
    public $asp          = false;   /* Flag that indicates Application Service Provider
                                       support enabled (from database). Allowed values
                                       are: FALSE, [C]olumn, [S]chema, [D]atabase       */
    public $campi        = array(); /* Array degli oggetti o2_campo della tabella       */
    public $chiavi       = array(); /* Array degli oggetti o2_chiave della tabella      */
    public $fix_nukeys   = array(); /* Array of not unique keys with mssing pk-segments */
    /**
     * @var o2_chiave
     */
    public $chiave       = "";      /* Oggetto o2_chiave chiave primaria della tabella  */
    public $pk_segments  = array(); /* List of PKey segments <phys_name> => <logic_name>*/
    public $batch_chunk  = 1024;    /* Chunk of records to retrieve when looping on file*/
    public $log_level    = false;   /* Active log level: [R]ecord[D]elete[I]nsert[B]oth */
    public $record_trace = false;   /* Active trace fields: cu, cd, ct, uu, ud, ut      */
    public $subquery     = false;   /* If <table> clausole is a SQL sub-select query    */
    public $write_name   = false;   /* Table name for write operations when subquery    */
    public $sys_tab      = false;   /* If table is defined in runtime only              */


    /**
     * Constructor
     *
     * @param string $con_nome
     * @param string $database
     * @param string $nome_fisico
     * @param string $chiave_primaria
     */
    function __construct($con_nome, $database, $nome_fisico, $chiave_primaria) {

        $app               = $_SESSION['o2_app'];
        $this->indice      = $con_nome;
        $this->repos_index = $con_nome;
        $this->db          = $app->db[$database];
        $this->nome        = $nome_fisico;
        $this->write_name  = $nome_fisico;
        $this->chiave      = $chiave_primaria;
        $this->log_table   = $con_nome."_log";
        $this->asp         = $app->db[$database]->asp;
        $this->sys_tab     = $app->runtime->read_sys_tabs;
        $app->runtime->load_gateway($this->db->server->type);

        }


    /**
     * Method called on cloning table. Fields and keys are cloned too to allow modifing.
     *
     */
    function __clone() {

        // ______________________________________________________________ Clone fields ___
        $fields = array();
        foreach ($this->campi as $f_id => $field) {
            $fields[$f_id] = clone $field;
            }
        unset($this->campi);
        $this->campi = $fields;
        // _______________________________________ Re-reference fields objects in keys ___
        foreach ($this->chiavi as $k_id => $key) {
            $this->chiavi[$k_id] = clone $key;
            foreach ($key->segmenti as $s_id => $segment) {
                $this->chiavi[$k_id]->segmenti[$s_id] = clone $segment;
                $f_name = $segment->campo->name;
                unset($this->chiavi[$k_id]->segmenti[$s_id]->campo);
                $this->chiavi[$k_id]->segmenti[$s_id]->campo = $this->campi[$f_name];
                }
            }
        // __________________________________________________ Re-reference primary key ___
        $pkey = $this->chiave->nome;
        unset($this->chiave);
        $this->chiave = $this->chiavi[$pkey];

        }


    /**
     * Returns physical table informations as an array.
     * If parameter $with_constraints is passed as TRUE then table contraints informations
     * are returned too.
     * If parameter $with_extraidxs is passed as TRUE then table extra indexes
     * informations are returned too.
     *
     * @param  boolean $with_constraints
     * @param  boolean $with_extraidxs
     * @return mix
     */
    function info($with_constraints = false, $with_extraidxs = false) {

        $app                = $_SESSION['o2_app'];
        $tab_exists         = false;
        $tab_data           = false;
        $phys_fields        = array();
        $def_fields         = array();
        $var_fields         = array();
        $def_keys           = array();
        $var_keys           = array();
        $fkeys_active       = array();
        $campi_comuni       = array();
        $campi_solo_fisico  = array();
        $campi_solo_ideale  = array();
        $chiavi_solo_fisico = array();
        $chiavi_solo_ideale = array();
        // ________________________________________________________ PHYSICAL STRUCTURE ___
        if ($this->exists()) {
            $tab_exists = true;
            // _____________________________________________ Physical structure fields ___
            $arr_fields_local = o2_gateway::tablefields($this->db->server->type,
                                                        $this->db->server->server,
                                                        $this->db->server->user,
                                                        $this->db->server->password,
                                                        $this->db->nome,
                                                        $this->db->proprietario,
                                                        $this->nome);
            // _______________________________________________ Physical structure keys ___
            $arr_keys_local   = o2_gateway::tableindexes($this->db->server->type,
                                                         $this->db->server->server,
                                                         $this->db->server->user,
                                                         $this->db->server->password,
                                                         $this->db->nome,
                                                         $this->db->proprietario,
                                                         $this->nome);
            // _________________________________________________ Data present in table ___
            $tab_data         = $this->data();
            // __________________________________________________________ Foreign keys ___
            if ($with_constraints) {
                $fkeys_active = o2_gateway::fkeystablist($this->db->server->type,
                                                         $this->db->server->server,
                                                         $this->db->server->user,
                                                         $this->db->server->password,
                                                         $this->db->nome,
                                                         $this->db->proprietario,
                                                         $this->nome);
                }
            }
        // ______________________________________________________________ ASP id field ___
        if ($this->asp == 'C') {
            $aspid = "O2ASPID";
            if (is_array($arr_fields_local)) {
                foreach ($arr_fields_local as $phys_field) {
                    if (strtoupper($phys_field['field']) == "O2ASPID") {
                        $aspid = $phys_field['field'];
                        break;
                        }
                    }
                }
            $def_fields[$aspid] = array('type' => "alpha",
                                        'dim'  => intval($app->vars['_area']->maschera->
                                                                              dimensione),
                                        'meta' => 'alpha',
                                        'name' => 'O2ASPID');
            }
        // ________________________________________________ JANOX DEFINITION STRUCTURE ___
        // _________________________________________ Janox definition structure fields ___
        foreach ($this->campi as $field_name => $single_field) {
            $meta = "";
            $type = "";
            switch ($single_field->tipo) {
                case "A":
                    $meta = "alpha";
                    $type = "alpha";
                    $dim  = intval($single_field->maschera->dimensione);
                    break;
                case "N":
                    $meta = "number";
                    $type = "number";
                    $dim  = intval($single_field->maschera->dimensione);
                    break;
                case "D":
                    $meta = "date";
                    $type = "alpha";
                    $dim  = 8;
                    break;
                case "O":
                    $meta = "time";
                    $type = "alpha";
                    $dim  = 6;
                    break;
                case "L":
                    $meta = "boolean";
                    $type = "alpha";
                    $dim  = 1;
                    break;
                default:
                    $meta = "structured";
                    $type = "alpha";
                    $dim  = 0;
                    break;
                }
            $def_fields[$single_field->phys_name] = array('type' => $type,
                                                          'dim'  => $dim,
                                                          'meta' => $meta,
                                                          'name' => $field_name);
            }
        // ___________________________________________ Janox definition structure keys ___
        foreach ($this->chiavi as $single_key) {
            $segms_local = array();
            if ($this->asp == 'C') {
                $segms_local[] = array('column' => $aspid, 'dir' => "A");
                }
            foreach ($single_key->segmenti as $single_segment) {
                $segms_local[] = array('column' => $single_segment->campo->phys_name,
                                       'dir'    => $single_segment->direzione);
                }
            $def_keys[$single_key->nome] = array('extra'    => false,
                                                 'unique'   => $single_key->unique,
                                                 'segments' => $segms_local);
            }
        // _____________________________________________________________ Extra indexes ___
        if ($with_extraidxs) {
            $eidxs_tab    = $app->get_table('o2_extraindexes');
            $c_o          = constant("o2_".$eidxs_tab->db->server->type."_o");
            $c_c          = constant("o2_".$eidxs_tab->db->server->type."_c");
            $eidxs_select = $eidxs_tab->campi['index_name']->nome_fisico.' '.
                            $c_o.'IDX_NAME'.$c_c.','.
                            $eidxs_tab->campi['index_fields']->nome_fisico.' '.
                            $c_o.'IDX_FIELDS'.$c_c.','.
                            $eidxs_tab->campi['index_unique']->nome_fisico.' '.
                            $c_o.'IDX_UNIQUE'.$c_c;
            $eidxs_where  = $eidxs_tab->campi['index_table']->nome_fisico."='".
                            $this->indice."'";
            $eidxs_order  = '';
            foreach (o2_gateway::recordset($eidxs_tab->db->server->type,
                                           $eidxs_tab->db->server->server,
                                           $eidxs_tab->db->server->user,
                                           $eidxs_tab->db->server->password,
                                           $eidxs_tab->db->nome,
                                           $eidxs_tab->db->proprietario,
                                           $eidxs_tab->nome,
                                           $eidxs_tab->nome,
                                           $eidxs_select,
                                           $eidxs_where,
                                           $eidxs_order,
                                           1000) as $eidx_def) {
                $segms_local = array();
                if ($this->asp == 'C') {
                    $segms_local[] = array('column' => $aspid, 'dir' => "A");
                    }
                foreach (explode(',', $eidx_def['IDX_FIELDS']) as $single_segment) {
                    list($idx_fld, $idx_dir) = explode('|', $single_segment);
                    $segms_local[] = array('column' => $idx_fld, 'dir' => $idx_dir);
                    }
                $def_keys[$eidx_def['IDX_NAME']] = array('extra'    => true,
                                                      'unique' => $eidx_def['IDX_UNIQUE'],
                                                         'segments' => $segms_local);
                }
            }
        // ______________________________ Comparing definition and physical structures ___
        // __________________________________________________________ Comparing fields ___
        if (is_array($arr_fields_local)) {
            foreach ($arr_fields_local as $single_field) {
                $parts = array();
                $type  = "";
                $dim   = 0;
                if (strpos($single_field['type'], "(") !== false) {
                    preg_match_all('/(\D*).*\((\d*)\)/', $single_field['type'], $parts);
                    $type = strtolower($parts[1][0]);
                    $dim  = intval($parts[2][0]);
                    }
                else {
                    $type = strtolower($single_field['type']);
                    }
                if (strpos($type, "char") !== false || strpos($type, "text") !== false) {
                    $type = "alpha";
                    }
                else {
                    $type = "number";
                    }
                $field_name               = $single_field['field'];
                $phys_fields[$field_name] = array('type' => $type,
                                                  'dim'  => $dim);
                if (isset($def_fields[$field_name])) {
                    $campi_comuni[$field_name] = $phys_fields[$field_name];
                    if ($type != $def_fields[$field_name]['type']   ||
                       ($type != "number"                           &&
                        $type != "structure"                        &&
                        (($def_fields[$field_name]['dim'] < 256     &&
                          $dim  != $def_fields[$field_name]['dim']) ||
                         ($def_fields[$field_name]['dim'] > 255     &&
                          $dim > 0                                  &&
                          $dim != $def_fields[$field_name]['dim'])))) {
                        $var_fields[$field_name] = $def_fields[$field_name];
                        }
                    }
                else {
                    $campi_solo_fisico[$field_name] = $phys_fields[$field_name];
                    }
                }
            }
        foreach ($def_fields as $field_name => $single_field) {
            if (!array_key_exists($field_name, $phys_fields)) {
                $campi_solo_ideale[$field_name] = $single_field;
                }
            }
        // ____________________________________________________________ Comparing keys ___
        if (is_array($arr_keys_local)) {
            // _________________________________________________ Loop on physical keys ___
            foreach ($arr_keys_local as $key_name => $single_key) {
                // _______________________ If physical key match a defined key by name ___
                if (isset($def_keys[$key_name])) {
                    // ________________________________________ Key changed uniqueness ___
                    if ($def_keys[$key_name]['unique'] != $single_key['unique']) {
                        $var_keys[$key_name] = $def_keys[$key_name];
                        }
                    // _______________________________ If segments number is different ___
                    elseif (count($def_keys[$key_name]['segments']) !=
                        count($single_key['segments'])) {
                        $var_keys[$key_name] = $def_keys[$key_name];
                        }
                    // ____ If physical and defined keys have the same segments number ___
                    else {
                        // _____________________________ Loop on physical key segments ___
                        foreach ($single_key['segments'] as $segm_index =>
                                                                        $single_segment) {
                            if (($def_keys[$key_name]['segments'][$segm_index]['column']
                                 != $single_segment['column']) ||
                                ($single_segment['dir'] &&
                                ($def_keys[$key_name]['segments'][$segm_index]['dir'] !=
                                 $single_segment['dir']))) {
                                $var_keys[$key_name] = $def_keys[$key_name];
                                }
                            }
                        }
                    }
                // _______________ If physical key name does not exist in defined list ___
                else {
                    $chiavi_solo_fisico[$key_name] = $single_key;
                    }
                }
            // __________________________________________________ Loop on defined keys ___
            foreach ($def_keys as $key_name => $single_key) {
                // _________________________________ If found an unmatched defined key ___
                if (!array_key_exists($key_name, $arr_keys_local)) {
                    $matched = false;
                    // _________________________________________ Loop on physical keys ___
                    foreach ($arr_keys_local as $ph_key_name => $ph_key) {
                        // ______________________________ Compare indexes by structure ___
                        if ($single_key == $ph_key) {
                            $matched = true;
                            unset($chiavi_solo_fisico[$ph_key_name]);
                            }
                        }
                    // ____________________ If no match add key to "only defined" list ___
                    if (!$matched) {
                        $chiavi_solo_ideale[$key_name] = $single_key;
                        }
                    }
                }
            }
        // ____________________________________________________________ Record tracing ___
        if ($app->record_trace && is_array($this->record_trace)) {
            $trace = implode(",", array_keys($this->record_trace));
            }
        else {
            $trace = false;
            }
        // ______________________________________________________________ Foreign keys ___
        if ($with_constraints) {
            $fkeys_tab    = $app->get_table('o2_constraints');
            $fkeys_select = $fkeys_tab->campi['constraint_name']->nome_fisico;
            $fkeys_where  = $fkeys_tab->campi['main_table']->nome_fisico."='".
                            $this->indice."'";
            $fkeys_order  = '';
            $fkeys_def    = array();
            $fkeys_less   = array();
            foreach (o2_gateway::recordset($fkeys_tab->db->server->type,
                                           $fkeys_tab->db->server->server,
                                           $fkeys_tab->db->server->user,
                                           $fkeys_tab->db->server->password,
                                           $fkeys_tab->db->nome,
                                           $fkeys_tab->db->proprietario,
                                           $fkeys_tab->nome,
                                           $fkeys_tab->nome,
                                           $fkeys_select,
                                           $fkeys_where,
                                           $fkeys_order,
                                           1000) as $fkey_def) {
                $fkeys_def[] = $fkey_def['constraint_name'];
                }
            foreach ($fkeys_def as $fkey) {
                if (!in_array($fkey, $fkeys_active)) {
                    $fkeys_less[] = $fkey;
                    }
                }
            $fkeys_more = array_diff($fkeys_active, $fkeys_def);
            }
        // __________________________________________________ Composing info structure ___
        $ret_obj = array('exist'  => $tab_exists,
                         'data'   => $tab_data,
                         'log'    => $this->log_level,
                         'trace'  => ($trace ? $trace : false),
                         'fields' => array('def'  => $def_fields,
                                           'phys' => $phys_fields,
                                           'less' => $campi_solo_ideale,
                                           'more' => $campi_solo_fisico,
                                           'var'  => $var_fields),
                         'keys'   => array('def'  => $def_keys,
                                           'phys' => $arr_keys_local,
                                           'less' => $chiavi_solo_ideale,
                                           'more' => $chiavi_solo_fisico,
                                           'var'  => $var_keys),
                         'fkeys'  => ($with_constraints ?
                                      array('def'  => $fkeys_def,
                                            'phys' => $fkeys_active,
                                            'less' => $fkeys_less,
                                            'more' => $fkeys_more) :
                                      array()));
        return $ret_obj;

        }


    /**
     * Rebuilt physical table structure according with o2 definition.
     * This method upgrades table structure to actual o2 structure.
     *
     * @param array   $corrispondenze   Array of fields matches
     * @param string  $backup_name      Name for old table for backup (instead of drop)
     */
    function ricostruisci($corrispondenze = null, $backup_name = false) {

        // _______________________________________________________ Table contains data ___
        if ($this->data()) {
            $arr_local           = array();
            $def_fields          = array();
            $phys_fields         = array();
            $tabella_info        = $this->info();
            $engine              = $this->db->server->type;
            $c_o                 = constant("o2_".$engine."_o");
            $c_c                 = constant("o2_".$engine."_c");
            // ______________________________________________________ Create TMP-table ___
            $tabella_local       = clone $this;
            if ($engine == "oracle") {
                list ($mt, $t)       = explode(" ", microtime(false));
                $mt                  = substr($mt, 2, -2);
                $tabella_local->nome = "rebuild_".date("His").$mt;
                }
            else {
                $tabella_local->nome = $this->nome."_".
                                       preg_replace('/[^a-zA-Z0-9]/', '_',
                                                    $_SESSION['o2_app']->user).
                                       mt_rand(11111111, 99999999);
                }
            $tabella_local->crea();
            // ______________________________________________ Error creating TMP-table ___
            if (!$tabella_local->exists()) {
                throw new o2_exception("<b>UNABLE TO CREATE</b> table <i>".
                                       $tabella_local->nome.": can't rebuild table <i>".
                                       $this->indice."</i> (<i>".$this->nome."</i>).",
                                       o2error_DBTABLEREBUILD);
                return false;
                }
            // ___________________ Retrieves defined fields physical names in an array ___
            $def_fields  = array_keys($tabella_info['fields']['def']);
            // __________________ Retrieves existing fields physical names in an array ___
            $phys_fields = array_keys($tabella_info['fields']['phys']);
            // ________________________________________________ Select matching fields ___
            if (is_array($corrispondenze)) {
                foreach ($corrispondenze as $campo_fisico => $campo_ideale) {
                    if (in_array($campo_fisico, $phys_fields) &&
                        in_array($campo_ideale, $def_fields)) {
                        $arr_local[$c_o.$campo_fisico.$c_c] = $c_o.$campo_ideale.$c_c;
                        }
                    // ______________________________________ Error in matching fields ___
                    elseif (!in_array($campo_fisico, $def_fields) ||
                            !in_array($campo_ideale, $def_fields)) {
                        throw new o2_exception("<b>ERROR MATCHING</b> fields <i>".
                                               $campo_fisico."/".$campo_ideale.
                                               "</i> in table <i>".$this->indice.
                                               "</i> (<i>".$this->nome."</i>)",
                                               o2error_DBTABLEREBUILD);
                        return false;
                        }
                    }
                }
            // _________________________________________ Compose matching fileds array ___
            foreach ($def_fields as $single_field) {
                if (!in_array($single_field, $arr_local)  &&
                     in_array($single_field, $phys_fields)) {
                    $arr_local[$c_o.$single_field.$c_c] = $c_o.$single_field.$c_c;
                    }
                }
            // _______________________________ Insert from original table to TMP-table ___
            o2_gateway::insertfrom($this->db->server->type,
                                   $this->db->server->server,
                                   $this->db->server->user,
                                   $this->db->server->password,
                                   $this->db->nome,
                                   $this->db->proprietario,
                                   $this->nome,
                                   $tabella_local->db->nome,
                                   $tabella_local->db->proprietario,
                                   $tabella_local->nome,
                                   $arr_local,
                                   "");
            // ________________________________________________ Error duplicating data ___
            if (!$tabella_local->data()) {
                throw new o2_exception("<b>UNABLE TO INSERT</b> records in table <i>".
                                       $this->indice."</i> (<i>".$this->nome."</i>).",
                                       o2error_DBTABLEREBUILD);
                return false;
                }
            // _____________________________________ Create backup from original table ___
            if ($backup_name) {
                $backup_tab = clone $this;
                $backup_tab->rinomina($backup_name);
                unset($backup_tab);
                }
            // _________________________________________________ Delete original table ___
            $this->elimina();
            // _____________________________________ Rename TMP-table to original name ___
            $tabella_local->rinomina($this->nome);
            unset($tabella_local);
            }
        // _______________________________________________________________ Empty table ___
        else {
            $this->elimina();
            }
        // _______________________________________________ Create new structured table ___
        $this->crea();
        // _____________________ Rebuild related log table (if not a log table itself) ___
        if (substr($this->nome, -6) != "_o2log") {
            $this->rebuild_log($corrispondenze);
            }
        return true;

        }


    /**
     * Rebuilt physical log table structure according o2 definition.
     * This method upgrades log table structure to actual o2 table structure.
     *
     * Rebuild method depends on two janox.ini parameters:
     *
     *  log_rebuild (default TRUE):    If TRUE log table is rebuilt to new structure
     *                                 saving data. If FALSE table is dropped.
     *  log_backup (default TRUE):     If TRUE log table is backupped before being rebuilt
     *                                 or dropped.
     *
     * @param array   $matches   Array of fields matches
     */
    function rebuild_log($matches = null) {

        $app    = $_SESSION['o2_app'];
        $backup = ($app->log_backup ?
                   $this->nome."_log_".date("Ymd_His")."_".
                   intval(strtok(microtime(), " ") * 1000000) :
                   false);
        if (substr($this->nome, -6) != "_o2log" && $app->tables_log && $this->log_level) {
            $log_tab = $this->logtable();
            if ($app->log_rebuild) {
                $log_tab->ricostruisci($matches, $backup);
                }
            else {
                $log_tab->elimina($backup);
                $log_tab->crea();
                }
            }

        }


    /**
     * Add new column to table
     *
     * @param  string  $name   New column name
     * @param  string  $type   New column type
     * @param  integer $int    New column size or integer digits
     * @param  integer $dec    New column decimal digits
     * @return boolean
     */
    function alter_column_add($name, $type, $int, $dec = 0) {

        if ($this->exists() && $this->db->create_tabs) {
            o2_gateway::fieldadd($this->db->server->type,
                                 $this->db->server->server,
                                 $this->db->server->user,
                                 $this->db->server->password,
                                 $this->db->nome,
                                 $this->db->proprietario,
                                 $this->nome,
                                 $name,
                                 $type,
                                 $int,
                                 $dec);
            // _____________ Alter log table too or drop it, according with parameters ___
            $app = $_SESSION['o2_app'];
            if ($app->tables_log &&
                $this->log_level &&
                substr($this->nome, -6) != "_o2log") {
                $log_tab = $this->logtable();
                if ($app->log_rebuild) {
                    $log_tab->alter_column_add($name, $type, $int, $dec);
                    }
                else {
                    $backup = ($app->log_backup ?
                               $this->nome."_log_".date("Ymd_His")."_".
                               intval(strtok(microtime(), " ") * 1000000) :
                               false);
                    $log_tab->elimina($backup);
                    $log_tab->crea();
                    }
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Remove column from table
     *
     * @param  string  $name   Column name
     * @return boolean
     */
    function alter_column_remove($name) {

        if ($this->exists() && $this->db->create_tabs) {
            o2_gateway::fieldremove($this->db->server->type,
                                    $this->db->server->server,
                                    $this->db->server->user,
                                    $this->db->server->password,
                                    $this->db->nome,
                                    $this->db->proprietario,
                                    $this->nome,
                                    $name);
            // _____________ Alter log table too or drop it, according with parameters ___
            $app = $_SESSION['o2_app'];
            if ($app->tables_log &&
                $this->log_level &&
                substr($this->nome, -6) != "_o2log") {
                $log_tab = $this->logtable();
                if ($app->log_rebuild) {
                    $log_tab->alter_column_remove($name);
                    }
                else {
                    $backup = ($app->log_backup ?
                               $this->nome."_log_".date("Ymd_His")."_".
                               intval(strtok(microtime(), " ") * 1000000) :
                               false);
                    $log_tab->elimina($backup);
                    $log_tab->crea();
                    }
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Change column definition in table
     *
     * @param  string  $name   Column name
     * @param  string  $type   Column type
     * @param  integer $int    Column size or integer digits
     * @param  integer $dec    Column decimal digits
     * @return boolean
     */
    function alter_column_change($name, $type, $int, $dec = 0) {

        if ($this->exists() && $this->db->create_tabs) {
            o2_gateway::fieldchange($this->db->server->type,
                                    $this->db->server->server,
                                    $this->db->server->user,
                                    $this->db->server->password,
                                    $this->db->nome,
                                    $this->db->proprietario,
                                    $this->nome,
                                    $name,
                                    $type,
                                    $int,
                                    $dec);
            // _____________ Alter log table too or drop it, according with parameters ___
            $app = $_SESSION['o2_app'];
            if ($app->tables_log &&
                $this->log_level &&
                substr($this->nome, -6) != "_o2log") {
                $log_tab = $this->logtable();
                if ($app->log_rebuild) {
                    $log_tab->alter_column_change($name, $type, $int, $dec);
                    }
                else {
                    $backup = ($app->log_backup ?
                               $this->nome."_log_".date("Ymd_His")."_".
                               intval(strtok(microtime(), " ") * 1000000) :
                               false);
                    $log_tab->elimina($backup);
                    $log_tab->crea();
                    }
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Rename column in table
     *
     * @param  string  $old_name   Old column name
     * @param  string  $new_name   New column name
     * @return boolean
     */
    function alter_column_rename($old_name, $new_name) {

        if ($this->exists() && $this->db->create_tabs) {
            o2_gateway::fieldrename($this->db->server->type,
                                    $this->db->server->server,
                                    $this->db->server->user,
                                    $this->db->server->password,
                                    $this->db->nome,
                                    $this->db->proprietario,
                                    $this->nome,
                                    $old_name,
                                    $new_name);
            // _____________ Alter log table too or drop it, according with parameters ___
            $app = $_SESSION['o2_app'];
            if ($app->tables_log &&
                $this->log_level &&
                substr($this->nome, -6) != "_o2log") {
                $log_tab = $this->logtable();
                if ($app->log_rebuild) {
                    $log_tab->alter_column_rename($old_name, $new_name);
                    }
                else {
                    $backup = ($app->log_backup ?
                               $this->nome."_log_".date("Ymd_His")."_".
                               intval(strtok(microtime(), " ") * 1000000) :
                               false);
                    $log_tab->elimina($backup);
                    $log_tab->crea();
                    }
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Add new index to table
     *
     * @param  string  $name       New index name
     * @param  array   $key_segs   New index segments
     * @param  boolean $unique     New index is unique
     * @return boolean
     */
    function alter_index_add($name, $key_segs, $unique = true) {

        if ($this->exists() && $this->db->create_tabs) {
            if ($this->asp == 'C' && !array_key_exists('O2ASPID', $key_segs)) {
                $key_segs = array('O2ASPID' => 'A') + $key_segs;
                }
            o2_gateway::indexadd($this->db->server->type,
                                 $this->db->server->server,
                                 $this->db->server->user,
                                 $this->db->server->password,
                                 $this->db->nome,
                                 $this->db->proprietario,
                                 $this->nome,
                                 $name,
                                 $key_segs,
                                 $unique);
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Remove an existing index from table
     *
     * @param  string $name   New index name
     * @return boolean
     */
    function alter_index_remove($name) {

        if ($this->exists() && $this->db->create_tabs) {
            o2_gateway::indexremove($this->db->server->type,
                                    $this->db->server->server,
                                    $this->db->server->user,
                                    $this->db->server->password,
                                    $this->db->nome,
                                    $this->db->proprietario,
                                    $this->nome,
                                    $name);
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Elimina fisicamente la tabella
     *
     * @param  string    $backup_name   Name for tabel backup (instead of drop)
     * @return boolean
     */
    function elimina($backup_name = false) {

        if ($this->exists() && $this->db->create_tabs) {
            if ($backup_name) {
                $save_name = $this->nome;
                $this->rinomina($backup_name);
                $this->nome = $save_name;
                }
            else {
                o2_gateway::droptable($this->db->server->type,
                                      $this->db->server->server,
                                      $this->db->server->user,
                                      $this->db->server->password,
                                      $this->db->nome,
                                      $this->db->proprietario,
                                      $this->nome);
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Rinomina il file fisico della tabella
     *
     * @param  string $nome
     * @return boolean
     */
    function rinomina($nome) {

        if ($this->exists()) {
            if (o2_gateway::tabexists($this->db->server->type,
                                      $this->db->server->server,
                                      $this->db->server->user,
                                      $this->db->server->password,
                                      $this->db->nome,
                                      $this->db->proprietario,
                                      $nome)) {
                throw new o2_exception("Cannot rename table <i>".$this->nome.
                                       "</i> to <i>".$nome.
                                       "</i>: target table already exists.",
                                       o2error_DBTABLECOPY);
                return false;
                }
            o2_gateway::renametable($this->db->server->type,
                                    $this->db->server->server,
                                    $this->db->server->user,
                                    $this->db->server->password,
                                    $this->db->nome,
                                    $this->db->proprietario,
                                    $this->nome,
                                    $nome);
            $this->nome = $nome;
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Se già non esiste crea il file fisico per la tabella come definita nel repository.
     * Con $execute passato a FALSE ritorna lo script di creazione senza eseguirlo.
     *
     * @param  boolean $execute
     * @return boolean
     */
    function crea($execute = true) {

        if ((!$this->exists() && $this->db->create_tabs) || !$execute) {
            return o2_gateway::createtable($this->db->server->type,
                                           $this->db->server->server,
                                           $this->db->server->user,
                                           $this->db->server->password,
                                           $this->db->nome,
                                           $this->db->proprietario,
                                           $this->nome,
                                           $this->metatable(),
                                           $execute);
            }
        else {
            return true;
            }

        }


    /**
     * Returns an array descriptive of the table as expected from
     * o2_gateway::createtable().
     *
     * Array is in the form:
     * array("fields"  => array([field_1]  => array("type"    => A|N|L|D|T|S,
     *                                              "size"    => size,
     *                                              "int"     => integers,
     *                                              "dec"     => decimals),
     *                          [field_2]  => array("type"    => A|N|L|D|T|S,
     *                                              "size"    => size,
     *                                              "int"     => integers,
     *                                              "dec"     => decimals),
     *                          [...]
     *                          [field_n]  => array("type"    => A|N|L|D|T|S,
     *                                              "size"    => size,
     *                                              "int"     => integers,
     *                                              "dec"     => decimals)),
     *       "keys"    => array([key_1]    => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D),
     *                          [key_2]    => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D),
     *                          [...]
     *                          [key_n]    => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D)),
     *       "indexes" => array([idx_1]    => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D),
     *                          [idx_2]    => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D),
     *                          [...]
     *                          [idx_n]    => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D)),
     *       "primary" => array([pkeyname] => array([field_1] => A|D,
     *                                              [field_2] => A|D,
     *                                              [...]
     *                                              [field_n] => A|D)))
     *
     * @return array
     */
    function metatable() {

        $fields_local = array();
        $keys_local   = array();
        $idxs_local   = array();
        $pkey_local   = array();
        if ($this->asp == 'C') {
            $asp_mask                = $_SESSION['o2_app']->vars['_area']->maschera;
            $fields_local['O2ASPID'] = array("type" => "A",
                                             "size" => $asp_mask->dimensione,
                                             "int"  => $asp_mask->interi,
                                             "dec"  => $asp_mask->decimali);
            $pkey_local['O2ASPID']   = "A";
            }
        // __________________________________________________________ Fields structure ___
        foreach ($this->campi as $single_field) {
            $field_mask = $single_field->maschera;
            $fields_local[$single_field->nome_fisico] =
             array("type" => ($field_mask->tipo != "O" ? $field_mask->tipo : "T"),
                   "size" => $field_mask->dimensione,
                   "int"  => $field_mask->interi,
                   "dec"  => $field_mask->decimali);
            }
        // _____________________________________________________ Primary key structure ___
        foreach ($this->chiave->segmenti as $single_segment) {
            $pkey_local[$single_segment->campo->nome_fisico] = $single_segment->direzione;
            }
        // ____________________________________________________________ Keys structure ___
        foreach ($this->chiavi as $single_key) {
            if ($single_key->unique) {
                $keys_local[$single_key->nome] = array();
                }
            else {
                $idxs_local[$single_key->nome] = array();
                }
            if ($this->asp == 'C') {
                if ($single_key->unique) {
                    $keys_local[$single_key->nome]['O2ASPID'] = "A";
                    }
                else {
                    $idxs_local[$single_key->nome]['O2ASPID'] = "A";
                    }
                }
            foreach ($single_key->segmenti as $single_segment) {
                if ($single_key->unique) {
                    $keys_local[$single_key->nome][$single_segment->campo->nome_fisico] =
                     $single_segment->direzione;
                    }
                else {
                    $idxs_local[$single_key->nome][$single_segment->campo->nome_fisico] =
                     $single_segment->direzione;
                    }
                }
            }
        return array("fields"  => $fields_local,
                     "keys"    => $keys_local,
                     "indexes" => $idxs_local,
                     "primary" => array($this->chiave->nome => $pkey_local));

        }


    /**
     * Returns o2 definition script for the file
     *
     * @return string
     */
    function def_script() {

        $meta       = $this->metatable();
        $fields     = $meta["fields"];
        $keys       = $meta["keys"];
        $idxs       = $meta["indexes"];
        $primary    = $meta["primary"];
        $pk_name    = array_shift(array_keys($primary));
        $script_txt = 'o2def::tab("'.($this->indice ? $this->indice : $this->nome).
                              '", "'.$this->db->nome.
                              '", "'.$this->nome.
                              '", "pk_index");'."\n\n";
        foreach ($fields as $field_name => $single_field) {
            $type       = $single_field["type"];
            $size       = ($type == "N" ?
                           $single_field["int"].".".$single_field["dec"] :
                           $single_field["size"]);
            $script_txt.= ' o2def::field("'.$field_name.'", "'.
                                            $field_name.'", "'.
                                            $type.$size.'");'."\n";

            }
        $script_txt.= "\n";
        foreach ($keys as $key_name => $single_key) {
            if ($key_name == $pk_name) {
                $key_name = "pk_index";
                }
            $segm_txt = "";
            foreach ($single_key as $single_segment => $direction) {
                $segm_txt.= ($segm_txt ? ', "' : '"').
                            $single_segment.'", "'.$direction.'"';
                }
            $script_txt.= ' o2def::index("'.$key_name.'", '.$segm_txt.");\n";

            }
        if (count($idxs)) {
            $script_txt.= "\n";
            }
        foreach ($idxs as $idx_name => $single_idx) {
            $segm_txt = "";
            foreach ($single_idx as $single_segment => $direction) {
                $segm_txt.= ($segm_txt ? ', "' : '"').
                            $single_segment.'", "'.$direction.'"';
                }
            $script_txt.= ' o2def::nuindex("'.$key_name.'", '.$segm_txt.");\n";

            }

        return $script_txt;

        }


    /**
     * Export the table content into a file in internal XML format
     *
     * @param string  $file
     * @param boolean $asp_mode
     */
    function export($file, $asp_mode = false) {

        $this->crea(true);
        $xmlparser_local = new o2_xmltable($this, $file);
        return $xmlparser_local->db2xml($asp_mode);

        }


    /**
     * Import the content of an internal XML format file into the o2 table
     *
     * @param string  $file
     * @param boolean $asp_mode
     */
    function import($file, $asp_mode = false) {

        $xmlparser_local = new o2_xmltable($this, $file);
        return $xmlparser_local->xml2db($asp_mode);

        }

    /**
     * Set DB for the table, changing existing fields delimiters
     *
     * @param o2_db $db   DB to be set for table
     */
    function set_db($db) {

        $this->db  = $db;
        $this->asp = $db->asp;
        $c_o       = constant("o2_".$db->server->type."_o");
        $c_c       = constant("o2_".$db->server->type."_c");
        foreach ($this->campi as $f_id => $field) {
            $this->campi[$f_id]->nome_fisico = $c_o.$field->phys_name.$c_c;
            }

        }


    /**
     * Returns TRUE if table exists on database
     *
     * @return boolean
     */
    function exists() {

        return o2_gateway::tabexists($this->db->server->type,
                                     $this->db->server->server,
                                     $this->db->server->user,
                                     $this->db->server->password,
                                     $this->db->nome,
                                     $this->db->proprietario,
                                     $this->nome);

        }


    /**
     * Checks if table contains any data and returns boolean true/false
     *
     * @return boolean
     */
    function data() {

        if ($this->exists()) {
            return (o2_gateway::verifyrec($this->db->server->type,
                                          $this->db->server->server,
                                          $this->db->server->user,
                                          $this->db->server->password,
                                          $this->db->nome,
                                          $this->db->proprietario,
                                          $this->nome,
                                          "jxtabdatacheck",
                                          "1",
                                          "",
                                          "") !== false);
            }
        else {
            return false;
            }

        }


    /**
     * Delete data with current ASP area code.
     * If asp mode is enabled for file method returns TRUE else FALSE.
     *
     */
    function asp_clear() {

        if ($this->asp) {
            if ($this->exists()) {
                if ($this->asp == 'C') {
                    $where = "O2ASPID = '".$_SESSION['o2_app']->vars['_area']->valore."'";
                    }
                else {
                    $where = '1=1';
                    }
                // __________________________ Delete existing records for current area ___
                o2_gateway::deleterec($this->db->server->type,
                                      $this->db->server->server,
                                      $this->db->server->user,
                                      $this->db->server->password,
                                      $this->db->nome,
                                      $this->db->proprietario,
                                      $this->nome,
                                      $this->indice,
                                      $where);
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Copy data from current ASP area code to another.
     * If asp mode is enabled for file method returns TRUE else FALSE.
     *
     * @param string $target_asp   target code to copy to
     */
    function asp_clone($target_asp) {

        if ($this->asp) {
            if ($this->exists()) {
                // __________________ Retrieves defined fields physical names as array ___
                $tab_info   = $this->info();
                $def_fields = array_keys($tab_info['fields']['def']);
                $arr_local  = array_combine($def_fields, $def_fields);
                unset($arr_local["O2ASPID"]);
                $arr_local["@o2CloneArea"] = "'".$target_asp."'";
                // ___________________________ Delete existing records for target area ___
                o2_gateway::deleterec($this->db->server->type,
                                      $this->db->server->server,
                                      $this->db->server->user,
                                      $this->db->server->password,
                                      $this->db->nome,
                                      $this->db->proprietario,
                                      $this->nome,
                                      $this->indice,
                                      "O2ASPID = '".$target_asp."'");
                // ________________________________________________________ Clone data ___
                o2_gateway::insertfrom($this->db->server->type,
                                       $this->db->server->server,
                                       $this->db->server->user,
                                       $this->db->server->password,
                                       $this->db->nome,
                                       $this->db->proprietario,
                                       $this->nome,
                                       $this->db->nome,
                                       $this->db->proprietario,
                                       $this->nome,
                                       $arr_local,
                                       "O2ASPID = '".
                                       $_SESSION['o2_app']->vars['_area']->valore."'");
                }
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Create an interface program for browsing table data. $index is the name of index
     * key to use for sorting data.
     *
     * @param  string $index
     * @return boolean
     */
    function data_browser($index = "") {

        if (!$index) {
            $index = $this->chiave->nome;
            }
        $app = $_SESSION['o2_app'];
        $app->intcall("tools/o2sys_table_browser", $this->indice, $index);
        $browser_prg  = $app->istanze_prg[$app->progressivo_istanze];
        $browser_form = $browser_prg->form['table_browser_form'];
        $browser_view = $browser_prg->contesto['browser'];
        $browser_form->internal = true;
        $field_counter = 0;
        foreach ($this->campi as $field_index => &$single_field) {
            $field_counter++;
            $browser_view->usa($field_index, "o2table_browser", $field_index);
            if ($single_field->maschera->tipo == "L") {
                $ctrl_type = "check";
                $ctrl_css  = "o2_ctrl_check";
                $wide_plus = 0;
                }
            elseif (count($single_field->maschera->valori)) {
                $ctrl_type = "listcombo";
                $ctrl_css  = "o2_ctrl_listcombo";
                $wide_plus = 50;
                }
            else {
                $ctrl_type = "edit";
                $ctrl_css  = "o2_ctrl_edit";
                $wide_plus = 0;
                }
            $browser_ctrl = $browser_form->ctrldef($field_index."_ctrl",
                                                   $ctrl_type,
                                                   "data_browser_tab",
                                                   "browser",
                                                   $field_index);
            $browser_ctrl->larghezza(max(20 + $wide_plus,
                                     min(300,
                                     intval($single_field->maschera->dimensione * 10))));
            $browser_ctrl->altezza(20);
            $browser_ctrl->css($ctrl_css);
            $browser_ctrl->info_padre(array("1",
                                            strval($field_counter),
                                            strval($field_counter),
                                            $field_index,
                                            False,
                                            "",
                                            False,
                                            ""));

            unset($browser_ctrl);
            }
        $browser_view->struttura();
        // ________________________________________ Preset variable with COUNT on view ___
        $browser_prg->contesto['prg§_§var']->imposta('tot_recs',
                                                     'Total records: '.
                                                     o2format($browser_view->totale_rec(),
                                                              'o2sys_full_number'));
        return true;

        }


    /**
     * Create an interface program for browsing table log data.
     *
     * @return boolean
     */
    function log_browser() {

        $app = $_SESSION['o2_app'];
        $this->logtable();
        $app->intcall("tools/o2sys_log_browser", $this->log_table, $this->indice);
        $log_prg            = $app->istanze_prg[$app->progressivo_istanze];
        $log_form           = $log_prg->form['o2sys_log_browser_form'];
        $log_view           = $log_prg->contesto['log_view'];
        $log_form->internal = true;
        $field_counter      = 5;
        // ______________________________________________________________________ Date ___
        $log_ctrl = $log_form->ctrldef("ctr_logdate",
                                       "edit",
                                       "ctrl_table",
                                       "log_view",
                                       "log_date");
        $log_ctrl->larghezza(60);
        $log_ctrl->altezza(20);
        $log_ctrl->maschera("_o2date");
        $log_ctrl->css("o2_ctrl_edit");
        $log_ctrl->info_padre(array("1", "1", "1", "Date"));
        unset($log_ctrl);
        // ______________________________________________________________________ Time ___
        $log_ctrl = $log_form->ctrldef("ctr_logtime",
                                       "edit",
                                       "ctrl_table",
                                       "log_view",
                                       "log_time");
        $log_ctrl->larghezza(60);
        $log_ctrl->altezza(20);
        $log_ctrl->maschera("_o2time");
        $log_ctrl->css("o2_ctrl_edit");
        $log_ctrl->info_padre(array("1", "2", "2", "Time"));
        unset($log_ctrl);
        // _________________________________________________________________ Microtime ___
        $log_ctrl = $log_form->ctrldef("ctr_logmicro",
                                       "edit",
                                       "ctrl_table",
                                       "log_view",
                                       "log_microsec");
        $log_ctrl->larghezza(60);
        $log_ctrl->altezza(20);
        $log_ctrl->maschera("_o2number");
        $log_ctrl->css("o2_ctrl_edit");
        $log_ctrl->info_padre(array("1", "3", "3", "Microtime"));
        unset($log_ctrl);
        // ____________________________________________________________________ Action ___
        $log_ctrl = $log_form->ctrldef("ctr_logact",
                                       "listcombo",
                                       "ctrl_table",
                                       "log_view",
                                       "log_act");
        $log_ctrl->larghezza(60);
        $log_ctrl->altezza(20);
        $log_ctrl->maschera("o2sys_logact");
        $log_ctrl->css("o2_ctrl_listcombo");
        $log_ctrl->info_padre(array("1", "4", "4", "Action"));
        unset($log_ctrl);
        // ______________________________________________________________________ User ___
        $log_ctrl = $log_form->ctrldef("ctr_loguser",
                                       "edit",
                                       "ctrl_table",
                                       "log_view",
                                       "log_user");
        $log_ctrl->larghezza(80);
        $log_ctrl->altezza(20);
        $log_ctrl->css("o2_ctrl_edit");
        $log_ctrl->info_padre(array("1", "5", "5", "User"));
        unset($log_ctrl);
        // ______________________________________________________________ Other fields ___
        foreach ($this->campi as $field_index => &$single_field) {
            if ($field_index != "o2log_id"  &&
                $field_index != "o2log_act" &&
                $field_index != "o2log_user") {
                $field_counter++;
                $log_view->usa($field_index, "o2_logtable", $field_index);
                if ($single_field->maschera->tipo == "L") {
                    $ctrl_type = "check";
                    $ctrl_css  = "o2_ctrl_check";
                    $wide_plus = 0;
                    }
                elseif (count($single_field->maschera->valori)) {
                    $ctrl_type = "listcombo";
                    $ctrl_css  = "o2_ctrl_listcombo";
                    $wide_plus = 50;
                    }
                else {
                    $ctrl_type = "edit";
                    $ctrl_css  = "o2_ctrl_edit";
                    $wide_plus = 0;
                    }
                $log_ctrl = $log_form->ctrldef($field_index."_ctrl",
                                               $ctrl_type,
                                               "ctrl_table",
                                               "log_view",
                                               $field_index);
                $log_ctrl->larghezza(max(20 + $wide_plus,
                                         min(300,
                                             intval($single_field->maschera->dimensione *
                                             10))));
                $log_ctrl->altezza(20);
                $log_ctrl->css($ctrl_css);
                $log_ctrl->info_padre(array("1",
                                            strval($field_counter),
                                            strval($field_counter),
                                            $field_index,
                                            False,
                                            "",
                                            False,
                                            ""));

                unset($log_ctrl);
                }
            }
        $log_view->struttura();
        return true;

        }


    /**
     * Creates a copy of the table for log and adds it to the files repository.
     *
     * To the original file will be added 3 fields for log data:
     *    o2log_id   : operation timestamp
     *    o2log_act  : [I]nsert/[D]elete
     *    o2log_user : operation user
     *
     * New fields are added as first segments in the primary key
     *
     * @param  boolean $create_phys   Create physical table too or only in app list
     * @return object                 Newly created log table
     */
    function create_logtable($create_phys = true) {

        $app = $_SESSION['o2_app'];
        if (!$app->tables_log) {
            return false;
            }
        $table_local            = clone $this;
        $table_local->indice    = $table_local->log_table;
        $table_local->nome      = $table_local->nome."_o2log";
        $table_local->log_level = false;
        $c_o                    = constant('o2_'.$table_local->db->server->type.'_o');
        $c_c                    = constant('o2_'.$table_local->db->server->type.'_c');
        $new_fields             = array('o2log_id'   => new o2_campo($table_local,
                                                                     'log_id',
                                                                     $c_o.'log_id'.$c_c,
                                                                     'o2sys_timestamp'),
                                        'o2log_act'  => new o2_campo($table_local,
                                                                     'log_act',
                                                                     $c_o.'log_act'.$c_c,
                                                                     'o2sys_logact'),
                                        'o2log_user' => new o2_campo($table_local,
                                                                     'log_user',
                                                                     $c_o.'log_user'.$c_c,
                                                                     '_o2alpha'));
        $table_local->campi     = $new_fields + $table_local->campi;
        // ______________________ Field name on segments is needed by __clone() method ___
        $table_local->campi['o2log_id']->name   = 'o2log_id';
        $table_local->campi['o2log_act']->name  = 'o2log_act';
        $table_local->campi['o2log_user']->name = 'o2log_user';
        foreach ($table_local->chiavi as $key_index => $single_key) {
            if ($single_key->nome != $table_local->chiave->nome) {
                unset($table_local->chiavi[$key_index]);
                }
            else {
                $table_local->chiave = clone $table_local->chiavi[$key_index];
                $table_local->chiavi[$key_index] = $table_local->chiave;
                }
            }
        array_unshift($table_local->chiave->segmenti,
                      new o2_segmento_indice($table_local->campi['o2log_id'],   'D'),
                      new o2_segmento_indice($table_local->campi['o2log_act'],  'A'),
                      new o2_segmento_indice($table_local->campi['o2log_user'], 'A'));

        // _____________________________________ Alternative log database from app.ini ___
        if ($app->log_db) {
            if ($app->db[$app->log_db]) {
                $table_local->set_db($app->db[$app->log_db]);
                $app->runtime->load_gateway($table_local->db->server->type);
                }
            else {
                throw new o2_exception('Unknown database <i>'.$app->log_db.
                                       '</i> set as log DB in application settings',
                                       o2error_UNKNOWNDBSERVER);
                return false;
                }
            }
        $app->tab[$table_local->indice] = $table_local;
        if ($create_phys) {
            $app->tab[$table_local->indice]->crea(true);
            }
        return $app->tab[$table_local->indice];

        }


    /**
     * Creates a copy of the table for log and returns its o2_file object
     *
     * @return o2_file
     */
    function logtable() {

        $app = $_SESSION['o2_app'];
        if ($app->tables_log) {
            if (array_key_exists($this->log_table, $app->tab)) {
                return $app->get_table($this->log_table);
                }
            else {
                throw new o2_exception("Cannot find log table for table <i>".
                                       $this->indice."</i> (<b>".
                                       $this->nome."</b>).",
                                       o2error_UNKNOWNTABLE);
                }
            }
        else {
            return false;
            }

        }


    /**
     * Set/unset log settings for table.
     * $log_level can assume one of these values:
     *  [R]ecord    - Full record log for Insert, Delete and Update
     *  [D]elete    - Only Delete operations are logged
     *  [I]nsert    - Only Insert operations are logged
     *  [C]hange    - Update operations are logged with previous values
     *  [E]xistence - Insert and Delete operations are logged (I+D)
     *  [M]odify    - Update and Delete operations are logged (C+D)
     * Any other value will unset log settings for table.
     *
     * @param  string $log_level
     * @return boolean
     */
    function set_log($log_level = false) {

        $app = $_SESSION['o2_app'];
        if ($app->tables_log) {
            switch (strtoupper($log_level)) {
                case "R":
                    $par = "R";
                    break;
                case "D":
                    $par = "D";
                    break;
                case "I":
                    $par = "I";
                    break;
                case "C":
                    $par = "C";
                    break;
                case "E":
                    $par = "E";
                    break;
                case "M":
                    $par = "M";
                    break;
                default:
                    $par = "";
                    break;
                }
            $this->log_level = $par;
            $app->intcall("tools/o2sys_table_set_log", $this->indice, $par);
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Insert a full copy of record(s) selected by $where into the log table,
     * adding log-id segments.
     * This method, unlike $view->log_write(), logs all defined fields in table.
     *
     * @param  string $act
     * @param  string $where
     * @return boolean
     */
    function log_write_full($act, $where) {

        $app    = $_SESSION['o2_app'];
        $fields = array();
        $values = array();
        $logid  = $app->runtime->microtime();
        // _____________________________________________________________ Get LOG-table ___
        $logtab = $this->logtable();
        // _______________________________ LOG-table is on the same db of source table ___
        if ($this->db->server->server == $logtab->db->server->server &&
            ($this->db->nome == $logtab->db->nome ||
             $this->db->server->type != 'postgres')) {
            $engine = $this->db->server->type;
            $c_o    = constant("o2_".$engine."_o");
            $c_c    = constant("o2_".$engine."_c");
            // __________________________________________________________ Manage ASPID ___
            if ($this->asp == 'C') {
                $aspval = o2_gateway::normalize($engine, $app->vars['_area']->valore);
                $fields["'".$aspval."'"] = 'O2ASPID';
                }
            // __________________________________________________ Compose fileds array ___
            foreach ($this->campi as $single_field) {
                $fields[$single_field->nome_fisico] = $single_field->nome_fisico;
                }
            // ____________________________________________ Add log fields with values ___
            $fields["'".$logid."'"]     = $c_o."log_id".$c_c;
            $fields["'".$act."'"]       = $c_o."log_act".$c_c;
            $fields["'".$app->user."'"] = $c_o."log_user".$c_c;
            // _______________________________ Insert from original table to LOG-table ___
            o2_gateway::insertfrom($engine,
                                   $this->db->server->server,
                                   $this->db->server->user,
                                   $this->db->server->password,
                                   $this->db->nome,
                                   $this->db->proprietario,
                                   $this->write_name,
                                   $logtab->db->nome,
                                   $logtab->db->proprietario,
                                   $logtab->nome,
                                   $fields,
                                   $where);
            }
        // ____________________________ LOG-table is on a different db of source table ___
        else {
            $engine = $logtab->db->server->type;
            $c_o    = constant("o2_".$engine."_o");
            $c_c    = constant("o2_".$engine."_c");
            // ________________________________________________ Get full record values ___
            $set    = o2_gateway::recordset($this->db->server->type,
                                            $this->db->server->server,
                                            $this->db->server->user,
                                            $this->db->server->password,
                                            $this->db->nome,
                                            $this->db->proprietario,
                                            $this->write_name,
                                            $this->write_name,
                                            "*",
                                            $where,
                                            "",
                                            1);
            $set    = $set[0];
            // __________________________________________________________ Manage ASPID ___
            if ($this->asp == 'C') {
                $aspval   = o2_gateway::normalize($engine, $app->vars['_area']->valore);
                $fields[] = 'O2ASPID';
                $values[] = "'".$aspval."'";
                }
            // _______________________________________ Compose fields and values lists ___
            foreach ($this->campi as $single_field) {
                $fields[] = $single_field->nome_fisico;
                if ($single_field->tipo == "N") {
                    $values[] = $set[$single_field->phys_name];
                    }
                else {
                    $values[] = "'".$set[$single_field->phys_name]."'";
                    }
                }
            // ____________________________________________ Add log fields with values ___
            $fields[] = $c_o."log_id".$c_c;
            $values[] = "'".$logid."'";
            $fields[] = $c_o."log_act".$c_c;
            $values[] = "'".$act."'";
            $fields[] = $c_o."log_user".$c_c;
            $values[] = "'".$app->user."'";
            // ________________________________________________ Insert record into log ___
            o2_gateway::insertrec($logtab->db->server->type,
                                  $logtab->db->server->server,
                                  $logtab->db->server->user,
                                  $logtab->db->server->password,
                                  $logtab->db->nome,
                                  $logtab->db->proprietario,
                                  $logtab->nome,
                                  $logtab->indice,
                                  $fields,
                                  $values);
            }

        }


    /**
     * Returns a Janox table definition from SQL table.
     * Returned code can be merged in file-repository to add the table to a project.
     *
     * @param  o2_db  $jxdb        o2_db database object to get table from
     * @param  string $tab_name    Physical name of the table to get
     * @param  string $tab_alias   Name to assign to table in definition
     * @return o2_file
     */
    static function get_def($db_name, $tab_name, $tab_alias = "") {

        $app       = $_SESSION['o2_app'];
        $jxdb      = $app->db[$db_name];
        $server    = $jxdb->server;
        $tab_idx   = ($tab_alias ? $tab_alias : $tab_name);
        $s         = "// __________ Table ".$tab_name." definition ___\n";
        $s_mask    = "// __________ Models definition ___\n";
        $fields    = o2_gateway::tablefields($server->type,
                                             $server->server,
                                             $server->user,
                                             $server->password,
                                             $jxdb->nome,
                                             $jxdb->proprietario,
                                             $tab_name);
        $indexes   = o2_gateway::tableindexes($server->type,
                                              $server->server,
                                              $server->user,
                                              $server->password,
                                              $jxdb->nome,
                                              $jxdb->proprietario,
                                              $tab_name);
        $pkey_name = "";
        $seg_count = 0;
        // ____________________________________________ Look for potential PRIMARY KEY ___
        foreach ($indexes as $index_name => $single_index) {
            if (!$seg_count || count($single_index) < $seg_count) {
                $pkey_name = $index_name;
                $seg_count = count($single_index);
                }
            }
        $s.= 'o2def::tab("'.$tab_idx.'", "'.
                            $jxdb->id.'", "'.
                            $tab_name.'", "'.
                            $tab_idx."_PK\");\n";
        // _____________________________________________________________ Define fields ___
        foreach ($fields as $single_field) {
            $field_name = $single_field['field'];
            $mask_name  = $tab_idx."_".$field_name;
            $mask_type  = "A";
            $mask_pict  = "C30";
            $size       = "";
            $type_local = strtolower($single_field['type']);
            if (strpos($type_local, "(")) {
                $size = substr($type_local, strpos($type_local, "(") + 1);
                $size = substr($size, 0, strpos($size, ")"));
                }
            if (strpos($type_local, "char") !== false ||
                strpos($type_local, "text") !== false) {
                if (($size == 8) && $single_field['default'] == "00000000") {
                    $mask_type = "D";
                    $mask_pict = "-DMY";
                    }
                elseif (($size == 6) && $single_field['default'] == "000000") {
                    $mask_type = "O";
                    $mask_pict = ":HMS";
                    }
                elseif (($size == 1) && $single_field['default'] == "0") {
                    $mask_type = "L";
                    $mask_pict = "";
                    }
                else {
                    $size      = ($size ? $size : "1024");
                    $mask_type = "A";
                    $mask_pict = "C".$size;
                    }
                }
            elseif (strpos($type_local, "int") !== false     ||
                    strpos($type_local, "decimal") !== false ||
                    strpos($type_local, "numeric") !== false) {
                if (strpos($size, ",") !== false) {
                    list($int, $dec) = explode(",", $size);
                    $size = (intval($int) - intval($dec)).".".$dec;
                    }
                else {
                    $size = ($size ? $size : "16");
                    }
                $mask_type = "N";
                $mask_pict = $size."NT";
                }
            else {
                $mask_type = "N";
                $mask_pict = "16.6TN";
                }
            if (isset($app->maschere[$mask_name])) {
                unset($app->maschere[$mask_name]);
                }
            if ($field_name != "O2ASPID") {
                $s_mask.= 'o2def::model("'.$mask_name.'", "'.
                                           $mask_type.'", "'.
                                           $mask_pict."\");\n";
                $s     .= 'o2def::field("'.$field_name.'", "'.
                                           $field_name.'", "'.
                                           $mask_name."\");\n";
                }
            }
        // ____________________________________________________________ Define indexes ___
        foreach ($indexes as $index_name => $single_index) {
            $seg_str = "";
            foreach ($single_index['segments'] as $single_seg) {
                if ($single_seg['column'] != "O2ASPID") {
                    $seg_str.= ', "'.$single_seg['column'].'","'.
                               ($single_seg['dir'] == "D" ? 'D"' : 'A"');
                    }
                }
            if ($single_index['unique']) {
                $s.= 'o2def::index("'.($index_name == $pkey_name ?
                                       $tab_idx."_PK" : $index_name).'" '.$seg_str.");\n";
                }
            else {
                $s.= 'o2def::nuindex("'.$index_name.'" '.$seg_str.");\n";
                }
            }
        return $s_mask."\n".$s;

        }

    }


/**
 * Collector of methods for db accessing and executing
 *
 */
class o2_gateway {


    /**
     * Normalize a string for db writing escaping special chars.
     * If $untrim param is FALSE or not passed then string is also blank-trimmed
     *
     * @param  string  $type
     * @param  string  $string
     * @param  boolean $untrim
     * @return string
     */
    static function normalize($type, $string, $untrim = false) {

        $o2normalize = "o2_".$type."_normalize";
        return $o2normalize($string, $untrim);

        }


    /**
     * Concatenate two or more strings and/or fields
     *
     * @param  string  $type
     * @param  array   $strings
     * @return string
     */
    static function concat($type, $strings) {

        $o2concat = "o2_".$type."_concat";
        return $o2concat($strings);

        }


    /**
     * Returns a database object name fully qualified.
     * For example $tab_name is returned as "db"."schema"."tab_name" for Oracle and
     * Postgres, `db`.`tab_name` for MySQL, ...
     * If $name is omitted a fully qualified prefix will be returned.
     *
     * @param  string $type
     * @param  string $database
     * @param  string $owner
     * @param  string $string
     * @return string
     */
    static function qualify($type, $database, $owner, $name = "") {

        $o2qualify = "o2_".$type."_qualify";
        return $o2qualify($database, $owner, $name);

        }


    /**
     * Return a db connection handle
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @return PDO
     */
    static function connect($type, $server, $user, $password) {

        try {
            $o2connect = "o2_".$type."_connect";
            $res = $o2connect($server, $user, $password, false);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            return false;
            }
        return $res;

        }


    /**
     * Manage log skip-by-key list for connection.
     * Passed keys are stored: if table/key already exists returns FALSE, else TRUE.
     * Function is intended to be used in this way:
     *  IF (o2_gateway::use_key_once($key)) THEN log_write($key)
     * Stored keys list is cleared on connection commit.
     * This function is useful to skip multiple change-logs on the same record in the same
     * transaction.
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $table
     * @return boolean
     */
    static function use_key_once($type, $server, $user, $table, $key) {

        $idx = ($_SESSION['o2_app']->isolated_trans ? 'I' : '').$type.$server.$user;
        // ________________________________________________ If missing global variable ___
        if (!isset($GLOBALS['jx_key_once'])) {
            $GLOBALS['jx_key_once'] = array($idx => array($table => array($key => true)));
            return true;
            }
        // _______________________________________________ If missing connection index ___
        elseif (!isset($GLOBALS['jx_key_once'][$idx])) {
            $GLOBALS['jx_key_once'][$idx] = array($table => array($key => true));
            return true;
            }
        // ___________________________________________ If missing table for connection ___
        elseif (!isset($GLOBALS['jx_key_once'][$idx][$table])) {
            $GLOBALS['jx_key_once'][$idx][$table] = array($key => true);
            return true;
            }
        // __________________________________________________ If missing key for table ___
        elseif (!isset($GLOBALS['jx_key_once'][$idx][$table][$key])) {
            $GLOBALS['jx_key_once'][$idx][$table][$key] = true;
            return true;
            }
        // _________________________________________________ If table key already done ___
        else {
            return false;
            }

        }


    /**
     * Commit the active transaction for the server
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  boolean $end        Close open statement for script ending
     * @return boolean
     */
    static function commit($type, $server, $user, $password, $end = false) {

        // ________________________________ Clear logs skip-by-key list for connection ___
        $idx = ($_SESSION['o2_app']->isolated_trans ? 'I' : '').$type.$server.$user;
        if (isset($GLOBALS['jx_key_once']) && isset($GLOBALS['jx_key_once'][$idx])) {
            unset($GLOBALS['jx_key_once'][$idx]);
            }
        // ________________________________________________________ Commit transaction ___
        $o2commit = "o2_".$type."_commit";
        if (function_exists($o2commit)) {
            try {
                $o2commit($server, $user, $password, $end);
                }
            catch (o2_exception $o2e) {
                $o2e->send();
                return false;
                }
            return true;
            }
        else {
            return true;
            }

        }


    /**
     * Execute a query on the given db server.
     *
     * For queries returning a recordset returns an array in the form:
     *    arr[0] = array(field_0 => value_0,
     *                   field_1 => value_1,
     *                   ...,
     *                   field_n => value_n)
     *    arr[1] = array(field_0 => value_0,
     *                   field_1 => value_1,
     *                   ...,
     *                   field_n => value_n)
     *    ...
     *    arr[n] = array(field_0 => value_0,
     *                   field_1 => value_1,
     *                   ...,
     *                   field_n => value_n)
     *
     * For queries in the form SELECT exp AS computed returns a numeric value
     *
     * For queries executing commands returns TRUE for correct execution else FALSE
     *
     * For failing queries returns an array in the form:
     *    arr['o2error'] = returned error string
     *
     * @param  string  $type
     * @param  string  $query
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  boolean $only_exe
     * @param  boolean $trans
     * @return mix
     */
    static function execute($type,
                            $query,
                            $server,
                            $user,
                            $password,
                            $only_exe = false,
                            $trans    = true) {

        $o2execute = "o2_".$type."_execute";
        try {
            $res_val = $o2execute($query, $server, $user, $password, $only_exe, $trans);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Retrieves user tables list from a database
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @return array
     */
    static function tables($type, $server, $user, $password, $database, $owner) {

        $o2tables = "o2_".$type."_tables";
        try {
            $res_val = $o2tables($server, $user, $password, $database, $owner);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = array();
            }
        return $res_val;

        }


    /**
     * Returns TRUE is table exists in database
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @return boolean
     */
    static function tabexists($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table) {

        $o2tabexists = "o2_".$type."_tabexists";
        if (function_exists($o2tabexists)) {
            try {
                $res_val = $o2tabexists($server,
                                        $user,
                                        $password,
                                        $database,
                                        $owner,
                                        $table);
                }
            catch (o2_exception $o2e) {
                $o2e->send();
                $res_val = false;
                }
            }
        else {
            $o2tables = "o2_".$type."_tables";
            try {
                $res_val = in_array(strtolower($table),
                                    array_map("strtolower",
                                              $o2tables($server,
                                                        $user,
                                                        $password,
                                                        $database,
                                                        $owner)));
                }
            catch (o2_exception $o2e) {
                $o2e->send();
                $res_val = false;
                }
            }
        return $res_val;

        }


    /**
     * Return an array containing informations about fileds of a db table in the form:
     *    $arr[n] = array('Field'   => field,
     *                    'Type'    => type(dim),
     *                    'Default' => default)
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @return string
     */
    static function tablefields($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table) {

        $o2tabfields = "o2_".$type."_tablefields";
        return $o2tabfields($server, $user, $password, $database, $owner, $table);

        }


    /**
     * Return an array containing informations about indexes of a db table in the form:
     *    $arr[n] = array('Field'   => field,
     *                    'Type'    => type(dim),
     *                    'Default' => default)
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @return string
     */
    static function tableindexes($type,
                                 $server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table) {

        $o2tabindexes = "o2_".$type."_tableindexes";
        return $o2tabindexes($server, $user, $password, $database, $owner, $table);

        }


    /**
     * Insert into table A data read from table B and return TRUE on success.
     * Matching fileds are passed by array $values in the form:
     *    $values[field_from] = field_to
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database_from
     * @param  string $owner_from
     * @param  string $table_from
     * @param  string $database_to
     * @param  string $owner_to
     * @param  string $table_to
     * @param  array  $values
     * @return boolean
     */
    static function insertfrom($type,
                               $server,
                               $user,
                               $password,
                               $database_from,
                               $owner_from,
                               $table_from,
                               $database_to,
                               $owner_to,
                               $table_to,
                               $values,
                               $where) {

        $res_val      = true;
        $o2insertfrom = "o2_".$type."_insertfrom";
        try {
            $o2insertfrom($server,
                          $user,
                          $password,
                          $database_from,
                          $owner_from,
                          $table_from,
                          $database_to,
                          $owner_to,
                          $table_to,
                          $values,
                          $where);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Phisically removes a db table
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @return boolean
     */
    static function droptable($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table) {

        $res_val     = true;
        $o2droptable = "o2_".$type."_droptable";
        try {
            $o2droptable($server, $user, $password, $database, $owner, $table);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Phisically renames a db table
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $new_name
     * @return boolean
     */
    static function renametable($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $new_name) {

        $res_val       = true;
        $o2renametable = "o2_".$type."_renametable";
        try {
            $o2renametable($server,
                           $user,
                           $password,
                           $database,
                           $owner,
                           $table,
                           $new_name);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * If $execute is passed TRUE function phisically creates a db table else it returns
     * the creation script for the table.
     * Array "fields" is an array of o2_campo objects.
     * Array "keys" is an array of unique o2_chiavi objects.
     * Array "indexes" is an array of not unique o2_chiavi objects.
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @param  string  $table
     * @param  array   $structure
     * @param  boolean $execute
     * @return boolean
     */
    static function createtable($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $structure,
                                $execute = true) {

        $res_val       = true;
        $o2createtable = "o2_".$type."_createtable";
        try {
            $res_val = $o2createtable($server,
                                      $user,
                                      $password,
                                      $database,
                                      $owner,
                                      $table,
                                      $structure,
                                      $execute);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Alter table to add a new column
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $field_name
     * @param  string $field_type
     * @param  string $field_int
     * @param  string $field_dec
     * @return boolean
     */
    static function fieldadd($type,
                             $server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $field_name,
                             $field_type,
                             $field_int,
                             $field_dec = 0) {

        if ($type == "sqlite3") {
            throw new o2_exception("Cannot alter a SQLite table!", o2error_DBTABLEALTER);
            return false;
            }
        $res_val    = true;
        $o2fieldadd = "o2_".$type."_field_add";
        try {
            $res_val = $o2fieldadd($server,
                                   $user,
                                   $password,
                                   $database,
                                   $owner,
                                   $table,
                                   $field_name,
                                   $field_type,
                                   $field_int, // _______ Passed twice for total size! ___
                                   $field_int,
                                   $field_dec);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Alter table to remove a column
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $field_name
     * @return boolean
     */
    static function fieldremove($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $field_name) {

        if ($type == "sqlite3") {
            throw new o2_exception("Cannot alter a SQLite table!", o2error_DBTABLEALTER);
            return false;
            }
        $res_val       = true;
        $o2fieldremove = "o2_".$type."_field_remove";
        try {
            $res_val = $o2fieldremove($server,
                                      $user,
                                      $password,
                                      $database,
                                      $owner,
                                      $table,
                                      $field_name);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Alter table to change a column definition
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $field_name
     * @param  string $field_type
     * @param  string $field_int
     * @param  string $field_dec
     * @return boolean
     */
    static function fieldchange($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $field_name,
                                $field_type,
                                $field_int,
                                $field_dec) {

        if ($type == "sqlite3") {
            throw new o2_exception("Cannot alter a SQLite table!", o2error_DBTABLEALTER);
            return false;
            }
        $res_val       = true;
        $o2fieldchange = "o2_".$type."_field_change";
        try {
            $res_val = $o2fieldchange($server,
                                      $user,
                                      $password,
                                      $database,
                                      $owner,
                                      $table,
                                      $field_name,
                                      $field_type,
                                      $field_int,
                                      $field_int, // ____ Passed twice for total size! ___
                                      $field_dec);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Alter table to change a column definition
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $old_name
     * @param  string $new_name
     * @return boolean
     */
    static function fieldrename($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $old_name,
                                $new_name) {

        if ($type == "sqlite3") {
            throw new o2_exception("Cannot alter a SQLite table!", o2error_DBTABLEALTER);
            return false;
            }
        $res_val       = true;
        $o2fieldrename = "o2_".$type."_field_rename";
        try {
            $res_val = $o2fieldrename($server,
                                      $user,
                                      $password,
                                      $database,
                                      $owner,
                                      $table,
                                      $old_name,
                                      $new_name);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Alter table to add a new index
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @param  string  $table
     * @param  string  $index_name
     * @param  array   $index_segs
     * @param  boolean $unique
     * @return boolean
     */
    static function indexadd($type,
                             $server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $index_name,
                             $index_segs,
                             $unique = true) {

        if ($type == "sqlite3") {
            throw new o2_exception("Cannot alter a SQLite table!", o2error_DBTABLEALTER);
            return false;
            }
        $res_val    = true;
        $o2indexadd = "o2_".$type."_index_add";
        try {
            $res_val = $o2indexadd($server,
                                   $user,
                                   $password,
                                   $database,
                                   $owner,
                                   $table,
                                   $index_name,
                                   $index_segs,
                                   $unique);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Alter table to remove an index
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $index_name
     * @return boolean
     */
    static function indexremove($type,
                                $server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $index_name) {

        if ($type == "sqlite3") {
            throw new o2_exception("Cannot alter a SQLite table!", o2error_DBTABLEALTER);
            return false;
            }
        $res_val       = true;
        $o2indexremove = "o2_".$type."_index_remove";
        try {
            $res_val = $o2indexremove($server,
                                      $user,
                                      $password,
                                      $database,
                                      $owner,
                                      $table,
                                      $index_name);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Return an array of calculated db aggragate functions for the table or view.
     * Array $functions contains the list of functions to calculate and is passed in the
     * form:
     *    $functions[o2aggrfunc_n] = array("func"  => aggr_function,
     *                                     "field" => on_field)
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $table_alias
     * @param  string $where
     * @param  array  $functions
     * @param  array  $links
     * @return array
     */
    static function aggregate($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $where,
                              $functions,
                              $links = null) {

        $o2aggregate = "o2_".$type."_aggregate";
        try {
            $res_val = $o2aggregate($server,
                                    $user,
                                    $password,
                                    $database,
                                    $owner,
                                    $table,
                                    $table_alias,
                                    $where,
                                    $functions,
                                    $links);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = array();
            }
        return $res_val;

        }


    /**
     * Verify if exist at last 1 record for given conditions. If it exists function
     * returns the record, else it returns FALSE.
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $table_alias
     * @param  string $select_str
     * @param  string $where
     * @param  string $order_by
     * @return boolean
     */
    static function verifyrec($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $select_str,
                              $where,
                              $order_by) {

        $o2verifyrec = "o2_".$type."_verifyrec";
        try {
            $res_val = $o2verifyrec($server,
                                    $user,
                                    $password,
                                    $database,
                                    $owner,
                                    $table,
                                    $table_alias,
                                    $select_str,
                                    $where,
                                    $order_by);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Modify record uniquely identified in $where clause with values in $sets.
     * Setting values are passed in array $sets in the form:
     *    [0] field1 = value1
     *    [1] field2 = value2
     *    [...] ...
     *    [n] fieldn = valuen
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $table_alias
     * @param  array  $sets
     * @param  string $where
     * @return boolean
     */
    static function modifyrec($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $sets,
                              $where) {

        $res_val     = true;
        $o2modifyrec = "o2_".$type."_modifyrec";
        try {
            $o2modifyrec($server,
                         $user,
                         $password,
                         $database,
                         $owner,
                         $table,
                         $table_alias,
                         $sets,
                         $where);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Insert passed record fields in a db table.
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $table_alias
     * @param  array  $fields
     * @param  array  $values
     * @return boolean
     */
    static function insertrec($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $fields,
                              $values) {

        $res_val     = true;
        $o2insertrec = "o2_".$type."_insertrec";
        try {
            $o2insertrec($server,
                         $user,
                         $password,
                         $database,
                         $owner,
                         $table,
                         $table_alias,
                         $fields,
                         $values);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Delete record of a db table for the passed $where clause.
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $table_alias
     * @param  string $where
     * @return boolean
     */
    static function deleterec($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $where) {

        $o2deleterec = "o2_".$type."_deleterec";
        $res_val     = true;
        try {
            $o2deleterec($server,
                         $user,
                         $password,
                         $database,
                         $owner,
                         $table,
                         $table_alias,
                         $where);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            }
        return $res_val;

        }


    /**
     * Returns number of total records for passed $where clause.
     *
     * @param  string $type
     * @param  string $server
     * @param  string $user
     * @param  string $password
     * @param  string $database
     * @param  string $owner
     * @param  string $table
     * @param  string $table_alias
     * @param  string $where
     * @param  array  $links
     * @return boolean
     */
    static function count($type,
                          $server,
                          $user,
                          $password,
                          $database,
                          $owner,
                          $table,
                          $table_alias,
                          $where,
                          $links = null) {

        $o2count = "o2_".$type."_count";
        try {
            $res_val = $o2count($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $table_alias,
                                $where,
                                $links);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = 0;
            }
        return $res_val;

        }


    /**
     * Returns a set of $recs records starting from $start for a passed $where clause from
     * a db table. Recordset is returned in the form:
     *    arr[0] = array(field_0 => value_0,
     *                   field_1 => value_1,
     *                   ...,
     *                   field_n => value_n)
     *    arr[1] = array(field_0 => value_0,
     *                   field_1 => value_1,
     *                   ...,
     *                   field_n => value_n)
     *    ...
     *    arr[n] = array(field_0 => value_0,
     *                   field_1 => value_1,
     *                   ...,
     *                   field_n => value_n)
     *
     * If $lock parameter is passed as TRUE method is used to SELECT FOR UPDATE.
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @param  string  $table
     * @param  string  $table_alias
     * @param  string  $select_str
     * @param  string  $where
     * @param  string  $order_by
     * @param  integer $recs
     * @param  array   $links
     * @param  boolean $lock
     * @param  string  $page_where
     * @param  string  $view_id
     * @param  array   $prepared_pars
     * @return array
     */
    static function recordset($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $select_str,
                              $where,
                              $order_by,
                              $recs,
                              $links         = null,
                              $lock          = false,
                              $page_where    = false,
                              $view_id       = false,
                              $prepared_pars = false) {

        $o2recordset = "o2_".$type."_recordset";
        try {
            $res_val = $o2recordset($server,
                                    $user,
                                    $password,
                                    $database,
                                    $owner,
                                    $table,
                                    $table_alias,
                                    $select_str,
                                    $where,
                                    $order_by,
                                    $recs,
                                    $links,
                                    $lock,
                                    $page_where,
                                    $view_id,
                                    $prepared_pars);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = array();
            }
        return $res_val;

        }


    /**
     * Alter a table to create a foreign key.
     * $main_fields and $ref_fields are list of filed names, from main and referenced
     * tables to create the key on.
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $main_db
     * @param  string  $main_owner
     * @param  string  $main_table
     * @param  array   $main_fields
     * @param  string  $ref_db
     * @param  string  $ref_owner
     * @param  string  $ref_table
     * @param  array   $ref_fields
     * @param  string  $key_name
     * @return boolean
     */
    static function fkeyadd($type,
                            $server,
                            $user,
                            $password,
                            $main_db,
                            $main_owner,
                            $main_table,
                            $main_fields,
                            $ref_db,
                            $ref_owner,
                            $ref_table,
                            $ref_fields,
                            $key_name) {

        $app       = $_SESSION['o2_app'];
        $o2fkeyadd = "o2_".$type."_fkeyadd";
        $res_val   = false;
        if ($main_db == $ref_db) {
            try {
                $res_val = $o2fkeyadd($server,
                                      $user,
                                      $password,
                                      $main_db,
                                      $main_owner,
                                      $main_table,
                                      $main_fields,
                                      $ref_db,
                                      $ref_owner,
                                      $ref_table,
                                      $ref_fields,
                                      $key_name);
                }
            catch (o2_exception $o2e) {
                $o2e->send();
                $res_val = false;
                $res_val = array();
                }
            }
        return $res_val;

        }


    /**
     * Alter a table to remove a foreign key.
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @param  string  $table
     * @param  string  $key_name
     * @return boolean
     */
    static function fkeyremove($type,
                               $server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

        $o2fkeycreate = "o2_".$type."_fkeyremove";
        try {
            $res_val = $o2fkeycreate($server,
                                     $user,
                                     $password,
                                     $database,
                                     $owner,
                                     $table,
                                     $key_name);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            $res_val = array();
            }
        return $res_val;

        }


    /**
     * Validate a foreign key in table against existing data.
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @param  string  $table
     * @param  string  $key_name
     * @return boolean
     */
    static function fkeyvalidate($type,
                                 $server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table,
                                 $key_name) {

        $o2fkeyvalidate = "o2_".$type."_fkeyvalidate";
        try {
            $res_val = $o2fkeyvalidate($server,
                                       $user,
                                       $password,
                                       $database,
                                       $owner,
                                       $table,
                                       $key_name);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            $res_val = array();
            }
        return $res_val;

        }


    /**
     * Returns the list of existing foreign keys for table
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @param  string  $table
     * @return boolean
     */
    static function fkeystablist($type,
                                 $server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table) {

        $o2fkeyslist = "o2_".$type."_fkeystablist";
        try {
            $res_val = $o2fkeyslist($server,
                                    $user,
                                    $password,
                                    $database,
                                    $owner,
                                    $table);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            $res_val = array();
            }
        return $res_val;

        }


    /**
     * Returns the list of existing foreign keys for database
     *
     * @param  string  $type
     * @param  string  $server
     * @param  string  $user
     * @param  string  $password
     * @param  string  $database
     * @param  string  $owner
     * @return boolean
     */
    static function fkeysinfo($type,
                              $server,
                              $user,
                              $password,
                              $database,
                              $owner) {

        $o2fkeysinfo = "o2_".$type."_fkeysinfo";
        try {
            $res_val = $o2fkeysinfo($server,
                                    $user,
                                    $password,
                                    $database,
                                    $owner);
            }
        catch (o2_exception $o2e) {
            $o2e->send();
            $res_val = false;
            $res_val = array();
            }
        return $res_val;

        }


    static function jxsdb($db, $command) {

        $server    = $db->server;
        $db_engine = $server->type;
        $db_host   = $server->server;
        $db_user   = $server->user;
        $db_pwd    = $server->password;
        $db_name   = $db->nome;
        $db_schema = $db->proprietario;
        $co        = constant('o2_'.$db_engine.'_o');
        $cc        = constant('o2_'.$db_engine.'_c');
        // _______________________________________________ Get parameters from Request ___
        $p1  = $_REQUEST['p1'];
        $p2  = $_REQUEST['p2'];
        $p3  = $_REQUEST['p3'];
        $p4  = $_REQUEST['p4'];
        $p5  = $_REQUEST['p5'];
        $p6  = $_REQUEST['p6'];
        $p7  = $_REQUEST['p7'];
        $p8  = $_REQUEST['p8'];
        $p9  = $_REQUEST['p9'];
        $p10 = $_REQUEST['p10'];
        // ________________ Tool function to raplace delimiters placeholders in params ___
        $fix = function($par) use (&$fix, $co, $cc) {
            if (is_array($par)) {
                $res = array();
                foreach ($par as $key => $value) {
                    $res[(is_string($key) ? $fix($key) : $key)] = (is_string($value) ||
                                                                   is_array($value) ?
                                                                   $fix($value) :
                                                                   $value);
                    }
                return $res;
                }
            else {
                return (strpos($par, '#jxdo#') === false ?
                        $par :
                        str_replace('#jxdc#', $cc, str_replace('#jxdo#', $co, $par)));
                }
            };
        // _______________________________________ Prepare command for implicit commit ___
        $commit = 'o2_'.$db_engine.'_commit';
        // __________________________________________________ Engine method to execute ___
        try {
            switch ($command) {
                case 'concat':
                    $exec = 'o2_'.$db_engine.'_concat';
                    return json_encode($exec(json_decode($p1, true)));
                    break;
                case 'tables':
                    $exec = 'o2_'.$db_engine.'_tables';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema));
                    break;
                case 'tabexists':
                    $exec = 'o2_'.$db_engine.'_tabexists';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1));
                    break;
                case 'tablefields':
                    $exec = 'o2_'.$db_engine.'_tablefields';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1));
                    break;
                case 'tableindexes':
                    $exec = 'o2_'.$db_engine.'_tableindexes';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1));
                    break;
                case 'insertfrom':
                    $exec = 'o2_'.$db_engine.'_insertfrom';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             $db_name,
                                             $db_schema,
                                             $p2,
                                             $fix(json_decode($p3, true)),
                                             $fix($p4)));
                    break;
                case 'droptable':
                    $exec = 'o2_'.$db_engine.'_droptable';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1));
                    break;
                case 'renametable':
                    $exec = 'o2_'.$db_engine.'_renametable';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             $p2));
                    break;
                case 'createtable':
                    $exec = 'o2_'.$db_engine.'_createtable';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             $fix(json_decode($p2, true)),
                                             $p3));
                    break;
                case 'aggregate':
                    $exec = 'o2_'.$db_engine.'_aggregate';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             $p2,
                                             $fix($p3),
                                             $fix(json_decode($p4, true)),
                                             $fix(json_decode($p5, true))));
                    break;
                case 'verifyrec':
                    $exec = 'o2_'.$db_engine.'_verifyrec';
                    // ___________________$p1 is table-from and may be a sub-select... ___
                    if (substr($p1, 0, 1) == '(') {
                        // _________________________ ... and so can contain JXSQL code ___
                        $p1 = jxsql($db->id, $p1);
                        }
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $fix($p1),
                                             $p2,
                                             $fix($p3),
                                             $fix($p4),
                                             $fix($p5)));
                    break;
                case 'modifyrec':
                    $exec = 'o2_'.$db_engine.'_modifyrec';
                    json_encode($exec($db_host,
                                      $db_user,
                                      $db_pwd,
                                      $db_name,
                                      $db_schema,
                                      $p1,
                                      $p2,
                                      $fix(json_decode($p3, true)),
                                      $fix($p4)));
                    return $commit($db_host, $db_user, $db_pwd, true);
                    break;
                case 'insertrec':
                    $exec = 'o2_'.$db_engine.'_insertrec';
                    json_encode($exec($db_host,
                                      $db_user,
                                      $db_pwd,
                                      $db_name,
                                      $db_schema,
                                      $p1,
                                      $p2,
                                      $fix(json_decode($p3, true)),
                                      json_decode($p4, true)));
                    return $commit($db_host, $db_user, $db_pwd, true);
                    break;
                case 'deleterec':
                    $exec = 'o2_'.$db_engine.'_deleterec';
                    json_encode($exec($db_host,
                                      $db_user,
                                      $db_pwd,
                                      $db_name,
                                      $db_schema,
                                      $p1,
                                      $p2,
                                      $fix($p3)));
                    return $commit($db_host, $db_user, $db_pwd, true);
                    break;
                case 'count':
                    // ___________________$p1 is table-from and may be a sub-select... ___
                    if (substr($p1, 0, 1) == '(') {
                        // _________________________ ... and so can contain JXSQL code ___
                        $p1 = jxsql($db->id, $p1);
                        }
                    $exec = 'o2_'.$db_engine.'_count';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $fix($p1),
                                             $p2,
                                             $fix($p3),
                                             $fix(json_decode($p4, true))));
                    break;
                case 'recordset':
                    // ___________________$p1 is table-from and may be a sub-select... ___
                    if (substr($p1, 0, 1) == '(') {
                        // _________________________ ... and so can contain JXSQL code ___
                        $p1 = jxsql($db->id, $p1);
                        }
                    $exec = 'o2_'.$db_engine.'_recordset';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $fix($p1),
                                             $p2,
                                             $fix($p3),
                                             $fix($p4),
                                             $fix($p5),
                                             $p6,
                                             $fix(json_decode($p7, true)),
                                             $p8,
                                             $fix($p9),
                                             $p10));
                    break;
                case 'fkeyadd':
                    $exec = 'o2_'.$db_engine.'_fkeyadd';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             json_decode($p2, true),
                                             $db_name,
                                             $db_schema,
                                             $p3,
                                             json_decode($p4, true),
                                             $p5));
                    break;
                case 'fkeyremove':
                    $exec = 'o2_'.$db_engine.'_fkeyremove';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             $p2));
                    break;
                case 'fkeyvalidate':
                    $exec = 'o2_'.$db_engine.'_fkeyvalidate';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1,
                                             $p2));
                    break;
                case 'fkeystablist':
                    $exec = 'o2_'.$db_engine.'_fkeystablist';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema,
                                             $p1));
                    break;
                case 'fkeysinfo':
                    $exec = 'o2_'.$db_engine.'_fkeysinfo';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $db_name,
                                             $db_schema));
                    break;
                case 'commit':
                    $exec = 'o2_'.$db_engine.'_commit';
                    return json_encode($exec($db_host,
                                             $db_user,
                                             $db_pwd,
                                             $p1));
                    break;
                }
            }
        catch (o2_exception $e) {
            return json_encode(array('!#ERROR' => $e->getmessage()));
            }
        }

    }


/**
 * Connection object for HTTP/S connections
 * Used by JXSDB gateway.
 *
 * This class is intended to provide an object to make requests to a Janox service,
 * passing a POST context.
 *
 * First request is used to create a remote session, with user and password credentials,
 * and to retrieve assigned session-ID.
 * Next requests will use the session-ID rerieved by the first request, avoiding
 * credentials.
 *
 */
class JXHttpConn {

    public $url       = '';
    public $user      = '';
    public $password  = '';
    public $sessname  = 'JXHttpConn';
    public $token     = false;


    function __construct($url, $user, $password) {

        $this->url      = $url;
        $this->user     = $user;
        $this->password = $password;
        $this->token    = false;

        }


    /**
     * Send POST $data and get remote output.
     *
     * @param  array $data   POST fields key/value pairs
     * @return mix
     */
    function get($data) {

        $data['JXSESSNAME'] = $this->sessname;
        // _______________ Already connected, use stored SESSION-ID token as a coockie ___
        if ($this->token) {
            return jx_post($this->url, $data, $this->sessname.'='.$this->token);
            }
        // _________________________________ First connection, retrieve SESSION-ID tag ___
        else {
            $data['user']       = $this->user;
            $data['password']   = $this->password;
            $headers            = array();
            $res                = jx_post($this->url, $data, false, $headers);
            $conn_ko            = true;
            foreach ($headers as $header) {
                if (substr($header, 0, 11) == 'Set-Cookie:') {
                    $header      = substr($header, 0, strpos($header, ';'));
                    $this->token = substr($header, strpos($header, '=') + 1);
                    $conn_ko     = false;
                    break;
                    }
                }
            if ($conn_ko) {
                throw new o2_exception("<b>JXHttpConn</b>: <i>".$this->url."</i>.<hr>".
                                       "Can't connect to server.",
                                       o2error_IO);
                }
            }
        return $res;

        }

    }


/**
 * Implements methods for reading from / writing to an internal XML file (o2x)
 *
 */
class o2_xmltable  {

    /*     ===== PROPERTIES =========================================================== */
    public $file           = "";      /* Physical XML file of the table                 */
    public $handler        = 0;       /* Function to activate on parsing                */
    public $parser         = false;   /* Resource of the active xmlparser for the file  */
    public $record         = -1;      /* Records counter or number of affected rows     */
    public $field          = -1;      /* Row fields counter                             */
    public $field_on       = false;   /* If reading a requested field data              */
    public $field_data     = "";      /* Cunk of data of a single field                 */
    /**
     * @var o2_file
     */
    public $table          = "";      /* o2_table obiect for import/export operations   */
    public $current_record = array(); /* Current record array in the form:
                                         current_record(0 => value0, 1 => value1, ...)  */
    public $recordset      = array(); /* Current recordset array in the form:
                                         recordset(0=>array(0=>value0, 1=>value1, ...),
                                                   1=>array(0=>value0, 1=>value1, ...),
                                                   ...)                                 */
    public $asp_mode       = false;   /* If 1st field (ASP CODE) filtering is enabled   */
    public $differential   = true;    /* If differential import (only missing records)  */


    /**
     * Constructor
     *
     * @param  o2_file $table
     * @param  string  $file
     * @return o2_xmltable
     */
    function __construct($table, $file) {

        $this->file   = $file;
        $this->table  = $table;
        $this->parser = xml_parser_create();
        xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, true);
        xml_set_object($this->parser, $this);
        xml_set_element_handler($this->parser, "open_tag", "close_tag");
        xml_set_character_data_handler($this->parser, "read_data");

        }


    /**
     * Parses the file
     *
     */
    function parse() {

        if (!$f_handle = fopen($this->file, "r")) {
            throw new o2_exception("Error reading from file <i>".$this->file."</i>",
                                   o2error_IO);
            return false;
            }
        while (!feof($f_handle)) {
            if (!($data = fread($f_handle, 65536))) {
                throw new o2_exception("Error reading from file <i>".$this->file."</i>",
                                       o2error_IO);
                return false;
                }
            if (!xml_parse($this->parser, $data, false)) {
                $error_local = xml_error_string(xml_get_error_code($this->parser));
                throw new o2_exception("Cannot read file <i>".$this->file.
                                       "</i> at line <i>".
                                       xml_get_current_line_number($this->parser).
                                       "</i>, column <i>".
                                       xml_get_current_column_number($this->parser).
                                       "</i><hr /><code>".$error_local."</code>",
                                       o2error_XML);
                return false;
                }
            }
        fclose($f_handle);
        xml_parse($this->parser, "", true);
        xml_parser_free($this->parser);

        }


    // =============================== SYNTAX FUNCTIONS ==================================

    /**
     * Handlers tag openening events
     *
     * @param resource $parser
     * @param string   $tag
     * @param array    $attributes
     */
    function open_tag($parser, $tag, $attributes) {

        switch ($tag) {
            case "RSET":
                // Reset records counter
                $this->record = -1;
                break;
            case "R":
                // Reset fields counter
                $this->field = -1;
                $this->record_start();
                break;
            case "F":
                // Increments fields counter
                $this->field++;
                // Declares that we are reading a field
                $this->field_on   = true;
                $this->field_data = "";
                break;
            }

        }


    /**
     * Handlers tag ending events
     *
     * @param resource $parser
     * @param string   $tag
     */
    function close_tag($parser, $tag) {

        switch ($tag) {
            case "RSET":
                break;
            case "R":
                $this->record_end();
                break;
            case "F":
                if ($this->field_on) {
                    $this->field_data($this->field_data);
                    $this->field_on = false;
                    }
                break;
            }

        }


    /**
     * Handlers data reading events
     *
     * @param resource $parser
     * @param string   $data
     */
    function read_data($parser, $data) {

        $this->field_data.= $data;

        }


    // ============================ STRUCTURE FUNCTIONS ==================================

    /**
     * Handler record starting events
     *
     */
    function record_start() {

        // _________________________________________________ Increments record counter ___
        $this->record++;
        $this->current_record = array();

        }


    /**
     * Handler record ending events
     *
     */
    function record_end() {

        $app = $_SESSION['o2_app'];
        if ($this->field < 0) {
            $this->record = -1;
            return;
            }
        $names_list  = array();
        $values_list = array();
        $field_index = -1;
        $type_local  = $this->table->db->server->type;
        foreach ($this->table->campi as &$singolo_campo) {
            if ($this->table->asp == 'C' && $field_index == -1) {
                $names_list[] = 'O2ASPID';
                if ($this->asp_mode) {
                    $asp_value     = $app->vars['_area']->valore;
                    $values_list[] = "'".
                                     o2_gateway::normalize($type_local, $asp_value).
                                     "'";
                    }
                else {
                    $values_list[] = "'".
                                     o2_gateway::normalize($type_local,
                                                           $this->current_record[0]).
                                     "'";
                    $field_index = 0;
                    }
                }
            $field_index++;
            $names_list[] = $singolo_campo->nome_fisico;
            if (isset($this->current_record[$field_index])) {
                $field_value   = $this->current_record[$field_index];
                $values_list[] = ($singolo_campo->maschera->tipo != "N" ?
                                  "'".o2_gateway::normalize($type_local,
                                                            iconv("UTF-8",
                                                                  $app->chr_encoding.
                                                                  "//TRANSLIT",
                                                                  $field_value))."'" :
                                  $field_value);
                }
            else {
                $values_list[] = $singolo_campo->maschera->default;
                }
            }
        if ($this->differential) {
            $GLOBALS['jxerrorhide'] = true;
            }
        o2_gateway::insertrec($type_local,
                              $this->table->db->server->server,
                              $this->table->db->server->user,
                              $this->table->db->server->password,
                              $this->table->db->nome,
                              $this->table->db->proprietario,
                              $this->table->nome,
                              $this->table->indice,
                              $names_list,
                              $values_list);
        if ($this->differential) {
            o2_gateway::commit($type_local,
                               $this->table->db->server->server,
                               $this->table->db->server->user,
                               $this->table->db->server->password);
            unset($GLOBALS['jxerrorhide']);
            }

        }


    /**
     * Handler field value reading event
     *
     * @param mix $value
     */
    function field_data($value) {

        $this->current_record[] = $value;

        }


    // ================================ TARGET FUNCTIONS =================================

    /**
     * Return an XML file content into a given o2 table
     *
     * @param boolean $asp_mode
     * @param boolean $differential
     */
    function xml2db($asp_mode, $differential = true) {

        $this->asp_mode     = ($this->table->asp == 'C' && $asp_mode);
        $this->differential = $differential;
        $this->table->crea();
        $this->parse();
        return ($this->record + 1);

        }


    /**
     * Exports the content of a given o2 table into an internal XML format file
     *
     * @param boolean $aspcode_less
     * @param string  $where
     * @param string  $order_by
     */
    function db2xml($aspcode_less, $where = "", $order_by = "") {

        $app = $_SESSION['o2_app'];
        /* o2aspid field is present if:
            - table belongs to an ASPed database AND
            - asp suppression has not been required OR
            - application variable [_area] has blank value                              */
        $asp_present = $this->table->asp == 'C' &&
                       (!$aspcode_less || !$app->vars['_area']->valore);
        // _________________________________ Create view on the fly in running program ___
        $exp_view    = new o2_dbview("", $app->progressivo_istanze, "o2exp_view");
        // ____________________________________ Add exporting table as view main table ___
        $exp_view->usa_file($this->table->indice,
                            $this->table->indice,
                            $this->table->chiave->nome);
        // _____________________________________________ Add SELECTs for single fields ___
        foreach (array_keys($this->table->campi) as $single_field) {
            $exp_view->usa($single_field, $this->table->indice, $single_field);
            }
        $exp_view->internal = true;
        $exp_view->struttura();
        $exp_view->righe_vis = $this->table->batch_chunk;
        $exp_view->record_primo();
        if (!($fh_local = fopen($this->file, "w"))) {
            throw new o2_exception("Error writing to file <i>".$this->file."</i>",
                                   o2error_IO);
            unset($exp_view);
            return false;
            }
        $tot_dim = 0;
        $exp_str = "<?xml version='1.0' encoding='UTF-8' standalone='yes'?".">\n".
                   "<!--                 o2xdb table document                -->\n".
                   "<!-- created       : ".str_pad(date("d.m.Y H:i:s"),
                                                   35, " ", STR_PAD_RIGHT)." -->\n".
                   "<!-- application   : ".str_pad($app->nome,
                                                   35, " ", STR_PAD_RIGHT)." -->\n".
                   "<!-- by            : ".str_pad($app->user,
                                                   35, " ", STR_PAD_RIGHT)." -->\n".
                   "<!-- from          : ".str_pad(strtoupper($this->table->db->
                                                               server->type),
                                                   35, " ", STR_PAD_RIGHT)." -->\n".
                   "<!-- database      : ".str_pad($this->table->db->nome,
                                                   35, " ", STR_PAD_RIGHT)." -->\n".
                   "<!-- table         : ".str_pad($this->table->nome,
                                                   35, " ", STR_PAD_RIGHT)." -->\n".
                   ($asp_present ?
                    "<!-- asp area code : PRESENT                             -->\n" :
                    ($this->table->asp == 'C' ?
                     "<!-- asp area code : FILTERED                            -->\n" :
                     "")).
                   "<rset>\n";
        $exp_len = strlen($exp_str);
        // ___________________________________________________ Loops until end of file ___
        do {
            $str2add = "<r>";
            foreach ($exp_view->corrente as $field_name => $field_value) {
                // ___________________________________________ o2aspid field filtering ___
                if ($field_name != "O2ASPID" || $asp_present) {
                    if ($exp_view->campi[$field_name]->maschera->tipo != "A") {
                        $str2add.= "<f>".$field_value."</f>";
                        }
                    else {
                        $str2add.= "<f>".($field_value ?
                                          "<![CDATA[".iconv($app->chr_encoding,
                                                            "UTF-8//TRANSLIT",
                                                            $field_value)."]]>" : "").
                                   "</f>";
                        }
                    }
                }
            $str2add.= "</r>\n";
            $exp_str.= $str2add;
            $exp_len+= strlen($str2add);
            if ($exp_len > 65536) {
                fwrite($fh_local, $exp_str);
                $tot_dim+= $exp_len;
                $exp_str = "";
                $exp_len = 0;
                }
            } while ($exp_view->record_avanti());
        $tot_dim+= ($exp_len + 35 + strlen($tot_dim + 35));
        $exp_str.= "</rset>\n<!-- Total ".$tot_dim." bytes wrote -->";
        fwrite($fh_local, $exp_str);
        fclose($fh_local);
        $tot_recs = $exp_view->totale_record;
        unset($exp_view);
        return $tot_recs;

        }

    }


/**
 * Collects methods for objects defining
 *
 */
class o2def {

    // ========================= DEFINIZIONE DELLE CONNETTIVITA'==========================
    /**
     * Define a new server for the application.
     *
     * @param string  $id           unique ID
     * @param string  $type         engine type (sqlite|mysql|postgres|...)
     * @param string  $host         server name or IP address
     * @param string  $user         login user Id
     * @param string  $password     login password
     * @param integer $chunk_size   retrieved data block size
     */
    static function server($id, $type, $host, $user, $password, $chunk_size = 0) {

        if (isset($_SESSION['o2_app']->server[$id])) {
            $srv = $_SESSION['o2_app']->server[$id];
            if ($type) {
                $srv->type = $type;
                }
            if ($host) {
                $srv->server = $host;
                }
            if ($user) {
                $srv->user = $user;
                }
            if ($password) {
                $srv->password = $password;
                }
            if ($chunk_size) {
                $srv->chunk_size = $chunk_size;
                }
            }
        else {
            $_SESSION['o2_app']->server[$id] = new o2_server($id,
                                                             $type,
                                                             $host,
                                                             $user,
                                                             $password,
                                                             $chunk_size);
            }

        }


    /**
     * Define a new database for the application.
     *
     * @param string $id         unique ID
     * @param string $server     existing server ID
     * @param string $database   database physical name
     * @param string $owner      database owner/schema
     * @param string $asp        activate Application Server Provider mode
     */
    static function db($id,
                       $server,
                       $database,
                       $owner,
                       $asp         = false,
                       $create_tabs = true) {

        $app = $_SESSION['o2_app'];
        if (isset($app->db[$id])) {
            $db = $app->db[$id];
            if ($server) {
                $db->server = $app->server[$server];
                }
            if ($database) {
                $db->nome = $database;
                }
            if ($owner) {
                $db->proprietario = $owner;
                }
            if ($asp) {
                $db->asp = $asp;
                }
            if ($create_tabs) {
                $db->create_tabs = $create_tabs;
                }
            // ____________________ Needed to change phy_names in system tables fields ___
            $app->runtime->load_gateway($db->server->type);
            foreach ($app->runtime->sys_tabs() as $tabname) {
                $systab = $app->tab[$tabname];
                if ($systab->db->id == $id) {
                    $systab->set_db($db);
                    }
                }
            }
        else {
            $app->db[$id] = new o2_db($id,
                                      $app->server[$server],
                                      $database,
                                      $owner,
                                      $asp,
                                      $create_tabs);
            }


        }


    /**
     * Definisce una tabella, di nome [$con_nome], per l'applicazione
     *    $database        = nome di un database definito per l'applicazione
     *    $nome_fisico     = nome sql della tabella
     *    $chiave_primaria = nome di una chiave fra quelle definite per la tabella
     *
     * @param string $con_nome
     * @param string $database
     * @param string $nome_fisico
     * @param string $chiave_primaria
     */
    static function tab($con_nome, $database, $nome_fisico, $chiave_primaria) {

        $app = $_SESSION['o2_app'];
        if (isset($app->tab[$con_nome])) {
            unset($app->tab[$con_nome]);
            }
        $app->tab[$con_nome] = new o2_file($con_nome,
                                           $database,
                                           $nome_fisico,
                                           $chiave_primaria);

        }


    /**
     * Definisce un campo, di nome [$con_nome], per l'ultima tabella definita
     *    $nome_fisico = nome sql del campo
     *    $tipo        = {[A]lfanumerico, [N]umerico, [L]ogico, [D]ata, [O]ra}
     *    $dimensione  = numero di caratteri del campo
     *
     * @param string $con_nome
     * @param string $nome_fisico
     * @param string $maschera
     */
    static function field($con_nome, $nome_fisico, $maschera = "") {

        $app       = $_SESSION['o2_app'];
        $tab_local = end($app->tab);
        $c_o       = constant("o2_".$tab_local->db->server->type."_o");
        $c_c       = constant("o2_".$tab_local->db->server->type."_c");
        $tab_local->campi[$con_nome] = new o2_campo($tab_local->indice,
                                                    $nome_fisico,
                                                    $c_o.$nome_fisico.$c_c,
                                                    $maschera);
        // _________________________________________________________ Tracing on record ___
        if ($app->record_trace &&
            ($ak = array_search(strtoupper($con_nome), $app->record_trace))) {
            $tab_local->record_trace[$ak] = $c_o.$nome_fisico.$c_c;
            }

        }


    /**
     * Definisce una chiave, di nome [$con_nome], per l'ultima tabella definita
     *
     * @param string $con_nome
     */
    static function index($con_nome) {

        $segmenti      = array();
        $pk_segments   = array();
        $uno_si_uno_no = false;
        $tab_local     = end($_SESSION['o2_app']->tab);
        $tot_args      = func_num_args();
        for ($i = 1; $i < $tot_args; $i++) {
            if ($uno_si_uno_no = !$uno_si_uno_no) {
                $field_name  = func_get_arg($i);
                $campo       = $tab_local->campi[$field_name];
                $campo->name = $field_name;
                $direzione   = func_get_arg($i + 1);
                $segmenti[]  = new o2_segmento_indice($campo, $direzione);
                $pk_segments[$campo->nome_fisico] = $field_name;
                }
            }
        $tab_local->chiavi[$con_nome] = new o2_chiave($con_nome, $segmenti, true);
        // ____________________________________________________________ If Primary Key ___
        if ($tab_local->chiave == $con_nome) {
            $tab_local->chiave      = $tab_local->chiavi[$con_nome];
            $tab_local->pk_segments = $pk_segments;
            }

        }


    /**
     * Define a not unique index.
     *
     * @param string $con_nome
     */
    static function nuindex($con_nome) {

        $segmenti      = array();
        $uno_si_uno_no = false;
        $tab_local     = end($_SESSION['o2_app']->tab);
        $pk_segments   = $tab_local->pk_segments;
        $tot_args      = func_num_args();
        for ($i = 1; $i < $tot_args; $i++) {
            if ($uno_si_uno_no = !$uno_si_uno_no) {
                $field_name  = func_get_arg($i);
                $campo       = $tab_local->campi[$field_name];
                $campo->name = $field_name;
                $direzione   = func_get_arg($i + 1);
                $segmenti[]  = new o2_segmento_indice($campo, $direzione);
                if (isset($pk_segments[$campo->nome_fisico])) {
                    unset($pk_segments[$campo->nome_fisico]);
                    }
                }
            }
        $tab_local->chiavi[$con_nome] = new o2_chiave($con_nome, $segmenti, false);
        // ____ Create a copy of index, fixed to be unique, adding missing pk-segments ___
        foreach ($pk_segments as $pk_seg) {
            foreach ($tab_local->chiave->segmenti as $key_seg) {
                if ($key_seg->campo->name == $pk_seg) {
                    break;
                    }
                }
            $segmenti[] = new o2_segmento_indice($key_seg->campo, $key_seg->direzione);
            }
        $tab_local->fix_nukeys[$con_nome] = new o2_chiave($con_nome, $segmenti, true);

        }


    /**
     * Defines a developer allowed to access applications in "DEV" mode
     *
     * @param string $developer
     * @param string $password
     */
    static function developer($developer, $password = "") {

        if (!$password) {
            $password = "";
            }
        elseif (strlen($password) < 32) {
            $password = md5($password);
            }
        $GLOBALS['o2_runtime']->developers[strtolower($developer)] = $password;

        }


    // ======================== DEFINIZIONE DELL'APPLICAZIONE ============================

    /**
     * Definisce un'applicazione per il contesto di interfaccia definito sulla runtime, di
     * versione $release, e ne assicura la continuità  nella sessione.
     *
     * @param string $release
     */
    static function app($release = "") {

        switch ($GLOBALS['o2_runtime']->interface) {
            case "HTML":
                o2html::app($release);
                break;
            case "BAT":
            default:
                o2_app::bat_app($release);
                break;
            }
        }


    /**
     * Definisce una variabile di applicazione di tipo $datatype
     *
     * @param string $var_name
     * @param string $datatype
     */
    static function appvar($var_name, $datatype = '') {

        $_SESSION['o2_app']->vars[$var_name] = new o2_appvar($var_name, $datatype);

        }


    /**
     * Definisce una maschera di input/output per un tipo di dati
     *
     * @param string $nome
     * @param string $tipo_campo
     * @param string $maschera
     * @param string $zoom_prg
     */
    static function model($nome, $tipo_campo, $maschera = "", $zoom_prg = "") {

        $_SESSION['o2_app']->maschere[$nome] = new o2_maschera($nome,
                                                               $tipo_campo,
                                                               jx_encode($maschera),
                                                               $zoom_prg);

        }


    /**
     * Definisce un menu o una voce di menu
     *
     */
    static function menu_item($padre,
                              $tipo     = "S",
                              $nome     = "",
                              $label    = "",
                              $data     = "",
                              $visibile = true,
                              $attivo   = true,
                              $ico      = "") {

        $app   = $_SESSION['o2_app'];
        $label = htmlentities($label, ENT_COMPAT | ENT_HTML5, $app->chr_encoding);
        if (strtoupper($tipo[0]) == "M" && !isset($app->menu[$nome])) {
            $app->menu[$nome] = new o2_menubar($nome, $ico);
            }
        if (isset($app->menu[$padre])) {
            $app->menu[$padre]->voce($tipo,
                                     $nome,
                                     $label,
                                     $data,
                                     $visibile,
                                     $attivo,
                                     $ico);
            }

        }


    // =========================== DEFINIZIONE DEL PRG ===================================

    /**
     * Define a program, named $nome_prg, in application context. Action $esecutivo will
     * be executed as program starts.
     *
     * @param string $nome_prg
     * @param string $prefisso
     * @param string $suffisso
     * @param string $script
     */
    static function prg($nome_prg, $prefisso, $suffisso, $script = null) {

        $app    = $_SESSION['o2_app'];
        $exe_id = ++$app->progressivo_istanze;
        // _________ Dovrebbe accadere solo con l'uso del debug (o2 client console)!!! ___
        if (isset($app->istanze_prg[$exe_id])) {
            if ($app->istanze_prg[$exe_id]->nome == $nome_prg) {
                return;
                }
            else {
                foreach (array_reverse(array_keys($app->istanze_prg)) as $indice) {
                    if ($indice >= $exe_id) {
                        $app->termina_prg($indice);
                        }
                    }
                }
            }
        $app->istanze_prg[$exe_id] = new o2_prg($nome_prg,
                                                $exe_id,
                                                $prefisso,
                                                $suffisso,
                                                $script);

        }


    /**
     * Define an input/output parameter for program
     *
     * @param integer $id
     * @param string  $nome_parametro
     * @param string  $maschera
     */
    static function par($id, $nome_parametro = "", $maschera = "") {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->set_par($id,
                                                              $nome_parametro,
                                                              $maschera);

        }


    // ========================== DEFINIZIONE DELLA VISTA ================================

    /**
     * Definisce una vista, di nome [$view_name], nel contesto del prg in definizione
     *
     * @param string  $view_name
     * @param boolean $not_used
     * @param string  $prefix
     * @param string  $suffix
     * @param boolean $auto_aggr
     * @param boolean $prepared_stmts
     */
    static function view($view_name,
                         $not_used       = false,
                         $prefix         = '',
                         $suffix         = '',
                         $auto_aggr      = true,
                         $prepared_stmts = false) {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->create_view($view_name,
                                                                  $prefix,
                                                                  $suffix,
                                                                  $auto_aggr,
                                                                  $prepared_stmts);

        }


    /**
     * Definisce un'azione, di nome [$con_nome], nel contesto del prg in definizione
     *
     * @param string $con_nome
     */
    static function act($con_nome) {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->azione($con_nome);

        }


    /**
     * Definisce una form, di nome [$con_nome], nel contesto del prg in definizione
     *
     * @param string  $con_nome
     * @param string  $linkto
     * @param boolean $menu_behavior
     * @param string  $vis_cond
     * @param string  $url
     */
    static function form($con_nome,
                         $linkto        = "",
                         $menu_behavior = false,
                         $vis_cond      = "true",
                         $url           = false) {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->form($con_nome,
                                                           $linkto,
                                                           $menu_behavior,
                                                           $vis_cond,
                                                           $url);

        }


    /**
     * Definisce una risorsa di importazione/esportazione di dati a livello di prg
     *
     * @param string $con_nome
     * @param string $tipo
     * @param string $risorsa
     * @param string $direzione
     */
    static function io($con_nome, $tipo, $risorsa = "", $direzione = "O") {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->risorsa($con_nome,
                                                              $tipo,
                                                              $risorsa,
                                                              $direzione);

        }


    /**
     * Definisce un protocollo di importazione/esportazione di dati a livello di prg
     *
     * @param string $con_nome
     * @param string $tipo
     */
    static function protocol($con_nome, $tipo = "XML") {

        $app = $_SESSION['o2_app'];
        $app->istanze_prg[$app->progressivo_istanze]->protocollo($con_nome, $tipo);

        }

    }


/**
 * o2 runtime environment main class
 *
 */
class o2_runtime {

    /*     ===== PROPERTIES =========================================================== */
    public $id              = 0;          /* Thread (runtime instance) unique id        */
    public $release         = "0.0.0";    /* Runtime release (see top of jxrnt.php)     */
    public $built           = "00000000"; /* Built date (see top of jxrnt.php)          */
    public $alias           = "/janox/";  /* Janox runtime web folder alias             */
    public $interface       = "BAT";      /* Interface layout [HTML|BAT(no GUI)]        */
    public $mswin           = false;      /* If running on a Windows machine            */
    public $last_microtime  = 0;          /* Last microtime, to avoid repetitions       */
    public $microtime_incr  = 1;          /* Microtime increment                        */
    public $conf_list       = array();    /* Configurations settings list from rnt ini  */
    public $read_sys_tabs   = false;      /* Sys tables reading vs app tables from rep  */
    /*     _____ Options ______________________________________________________________ */
    public $multisession    = true;       /* If multiple sessions are allowed           */
    public $decimals_point  = ".";        /* Decimals separator character               */
    public $thousands_point = ",";        /* Thousands separator character              */
    public $def_datachunk   = 65536;      /* Default data chunk value for all databases */
    /*     _____ Development __________________________________________________________ */
    public $developers      = array();    /* List of allowed developers                 */
    public $developer       = false;      /* Developer ID, if passed and validated      */
    public $dev_key         = "";         /* Developer password (MD5)                   */
    /*     _____ Filesystem ___________________________________________________________ */
    public $php_engine      = "php";      /* Complete path to PHP (or PHP CLI) exe      */
    public $rnt_script      = "";         /* Runtime main script (jxrnt/jxrnt.php)      */
    /**
     * @var o2_dir
     */
    public $root            = "";         /* o2 runtime root folder path                */
    /**
     * @var o2_dir
     */
    public $dir_dbms        = "";         /* o2 DBMS gateways                           */
    /**
     * @var o2_dir
     */
    public $dir_threads     = "";         /* Temporary files for running threads folder */
    /**
     * @var o2_dir
     */
    public $dir_css         = "";         /* Path to runtime CSS folder                 */


    /**
     * Janox runtime constructor
     *
     * @return o2_runtime
     */
    function __construct() {

        $GLOBALS['o2_runtime'] = $this;
        // ________________________________________________ Error constants definition ___
        o2_exception::def_errors();
        // ___________________________________________________ Top level error handler ___
        set_error_handler(array('o2_exception', 'errors_handler'));
        // _______________________________________________ Top level exception handler ___
        set_exception_handler(array('o2_exception', 'autosend'));
        $this->id          = "jxrnt_".$this->microtime();
        $this->release     = $GLOBALS['jxrel'];
        $this->built       = $GLOBALS['jxbuilt'];
        $this->root        = new o2_dir(realpath(dirname(__FILE__).DIRECTORY_SEPARATOR.
                                                 "..".DIRECTORY_SEPARATOR));
        $this->rnt_script  = $this->root."jxrnt.php";
        $this->mswin       = (stripos(PHP_OS, "win") !== false);
        // _______________________________________________________ Runtime directories ___
        $this->dir_dbms    = new o2_dir($this->root."lib".DIRECTORY_SEPARATOR.
                                                    "dbms".DIRECTORY_SEPARATOR);
        $this->dir_threads = new o2_dir($this->root."threads".DIRECTORY_SEPARATOR);
        $this->dir_css     = new o2_dir($this->root."htdocs".DIRECTORY_SEPARATOR.
                                                    "css".DIRECTORY_SEPARATOR);
        /********************************************************************************
         *                              Interface setting                               *
         ********************************************************************************/
        // _____________________________________________________________________ BATCH ___
        if (defined("STDIN")) {
            $this->get_cli_params();
            $this->interface = "BAT";
            }
        // ______________________________________________________________________ HTML ___
        else {
            $this->interface = "HTML";
            // _______________________________________________ Provide HTML interfaces ___
            include_once $this->root."lib/jxhtml.inc";
            }
        // ________________________________ Read options from configuration file (INI) ___
        $this->read_options();
        // ___________________________________________________ Manage development mode ___
        $this->set_developer();
        // ___________________________________________________ Command line parameters ___
        if ($GLOBALS['jxdirect']) {
            // ____________________ Set execution mode to RUNTIME for system functions ___
            switch (strtolower($_SERVER['argv'][1])) {
                /************************** RELEASE REQUEST *****************************
                 * ::: Instructions                                                     *
                 *     execute php.exe from shell/console, passing following positional *
                 *     parameters:                                                      *
                 *     1. Janox runtime main script (jxrnt/jxrnt.php)                   *
                 *     2. String "rel" meaning version string request                   *
                 ************************************************************************/
                case "rel":
                    print "\nJanox ".$this->release()."\n\n";
                    die();
                    break;
                /*********************** INSPECT APPLICATION ****************************
                 * ::: Instructions                                                     *
                 *     execute php.exe from shell/console, passing following positional *
                 *     parameters:                                                      *
                 *     1. Janox runtime main script (jxrnt/jxrnt.php)                   *
                 *     2. String "inspect" application inspection request               *
                 *     3. Inspection method exposed by o2_inspector class               *
                 *    ... Specific parameters needed by requested method                *
                 ************************************************************************/
                case "inspect":
                    $this->inspect();
                    die();
                    break;
                /*********************** OBSCURE APPLICATION ****************************
                 * ::: Instructions                                                     *
                 *     execute php.exe from shell/console, passing following positional *
                 *     parameters:                                                      *
                 *     1. Janox runtime main script (jxrnt/jxrnt.php)                   *
                 *     2. String "obscure" application code protection request          *
                 ************************************************************************/
                case "obscure":
                    $this->obscure($_SERVER['argv'][2]);
                    die();
                    break;
                default:
                    $jxrel = $this->release();
                    print <<<endofjxmsg

   Welcome to Janox, the full cross-tech development suite.
   Release: $jxrel
   Copyright: Tommaso Vannini (tvannini@janox.it) 2007

      Janox is free software;  you can redistribute it and/or  modify it
      under  the  terms  of  the  GNU  Lesser General Public License  as
      published by the Free Software Foundation; either version 3 of the
      License, or (at your option) any later version.

      Janox is distributed in the hope  that  it  will  be  useful,  but
      WITHOUT  ANY  WARRANTY;  without  even  the  implied  warranty  of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU
      Lesser General Public License for more details.

      You should have received a copy of the  GNU  Lesser General Public
      License along with this program.
      If not see <http://www.gnu.org/licenses>.

   Usage:

      Janox runtime is used to create  and  manage  full  futered,  rich
      internet applications. It is used within a PHP Application-Server,
      to run on-line applications served by a WEB-Server.
      Please visit www.janox.it.


endofjxmsg;

                    die();
                    break;
                }
            }

        }


    /**
     * Returns 16 digits string, representing unique time-tamp in millionths of a second.
     * Use this method instead of microtime() PHP function to avoid repetitions.
     *
     * @return string
     */
    function microtime() {

        $mic = microtime(true) * 1000000;
        if ($mic == $this->last_microtime) {
            $mic+= $this->microtime_incr;
            $this->microtime_incr++;
            }
        else {
            $this->last_microtime = $mic;
            $this->microtime_incr = 1;
            }
        return number_format($mic, 0, "", "");

        }


    /**
     * Set developer for Janox runtime
     *
     * @return array
     */
    function set_developer() {

        $dev             = "";
        $this->developer = "";
        // _________________________________________ Get passed developer and password ___
        if (isset($_REQUEST["dev"])) {
            $dev = trim(strtolower($_REQUEST["dev"]));
            $key = $_REQUEST["key"];
            }
        if ($dev) {
            // _______________________________________ Read list of allowed developers ___
            include($this->root."developers");
            // ______________________________________________ Set developer if correct ___
            if (in_array($dev, array_keys($this->developers))) {
                if (!$this->developers[$dev] || $this->developers[$dev] == $key) {
                    $this->developer = $dev;
                    $this->dev_key   = $key;
                    }
                }
            }

        }


    /**
     * Returns o2 system tables list as array
     *
     * @return array
     */
    function sys_tabs() {

        return array('o2_users',
                     'o2_sessions',
                     'o2_options',
                     'o2_rights',
                     'o2_roles',
                     'o2_rights_per_role',
                     'o2_roles_per_user',
                     'o2_rights_roles_desc',
                     'o2_tabslog',
                     'o2_profiling',
                     'o2_tmp_where',
                     'o2_custom_export',
                     'o2_custom_grids',
                     'o2_constraints',
                     'o2_extraindexes',
                     'jx_jobs',
                     'jx_running_jobs',
                     'jx_scheduler',
                     'jx_services',
                     'jx_hosts',
                     'jx_translations',
                     'jx_access_log',
                     'jx_asp_areas',
                     'jx_otp');

        }


    /**
     * Retrieves and stores (in $_REQUEST) command line parameters
     *
     */
    function get_cli_params() {

        $cmdline_params = array();
        for ($par_index = 1; $par_index < $_SERVER['argc']; $par_index++) {
            $single_par = $_SERVER['argv'][$par_index];
            if (strpos($single_par, '=')) {
                list ($par_name, $par_value) = explode('=', $single_par, 2);
                }
            else {
                $par_name  = $single_par;
                $par_value = true;
                }
            $cmdline_params[$par_name] = $par_value;
            }
        $_REQUEST = array_merge($_REQUEST, $cmdline_params);

        }


    /**
     * Sets runtime options from ini file or from passed list
     *
     * @param array $list   List of passed parameters to set
     */
    function read_options($list = false) {

        if ($list) {
            // _________________________________________ Gets options from passed list ___
            $conf            = $list;
            $this->conf_list = array();
            }
        elseif ($this->root->exists("janox.ini")) {
            // ___________________________ Gets options from RUNTIME INI file settings ___
            $ini_file = $this->root->element("janox.ini");
            if (!($conf = parse_ini_file($ini_file))) {
                throw new o2_exception("Cannot access setting file <i>".$ini_file."</i>",
                                       o2error_IO);
                return;
                }
            $this->conf_list = $conf;
            }
        else {
            return;
            }
        $conf['o2_alias']         && $this->alias           = $conf['o2_alias'];
        $conf['jx_alias']         && $this->alias           = $conf['jx_alias'];
        isset($conf['decimals'])  && $this->decimals_point  = $conf['decimals'];
        isset($conf['thousands']) && $this->thousands_point = $conf['thousands'];
        isset($conf['data_chunk']) && $this->def_datachunk  = intval($conf['data_chunk']);
        $conf['php_exe']          && $this->php_engine      = $conf['php_exe'];
        isset($conf['multisession']) && $this->multisession = $conf['multisession'];

        }


    function find_php_exe() {

        if ($this->mswin) {
            $php_exe = 'php.exe';
            }
        else {
            $php_exe = 'php';
            }
        $found = false;
        // ______________________________________________________ Found PHP_BINDIR ___
        if (PHP_BINDIR) {
            if (file_exists(PHP_BINDIR.DIRECTORY_SEPARATOR.$php_exe)) {
                $php_exe = PHP_BINDIR.DIRECTORY_SEPARATOR.$php_exe;
                $found   = true;
                }
            }
        if (!$found && ($dir = ini_get('extension_dir'))) {
            if (file_exists($dir.DIRECTORY_SEPARATOR.$php_exe)) {
                $php_exe = $dir.DIRECTORY_SEPARATOR.$php_exe;
                $found   = true;
                }
            }
        if (!$found && ($dir = php_ini_loaded_file())) {
            if (file_exists(dirname($dir).DIRECTORY_SEPARATOR.$php_exe)) {
                $php_exe = dirname($dir).DIRECTORY_SEPARATOR.$php_exe;
                $found   = true;
                }
            }
        $this->php_engine = ($found ? $php_exe : false);

        }


    function load_gateway($type = 'sqlite3') {

        if ($type) {
            require_once($this->dir_dbms."jxdb_".$type.".inc");
            }
        else {
            throw new o2_exception("Missing database engine", o2error_UNKNOWNDBSERVER);
            return false;
            }
        return true;

        }


    function menu() {

        require_once($this->root."lib/prgs/menus.inc");

        }


    function prg($prg_name) {

        require($this->root."lib/prgs/".$prg_name.".prg");

        }


    function prf($prg_name) {

        require_once($this->root."lib/prgs/".$prg_name.".prf");

        }


    /*
     * Load repositories for the Janox runtime to define system objects
     */
    function tools_repository() {

        require_once($this->root."lib/prgs/models.inc");
        require_once($this->root."lib/prgs/dbs.inc");
        $this->read_sys_tabs = true;
        require_once($this->root."lib/prgs/tables.inc");
        $this->read_sys_tabs = false;

        }


    /**
     * Returns runtime release code and max modification date for core scripts
     *
     * @return string
     */
    function release() {

        return $this->release." :: ".$this->built;

        }


    /**
     * Execute command in a separate thread, starting a new PHP engine, outside web server
     * control. Returns TRUE if execution is started.
     *
     * @param  string $cmd
     * @return boolean
     */
    function batch_exec($cmd) {

        if ($this->mswin) {
            // _______________________ Unrem next line to debug run process on Windows ___
//          $cmd = "%comspec% /c ".$cmd." > ".
//                 $_SESSION['o2_app']->dir_logs."exe".substr(time(), -5);
            $wshshell = new COM("WScript.Shell");
            $wshshell->Run($cmd, 0, false);
            }
        else {
            system("(".$cmd.") > /dev/null &");
            // ________________________ Use this command to debug run process on Gnome ___
//          system("gnome-terminal --command=\"".$cmd."\"\nread");
            }
        return true;

        }


    /**
     * Returns list of active processes with PIDs.
     * If $php_only parameter is passed as TRUE, then only PHP processes PIDs are
     * returned, else all processes, regardless the executable file.
     *
     * @param  boolean $php_only
     * @return array
     */
    function proc_list($php_only = false) {

        $list = array();
        if ($this->mswin) {
            $ps = explode("\n", shell_exec("TASKLIST /FO CSV /NH"));
            if (count($ps) > 1) {
                foreach ($ps as $line) {
                    $info = explode(",", $line, 3);
                    $pid  = intval(trim($info[1], '"'));
                    $exe  = trim($info[0], '"');
                    if (!$php_only || strtolower(substr($exe, 0, 3)) == 'php') {
                        $list[$pid] = $exe;
                        }
                    }
                }
            }
        else {
            $ps = explode("\n", shell_exec("ps h -e -o pid,comm"));
            if (count($ps) > 1) {
                foreach ($ps as $line) {
                    $info = explode(" ", ltrim($line), 3);
                    $pid  = intval(trim($info[0], '"'));
                    $exe  = trim($info[1], '"');
                    if (!$php_only || strtolower(substr($exe, 0, 3)) == 'php') {
                        $list[$pid] = $exe;
                        }
                    }
                }
            }
        return $list;

        }


    /**
     * Kills process with ID $pid
     *
     * @return boolean
     */
    function kill($pid) {

        if (array_key_exists($pid, $this->proc_list())) {
            if ($this->mswin) {
                shell_exec("TASKKILL /F /PID ".$pid);
                }
            else {
                shell_exec("kill -9 ".$pid);
                }
            }
        else {
            return true;
            }
        return true;

        }


    /**
     * Provide access to o2_inspector object
     *
     */
    function inspect() {

        $params     = $_SERVER['argv'];
        $app_script = $params[2];
        $developer  = $params[3];
        $method     = $params[4];
        array_shift($params); // _______________________________ Remove runtime script ___
        array_shift($params); // _______________________________ Remove "inspect" word ___
        array_shift($params); // ___________________________ Remove application script ___
        array_shift($params); // ____________________________________ Remove developer ___
        array_shift($params); // _______________________________________ Remove method ___
        // ____________________________________________________ Check requested method ___
        if (method_exists('o2_inspector', 'find_'.$method)) {
            $inspect = new o2_inspector($app_script, $developer);
            $nodes   = call_user_func_array(array($inspect, 'find_'.$method), $params);
            $json    = "{";
            foreach ($nodes as $node_id => $info) {
               $json.= '"'.$node_id.'":["'.$info[0].'","'.
                                           addslashes($info[1]).'","'.
                                           (isset($info[2]) ? $info[2] : '').'","'.
                                           (isset($info[3]) ? $info[3] : '').'"],';
               }
            print substr($json, 0, -1)."}\n";
            }
        else {
            print 'ERROR: Unknown requested inspection method "'.$method.'"'."\n";
            }
        }


    /**
     * Provide access to Janox Obscure methods
     *
     */
    function obscure($app_script) {

        // __________________________________________________ Check application script ___
        if (!file_exists($app_script)) {
            print 'ERROR: Unknown application ('.$app_script.")\n";
            die();
            }
        // _____________________ Get Inspector (for application file-system structure) ___
        $inspector = new o2_inspector($app_script);
        // __________________________________________________ Get application programs ___
        $prgs      = array();
        foreach ($inspector->dir_prgs->all_elements("*.prf") as $prf) {
            $prgs[$prf->nome] = $prf->nome_completo;
            }
        // _____________________________ Load configuration and preset default level 2 ___
        $conf          = @parse_ini_file($inspector->dir_home.'obs.ini', true);
        $conf['level'] = (isset($conf['level']) ? $conf['level'] : 2);
        $header        = (isset($conf['header']) ? $conf['header'] : false);
        // _______________________________________________ Start obscuring application ___
        print "\n ************************************".
              "\n *   Janox Application Osbscuring   *".
              "\n ************************************\n".
              "\n Application: ".ucfirst(substr(substr($app_script, 0, -4),
                                          strrpos($app_script, DIRECTORY_SEPARATOR) + 1)).
              "\n From: ".$inspector->dir_home.
              "\n Level: ".$conf['level']."\n\nObscuring programs...\n";
        // _________________________________________________ Get file-system structure ___
        $dir_prgs_obs = new o2_dir($inspector->dir_home.'prgs_obs');
        print "> Create or refresh folder \"prgs_obs\"\n";
        $dir_prgs_obs->remove();
        $dir_prgs_obs->crea();
        // __________________________________________________________ Obscure programs ___
        foreach (array_keys($prgs) as $prg) {
            $res = $inspector->obscure($prg, $conf);
            file_put_contents($dir_prgs_obs.$prg.'.prg', $res['prg']);
            file_put_contents($dir_prgs_obs.$prg.'.prf', $res['prf']);
            if ($res['log']) {
                print '> Program "'.$prg."\":\n".$res['log'];
                }
            }
        // _______________________________________________________ Obscure extra files ___
        if (isset($conf['EXTRA']) &&
            isset($conf['EXTRA']['file'])) {
            print "\nObscuring extra files...\n";
            $extra_files = $conf['EXTRA']['file'];
            // _____________________________________ Support syntax "file=single_file" ___
            if (!is_array($extra_files)) {
                $extra_files = array($extra_files);
                }
            $files    = array();
            $obs_dirs = array();
            foreach ($extra_files as $file) {
                // _________________________________ Path relative to application root ___
                if (substr($file, 0, 1) == DIRECTORY_SEPARATOR ||
                    substr($file, 0, 1) == '/') {
                    $file = substr($file, 1);
                    }
                $full_file = $inspector->dir_home.$file;
                if (file_exists($full_file)) {
                    $file_obj = new o2_fsitem($full_file);
                    }
                else {
                    print "> Extra file ".$full_file." not found\n";
                    continue;
                    }
                // _____________________________________ Create obs root for each file ___
                $file  = str_replace(DIRECTORY_SEPARATOR, '/', $file);
                $parts = explode('/', $file, 2);
                if (count($parts) > 1) {
                    $dir_obs  = new o2_dir($inspector->dir_home.$parts[0].'_obs');
                    $file_obs = $parts[1];
                    $obs_dirs[$parts[0].'_obs'] = $dir_obs;
                    }
                else {
                    $dir_obs  = new o2_dir($inspector->dir_home.'obs');
                    $file_obs = $parts[0];
                    $obs_dirs['obs'] = $dir_obs;
                    }
                $files[''.$file_obj] = array(''.$file_obs, ''.$dir_obs);
                }
            foreach ($obs_dirs as $log_dir => $dir_obj) {
                if (''.$dir_obj != ''.$dir_prgs_obs) {
                    print "> Create or refresh folder \"".$log_dir."\"\n";
                    $dir_obj->remove();
                    $dir_obj->crea();
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
            foreach ($files as $full_file => $file_info) {
                $full_file_obs = new o2_fsitem($file_info[1].$file_info[0]);
                $full_file_dir = new o2_dir($full_file_obs->path);
                $full_file_dir->crea();
                // ___________________________________ Strip white spaces and comments ___
                $code = $header.substr(php_strip_whitespace($full_file), 6);
                file_put_contents($full_file_obs, $code);
                }
            }
        print "\nDone.\n\n";

        }


    }


/**
 * Mail message object
 *
 */
class o2_mail {

    /*     ===== PROPERTIES =========================================================== */
    private $from       = "";      /* FROM address                                      */
    private $to         = "";      /* TO addresses list                                 */
    private $cc         = "";      /* CC addresses list                                 */
    private $bcc        = "";      /* BCC addresses list                                */
    private $attachment = array(); /* Attachments list                                  */
    private $chr_set    = "";      /* Character set encoding                            */
    private $boundary   = "";      /* Boundary unique string                            */
    private $header     = "";      /* Message header                                    */
    private $subject    = "";      /* Message subject                                   */
    private $body       = "";      /* Message body                                      */


    function __construct($from, $encoding = false) {

        $this->boundary = ">".md5(uniqid(time()))."<";
        $this->chr_set  = ($encoding ? $encoding : "windows-1252");
        $this->header   = "From: ".$from."\r\n";
        $this->from     = $from;

        }


    function to($to) {

        if (is_array($to)) {
            $to = implode(",", $to);
            }
        else {
            $to = str_replace(";", ",", $to);
            }
        $this->to = ($this->to ? $this->to."," : "").$to;

        }


    function cc($cc) {

        if (is_array($cc)) {
            $cc = implode(",", $cc);
            }
        else {
            $cc = str_replace(";", ",", $cc);
            }
        $this->cc = ($this->cc ? $this->cc."," : "").$cc;

        }


    function bcc($bcc) {

        if (is_array($bcc)) {
            $bcc = implode(",", $bcc);
            }
        else {
            $bcc = str_replace(";", ",", $bcc);
            }
        $this->bcc = ($this->bcc ? $this->bcc."," : "").$bcc;

        }


    function attachment($file) {

        if (is_array($file)) {
            $list = $file;
            }
        else {
            $file = str_replace(";", ",", $file);
            $list = explode(",", $file);
            }
        foreach ($list as $single_att) {
            $this->attachment[] = str_replace(chr(92), chr(47), $single_att);
            }

        }


    function subject($subject) {

        $this->subject = $subject;

        }


    function text($text) {

        $this->body.= "Content-Type: text/plain; charset=".$this->chr_set."\n";
        $this->body.= "Content-Transfer-Encoding: 7bit\n\n";
        $this->body.= $text."\n";

        }


    function html($html) {

        $this->body.= "Content-Type: text/html; charset=".$this->chr_set."\n";
        $this->body.= "Content-Transfer-Encoding: quoted-printable\n\n";
        $this->body.= "<html><body>\n".$html."\n</body></html>\n\n";

        }


    function send() {

        // ____________________________________________________________________ Headers ___
        $this->header.= "To: ".$this->to."\r\n".
                        ($this->cc ? "Cc: ".$this->cc."\r\n" : "").
                        ($this->bcc ? "Bcc: ".$this->bcc."\r\n" : "");
        // ____________________________________ Extra parameters, needed in Linux only ___
        if (!$GLOBALS['o2_runtime']->mswin) {
            $extra_params = "-r".$this->from;
            }
        else {
            $extra_params = "";
            }
        // _____________________________________________________ Needed for attachment ___
        $this->header.= "Content-Type: multipart/mixed; boundary=\"".$this->boundary."\"";
        // _____________________________________________________________ Message body  ___
        $msg = "--".$this->boundary."\n";
        // _____________________________________________________ Add message body text ___
        $msg.= $this->body."\n";
        // _______________________________________________________ Loop on attachments ___
        foreach ($this->attachment as $att) {
            if (file_exists($att)) {
                $att_type = o2file_ext($att);
                $att_name = o2file_basename($att).($att_type ? ".".$att_type : "");
                // _________________________________ Read file and encode it in base64 ___
                $file = fopen($att, 'rb');
                $data = chunk_split(base64_encode(fread($file, filesize($att))));
                fclose($file);
                $msg.= "--".$this->boundary."\n";
                // ____________________________________ Add attachment to message body ___
                $msg.= "Content-Type: application/".$att_type."; name=\"".$att_name.
                       "\"\nContent-Disposition: attachment; filename=\"".$att_name.
                       "\"\nContent-Transfer-Encoding: base64\n\n".$data;
                }
            }
        $msg.= "--".$this->boundary."--\n";
        if (!mail($this->to, $this->subject, $msg, $this->header, $extra_params)) {
            throw new o2_exception("Error sending mail to <i>".$this->to."</i>.",
                                   o2error_IO);
            return false;
            }
        return true;

        }

    }


/**
 * Dispatcher object class.
 * Dispatcher is used to manage messages readable on demand by user.
 *
 */
class o2_dispatcher {

    private $dispatches = array(); /* Messages list                                     */
    private $id_counter = 0;       /* Internal counter for unique id                    */
    private $alert      = false;   /* Alert status flag                                 */


    /**
     * Return a dispatcher object for Janox runtime. Can be created only one dispatcher at
     * a time.
     * Method must be called statically.
     *
     * @return o2_dispatcher
     */
    static function get_dispatcher() {

        $app = $_SESSION['o2_app'];
        if (!$app->dispatcher) {
            $app->dispatcher = new o2_dispatcher();
            }
        return $app->dispatcher;

        }


    /**
     * Add a dispatch to the list.
     * Return new ID on success and FALSE on fail.
     * Parameters passed after $prg_name are passed as parameters to called program.
     *
     * @param  string  $title
     * @param  string  $body
     * @param  string  $icon
     * @param  boolean $only_label
     * @param  string  $prg_name
     * @return integer
     */
    function dispatch_add($title,
                          $body       = "",
                          $icon       = false,
                          $only_label = false,
                          $prg_name   = false) {

        // _____________________________________________________ Gets extra parameters ___
        $params = func_get_args();
        array_shift($params); // _______________________________________ Shifts $title ___
        array_shift($params); // ________________________________________ Shifts $body ___
        array_shift($params); // ________________________________________ Shifts $icon ___
        array_shift($params); // __________________________________ Shifts $only_label ___
        array_shift($params); // ____________________________________ Shifts $prg_name ___
        $this->id_counter++;
        // ____________________________________________________ Create dispatch packet ___
        $this->dispatches[$this->id_counter] = array("title" => $title,
                                                     "body"  => $body,
                                                     "img"   => $icon,
                                                     "prg"   => $prg_name,
                                                     "pars"  => $params,
                                                     "label" => $only_label);
        return $this->id_counter;

        }


    /**
     * Set attivation program and parameters for requested dispatch.
     * Return TRUE on success and FALSE on fail.
     * Parameters passed after $prg_name are passed as parameters to called program.
     *
     * @param  integer  $icon
     * @param  string   $prg_name
     * @return boolean
     */
    function dispatch_set_prg($id, $prg_name) {

        // _____________________________________________________ Gets extra parameters ___
        $params = func_get_args();
        array_shift($params); // __________________________________________ Shifts $id ___
        array_shift($params); // ____________________________________ Shifts $prg_name ___
        // _________________________________ Set prg and parameters in dispatch packet ___
        if ($this->dispatches[$id]) {
            $this->dispatches[$id]["prg"]  = $prg_name;
            $this->dispatches[$id]["pars"] = $params;
            return true;
            }
        else {
            return false;
            }

        }


    /**
     * Return a dispatch from the list.
     * Return dispatch packet on success and FALSE on fail.
     *
     * @param  integer $id
     * @return array
     */
    function get_dispatch($id) {

        if (isset($this->dispatches[$id])) {
            return $this->dispatches[$id];
            }
        else {
            return false;
            }

        }


    /**
     * Remove a dispatch from the list.
     * Return the removed dispatch packet on success and FALSE on fail.
     *
     * @param  integer $id
     * @return array
     */
    function dispatch_remove($id) {

        if (isset($this->dispatches[$id])) {
            $dispatch = $this->dispatches[$id];
            // _____________________________________________ Remove dispatch from list ___
            unset($this->dispatches[$id]);
            return $dispatch;
            }
        else {
            return false;
            }

        }


    /**
     * Return TRUE if dispatch is activable, else FALSE.
     *
     * @param  integer $id
     * @return boolean
     */
    function dispatch_can_activate($id) {

        return (isset($this->dispatches[$id]) && $this->dispatches[$id]["prg"]);

        }


    /**
     * Activate the program binded to dispatch, if any.
     * Return TRUE on success and FALSE on fail.
     * If $preserve is passed as FALSE (default) dispatch is removed after activation.
     *
     * @param  integer $id
     * @param  boolean $preserve
     * @return boolean
     */
    function dispatch_activate($id, $preserve = false) {

        if (isset($this->dispatches[$id]) && $this->dispatches[$id]["prg"]) {
            $pars = $this->dispatches[$id]["pars"];
            array_unshift($pars, $this->dispatches[$id]["prg"]);
            return call_user_func_array(array($_SESSION['o2_app'], "intcall"), $pars);
            }
        else {
            return false;
            }

        }


    /**
     * Return the number of dispatches in list.
     *
     * @return integer
     */
    function count() {

        $count = 0;
        foreach ($this->dispatches as $dispatch) {
            if (!$dispatch["label"]) {
                $count++;
                }
            }
        return $count;

        }


    /**
     * Set alert status ON and show $msg
     *
     * @param  string $msg
     * @return mix
     */
    function alert($msg = '') {

        if (!$msg) {
            $msg = 'You have unread messages!';
            }
        $ret         = $this->alert;
        $this->alert = $msg;
        return $ret;

        }


    /**
     * Return alert message and reset alert status
     *
     * @return mix
     */
    function get_alert() {

        $ret         = $this->alert;
        $this->alert = false;
        return $ret;

        }


    /**
     * Return the array of dispatches IDs.
     *
     * @return array
     */
    function get_list() {

        return array_keys($this->dispatches);

        }


    /**
     * Clear dispatches list and reset dispatcher status
     *
     * @return boolean
     */
    function clear() {

        $this->dispatches = array();
        $this->id_counter = 0;
        return true;

        }

    }


/**
 * Janox Updates Distribution object class.
 * This class check for new runtime/applications releases over the internet and manages
 * download, alerting users and upgrades installation.
 *
 */
class jud {

    private $repository = ""; /* Jud over the internet site address                     */


    function __construct($repository = false) {

        if (!$repository) {
            $repository = "http://xeon.software4u.it/jud/jud.php";
            }
        $this->repository = $repository;

        }


    /**
     * Execute a request to the repository and return the response as an array
     *
     * @param  string $prj   Project to check: if not passed Janox runtime project will be
     *                       checked
     * @param  string $ver   Version label
     * @param  string $os    Operating system platform
     * @return array
     */
    function request($prj = "", $ver = "", $os = "") {

        // _____________________________ If no project check for Janox runtime updates ___
        if (!$prj) {
            $prj = "JANOX";
            }
        else {
            $prj = strtoupper($prj);
            }
        $ver     = strtolower($ver);
        $os      = strtoupper($os);
        $resp    = file($this->repository.
                        "?user=default&get=latest&prj=".$prj."&ver=".$ver."&os=".$os,
                        FILE_IGNORE_NEW_LINES);
        $errlist = array();
        foreach ($resp as $line) {
            @eval('$'.trim($line).";");
            if (isset($error)) {
                $errlist[] = $error."<br>";
                unset($error);
                }
            elseif (isset($warning)) {
                $errlist[] = $warning."<br>";
                unset($warning);
                }
            }
        return array("project"  => $project,
                     "version"  => $version,
                     "release"  => $release,
                     "package"  => $builtid,
                     "platform" => $platform,
                     "reldate"  => $reldate,
                     "reltime"  => $reltime,
                     "filehash" => $filehash,
                     "downurl"  => $downurl,
                     "errors"   => implode("\n", $errlist));

        }

    }

/******************************************************************* system functions ***/

/**
 * Returns actual timestamp in milliseconds
 *
 * @return float
 */
function o2_time(){

    return microtime(true);

    }


/**
 * Returns passed $string converted to current application encoding (HTML output)
 *
 * @param  string $string   String to bge converted
 * @return string
 */
function jx_encode($string) {

    $encod = $_SESSION['o2_app']->chr_encoding;
    if ($encod == "UTF-8") {
        try {
            if (!mb_check_encoding($string, $encod)) {
                $string = mb_convert_encoding($string, $encod);
                }
            }
        catch (Exception $e) {
            throw new o2_exception("Error converting string <code>\"".$string.
                                   '"</code> to chr-encoding <i>'.$encod.'</i>.',
                                   o2error_IO);
            return false;
            }
        }
    return $string;

    }


/**
 * Prints out Janox logo. Parameter is proporsional to text dimension: 1 (default) = 1em.
 *
 * @param  float $dim
 * @return string
 */
function jxlogo($dim = 1) {

    $t    = "Visit www.janox.it";
    $f    = "font-family:Arial,sans-serif;";
    $fj   = "font-family:mono;";
    $g    = "#999999;";
    $b    = "font-weight:bold;";
    $fs   = "font-size:";
    $tb   = "border:1px solid ".$g;
    $td   = "padding:0 3px 0 3px;margin:0;text-align:center;";
    $bg   = "padding:10px;background-color:#fefefe;";
    $red  = $td."color:#fefefe;background-color:#993300;".$fj.$b.$fs.$dim.
              "em;border-left:1px solid ".$g;
    $grey = $td."color:#fefefe;white-space:nowrap;".$f.
            $fs.($dim * 0.72)."em;background-color:".$g;
    $j    = $td."color:#993300;background-color:#f7c000;".$fj.$b.$fs.($dim * 2)."em;";
    return '<a style="text-decoration:none;" href="http://www.janox.it" title="'.$t.
           '"><table cellspacing="0" cellpadding="0" style="'.$tb.
           '"><tr><td rowspan="2" style="'.$j.'">J</td><td colspan="4" style="'.$grey.
           '">development suite</td></tr><tr><td style="'.$red.
           '">a</td><td style="'.$red.'">n</td><td style="'.$red.
           '">o</td><td style="'.$red.'">x</td></tr></table></a>';

    }


/**
 * Converte una qualunque variabile in array, ricorsivamente nei suoi sottoelementi
 *
 * @param  mix $obj
 * @return array
 */
function obj2array($obj = null) {

    switch (gettype($obj)) {
        case "NULL":
            $array_local = array();
            break;
        case "object":
            $array_local = array();
            $array_tmp = (array) $obj;
            foreach ($array_tmp as $indice => $singolo_elemento) {
                $array_local[$indice] = obj2array($singolo_elemento);
                }
            break;
        case "array":
            $array_local = array();
            $array_tmp = $obj;
            foreach ($array_tmp as $indice => $singolo_elemento) {
                $array_local[$indice] = obj2array($singolo_elemento);
                }
            break;
        default:
            $array_local = $obj;
        }
    return $array_local;

    }


/**
 * Tries to return the correct mime type for the given file by extension.
 *
 * @param  string $file_name
 * @param  string $file_ext
 * @return string
 */
function o2_mime_content($file_name, $file_ext = "") {

    // _________________________________________________________ Load mime-types array ___
    $mime_types = array("323"     => "text/h323",
                        "acx"     => "application/internet-property-stream",
                        "ai"      => "application/postscript",
                        "aif"     => "audio/x-aiff",
                        "aifc"    => "audio/x-aiff",
                        "aiff"    => "audio/x-aiff",
                        "asf"     => "video/x-ms-asf",
                        "asr"     => "video/x-ms-asf",
                        "asx"     => "video/x-ms-asf",
                        "au"      => "audio/basic",
                        "avi"     => "video/x-msvideo",
                        "axs"     => "application/olescript",
                        "bas"     => "text/plain",
                        "bcpio"   => "application/x-bcpio",
                        "bin"     => "application/octet-stream",
                        "bmp"     => "image/bmp",
                        "c"       => "text/plain",
                        "cat"     => "application/vnd.ms-pkiseccat",
                        "cdf"     => "application/x-cdf",
                        "cer"     => "application/x-x509-ca-cert",
                        "class"   => "application/octet-stream",
                        "clp"     => "application/x-msclip",
                        "cmx"     => "image/x-cmx",
                        "cod"     => "image/cis-cod",
                        "cpio"    => "application/x-cpio",
                        "crd"     => "application/x-mscardfile",
                        "crl"     => "application/pkix-crl",
                        "crt"     => "application/x-x509-ca-cert",
                        "csh"     => "application/x-csh",
                        "css"     => "text/css",
                        "dcr"     => "application/x-director",
                        "der"     => "application/x-x509-ca-cert",
                        "dir"     => "application/x-director",
                        "dll"     => "application/x-msdownload",
                        "dms"     => "application/octet-stream",
                        "doc"     => "application/msword",
                        "docx"    => "application/vnd.openxmlformats-officedocument".
                                     ".wordprocessingml.document",
                        "dot"     => "application/msword",
                        "dvi"     => "application/x-dvi",
                        "dxr"     => "application/x-director",
                        "eps"     => "application/postscript",
                        "etx"     => "text/x-setext",
                        "evy"     => "application/envoy",
                        "exe"     => "application/octet-stream",
                        "fif"     => "application/fractals",
                        "flr"     => "x-world/x-vrml",
                        "gif"     => "image/gif",
                        "gtar"    => "application/x-gtar",
                        "gz"      => "application/x-gzip",
                        "h"       => "text/plain",
                        "hdf"     => "application/x-hdf",
                        "hlp"     => "application/winhlp",
                        "hqx"     => "application/mac-binhex40",
                        "hta"     => "application/hta",
                        "htc"     => "text/x-component",
                        "htm"     => "text/html",
                        "html"    => "text/html",
                        "htt"     => "text/webviewhtml",
                        "ico"     => "image/x-icon",
                        "ief"     => "image/ief",
                        "iii"     => "application/x-iphone",
                        "ins"     => "application/x-internet-signup",
                        "isp"     => "application/x-internet-signup",
                        "jfif"    => "image/pipeg",
                        "jpe"     => "image/jpeg",
                        "jpeg"    => "image/jpeg",
                        "jpg"     => "image/jpeg",
                        "js"      => "text/javascript",
                        "latex"   => "application/x-latex",
                        "lha"     => "application/octet-stream",
                        "log"     => "text/plain",
                        "lsf"     => "video/x-la-asf",
                        "lsx"     => "video/x-la-asf",
                        "lzh"     => "application/octet-stream",
                        "m13"     => "application/x-msmediaview",
                        "m14"     => "application/x-msmediaview",
                        "m3u"     => "audio/x-mpegurl",
                        "man"     => "application/x-troff-man",
                        "mdb"     => "application/x-msaccess",
                        "me"      => "application/x-troff-me",
                        "mht"     => "message/rfc822",
                        "mhtml"   => "message/rfc822",
                        "mid"     => "audio/mid",
                        "mny"     => "application/x-msmoney",
                        "mov"     => "video/quicktime",
                        "movie"   => "video/x-sgi-movie",
                        "mp2"     => "video/mpeg",
                        "mp3"     => "audio/mpeg",
                        "mpa"     => "video/mpeg",
                        "mpe"     => "video/mpeg",
                        "mpeg"    => "video/mpeg",
                        "mpg"     => "video/mpeg",
                        "mpp"     => "application/vnd.ms-project",
                        "mpv2"    => "video/mpeg",
                        "ms"      => "application/x-troff-ms",
                        "mvb"     => "application/x-msmediaview",
                        "nws"     => "message/rfc822",
                        "oda"     => "application/oda",
                        "p10"     => "application/pkcs10",
                        "p12"     => "application/x-pkcs12",
                        "p7b"     => "application/x-pkcs7-certificates",
                        "p7c"     => "application/x-pkcs7-mime",
                        "p7m"     => "application/x-pkcs7-mime",
                        "p7r"     => "application/x-pkcs7-certreqresp",
                        "p7s"     => "application/x-pkcs7-signature",
                        "pbm"     => "image/x-portable-bitmap",
                        "pdf"     => "application/pdf",
                        "pfx"     => "application/x-pkcs12",
                        "pgm"     => "image/x-portable-graymap",
                        "pko"     => "application/ynd.ms-pkipko",
                        "pma"     => "application/x-perfmon",
                        "pmc"     => "application/x-perfmon",
                        "pml"     => "application/x-perfmon",
                        "pmr"     => "application/x-perfmon",
                        "pmw"     => "application/x-perfmon",
                        "pnm"     => "image/x-portable-anymap",
                        "png"     => "image/png",
                        "pot"     => "application/vnd.ms-powerpoint",
                        "ppm"     => "image/x-portable-pixmap",
                        "pps"     => "application/vnd.ms-powerpoint",
                        "ppt"     => "application/vnd.ms-powerpoint",
                        "prf"     => "application/pics-rules",
                        "ps"      => "application/postscript",
                        "pub"     => "application/x-mspublisher",
                        "qt"      => "video/quicktime",
                        "ra"      => "audio/x-pn-realaudio",
                        "ram"     => "audio/x-pn-realaudio",
                        "ras"     => "image/x-cmu-raster",
                        "rgb"     => "image/x-rgb",
                        "rmi"     => "audio/mid",
                        "roff"    => "application/x-troff",
                        "rtf"     => "application/rtf",
                        "rtx"     => "text/richtext",
                        "scd"     => "application/x-msschedule",
                        "sct"     => "text/scriptlet",
                        "setpay"  => "application/set-payment-initiation",
                        "setreg"  => "application/set-registration-initiation",
                        "sh"      => "application/x-sh",
                        "shar"    => "application/x-shar",
                        "sit"     => "application/x-stuffit",
                        "snd"     => "audio/basic",
                        "spc"     => "application/x-pkcs7-certificates",
                        "spl"     => "application/futuresplash",
                        "src"     => "application/x-wais-source",
                        "sst"     => "application/vnd.ms-pkicertstore",
                        "stl"     => "application/vnd.ms-pkistl",
                        "stm"     => "text/html",
                        "svg"     => "image/svg+xml",
                        "sv4cpio" => "application/x-sv4cpio",
                        "sv4crc"  => "application/x-sv4crc",
                        "swf"     => "application/x-shockwave-flash",
                        "t"       => "application/x-troff",
                        "tar"     => "application/x-tar",
                        "tcl"     => "application/x-tcl",
                        "tex"     => "application/x-tex",
                        "texi"    => "application/x-texinfo",
                        "texinfo" => "application/x-texinfo",
                        "tgz"     => "application/x-compressed",
                        "tif"     => "image/tiff",
                        "tiff"    => "image/tiff",
                        "tr"      => "application/x-troff",
                        "trm"     => "application/x-msterminal",
                        "tsv"     => "text/tab-separated-values",
                        "txt"     => "text/plain",
                        "uls"     => "text/iuls",
                        "ustar"   => "application/x-ustar",
                        "vcf"     => "text/x-vcard",
                        "vrml"    => "x-world/x-vrml",
                        "wav"     => "audio/wav",
                        "wcm"     => "application/vnd.ms-works",
                        "wdb"     => "application/vnd.ms-works",
                        "wks"     => "application/vnd.ms-works",
                        "wmf"     => "application/x-msmetafile",
                        "wps"     => "application/vnd.ms-works",
                        "wri"     => "application/x-mswrite",
                        "wrl"     => "x-world/x-vrml",
                        "wrz"     => "x-world/x-vrml",
                        "xaf"     => "x-world/x-vrml",
                        "xbm"     => "image/x-xbitmap",
                        "xla"     => "application/vnd.ms-excel",
                        "xlc"     => "application/vnd.ms-excel",
                        "xlm"     => "application/vnd.ms-excel",
                        "xls"     => "application/vnd.ms-excel",
                        "xlt"     => "application/vnd.ms-excel",
                        "xlw"     => "application/vnd.ms-excel",
                        "xml"     => "text/xml",
                        "xof"     => "x-world/x-vrml",
                        "xpm"     => "image/x-xpixmap",
                        "xwd"     => "image/x-xwindowdump",
                        "z"       => "application/x-compress",
                        "zip"     => "application/zip");
    // __________________________________________________ Check extension against list ___
    if (!$file_ext) {
        $psl      = pathinfo($file_name);
        $file_ext = $psl['extension'];
        }
    $file_ext = strtolower($file_ext);
    if (isset($mime_types[$file_ext])) {
        return $mime_types[$file_ext];
        }
    else {
        return "application/force-download";
        }

    }


/**
 * Returns passed string in a nice reading format, with "all" first letters capitalized.
 * Function is first intended to be used with HOST information.
 * For example, a host named:
 *    SCRIPT@MACHINE
 * is returned in the form:
 *    Script@Machine
 *
 * @param string $str
 * return string
 */
function o2_str_nice($str) {

    return preg_replace_callback('/\W(\w)/',
                                 function($m) { return strtoupper($m[0]); },
                                 ucfirst(strtolower($str)));

    }


/**
 * Returns passed object size, calculated in bytes.
 * This function is mostly used to weigth session data from analisys.
 *
 * @param mix       $obj
 * @param string    $name
 * @param string    $parent
 * @param integer   $limit
 * return integer
 */
function jx_sizeof($obj, $name = 'o2_app', $parent = 0, $limit = 0) {

    $res  = array();
    $size = 0;
    // __________________________________ Limit recursion deepness, to avoid crash (?) ___
    if ($limit > 20000) {
        return array($res, $size);
        }
    // _________________________________________________________________________ Array ___
    if (is_array($obj)) {
        $id   = md5(serialize($obj));
        $type = 'Array';
        $img  = 'menu';
        if (isset($GLOBALS['jxallobjs'][$id])) {
            $size+= $GLOBALS['jxallobjs'][$id];
            }
        else {
            foreach ($obj as $key => $value) {
                list($r, $s) = jx_sizeof($value, $key, $id, $limit + 1);
                $res += $r;
                $size+= $s;
                }
            $GLOBALS['jxallobjs'][$id] = $size;
            }
        }
    // ________________________________________________________________________ Object ___
    elseif (is_object($obj)) {
        $id   = spl_object_hash($obj);
        $type = get_class($obj);
        $img  = 'tab';
        if (isset($GLOBALS['jxallobjs'][$id])) {
            $size+= $GLOBALS['jxallobjs'][$id];
            }
        else {
            foreach (get_object_vars($obj) as $prop => $value) {
                list($r, $s) = jx_sizeof($value, $prop, $id, $limit + 1);
                $res += $r;
                $size+= $s;
                }
            $GLOBALS['jxallobjs'][$id] = $size;
            }
        }
    // _________________________________________________________________________ Value ___
    elseif (is_scalar($obj)) {
        $id   = uniqid('V', true);
        $type = 'Value';
        $size+= mb_strlen(serialize($obj), '8bit');
        $img  = 'field';
        }
    // _________________________________________________________ Resources and unknown ___
    else {
        $id   = uniqid('U', true);
        $type = 'Other';
        $size = 0;
        $img  = 'var';
        }
    $s = intval($size / 8);
    // __________________________________ Limit to show only items over a certain size ___
    if ($s >= 1000) {
        $res[$id] = array($parent,
                          $name.' ['.$type.'] '.$s,
                          '<jx>/img/tools/tree/'.$img.'.png');
        }
    return array($res, $size);

    }


/**
 * Define a provided update script for release back compatibility.
 * Actually upgrades are used only for system tables structure change.
 * When $test parameter is passed as TRUE only a check will be performed, without any
 * change applied: if upgrades are needed return TRUE, else FALSE.
 *
 * @param  o2_file $table      Table object to be upgraded
 * @param  string  $rel_code   Release code in the form v.r.m
 * @param  boolean $test       If test mode (no real upgrade)
 * @return boolean
 */
function o2_upgrade($table, $rel_code, $test = false) {

    $msg_txt = false;
    // ___________________________________________ First managed upgrade - Rel. 1.3.00 ___
    if ($rel_code < '1.2.08') {
        switch ($table->indice) {
            // _____________________________________________________ o2_users - 1.3.00 ___
            case "o2_users":
                $msg_txt = "Converting USERS";
                if (!$test) {
                    $table->ricostruisci(Array("O2USER"     => "O2USER",
                                               "O2PASSWORD" => "O2PASSWORD",
                                               "AREA"       => "ASP_AREA"));
                    }
                break;
            // ___________________________________________ o2_rights_per_role - 1.3.00 ___
            case "o2_rights_per_role":
                $msg_txt = "Converting RIGHTS PER ROLE";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    // ________________________________________________________ Upgrade to Rel. 2.2.00 ___
    if ($rel_code < '2.2.00') {
        switch ($table->indice) {
            // _____________________________________________________ o2_users - 2.2.00 ___
            case "o2_users":
                $msg_txt = "Converting USERS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            // __________________________________________________ o2_sessions - 2.2.00 ___
            case "o2_sessions":
                $msg_txt = "Converting SESSIONS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            // ______________________________________________________ jx_jobs - 2.2.00 ___
            case "jx_jobs":
                $msg_txt = "Converting JOBS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            // ______________________________________________ jx_running_jobs - 2.2.00 ___
            case "jx_running_jobs":
                $msg_txt = "Converting RUNNING-JOBS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    // ________________________________________________________ Upgrade to Rel. 2.3.00 ___
    if ($rel_code < '2.3.00') {
        switch ($table->indice) {
            // __________________________________________________ o2_sessions - 2.3.00 ___
            case "o2_sessions":
                $msg_txt = "Converting SESSIONS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    // ________________________________________________________ Upgrade to Rel. 2.4.00 ___
    if ($rel_code < '2.4.00') {
        switch ($table->indice) {
            // _____________________________________________________ o2_users - 2.4.00 ___
            case "o2_users":
                $msg_txt = "Converting USERS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    // ________________________________________________________ Upgrade to Rel. 2.5.00 ___
    if ($rel_code < '2.5.00') {
        switch ($table->indice) {
            // _____________________________________________________ o2_users - 2.5.00 ___
            case "o2_users":
                $msg_txt = "Converting USERS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    // ________________________________________________________ Upgrade to Rel. 2.6.00 ___
    if ($rel_code < '2.6.00') {
        switch ($table->indice) {
            // __________________________________________________ o2_sessions - 2.6.00 ___
            case "o2_sessions":
                $msg_txt = "Converting SESSIONS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            // ______________________________________________ jx_running_jobs - 2.6.00 ___
            case "jx_running_jobs":
                $msg_txt = "Converting RUNNING-JOBS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    // ________________________________________________________ Upgrade to Rel. 2.7.00 ___
    if ($rel_code < '2.7') {
        switch ($table->indice) {
            // _____________________________________________________ o2_users - 2.7.00 ___
            case "o2_users":
                $msg_txt = "Converting USERS";
                if (!$test) {
                    $table->ricostruisci();
                    }
                break;
            }
        }
    if ($msg_txt) {
        if (!$test) {
            if ($GLOBALS['o2_runtime']->interface == "HTML") {
                print "<h4>".$msg_txt."</h4>";
                }
            else {
                print $msg_txt."\n";
                }
            }
        return true;
        }
    else {
        return false;
        }

    }


/**
 * Reads a PHP script and returns an array of defined functions structure, with
 * description, returned data type, parameters and other informations from documentor
 * tags.
 * This function is used by Release Production Application (jam) to provide functions
 * list to the Development Interface and to create reference manual.
 *
 * string $text   Text to parse
 */
function read_func_def($text) {

    $matches = array();
    // ____________________________________ Get functions names and description blocks ___
    preg_match_all('/\/\*\*(.*?)\*\/\s*function\s&?(\w+)\s*\(/s', $text, $matches);
    $func = $matches[2];
    $desc = $matches[1];
    $ret  = array();
    // _____________________________________________________________ Loop on functions ___
    foreach ($func as $index => $single_func) {
        // _________________________________________________ Get "@package" tag values ___
        $matches = array();
        preg_match_all('/\s*\*\s*\@package\s*(\w+)/s', $desc[$index], $matches);
        $package = $matches[1][0];
        // __________________________________________________ Get "@return" tag values ___
        $matches = array();
        preg_match_all('/\s*\*\s*\@return\s*(\w+)/s', $desc[$index], $matches);
        $return  = $matches[1][0];
        // ______________________________________________________ Get description text ___
        $matches = array();
        preg_match_all('/^[^@]*/s', $desc[$index], $matches);
        $desc_txt = trim(str_replace("*", "", $matches[0][0]), "\n");
        $manual   = "";
        $feed     = false;
        $indent   = 0;
        $lastfree = 0;
        // ________________________________ Loop on description lines to get linefeeds ___
        foreach (explode("\n", $desc_txt) as $desc_line) {
            // __________________________________________ Remove first 2 empty columns ___
            $desc_line = substr(rtrim($desc_line), 2);
            // _________________ Check first word against previous trailing free space ___
            if (!$feed && strcspn(ltrim($desc_line), " \n\t") < $lastfree) {
                $manual.= "\n";
                $feed   = true;
                }
            // __________________________________________________ Get line indentation ___
            if ($feed && strspn($desc_line, " ")) {
                $manual.= " ";
                }
            // _______________________________________________ Get trailing free space ___
            $lastfree = 86 - strlen(rtrim($desc_line));
            // _____________________________________________________ Check empty lines ___
            if (!trim($desc_line)) {
                $desc_line = "\n";
                $feed      = true;
                }
            // _____________________________________ Add feed to lines ending with "." ___
            elseif (strspn(substr($desc_line, -1), ".!?")) {
                $desc_line = trim(preg_replace("/\s+/", " ", $desc_line))."\n";
                $feed      = true;
                }
            else {
                $desc_line = trim(preg_replace("/\s+/", " ", $desc_line))." ";
                $feed      = false;
                }
            $manual.= $desc_line;
            }

        // ____________________________________________________________ Get parameters ___
        $matches = array();
        preg_match_all('/\@param\s*(\w+)\s*(\W\w+)([^@]*)(\s*\*\/)?/s',
                       $desc[$index], $matches);
        $params    = array();
        $par_descs = array();
        $counter   = 0;
        if (count($matches[1])) {
            $par_name = "";
            $par_type = "";
            foreach ($matches[1] as $single_par) {
                // ___________ Sometimes type comes before param name, sometimes after ___
                if (strpos($matches[2][$counter], "$") !== false) {
                    $par_name = $matches[2][$counter];
                    $par_type = $single_par;
                    }
                else {
                    $par_name = (strpos($single_par, "$") !== false ? "" : "$").
                                $single_par;
                    $par_type = $matches[2][$counter];
                    }
                if ($par_name) {
                    $params[$par_name]    = $par_type;
                    $par_descs[$par_name] = trim(preg_replace('/\s\s+/', ' ',
                                            str_replace("*", "", $matches[3][$counter])));
                    }
                $counter++;
                }
            }
        // _____________________________________________________________ Get @see tags ___
        $matches = array();
        preg_match_all('"\@see\s*([^/@]*)"s', $desc[$index], $matches);
        $related = array();
        $counter = 0;
        if (count($matches[1])) {
            // ___________________________________________ There can be more @see tags ___
            foreach ($matches[1] as $single_tag) {
                // ________________ Each @see tag can have more comma separated values ___
                foreach (explode(",", $single_tag) as $single_see) {
                    $related[] = trim(str_replace("*", "", $single_see));
                    }
                $counter++;
                }
            }
        $ret[$single_func] = array("group"  => $package,
                                   "return" => $return,
                                   "params" => $params,
                                   "pdesc"  => $par_descs,
                                   "desc"   => $manual,
                                   "see"    => $related);
        }
    return $ret;

    }

?>
