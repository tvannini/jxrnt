<?php

/**
 * Janox Functions Library Module
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
 * @name      jxfnx
 * @package   janox/jxfnx.inc
 * @version   2.3
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * Returns current value of a field in a view, program or application variable.
 * When the view is a db-view it must be defined in the program where o2val() is used and
 * the field must be a field defined in that view.
 *
 * To get a program-variable value pass "prg§_§var" as view name.
 * To get an application (session) variable pass "_o2SESSION" as view name.
 *
 * If returned value is an array, indexed subvalues can be retrieved passing indexes as
 * supplementary parameters. For example if you have:
 *  o2val("view", "field") = array(1 => array("c" => "value"))
 * you can get "value" directly by calling:
 *  o2val("view", "field", 1, "c")
 *
 * If returned value is a Date or Time format, single parts of the value can be retrieved
 * passing a single, key-unsensitive letter as supplementary parameter:
 * "y"=Year, "m"=Month, "d"=Day, "h"=Hours, "i"=Minutes and "s"=Seconds.
 * For example if you have:
 *  o2val("view", "field") = (Date)"20130531" (31/05/2013)
 * you can get the month value directly by calling:
 *  o2val("view", "field", "m")
 * or if you have
 *  o2val("view", "field") = (Time)"184510" (18:45:10)
 * you can get the hours value directly by calling:
 *  o2val("view", "field", "h")
 *
 * @param  string $view_name    Name of the db-view or "prg§_§var" or "_o2SESSION"
 * @param  string $field_name   Name of the field or variable
 * @return mix
 * @see    o2pre()
 * @see    o2zero()
 * @see    o2par()
 * @see    o2format()
 */
function o2val($view_name, $field_name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    // _________________________________________________________ Application variables ___
    if ($view_name == '_o2SESSION') {
        if (!array_key_exists($field_name, $app->vars)) {
            throw new o2_exception("Unknown application variable <i>".$field_name."</i>",
                                   o2error_UNKNOWNAPPVAR);
            }
        else {
            return $app->vars[$field_name]->valore;
            }
        }
    // ________________________________________________________________ View variables ___
    $view_local = $prg->contesto[$view_name];
    if (!($view_local instanceof o2_view )) {
        throw new o2_exception("Unknown view <i>".$view_name."</i> in program <i>".
                               $app->istanze_prg[$app->progressivo_istanze]->nome."</i>",
                               o2error_UNKNOWNVIEW);
        }
    // ____ Tracing link criteria for SQL-LINK: excluded ones are treated as constants ___
    if ($view_name != "prg§_§var" &&
        $view_local->status != "I" &&
        !$view_local->modificato) {
        $idx = (count($prg->link_check) - 1);
        if ($idx > -1) {
            $prg->link_field[$idx] = $field_name;
            $prg->link_view[$idx]  = $view_name;
            $prg->link_check[$idx] = false;
            }
        }
    // ______________________________________________________________ Value retrieving ___
    $value_local = $view_local->campo($field_name);
    // ______________________________________________________ Extra arguments managing ___
    if (func_num_args() > 2) {
        // ____________________________________________________________ Date variables ___
        if ($view_local->campo_per_controllo($field_name)->maschera->tipo == "D") {
            switch (strtolower(func_get_arg(2))) {
                case "d":
                    $value_local = substr($value_local, 6, 2);
                    break;
                case "m":
                    $value_local = substr($value_local, 4, 2);
                    break;
                case "y":
                    $value_local = substr($value_local, 0, 4);
                    break;
               }
           }
        // ____________________________________________________________ Time variables ___
        elseif ($view_local->campo_per_controllo($field_name)->maschera->tipo == "O") {
            switch (strtolower(func_get_arg(2))) {
                case "h":
                    $value_local = substr($value_local, 0, 2);
                    break;
                case "i":
                    $value_local = substr($value_local, 2, 2);
                    break;
                case "s":
                    $value_local = substr($value_local, 4, 2);
                    break;
                }
            }
        else {
            $array_keys = func_get_args();
            array_shift($array_keys);
            array_shift($array_keys);
            $keys = "['".implode("']['", $array_keys)."']";
            eval("\$value_local = \$value_local$keys;");
            }
        }
    return $value_local;

    }


/**
 * Returns previous value of the requested field of a view, as red from database, before
 * any update operation.
 * The view must be defined in the program where o2pre() is used and the field must be a
 * field defined in that view.
 *
 *  ATTENTION: No previous value is defined for program and application (session)
 *             variables.
 *
 * If returned value is an array, indexed subvalues can be retrieved passing indexes as
 * supplementary parameters. For example if you have:
 *  o2pre("view", "field") = array(1 => array("c" => "previous"))
 * you can get "previous" directly by calling:
 *  o2pre("view", "field", 1, "c")
 *
 * @param  string $view_name    Name of the db-view
 * @param  string $field_name   Name of the field
 * @return mix
 * @see    o2val()
 * @see    o2zero()
 * @see    o2par()
 * @see    o2format()
 */
function o2pre($view_name, $field_name) {

    $app  = $_SESSION['o2_app'];
    $view = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if (func_num_args() > 2 &&
        $view->campo_per_controllo($field_name)->maschera->tipo == "S") {
        $array_keys = func_get_args();
        array_shift($array_keys);
        array_shift($array_keys);
        $keys  = "['".implode("']['", $array_keys)."']";
        $value = obj2array($view->precedente($field_name));
        eval("\$value = \$value".$keys.";");
        return $value;
        }
    else {
        return $view->precedente($field_name);
        }

    }


/**
 * Returns boolean value TRUE/FALSE if field value is equal/different from its type
 * default value.
 * This function is the right way to check if a field or variable has a value.
 *
 * When the view is a db-view it must be defined in the program where o2zero() is used and
 * the field must be a field defined in that view.
 *
 * To check a program-variable value pass "prg§_§var" as view name.
 * To check an application (session) variable pass "_o2SESSION" as view name.
 *
 * @param  string $view_name    Name of the db-view or "prg§_§var" or "_o2SESSION"
 * @param  string $field_name   Name of the field or variable
 * @return boolean
 * @see    o2val()
 * @see    o2pre()
 * @see    o2par()
 * @see    o2format()
 */
function o2zero($view_name, $field_name) {

    $app = $_SESSION['o2_app'];
    if ($view_name == '_o2SESSION') {
        if (array_key_exists($field_name, $app->vars)) {
            $field_local = $app->vars[$field_name];
            }
        else {
            throw new o2_exception("Unknown application variable <i>".$field_name."</i>",
                                   o2error_UNKNOWNAPPVAR);
            return true;
            }
        }
    else {
        $field_local = $app->istanze_prg[$app->progressivo_istanze]->
                        contesto[$view_name]->campo_per_controllo($field_name);
        }
    $default_local = $field_local->maschera->default;
    $val_local     = o2val($view_name, $field_name);
    $ret_local     = false;
    switch ($field_local->maschera->tipo) {
        case "S":
            $ret_local = (obj2array($val_local) === obj2array($default_local));
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


/**
 * Returns boolean value TRUE/FALSE if field value is equal/different to NULL.
 * This function is the right way to check if a field has a NULL db value.
 *
 * @param  string $view_name    Name of the db-view
 * @param  string $field_name   Name of the field
 * @return boolean
 * @see    o2val()
 * @see    o2pre()
 * @see    o2par()
 */
function o2isnull($view_name, $field_name) {

    $app = $_SESSION['o2_app'];
    $res = false;
    // __________________________________________________________________________ View ___
    $view = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view instanceof o2_dbview) {
        // _______________________________________________________ NULL value checking ___
        $value = $view->campo($field_name);
        $res   = is_null($view->corrente[strtoupper($field_name)]);
        }
    else {
        throw new o2_exception("Unknown view <i>".$view_name."</i> in program <i>".
                               $app->istanze_prg[$app->progressivo_istanze]->nome."</i>",
                               o2error_UNKNOWNVIEW);
        }
    return $res;

    }


/**
 * Set a field value to NULL on db.
 * Operation is executed directly against the db: no POST-ROW is needed.
 *
 * ATTENTION: field on db must be allowed to be NULL in order to use this function.
 *
 * @param  string $view_name    Name of the db-view
 * @param  string $field_name   Name of the field
 * @return boolean
 * @see    o2val()
 * @see    o2pre()
 * @see    o2par()
 */
function o2nullify($view_name, $field_name) {

    $app = $_SESSION['o2_app'];
    $res = false;
    // __________________________________________________________________________ View ___
    $view = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view instanceof o2_dbview) {
        $field = $view->campi[strtoupper($field_name)];
        $file  = $view->files[$field->file];
        $db    = $file->db;
        $res   = o2_gateway::modifyrec($db->server->type,
                                       $db->server->server,
                                       $db->server->user,
                                       $db->server->password,
                                       $db->nome,
                                       $db->proprietario,
                                       $file->nome,
                                       $file->indice,
                                       array($field->nome_fisico => "NULL"),
                                       $view->chiave_corrente($file));
        }
    else {
        throw new o2_exception("Unknown view <i>".$view_name."</i> in program <i>".
                               $app->istanze_prg[$app->progressivo_istanze]->nome."</i>",
                               o2error_UNKNOWNVIEW);
        }
    return $res;

    }


/**
 * Returns the value of a requested expression.
 *
 * ATTENTION! Expression value is not cached in any way and expression code will be
 *            re-evaluated. For complex and time-consuming expressions consider storing
 *            value in variables to avoid multiple evaluations.
 *
 * @param  integer $exp_n   Expression index
 * @return mix
 * @see    o2val()
 */
function o2exp($exp_n = 0) {

    $app       = $_SESSION['o2_app'];
    $exp_n     = intval($exp_n);
    $func_name = $app->istanze_prg[$app->progressivo_istanze]->nome."_exp_".$exp_n;
    if (is_callable($func_name)) {
        return $func_name();
        }
    else {
        throw new o2_exception("Expression number <i>".$exp_n."</i> not found.",
                               o2error_UNKNOWNEXP);
        return "";
        }

    }


/**
 * Returns value of the requested parameter.
 * Parameter must be defined in the program where o2par() is used.
 *
 * Parameters don't change their value during program execution so values returned by
 * o2par() are consistent along program running.
 *
 * To set the value for a returning parameter (variable passed "by reference") use the
 * Development Interface instruction "Return parameter".
 *
 * @param  integer $parameter_id   Ordinal parameter index
 * @return mix
 * @see    o2val()
 * @see    o2pre()
 * @see    o2zero()
 * @see    o2format()
 */
function o2par($parameter_id) {

    $app  = $_SESSION['o2_app'];
    $args = func_get_args();
    return call_user_func_array(array(&$app->istanze_prg[$app->progressivo_istanze],
                                      'get_par'),
                                $args);

    }


/**
 * Returns passed value as a string formatted according with model.
 * Model must be defined for the application.
 *
 * @param  mix    $value   Value to format
 * @param  string $model   Model name to format value with
 * @return string
 * @see    o2val()
 * @see    o2pre()
 * @see    o2zero()
 * @see    o2par()
 */
function o2format($value, $model) {

    $app = $_SESSION['o2_app'];
    if (!$app->maschere[$model]) {
        throw new o2_exception("Data model <i>".$model."</i> not found.",
                               o2error_UNKNOWNMODEL);
        return "";
        }
    else {
        $dummy_ctrl           = new o2_ctrl();
        $dummy_ctrl->maschera = $app->maschere[$model];
        $dummy_ctrl->valore   = $value;
        return $dummy_ctrl->sql2ctrl();
        }

    }


/**
 * Returns a UNIQUE 16 digits string
 *
 * @return string
 */
function o2uid() {

    return $_SESSION['o2_app']->runtime->microtime();

    }


/**
 * Converts a Janox date and/or time format ('YYYYMMDD' - 'HHMMSS') to a PHP DateTime
 * object
 *
 * @param  string $date_str   String date in Janox format
 * @param  string $time_str   String time in Janox format
 * @return DateTime
 */
function o2_date_obj($date_str = '', $time_str = '000000') {

    $d = ($date_str ? $date_str : '00000000');
    $t = ($time_str ? $time_str : '000000');
    return new DateTime($d{0}.$d{1}.$d{2}.$d{3}.'-'.
                        $d{4}.$d{5}.'-'.
                        $d{6}.$d{7}.' '.
                        $t{0}.$t{1}.':'.
                        $t{2}.$t{3}.':'.
                        $t{4}.$t{5});

    }


/**
 * Returns the difference between two dates/times, passed in Janox format.
 * If last parameter $format is not passed, then difference is returned in number of days.
 * If last parameter $format is passed, then it is used as PHP date/time format string.
 *
 * @return mixed
 */
function o2_date_diff($date1, $time1, $date2, $time2 = '000000', $format = false) {

    $d1 = ($date1 ? $date1 : '00000000');
    $t1 = ($time1 ? $time1 : '000000');
    $o1 = new DateTime($d1{0}.$d1{1}.$d1{2}.$d1{3}.'-'.
                       $d1{4}.$d1{5}.'-'.
                       $d1{6}.$d1{7}.' '.
                       $t1{0}.$t1{1}.':'.
                       $t1{2}.$t1{3}.':'.
                       $t1{4}.$t1{5});
    $d2 = ($date2 ? $date2 : '00000000');
    $t2 = ($time2 ? $time2 : '000000');
    $o2 = new DateTime($d2{0}.$d2{1}.$d2{2}.$d2{3}.'-'.
                       $d2{4}.$d2{5}.'-'.
                       $d2{6}.$d2{7}.' '.
                       $t2{0}.$t2{1}.':'.
                       $t2{2}.$t2{3}.':'.
                       $t2{4}.$t2{5});
    $do = $o1->diff($o2);
    if ($format) {
        return $do->format($format);
        }
    else {
        return $do->days;
        }

    }


/**
 * Takes a Janox date as first parameter, adds to it the passed numbers of $years, $months
 * and $days and returns the result as a Janox date.
 * Janox date format = 'YYYYMMDD'.
 *
 * If start date is an end-of-month and passed $days is 0, then end-of-month is preserved.
 * End-of-month samples:
 *  o2_mkdate('20160229', 0, 1, 0) = '20160331'
 *  o2_mkdate('20160229', 1, 0, 0) = '20170228'
 *
 * @param  string  $date     Start date in Janox format 'YYYYMMDD'
 * @param  integer $years    Years to add
 * @param  integer $months   Months to add
 * @param  integer $days     Days to add
 * @return string
 * @see    o2_mktime()
 */
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


/**
 * Takes an Janox-time as first parameter, adds to it the passed numbers of hours,
 * minutes and seconds and returns the result as a Janox-time.
 * Janox time format = 'HHMMSS'.
 *
 * @param  string  $time
 * @param  integer $hours
 * @param  integer $minutes
 * @param  integer $seconds
 * @return string
 * @see    o2_mkdate()
 */
function o2_mktime($time, $hours = 0, $minutes = 0, $seconds = 0) {

    $ts = @mktime(substr($time, 0, 2) + $hours,
                  substr($time, 2, 2) + $minutes,
                  substr($time, 4, 2) + $seconds,
                  1,
                  1,
                  1);
    return ($ts ? date("His", $ts) : "000000");

    }


/**
 * Executes a Janox program and returns 1st parameter. Can accept infinite parameters.
 *
 * @param  string $prg_name
 * @return mix
 * @see    o2_prg4http()
 */
function o2_callprg($prg_name) {

    $params = func_get_args();
    return call_user_func_array(array($_SESSION['o2_app'], "intcall"), $params);

    }


/**
 * Returns the url to request for calling a program and passing parameters.
 * This function can accept infinite parameters: parameters after the first will be
 * passed to the called program.
 *
 * @param  string $prg_name
 * @return string
 * @see    o2_callprg(), o2_act4js()
 */
function o2_prg4http($prg_name = "") {

    $app        = $_SESSION['o2_app'];
    $sess_name  = $app->session_name;
    $parameters = func_get_args();
    array_shift($parameters);
    $parameters_str = "";
    $counter_local  = 0;
    foreach ($parameters as $single_parameter) {
        $counter_local++;
        $parameters_str.= "&extp_".$counter_local."=".
                           htmlentities(urlencode($single_parameter),
                                        ENT_COMPAT | ENT_HTML5,
                                        $app->chr_encoding);
        }
    return $app->referer."?o2c=".$prg_name."&JXSESSNAME=".$sess_name.$parameters_str;

    }


/**
 * Returns the javascript for executing an action into a running program
 * ($prg_id = 0 for actually running program).
 * This function can accept infinite parameters: parameters after the second will be
 * passed in the call. See function o2_reqpar() to retrieve passed parameters.
 *
 * @param  integer $prg_id
 * @param  string  $action_name
 * @return string
 * @see    o2_reqpar(), o2_prg4http()
 */
function o2_act4js($prg_id, $action_name) {

    $parameters = func_get_args();
    array_shift($parameters);
    array_shift($parameters);
    $prg_id         = ($prg_id ? $prg_id : $_SESSION['o2_app']->progressivo_istanze);
    $parameters_str = "";
    $counter_local  = 0;
    foreach ($parameters as $single_parameter) {
        $counter_local++;
        $parameters_str.= ",'".htmlentities(urlencode($single_parameter),
                                            ENT_COMPAT | ENT_HTML5,
                                            $_SESSION['o2_app']->chr_encoding)."'";
        }
    return "o2jse.cmd.exe(event,".$prg_id.",'".$action_name."'".$parameters_str.");";

    }


/**
 * Recover the value of a parameter passed by o2_act4js() method by index order.
 *
 * @param  integer $par_id
 * @return string
 * @see    o2_act4js()
 */
function o2_reqpar($par_id) {

    if (isset($_REQUEST['extp_'.$par_id])) {
        return urldecode($_REQUEST['extp_'.$par_id]);
        }
    else {
        return "";
        }

    }


/**
 * In HTML disposes requested file for download and returns the url leading to it.
 *
 * Second parameter $mode is effective in HTML context only. It can take one of three
 * values:
 *  = 1 File is forced to download by headers;
 *  = 2 File is always disposed by Transitional File System (TRN)
 *  = 0 (or any other value) file is delivered as a Stream Resource or TRN depending on
 *      application .INI parameter "trn_deliver".
 *
 * @param  string  $local_file   Local path to file to deliver
 * @param  integer $mode         1=Force download; 2=Force TRN; else default
 * @return string
 */
function o2_path($local_file, $mode = 0) {

    $app      = $_SESSION['o2_app'];
    $ret_path = "";
    switch ($app->runtime->interface) {
        case "HTML":
            $ret_path = $app->file2http($local_file, $mode);
            break;
        case "BAT":
        default:
            $ret_path = "#@JXPATH{".$local_file."}";
            break;
        }
    return $ret_path;

    }


/**
 *  Returns passed string translated to the current application character encoding.
 *  Translation is applied only when needed, according with string and application
 *  encoding.
 *  This function is mostly useful in HTML context, when you need to out a text and you
 *  don't know its actual encoding (Windows-1252 from program or UTF-8 from DB).
 *
 * @param  string $string   String to be encoded
 * @return string           Encoded string
 */
function o2_encode($string) {

    return jx_encode($string);

    }


/**
 * Dispose a file for download by URL.
 * File is sent to client without a user's click on link.
 *
 * Correct use to send a local file by path:
 *  o2_send(o2_path($file, 1))
 *
 * @param  string $file_url   URL to file to send
 */
function o2_send($file_url) {

    o2html::send_file($file_url);

    }


/**
 *  Return a passed text as plain text, as response for client request.
 *  This method is specifically useful for returning code to client runtime (Javascript,
 *  JSON, custom libraries, ...).
 *
 * @param string $text   Text (or code) to be sent to client
 */
function o2out($text) {

    ob_clean();
    print $text;
    $_SESSION['o2_app']->custom_response = ob_get_length();
    return true;

    }


/**
 * Sends a mail. Returns TRUE on success.
 *
 * TO, CC, BCC and Attachments, if multiple, can be provided in two ways:
 *  1. as array of strings in the form:
 *      array(1 => "first_item",
 *            2 => "second_item",
 *            ...
 *            n => "nth_item");
 *     keys are ignored
 *  2. as "," or ";" separated string in the form:
 *      "first_item,second_item,...,nth_item"
 *
 * @param  string  $from      FROM address
 * @param  string  $to        TO address(es)
 * @param  string  $cc        Carbon Copy address(es)
 * @param  string  $bcc       Blind Carbon Copy address(es)
 * @param  string  $subject   Mail subject
 * @param  string  $text      Mail text
 * @param  boolean $as_html   If mail text is HTML formatted
 * @param  string  $attach    File(s) to attach to mail
 * @return boolean
 */
function o2mail_send($from,
                     $to,
                     $cc      = "",
                     $bcc     = "",
                     $subject = "No subject",
                     $text    = "",
                     $as_html = false,
                     $attach  = false) {

    if ($from) {
        $mail_local = new o2_mail($from, $_SESSION['o2_app']->chr_encoding);
        if ($to) {
            $mail_local->to($to);
            }
        else {
            return false;
            }
        if ($cc) {
            $mail_local->cc($cc);
            }
        if ($bcc) {
            $mail_local->bcc($bcc);
            }
        if ($subject) {
            $mail_local->subject($subject);
            }
        if ($as_html) {
            $mail_local->html($text);
            }
        else {
            $mail_local->text($text);
            }
        if ($attach) {
            $mail_local->attachment($attach);
            }
        return $mail_local->send();
        }
    else {
        return false;
        }

    }


/**
 * Returns translation for the passed text
 *
 * @param  string $text   Text to be translated
 * @return string
 */
function o2_translate($text) {

    $app = $_SESSION['o2_app'];
    if ($app->multilang) {
        if ($app->harvest_labels) {
            $app->harvest_list[] = $text;
            }
        if (trim($app->translation[$text])) {
            return $app->translation[$text];
            }
        else {
            return $text;
            }
        }
    else {
        return $text;
        }

    }


/**
 * Sets HTML headers to allow browsers to recover page in production
 * from cache. Returns TRUE if set possible, FALSE if headers
 * have already been sent.
 *
 * @param  boolean $allow
 * @return boolean
 */
function o2_allow_history($allow = true) {

    if (headers_sent()) {
        return false;
        }
    elseif ($allow) {
        session_cache_limiter("nocache");
        print "<script> document.o2form.o2pagemark.value=''; </script>";
        return true;
        }
    else {
        session_cache_limiter("must_revalidate");
        return true;
        }

    }


/**
 * Plays an audio file.
 * The audio file parameter is the URL to the file to play.
 *
 * Correct use to play a local file by path:
 *  o2_play(o2_path($file, 1))
 *
 * If parameter $sound_file is not passed or it is passed as blank (""), then default
 * runtime "beep" sound file will be played.
 *
 * @param  string $sound_file   URL to file audio to play
 */
