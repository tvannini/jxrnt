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
 * @version   2.9
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
 * passing a single letter as supplementary parameter:
 * "y"=Year(2 digits), "Y"=Year(4 digits), "m"=Month, "d"=Day, "h"=Hours, "i"=Minutes and
 * "s"=Seconds.
 * Excepted "y/Y" paramter for year, all other letters are key-unsensitive.
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
            $ret = array();
            break;
        }
    return $ret;

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

    return o2val($view_name, $field_name);

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

    return false;

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

    return false;

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

    return true;

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

    return true;

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
 * @see    o2par_zero()
 * @see    o2val()
 * @see    o2format()
 */
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


/**
 * Returns boolean value TRUE/FALSE if parameter value is equal/different from its type
 * default value.
 * This function is the right way to check if a parameter has a value.
 *
 * Parameters don't change their value during program execution so values returned by
 * o2par_zero() are consistent along program running.
 *
 * @param  integer $parameter_id   Ordinal parameter index
 * @return boolean
 * @see    o2par()
 * @see    o2val()
 * @see    o2zero()
 */
function o2par_zero($parameter_id) {

    return false;

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

    return '';

    }


/**
 * Returns a UNIQUE 16 digits string
 *
 * @return string
 */
function o2uid() {

    return '1000';

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
    return new DateTime($d[0].$d[1].$d[2].$d[3].'-'.
                        $d[4].$d[5].'-'.
                        $d[6].$d[7].' '.
                        $t[0].$t[1].':'.
                        $t[2].$t[3].':'.
                        $t[4].$t[5]);

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
function o2_callprg($prg) {

    return true;

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

    return $prg_name;

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

    return $action_name;

    }


/**
 * Recover the value of a parameter passed by o2_act4js() method by index order.
 *
 * @param  integer $par_id
 * @return string
 * @see    o2_act4js()
 */
function o2_reqpar($par_id) {

    return '';

    }


/**
 * In HTML disposes requested file for download and returns the url leading to it.
 *
 * @param  string  $local_file      Local path to file to deliver
 * @param  boolean $force_download  TRUE=Force download, FALSE=Can be used on-line (PDF)
 * @return string
 */
function o2_path($local_file, $force_download = false) {

    return '';

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

    return $string;

    }


/**
 * Dispose a file for download by URL.
 * File is sent to client without a user's click on link.
 *
 * Correct use to send a local file by path:
 *  o2_send(o2_path($file, true))
 *
 * @param  string $file_url   URL to file to send
 */
function o2_send($file_url) {

    return true;

    }


/**
 *  Return a passed text as plain text, as response for client request.
 *  This method is specifically useful for returning code to client runtime (Javascript,
 *  JSON, custom libraries, ...).
 *
 * @param string $text   Text (or code) to be sent to client
 */
function o2out($text) {

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

    return true;

    }


/**
 * Returns translation for the passed code.
 * Function can accept infinite parameters to replace placeholders in string.
 * Placeholders are in the form $n, where $1 is the first substitution parameter, $2 the
 * second one and so on.
 *
 * @param  string $code   Unique code identifier for string
 * @param  string $text   Text to be translated
 * @return string
 */
function o2_translate($code, $string) {

    return $string;

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

    return true;

    }


/**
 * Sets status bar custom content.
 * Content $code can be plain text or HTML code.
 *
 * @param string $code
 */
function o2_status($code) {

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return array();

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

   return array();

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

    return '';

    }


/**
 * Returns current o2runtime root path
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_alias(), o2rnt_interface()
 */
function o2rnt_root() {

    return '';

    }


/**
 * Returns current o2runtime running threads folder path
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_dircss()
 */
function o2rnt_dirthreads() {

    return '';

    }


/**
 * Returns current o2runtime css folder path
 *
 * @package runtime
 * @return  string
 * @see     o2rnt_dirthreads()
 */
function o2rnt_dircss() {

    return '';

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

    return 'HTML';

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

    return false;

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

    return false;

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
 * ATTENTION: This function is intended to be an administrative tool and only
 *            administrators and developers can call this function!
 *
 * @package   session
 * @parameter $on
 * @return    boolean
 * @see       o2session_is_job()
 */
function o2session_lock($on = true) {

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return 1;

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

    return true;

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

    return '';

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

    return true;

    }


/**
 * Queue for execution job with passed name.
 * Job is queued for execution but a dedicated batch process is NOT started: some other
 * logic (scheduler) will take care of executing it later.
 * A list of parameters (by name) can be provided: list will override parameters passed to
 * program called by job.
 * If a host name is provided then execution is assigned directly to that host, regardless
 * of services logics.
 * If "*" (star) is passed for host parameter then job is queued for all active hosts.
 * NOTE: If no host name is provided execution can be assigned to local host by
 *       application settings: see "jobs_use_local_host" parameter in application
 *       settings.
 * If job exists then new job execution ID is returned, else FALSE.
 *
 * @package jobs
 * @param   string $job_name   Job name to look for
 * @param   array  $params     List of overriding parameters passed to program
 * @param   string $host       Host name to override assigned host/service
 * @return  integer
 * @see     o2job_create()
 * @see     o2job_get_by_name()
 * @see     o2job_run()
 * @see     o2job_is_running()
 */
function o2job_queue($job_name, $params = array(), $host = false) {

    return 1;

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

    return 1;

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

    return true;

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

    return 'C';

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

    return true;

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
 * @param   boolean $all_hosts    Run job on all active hosts
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
                        $aspid      = "",
                        $all_hosts  = false) {

    return 1;

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

    return false;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return array();

    }


/**
 * Returns logged user rights list as an array
 *
 * @package user
 * @return  array
 * @see     o2user_roles(), o2user_has()
 */
function o2user_rights() {

    return array();

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

    return true;

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

    return true;

    }


/**
 * Returns TRUE if logged user is Administrator
 *
 * @package user
 * @return  boolean
 * @see     o2user_is(), o2user_roles(), o2user_has(), o2user_is_poweruser()
 */
function o2user_is_admin() {

    return true;

    }


/**
 * Returns TRUE if logged user is Administrator
 *
 * @package user
 * @return  boolean
 * @see     o2user_is_admin(), o2user_is(), o2user_roles(), o2user_has()
 */
function o2user_is_poweruser() {

    return true;

    }


/**
 * Reloads user rights and roles
 *
 * @package user
 * @return  boolean
 * @see     o2user_rights(), o2user_roles()
 */
function o2user_load_granting() {

    return true;

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

    return true;

    }


/**
 * Returns number of days before user password expiring.
 *
 * Function can return a negative value, if password already expired.
 * If no expiration time is set for application or if password never expires for requested
 * user, then function returns (strict) FALSE.
 * If "last password change" date is not set or flag "force password change" is set for
 * user, then function returns 0 (zero).
 *
 * If no any user will be passed as parameter then currently logged user will be used.
 *
 * @package user
 * @param   string $user   User to be check password of
 * @return  integer
 * @see     o2user_is(), o2user_has()
 */
function o2user_pwd_expires($user = false) {

    return 0;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return '';

    }


/**
 * Returns client browser ip address
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_host(), o2client_platform(), o2client_os()
 */
function o2client_ip() {

    return '';

    }


/**
 * Returns client machine host name
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_ip(), o2client_platform(), o2client_os()
 */
function o2client_host() {

    return '';

    }


/**
 * Returns client machine platform description
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_host(), o2client_ip(), o2client_os()
 */
function o2client_platform() {

    return '';

    }


/**
 * Returns client machine operating system description
 *
 * @package client
 * @return  string
 * @see     o2client_browser(), o2client_host(), o2client_platform(), o2client_ip()
 */
function o2client_os() {

    return '';

    }


/**
 * Returns client browser window width
 *
 * @package client
 * @return  integer
 * @see     o2client_browser(), o2client_height()
 */
function o2client_width() {

    return 1200;

    }


/**
 * Returns client browser window height
 *
 * @package client
 * @return  integer
 * @see     o2client_browser(), o2client_width()
 */
function o2client_height() {

    return 900;

    }


// ==================================== APPLICATION ======================================

/**
 * Returns PHP session ID
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_alias()
 */
function o2app_sid() {

    return '123456789';

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

    return 'root';

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

    return '';

    }


/**
 * Returns application css file name
 *
 * @package application
 * @return  string
 * @see     o2app_alias(), o2app_menu()
 */
function o2app_css() {

    return '';

    }


/**
 * Returns the names of running programs in an array
 *
 * @package application
 * @return  array
 */
function o2app_prg() {

    return array();

    }


/**
 * Returns application name
 *
 * @package application
 * @return  string
 * @see     o2app_sid(), o2app_alias()
 */
function o2app_name() {

    return '';

    }


/**
 * Returns application instance unique ID.
 * This ID is a 8 characters string, unique for application home folder.
 * Instance ID is useful for generating unique file names in applications shared folders.
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_alias()
 */
function o2app_instance_id() {

    return '12345678';

    }


/**
 * Returns application alias as an URL prefix, mostly useful to address application
 * resources (images, JS libraries, ...)
 *
 * NOTE: application alias is returned with ending back-slash.
 *
 * Sample: if you are serving your application from "[myhost]/apps/myapp/myapp.php" main
 *         page, then application alias will be returned as "/apps/myapp/".
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_dir_htdocs()
 */
function o2app_alias() {

    return '';

    }


/**
 * Returns application host, machine name on which application is executed.
 *
 * NOTE: Host can be customized by application parameter "host".
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_runmode()
 */
function o2app_host() {

    return '';

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

    return '';

    }


/**
 * Returns application type of validation
 *
 * @package application
 * @return  integer
 * @see     o2app_nologin(), o2app_user(), o2app_password()
 */
function o2app_validate() {

    return 1;

    }


/**
 * Returns application character encoding
 *
 * @package application
 * @return  string
 * @see     o2app_lang()
 */
function o2app_encoding() {

    return '';

    }


/**
 * Returns session timeout
 *
 * @package application
 * @return  integer
 * @see     o2app_exetime()
 */
function o2app_timeout() {

    return 1000;

    }


/**
 * Returns elapsed time in microseconds since script started
 *
 * @package application
 * @return  float
 * @see     o2app_timeout()
 */
function o2app_exetime() {

    return 1000000;

    }


/**
 * Returns application run-level: [DEV] for development or [EXE] for normal
 * execution
 *
 * @package application
 * @return  string
 * @see     o2app_developer(), o2app_runmode()
 */
function o2app_runlevel() {

    return 'DEV';

    }


/**
 * Returns application run-mode:
 *
 *  - WEB: for standard WEB mode
 *  - JOB: when running as a job
 *  - CMD: for console executions
 *  - RPC: for WEB service mode
 *
 * @package application
 * @return  string
 * @see     o2rnt_interface(), o2app_runlevel()
 */
function o2app_runmode() {

    return 'WEB';

    }


/**
 * Returns currently logged developer name
 *
 * @package application
 * @return  string
 * @see     o2app_runlevel()
 */
function o2app_developer() {

    return 'supervisor';

    }


/**
 * Returns page to be displayed on login failure
 *
 * @package application
 * @return  string
 * @see     o2app_validate(), o2app_user(), o2app_password(), o2app_sesserror()
 */
function o2app_nologin() {

    return '';

    }


/**
 * Returns page to be displayed on session validation failure
 *
 * @package application
 * @return  string
 * @see     o2app_validate(), o2app_user(), o2app_password(), o2app_nologin()
 */
function o2app_sesserror() {

    return '';

    }


/**
 * Returns the name of the currently active application menu
 *
 * @package application
 * @return  string
 * @see     o2app_menus(), o2app_user()
 */
function o2app_menu() {

    return 'base';

    }


/**
 * Returns FALSE if no translation is active, else returns the string-code for the active
 * language
 *
 * @package application
 * @return  string
 * @see     o2_translate()
 */
function o2app_lang() {

    return 'it';

    }


/**
 * Returns application variables names as an array
 *
 * @package application
 * @return  array
 * @see     o2val()
 */
function o2app_vars() {

    return array();

    }


/**
 * Returns menus repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_menu()
 */
function o2app_menus() {

    return array();

    }


/**
 * Returns data models repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_user(), o2app_dir_home()
 */
function o2app_models() {

    return array();

    }


/**
 * Returns servers repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_databases(), o2app_tables()
 */
function o2app_servers() {

    return array();

    }


/**
 * Returns databases repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_servers(), o2app_tables()
 */
function o2app_databases() {

    return array();

    }


/**
 * Returns tables repository as an array
 *
 * @package application
 * @return  array
 * @see     o2app_databases(), o2app_servers()
 */
function o2app_tables() {

    return array();

    }


/**
 * Creates unexisting tables for the whole application
 *
 * @package application
 * @return  boolean
 * @see     o2app_servers(), o2app_databases(), o2app_tables()
 */
function o2app_createtables() {

    return true;

    }


/**
 * Returns list of defined programs
 *
 * @package application
 * @return  array
 * @see     o2app_startprg(), o2app_reqprg(), o2app_menus()
 */
function o2app_prgs_list() {

    return array();

    }


/**
 * Returns the name of the program to be executed once after login
 *
 * @package application
 * @return  string
 * @see     o2app_reqprg(), o2app_user()
 */
function o2app_startprg() {

    return '';

    }


/**
 * Returns the name of the program to be executed on every request
 *
 * @package application
 * @return  string
 * @see     o2app_startprg(), o2app_user()
 */
function o2app_reqprg() {

    return '';

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


/**
 * Returns application "home" folder path
 *
 * @package application
 * @return  string
 * @see     o2app_name(), o2app_alias()
 */
function o2app_dir_home() {

    return '';

    }


/**
 * Returns application WEB root folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_home(), o2app_alias()
 */
function o2app_dir_htdocs() {

    return '';

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

    return '';

    }


/**
 * Returns current cvs (team development) folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_prgs(), o2app_dir_home()
 */
function o2app_dir_cvsprgs() {

    return '';

    }


/**
 * Returns current tmp folder path, depending on logged user name
 *
 * @package application
 * @return  string
 * @see     o2app_dir_uploads(), o2app_dir_home()
 */
function o2app_dir_tmp() {

    return '';

    }


/**
 * Returns current data folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_data() {

    return '';

    }


/**
 * Returns path to documentation files folder
 *
 * @package application
 * @return  string
 * @see     o2app_dir_htdocs(), o2app_dir_home()
 */
function o2app_dir_doc() {

    return '';

    }


/**
 * Returns CSS folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_img(), o2app_dir_home()
 */
function o2app_dir_css() {

    return '';

    }


/**
 * Returns logs folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_logs() {

    return '';

    }


/**
 * Returns pdf printing templates folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_tmp(), o2app_dir_home()
 */
function o2app_dir_templates() {

    return '';

    }


/**
 * Returns images folder path
 *
 * @package application
 * @return  string
 * @see     o2app_dir_css(), o2app_dir_home()
 */
function o2app_dir_img() {

    return '';

    }


/**
 * Returns currently logged user
 *
 * @package application
 * @return  string
 * @see     o2app_userimg(), o2app_dir_home(), o2app_user()
 */
function o2app_dir_user() {

    return '';

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

    return '';

    }


/**
 * Execute session logout: delete session record and destroy WEB session related context.
 * If any, after logout navigation will be redirected to provided URL.
 *
 * A list of parameters to be POSTed to the exit-url can be passaed as associtative array,
 * this way: ['par1'=>'val1', 'par2'=>'val2', ...]
 *
 * If $exit_url is passed as (strict) TRUE then client will be redirected to a new session
 *
 * @package application
 * @param   string $exit_url
 * @param   array  $params
 * @return  boolean
 */
function o2app_logout($exit_url = false, $params = false) {

    return true;

    }


/**
 * Generates a One Time Password for application user.
 * OTP is a time-based access code provided to user to override his standard password
 * and can be used only once.
 * Returned OTP is a string of $otp_len chars, default length is 6.
 * If $user is blank then OTP is generated for the currently logged user.
 *
 * OTP string can be used to access to the application by a request like this:
 *
 *  <domain>/app_name.php?user=otp_user&jxotp=otp_code
 *
 * @param  string  $user      User to generate the OTP for
 * @param  integer $otp_len   Length of the OTP token
 * @return string
 */
function o2app_otp($user = '', $otp_len = 6) {

    return '123456';

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

    return true;

    }


/**
 * Return type of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_type($menu_name = "") {

    return '';

    }


/**
 * Return label of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_label($menu_name = "") {

    return '';

    }


/**
 * Return data value of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_data($menu_name = "") {

    return '';

    }


/**
 * Return icon file name of a requested menu item
 *
 * @package menu
 * @param   string $menu_name
 * @return  string
 */
function o2menu_icon($menu_name = "") {

    return '';

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

    return true;

    }


/**
 * Return HTML code for a requested menu.
 * Menu code can be requested for available styles ([T]op, [L]eft, [H]ide, [A]ccordion).
 * If style parameter is left out, then actual INI style will be used.
 *
 * @package menu
 * @param   string $menu_name
 * @param   string $style       [T]op, [L]eft, [H]ide or [A]ccordion
 * @return  boolean
 */
function o2menu_code($menu_name = '', $style = '') {

    return '';

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

    return '"';

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

    return '"';

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

    return '';

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

    return '';

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

    return '';

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

    return '';

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

    return array();

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

    return 1;

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

    return true;

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

    return true;

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

    return '';

    }


/**
 * Returns database server name
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_server($db_name) {

    return '';

    }


/**
 * Returns database engine type (mysql, postgres, oracle, sqlite, ...)
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_engine($db_name) {

    return '';

    }


/**
 * Returns database specific prefix to fully qualify elements
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_qprefix($db_name) {

    return '';

    }


/**
 * Returns database schema identifier
 *
 * @package database
 * @param   string $db_name
 * @return  string
 */
function o2db_schema($db_name) {

    return '';

    }


/**
 * Returns database tables list as an array
 *
 * @package database
 * @param   string $db_name
 * @return  array
 */
function o2db_tables($db_name) {

    return array();

    }


/**
 * Returns true if is active ASP mode for the database
 *
 * @package database
 * @param   string $db_name
 * @return  mixed
 */
function o2db_asp($db_name) {

    return false;

    }


/**
 * Returns "Create tables" flag value for requested database
 *
 * @package database
 * @param   string $db_name
 * @return  boolean
 */
function o2db_createtab($db_name) {

    return '';

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

    return 1;

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

    return 1;

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

    return 1;

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

    return 1;

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

    return 'A';

    }


/**
 * Returns model input/output mask
 *
 * @package model
 * @param   string $model_name
 * @return  string
 */
function o2model_mask($model_name) {

    return 'C50';

    }


/**
 * Returns model data dimension as "string_length"
 *
 * @package model
 * @param   string $model_name
 * @return  integer
 */
function o2model_size($model_name) {

    return 50;

    }


/**
 * Returns an array of model possible values as defined in picture.
 *
 * @package model
 * @param   string  $model_name
 * @return  array
 */
function o2model_values($model_name) {

    return array();

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

    return array();

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

    return '';

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

    return '';

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

    return '_o2alpha';

    }


// ====================================== TABLE ==========================================

/**
 * Checks if table is defined in session (in application repository or in other way).
 * Table name passed as parameter is not key-sensitive.
 * If table is defined then function returns the real key-sensitive name of the table,
 * else it returns FALSE.
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_is_defined($table_name) {

    return true;

    }


/**
 * Returns true if table physically exists, else false
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_exists($table_name) {

    return true;

    }


/**
 * Returns true if table contains any data, else false
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_data($table_name) {

    return true;

    }


/**
 * Creates the table
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_create($table_name) {

    return true;

    }


/**
 * Drops the table
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_drop($table_name) {

    return true;

    }


/**
 * Return sql creation script for the table
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_script($table_name) {

    return '';

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

    return '';

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

    return '';

    }


/**
 * Returns table physical name
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_name($table_name) {

    return $table_name;

    }


/**
 * Returns table fully qualified physiacal name, according with table engine type
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_qname($table_name) {

    return $table_name;

    }


/**
 * Returns table fields as an array
 *
 * @package table
 * @param   string $table_name
 * @return  array
 */
function o2tab_fields($table_name) {

    return array();

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

    return array();

    }


/**
 * Returns table primary key name
 *
 * @package table
 * @param   string $table_name
 * @return  string
 */
function o2tab_pkey($table_name) {

    return 'PK';

    }


/**
 * Returns TRUE if table log is active else FALSE
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_log($table_name) {

    return true;

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

    return 'M';

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

    return true;

    }


/**
 * Deactivate logging for a table.
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_unset_log($table_name) {

    return true;

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

    return true;

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

    return '';

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

    return true;

    }


/**
 * Returns physical table informations as an array
 *
 * @package table
 * @param   string $table_name
 * @return  array
 */
function o2tab_info($table_name) {

    return array();

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return 1;

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

    return 1;

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

    return true;

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

    return true;

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

    return true;

    }


/**
 * Open a window on table log content, allowing data browsing.
 *
 * @package table
 * @param   string $table_name
 * @return  boolean
 */
function o2tab_browselog($table_name) {

    return true;

    }


/**
 * Returns ASP mode for the table if it is active, else false
 *
 * @package table
 * @param   string $table_name
 * @return  mixed
 */
function o2tab_asp($table_name) {

    return false;

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

    return true;

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

    return true;

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

    return array();

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

    return true;

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

    return true;

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

    return 1;

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

    return 1;

    }


// ===================================== PROGRAM =========================================

/**
 * Returns an array of program received parameters, in the form:
 *
 *  param_name => param_value
 *
 * @package prg
 * @param   integer $exe_id
 * @return  array
 * @see     o2par(), o2_callprg(), o2_reqpar()
 */
function o2prg_params($exe_id = 0) {

    return array();

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return array();

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return true;

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

    return 'M';

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

    return true;

    }


/**
 * Returns view start record offset
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 */
function o2view_start($view_name) {

    return 1;

    }


/**
 * Returns view end record offset
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 */
function o2view_end($view_name) {

    return 10;

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

    return 10;

    }


/**
 * Returns number of returned records in the actual dataset page.
 * Returned rows number is equal to requested rows number, until the recordset end is
 * reached.
 *
 * NOTE: After a recordset-goto-first instruction, this function may be useful to get the
 *       actual number of records in a view (0, 1 or more, up to the defined view number
 *       of records).
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 * @see     o2view_reqrows(), o2view_total(), o2view_rec()
 */
function o2view_retrows($view_name) {

    return 10;

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

    return 10;

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

    return true;

    }


/**
 * Returns the current record as an array, or FALSE if there's no current record
 *
 * @package view
 * @param   string $view_name
 * @return  array
 */
function o2view_row($view_name) {

    return array();

    }


/**
 * Returns actual view selection [from 0 to o2view_retrows() - 1]
 *
 * @package view
 * @param   string $view_name
 * @return  integer
 */
function o2view_select($view_name) {

    return 1;

    }


/**
 * Export data from a prg view to a CSV format file.
 * File is zipped and the full path to file is returned.
 *
 * @package view
 * @param   string  $view_name           Name of the view to export data from
 * @param   string  $csv_file_name       Target file name
 * @param   boolean $only_visible        Export only data visible on prg forms
 * @param   string  $fields_separator    Character use to separate fields
 * @param   string  $text_delimiter      Character used to delimiter text values
 * @param   string  $decimal_separator   Character used as decimal point in numbers
 * @param   string  $date_format         Format used for data fields (PHP date() style)
 * @param   string  $time_format         Format used for time fields (PHP date() style)
 * @param   boolean $titles_from_view    If TRUE field alias in view is used as title
 * @param   array   $excluded_aliases    List of aliases excluded in export
 * @param   array   $used_aliases        List of (alias => column-title) to override all
 * @return  string
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

    return true;

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

    return '';

    }


/**
 * Creates a query statement and executes records fetching on it.
 *
 * This function allows fetching records one-by-one from a Janox view and returns each one
 * as an array, in the form:
 *
 *   array('FIELD_NAME1' => $vield_value1,
 *         'FIELD_NAME2' => $vield_value2,
 *         ...,
 *         'FIELD_NAMEn' => $vield_valueN)
 *
 * NOTE: fields names are always uppercase.
 *
 * This function can be used in this way:
 *
 *    while($record = o2view_fetch($view_name)) {
 *       $field = $record['FIELD'];
 *       }
 *
 * NOTE: If you unexpectedly stop fetching or you need to reset fetching, consider using
 *       o2view_fetch_free() to reset the statement.
 *
 * @package view
 * @param   string $view_name
 * @return  array
 * @see     o2view_fetch_free(), o2view_get_query()
 */
function o2view_fetch($view_name) {

    return array();

    }


/**
 * Resets and frees a view statement created for fetching rows
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_fetch(), o2view_get_query()
 */
function o2view_fetch_free($view_name) {

    return true;

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

    return true;

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

    return 1;

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

    return true;

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

    return true;

    }


/**
 * Unselect all selected rows of a view in a view-selector structure.
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_selector(), o2view_selector_all()
 */
function o2view_selector_none($view_name) {

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

    return true;

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

    return array();

    }


/**
 * Returns TRUE if the view is currently in snapshot mode, else FALSE.
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_snapshot_name(), o2view_snapshot_exists()
 */
function o2view_snapshot($view_name) {

    return true;

    }


/**
 * ATTENTION: Use of function o2view_snapshot_name() to set snapshot name is deprecated,
 *            use parameter on Snapshot-start or Snapshot-attach instructions instead.
 *            Function is still valid to get actual snapshot name from view.
 *
 * Returns or sets snapshot table name for the view.
 * When called without $snapshot_name parameter function returns the used snapshot table
 * name (useful in table expression for a view, to address other views snapshots).
 * When parameter $snapshot_name is passed then it is used as snapshot table name for the
 * view and can be the same for different views.
 *
 * @package view
 * @param   string $view_name
 * @param   string $snapshot_name
 * @return  string
 * @see     o2view_snapshot(), o2view_snapshot_exists()
 */
function o2view_snapshot_name($view_name, $snapshot_name = false) {

    return '';

    }


/**
 * Returns TRUE if the snapshot for the view $view_name already exists in session
 *
 * @package view
 * @param   string $view_name
 * @return  boolean
 * @see     o2view_snapshot(), o2view_snapshot_name()
 */
function o2view_snapshot_exists($view_name) {

    return true;

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

    return 1;

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

    return 100;

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

    return 100;

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

    return 100;

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

    return 100;

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
function o2form_maximize($form_name, $maximized = true) {

    return true;

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

    return array();

    }


/**
 * Creates directory if it does not exist
 *
 * @package filesystem
 * @param   string $dir_path
 * @return  boolean
 */
function o2dir_create($dir_path) {

    return true;

    }


/**
 * Removes a folder and all its content
 *
 * @package filesystem
 * @param   string $dir_path
 * @return  boolean
 */
function o2dir_delete($dir_path) {

    return true;

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

    return $file_path;

    }


/**
 * Returns file base name, without path and without extension
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_basename($file_path) {

    return $file_path;

    }


/**
 * Returns file extension without dot (.)
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_ext($file_path) {

    return $file_path;

    }


/**
 * Returns path to the folder containing file
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_dir($file_path) {

    return '';

    }


/**
 * Returns file type ([D]irectory - [F]file)
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_type($file_path) {

    return 'F';

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

    return '';

    }


/**
 * Returns file size in bytes
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_size($file_path) {

    return 50;

    }


/**
 * Returns file last modification date as o2-date
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_date($file_path) {

    return '20000101';

    }


/**
 * Returns file last modification time as o2-time
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_time($file_path) {

    return '000000';

    }


/**
 * Returns true if file exists, else false
 *
 * @package filesystem
 * @param   string $file_path
 * @return  string
 */
function o2file_exists($file_path) {

    return true;

    }


/**
 * Physically removes file from file system
 *
 * @package filesystem
 * @param   string $file_path
 * @return  boolean
 */
function o2file_delete($file_path) {

    return true;

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

    return true;

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

    return true;

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

    return $file_path;

    }


// ====================================== ASP ============================================

/**
 * Returns the current ASP-ID set for the session.
 *
 * @package asp
 * @return  string
 */
function o2asp_code_get() {

    return '';

    }


/**
 * Set the new ASP-ID for the session and returns the old one.
 *
 * @package asp
 * @param   string $new_aspid
 * @return  string
 */
function o2asp_code_set($new_aspid) {

    return '';

    }


/**
 * Returns the list of ASP-IDs defined in users table.
 *
 * @package asp
 * @return  array
 */
function o2asp_codes() {

    return array();

    }


// =================================== DISPATCHER ========================================

/**
 * Returns the array of all dispatches IDs in the dispatcher list.
 *
 * @package dispatcher
 * @return  array
 */
function o2dispatcher_get_list() {

    return array();

    }


/**
 * Returns the number of dispatches in the dispatcher list.
 *
 * @package dispatcher
 * @return  integer
 */
function o2dispatcher_count() {

    return 1;

    }


/**
 * Removes all dispatches from the list.
 * Returns TRUE on success, FALSE on fail.
 *
 * @package dispatcher
 * @return  boolean
 */
function o2dispatcher_clear() {

    return true;

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

    return true;

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

    return 1;

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

    return '';

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

    return true;

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

    return true;

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

    return array();

    }


/**
 * Returns TRUE if dispatch with requested ID has an activation program.
 *
 * @package dispatcher
 * @param   integer $id   Dispatch ID to check
 * @return  boolean
 */
function o2dispatch_can_activate($id) {

    return true;

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

    return true;

    }


// ================================= TREEVIEW CONTROL ====================================


/**
 *
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
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

    return new stdClass();

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
 * Destroys an existing treeview control in the current program
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2tree_destroy($name) {

    return true;

    }


/**
 * DEPRECATED!
 * This function is deprecated.
 * Use o2tree_expand() instead.
 *
 * Fold all foldable nodes in treeview down to $level.
 * If $level is 0 all folders are expanded.
 *
 * @package treeview
 * @param   string  $name    Control unique name
 * @param   integer $level   Folding level to fold down to
 * @return  boolean
 */
function o2tree_fold($name, $level = 0) {

    return true;

    }


/**
 * DEPRECATED!
 * This function is deprecated.
 * Use o2tree_collapse() instead.
 *
 * Unfold all foldable nodes in treeview
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2tree_unfold($name) {

    return true;

    }


/**
 * Expand all expandable nodes in treeview down to $level.
 * If $level is 0 all folders are expanded.
 *
 * @package treeview
 * @param   string  $name    Control unique name
 * @param   integer $level   Folding level to fold down to
 * @return  boolean
 */
function o2tree_expand($name, $level = 0) {

    return true;

    }


/**
 * Collapse all foldable nodes in treeview
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2tree_collapse($name) {

    return true;

    }


/**
 * DEPRECATE!
 * This function is deprecated.
 * Use o2tree_node_expand() instead.
 *
 * Fold a node in treeview.
 *
 * @package treeview
 * @param   string  $name      Control unique name
 * @param   mix     $node_id   Folding node ID
 * @return  boolean
 */
function o2tree_node_fold($name, $node_id) {

    return true;

    }


/**
 * DEPRECATED!
 * This function is deprecated.
 * Use o2tree_node_collapse() instead.
 *
 * Unfold a node in treeview.
 *
 * @package treeview
 * @param   string $name      Control unique name
 * @param   mix    $node_id   Unfolding node ID
 * @return  boolean
 */
function o2tree_node_unfold($name, $node_id) {

    return true;

    }


/**
 * Fold a node in treeview.
 *
 * @package treeview
 * @param   string  $name      Control unique name
 * @param   mix     $node_id   Folding node ID
 * @return  boolean
 */
function o2tree_node_expand($name, $node_id) {

    return true;

    }


/**
 * Unfold a node in treeview
 *
 * @package treeview
 * @param   string $name      Control unique name
 * @param   mix    $node_id   Unfolding node ID
 * @return  boolean
 */
function o2tree_node_collapse($name, $node_id) {

    return true;

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
 * Select a node in treeview and make it visible
 *
 * @package treeview
 * @param   string $name      Treeview control unique name
 * @param   string $node_id   Node id to be selected
 * @return  boolean
 */
function o2tree_node_select($name, $node_id) {

    return true;

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
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

    return true;

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
 * Removes a node and all its sub-nodes from a treeview.
 * Function returns tha array of all removed nodes.
 *
 * @package treeview
 * @param   string $name        Control unique name
 * @param   string $node_id     Node unique ID
 * @return  array
 */
function o2tree_node_remove($name, $node_id) {

    return true;

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

    return true;

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
 * Returns an array of the treeview nodes structure
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  array
 */
function o2tree_get_nodes($name) {

    return array();

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
 * Returns the ID of the selected node in treeview
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  string
 */
function o2tree_get_selection($name) {

    return '1';

    }


/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
 * Returns the HTML code for defined Treeview control $name
 *
 * @package treeview
 * @param   string $name     Control unique name
 * @return  string
 */
function o2tree_get_code($name) {

    return '';

    }



/**
 * ATTENTION: Treeview scripting definition and usage are DEPRECATED. Use a Treeview
 *            control from development interface (JXOB) instead.
 *
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

    return true;

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
                       $csv_file_name    = '',
                       $fields_separator = ',',
                       $text_delimiter   = '"') {

    return '';

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

    return true;

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

    return new stdClass();

    }


/**
 * Destroys an existing images lister control in the current program
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  boolean
 */
function o2imglist_destroy($name) {

    return true;

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

    return true;

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

    return true;

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

    return true;

    }


/**
 * Returns an array of the images lister items list
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  array
 */
function o2imglist_get_items($name) {

    return array();

    }


/**
 * Returns the HTML code for defined images lister control $name
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  string
 */
function o2imglist_get_code($name) {

    return '';

    }


/**
 * ATTENTION: Images-lister scripting definition and usage are DEPRECATED. Use a
 *            Images-lister control from development interface (JXOB) instead.
 *
 * Returns the ID of the selected item in images lister
 *
 * @package imglist
 * @param   string $name     Control unique name
 * @return  string
 */
function o2imglist_get_selection($name) {

    return '';

    }


// =============================== PROGRESS BAR CONTROL ==================================

/**
 * ATTENTION: Progress-bar scripting definition and usage are DEPRECATED. Use a
 *            Progress-bar control from development interface (JXOB) instead.
 *
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

    return '';

    }


/**
 * ATTENTION: Progress-bar scripting definition and usage are DEPRECATED. Use a
 *            Progress-bar control from development interface (JXOB) instead.
 *
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

    return '';

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
    $GLOBALS['reference_vars'][$name] = $value;

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
    return $GLOBALS['reference_vars'][$name];

    }


// ==================================== LAST ERROR =======================================

/**
 * Clears last error informations
 *
 * @package error
 */
function o2error_lastclear() {

    return true;

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

    return 0;

    }


/**
 * Returns last error numeric code, FALSE if no error is on.
 *
 * @package error
 * @return  integer
 */
function o2error_getlastcode() {

    return false;

    }


/**
 * Returns last error error type string, FALSE if no error is on.
 *
 * @package error
 * @return  string
 */
function o2error_getlastclass() {

    return false;

    }


/**
 * Returns last error text message, FALSE if no error is on.
 *
 * @package error
 * @return  string
 */
function o2error_getlastmsg() {

    return '';

    }


/**
 * Returns last error complete text, as it appears in errors.log, FALSE if no error is on.
 *
 * @package error
 * @return  string
 */
function o2error_getlasttxt() {

    return false;

    }


/**
 * Returns last error complete HTML, as it appears in error-boxes, FALSE if no error is
 * on.
 *
 * @package error
 * @return  string
 */
function o2error_getlasthtml() {

    return '';

    }


// ================================== LAST EXCEPTION =====================================

/**
 * Return last catched error as an o2_exception object.
 * This function is intended to be used inside catch-actions, to get the fired error.
 * Returned object is always of class "o2_exception", regardless the catched error type.
 *
 * Tip: use o2exception_get()->getPrevious() to get the original catched Throwable object.
 *
 * @package error
 * return   o2_exception
 */
function o2exception_get() {

    return new stdClass();

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

    return $text;

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

    return $text_left.' '.$text_right;

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

    return $text;

    }


function jxsql_parse($dbname, $sql, $fq_columns = false) {

    return '';

    }


function jxsql($dbname, $sql, $fq_columns = false) {

    return '';

    }

?>