function o2_play($sound_file = false) {

    if (!$sound_file) {
        $sound_file = $GLOBALS['o2_runtime']->alias."beep.wav";
        }
    $mime = o2_mime_content($sound_file);
    $code = "o2jse.cmd.playSound('".$sound_file."', '".$mime."');";
    if ($GLOBALS['jxjs']) {
        $_SESSION['o2_app']->js_cache.= $code."\n";
        }
    else {
        print "<script> ".$code." </script>\n";
        }

    }


//= ==================================== CONTROL =========================================

/**
 * Sets focus to the specified control of form.
 * $select_mode parameter can take 3 values:
 *  - null (or not passed) select all value
 *  - 0 (zero) cursor at value start position
 *  - 1 (or any other value) cursor at value end position
 *
 * @package control
 * @param   string  $form_name
 * @param   string  $ctrl_name
 * @param   integer $select_mode
 * @return  boolean
 */
function o2ctrl_focus($form_name, $ctrl_name, $select_mode = null) {

    $app       = $_SESSION['o2_app'];
    $form_name = $form_name."_form"."_".$app->progressivo_istanze."_html_form";
    $app->istanze_prg[$app->progressivo_istanze]->set_caret($form_name,
                                                            $ctrl_name,
                                                            $select_mode,
                                                            true);
    return true;

    }


/**
 * Activate the specified control of form.
 * Control activation depends on control type:
 *
 *  - combo-lookup:       focus and open items list;
 *  - all other controls: just focus
 *
 * @package control
 * @param   string  $form_name
 * @param   string  $ctrl_name
 * @return  boolean
 */
function o2ctrl_activate($form_name, $ctrl_name) {

    o2ctrl_focus($form_name, $ctrl_name);
    $app  = $_SESSION['o2_app'];
    $prg  = $app->istanze_prg[$app->progressivo_istanze];
    $form = $prg->form[$form_name."_form"];
    if ($form->controlli[$ctrl_name]) {
        $ctrl = $form->controlli[$ctrl_name];
        switch ($ctrl->tipo_ctrl) {
            case "listcombo":
                if ($ctrl->righe === 1) {
                    o2js_exe("o2jse.lu.list(o2jse.infoForm['".$ctrl_name.$prg->id.
                             "_desc'], true, true);");
                    }
                break;
            case "file":
                o2js_exe("o2jse.infoForm['".$ctrl_name.$prg->id."'].click();");
                break;
            }
        }
    else {
        throw new o2_exception("Unknown control <i>".$ctrl_name."</i> in program <i>".
                               $prg->nome."</i>", o2error_UNKNOWNCTRL);
        }
    return true;

    }


/**
 * Set horizontal position to $x for control and preserve it untill next call to function.
 * If $x is avaluated to 0 custom horizontal positioning is disabled for control.
 *
 * @package control
 * @param   string  $form_name
 * @param   string  $ctrl_name
 * @param   integer $x
 * @return  boolean
 */
function o2ctrl_set_x($form_name, $ctrl_name, $x = 0) {

    $app  = $_SESSION['o2_app'];
    $prg  = $app->istanze_prg[$app->progressivo_istanze];
    $form = $prg->form[$form_name."_form"];
    if ($form && $form->controlli[$ctrl_name]) {
        $ctrl = $form->controlli[$ctrl_name];
        if ($x) {
            $ctrl->x        = $x;
            $ctrl->custom_x = true;
            }
        else {
            $ctrl->custom_x = false;
            }
        }
    else {
        if (!$prg->custom_ctrls[$form->nome]) {
            $prg->custom_ctrls[$form->nome] = array();
            }
        if (!$prg->custom_ctrls[$form->nome][$ctrl_name]) {
            $prg->custom_ctrls[$form->nome][$ctrl_name] = array();
            }
        $prg->custom_ctrls[$form->nome][$ctrl_name]["X"] = $x;
        }

    }


/**
 * Set vertical position to $y for control and preserve it untill next call to function.
 * If $y is avaluated to 0 custom vertical positioning is disabled for control.
 *
 * @package control
 * @param   string  $form_name
 * @param   string  $ctrl_name
 * @param   integer $y
 * @return  boolean
 */
function o2ctrl_set_y($form_name, $ctrl_name, $y = 0) {

    $app  = $_SESSION['o2_app'];
    $prg  = $app->istanze_prg[$app->progressivo_istanze];
    $form = $prg->form[$form_name."_form"];
    if ($form && $form->controlli[$ctrl_name]) {
        $ctrl = $form->controlli[$ctrl_name];
        if ($y) {
            $ctrl->y        = $y;
            $ctrl->custom_y = true;
            }
        else {
            $ctrl->custom_y = false;
            }
        }
    else {
        if (!$prg->custom_ctrls[$form->nome]) {
            $prg->custom_ctrls[$form->nome] = array();
            }
        if (!$prg->custom_ctrls[$form->nome][$ctrl_name]) {
            $prg->custom_ctrls[$form->nome][$ctrl_name] = array();
            }
        $prg->custom_ctrls[$form->nome][$ctrl_name]["Y"] = $y;
        }

    }


/**
 * Set control width to $width and preserve it untill next call to function.
 * If $width is avaluated to 0 custom width sizing is disabled for control.
 *
 * @package control
 * @param   string  $form_name
 * @param   string  $ctrl_name
 * @param   integer $width
 * @return  boolean
 */
function o2ctrl_set_width($form_name, $ctrl_name, $width = 0) {

    $app  = $_SESSION['o2_app'];
    $prg  = $app->istanze_prg[$app->progressivo_istanze];
    $form = $prg->form[$form_name."_form"];
    if ($form && $form->controlli[$ctrl_name]) {
        $ctrl = $form->controlli[$ctrl_name];
        if ($width) {
            $ctrl->larghezza    = $width;
            $ctrl->custom_width = true;
            }
        else {
            $ctrl->custom_width = false;
            }
        }
    else {
        if (!$prg->custom_ctrls[$form->nome]) {
            $prg->custom_ctrls[$form->nome] = array();
            }
        if (!$prg->custom_ctrls[$form->nome][$ctrl_name]) {
            $prg->custom_ctrls[$form->nome][$ctrl_name] = array();
            }
        $prg->custom_ctrls[$form->nome][$ctrl_name]["W"] = $width;
        }

    }


/**
 * Set control height to $height and preserve it untill next call to function.
 * If $height is avaluated to 0 custom height sizing is disabled for control.
 *
 * @package control
 * @param   string  $form_name
 * @param   string  $ctrl_name
 * @param   integer $height
 * @return  boolean
 */
function o2ctrl_set_height($form_name, $ctrl_name, $height = 0) {

    $app  = $_SESSION['o2_app'];
    $prg  = $app->istanze_prg[$app->progressivo_istanze];
    $form = $prg->form[$form_name."_form"];
    if ($form && $form->controlli[$ctrl_name]) {
        $ctrl = $form->controlli[$ctrl_name];
        if ($height) {
            $ctrl->altezza       = $height;
            $ctrl->custom_height = true;
            }
        else {
            $ctrl->custom_height = false;
            }
        }
    else {
        if (!$prg->custom_ctrls[$form->nome]) {
            $prg->custom_ctrls[$form->nome] = array();
            }
        if (!$prg->custom_ctrls[$form->nome][$ctrl_name]) {
            $prg->custom_ctrls[$form->nome][$ctrl_name] = array();
            }
        $prg->custom_ctrls[$form->nome][$ctrl_name]["H"] = $height;
        }

    }


/**
 * Returns an array of events happpened on field related to control.
 * Each record is in the form:
 *  "id"    => event timestamp with microseconds,
 *  "date"  => event date,
 *  "time"  => event time,
 *  "act"   => event action [I|D|C|U],
 *  "user"  => user who fired the event,
 *  "value" => field value for event.
 *
 * Insert records contains field value the record was created with.
 *
 * For each Update record there is a Change record with the same ID: the Change record
 * contains the Value before modification, the Update record contains the Value after
 * modification.
 *
 * Delete records contains the last field Value before deleting.
 * Delete records matching Update records are changed to "C" for easy filtering.
 *
 * @package control
 * @param   string  $ctrl_name   Name of the control to get history for
 * @param   integer $exe_id      Program execution ID: use 0 or nothing for current
 *                               program
 * @see     o2record_log()
 */
function o2ctrl_log($ctrl_name, $exe_id = 0) {

    $app = $_SESSION['o2_app'];
    if (!$exe_id) {
        $exe_id = $app->progressivo_istanze;
        }
    $prg = $app->istanze_prg[$exe_id];
    return $prg->log_history($ctrl_name);

    }


/**
 * Returns an array of events happpened on a record of a view.
 * Each record is in the form:
 *  "id"    => event timestamp with microseconds,
 *  "date"  => event date,
 *  "time"  => event time,
 *  "act"   => event action [I|D|C|U],
 *  "user"  => user who fired the event,
 *  "<...>" => record fields with values.
 *
 * Insert records contains fields values the record was created with.
 *
 * For each Update record there is a Change record with the same ID: the Change record
 * contains the Values before modification, the Update record contains the Values after
 * modification.
 *
 * Delete records contains the last fields Values before deleting.
 * Delete records matching Update records are changed to "C" for easy filtering.
 *
 * @package view
 * @param   string  $view_name     Name of the view to get history for
 * @param   string  $table_alias   Alias of the view table to get history for
 * @param   integer $exe_id        Program execution ID: use 0 or nothing for current
 *                                 program
 * @see     o2ctrl_log()
 */
function o2record_log($view_name, $table_alias, $exe_id = 0) {

    $app = $_SESSION['o2_app'];
    if (!$exe_id) {
        $exe_id = $app->progressivo_istanze;
        }
    $prg = $app->istanze_prg[$exe_id];
    return $prg->log_record($view_name, $table_alias);

    }


// ==================================== o2 RUNTIME =======================================

/**
 * Returns current o2runtime alias
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_root(), o2rnt_interface()
 */
function o2rnt_alias() {

    return $GLOBALS['o2_runtime']->alias;

    }


/**
 * Returns current o2runtime root path
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_alias(), o2rnt_interface()
 */
function o2rnt_root() {

    return $GLOBALS['o2_runtime']->root->nome_completo;

    }


/**
 * Returns current o2runtime running threads folder path
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_dircss()
 */
function o2rnt_dirthreads() {

    return $GLOBALS['o2_runtime']->dir_threads->nome_completo;

    }


/**
 * Returns current o2runtime css folder path
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_dirthreads()
 */
function o2rnt_dircss() {

    return $GLOBALS['o2_runtime']->dir_css->nome_completo;

    }


/**
 * Returns current o2runtime interface type.
 *
 * Possible returned values are:
 * HTML: application is executed by a webserver and interfaces are delivered to a client
 *       browser in HTML format
 * BAT:  application is executed by system (command line or double click), so console
 *       STDOUT is the only interface
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_alias()
 * @see     o2rnt_root()
 */
function o2rnt_interface() {

    return $GLOBALS['o2_runtime']->interface;

    }


// ===================================== SESSION =========================================

/**
 * Returns TRUE if session is a job-worker (batch session used to execute a job), else
 * FALSE
 *
 * @package session
 * @return  boolean
 * @see     o2session_is_scheduler()
 */
function o2session_is_job() {

    return (isset($_REQUEST[o2_run_job::$run_word]) ||
            isset($_REQUEST[o2_run_job::$job_word]));

    }


/**
 * Returns TRUE if session is a scheduler (batch session used to run scheduled jobs), else
 * FALSE
 *
 * @package session
 * @return  boolean
 * @see     o2session_is_job()
 */
function o2session_is_scheduler() {

    return (isset($_REQUEST[o2_scheduler::$sched_word]));

    }


/**
 * When function is called without parameter (or passing TRUE) application is locked and
 * current session is turned into an exclusive-administration session.
 * All other sessions are destroyed and new logins are not allowed from standard users.
 *
 * When function is called with parameter FALSE application is unlocked and new logins are
 * allowed again.
 *
 * Function returns boolean new application lock status.
 *
 * ATTENTION: This function is intended to be an administrative tool and only "root" and
 *            developers can call this function!
 *
 * @package   session
 * @parameter $on
 * @return    boolean
 * @see       o2session_is_job()
 */
function o2session_lock($on = true) {

    $app = $_SESSION['o2_app'];
    $ret = $app->lock;
    if (($app->user == 'root' || $app->run_level == 'DEV') && $app->lock != $on) {
        $res = $app->intcall('tools/jxlock', true);
        $ret = $res[1];
        }
    return $ret;

    }


// ======================================= JOBS ==========================================

/**
 * Sets completion percentage for job running in session
 *
 * @package jobs
 * @param   integer $perc   Completion percentage
 * @see     o2job_add_file()
 */
function o2job_set_progress($perc) {

    if ($job = o2_run_job::get_by_session()) {
        $job->set_progress(min(99, max(1, intval($perc))));
        }

    }


/**
 * Adds a produced file to job running in session.
 * File path is intended to be any absolute internal path.
 *
 * @package jobs
 * @param   string $file_path   Path to file to add to job
 * @see     o2job_set_progress()
 */
function o2job_add_file($file_path) {

    if ($job = o2_run_job::get_by_session()) {
        $job->add_file($file_path);
        }

    }


/**
 * Creates a new job, assigns a new ID and stores it in system data.
 * New assigned ID is returned.
 *
 * @package jobs
 * @param   string  $name            Job name
 * @param   string  $prg             Program name caled by job
 * @param   array   $params          List of program parameters, referenced by name
 * @param   string  $desc            Job description
 * @param   integer $keep_days       Days job instances are preserved from deleting
 * @param   integer $max_instances   Max number of instances can run at a time
 * @param   string  $block_mode      Run blocking mode: [B]lock, [Q]ueue
 * @param   boolean $disabled        Disabled job will not run
 * @return  integer                  New job assigned ID
 * @see     o2job_get_by_name()
 */
function o2job_create($name,
                      $prg,
                      $params        = false,
                      $desc          = "",
                      $keep_days     = 0,
                      $max_instances = false,
                      $block_mode    = "B",
                      $disabled      = false) {

    if (!$params || !is_array($params)) {
        $params = array();
        }
    $job = new o2_job(0,
                      $name,
                      $prg,
                      $params,
                      $desc,
                      $keep_days,
                      $max_instances,
                      $block_mode,
                      $disabled);
    return $job->create();

    }


/**
 * Returns ID of job with passed name, if any, else FALSE
 *
 * @package jobs
 * @param   string $job_name   Name to look for
 * @return  integer
 * @see     o2job_create()
 * @see     o2session_is_job()
 */
function o2job_get_by_name($job_name) {

    if ($job = o2_job::get_by_name($job_name)) {
        return $job->id;
        }
    else {
        return false;
        }

    }


/**
 * Returns TRUE if job is enabled, FALSE if it is disabled
 *
 * @package jobs
 * @param   string $job_name   Name to look for
 * @return  boolean
 * @see     o2job_enable()
 * @see     o2job_get_by_name()
 * @see     o2job_run()
 */
function o2job_is_enabled($job_name) {

    if ($job = o2_job::get_by_name($job_name)) {
        return !$job->disabled;
        }
    else {
        return false;
        }

    }


/**
 * Returns HTML code to show a progress bar for the requested running job instance
 *
 * @package jobs
 * @param   integer $run_job_id        Running instance ID
 * @param   string  $complete_action   Program action to be executed on bar completion
 * @param   string  $start_action      Program action to be executed on bar start
 * @return  string
 * @see     o2job_is_running()
 * @see     o2job_run()
 * @see     o2job_set_progress()
 * @see     o2progress_bar()
 */
function o2job_get_bar($run_job_id, $complete_action = false, $start_action = false) {

    if ($run_job = o2_run_job::get_by_id($run_job_id)) {
        return $run_job->get_bar($complete_action, $start_action);
        }
    else {
        return false;
        }

    }


/**
 * Sets job enabled/disabled, depending on passed parameter.
 * Old value is returned.
 *
 * @package jobs
 * @param   string $job_name   Job name to look for
 * @param   string $on         TRUE to enable, FALSE to disable
 * @return  boolean
 * @see     o2job_is_enabled()
 * @see     o2job_get_by_name()
 * @see     o2job_run()
 */
function o2job_enable($job_name, $on = true) {

    if ($job = o2_job::get_by_name($job_name)) {
        $ret           = !$job->disabled;
        $job->disabled = !$on;
        $job->save();
        return $ret;
        }
    else {
        return false;
        }

    }


/**
 * Queue for execution job with passed name.
 * Job is queued for execution but a dedicated batch process is NOT started: some other
 * logic (scheduler) will take care of executing it later.
 * A list of parameters (by name) can be provided: list will override parameters passed to
 * program called by job.
 * If job exists then new job execution ID is returned, else FALSE.
 *
 * @package jobs
 * @param   string $job_name   Job name to look for
 * @param   array  $params     List of overriding parameters passed to program
 * @return  integer
 * @see     o2job_create()
 * @see     o2job_run()
 * @see     o2job_get_by_name()
 * @see     o2job_is_running()
 */
function o2job_queue($job_name, $params = array()) {

    if ($job = o2_job::get_by_name($job_name)) {
        $aspid   = $_SESSION['o2_app']->vars['_area']->valore;
        $run_job = o2_run_job::queue($job, $params, $aspid);
        return $run_job->id;
        }
    else {
        return false;
        }

    }


/**
 * Runs job with passed name.
 * Job is queued for execution and a dedicated batch process is started.
 * A list of parameters (by name) can be provided: list will override parameters passed to
 * program called by job.
 * If job exists then new job execution ID is returned, else FALSE.
 *
 * @package jobs
 * @param   string $job_name   Job name to look for
 * @param   array  $params     List of overriding parameters passed to program
 * @return  integer
 * @see     o2job_create()
 * @see     o2job_queue()
 * @see     o2job_get_by_name()
 * @see     o2job_is_running()
 */
function o2job_run($job_name, $params = array()) {

    if ($job = o2_job::get_by_name($job_name)) {
        $aspid = $_SESSION['o2_app']->vars['_area']->valore;
        return $job->run($aspid, $params);
        }
    else {
        return false;
        }

    }


/**
 * Returns running-ID if a job with passed name is running, else FALSE
 *
 * @package jobs
 * @param   string $job_name   Name to look for
 * @return  integer
 * @see     o2job_create()
 * @see     o2job_get_by_name()
 * @see     o2job_is_scheduled()
 */
function o2job_is_running($job_name) {

    if ($job = o2_job::get_by_name($job_name)) {
        $runs = o2_run_job::get_by_job($job->id);
        if (is_array($runs) && count($runs)) {
            foreach ($runs as $run_id => $run) {
                if ($run->is_running()) {
                    return $run_id;
                    }
                }
            }
        return false;
        }
    else {
        return false;
        }

    }


/**
 * Returns the run-job process status.
 * The run-job ID ($runjob_id) is the process unique ID, as returned by o2job_run(),
 * o2job_queue() e o2job_is_running() functions.
 * Status is one of values:
 *  - Queued
 *  - Running
 *  - Complited
 *  - Error
 *
 * @package jobs
 * @param   integer $runjob_id   Run-job (process) ID to look for
 * @return  string
 * @see     o2job_run()
 * @see     o2job_queue()
 * @see     o2job_is_running()
 */
function o2job_proc_get_status($runjob_id) {

    $runjob = o2_run_job::get_by_id($runjob_id);
    if ($runjob) {
        return $runjob->get_status();
        }
    else {
        throw new o2_exception("Can't find run-job process with ID <i>".$runjob_id."</i>",
                               o2error_EXECUTION);
        return false;
        }

    }


/**
 * Kills the run-job process.
 * The run-job ID ($runjob_id) is the process unique ID, as returned by o2job_run(),
 * o2job_queue() e o2job_is_running() functions.
 * Processes running on this host are killed, ones running on other machines are marked
 * for killing and they will be physically killed by owner registered service.
 *
 * @package jobs
 * @param   integer $runjob_id   Run-job (process) ID to look for
 * @return  string
 * @see     o2job_run()
 * @see     o2job_queue()
 * @see     o2job_proc_get_status()
 */
function o2job_proc_kill($runjob_id) {

    $runjob = o2_run_job::get_by_id($runjob_id);
    if ($runjob) {
        if ($runjob->is_running()) {
            $runjob->kill();
            return true;
            }
        else {
            return false;
            }
        }
    else {
        throw new o2_exception("Can't find run-job process with ID <i>".$runjob_id."</i>",
                               o2error_EXECUTION);
        return false;
        }

    }


/**
 * Starts over an existing process.
 * If parameter $preserve_id is passed TRUE the same process is set to queued and reused,
 * else, if FALSE, a new process is created with the same parameters and APSID of the
 * original one.
 *
 * @package jobs
 * @param   integer $runjob_id     Run-job (process) ID to look for
 * @param   boolean $preserve_id   If the same process is to be reused
 * @return  string
 * @see     o2job_run()
 * @see     o2job_queue()
 * @see     o2job_proc_get_status()
 */
function o2job_proc_restart($runjob_id, $preserve_id = false) {

    $runjob = o2_run_job::get_by_id($runjob_id);
    if ($runjob) {
        $app = $_SESSION['o2_app'];
        if ($preserve_id) {
            $tab                 = $app->get_table(o2_run_job::$run_table);
            $db                  = $tab->db;
            $server              = $db->server;
            $fields              = array($tab->campi['run_host']->nome_fisico,
                                         $tab->campi['run_start_date']->nome_fisico,
                                         $tab->campi['run_start_time']->nome_fisico,
                                         $tab->campi['run_end_date']->nome_fisico,
                                         $tab->campi['run_end_time']->nome_fisico,
                                         $tab->campi['run_status']->nome_fisico,
                                         $tab->campi['run_pid']->nome_fisico,
                                         $tab->campi['run_error']->nome_fisico,
                                         $tab->campi['run_files']->nome_fisico,
                                         $tab->campi['run_at_date']->nome_fisico,
                                         $tab->campi['run_at_time']->nome_fisico);
            $now                 = time();
            $values              = array("'".$run_job->host."'",
                                         "'".date('Ymd', $now)."'",
                                         "'".date('His', $now)."'",
                                         "'00000000'",
                                         "'000000'",
                                         "'".$run_job->status."'",
                                         $run_job->pid,
                                         "''",
                                         "''",
                                         "'00000000'",
                                         "'000000'");
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
            return $runjob->id;
            }
        else {
            $newjob = o2_run_job::queue(o2_job::get_by_id($runjob->job),
                                        $runjob->params,
                                        $runjob->aspid,
                                        $runjob->service);
            return $newjob->id;
            }
        }
    else {
        throw new o2_exception("Can't find run-job process with ID <i>".$runjob_id."</i>",
                               o2error_EXECUTION);
        return false;
        }

    }


/**
 * Schedule job with passed name.
 *
 * A scheduled job is a task.
 * A task can be of one of 3 types (char(1) passed as $type):
 *  "O": Once         Task is run once in day $date at time $time
 *  "I": Interval     Task is run once every $interval minutes
 *  "R": Recurrence   Task is run with a defined recurrence (see below)
 *
 * In case of recurrence, $recurrence parameter can take value:
 *  "D": Day     Task in run once every day at $time
 *  "W": Week    Task in run once every week at $week_day (see below)
 *  "M": Month   Task in run once every month, day $month_day (see below)
 *
 * Parameters $date and $time are passed in Janox format ("yyyymmdd", "hhmmss").
 *
 * Parameter $week_day (char(3)) can take value:
 *  "Mon|Tue|Wed|Thu|Fri|Sat|Sun":   Day of week
 *
 * Parameter $month_day (integer) can take value:
 *  1-31:   Day of month
 *
 * If task is correctly created new tasl-ID is returned, else FALSE.
 *
 * @package jobs
 * @param   string  $job_name     Name to look for
 * @param   string  $type         Task type: [O]nce, [I]nterval, [R]ecurrence
 * @param   integer $interval     For type [I]nterval: number of minutes
 * @param   string  $date         For type [O]nce & [R]ecurrence: Janox format date
 * @param   string  $time         For type [O]nce & [R]ecurrence: Janox format time
 * @param   string  $recurrence   Recurrence type: [D]ay, [W]eek, [M]onth
 * @param   string  $week_day     Day of week [Mon|Tue|Wed|Thu|Fri|Sat|Sun]
 * @param   integer $month_day    Day of month [0-31]
 * @param   string  $aspid        ASPI-ID to run job for
 * @return  integer
 * @see     o2job_create()
 * @see     o2job_get_by_name()
 * @see     o2job_run()
 * @see     o2job_is_running()
 */
function o2job_schedule($job_name,
                        $type,
                        $interval   = 0,
                        $date       = "00000000",
                        $time       = "000000",
                        $recurrence = "D",
                        $week_day   = "",
                        $month_day  = 0,
                        $aspid      = "") {

    if ($job = o2_job::get_by_name($job_name)) {
        $app  = $_SESSION["o2_app"];
        $task = new o2_task(0,
                            $job->id,
                            $app->user,
                            $type,
                            $interval,
                            $date,
                            $time,
                            $recurrence,
                            $week_day,
                            $month_day,
                            $aspid);
        return $task->create();
        }
    else {
        return false;
        }

    }


/**
 * Returns TRUE if job with passed name is scheduled, else FALSE.
 * NOTE: if session ASPID is set then only scheduled items for current ASPID are
 *       considered (ASP blank, 'NOASP' or equal to current ASPID).
 *
 * @package jobs
 * @param   string $job_name   Name to look for
 * @return  boolean
 * @see     o2job_create()
 * @see     o2job_get_by_name()
 * @see     o2job_run()
 * @see     o2job_is_running()
 */
function o2job_is_scheduled($job_name) {

    if ($job = o2_job::get_by_name($job_name)) {
        $sched = o2_scheduler::get_by_job($job->id);
        return (is_array($sched) && count($sched));
        }
    else {
        return false;
        }

    }


// ======================================= CRON ==========================================

/**
 * Return TRUE if application scheduler is registered on system CRON
 *
 * @package cron
 * @return  boolean
 * @see     o2cron_register()
 */
function o2cron_is_registered() {

    return o2_cron::is_registered();

    }


/**
 * Register/unregister application scheduler on system CRON.
 * Old value is returned.
 *
 * @package cron
 * @return  boolean
 * @see     o2cron_is_registered()
 */
function o2cron_register($on = true) {

    $old = o2_cron::is_registered();
    if ($old !== $on) {
        o2_cron::register($on);
        }
    return $old;

    }


// ==================================== DEVELOPER ========================================

/**
 * Logs out a dump of a passed object on the development console, with an execution stack
 * trace.
 *
 * @package debug
 * @param   mix     $logobj     Object to dump out, can by any type (Str, Obj, Array, ...)
 * @param   boolean $log2file   If TRUE log is put to file too (app/logs/<developer>.log)
 * @see     o2_timestart()
 * @see     o2_timeend()
 */
function o2log($logobj = null, $log2file = false) {

    $app = $_SESSION['o2_app'];
    $log = $app->log2file;
    if ($log2file) {
        $app->log2file = true;
        }
    jxlog($logobj);
    $app->log2file = $log;

    }


/**
 * Starts and stops SQL-tracing in developer console.
 * This function can be used either for starting tracing or for ending it:
 *  - with parameter TRUE (or without parameter): starts tracing;
 *  - with parameter FALSE: stops tracing.
 *
 * @package debug
 * @param   boolean $start   If TRUE (default) tracing is started else stopped (if active)
 * @return  boolean          Old tracing status
 * @see     o2log()
 * @see     o2_timestart()
 * @see     o2_timeend()
 */
function o2sqltrace($start = true) {

    $app           = $_SESSION['o2_app'];
    $old_status    = ($app->sqltrace ? true : false);
    $app->sqltrace = ($start ? true : false);
    return $old_status;

    }


// ======================================= USER ==========================================
/**
 * Returns logged user roles list as an array
 *
 * @package user
 * @return  array
 * @see     o2user_rights(), o2user_is()
 */
function o2user_roles() {

    return $_SESSION['o2_app']->user_roles;

    }


/**
 * Returns logged user rights list as an array
 *
 * @package user
 * @return  array
 * @see     o2user_roles(), o2user_has()
 */
function o2user_rights() {

    return $_SESSION['o2_app']->user_rights;

    }


/**
 * Returns TRUE if logged user has got requested right
 *
 * @package user
 * @param   string $right_code
 * @return  boolean
 * @see     o2user_rights(), o2user_is()
 */
function o2user_has($right_code) {

    if ($right_code) {
        return $_SESSION['o2_app']->has_right($right_code);
        }
    else {
        return true;
        }

    }


/**
 * Returns TRUE if logged user is in the requested role
 *
 * @package user
 * @param   string $role_name
 * @return  boolean
 * @see     o2user_roles(), o2user_has()
 */
function o2user_is($role_name) {

    if ($role_name) {
        return $_SESSION['o2_app']->is_user($role_name);
        }
    else {
        return true;
        }

    }


/**
 * Deletes a registered user and all related context (roles, rights, options, ...)
 *
 * @package user
 * @param   string $user   User to be deleted
 * @return  boolean
 * @see     o2user_is(), o2user_has()
 */
function o2user_delete($user) {

    return $_SESSION['o2_app']->delete_user($user);

    }


// ===================================== JAVASCRIPT =======================================
/**
 * Executes arbitrary JavaScript code on client browser
 *
 * NOTE: Code is JavaScript code, so it must be passed without -SCRIPT- tags.
 *
 * @package javascript
 * @param   string $js_code   JavaScript code to be executed
 * @return  boolean
 * @see     o2_act4js(), o2js_doc_ready(), o2js_load_script()
 */
function o2js_exe($js_code) {

    if ($GLOBALS['jxjs']) {
        $_SESSION['o2_app']->js_cache.= $js_code."\n";
        }
    else {
        print "<script> ".$js_code." </script>\n";
        }

    }


/**
 * Executes arbitrary JavaScript after all HTML and scripts have finished loading.
 * This method is useful to execute code normally called on onLoad events.
 *
 * @package javascript
 * @param   string $js_code   JavaScript code to be executed
 * @return  boolean
 * @see     o2js_exe(), o2js_load_script(), o2js_load_css()
 */
function o2js_doc_ready($js_code) {

    $js_code = "o2jse.docReady=function(){\n".$js_code."\n};";
    if ($GLOBALS['jxjs']) {
        $_SESSION['o2_app']->js_cache.= $js_code."\n";
        }
    else {
        print "<script> ".$js_code." </script>\n";
        }

    }


/**
 * Adds a JS resource, as a -SCRIPT- tag in HTML -HEAD- tag.
 * This method, in a dynamic context, is equivalent to add a JS link in page head.
 *
 * @package javascript
 * @param   string $js_url   URL to JS resource to link
 * @return  boolean
 * @see     o2js_doc_ready(), o2js_exe(), o2js_load_css()
 */
function o2js_load_script($js_url) {

    $js_code = 'o2jse.loadScript("'.$js_url.'");';
    if ($GLOBALS['jxjs']) {
        $_SESSION['o2_app']->js_cache.= $js_code."\n";
        }
    else {
        print "<script> ".$js_code." </script>\n";
        }

    }


/**
 * Adds a CSS resource, as a -STYLE- tag in HTML -HEAD- tag.
 * This method, in a dynamic context, is equivalent to add a CSS-STYLE link in page head.
 *
 * @package javascript
 * @param   string $css_url   URL to CSS resource to link
 * @return  boolean
 * @see     o2js_doc_ready(), o2js_exe(), o2js_load_script()
 */
function o2js_load_css($css_url) {

    $js_code = 'o2jse.loadCSS("'.$css_url.'");';
    if ($GLOBALS['jxjs']) {
        $_SESSION['o2_app']->js_cache.= $js_code."\n";
        }
    else {
        print "<script> ".$js_code." </script>\n";
        }

    }


// ====================================== BROWSER ========================================
/**
 * Returns information string provided by the client browser
 *
 * @package client
 * @return  string
 * @see     o2client_ip(), o2client_width(), o2client_height()
 */
function o2client_browser() {

    return $_SERVER['HTTP_USER_AGENT'];

    }


/**
 * Returns client browser ip address
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_host(), o2client_platform(), o2client_os()
 */
function o2client_ip() {

    return $_SERVER['REMOTE_ADDR'].
           ($_SERVER['REMOTE_PORT'] ? ":".$_SERVER['REMOTE_PORT'] : "");

    }


/**
 * Returns client machine host name
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_ip(), o2client_platform(), o2client_os()
 */
function o2client_host() {

    return $_SERVER['HTTP_HOST'];

    }


/**
 * Returns client machine platform description
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_host(), o2client_ip(), o2client_os()
 */
function o2client_platform() {

    $cli_str = $_SERVER['HTTP_USER_AGENT'];
    $sub_str = array();
    preg_match('/\(([^\(\)]*)\)/', $cli_str, $sub_str);
    $str_parts = explode(";", $sub_str[1]);
    return $str_parts[0];

    }


/**
 * Returns client machine operating system description
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_host(), o2client_platform(), o2client_ip()
 */
function o2client_os() {

    $cli_str = $_SERVER['HTTP_USER_AGENT'];
    $sub_str = array();
    preg_match('/\(([^\(\)]*)\)/', $cli_str, $sub_str);
    $str_parts = explode(";", $sub_str[1]);
    return $str_parts[2];

    }


/**
 * Returns client browser window width
 *
 * @package client
 * @return  integer
 * @see     o2client_browser(), o2client_height()
 */
function o2client_width() {

    $app = $_SESSION['o2_app'];
    return intval($app->client_width -
                  ($app->menu_attivo && $app->menu_style != 'T' ? $app->menu_width : 0));

    }


/**
 * Returns client browser window height
 *
 * @package client
 * @return  integer
 * @see     o2client_browser(), o2client_width()
 */
function o2client_height() {

    $app = $_SESSION['o2_app'];
    return intval($app->client_height -
                  ($app->menu_attivo && $app->menu_style == 'T' ? $app->menu_height : 0) -
                  $app->status_height);

    }


// ==================================== APPLICATION ======================================

/**
 * Returns php session id
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_alias()
 */
function o2app_sid() {

    $app_sess_id = $_SESSION['o2_app']->session_id;
    return ($app_sess_id ? $app_sess_id : session_id());

    }


/**
 * If called with no parameters simply returns currently logged user.
 * If $new_user is valorized it will try to change the current user with user=$new_user
 * and the OLD user is returned.
 *
 * @package application
 * @param   string $new_user   New user name
 * @return  string
 * @see     o2app_password(), o2user_roles(), o2user_rights(), o2app_runlevel()
 */
function o2app_user($new_user = false) {

    $app  = $_SESSION['o2_app'];
    $user = $app->user;
    // __________________________________________________________________ User playing ___
    if ($new_user) {
        $app->set_user($new_user, $app->get_user_password($new_user));
        }
    return $user;

    }


/**
 * Sets application parameter named $parameter to the passed $value for the current
 * session
 *
 * @package application
 * @param   string $parameter   Application setting name
 * @param   mix    $value       Value to set
 * @return  boolean
 * @see     o2app_user(), o2app_runlevel()
 */
function o2app_iniset($parameter, $value) {

    $_SESSION['o2_app']->settings(array($parameter => $value));
    return true;

    }


/**
 * Returns currently logged user password (MD5)
 *
 * @package application
 * @return  string
 * @see     o2app_user(), o2app_developer(), o2app_runlevel()
 */
function o2app_password() {

    return $_SESSION['o2_app']->password;

    }


/**
 * Returns application css file name
 *
 * @package application
 * @return  string
 * @see     o2app_alias(), o2app_menu()
 */
function o2app_css() {

    return $_SESSION['o2_app']->css;

    }


/**
 * Returns the names of running programs in an array
 *
 * @package application
 * @return  array
 */
function o2app_prg() {

    $prg_local = array();
    foreach ($_SESSION['o2_app']->istanze_prg as $single_prg) {
        $prg_local[] = $single_prg->nome;
        }
    return $prg_local;

    }


/**
 * Returns application name
 *
 * @package application
 * @return  string
 * @see     o2app_sid(), o2app_alias()
 */
function o2app_name() {

    return $_SESSION['o2_app']->nome;

    }


/**
 * Returns application alias as an URL prefix, mostly useful to address application
 * resuources (images, JS libraries, ...)
 *
 * NOTE: application alias is returned with ending back-slash.
 *
 * Sample: if you are serving your application from "<myhost>/apps/myapp/myapp.php" main
 *         page, then application alias will be returned as "/apps/myapp/".
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_dir_htdocs()
 */
function o2app_alias() {

    return $_SESSION['o2_app']->alias;

    }


/**
 * If parameter $title is passed then its value is set as new application title and the
 * old one is returned.
 * If no $title is passed then the actual title is just returned.
 *
 * @package application
 * @param   string $title
 * @return  string
 * @see     o2app_name(), o2app_alias()
 */
function o2app_title($title = false) {

    $app = $_SESSION['o2_app'];
    $old = $app->titolo;
    if ($title) {
        $script = "document.getElementsByTagName('TITLE')[0].innerHTML='".
                  addslashes(jx_encode($title))."'";
        if ($GLOBALS['jxjs']) {
            $app->js_cache.= $script.";\n";
            }
        else {
            print "<script>".$script."</script>\n";
            }
        $app->titolo = $title;
        }
    return $old;

    }


/**
 * Returns application type of validation
 *
 * @package application
 * @return  integer
 * @see     o2app_nologin(), o2app_user(), o2app_password()
 */
function o2app_validate() {

    return $_SESSION['o2_app']->validazione;

    }


/**
 * Returns application character encoding
 *
 * @package application
 * @return  string
 * @see     o2app_lang()
 */
function o2app_encoding() {

    return $_SESSION['o2_app']->chr_encoding;

    }


/**
 * Returns session timeout
 *
 * @package application
 * @return  integer
 * @see     o2app_exetime()
 */
function o2app_timeout() {

    return $_SESSION['o2_app']->timeout;

    }


/**
 * Returns elapsed time in microseconds since script started
 *
 * @package application
 * @return  float
 * @see     o2app_timeout()
 */
function o2app_exetime() {

    return o2_time() - $GLOBALS['jxtime'];

    }


/**
 * Returns application run-level: [DEV] for development or [EXE] for normal
 * execution
 *
 * @package application
 * @return  string
 * @see     o2app_developer()
 */
function o2app_runlevel() {

    return $_SESSION['o2_app']->run_level;

    }


/**
 * Returns application run-mode: [HTML] for standard WEB mode, [BAT] for console execution
 * and [RPC] for WEB service mode
 *
 * @package application
 * @return  string
 * @see     o2app_runlevel()
 */
function o2app_runmode() {

    $app = $_SESSION['o2_app'];
    return ($app->rpc_server ? 'RPC' : $app->runtime->interface);

    }


/**
 * Returns currently logged developer name
 *
 * @package application
 * @return  string
 * @see     o2app_runlevel()
 */
function o2app_developer() {

    return $_SESSION['o2_app']->developer;

    }


/**
 * Returns page to be displayed on login failure
 *
 * @package application
 * @return  string
 * @see     o2app_validate(), o2app_user(), o2app_password(), o2app_sesserror()
 */
function o2app_nologin() {

    return $_SESSION['o2_app']->no_login;

    }


/**
 * Returns page to be displayed on session validation failure
 *
 * @package application
 * @return  string
 * @see     o2app_validate(), o2app_user(), o2app_password(), o2app_nologin()
 */
function o2app_sesserror() {

    return $_SESSION['o2_app']->sess_error;

    }


/**
 * Returns the name of the currently active application menu
 *
 * @package application
 * @return  string
 * @see     o2app_menus(), o2app_user()
 */
function o2app_menu() {

    return $_SESSION['o2_app']->menu_attivo;

    }


/**
 * Returns FALSE if no translation is active, else returns the string-code for the active
 * language
 *
 * @return string
 * @see    o2_translate()
 */
function o2app_lang() {

    return $_SESSION['o2_app']->multilang;

    }


/**
 * Returns application variables names as an array
 *
 * @package application
 * @return  array
 * @see     o2val()
 */
function o2app_vars() {


    $list = array();
    foreach ($_SESSION['o2_app']->vars as $var_id => $single_var) {
        $list[$var_id] = $single_var->maschera->nome;
        }
    return $list;

    }


/**
 * Returns menus repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_menu()
 */
function o2app_menus() {

    $app       = $_SESSION['o2_app'];
    $arr_local = array_keys($app->menu);
    foreach ($arr_local as $single_menu) {
        if (isset($app->menu[$single_menu]) && count($app->menu[$single_menu]->voci)) {
            $arr_local = array_merge($arr_local,
                                     array_keys($app->menu[$single_menu]->voci));
            }
        }
    return array_unique($arr_local);

    }


/**
 * Returns data models repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_user(), o2app_dir_home()
 */
function o2app_models() {

    return array_keys($_SESSION['o2_app']->maschere);

    }


/**
 * Returns servers repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_databases(), o2app_tables()
 */
function o2app_servers() {

    return array_keys($_SESSION['o2_app']->server);

    }


/**
 * Returns databases repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_servers(), o2app_tables()
 */
function o2app_databases() {

    return array_keys($_SESSION['o2_app']->db);

    }


/**
 * Returns tables repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_databases(), o2app_servers()
 */
function o2app_tables() {

    $app = $_SESSION['o2_app'];
    if ($app->split_tab_repos) {
        if ($app->run_level == "DEV" &&
            $app->dir_cvs_progs->exists($app->file_tabelle)) {
            $file2inc = $app->dir_cvs_progs->element($app->file_tabelle);
            }
        else {
            $file2inc = $app->dir_progs->element($app->file_tabelle);
            }
        include($file2inc);
        $app->remap_tables_on_db();
        $app->create_logtabs();
        }
    return array_keys($app->tab);

    }


/**
 * Creates unexisting tables for the whole application
 *
 * @package application
 * @return  boolean
 * @see     o2app_servers(), o2app_databases(), o2app_tables()
 */
function o2app_createtables() {

    $app = $_SESSION['o2_app'];
    if ($app->split_tab_repos) {
        if ($app->run_level == "DEV" &&
            $app->dir_cvs_progs->exists($app->file_tabelle)) {
            $file2inc = $app->dir_cvs_progs->element($app->file_tabelle);
            }
        else {
            $file2inc = $app->dir_progs->element($app->file_tabelle);
            }
        include($file2inc);
        $app->remap_tables_on_db();
        }
    return $app->create_tables();

    }


/**
 * Returns list of defined programs
 *
 * @package application
 * @return  array
 * @see     o2app_startprg(), o2app_reqprg(), o2app_menus()
 */
function o2app_prgs_list() {

    $list = array();
    $dir  = o2app_dir_prgs();
    if (is_dir($dir)) {
        if ($dh = opendir($dir)) {
            while (($file = readdir($dh)) !== false) {
                $file = trim($file);
                if (substr($file, -3) == 'prg') {
                    $prg        = substr($file, 0, -4);
                    $list[$prg] = $prg;
                    }
                }
            }
        closedir($dh);
        }
    asort($list);
    return $list;

    }


/**
 * Returns the name of the program to be executed once after login
 *
 * @package application
 * @return  string
 * @see     o2app_reqprg(), o2app_user()
 */
function o2app_startprg() {

    return $_SESSION['o2_app']->prg_start;

    }


/**
 * Returns the name of the program to be executed on every request
 *
 * @package application
 * @return  string
 * @see     o2app_startprg(), o2app_user()
 */
function o2app_reqprg() {

    return $_SESSION['o2_app']->prg_request;

    }


/**
 * Returns information about html form and controls which sent the
 * submit.
 * $index parameter can be passed as "form" or "ctrl", to get submit form name or control
 * name.
 * If $index is passed as something else or it is omitted, an array is returned in the
 * form: array("form" => form_name, "ctrl" => ctrl_name).
 *
 *
 * @package application
 * @param   string $index
 * @return  array
 */
function o2app_submit($index = false) {

    $ret = $_SESSION['o2_app']->info_submit;
    switch (strtolower($index)) {
        case "form":
           $ret = $ret["form"];
           break;
        case "ctrl":
           $ret = $ret["ctrl"];
           break;
        }
    return $ret;

    }


/**
 * Returns application "home" folder path
 *
 * @package application
 * @return  string
 * @see     o2app_sharedir(), o2app_name(), o2app_alias()
 */
function o2app_dir_home() {

    return $_SESSION['o2_app']->dir_home->nome_completo;

    }


/**
 * Returns application WEB root folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_home(), o2app_alias()
 */
function o2app_dir_htdocs() {

    return $_SESSION['o2_app']->dir_htdocs->nome_completo;

    }


/**
 * Returns application programs folder path, containing programs
 * and repositories
 *
 * @package application
 * @return  string
 * @see     o2app_dir_cvsprgs(), o2app_dir_home()
 */
function o2app_dir_prgs() {

    return $_SESSION['o2_app']->dir_progs->nome_completo;

    }


/**
 * Returns current cvs (team development) folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_prgs(), o2app_dir_home()
 */
function o2app_dir_cvsprgs() {

    return $_SESSION['o2_app']->dir_cvs_progs->nome_completo;

    }


/**
 * Returns current tmp folder path, depending on logged user name
 *
 * @package application
 * @return  string
 * @see     o2app_dir_uploads(), o2app_dir_trn(), o2app_dir_home()
 */
function o2app_dir_tmp() {

    return $_SESSION['o2_app']->dir_tmp->nome_completo;

    }


/**
 * Returns current data folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_data() {

    return $_SESSION['o2_app']->dir_data->nome_completo;

    }


/**
 * Returns path to documentation files folder
 *
 * @package application
 * @return  string
 * @see     o2app_dir_htdocs(), o2app_dir_home()
 */
function o2app_dir_doc() {

    return $_SESSION['o2_app']->dir_doc->nome_completo;
    }


/**
 * Returns CSS folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_img(), o2app_dir_home()
 */
function o2app_dir_css() {

    return $_SESSION['o2_app']->dir_css->nome_completo;

    }


/**
 * Returns logs folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_logs() {

    return $_SESSION['o2_app']->dir_logs->nome_completo;

    }


/**
 * Returns transit (delivering system) folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp(), o2app_dir_home(), o2app_alias()
 */
function o2app_dir_trn() {

    return $_SESSION['o2_app']->dir_trn->nome_completo;

    }


/**
 * Returns pdf printing templates folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_usertemplates(), o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_templates() {

    return $_SESSION['o2_app']->dir_models->nome_completo;

    }


/**
 * Returns user personal pdf printing templates folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_templates(), o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_usertemplates() {

    return $_SESSION['o2_app']->dir_models_u->nome_completo;

    }


/**
 * Returns images folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_userimg(), o2app_dir_css(), o2app_dir_home()
 */
function o2app_dir_img() {

    return $_SESSION['o2_app']->dir_img->nome_completo;

    }


/**
 * Returns user personal images folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_img(), o2app_dir_css(), o2app_dir_home()
 */
function o2app_dir_userimg() {

    return $_SESSION['o2_app']->dir_img_u->nome_completo;

    }


/**
 * Returns currently logged user
 *
 * @package application
 * @return  string
 * @see     o2app_dir_usertemplates(), o2app_userimg(), o2app_dir_home(), o2app_user()
 */
function o2app_dir_user() {

    return $_SESSION['o2_app']->dir_user->nome_completo;

    }


/**
 * Returns uploaded files folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp()
 * @see     o2app_dir_home()
 */
function o2app_dir_uploads() {

    return $_SESSION['o2_app']->dir_uploads->nome_completo;

    }


/**
 * Execute session logout: delete session record and destroy WEB session related context.
 * If any, after logout navigation will be redirected to provided URL.
 *
 * @package application
 * @param   string $exit_url
 * @return  boolean
 */
function o2app_logout($exit_url = false) {

    $_SESSION['o2_app']->logout($exit_url);

    }


// ======================================= MENU ==========================================

/**
 * Return TRUE if requested menu or menu item is active for the current session
 *
 * @package menu
 * @param   string $item_name
 * @return  string
 */
function o2menu_isactive($menu_item) {

    $app = $_SESSION['o2_app'];
    if ($app->menu_attivo) {
        $app->includi_repository($app->file_menu);
        // ___________________________________________________________ Menus profiling ___
        if ($app->profiling == "M" || $app->profiling == "B") {
            $app->intcall("tools/o2sys_filter_menu");
            }
        if ($app->menu[$app->menu_attivo]->get_subitem($menu_item)) {
            return true;
            }
        else {
            return false;
            }
        $app->menu = array();
        }
    return false;

    }


/**
 * Return type of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_type($menu_name = "") {

    $app       = $_SESSION['o2_app'];
    $arr_local = $app->menu;
    foreach ($arr_local as $single_menu) {
        if (isset($app->menu[$single_menu->nome]) &&
            count($app->menu[$single_menu->nome]->voci)) {
            $arr_local = array_merge($arr_local, $app->menu[$single_menu->nome]->voci);
            }
        }
    foreach ($arr_local as $single_menu) {
        if ($single_menu->nome == $menu_name) {
            return ($single_menu->tipo ? strtoupper($single_menu->tipo) : "M");
            }
        }
    return false;

    }


/**
 * Return label of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_label($menu_name = "") {

    $app = $_SESSION['o2_app'];
    if ($app->menu == array()) {
        $app->includi_repository($app->file_menu);
        }
    $arr_local = $app->menu;
    foreach ($arr_local as $single_menu) {
        if (isset($app->menu[$single_menu->nome]) &&
            count($app->menu[$single_menu->nome]->voci)) {
            $arr_local = array_merge($arr_local, $app->menu[$single_menu->nome]->voci);
            }
        }
    foreach ($arr_local as $single_menu) {
        if ($single_menu->nome == $menu_name) {
            return $single_menu->label;
            }
        }
    return false;

    }


/**
 * Return data value of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_data($menu_name = "") {

    $app       = $_SESSION['o2_app'];
    $arr_local = $app->menu;
    foreach ($arr_local as $single_menu) {
        if (isset($app->menu[$single_menu->nome]) &&
            count($app->menu[$single_menu->nome]->voci)) {
            $arr_local = array_merge($arr_local, $app->menu[$single_menu->nome]->voci);
            }
        }
    foreach ($arr_local as $single_menu) {
        if ($single_menu->nome == $menu_name) {
            return $single_menu->data;
            }
        }
    return false;

    }


/**
 * Return icon file name of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_icon($menu_name = "") {

    $app       = $_SESSION['o2_app'];
    $arr_local = $app->menu;
    foreach ($arr_local as $single_menu) {
        if (isset($app->menu[$single_menu->nome]) &&
            count($app->menu[$single_menu->nome]->voci)) {
            $arr_local = array_merge($arr_local, $app->menu[$single_menu->nome]->voci);
            }
        }
    foreach ($arr_local as $single_menu) {
        if ($single_menu->nome == $menu_name) {
            return $single_menu->ico;
            }
        }
    return false;

    }


/**
 * Add an item to an existing menu. If item is a menu ($item_type == "m") menu container
 * is created too.
 * New item is inserted at $position in parent menu (starting from 1). If $position is
 * zero, or not passed, then new item is appended at last position to parent menu.
 *
 * @package menu
 * @param   string  $menu_name
 * @param   string  $item_type
 * @param   string  $item_name
 * @param   string  $item_label
 * @param   string  $item_data
 * @param   boolean $visible
 * @param   boolean $active
 * @param   string  $item_ico
 * @param   integer $position
 * @return  boolean
 */
function o2menu_additem($menu_name  = "",
                        $item_type  = "S",
                        $item_name  = "",
                        $item_label = "",
                        $item_data  = "",
                        $visible    = true,
                        $active     = true,
                        $item_ico   = "",
                        $position   = 0) {

    $app = $_SESSION['o2_app'];
    $ret = false;
    if (strtolower($item_type[0]) == "m" && !isset($app->menu[$item_name])) {
        $app->menu[$item_name] = new o2_menubar($item_name, $item_ico);
        $ret = true;
        }
    if (isset($app->menu[$menu_name])) {
        $app->menu[$menu_name]->voce($item_type,
                                     $item_name,
                                     $item_label,
                                     $item_data,
                                     $visible,
                                     $active,
                                     $item_ico,
                                     $position);
        $ret = true;
        }
    return $ret;

    }


/**
 * Return HTML code for a requested menu
 *
 * @package menu
 * @param   string $menu_name
 * @param   string $position
 * @return  boolean
 */
function o2menu_code($menu_name = "", $position = "t") {

    if (strtolower($position) != "b") {
        $position = "t";
        }
    $app = $_SESSION['o2_app'];
    if (isset($app->menu[$menu_name])) {
        return o2html::menu_bar($app->menu[$menu_name], true);
        }
    else {
        return false;
        }

    }


// ==================================== SQL ENGINE =======================================

/**
 * Returns SQL engine type specific open delimiter for quoting elements names
 *
 * @package sql
 * @param   string $engine_type
 * @return  string
 * @see     o2sql_delimiter_close(), o2dbs_engine(), o2db_engine(), o2tab_engine()
 */
function o2sql_delimiter_open($engine_type) {

    if ($engine_type) {
        return constant("o2_".$engine_type."_o");
        }
    else {
        return false;
        }

    }


/**
 * Returns SQL engine type specific close delimiter for quoting elements names
 *
 * @package sql
 * @param   string $engine_type
 * @return  string
 * @see     o2sql_delimiter_open(), o2dbs_engine(), o2db_engine(), o2tab_engine()
 */
function o2sql_delimiter_close($engine_type) {

    if ($engine_type) {
        return constant("o2_".$engine_type."_c");
        }
    else {
        return false;
        }

    }


// ====================================== SERVER =========================================

/**
 * Returns database server host name or ip
 *
 * @package server
 * @param   string $server_name
 * @return  string
 * @see     o2dbs_engine(), o2dbs_user(), o2dbs_password()
 */
function o2dbs_name($server_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        return $app->server[$server_name]->server;
        }
    else {
        return false;
        }

    }


/**
 * Returns database server engine type (mysql, postgres, oracle, sqlite, ...)
 *
 * @package server
 * @param   string $server_name
 * @return  string
 * @see     o2dbs_name(), o2dbs_user(), o2dbs_password()
 */
function o2dbs_engine($server_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        return $app->server[$server_name]->type;
        }
    else {
        return false;
        }

    }


/**
 * Returns database server user id
 *
 * @package server
 * @param   string $server_name
 * @return  string
 * @see     o2dbs_password(), o2dbs_name()
 */
function o2dbs_user($server_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        return $app->server[$server_name]->user;
        }
    else {
        return false;
        }

    }


/**
 * Returns database server user password
 *
 * @package server
 * @param   string $server_name
 * @return  string
 * @see     o2dbs_user(), o2dbs_name()
 */
function o2dbs_password($server_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        return $app->server[$server_name]->password;
        }
    else {
        return false;
        }

    }


/**
 * Executes a sql query directly on database server and returns the result dataset as an
 * array, in the form:
 *
 *    arr[0] = array(field_0 => value_0, field_1 => value_1, ...)
 *    arr[1] = array(field_0 => value_0, field_1 => value_1, ...)
 *    ...
 *    arr[n] = array(field_0 => value_0, field_1 => value_1, ...)
 *
 * This function is useful to execute queries for dataset retrieving. If you need to
 * execute statements, like UPDATE, DELETE, CREATE, ecc... see o2dbs_execute() function
 * instead.
 *
 * @package server
 * @param   string $server_name
 * @param   string $command_str
 * @return  array
 * @see     o2dbs_execute(), o2dbs_engine()
 */
function o2dbs_query($server_name, $command_str) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        return $app->server[$server_name]->esegui($command_str, false);
        }
    else {
        return false;
        }

    }


/**
 * Executes sql script command directly on database server.
 * Returns the number of affected rows.
 * This function is useful to execute statements, like UPDATE, DELETE, CREATE, ecc...
 * If you need to retrieve a dataset see o2dbs_query() function instead.
 *
 * @package server
 * @param   string $server_name
 * @param   string $command_str
 * @return  integer
 * @see     o2dbs_query(), o2dbs_commit(), o2dbs_engine()
 */
function o2dbs_execute($server_name, $command_str) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        return $app->server[$server_name]->esegui($command_str, true);
        }
    else {
        return false;
        }

    }


/**
 * Commits transaction on server, if transaction started, for the current transaction
 * isolation level.
 *
 * @package server
 * @param   string $server_name
 * @return  boolean
 * @see     o2transaction_isolate(), o2dbs_rollback(), o2dbs_name(), o2dbs_engine()
 */
function o2dbs_commit($server_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        $server = $app->server[$server_name];
        return o2_gateway::commit($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password);
        // _____________________________________________________ Reset database errors ___
        unset($GLOBALS["jxlasterror"]);
        }
    else {
        return false;
        }

    }


/**
 * Rollsback transaction on server, if transaction started, for the current transaction
 * isolation level.
 *
 * @package server
 * @param   string $server_name
 * @return  boolean
 * @see     o2transaction_isolate(), o2dbs_commit(), o2dbs_name(), o2dbs_engine()
 */
function o2dbs_rollback($server_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->server[$server_name])) {
        $server = $app->server[$server_name];
        $engine = $server->type;
        // _____________________________ If an active transaction is found on a server ___
        if (isset($GLOBALS['o2_'.$engine.'_trans'])) {
            $keys = array_keys($GLOBALS['o2_'.$engine.'_trans']);
            $key  = $keys[count($keys) - 1];
            if (!is_array($GLOBALS['o2_'.$engine.'_error'])) {
                $GLOBALS['o2_'.$engine.'_error'] = array();
                }
            $GLOBALS['o2_'.$engine.'_error'][$key] = true;
            }
        return o2_gateway::commit($engine,
                                  $server->server,
                                  $server->user,
                                  $server->password);
        // _____________________________________________________ Reset database errors ___
        unset($GLOBALS["jxlasterror"]);
        }
    else {
        return false;
        }

    }


// ===================================== DATABASE ========================================

/**
 * Returns database physical name
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_name($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return $app->db[$db_name]->nome;
        }
    else {
        return false;
        }

    }


/**
 * Returns database server name
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_server($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return $app->db[$db_name]->server->nome;
        }
    else {
        return false;
        }

    }


/**
 * Returns database engine type (mysql, postgres, oracle, sqlite, ...)
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_engine($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return $app->db[$db_name]->server->type;
        }
    else {
        return false;
        }

    }


/**
 * Returns database specific prefix to fully qualify elements
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_qprefix($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        $db = $app->db[$db_name];
        return o2_gateway::qualify($db->server->type, $db->nome, $db->proprietario);
        }
    else {
        return false;
        }

    }


/**
 * Returns database schema identifier
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_schema($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return $app->db[$db_name]->proprietario;
        }
    else {
        return false;
        }

    }


/**
 * Returns database tables list as an array
 *
 * @package database
 * @param   string $db_name
 * @return  array
 */
function o2db_tables($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return $app->db[$db_name]->tabelle();
        }
    else {
        return false;
        }

    }


/**
 * Returns true if is active ASP mode for the database
 *
 * @package database
 * @param   string $db_name
 * @return  mixed
 */
function o2db_asp($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return $app->db[$db_name]->asp;
        }
    else {
        return false;
        }

    }


/**
 * Returns "Create tables" flag value for requested database
 *
 * @package database
 * @param   string $db_name
 * @return  boolean
 */
function o2db_createtab($db_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->db[$db_name])) {
        return ($app->db[$db_name]->create_tabs ? true : false);
        }
    else {
        return false;
        }

    }


/**
 * Activates all defined indexes for the database and returns the number of activated
 * ones.
 * Function activates both kind of indexes, from repository and extra indexes.
 *
 * @package database
 * @param   string $db_name
 * @return  integer
 * @see     o2db_indexes_setoff(), o2tab_indexes_seton(), o2tab_indexes()
 */
function o2db_indexes_seton($db_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if ($app->split_tab_repos && $app->run_level != "DEV") {
        include($app->dir_progs->element($app->file_tabelle));
        $app->remap_tables_on_db();
        }
    foreach ($app->tab as $tab_name => $tab) {
        if ($tab->db->id == $db_name && $tab->exists()) {
            $ret+= o2tab_indexes_seton($tab_name);
            }
        }
    return $ret;

    }


/**
 * Deactivates all active indexes for the database and returns the number of
 * deactivated ones.
 * Function deactivates both kind of indexes, from repository and extra indexes.
 *
 * @package database
 * @param   string $db_name
 * @return  integer
 * @see     o2db_indexes_seton(), o2tab_index_setoff(), o2tab_indexes()
 */
function o2db_indexes_setoff($db_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if ($app->split_tab_repos && $app->run_level != "DEV") {
        include($app->dir_progs->element($app->file_tabelle));
        $app->remap_tables_on_db();
        }
    foreach ($app->tab as $tab_name => $tab) {
        if ($tab->db->id == $db_name && $tab->exists()) {
            $ret+= o2tab_fkeys_setoff($tab_name);
            }
        }
    return $ret;

    }


/**
 * Activates all defined foreign keys for the database and returns the number of activated
 * ones
 *
 * @package database
 * @param   string $db_name
 * @return  integer
 * @see     o2db_fkeys_setoff(), o2tab_fkeys_seton(), o2tab_fkeys_list()
 */
function o2db_fkeys_seton($db_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if ($app->split_tab_repos && $app->run_level != "DEV") {
        include($app->dir_progs->element($app->file_tabelle));
        $app->remap_tables_on_db();
        }
    foreach ($app->tab as $tab_name => $tab) {
        if ($tab->db->id == $db_name && $tab->exists()) {
            $ret+= o2tab_fkeys_seton($tab_name);
            }
        }
    return $ret;

    }


/**
 * Deactivates all active foreign keys for the database and returns the number of
 * deactivated ones
 *
 * @package database
 * @param   string $db_name
 * @return  integer
 * @see     o2db_fkeys_seton(), o2tab_fkey_setoff(), o2tab_fkeys_list()
 */
function o2db_fkeys_setoff($db_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if ($app->split_tab_repos && $app->run_level != "DEV") {
        include($app->dir_progs->element($app->file_tabelle));
        $app->remap_tables_on_db();
        }
    foreach ($app->tab as $tab_name => $tab) {
        if ($tab->db->id == $db_name && $tab->exists()) {
            $ret+= o2tab_fkeys_setoff($tab_name);
            }
        }
    return $ret;

    }


// ====================================== MODEL ==========================================

/**
 * Returns model type
 *
 * @package model
 * @param   string $model_name
 * @return  string
 */
function o2model_type($model_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->maschere[$model_name])) {
        return $app->maschere[$model_name]->tipo;
        }
    else {
        return false;
        }

    }


/**
 * Returns model input/output mask
 *
 * @package model
 * @param   string $model_name
 * @return  string
 */
function o2model_mask($model_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->maschere[$model_name])) {
        return $app->maschere[$model_name]->maschera;
        }
    else {
        return false;
        }

    }


/**
 * Returns model data dimension as "string_length"
 *
 * @package model
 * @param   string $model_name
 * @return  integer
 */
function o2model_size($model_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->maschere[$model_name])) {
        return $app->maschere[$model_name]->dimensione;
        }
    else {
        return false;
        }

    }


/**
 * Returns an array of model possible values as defined in picture.
 *
 * @package model
 * @param   string  $model_name
 * @return  array
 */
function o2model_values($model_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->maschere[$model_name])) {
        return $app->maschere[$model_name]->valori;
        }
    else {
        return false;
        }

    }


/**
 * Returns an array of model possible values as defined in picture.
 * If second parameter $add_blank is passed as TRUE, then blank ('' or 0) value is added
 * to list, where not already present.
 * This is mostly useful for feeding combos filtering $model_name related data.
 *
 * @package model
 * @param   string  $model_name
 * @param   boolean $add_blank
 * @param   string  $blank_label
 * @return  array
 */
function o2model_values4combo($model_name, $add_blank = false, $blank_label = "") {

    $app = $_SESSION['o2_app'];
    if (isset($app->maschere[$model_name])) {
        $mask = $app->maschere[$model_name];
        $list = array();
        // __________________________________________________ Values list from picture ___
        if (is_array($mask->valori)) {
            foreach ($mask->valori as $singvalue) {
                $list[substr($singvalue, 0, $mask->dimensione)] = $singvalue;
                }
            }
        if ($add_blank && !isset($list[" "])) {
            $list = array(" " => ($blank_label ? $blank_label : "")) + $list;
            }
        return $list;
        }
    else {
        return false;
        }

    }


// ====================================== FIELD ==========================================

/**
 * Returns a table field physical name
 *
 * @package field
 * @param   string $table_name
 * @param   string $field_name
 * @return  string
 */
function o2field_name($table_name, $field_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->tab[$table_name]) &&
        isset($app->tab[$table_name]->campi[$field_name])) {
        // ______________________________ Physical name is returned without delimiters ___
        return $app->tab[$table_name]->campi[$field_name]->phys_name;
        }
    elseif (isset($app->tab[$table_name]) &&
            $app->tab[$table_name]->asp == 'C' &&
            strtoupper($field_name) == 'O2ASPID') {
        return $app->tab[$table_name]->campi['O2ASPID']->phys_name;
        }
    else {
        return false;
        }

    }


/**
 * Returns a table field delimited physical name
 *
 * @package field
 * @param   string $table_name
 * @param   string $field_name
 * @return  string
 */
function o2field_fname($table_name, $field_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->tab[$table_name]) &&
        isset($app->tab[$table_name]->campi[$field_name])) {
        // _________________________________ Physical name is returned with delimiters ___
        return $app->tab[$table_name]->campi[$field_name]->nome_fisico;
        }
    elseif (isset($app->tab[$table_name]) &&
            $app->tab[$table_name]->asp == 'C' &&
            strtoupper($field_name) == 'O2ASPID') {
        return $app->tab[$table_name]->campi['O2ASPID']->nome_fisico;
        }
    else {
        return false;
        }

    }


/**
 * Returns a table field model
 *
 * @package field
 * @param   string $table_name
 * @param   string $field_name
 * @return  string
 */
function o2field_model($table_name, $field_name) {

    $app = $_SESSION['o2_app'];
    if (isset($app->tab[$table_name]) &&
        isset($app->tab[$table_name]->campi[$field_name])) {
        return $app->tab[$table_name]->campi[$field_name]->maschera->nome;
        }
    elseif (isset($app->tab[$table_name]) &&
            $app->tab[$table_name]->asp == 'C' &&
            strtoupper($field_name) == "O2ASPID") {
        return $app->vars['_area']->maschera->nome;
        }
    else {
        return false;
        }

    }


// ====================================== TABLE ==========================================

/**
 * Returns true if table physically exists, else false
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_exists($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->exists();
        }
    else {
        return false;
        }

    }


/**
 * Returns true if table contains any data, else false
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_data($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->data();
        }
    else {
        return false;
        }

    }


/**
 * Creates the table
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_create($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->crea();
        }
    else {
        return false;
        }

    }


/**
 * Drops the table
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_drop($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->elimina();
        }
    else {
        return false;
        }

    }


/**
 * Return sql creation script for the table
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_script($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->crea(false);
        }
    else {
        return false;
        }

    }


/**
 * Returns table database
 *
 * @package table
 * @param   string $table_name
 * @return  string
 * @see o2tab_exists(), o2tab_engine(), o2tab_info()
 */
function o2tab_db($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->db->id;
        }
    else {
        return false;
        }

    }


/**
 * Returns table engine type (mysql, postgres, oracle, sqlite, ...)
 *
 * @package table
 * @param   string $table_name
 * @return  string
 * @see o2tab_db(), o2tab_info()
 */
function o2tab_engine($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->db->server->type;
        }
    else {
        return false;
        }

    }


/**
 * Returns table physical name
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_name($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->nome;
        }
    else {
        return false;
        }

    }


/**
 * Returns table fully qualified physiacal name, according with table engine type
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_qname($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return o2_gateway::qualify($table->db->server->type,
                                   $table->db->nome,
                                   $table->db->proprietario,
                                   $table->nome);
        }
    else {
        return false;
        }

    }


/**
 * Returns table fields as an array
 *
 * @package table
 * @param   string $table_name
 * @return  array
 */
function o2tab_fields($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        $fields_list = array_keys($table->campi);
        if ($table->asp == 'C') {
            array_unshift($fields_list, "O2ASPID");
            }
        return $fields_list;
        }
    else {
        return false;
        }

    }


/**
 * Returns table indexes as an array.
 * If $type_info parameter is passed as TRUE extra information are returned, about index
 * from repository or from extra-indexes and index unicity.
 *
 * @package table
 * @param   string  $table_name
 * @param   boolean $type_info
 * @return  array
 */
function o2tab_indexes($table_name, $type_info = false) {

    $app = $_SESSION['o2_app'];
    $ret = array();
    if ($table = $app->get_table($table_name)) {
        $db     = $table->db;
        $server = $db->server;
        // __________________________________________________________ Standard indexes ___
        foreach ($table->chiavi as $idx) {
            if ($type_info) {
                $ret[$idx->nome] = array('UNIQUE' => true, 'EXTRA' => false);
                }
            else {
                $ret[] = $idx->nome;
                }
            }
        // ____________________________________________________________  Extra indexes ___
        $ei_tab          = $app->get_table('o2_extraindexes');
        $ei_db           = $ei_tab->db;
        $ei_server       = $ei_db->server;
        $ei_type         = $ei_server->type;
        $ei_o            = constant("o2_".$ei_type."_o");
        $ei_c            = constant("o2_".$ei_type."_c");
        $fld_idx_name    = $ei_tab->campi["index_name"]->nome_fisico;
        $fld_table_name  = $ei_tab->campi["index_table"]->nome_fisico;
        $fld_unique_name = $ei_tab->campi["index_unique"]->nome_fisico;
        $ei_where        = $fld_table_name."='".$table_name."'";
        $ei_select       = $fld_idx_name." AS ".$ei_o."INDEX_NAME".$ei_c.",".
                           $fld_unique_name." AS ".$ei_o."INDEX_UNIQUE".$ei_c;
        $eidxs           = o2_gateway::recordset($ei_server->type,
                                                 $ei_server->server,
                                                 $ei_server->user,
                                                 $ei_server->password,
                                                 $ei_db->nome,
                                                 $ei_db->proprietario,
                                                 $ei_tab->nome,
                                                 $ei_tab->nome,
                                                 $ei_select,
                                                 $ei_where,
                                                 '',
                                                 100);
        foreach ($eidxs as $eidx) {
            $idx_name = $eidx['INDEX_NAME'];
            $unique   = ($eidx['INDEX_UNIQUE'] == '1');
            if ($type_info) {
                $ret[$idx_name] = array('UNIQUE' => $unique, 'EXTRA' => true);
                }
            else {
                $ret[] = $idx_name;
                }
            }
        return $ret;
        }
    else {
        return false;
        }

    }


/**
 * Returns table primary key name
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_pkey($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->chiave->nome;
        }
    else {
        return false;
        }

    }


/**
 * Returns TRUE if table log is active else FALSE
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_log($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return ($table->log_level ? true : false);
        }
    else {
        return false;
        }

    }


/**
 * Returns the log level 1-char setting for table, or FALSE if log is not active.
 * Possible log levels are:
 *  [R]ecord - Full record log for Insert, Delete and Update
 *  [D]elete - Only Delete operations are logged
 *  [I]nsert - Only Insert operations are logged
 *  [E]xistence - Insert and Delete operations are logged (I+D)
 *  [M]odify    - Update and Delete operations are logged (C+D)
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_log_level($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return ($table->log_level ? $table->log_level : false);
        }
    else {
        return false;
        }

    }


/**
 * Activate logging for a table and/or set/change log level.
 * Possible log levels are:
 *  [R]ecord    - Full record log for Insert, Delete and Update
 *  [D]elete    - Only Delete operations are logged
 *  [I]nsert    - Only Insert operations are logged
 *  [C]hange    - Update operations are logged with previous values
 *  [E]xistence - Insert and Delete operations are logged (I+D)
 *  [M]odify    - Update and Delete operations are logged (C+D)
 *
 * @package table
 * @param   string $table_name
 * @param   string $log_level
 * @return  boolean
 */
function o2tab_set_log($table_name, $log_level = "R") {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        if (strpos("RDICEM", strtoupper($log_level)) === false) {
            throw new o2_exception("Invalid log level option \"".$log_level.
                                  '": allowed options are "R", "D", "I", "C", "E" or "M"',
                                   o2error_UNKNOWN);
            return false;
            }
        else {
            $table->set_log($log_level);
            return true;
            }
        }
    else {
        throw new o2_exception("Unknown table <i>".$table_name.
                               "</i> requested for activating log",
                               o2error_UNKNOWNTABLE);
        return false;
        }

    }


/**
 * Deactivate logging for a table.
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_unset_log($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        $table->set_log();
        return true;
        }
    else {
        throw new o2_exception("Unknown table <i>".$table_name.
                               "</i> requested for deactivating log",
                               o2error_UNKNOWNTABLE);
        return false;
        }

    }


/**
 * Suspend active logging for a table, untill end of script or new function call to
 * reactivate (second parameter as FALSE).
 * If second parameter $suspend is passed as TRUE (default) logging is supended, else it
 * is reactivated.
 * This function is primary intended to create administrative transactions to work on
 * tables without triggering active logs.
 *
 * @package table
 * @param   string  $table_name
 * @param   boolean $suspend
 * @return  boolean
 */
function o2tab_suspend_log($table_name, $suspend = true) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        if ($suspend) {
            if (!isset($GLOBALS['jxlogsuspend'])) {
                $GLOBALS['jxlogsuspend'] = array();
                }
            $GLOBALS['jxlogsuspend'][] = $table_name;
            }
        elseif (isset($GLOBALS['jxlogsuspend'])) {
            $GLOBALS['jxlogsuspend'] = array_diff($GLOBALS['jxlogsuspend'],
                                                  array($table_name));
            }
        return true;
        }
    else {
        throw new o2_exception("Unknown table <i>".$table_name.
                               "</i> requested for suspending log",
                               o2error_UNKNOWNTABLE);
        return false;
        }

    }


/**
 * If record tracing is enabled for table returns a string with record tracing settings,
 * else FALSE.
 * Possible tracing settings are:
 *  - cu - creation user
 *  - cd - creation date
 *  - ct - creation time
 *  - uu - update user
 *  - ud - update date
 *  - ut - update time
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_trace($table_name) {

    $app = $_SESSION['o2_app'];
    if ($app->record_trace) {
        if ($table = $app->get_table($table_name)) {
            $trace = false;
            if (is_array($table->record_trace)) {
                $trace = implode(",", array_keys($table->record_trace));
                }
            return ($trace ? $trace : false);
            }
        else {
            throw new o2_exception("Unknown table <i>".$table_name.
                                   "</i> requested for record tracing settings",
                                   o2error_UNKNOWNTABLE);
            return false;
            }
        }
    else {
        return false;
        }

    }


/**
 * Suspend active tracing for a table, untill end of script or new function call to
 * reactivate (second parameter as FALSE).
 * If second parameter $suspend is passed as TRUE (default) tracing is supended, else it
 * is reactivated.
 * This function is primary intended to create administrative transactions to work on
 * tables without triggering active tracing.
 *
 * @package table
 * @param   string  $table_name
 * @param   boolean $suspend
 * @return  boolean
 */
function o2tab_suspend_trace($table_name, $suspend = true) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        if ($suspend) {
            if (!isset($GLOBALS['jxtracesuspend'])) {
                $GLOBALS['jxtracesuspend'] = array();
                }
            $GLOBALS['jxtracesuspend'][] = $table_name;
            }
        elseif (isset($GLOBALS['jxtracesuspend'])) {
            $GLOBALS['jxtracesuspend'] = array_diff($GLOBALS['jxtracesuspend'],
                                                    array($table_name));
            }
        return true;
        }
    else {
        throw new o2_exception("Unknown table <i>".$table_name.
                               "</i> requested for suspending tracing",
                               o2error_UNKNOWNTABLE);
        return false;
        }

    }


/**
 * Returns physical table informations as an array
 *
 * @package table
 * @param   string $table_name
 * @return  array
 */
function o2tab_info($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->info(true, true);
        }
    else {
        return false;
        }

    }


/**
 * Rebuilds table, eliminating differences between o2 structure and physical one
 *
 * @package table
 * @param   string $table_name
 * @param   array  $matches
 * @return  boolean
 */
function o2tab_rebuild($table_name, $matches = null) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->ricostruisci($matches);
        }
    else {
        return false;
        }

    }


/**
 * Physically renames table
 *
 * @package table
 * @param   string $table_name
 * @param   string $new_name
 * @return  boolean
 */
function o2tab_rename($table_name, $new_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->rinomina($new_name);
        }
    else {
        return false;
        }

    }


/**
 * Add a new column to table.
 *
 * @package table
 * @param   string  $table_name    Table name
 * @param   string  $column_name   New column physical name.
 * @param   string  $type          New column Janox type (A|N|L|D|T)
 * @param   integer $int           New column size (alpha) or integer digits (number)
 * @param   integer $dec           New column decimal digits (only number)
 * @return  boolean
 */
function o2tab_column_add($table_name, $column_name, $type, $int = 0, $dec = 0) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->alter_column_add($column_name, $type, $int, $dec);
        }
    else {
        return false;
        }

    }


/**
 * Remove a column from table.
 *
 * @package table
 * @param   string  $table_name    Table name
 * @param   string  $column_name   Column physical name.
 * @return  boolean
 */
function o2tab_column_remove($table_name, $column_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->alter_column_remove($column_name);
        }
    else {
        return false;
        }

    }


/**
 * Change a column size in table.
 *
 * @package table
 * @param   string  $table_name    Table name
 * @param   string  $column_name   Column physical name.
 * @param   integer $int           Column new size (alpha) or integer digits (number)
 * @param   integer $dec           Column new decimal digits (only number)
 * @return  boolean
 */
function o2tab_column_resize($table_name, $column_name, $int, $dec = 0) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        $type = false;
        foreach ($table->campi as $fld_name => $fld) {
            if ($fld->phys_name == $column_name) {
                $type = $fld->tipo;
                break;
                }
            }
        if ($type === false) {
            throw new o2_exception("Unknown field <i>".$column_name.
                                   "</i> in table <i>".$table_name.
                                   "</i> requested for resizing",
                                   o2error_DBTABLEREBUILD);
            return false;
            }
        return $table->alter_column_change($column_name, $type, $int, $dec);
        }
    else {
        return false;
        }

    }


/**
 * Rename a column in table.
 *
 * @package table
 * @param   string $table_name    Table name
 * @param   string $column_name   Old column physical name
 * @param   string $type          New column physical name
 * @return  boolean
 */
function o2tab_column_rename($table_name, $old_name, $new_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->alter_column_rename($old_name, $new_name);
        }
    else {
        return false;
        }

    }


/**
 * Add a new index to table.
 * $key_segs is a list of key segments, in the form <field-name> => <direction>,
 * where <direction> is A|D for ascending or descending.
 *
 * @package table
 * @param   string  $table_name   Table name
 * @param   string  $index_name   New index physical name.
 * @param   array   $index_segs   New index segments
 * @param   boolean $unique       New index is unique
 * @return  boolean
 */
function o2tab_index_add($table_name, $index_name, $index_segs, $unique = true) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->alter_index_add($index_name, $index_segs, $unique);
        }
    else {
        return false;
        }

    }


/**
 * Remove an index from table.
 *
 * @package table
 * @param   string  $table_name   Table name
 * @param   string  $index_name   New index physical name.
 * @return  boolean
 */
function o2tab_index_remove($table_name, $index_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->alter_index_remove($index_name);
        }
    else {
        return false;
        }

    }


/**
 * Activates a defined table index by name.
 * Function activates both kind of indexes, from repository and extra indexes.
 *
 * @package table
 * @param   string $table_name
 * @param   string $index_name
 * @return  boolean
 * @see     o2tab_index_setoff(), o2tab_indexes_seton(), o2tab_indexes()
 */
function o2tab_index_seton($table_name, $index_name) {

    $app = $_SESSION['o2_app'];
    if ($table = $app->get_table($table_name)) {
        $db     = $table->db;
        $server = $db->server;
        // ________________________________________________ Physical structure indexes ___
        $phys   = array_keys(o2_gateway::tableindexes($server->type,
                                                      $server->server,
                                                      $server->user,
                                                      $server->password,
                                                      $db->nome,
                                                      $db->proprietario,
                                                      $table->nome));
        // _____________________________________ If index is not in physicals tructure ___
        if (!in_array($index_name, $phys)) {
            $index_segs = array();
            // _______________________________________  Standard index from repository ___
            if (isset($table->chiavi[$index_name])) {
                $unique = true;
                foreach ($table->chiavi[$index_name]->segmenti as $segment) {
                    $index_segs[$segment->campo->phys_name] = $segment->direzione;
                    }
                }
            // __________________________________________________________  Extra index ___
            else {
                $ei_tab          = $app->get_table('o2_extraindexes');
                $ei_db           = $ei_tab->db;
                $ei_server       = $ei_db->server;
                $ei_type         = $ei_server->type;
                $ei_o            = constant("o2_".$ei_type."_o");
                $ei_c            = constant("o2_".$ei_type."_c");
                $fld_idx_name    = $ei_tab->campi["index_name"]->nome_fisico;
                $fld_table_name  = $ei_tab->campi["index_table"]->nome_fisico;
                $fld_unique_name = $ei_tab->campi["index_unique"]->nome_fisico;
                $fld_fields_name = $ei_tab->campi["index_fields"]->nome_fisico;
                $ei_where        = $fld_idx_name."='".$index_name."'";
                $ei_select       = $fld_table_name." AS ".$ei_o."INDEX_TABLE".$ei_c.",".
                                   $fld_unique_name." AS ".$ei_o."INDEX_UNIQUE".$ei_c.",".
                                   $fld_fields_name." AS ".$ei_o."INDEX_FIELDS".$ei_c;
                $eidxs           = o2_gateway::recordset($ei_server->type,
                                                         $ei_server->server,
                                                         $ei_server->user,
                                                         $ei_server->password,
                                                         $ei_db->nome,
                                                         $ei_db->proprietario,
                                                         $ei_tab->nome,
                                                         $ei_tab->nome,
                                                         $ei_select,
                                                         $ei_where,
                                                         '',
                                                         1);
                if (isset($eidxs[0])) {
                    $def = $eidxs[0];
                    if ($def['INDEX_TABLE'] == $table_name) {
                        $unique = ($def['INDEX_UNIQUE'] == '1');
                        foreach (explode(',', $def['INDEX_FIELDS']) as $seg) {
                            list($seg_name, $seg_dir) = explode('|', $seg);
                            $index_segs[$seg_name] = $seg_dir;
                            }
                        }
                    // _____________________________________ ERROR: not matching table ___
                    else {
                        throw new o2_exception("Index <i>".$index_name.
                                               "</i> is not defined in table <i>".
                                               $table_name."</i>",
                                               o2error_DBINDEX);
                        return false;
                        }
                    }
                // ____________________________________________ ERROR: index not found ___
                else {
                    throw new o2_exception("Unknown index <i>".$index_name."</i>",
                                           o2error_DBINDEX);
                    return false;
                    }

                }
            $table->alter_index_add($index_name, $index_segs, $unique);
            return true;
            }
        }
    else {
        return false;
        }

    }


/**
 * Deactivates an active index by name.
 * Function deactivates both kind of indexes, from repository and extra indexes.
 *
 * @package table
 * @param   string $table_name
 * @param   string $index_name
 * @see     o2tab_index_seton(), o2tab_index_setoff(), o2tab_indexes()
 * @return  boolean
 */
function o2tab_index_setoff($table_name, $fkey_name) {

    $app = $_SESSION['o2_app'];
    if ($table = $app->get_table($table_name)) {
        $db     = $table->db;
        $server = $db->server;
        // ________________________________________________ Physical structure indexes ___
        $phys   = array_keys(o2_gateway::tableindexes($server->type,
                                                      $server->server,
                                                      $server->user,
                                                      $server->password,
                                                      $db->nome,
                                                      $db->proprietario,
                                                      $table->nome));
        // _________________________________________ If index is in physicals tructure ___
        if (in_array($index_name, $phys)) {
            return $table->alter_index_remove($index_name);
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
 * Activates all defined indexes for the table and returns the number of activated ones.
 * Function activates both kind of indexes, from repository and extra indexes.
 *
 * @package table
 * @param   string $table_name
 * @return  integer
 * @see     o2tab_indexes_setoff(), o2tab_index_seton(), o2tab_indexes()
 */
function o2tab_indexes_seton($table_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if ($table = $app->get_table($table_name)) {
        $db     = $table->db;
        $server = $db->server;
        // ________________________________________________ Physical structure indexes ___
        $phys   = array_keys(o2_gateway::tableindexes($server->type,
                                                      $server->server,
                                                      $server->user,
                                                      $server->password,
                                                      $db->nome,
                                                      $db->proprietario,
                                                      $table->nome));
        foreach ($table->chiavi as $idx_name => $idx_obj) {
            $index_segs = array();
            // _______________________________________  Standard index from repository ___
            if (!in_array($idx_name, $phys)) {
                foreach ($idx_obj->segmenti as $segment) {
                    $index_segs[$segment->campo->phys_name] = $segment->direzione;
                    }
                $table->alter_index_add($idx_name, $index_segs, true);
                $ret++;
                }
            }
        // ______________________________________________________________  Extra index ___
        $ei_tab          = $app->get_table('o2_extraindexes');
        $ei_db           = $ei_tab->db;
        $ei_server       = $ei_db->server;
        $ei_type         = $ei_server->type;
        $ei_o            = constant("o2_".$ei_type."_o");
        $ei_c            = constant("o2_".$ei_type."_c");
        $fld_idx_name    = $ei_tab->campi["index_name"]->nome_fisico;
        $fld_table_name  = $ei_tab->campi["index_table"]->nome_fisico;
        $fld_unique_name = $ei_tab->campi["index_unique"]->nome_fisico;
        $fld_fields_name = $ei_tab->campi["index_fields"]->nome_fisico;
        $ei_where        = $fld_table_name."='".$table_name."'";
        $ei_select       = $fld_idx_name." AS ".$ei_o."INDEX_NAME".$ei_c.",".
                           $fld_unique_name." AS ".$ei_o."INDEX_UNIQUE".$ei_c.",".
                           $fld_fields_name." AS ".$ei_o."INDEX_FIELDS".$ei_c;
        $eidxs           = o2_gateway::recordset($ei_server->type,
                                                 $ei_server->server,
                                                 $ei_server->user,
                                                 $ei_server->password,
                                                 $ei_db->nome,
                                                 $ei_db->proprietario,
                                                 $ei_tab->nome,
                                                 $ei_tab->nome,
                                                 $ei_select,
                                                 $ei_where,
                                                 '',
                                                 100);
        if (count($eidxs)) {
            foreach ($eidxs as $def) {
                if (!in_array($def['INDEX_NAME'], $phys)) {
                    $index_segs = array();
                    $unique     = ($def['INDEX_UNIQUE'] == '1');
                    foreach (explode(',', $def['INDEX_FIELDS']) as $seg) {
                        list($seg_name, $seg_dir) = explode('|', $seg);
                        $index_segs[$seg_name] = $seg_dir;
                        }
                    $table->alter_index_add($def['INDEX_NAME'], $index_segs, $unique);
                    $ret++;
                    }
                }
            }
        return $ret;
        }
    else {
        return false;
        }

    }


/**
 * Deactivates all active indexes for the table and return the number of deactivated ones.
 * Function deactivates both kind of indexes, from repository and extra indexes.
 *
 * @package table
 * @param   string $table_name
 * @return  integer
 * @see     o2tab_indexes_seton(), o2tab_index_setoff(), o2tab_indexes()
 */
function o2tab_indexes_setoff($table_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if ($table = $app->get_table($table_name)) {
        $db     = $table->db;
        $server = $db->server;
        // ________________________________________________ Physical structure indexes ___
        $phys   = array_keys(o2_gateway::tableindexes($server->type,
                                                      $server->server,
                                                      $server->user,
                                                      $server->password,
                                                      $db->nome,
                                                      $db->proprietario,
                                                      $table->nome));
        foreach ($phys as $index_name) {
            $table->alter_index_remove($index_name);
            $ret++;
            }
        return $ret;
        }
    else {
        return false;
        }

    }


/**
 * Export table data to an o2xml format file
 *
 * @package table
 * @param   string  $table_name
 * @param   string  $file_name
 * @param   boolean $asp_mode
 * @return  boolean
 */
function o2tab_export($table_name, $file_name, $asp_mode = false) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->export($file_name, $asp_mode);
        }
    else {
        return false;
        }

    }


/**
 * Import table data from an o2xml format file
 *
 * @package table
 * @param   string  $table_name
 * @param   string  $file_name
 * @param   boolean $asp_mode
 * @return  boolean
 */
function o2tab_import($table_name, $file_name, $asp_mode = false) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->import($file_name, $asp_mode);
        }
    else {
        return false;
        }

    }


/**
 * Open a window on table content allowing data browsing and modifying.
 * If an index name is passed data will be ordered by the index.
 *
 * @package table
 * @param   string $table_name
 * @param   string $index
 * @return  boolean
 */
function o2tab_browse($table_name, $index = "") {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->data_browser($index);
        }
    else {
        return false;
        }

    }


/**
 * Open a window on table log content, allowing data browsing.
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_browselog($table_name) {

    $table = $_SESSION['o2_app']->get_table($table_name);
    if ($table->log_level) {
        return $table->log_browser();
        }
    else {
        return false;
        }

    }


/**
 * Returns ASP mode for the table if it is active, else false
 *
 * @package table
 * @param   string $table_name
 * @return  mixed
 */
function o2tab_asp($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->asp;
        }
    else {
        return false;
        }

    }


/**
 * Removes from table all existing records with current ASP area code.
 * Table is skipped if ASP mode is not enabled.
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_aspclear($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->asp_clear();
        }
    else {
        return false;
        }

    }


/**
 * Copy table records from current ASP area to $target_asp area.
 * Previously removes all existing records for $target_asp area.
 * Table is skipped if ASP mode is not enabled.
 *
 * @package table
 * @param   string $table_name
 * @param   string $target_asp
 * @return  boolean
 */
function o2tab_aspclone($table_name, $target_asp) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return $table->asp_clone($target_asp);
        }
    else {
        return false;
        }

    }


/**
 * Returns a list of foreign keys active for the table
 *
 * @package table
 * @param   string $table_name
 * @return  array
 * @see     o2tab_fkey_seton(), o2tab_fkey_setoff()
 */
function o2tab_fkeys_list($table_name) {

    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        return o2_gateway::fkeystablist($table->db->server->type,
                                        $table->db->server->server,
                                        $table->db->server->user,
                                        $table->db->server->password,
                                        $table->db->nome,
                                        $table->db->proprietario,
                                        $table->nome);
        }
    else {
        return false;
        }

    }


/**
 * Activates a defined foreign key by name
 *
 * @package table
 * @param   string $table_name
 * @param   string $fkey_name
 * @return  boolean
 * @see     o2tab_fkey_setoff(), o2tab_fkeys_seton(), o2tab_fkeys_list()
 */
function o2tab_fkey_seton($table_name, $fkey_name) {

    $app = $_SESSION['o2_app'];
    if (($table = $app->get_table($table_name)) &&
        $table->exists() &&
        ($fktab = $app->get_table("o2_constraints"))) {
        // ___________________________ Get list of Janox defined constraints for table ___
        $fktabtype     = $fktab->db->server->type;
        $fktab_o       = constant("o2_".$fktabtype."_o");
        $fktab_c       = constant("o2_".$fktabtype."_c");
        $fld_key_name  = $fktab->campi["constraint_name"]->nome_fisico;
        $fld_main_name = $fktab->campi["main_table"]->nome_fisico;
        $fld_link_name = $fktab->campi["linked_table"]->nome_fisico;
        $fld_main_flds = $fktab->campi["main_fields"]->nome_fisico;
        $fld_link_flds = $fktab->campi["linked_fields"]->nome_fisico;
        $where         = $fld_main_name."='".$table_name."'";
        $select_str    = $fld_key_name." AS ".$fktab_o."FKNAME".$fktab_c.", ".
                         $fld_link_name." AS ".$fktab_o."FKLINKNAME".$fktab_c.", ".
                         $fld_main_flds." AS ".$fktab_o."FKMAINFLDS".$fktab_c.", ".
                         $fld_link_flds." AS ".$fktab_o."FKLINKFLDS".$fktab_c;
        $fdeflist      = o2_gateway::recordset($fktabtype,
                                               $fktab->db->server->server,
                                               $fktab->db->server->user,
                                               $fktab->db->server->password,
                                               $fktab->db->nome,
                                               $fktab->db->proprietario,
                                               $fktab->nome,
                                               $fktab->nome,
                                               $select_str,
                                               $where,
                                               false,
                                               100);
        $fdefkeys = array();
        foreach ($fdeflist as $row) {
            $fdefkeys[$row["FKNAME"]] = array("LINKNAME" => $row["FKLINKNAME"],
                                              "MAINFLDS" => $row["FKMAINFLDS"],
                                              "LINKFLDS" => $row["FKLINKFLDS"]);
            }
        // ________________________________ If requested constraint is in defined list ___
        if (array_key_exists($fkey_name, $fdefkeys)) {
            $factkeys = o2_gateway::fkeystablist($table->db->server->type,
                                                 $table->db->server->server,
                                                 $table->db->server->user,
                                                 $table->db->server->password,
                                                 $table->db->nome,
                                                 $table->db->proprietario,
                                                 $table->nome);
            // _________________________ If requested constraint is not already active ___
            if (!in_array($fkey_name, $factkeys)) {
                $fkey = $fdefkeys[$fkey_name];
                if ($reftab = $app->get_table($fkey["LINKNAME"])) {
                    $main_fields = array();
                    $ref_fields  = array();
                    // _________________________ Add O2ASPID if present in both tables ___
                    if ($table->asp == 'C' && $reftab->asp == 'C') {
                        $main_fields[] = "O2ASPID";
                        $ref_fields[]  = "O2ASPID";
                        }
                    // ____________________________________ Internal constraint fields ___
                    foreach (explode(",", $fkey["MAINFLDS"]) as $field) {
                        $main_fields[] = $table->campi[$field]->phys_name;
                        }
                    // ____________________________________ External referenced fields ___
                    foreach (explode(",", $fkey["LINKFLDS"]) as $field) {
                        $ref_fields[] = $reftab->campi[$field]->phys_name;
                        }
                    return o2_gateway::fkeyadd($table->db->server->type,
                                               $table->db->server->server,
                                               $table->db->server->user,
                                               $table->db->server->password,
                                               $table->db->nome,
                                               $table->db->proprietario,
                                               $table->nome,
                                               $main_fields,
                                               $reftab->db->nome,
                                               $reftab->db->proprietario,
                                               $reftab->nome,
                                               $ref_fields,
                                               $fkey_name);
                    }
                else {
                    throw new o2_exception("Malformed constraint <i>".$fkey_name.
                                           "</i> in table <i>".$table_name.
                                           "</i>: unknown referenced table <i>".
                                           $fkey["LINKNAME"]."</i>",
                                           o2error_DBFKEY);
                    return false;
                    }
                }
            else {
                return true;
                }
            }
        else  {
            throw new o2_exception("Unknown constraint <i>".$fkey_name.
                                   "</i> in table <i>".$table_name."</i>",
                                   o2error_DBFKEY);
            return false;
            }
        }
    else {
        return false;
        }

    }


/**
 * Deactivates an active foreign key by name
 *
 * @package table
 * @param   string $table_name
 * @param   string $fkey_name
 * @see     o2tab_fkey_seton(), o2tab_fkeys_setoff(), o2tab_fkeys_list()
 * @return  boolean
 */
function o2tab_fkey_setoff($table_name, $fkey_name) {

    if (($table = $_SESSION['o2_app']->get_table($table_name)) && $table->exists()) {
        $fkeys = o2_gateway::fkeystablist($table->db->server->type,
                                          $table->db->server->server,
                                          $table->db->server->user,
                                          $table->db->server->password,
                                          $table->db->nome,
                                          $table->db->proprietario,
                                          $table->nome);
        if (in_array($fkey_name, $fkeys)) {
            return o2_gateway::fkeyremove($table->db->server->type,
                                          $table->db->server->server,
                                          $table->db->server->user,
                                          $table->db->server->password,
                                          $table->db->nome,
                                          $table->db->proprietario,
                                          $table->nome,
                                          $fkey_name);
            }
        else {
            return true;
            }
        }
    else {
        return false;
        }

    }


/**
 * Activates all defined foreign keys for the table and returns the number of activated
 * ones
 *
 * @package table
 * @param   string $table_name
 * @return  integer
 * @see     o2tab_fkeys_setoff(), o2tab_fkey_seton(), o2tab_fkeys_list()
 */
function o2tab_fkeys_seton($table_name) {

    $app = $_SESSION['o2_app'];
    $ret = 0;
    if (($table = $app->get_table($table_name)) &&
        $table->exists() &&
        ($fktab = $app->get_table("o2_constraints"))) {
        // ___________________________ Get list of Janox defined constraints for table ___
        $fktabtype     = $fktab->db->server->type;
        $fktab_o       = constant("o2_".$fktabtype."_o");
        $fktab_c       = constant("o2_".$fktabtype."_c");
        $fld_key_name  = $fktab->campi["constraint_name"]->nome_fisico;
        $fld_main_name = $fktab->campi["main_table"]->nome_fisico;
        $fld_link_name = $fktab->campi["linked_table"]->nome_fisico;
        $fld_main_flds = $fktab->campi["main_fields"]->nome_fisico;
        $fld_link_flds = $fktab->campi["linked_fields"]->nome_fisico;
        $where         = $fld_main_name."='".$table_name."'";
        $select_str    = $fld_key_name." AS ".$fktab_o."FKNAME".$fktab_c.", ".
                         $fld_link_name." AS ".$fktab_o."FKLINKNAME".$fktab_c.", ".
                         $fld_main_flds." AS ".$fktab_o."FKMAINFLDS".$fktab_c.", ".
                         $fld_link_flds." AS ".$fktab_o."FKLINKFLDS".$fktab_c;
        $fdeflist      = o2_gateway::recordset($fktabtype,
                                               $fktab->db->server->server,
                                               $fktab->db->server->user,
                                               $fktab->db->server->password,
                                               $fktab->db->nome,
                                               $fktab->db->proprietario,
                                               $fktab->nome,
                                               $fktab->nome,
                                               $select_str,
                                               $where,
                                               false,
                                               100);
        // ____________________________ Get list of active constraints for table on db ___
        $factkeys = o2_gateway::fkeystablist($table->db->server->type,
                                             $table->db->server->server,
                                             $table->db->server->user,
                                             $table->db->server->password,
                                             $table->db->nome,
                                             $table->db->proprietario,
                                             $table->nome);
        // _______________________________________ Loop on constraints in defined list ___
        foreach ($fdeflist as $row) {
            // _________________________ If requested constraint is not already active ___
            if (!in_array($row["FKNAME"], $factkeys)) {
                $fkey = $fdefkeys[$fkey_name];
                if ($reftab = $app->get_table($row["FKLINKNAME"])) {
                    $main_fields = array();
                    $ref_fields  = array();
                    // _________________________ Add O2ASPID if present in both tables ___
                    if ($table->asp == 'C' && $reftab->asp == 'C') {
                        $main_fields[] = "O2ASPID";
                        $ref_fields[]  = "O2ASPID";
                        }
                    // ____________________________________ Internal constraint fields ___
                    foreach (explode(",", $row["FKMAINFLDS"]) as $field) {
                        $main_fields[] = $table->campi[$field]->phys_name;
                        }
                    // ____________________________________ External referenced fields ___
                    foreach (explode(",", $row["FKLINKFLDS"]) as $field) {
                        $ref_fields[] = $reftab->campi[$field]->phys_name;
                        }
                    if (o2_gateway::fkeyadd($table->db->server->type,
                                            $table->db->server->server,
                                            $table->db->server->user,
                                            $table->db->server->password,
                                            $table->db->nome,
                                            $table->db->proprietario,
                                            $table->nome,
                                            $main_fields,
                                            $reftab->db->nome,
                                            $reftab->db->proprietario,
                                            $reftab->nome,
                                            $ref_fields,
                                            $row["FKNAME"])) {
                        $ret++;
                        }
                    }
                else {
                    throw new o2_exception("Malformed constraint <i>".$fkey_name.
                                           "</i> in table <i>".$table_name.
                                           "</i>: unknown referenced table <i>".
                                           $row["FKLINKNAME"]."</i>",
                                           o2error_DBFKEY);
                    }
                }
            }
        return $ret;
        }
    else {
        return false;
        }

    }


/**
 * Deactivates all active foreign keys for the table and return the number of deactivated
 * ones
 *
 * @package table
 * @param   string $table_name
 * @return  integer
 * @see     o2tab_fkeys_seton(), o2tab_fkey_setoff(), o2tab_fkeys_list()
 */
function o2tab_fkeys_setoff($table_name) {

    $ret = 0;
    if ($table = $_SESSION['o2_app']->get_table($table_name)) {
        if ($table->exists()) {
            // _____________________________ Get list of active foreign keys for table ___
            $fkeys = o2_gateway::fkeystablist($table->db->server->type,
                                              $table->db->server->server,
                                              $table->db->server->user,
                                              $table->db->server->password,
                                              $table->db->nome,
                                              $table->db->proprietario,
                                              $table->nome);
            // ___________________________________________ Loop on active foreign keys ___
            foreach ($fkeys as $fkey_name) {
                o2_gateway::fkeyremove($table->db->server->type,
                                       $table->db->server->server,
                                       $table->db->server->user,
                                       $table->db->server->password,
                                       $table->db->nome,
                                       $table->db->proprietario,
                                       $table->nome,
                                       $fkey_name);
                $ret++;
                }
            }
        return $ret;
        }
    else {
        return false;
        }

    }


// =================================== VIEW TABLE ========================================

/**
 * Returns true if table used in view physically exists, else false
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @return  boolean
 */
function o2viewtab_exists($view_name, $table_alias) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->exists();
        }
    else {
        return false;
        }

    }


/**
 * Returns true if table contains any data, else false
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @return  boolean
 */
function o2viewtab_data($view_name, $table_alias) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->data();
        }
    else {
        return false;
        }

    }


/**
 * Creates table used in a view
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @return  boolean
 */
function o2viewtab_create($view_name, $table_alias) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->crea();
        }
    else {
        return false;
        }

    }


/**
 * Drops the table
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @return  boolean
 */
function o2viewtab_drop($view_name, $table_alias) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->elimina();
        }
    else {
        return false;
        }

    }


/**
 * Returns physical informations as an array for a table used in a view
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @return  boolean
 */
function o2viewtab_info($view_name, $table_alias) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->info(true, true);
        }
    else {
        return false;
        }

    }


/**
 * Rebuilds table used in a view, eliminating differences between o2 structure and
 * physical one
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @param   array  $matches
 * @return  boolean
 */
function o2viewtab_rebuild($view_name, $table_alias, $matches = null) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->ricostruisci($matches);
        }
    else {
        return false;
        }

    }


/**
 * Physically renames table used in a view
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @return  boolean
 */
function o2viewtab_rename($view_name, $table_alias, $new_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->rinomina($new_name);
        }
    else {
        return false;
        }

    }


/**
 * Export table data to an o2xml format file
 *
 * @package view-table
 * @param   string  $view_name
 * @param   string  $table_alias
 * @param   string  $file_name
 * @param   boolean $asp_mode
 * @return  boolean
 */
function o2viewtab_export($view_name, $table_alias, $file_name, $asp_mode = false) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->export($file_name, $asp_mode);
        }
    else {
        return false;
        }

    }


/**
 * Import data from an o2xml format file to a table defined in a view
 *
 * @package view-table
 * @param   string  $view_name
 * @param   string  $table_alias
 * @param   string  $file_name
 * @param   boolean $asp_mode
 * @return  boolean
 */
function o2viewtab_import($view_name, $table_alias, $file_name, $asp_mode = false) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->import($file_name, $asp_mode);
        }
    else {
        return false;
        }

    }


/**
 * Open a window on the content of a table defined in a view, allowing data browsing and
 * modifying.
 * If an index name is passed data will be ordered by the index.
 *
 * @package view-table
 * @param   string  $view_name
 * @param   string  $table_alias
 * @param   string $index
 * @return  boolean
 */
function o2viewtab_browse($view_name, $table_alias, $index = "") {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->data_browser($index);
        }
    else {
        return false;
        }

    }


/**
 * Removes all existing records with current ASP area code from table used in a view.
 * Table is skipped if ASP mode is not enabled.
 *
 * @package view-table
 * @param   string  $view_name
 * @param   string  $table_alias
 * @return  boolean
 */
function o2viewtab_aspclear($view_name, $table_alias) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->asp_clear();
        }
    else {
        return false;
        }

    }


/**
 * Copy records from current ASP area to $target_asp area for a table used in a view.
 * Previously removes all existing records for $target_asp area.
 * Table is skipped if ASP mode is not enabled.
 *
 * @package view-table
 * @param   string $view_name
 * @param   string $table_alias
 * @param   string $target_asp
 * @return  boolean
 */
function o2viewtab_aspclone($view_name, $table_alias, $target_asp) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($table = $view_obj->files[$table_alias]) {
        return $table->asp_clone($target_asp);
        }
    else {
        return false;
        }

    }


// ====================================== VIEW ===========================================

/**
 * Returns true if view current record has been modified
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 */
function o2view_mod($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        return $view_obj->modificato;
        }
    else {
        return false;
        }

    }


/**
 * Returns view mode flag.
 *
 * Flag can be [I]nsert, [D]elete or [M]odify.
 * Modify mode is the standard mode for a record after selection.
 * Insert mode is after a prepare-insert action, until next post-row or record change.
 * Delete mode is rarely detected and it is set after e delete action on record.
 *
 * This function is mostly useful to detect [I]nsert mode status.
 *
 * @package view
 * @param   string $view_name
 * @return  string
 */
function o2view_status($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        return $view_obj->status;
        }
    else {
        return false;
        }

    }


/**
 * Returns TRUE if $view_name is in insert mode, else FALSE.
 * This function is equivalent to expression:
 *
 *  o2view_status($view_name) == "I"
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_status(), o2view_mod()
 */
function o2view_ins($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        return ($view_obj->status == "I");
        }
    else {
        return false;
        }

    }


/**
 * Returns view start record offset
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 */
function o2view_start($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        if (!$view_obj->vista) {
            $view_obj->record_primo();
            }
        return $view_obj->offset_ini;
        }
    else {
        return false;
        }

    }


/**
 * Returns view end record offset
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 */
function o2view_end($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        if (!$view_obj->vista) {
            $view_obj->record_primo();
            }
        return $view_obj->offset_fine;
        }
    else {
        return false;
        }

    }


/**
 * Returns number or requested records for actual paging.
 * Requested records number is set by the view property "Requested rows number".
 * Note: requested records number can differ from actually returned records number, when
 * recordset end is reached.
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 * @see     o2view_retrows(), o2view_total()
 */
function o2view_reqrows($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        return $view_obj->righe_vis;
        }
    else {
        return false;
        }

    }


/**
 * Returns number of returned records in the actual dataset page.
 * Returned rows number is equal to requested rows number, until the recordset end is
 * reached.
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 * @see     o2view_reqrows(), o2view_total(), o2view_rec()
 */
function o2view_retrows($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        if (!$view_obj->vista) {
            $view_obj->record_primo();
            }
        return count($view_obj->recordset);
        }
    else {
        return false;
        }

    }


/**
 * Returns total number of records in the view dataset.
 * This is the function to use to get the actual view records number, regardless of
 * paging, offsets and cursor position.
 *
 * If parameter $from_cache is not passed or it is passed as FALSE total records number
 * will be forced to be recounted.
 * If $from_cache parameter is passed as TRUE an extra check will be performed on dataset
 * to verify if it has changed from last count.
 * The $from_cache parameter is mostly intended to be used as TRUE in UI expression, i.e.
 * in a grid footer cell value.
 *
 * @package view
 * @param   string  $view_name
 * @param   boolean $from_cache
 * @return  integer
 * @see     o2view_retrows(), o2view_rec()
 */
function o2view_total($view_name, $from_cache = false) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        // _____________________________________ Custom FROM (table name or SQL query) ___
        if ($view_obj->custom_from) {
            // ________________________ Needed to fix defined check-box view-selectors ___
            if (count($view_obj->formule)) {
                foreach ($view_obj->formule as $formula) {
                    if ($formula->selector) {
                        $view_obj->vista = false;
                        break;
                        }
                    }
                }
            $view_obj->struttura();
            }
        $changed = false;
        if (!$view_obj->vista) {
            $view_obj->record_primo();
            $changed = true;
            }
        elseif ($view_obj->status != "I") {
            $range_mod = $view_obj->range_mod();
            if ($range_mod) {
                $view_obj->vista     = false;
                $view_obj->direzione = 0;
                $view_obj->record_primo();
                }
            if ($from_cache) {
                $changed = $range_mod || ($view_obj->totale_record == 101);
                }
            else {
                $changed = true;
                }
            }
        if ($changed) {
            // ____________________________________________ Check if view is COUNTABLE ___
            $countable = true;
            // __________________________________________________ Check ranges on link ___
            foreach ($view_obj->esclusivi as $file_name => $per_file) {
                if ($per_file[0] &&
                    $file_name != $view_obj->file->indice &&
                    !$view_obj->files[$file_name]->link_by_sql) {
                    $countable = false;
                    break;
                    }
                }
            // ______________________________________________ Check ranges on formulas ___
            if ($view_obj->wheres["o2calc§_§exps"]) {
                $countable = false;
                }
            // ________________________________________ Use database COUNT (COUNTABLE) ___
            if ($countable) {
                $view_obj->totale_record = $view_obj->totale_rec();
                }
            // ________________________________________ Use loop on view (UNCOUNTABLE) ___
            else {
                $view_obj->totale_record = $view_obj->count_loop();
                }
            }
        return $view_obj->totale_record;
        }
    else {
        return 0;
        }

    }


/**
 * Return TRUE if view has a current record, else FALSE.
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_total(), o2view_retrows()
 */
function o2view_rec($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        // __________________________________________ Used to update range expressions ___
        foreach ($view_obj->campi as $field) {
            if ($field->file == $view_obj->file->indice) {
                break;
                }
            }
        $view_obj->campo($field->nome);
        return (count($view_obj->recordset) > 0);
/*
        return (($view_obj->campo($field->nome) !== $field->maschera->default) ||
                (count($view_obj->recordset) > 0));
*/
        }
    else {
        return false;
        }

    }


/**
 * Returns the current record as an array, or FALSE if there's no current record
 *
 * @package view
 * @param   string $view_name
 * @return  array
 */
function o2view_row($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj && count($view_obj->recordset)) {
        $ret = array();
        if (count($view_obj->campi)) {
            foreach ($view_obj->campi as $field) {
                $ret[$field->nome] = $view_obj->campo($field->nome);
                }
            }
        if (count($view_obj->formule)) {
            foreach ($view_obj->formule as $formula) {
                $ret[$formula->nome] = $view_obj->corrente[$formula->nome];
                }
            }
        return $ret;
        }
    else {
        return false;
        }

    }


/**
 * Returns actual view selection [from 0 to o2view_retrows() - 1]
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 */
function o2view_select($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        if (!$view_obj->vista) {
            $view_obj->record_primo();
            }
        return $view_obj->selezione;
        }
    else {
        return false;
        }

    }


/**
 * Export data from a prg view to a CSV format file.
 * File is zipped and the full path to file is returned.
 *
 * @param  string  $view_name           Name of the view to export data from
 * @param  string  $csv_file_name       Target file name
 * @param  boolean $only_visible        Export only data visible on prg forms
 * @param  string  $fields_separator    Character use to separate fields
 * @param  string  $text_delimiter      Character used to delimiter text values
 * @param  string  $decimal_separator   Character used as decimal point in numbers
 * @param  string  $date_format         Format used for data fields (PHP date() style)
 * @param  string  $time_format         Format used for time fields (PHP date() style)
 * @param  boolean $titles_from_view    If TRUE field alias in view is used as title
 * @param  array   $excluded_aliases    List of aliases excluded in export
 * @param  array   $used_aliases        List of (alias => column-title) to override all
 * @return string
 */
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

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (is_array($excluded_aliases)) {
        $exclude = array();
        foreach ($excluded_aliases as $alias) {
            $exclude[] = strtoupper($alias);
            }
        }
    else {
        $exclude = false;
        }
    if (is_array($used_aliases)) {
        $used = array();
        foreach ($used_aliases as $alias => $title) {
            $used[strtoupper($alias)] = $title;
            }
        }
    else {
        $used = false;
        }
    return $prg->export_data($view_name,
                             $csv_file_name,
                             $only_visible,
                             $fields_separator,
                             $text_delimiter,
                             $decimal_separator,
                             $date_format,
                             $time_format,
                             $titles_from_view,
                             $exclude,
                             $used);

    }


/**
 * This function returns SQL query code generated by the view $view_name.
 *
 * By default ORDER BY and LIMIT clauses are excluded from query: if you need to uinclude
 * them too use specific parameters.
 *
 * NOTE: Links unresolved as JOIN and calculated (formula) fields will not be included in
 *       SQL query code.
 *
 * @package view
 * @param   string  $view_name
 * @param   boolean $with_orderby
 * @param   boolean $with_limit
 * @return  string
 */
function o2view_get_query($view_name, $with_orderby = false, $with_limit = false) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        $GLOBALS['jxviewsql'] = 'Q'.($with_orderby ? 'O' : '').($with_limit ? 'L' : '');
        $view_obj->record_primo();
        $query = $GLOBALS['jxviewsql'];
        unset($GLOBALS['jxviewsql']);
        $view_obj->record_primo();
        return $query;
        }
    else {
        return false;
        }

    }


/**
 * === DEPRECATED ===
 * This function has been deprecated: use recordset-DeleteAll instruction instead.
 *
 * Delete all selected records in view
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 */
function o2view_delete($view_name) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        return $view_obj->delete_recordset();
        }
    else {
        return false;
        }

    }


/**
 * Execute an SQL INSERT-INTO, from the given view to the target table.
 * All aliases of fields selected in the source view must match the name of a field in the
 * target table.
 * This method returns the number of inserted rows, or FALSE on failure.
 *
 * @package view
 * @param   string $view_name
 * @param   string $target_table
 * @return  integer
 */
function o2view_insertinto($view_name, $target_table) {

    $app      = $_SESSION['o2_app'];
    $view_obj = $app->istanze_prg[$app->progressivo_istanze]->contesto[$view_name];
    if ($view_obj) {
        return $view_obj->insert_into($target_table);
        }
    else {
        return false;
        }

    }


/**
 * Returns row selection in a "view-selector" structure.
 * This function is intended to be used in the expression of a view calculated (formula)
 * field.
 * This way calculated field turns into a "view-selector" and can be used as field for a
 * checkbox control in a grid.
 * View-selector checkbox works as a view-selector, regardless of grid is enabled or not.
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_selector_all(), o2view_selector_none()
 */
function o2view_selector($view_name) {

    $app                 = $_SESSION['o2_app'];
    $prg                 = $app->istanze_prg[$app->progressivo_istanze];
    $view                = $prg->contesto[$view_name];
    $item                = $view->chiave_corrente($view->file,
                                                  false,
                                                  false,
                                                  false,
                                                  true,
                                                  true,
                                                  true);
    $app->defining_check = $view_name;
    return in_array($item, $view->selections);

    }


/**
 * Select all (filtered) rows of a view in a view-selector structure.
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_selector(), o2view_selector_none()
 */
function o2view_selector_all($view_name) {

    $app   = $_SESSION['o2_app'];
    $prg   = $app->istanze_prg[$app->progressivo_istanze];
    $view  = $prg->contesto[$view_name];
    $cview = clone $view;
    $cview->record_primo();
    $view->selections = array();
    // ____________________________________________________ Action loop on cloned view ___
    do {
        $view->selections[] = $cview->chiave_corrente($cview->file,
                                                      false,
                                                      false,
                                                      false,
                                                      true,
                                                      true,
                                                      true);
        $go_on = $cview->record_avanti();
        } while ($go_on);
    unset($cview);
    $view->smart_refresh();
    return true;

    }


/**
 * Unselect all (filtered) rows of a view in a view-selector structure.
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_selector(), o2view_selector_all()
 */
function o2view_selector_none($view_name) {

    $app              = $_SESSION['o2_app'];
    $prg              = $app->istanze_prg[$app->progressivo_istanze];
    $view             = $prg->contesto[$view_name];
    $view->selections = array();
    $view->smart_refresh();
    return true;

    }


/**
 * Select or deselect current row of a view in a view-selector structure.
 * If parameter $on evaluates to TRUE then current row is selected, else it is unselected.
 *
 * @package view
 * @param   string  $view_name
 * @param   boolean $on
 * @return  boolean
 * @see     o2view_selector(), o2view_selector_all(), o2view_selector_none()
 */
function o2view_selector_set($view_name, $on = true) {

    $app  = $_SESSION['o2_app'];
    $prg  = $app->istanze_prg[$app->progressivo_istanze];
    $view = $prg->contesto[$view_name];
    $item = $view->chiave_corrente($view->file,
                                   false,
                                   false,
                                   false,
                                   true,
                                   true,
                                   true);
    if ($on) {
        $old_value = true;
        if (!in_array($item, $view->selections)) {
            $old_value          = false;
            $view->selections[] = $item;
            }
        }
    else {
        $old_value = false;
        if (($key = array_search($item, $view_obj->selections)) !== false) {
            unset($view->selections[$key]);
            $old_value = true;
            }
        }
    return $old_value;

    }


/**
 * Returns list of selected records by primary key.
 *
 * @package view
 * @param   string  $view_name
 * @return  array
 * @see     o2view_selector(), o2view_selector_set()
 */
function o2view_selector_get_selected($view_name) {

    $app    = $_SESSION['o2_app'];
    $prg    = $app->istanze_prg[$app->progressivo_istanze];
    $view   = $prg->contesto[$view_name];
    $file   = $view->file->indice;
    $list   = array();
    $fields = array();
    foreach ($view->campi as $field_obj) {
        if ($field_obj->file == $file) {
            $fields[$field_obj->phys_name] = $field_obj->nome;
            }
        }
    foreach ($view->selections as $str_key) {
        $segs = array();
        foreach (explode('AND', $str_key) as $seg) {
            list($field, $value) = explode('=', $seg);
            $segs[$fields[trim($field, ' "[]`')]] = trim($value, "'");
            }
        $list[] = $segs;
        }
    return $list;

    }

// =============================== TRANSACTION ===========================================


/**
 * Commits all active transactions on all used servers, where transactions started, for
 * the current transaction isolation level.
 *
 * @package transaction
 * @return  boolean
 * @see     o2transaction_isolate(), o2transaction_rollback()
 */
function o2transaction_commit() {

    $_SESSION['o2_app']->commit_all(false);
    return true;

    }


/**
 * Rollsback all active transactions on all used servers, where transactions started, for
 * the current transaction isolation level.
 *
 * @package transaction
 * @return  boolean
 * @see     o2transaction_isolate(), o2transaction_commit()
 */
function o2transaction_rollback() {

    $app = $_SESSION['o2_app'];
    foreach ($app->server as $server) {
        $engine = $server->type;
        // _____________________________ If an active transaction is found on a server ___
        if (isset($GLOBALS['o2_'.$engine.'_trans'])) {
            $keys = array_keys($GLOBALS['o2_'.$engine.'_trans']);
            $key  = $keys[count($keys) - 1];
            if (!is_array($GLOBALS['o2_'.$engine.'_error'])) {
                $GLOBALS['o2_'.$engine.'_error'] = array();
                }
            $GLOBALS['o2_'.$engine.'_error'][$key] = true;
            o2_gateway::commit($engine,
                               $server->server,
                               $server->user,
                               $server->password);
            }
        }
    // _________________________________________________________ Reset database errors ___
    unset($GLOBALS["jxlasterror"]);
    return true;

    }


/**
 * Isolates queries for next transaction. Isolation level will be set off at program end.
 * Function returns TRUE if isolation mode is enabled, FALSE if isolation mode was
 * previously activated.
 *
 * @package transaction
 * @return  boolean
 * @see     o2dbs_commit(), o2dbs_engine(), o2dbs_execute()
 */
function o2transaction_isolate() {

    $app = $_SESSION['o2_app'];
    if ($app->isolated_trans === false) {
        $app->isolated_trans = $app->progressivo_istanze;
        }
    else {
        return false;
        }
    return true;

    }


// ================================ ACTION ===============================================

/**
 * Rerurns loop counter for a running action
 *
 * @package action
 * @param   string $action_name
 * @return  integer
 */
function o2act_counter($action_name = "") {

    return $_SESSION['o2_app']->action_counter($action_name);

    }

// ================================= FORM ================================================

/**
 * Rerurns form X position in px
 *
 * @package form
 * @param   string  $form_name
 * @param   integer $level
 * @return  integer
 */
function o2form_x($form_name, $level = 0) {

    $app       = $_SESSION['o2_app'];
    $form_name = trim($form_name)."_form";
    if (!$level) {
        $level = $app->progressivo_istanze;
        }
    if (isset($app->istanze_prg[$level]->form[$form_name])) {
        $jxform = $app->istanze_prg[$level]->form[$form_name];
        return $jxform->absolute_x;
        }
    else {
        throw new o2_exception("Unknown form <i>".$form_name."</i>",
                               o2error_UNKNOWNFORM);
        }

    }


/**
 * Rerurns form Y position in px
 *
 * @package form
 * @param   string  $form_name
 * @param   integer $level
 * @return  integer
 */
function o2form_y($form_name, $level = 0) {

    $app       = $_SESSION['o2_app'];
    $form_name = trim($form_name)."_form";
    if (!$level) {
        $level = $app->progressivo_istanze;
        }
    if (isset($app->istanze_prg[$level]->form[$form_name])) {
        $jxform = $app->istanze_prg[$level]->form[$form_name];
        return $jxform->absolute_y;
        }
    else {
        throw new o2_exception("Unknown form <i>".$form_name."</i>",
                               o2error_UNKNOWNFORM);
        }

    }


/**
 * Rerurns form width in px
 *
 * @package form
 * @param   string  $form_name
 * @param   integer $level
 * @return  integer
 */
function o2form_width($form_name, $level = 0) {

    $app       = $_SESSION['o2_app'];
    $form_name = trim($form_name)."_form";
    if (!$level) {
        $level = $app->progressivo_istanze;
        }
    if (isset($app->istanze_prg[$level]->form[$form_name])) {
        return $app->istanze_prg[$level]->form[$form_name]->larghezza;
        }
    else {
        throw new o2_exception("Unknown form <i>".$form_name."</i>",
                               o2error_UNKNOWNFORM);
        }

    }


/**
 * Rerurns form height in px
 *
 * @package form
 * @param   string  $form_name
 * @param   integer $level
 * @return  integer
 */
function o2form_height($form_name, $level = 0) {

    $app       = $_SESSION['o2_app'];
    $form_name = trim($form_name)."_form";
    if (!$level) {
        $level = $app->progressivo_istanze;
        }
    if (isset($app->istanze_prg[$level]->form[$form_name])) {
        return $app->istanze_prg[$level]->form[$form_name]->altezza;
        }
    else {
        throw new o2_exception("Unknown form <i>".$form_name."</i>",
                               o2error_UNKNOWNFORM);
        }

    }


/**
 * Maximize or restore form size.
 * When maximized the form takes all the available space on client browser.
 * When $maximized parameter is passad as TRUE (default) form is maximized, if not already
 * maximized.
 * When parameter $maximized is passed as FALSE, if form is maximized, then it is reset to
 * its original size and position.
 *
 * @package form
 * @param   string  $form_name
 * @param   boolean $maximized
 * @return  boolean
 */
function o2form_maximize($form_name = "", $maximized = true) {

    $app       = $_SESSION['o2_app'];
    $form_name = trim($form_name)."_form";
    $prg       = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->form[$form_name])) {
        $form = $prg->form[$form_name];
        if (($maximized && !$form->maximized) ||
            (!$maximized && $form->maximized)) {
            $form->max_restore();
            }
        return true;
        }
    else {
        throw new o2_exception("Unknown form <i>".$form_name."</i>",
                               o2error_UNKNOWNFORM);
        }

    }


// ========================================= DIR =========================================

/**
 * Returns complete names list of files of a file system folder. Accepts a
 * standard wildcards expression as second parameter.
 *
 * @package filesystem
 * @param   string $dir_path
 * @param   string $file_match
 * @return  array
 */
function o2dir_list($dir_path, $file_match = "*") {

    $dir_obj_local = new o2_dir($dir_path.DIRECTORY_SEPARATOR, $file_match);
    $list_local    = array();
    foreach ($dir_obj_local->all_elements() as $single_file) {
        $list_local[] = $single_file->nome_completo;
        }
    return $list_local;

    }


/**
 * Creates directory if it does not exist
 *
 * @package filesystem
 * @param   string $dir_path
 * @return  boolean
 */
function o2dir_create($dir_path) {

    $dir_obj_local = new o2_dir($dir_path.DIRECTORY_SEPARATOR);
    return $dir_obj_local->crea();

    }


/**
 * Removes a folder and all its content
 *
 * @package filesystem
 * @param   string $dir_path
 * @return  boolean
 */
function o2dir_delete($dir_path) {

    $dir_obj_local = new o2_dir($dir_path.DIRECTORY_SEPARATOR);
    return $dir_obj_local->remove();

    }


// ========================================= FILE ========================================

/**
 * Returns the rightly separated absolute path to file, with relatives paths
 * resolved
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_fullname($file_path) {

    $file_local = new o2_fsitem($file_path);
    return $file_local->nome_completo;

    }


/**
 * Returns file name and extension
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_basename($file_path) {

    $file_local = new o2_fsitem($file_path);
    return $file_local->nome;

    }


/**
 * Returns file extension without dot (.)
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_ext($file_path) {

    $file_local = new o2_fsitem($file_path);
    return $file_local->ext;

    }


/**
 * Returns path to the folder containing file
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_dir($file_path) {

    $file_local = new o2_fsitem($file_path);
    return $file_local->path;

    }


/**
 * Returns file type ([D]irectory - [F]file)
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_type($file_path) {

    $file_local = new o2_fsitem($file_path);
    return $file_local->tipo;

    }


/**
 * Returns full path to icon file according with extension.
 * If $large is passed as TRUE PNG128 image is returned, PNG16 otherwise.
 * If $internal_use parameter is passed as TRUE path is returned for using icon in Janox
 * controls (image control source file).
 *
 * @package filesystem
 * @param   string  $file_path      Path to file you want to set icon for
 * @param   boolean $large          16x16(FALSE) or 128x128(TRUE) images
 * @param   boolean $internal_use   Path to use within Janox controls
 * @return  string
 */
function o2file_ico($file_path, $large = false, $internal_use = false) {

    $file_local = new o2_fsitem($file_path);
    if ($file_local->tipo == "D") {
        $ico_file = "folder.png";
        }
    else {
        switch (strtolower($file_local->ext)) {
            case "xls":
            case "ods":
                $ico_file = "excel.png";
                break;
            case "doc":
            case "odt":
                $ico_file = "word.png";
                break;
            case "txt":
            case "ini":
            case "conf":
                $ico_file = "text.png";
                break;
            case "exe":
            case "bin":
            case "com":
            case "cmd":
            case "bat":
            case "sh":
            case "so":
            case "dll":
                $ico_file = "exe.png";
                break;
            case "chm":
            case "hlp":
                $ico_file = "help.png";
                break;
            case "mp3":
            case "wav":
            case "flac":
            case "ape":
            case "ogg":
            case "mid":
                $ico_file = "sound.png";
                break;
            case "xml":
            case "xhtml":
                $ico_file = "xml.png";
                break;
            case "avi":
            case "mpg":
            case "mpeg":
            case "wmv":
                $ico_file = "movie.png";
                break;
            case "htm":
            case "html":
                $ico_file = "html.png";
                break;
            case "img":
            case "png":
            case "gif":
            case "jpg":
            case "tif":
            case "ico":
                $ico_file = "img.png";
                break;
            case "pdf":
                $ico_file = "pdf.png";
                break;
            case "zip":
            case "rar":
            case "tar":
            case "gz":
            case "bz":
            case "deb":
            case "7z":
                $ico_file = "zip.png";
                break;
            case "php":
            case "inc":
                $ico_file = "php.png";
                break;
            case "prf":
                $ico_file = "o2prf.png";
                break;
            case "prg":
                $ico_file = "o2prg.png";
                break;
            default:
                $ico_file = "unknown.png";
                break;
            }
        }
    if ($large) {
        $ico_file = "x".$ico_file;
        }
    if ($internal_use) {
        $ico_file = "<jx>/img/fs/".$ico_file;
        }
    else {
        $ico_file = $GLOBALS['o2_runtime']->alias."img/fs/".$ico_file;
        }
    return $ico_file;

    }


/**
 * Returns file size in bytes
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_size($file_path) {

    $file_local = new o2_fsitem($file_path);
    clearstatcache();
    return $file_local->dimensioni;

    }


/**
 * Returns file last modification date as o2-date
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_date($file_path) {

    $file_local = new o2_fsitem($file_path);
    clearstatcache();
    return $file_local->data_modifica;

    }


/**
 * Returns file last modification time as o2-time
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_time($file_path) {

    $file_local = new o2_fsitem($file_path);
    clearstatcache();
    return $file_local->ora_modifica;

    }


/**
 * Returns true if file exists, else false
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_exists($file_path) {

    $folder_local = new o2_dir(dirname($file_path).DIRECTORY_SEPARATOR);
    clearstatcache();
    return $folder_local->exists($file_path);

    }


/**
 * Physically removes file from file system
 *
 * @package filesystem
 * @param   string $file_path
 * @return  boolean
 */
function o2file_delete($file_path) {

    $folder_local = new o2_dir(dirname($file_path).DIRECTORY_SEPARATOR);
    $file_local   = $folder_local->element($file_path);
    clearstatcache();
    if ($folder_local->exists($file_local->nome_completo)) {
        return unlink($file_local->nome_completo);
        }
    else {
        return true;
        }

    }


/**
 * Renames file
 *
 * @package filesystem
 * @param   string $file_path
 * @param   string $new_name
 * @return  boolean
 */
function o2file_rename($file_path, $new_name) {

    $folder_local = new o2_dir(dirname($file_path).DIRECTORY_SEPARATOR);
    $file_local   = $folder_local->element($file_path);
    clearstatcache();
    if ($folder_local->exists($file_local->nome_completo) &&
       !$folder_local->exists($new_name)) {
        return rename($file_local->nome_completo, $file_local->path.basename($new_name));
        }
    else {
        return false;
        }

    }


/**
 * Makes a copy of the file
 *
 * @package filesystem
 * @param   string $file_path
 * @param   string $new_file
 * @return  string
 */
function o2file_copy($file_path, $new_file) {

    $folder_local_s = new o2_dir(dirname($file_path).DIRECTORY_SEPARATOR);
    $file_local_s   = $folder_local_s->element($file_path);
    $folder_local_t = new o2_dir(dirname($new_file).DIRECTORY_SEPARATOR);
    $file_local_t   = $folder_local_t->element($new_file);
    clearstatcache();
    if ($folder_local_s->exists($file_local_s->nome_completo) &&
       !$folder_local_t->exists($file_local_t->nome_completo)) {
        return copy($file_local_s->nome_completo, $file_local_t->nome_completo);
        }
    else {
        return false;
        }

    }


/**
 * Returns a base64 represntation a the file content.
 * Useful to embed binary data into plain-text context (xml, email body, etc...).
 *
 * @package filesystem
 * @param   string $file_path   File to encode
 * @return  string
 */
function o2file_encode($file_path) {

    $folder_local = new o2_dir(dirname($file_path).DIRECTORY_SEPARATOR);
    $file_local   = $folder_local->element($file_path);
    $res          = false;
    clearstatcache();
    if ($folder_local->exists($file_local->nome_completo)) {
        $res = base64_encode(file_get_contents($file_local->nome_completo));
        $res = chunk_split($res, 64, "\n");
        }
    else {
        throw new o2_exception("Unknown file <i>".$file_path.
                               "</i> requested for encoding",
                               o2error_IO);
        }
    return $res;

    }


// ====================================== ASP ============================================

/**
 * Returns the current ASP-ID set for the session.
 *
 * @package asp
 * @return  string
 */
function o2asp_code_get() {

    return $_SESSION['o2_app']->vars['_area']->valore;

    }


/**
 * Set the new ASP-ID for the session and returns the old one.
 *
 * @package asp
 * @param   string $new_aspid
 * @return  string
 */
function o2asp_code_set($new_aspid) {

    $app          = $_SESSION['o2_app'];
    $area         = $app->vars['_area'];
    $old_val      = $area->valore;
    $area->valore = $new_aspid;
    $app->set_aspid();
    return $old_val;

    }


/**
 * Returns the list of ASP-IDs defined in users table.
 *
 * @package asp
 * @return  array
 */
function o2asp_codes() {

    return $_SESSION['o2_app']->asp_codes();

    }


// =================================== DISPATCHER ========================================

/**
 * Returns the array of all dispatches IDs in the dispatcher list.
 *
 * @package dispatcher
 * @return  array
 */
function o2dispatcher_get_list() {

    return o2_dispatcher::get_dispatcher()->get_list();

    }


/**
 * Returns the number of dispatches in the dispatcher list.
 *
 * @package dispatcher
 * @return  integer
 */
function o2dispatcher_count() {

    return o2_dispatcher::get_dispatcher()->count();

    }


/**
 * Removes all dispatches from the list.
 * Returns TRUE on success, FALSE on fail.
 *
 * @package dispatcher
 * @return  boolean
 */
function o2dispatcher_clear() {

    return o2_dispatcher::get_dispatcher()->clear();

    }


/**
 * Set ALERT status on dispatcher.
 * $msg will displayed to the user by JS-alert.
 *
 * @package dispatcher
 * @param   string $msg
 * @return  boolean
 */
function o2dispatcher_alert($msg = '') {

    return o2_dispatcher::get_dispatcher()->alert($msg);

    }


/**
 * Adds a new dispatch to dispatcher list and return assigned unique ID.
 *
 * @package dispatcher
 * @param   string  $title   Title, visible in the list, to assign to dispatch
 * @param   string  $body    Text of the dispatch, visible in the tooltip
 * @param   string  $icon    Icon to show for the dispatch in the list
 * @return  integer
 */
function o2dispatch_add($title, $body = "", $icon = false) {

    return o2_dispatcher::get_dispatcher()->dispatch_add($title, $body, $icon);

    }


/**
 * Adds a new label line to dispatcher list and return assigned unique ID.
 * A labels is the same as a dispatch but labels are not involved in items count and
 * notification status.
 * Labels are intended to add commands, tools, separators, group labels, etc... in
 * notification window.
 *
 * @package dispatcher
 * @param   string  $title   Title, visible in the list, to assign to dispatch
 * @param   string  $body    Text of the dispatch, visible in the tooltip
 * @param   string  $icon    Icon to show for the dispatch in the list
 * @return  integer
 */
function o2dispatch_add_label($title, $body = "", $icon = false) {

    return o2_dispatcher::get_dispatcher()->dispatch_add($title, $body, $icon, true);

    }


/**
 * Set the activation program for the requested ID.
 * Returns TRUE on success and FALSE on fail.
 * To pass parameters for activation program just pass them after $prg_name.
 *
 * @package dispatcher
 * @param   integer $id         Dispatch ID to set program for
 * @param   string  $prg_name   Activation program to call on click on dispatch
 * @return  boolean
 */
function o2dispatch_set_prg($id, $prg_name) {

    $params = func_get_args();
    return call_user_func_array(array(o2_dispatcher::get_dispatcher(),
                                      "dispatch_set_prg"),
                                $params);

    }


/**
 * Remove a dispatch from the list.
 * The ID is the one returned by o2dispatch_add().
 * Returns TRUE on success, FALSE on fail.
 *
 * @package dispatcher
 * @param   integer $id   Dispatch ID to remove
 * @return  boolean
 */
function o2dispatch_remove($id) {

    return o2_dispatcher::get_dispatcher()->dispatch_remove($id);

    }


/**
 * Returns the dispatch packet with requested ID.
 * Dispatch packet is an array with keys:
 *
 *  "title"   Title string
 *  "body"    Message body text
 *  "alert"   Alert state
 *  "img"     Icon name
 *  "prg"     Activation program name
 *  "pars"    Activation parameters array
 *
 * @package dispatcher
 * @param   integer $id   Dispatch ID to return
 * @return  array
 */
function o2dispatch_get($id) {

    return o2_dispatcher::get_dispatcher()->get_dispatch($id);

    }


/**
 * Returns TRUE if dispatch with requested ID has an activation program.
 *
 * @package dispatcher
 * @param   integer $id   Dispatch ID to check
 * @return  boolean
 */
function o2dispatch_can_activate($id) {

    return o2_dispatcher::get_dispatcher()->dispatch_can_activate($id);

    }


/**
 * Executes activation program for dispatch with requested ID.
 * Usually a message is removed from dispatcher when activated: if you want to preserve it
 * from being removed pass the parameter $preserve as TRUE.
 * Returns TRUE on success, FALSE on fail.
 *
 * @package dispatcher
 * @param   integer $id         Dispatch ID to check
 * @param   boolean $preserve   Preserve dispatch from being removed
 * @return  boolean
 */
function o2dispatch_activate($id, $preserve = false) {

    return o2_dispatcher::get_dispatcher()->dispatch_activate($id, $preserve);

    }


// ====================================== ROBOT ==========================================

/**
 * Adds a task to be executed on a certain frequency: each time task is executed a time
 * marker is reset to evaluate next frequency expiration.
 *
 * @package robot
 * @param   string  $id         Unique name for the task
 * @param   integer $freq       Execution frequency in hours
 * @param   string  $prg_name   Program name to execute for task
 */
function o2robot_task($id, $freq, $prg_name) {

    // ______________________________________________________ Register scheduled tasks ___
    $robot = o2_robot::get_instance();
    /*
    // ___________________________________________________________________ Maintenance ___
    $robot->add_task($id, array($this, "maintenance"), $this->maint_time);
    $robot->run();
    */

    }


// ================================= TREEVIEW CONTROL ====================================


/**
 * Defines a treeview control in the current program.
 * $structure can be an array of nodes to fill-up treeview on creation. Array is in the
 * form:
 *  array($node1_id => array($parent_id, $html_code, icon_file),
 *        $node2_id => array($parent_id, $html_code, icon_file),
 *        ...,
 *        $nodeN_id => array($parent_id, $html_code, icon_file))
 *
 * @package treeview
 * @param   string  $name         Control unique name
 * @param   array   $structure    Tree view structure array
 * @param   integer $activation   Active nodes 0=None|1=All|2=Leavs|3=Folders
 * @param   string  $action       Action to be executed on nodes activation
 * @return  o2_ctrl_tree
 */
function o2tree_def($name, $structure = false, $activation = 0, $action = false) {

    return o2_ctrl_tree::def($name, $structure, $activation, $action);

    }


/**
 * Destroys an existing treeview control in the current program
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2tree_destroy($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        unset($prg->tree_ctrls[$name]);
        return true;
        }
    return false;

    }


/**
 * Fold all foldable nodes in treeview down to $level.
 * If $level is 0 all folders are expanded.
 *
 * @package treeview
 * @param   string  $name    Control unique name
 * @param   integer $level   Folding level to fold down to
 * @return  boolean
 */
function o2tree_fold($name, $level = 0) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        $prg->tree_ctrls[$name]->fold_all($level);
        return true;
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Unfold all foldable nodes in treeview
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2tree_unfold($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        $prg->tree_ctrls[$name]->unfold_all();
        return true;
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Fold a node in treeview.
 *
 * @package treeview
 * @param   string  $name      Control unique name
 * @param   mix     $node_id   Folding node ID
 * @return  boolean
 */
function o2tree_node_fold($name, $node_id) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        $prg->tree_ctrls[$name]->node_fold($node_id);
        return true;
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Unfold a node in treeview
 *
 * @package treeview
 * @param   string $name      Control unique name
 * @param   mix    $node_id   Unfolding node ID
 * @return  boolean
 */
function o2tree_node_unfold($name, $node_id) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        $prg->tree_ctrls[$name]->node_unfold($node_id);
        return true;
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Select a node in treeview and make it visible
 *
 * @package treeview
 * @param   string $name      Treeview control unique name
 * @param   string $node_id   Node id to be selected
 * @return  boolean
 */
function o2tree_node_select($name, $node_id) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->node_select($node_id);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Add a new node to treeview.
 * If Node ID is zero a progressive numeric ID will be assigned to node.
 * If Parent ID is zero node will be top level.
 * Function returns new node assigned ID.
 *
 * @package treeview
 * @param   string $name        Control unique name
 * @param   string $node_id     Node unique ID
 * @param   string $parent_id   Parent node ID, if none node is top level
 * @param   string $node_code   HTML code to be displayed as node content
 * @return  string
 */
function o2tree_node_add($name, $node_id, $parent_id, $node_code, $node_icon = "") {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->node_add($node_id,
                                                 $parent_id,
                                                 $node_code,
                                                 $node_icon);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Removes a node and all its sub-nodes from a treeview.
 * Function returns tha array of all removed nodes.
 *
 * @package treeview
 * @param   string $name        Control unique name
 * @param   string $node_id     Node unique ID
 * @return  array
 */
function o2tree_node_remove($name, $node_id) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->node_remove($node_id);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Makes a node visible, opening all its ancestors
 *
 * @package treeview
 * @param   string $name      Control unique name
 * @param   mix    $node_id   ID of node to be shown
 * @return  boolean
 */
function o2tree_node_show($name, $node_id) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->node_show($node_id);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Returns an array of the treeview nodes structure
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  array
 */
function o2tree_get_nodes($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->list;
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Returns the ID of the selected node in treeview
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  string
 */
function o2tree_get_selection($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->last_node;
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Returns the HTML code for defined Treeview control $name
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  string
 */
function o2tree_get_code($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return o2html::ctrl_tree($name);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }



/**
 * Filter Treeview control $name elements using $text.
 * Only elements containing $text in their description, and their ancestors, are left in.
 *
 * If param $hide_down is passed as FALSE subnodes are added to filtered ones, else
 * they are excluded.
 *
 * @package treeview
 * @param   string  $name        Control unique name
 * @param   string  $text        Text to filter for
 * @param   boolean $hide_down   If FALSE add subnodes, else hide them
 * @return  boolean
 */
function o2tree_filter($name, $text, $hide_down = false) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$name])) {
        return $prg->tree_ctrls[$name]->filter($text, $hide_down);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$name."</i> in program <i>".
                               $prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        return false;
        }

    }


/**
 * Export data from a TreeView control to a CSV format file.
 * File is zipped and the full path to file is returned.
 *
 * @package treeview
 * @param   string  $tree_name           Name of the TreeView control to export data from
 * @param   string  $csv_file_name       Target file name
 * @param   string  $fields_separator    Character use to separate fields
 * @param   string  $text_delimiter      Character used to delimiter text values
 * @return  string
 */
function o2tree_export($tree_name,
                       $csv_file_name     = "",
                       $fields_separator  = ",",
                       $text_delimiter    = '"') {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->tree_ctrls[$tree_name])) {
        return $prg->tree_ctrls[$tree_name]->export($csv_file_name,
                                                    $fields_separator,
                                                    $text_delimiter);
        }
    else {
        throw new o2_exception("Unknown Treeview control <i>".$tree_name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        return false;
        }

    }


// ================================= IMAGE FUNCTIONS =====================================

/**
 * Creates a thumbnail for a given source image file.
 * When $ratio is FALSE thumbnail image will stratch to fit width and height.
 * When $ratio is TRUE humbnail image will maintain aspect ratio to fit width or height
 * without stratching.
 * Supported image formats are PNG, GIF and JPG.
 *
 * @package image
 * @param   string  $source_file   Source image file to be thumbnailed
 * @param   string  $target_file   Target image file to be created
 * @param   integer $width         Thumbnail width
 * @param   integer $height        Thumbnail height
 * @param   boolean $ratio         If TRUE thumbnail will maintain source aspect ratio
 * @return  boolean
 */
function o2img_thumbnail($source_file, $target_file, $width, $height, $ratio = true) {

    if (file_exists($source_file)) {
        $file = new o2_fsitem($source_file);
        switch (strtolower($file->ext)) {
            case "jpg":
                $simg = imagecreatefromjpeg($file->nome_completo);
                break;
            case "gif":
                $simg = imagecreatefromgif($file->nome_completo);
                break;
            case "png":
                $simg = imagecreatefrompng($file->nome_completo);
                break;
            default:
                return;
            }
        $oldx   = imagesx($simg);
        $oldy   = imagesy($simg);
        $ratiox = ($width / $oldx);
        $ratioy = ($height / $oldy);
        if ($ratio) {
            $ratiox = $ratioy = min($ratiox, $ratioy);
            }
        $newx   = floor($oldx * $ratiox);
        $newy   = floor($oldy * $ratioy);
        $newimg = imagecreatetruecolor($newx, $newy);
        imagecopyresized($newimg, $simg, 0, 0, 0, 0, $newx, $newy, $oldx, $oldy);
        $target = new o2_fsitem($target_file);
        if (file_exists($target->nome_completo)) {
            unlink($target->nome_completo);
            }
        imagejpeg($newimg, $target->path.$target->nome.".jpg");
        }
    else {
        throw new o2_exception("Unknown file <i>".$source_file.
                               "</i> requested for thumbnailing",
                               o2error_IO);
        }

    }


// ============================== IMAGES LISTER CONTROL ==================================


/**
 * Defines an images lister control in the current program.
 * $items can be an array of items to fill-up images lister on creation. Array is in the
 * form:
 *  array($item1_id => array($image_url, $text_code),
 *        $item2_id => array($image_url, $text_code),
 *        ...,
 *        $itemN_id => array($image_url, $text_code))
 *
 * @package imglist
 * @param   string  $name           Control unique name
 * @param   array   $items          Images list array
 * @param   integer $width          Items width
 * @param   integer $height         Items height
 * @param   string  $act_activate   Program action to be executed on click on item
 * @param   string  $act_delete     Program action to be executed on click on remove icon
 * @return  o2_ctrl_imglist
 */
function o2imglist_def($name,
                       $items        = false,
                       $width        = false,
                       $height       = false,
                       $act_activate = false,
                       $act_delete   = false) {

    return o2_ctrl_imglist::def($name,
                                $items,
                                $width,
                                $height,
                                $act_activate,
                                $act_delete);

    }


/**
 * Destroys an existing images lister control in the current program
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2imglist_destroy($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        unset($prg->imglist_ctrls[$name]);
        return true;
        }
    return false;

    }


/**
 * Add a new item to images lister.
 * If Item ID is zero a progressive numeric ID will be assigned to item.
 * Function returns new item assigned ID.
 *
 * @package imglist
 * @param   string $name        Control unique name
 * @param   string $item_id     Item unique ID
 * @param   string $text_code   HTML code to be displayed as item text
 * @return  string
 */
function o2imglist_item_add($name, $item_id, $text_code) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        return $prg->imglist_ctrls[$name]->img_add($item_id, $text_code);
        }
    else {
        throw new o2_exception("Unknown ImagesLister control <i>".$name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Removes an item from an images lister.
 * Function returns tha array of all removed nodes.
 *
 * @package imglist
 * @param   string $name        Control unique name
 * @param   string $item_id     Item unique ID
 * @return  array
 */
function o2imglist_item_remove($name, $item_id) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        return $prg->imglist_ctrls[$name]->img_remove($item_id);
        }
    else {
        throw new o2_exception("Unknown ImagesLister control <i>".$name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Set width and height for the images lister items
 *
 * @package imglist
 * @param   string  $name     Control unique name
 * @param   integer $width    Items width
 * @param   integer $height   Items height
 * @return  boolean
 */
function o2imglist_set_size($name, $width = false, $height = false) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        return $prg->imglist_ctrls[$name]->set_items_size($width, $height);
        }
    else {
        throw new o2_exception("Unknown ImagesLister control <i>".$name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Returns an array of the images lister items list
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  array
 */
function o2imglist_get_items($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        return $prg->imglist_ctrls[$name]->list;
        }
    else {
        throw new o2_exception("Unknown ImagesLister control <i>".$name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Returns the HTML code for defined images lister control $name
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  string
 */
function o2imglist_get_code($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        return o2html::ctrl_imglist($name);
        }
    else {
        throw new o2_exception("Unknown ImagesLister control <i>".$name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


/**
 * Returns the ID of the selected item in images lister
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  string
 */
function o2imglist_get_selection($name) {

    $app = $_SESSION['o2_app'];
    $prg = $app->istanze_prg[$app->progressivo_istanze];
    if (isset($prg->imglist_ctrls[$name])) {
        return $prg->imglist_ctrls[$name]->last_item;
        }
    else {
        throw new o2_exception("Unknown ImagesLister control <i>".$name.
                               "</i> in program <i>".$prg->nome."</i>",
                               o2error_UNKNOWNCTRL);
        }
    return false;

    }


// =============================== PROGRESS BAR CONTROL ==================================

/**
 * Define a bar-control and returns its HTML code, to be displayed in a HTML-area control.
 * Function calling this method (usually a Janox expression) is used each time to update
 * value.
 *
 * @package progress_bar
 * @param   string  $bar_name          Control unique name
 * @param   integer $value             Constrol value
 * @param   string  $complete_action   Action to be executed on completion
 * @param   string  $start_action      Action to be executed on start (from 0 to >0)
 * @param   boolean $active            If bar is active and it makes update requests
 * @return  string
 */
function o2progress_bar($bar_name,
                        $value,
                        $complete_action = "",
                        $start_action    = "",
                        $active          = true) {

    return o2_ctrl_progress::get_bar($bar_name,
                                     $value,
                                     $complete_action,
                                     $start_action,
                                     $active);

    }


/**
 * Defines a bar-ctrl and returns its HTML code, to be displayed in a HTML-area control.
 * This method is primary intended to be used in a grid.
 * Control name must be unique: in a grid a unique column value can be used as name.
 *
 * @package progress_bar
 * @param   string $bar_name          Control unique name
 * @param   string $view              View name
 * @param   string $id_field          Name of view-field containing name
 * @param   string $progress_field    Name of view-field containing value
 * @param   string $complete_action   Action to be executed on progress complete
 * @param   string $start_action      Action to be executed on start (from 0 to >0)
 * @param   boolean $active           If bar is active and it makes update requests
 * @return  string
 */
function o2progress_view_bar($bar_name,
                             $view,
                             $id_field,
                             $progress_field,
                             $complete_action = "",
                             $start_action    = "",
                             $active          = true) {

    return o2_ctrl_progress::get_by_view($bar_name,
                                         $view,
                                         $id_field,
                                         $progress_field,
                                         $complete_action,
                                         $start_action,
                                         $active);

    }


// ============================ PROGRAM REFRENCED VARIABLES ==============================

/**
 * Creates a reference-variable for current program and set it to passed value.
 *
 * @package prg
 * @param   string $name    Variable unique name
 * @param   mix    $value   Variable definition value
 * @see     o2refvar_get()
 */
function o2refvar_set($name, $value = false) {

    $name = strtolower($name);
    $app  = $_SESSION['o2_app'];
    $app->istanze_prg[$app->progressivo_istanze]->reference_vars[$name] = $value;

    }


/**
 * Returns a refernce to a reference-variable
 *
 * ATTENTION: In order to get a safe reference to the variable, regardless the variable
 *            type, you must use the referenced-assign PHP syntax, in this way:
 *
 *             $var = &o2refvar_get("<var-name>");
 *
 * @package prg
 * @param   string $name    Variable unique name
 * @param   mix    $value   Variable definition value
 * @see     o2refvar_set()
 */
function &o2refvar_get($name) {

    $name = strtolower($name);
    $app  = $_SESSION['o2_app'];
    return $app->istanze_prg[$app->progressivo_istanze]->reference_vars[$name];

    }


// ==================================== LAST ERROR =======================================

/**
 * Clears last error informations
 *
 * @package error
 */
function o2error_lastclear() {

    unset($GLOBALS["jxlasterror"]);

    }


/**
 * Returns last error unique id.
 * If parameter $box_div is passed as TRUE, then numeric id is prefixed with "jxerror_":
 * this parameter is useful to get the message box div id in custom error files.
 *
 * @package error
 * @param   boolean $box_div   Numeric unique id or box div id
 * @return  integer
 */
function o2error_getlastid($box_div = false) {

    if ($GLOBALS["jxlasterror"]) {
        return ($box_div ? "jxerror_" : "").$GLOBALS["jxlasterror"]->id;
        }
    else {
        return false;
        }

    }


/**
 * Returns last error numeric code, FALSE if no error is on.
 *
 * @package error
 * @return  integer
 */
function o2error_getlastcode() {

    if ($GLOBALS["jxlasterror"]) {
        return $GLOBALS["jxlasterror"]->get_code();
        }
    else {
        return false;
        }

    }


/**
 * Returns last error error type string, FALSE if no error is on.
 *
 * @package error
 * @return  string
 */
function o2error_getlastclass() {

    if ($GLOBALS["jxlasterror"]) {
        return $GLOBALS["jxlasterror"]->get_class();
        }
    else {
        return false;
        }

    }


/**
 * Returns last error text message, FALSE if no error is on.
 *
 * @package error
 * @return  string
 */
function o2error_getlastmsg() {

    if ($GLOBALS["jxlasterror"]) {
        return $GLOBALS["jxlasterror"]->get_message();
        }
    else {
        return false;
        }

    }


/**
 * Returns last error complete text, as it appears in errors.log, FALSE if no error is on.
 *
 * @package error
 * @return  string
 */
function o2error_getlasttxt() {

    if ($GLOBALS["jxlasterror"]) {
        return $GLOBALS["jxlasterror"]->to_raw_text();
        }
    else {
        return false;
        }

    }


/**
 * Returns last error complete HTML, as it appears in error-boxes, FALSE if no error is
 * on.
 *
 * @package error
 * @return  string
 */
function o2error_getlasthtml() {

    if ($GLOBALS["jxlasterror"]) {
        return $GLOBALS["jxlasterror"]->to_html();
        }
    else {
        return false;
        }

    }


// ================================= FORMATTING TEXT =====================================

/**
 * Format text as justified
 *
 * @package text
 * @param   string  $text         Text to format
 * @param   integer $width        Width to format text to
 * @param   integer $indent       Number of characters for indentation
 * @param   string  $indent_chr   Character to be used for indentation
 * @return  string
 */
function o2txt_justify($text = "", $width = 60, $indent = 0, $indent_chr = " ") {

    $width  = $width - strlen($indent_chr) * $indent;
    $t      = "";
    $needle = "";
    foreach (explode("\n", $text) as $single_parag) {
        $parag_array = explode("\n", wordwrap($single_parag, $width, "\n"));
        $parag_lines = count($parag_array);
        for($line_index = 0; $line_index < ($parag_lines - 1); $line_index++) {
            $s_line       = rtrim($parag_array[$line_index]);
            $offset_local = 0;
            $needle       = " ";
            while (strlen($s_line) < $width) {
                $pos_local = strpos(strrev($s_line), $needle, $offset_local + 1);
                if (!$pos_local) {
                    $offset_local = 0;
                    $needle      .= " ";
                    $pos_local    = strpos(strrev($s_line), $needle, 0);
                    }
                $offset_local = $pos_local + strlen($needle);
                $s_line       = substr_replace($s_line, $needle." ",
                                               -($offset_local),
                                               strlen($needle));
                }
            $t.= str_repeat($indent_chr, $indent).$s_line."\n";
            }
        $t.= str_repeat($indent_chr, $indent).$parag_array[$line_index]."\n";
        }
    return $t;

    }


/**
 * Format text as splitted in two parts, one aligned to left, the other aligned to right
 *
 * @package text
 * @param   string  $text_left    Text to be aligned to left
 * @param   string  $text_right   Text to be aligned to right
 * @param   integer $width        Width to format text to
 * @param   integer $indent       Number of characters for indentation
 * @param   string  $indent_chr   Character to be used for indentation
 * @param   string  $sep_chr      Character to be used to fill space between textes
 * @return  string
 */
function o2txt_split ($text_left  = "",
                      $text_right = "",
                      $width      = 60,
                      $indent     = 0,
                      $indent_chr = " ",
                      $sep_chr    = ".") {

    if ((strlen($text_left) + strlen($indent_chr) * $indent) < $width) {
        $line_local = str_repeat($indent_chr, $indent).$text_left;
        $line_local.= str_repeat($sep_chr,
                                 max(0, ($width - strlen($line_local.$text_right)))).
                      $text_right;
        }
    else {
        $line_local = rtrim(o2txt_justify($text_left,
                                          intval($width / 4 * 3) + 1,
                                          $indent, $indent_chr));
        $sep_len    = max(0,
                          ($width - (strlen($line_local) % (intval($width / 4 * 3) + 1)))
                          );
        $line_local.= str_repeat($sep_chr, $sep_len).$text_right;
        }
    return $line_local;

    }


/**
 * Format text as splitted in two parts, one aligned to left, the other aligned to right
 *
 * @package text
 * @param   string  $text         Text to be aligned to left
 * @param   integer $width        Width to format text to
 * @param   integer $indent       Number of characters for indentation
 * @param   string  $indent_chr   Character to be used for indentation
 * @param   string  $fill_chr     Character to be used to fill space out of text
 * @return  string
 */
function o2txt_center($text       = "",
                      $width      = 60,
                      $indent     = 0,
                      $indent_chr = " ",
                      $fill_chr   = " ") {

    $double      = ($width - strlen($indent_chr) * $indent - strlen($text));
    $left_local  = str_repeat($fill_chr, intval($double / 2));
    $right_local = str_repeat($fill_chr, intval($double / 2) + (1&$double ? 1 : 0));
    $line_local  = str_repeat($indent_chr, $indent).$left_local.$text.$right_local;
    return $line_local;

    }

?>
