<?php

/**
 * Janox Application Test
 * PHP7
 *
 * @name      jxtest
 * @package   janox/jxtest.php
 * @version   2.9
 * @copyright Tommaso Vannini (tvannini@janox.it) 2024
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

 $info  = <<<JANOX_SCRIPT_HEAD

 Janox Application Test


This file is part of Janox.

- LICENSE -

Janox is free software; you can redistribute  it  and/or
modify it under the  terms  of  the  GNU  Lesser General
Public License  as  published   by   the   Free Software
Foundation; either version 3 of the License, or (at your
option) any later version.

Janox is distributed in the hope that it will be useful,
but WITHOUT  ANY  WARRANTY;  without  even  the  implied
warranty of MERCHANTABILITY or FITNESS FOR A  PARTICULAR
PURPOSE. See the  GNU Lesser General Public License  for
more details.

You should have  received  a  copy  of  the  GNU  Lesser
General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

- DESCRIPTION -

This script contains needed functions to  test  a  Janox
application expressions for errors.
Errors are reported according with "error-mode" setting.

- USAGE -

You can run jxtest.php using a working PHP installation,
typing at the command line:

<php_dir>/php <jxroot>/jxtest.php <main_file> [<mode>]

Where <main_file> is the main file  full  path  for  the
application you want to test.

Optional  parameter  <mode>  can  be  passed  to   force
error-mode check (DEV|EXE).

If <mode> is not passed then application error-mode will
be used (from application .INI settings).

Script will output errors to console, so redirect output
to file to get a persistent log.

JANOX_SCRIPT_HEAD;


// ========================================================= BUILD APPLICATION CONTEXT ===
$params = $_SERVER['argv'];
if (!isset($params[1])) {
    die($info."\n");
    }
// ___________________________________________________________ Application main script ___
$app    = $params[1];
if (!file_exists($app)) {
    die ("\nERROR:\nCan't find application main script in ".$app."\n\n");
    }
$err_mode = (isset($params[2]) ? (strtoupper(substr($params[2], 0, 1)) == 'D' ?
             'DEV' : 'EXE') : false);
print '# '.$err_mode."\n";
$app_name = pathinfo($app, PATHINFO_FILENAME);
// ___________________________________________________________ Application directories ___
$dir_root = dirname(dirname($app)).DIRECTORY_SEPARATOR;
$dir_prgs = $dir_root.'prgs'.DIRECTORY_SEPARATOR;
// _______________________________________________________________ Application options ___
$app_pars = parse_ini_file($dir_root.$app_name.'.ini');
$encoding = (isset($app_pars['encoding']) ? $app_pars['encoding'] : 'windows-1252');
$err_mode = ($err_mode ? $err_mode :
            (isset($app_pars['error_mode']) ? $app_pars['error_mode'] : ''));
$f_models = (isset($app_pars['models']) ? $app_pars['models'] : 'mod_repository.inc');
$f_vars   = (isset($app_pars['vars']) ? $app_pars['vars'] : 'var_repository.inc');
$f_tabs   = (isset($app_pars['tables']) ? $app_pars['tables'] : 'file_repository.inc');
// __________________________________________________________ Application repositories ___
$models   = app_get_models($dir_prgs.$f_models);
$vars     = app_get_vars($dir_prgs.$f_vars, $models);
$fields   = app_get_fields($dir_prgs.$f_tabs, $models);
$prgs     = app_get_prgs($dir_prgs);
// ______________________________________________________ Load list of exclueded words ___
$exc_file = __DIR__.DIRECTORY_SEPARATOR.'jxtest.exclude';
$excluded = array();
if (file_exists($exc_file)) {
    $excluded = file($exc_file, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    }
// _____________________________________________________________________ Start logging ___
print '*** Janox Test application "'.$app_name.'" from '.$dir_root."\n".
      '    Test with error-mode='.$err_mode.' on '.count($prgs)." programs:\n\n";
$start_time = time();
$err_count  = 0;
$err_prgs   = 0;
// ============================================= LOOP ON PROGRAMS TO CHECK EXPRESSIONS ===
foreach ($prgs as $prg_name => $f_prf) {
    if ($prg_name != '_o2viewmodels') {
        $errs = prg_check($f_prf, $vars, $fields, $models, $excluded, $err_mode);
        if ($errs > 0) {
            $err_prgs++;
            $err_count+= $errs;
            }
        }
    }
// _______________________________________________________________________ End logging ___
$exe_time = (time() - $start_time);
// _________________________________________ Less then 3 minutes, show time in seconds ___
if ($exe_time < 180) {
    $time = $exe_time.' seconds';
    }
// ___________________________________________ Less then 3 hours, show time in minutes ___
elseif ($exe_time < 10800) {
    $time = intval($exe_time / 60).' minutes';
    }
// ________________________________________________________________ Show time in hours ___
else {
    $time = intval($exe_time / 3600).' hours';
    }
print '*** Janox Test - Execution correctly ended in '.$time.
      "\n    Found ".$err_count.' problems in '.$err_prgs." programs.\n\n";


/**
 * Sets error mode. Possible modes are:
 *  none   PHP settings are used from php.ini
 *  DEV    Development mode
 *  EXE    Production mode
 *
 *  @param $string    $err_mode
 */
function error_mode($error_mode) {

    $ret = '';
    switch ($error_mode) {
        case 'DEV':
            $ret = 'error_reporting(E_ALL & ~E_STRICT & ~E_NOTICE); ';
            break;
        case 'EXE':
            $ret = 'error_reporting(E_ALL & ~E_WARNING & ~E_DEPRECATED & ~E_STRICT & '.
                   '~E_NOTICE); ';
            break;
        }
    $ret.= 'ini_set("display_errors", false); ini_set("log_errors", true); ';
    return $ret;

    }


/**
 * Read application and runtime models and return them in the form:
 *
 *  array("name1" => type1,
 *        "name2" => type2,
 *        ...,
 *        "namen" => typen);
 *
 *  where "type*" is "A|N|D|O|L|S".
 *
 *  @param  string $models_file   Models repository
 *  @return array
 */
function app_get_models($models_file) {

    // ___________________________________________________ Read application repository ___
    $code  = file_get_contents($models_file);
    $parts = array();
    $regex = '/o2def::model\(\s*["\'](\w*)["\']\s*,\s*["\'](\w*)["\'][^;]*\);/';
    preg_match_all($regex, $code, $parts);
    // _______________________________________________________ Read runtime repository ___
    $code   = file_get_contents(__DIR__.DIRECTORY_SEPARATOR.'lib'.DIRECTORY_SEPARATOR.
                                'prgs'.DIRECTORY_SEPARATOR.'models.inc');
    $parts2 = array();
    preg_match_all($regex, $code, $parts2);
    return array_merge(array_combine($parts2[1], $parts2[2]),
                       array_combine($parts[1], $parts[2]));

    }


/**
 * Read application variables and return them in the form:
 *
 *  array("name1" => type1,
 *        "name2" => type2,
 *        ...,
 *        "namen" => typen);
 *
 *  where "type*" is "A|N|D|O|L|S".
 *
 *  @param  string $vars_file   Application variables repository
 *  @param  array  $models      List of models with their types
 *  @return array
 */
function app_get_vars($vars_file, $models) {

    $code  = file_get_contents($vars_file);
    $res   = array();
    $parts = array();
    preg_match_all('/o2def::appvar\(\s*["\'](\w*)["\']\s*,\s*["\'](\w*)["\']\s*\);/',
                   $code, $parts);
    foreach ($parts[1] as $idx => $var) {
        $res[$var] = (isset($models[$parts[2][$idx]]) ? $models[$parts[2][$idx]] : 'A');
        }
    return $res;

    }


/**
 * Return list of application programs definition scripts (PRF) in the form:
 *
 *  array('prg1' => 'path1',
 *        'prg2' => 'path2',
 *        ...,
 *        'prgn' => 'pathn');
 *
 * where "prg*" is the program name and "path*" is the PRF script path.
 *
 * @param  string $prgs_dir   Application "prgs" directory
 * @return array
 */
function app_get_prgs($prgs_dir) {

    // ___________________________________________________ Load standard programs list ___
    $prgs       = array();
    $prgs_names = glob($prgs_dir.'*.prf');
    if ($prgs_names) {
        foreach ($prgs_names as $prg) {
            $prgs[pathinfo($prg, PATHINFO_FILENAME)] = $prg;
            }
        }
    return $prgs;

    }


/**
 * Reads all tables and fields to look for data-types and returns an array in the
 * form:
 *
 * [table1 =>
 *  [field11 => type11,
 *   field12 => type12,
 *   ...],
 *  table2 =>
 *   [field21 => type21,
 *    field22 => type22,
 *    ...],
 *  ...]
 *
 * @param  string $tabs_file   Application tables repository
 * @param  array  $models      List of models with their types
 * @return array
 */
function app_get_fields($tabs_file, $models) {

    $code  = file_get_contents($tabs_file);
    $ret   = array();
    $parts = array();
    $res   = preg_match_all('/o2def::tab\(["\'](\w*)["\'],[^;]*\);\s+(o2def::field\('.
                            '[^;]+\);\s+)++o2def::index/',
                            $code, $parts);
    if ($res) {
        for ($i = 0; $i < $res; $i++) {
            // ________________________________________________ Search fields by table ___
            $table     = $parts[1][$i];
            $match     = '/o2def::field\(["\'](\w*)["\']\s*,\s*["\']\w*["\']\s*,'.
                         '\s*["\'](\w*)["\']\);/';
            $partsflds = array();
            $resflds   = preg_match_all($match, $parts[0][$i], $partsflds);
            if ($resflds) {
                for ($j = 0; $j < $resflds; $j++) {
                    $field = $partsflds[1][$j];
                    $model = $partsflds[2][$j];
                    if (!isset($ret[$table])) {
                        $ret[$table] = array();
                        }
                    $ret[$table][$field] = (isset($models[$model]) ?
                                            $models[$model] : 'A');
                    }
                }
            }
        }
    return $ret;

    }


/**
 * Reads program parameters and returns a list in the form:
 *
 *  [pram-id => type]
 *
 * @param  string $prf_path   Path to program definition script (PRF)
 * @param  array  $models     List of models with their types
 * @return array
 */
function prg_get_parameters($prf_path, $models) {

    $prg = substr($prf_path, 0, -1)."g";
    $ret = array();
    if (file_exists($prg)) {
        $parts = array();
        $res   = preg_match_all('/o2def::par\((\d*),\s*["\'][^"]*["\']\s*,'.
                                '\s*["\']([^"]*)["\']\);/',
                                file_get_contents($prg), $parts);
        if ($res) {
            for ($i = 0; $i < $res; $i++) {
                $param       = $parts[1][$i];
                $model       = $parts[2][$i];
                $ret[$param] = (isset($models[$model]) ? $models[$model] : 'A');
                }
            }
        }
    return $ret;

    }


/**
 * Reads program variables and returns a list in the form:
 *
 *  [variable => type]
 *
 * NOTE:
 *
 * Syntax "prg[^-]*var" is used to avoid problems with UTF8/Win1252 ecncoding problems.
 *
 * @param  string $prf_code   Program definition script (PRF)
 * @param  array  $models     List of models with their types
 * @return array
 */
function prg_get_variables($prf_code, $models) {

    $parts = array();
    $res  = preg_match_all('/\$task_prg[^-]*var\-\>definisci\(["\'](\w*)["\']\s*,\s*'.
                            '["\'](\w*)["\']\);/',
                            $prf_code, $parts);
    $ret = array();
    if ($res) {
        for ($i = 0; $i < $res; $i++) {
            $variable       = $parts[1][$i];
            $model          = $parts[2][$i];
            $ret[$variable] = (isset($models[$model]) ? $models[$model] : 'A');
            }
        }
    return $ret;

    }


/**
 * Reads program tables from views and returns a list in the form:
 *
 *  [view => [tab_alias => tab_name, ...], ...]
 *
 * @param  string $prf_code   Program definition script (PRF)
 * @return array
 */
function prg_get_tabs($prf_code) {

    $parts = array();
    $res   = preg_match_all('/\$task_(\w*)\-\>usa_file(_link)*\(\s*["\'](\w*)["\']\s*,'.
                            '\s*["\'](\w*)["\']\s*,/',
                            $prf_code, $parts);
    $ret   = array();
    if ($res) {
        for ($i = 0; $i < $res; $i++) {
            $tab_view     = $parts[1][$i];
            $tab_name     = $parts[3][$i];
            $tab_alias    = $parts[4][$i];
            if (!isset($ret[$tab_view])) {
                $ret[$tab_view] = array();
                }
            $ret[$tab_view][$tab_alias] = $tab_name;
            }
        }
    return $ret;

    }


/**
 * Reads program fields from views and returns a list in the form:
 *
 *  [view => [field => type, ...], ...]
 *
 * @param  string $prf_code   Program definition script (PRF)
 * @param  array  $fields     List of fields form tables repository
 * @param  array  $models     List of models with their types
 * @return array
 */
function prg_get_fields($prf_code, $fields, $models) {

    $prg_tabs = prg_get_tabs($prf_code);

    $parts = array();
    $res   = preg_match_all('/\$task_(\w*)\-\>usa\(["\'](\w*)["\']\s*,'.
                            '\s*["\'](\w*)["\']\s*,\s*["\'](\w*)["\']\s*,/',
                            $prf_code, $parts);
    $ret   = array();
    if ($res) {
        for ($i = 0; $i < $res; $i++) {
            $tab_view     = $parts[1][$i];
            $tab_alias    = $parts[3][$i];
            $field_name   = $parts[4][$i];
            $field_alias  = $parts[2][$i];
            if (!isset($ret[$tab_view])) {
                $ret[$tab_view] = array();
                }
            if (isset($prg_tabs[$tab_view][$tab_alias])) {
                $rep_tab = $prg_tabs[$tab_view][$tab_alias];
                if (isset($fields[$rep_tab][$field_name])) {
                    $type = $fields[$rep_tab][$field_name];
                    }
                else {
                    $type = 'A';
                    }
                $ret[$tab_view][$field_alias] = $type;
                }
            }
        }
    return $ret;

    }


/**
 * Reads program formulas from views and returns a list in the form:
 *
 *  [view => [formula => type, ...], ...]
 *
 * @param  string $prf_code   Program definition script (PRF)
 * @param  array  $models     List of models with their types
 * @return array
 */
function prg_get_formulas($prf_code, $models) {

    $parts = array();
    $res   = preg_match_all('/\$task_(\w*)\-\>(calcola|sql_formula)\(["\'](\w*)["\']\s*,'.
                            '\s*["\'](\w*)["\']\s*,/',
                            $prf_code, $parts);
    $ret   = array();
    if ($res) {
        for ($i = 0; $i < $res; $i++) {
            $view    = $parts[1][$i];
            $formula = $parts[3][$i];
            $model   = $parts[4][$i];
            if (!isset($ret[$view])) {
                $ret[$view] = array();
                }
            $type = (isset($models[$model]) ? $models[$model] : 'A');
            $ret[$view][$formula] = $type;
            }
        }
    return $ret;

    }


/**
 * Reads program selects (fields and formulas) from views and returns a list in the form:
 *
 *  [view => [select => type, ...], ...]
 *
 * @param  string $prf_code   Program definition script (PRF)
 * @param  array  $fields     List of fields form tables repository
 * @param  array  $models     List of models with their types
 * @return array
 */
function prg_get_selects($prf_code, $fields, $models) {

    return array_merge_recursive(prg_get_fields($prf_code, $fields, $models),
                                 prg_get_formulas($prf_code, $models));

    }


/**
 * Reads program expressions and returns a list in the form:
 *
 *  [exp_name => body]
 *
 * @param  String $prg_name   Program name
 * @param  string $prf_code   Program definition script (PRF)
 * @return array
 */
function prg_get_exps($prg_name, $prf_code) {


    $ret = array();
    $res = preg_split('/function '.$prg_name.'_exp_(\d+)\(\) \{/', $prf_code, -1,
                      PREG_SPLIT_DELIM_CAPTURE);
    if ($res) {
        array_shift($res);
        $yorn = true;
        foreach ($res as $id => $part) {
            if ($yorn) {
                $exp       = $prg_name.'_exp_'.$part;
                $expbody   = substr($res[$id + 1], 0, strrpos($res[$id + 1], "}"));
                $ret[$exp] = $expbody;
                }
            $yorn = !$yorn;
            }
        }
    return $ret;

    }


/**
 * Log informations about catched error
 *
 * @param  string    $log_text   File path to log to
 * @param  Throwable $error      Data to log to file
 * @return integer               "1" if error is logged, "0" if error is skippeed
 */
function test_log($msg, $prg_name, $exp) {

    // ______________________________________________ Exclude some errors from logging ___
    // _____________________________________________________ Call to a member function ___
    if (strpos($msg, 'Call to a member function') !== false) {
        return 0;
        }
    // ____________________________________________________ Call to undefined function ___
    elseif (strpos($msg, 'Call to undefined function') !== false) {
        return 0;
        }
    // _______________________________________________________ Call to undefined class ___
    elseif (strpos($msg, 'Class') !== false && strpos($msg, 'not found') !== false) {
        return 0;
        }
    print '[ERROR] Program "'.$prg_name.
          '" expression '.substr($exp, strrpos($exp, '_') + 1).":\n".
          $msg."\n\n";
    return 1;

    }


/**
 * Check single program against application context
 *
 * @param string  $f_prf         Path to program definition script (PRF)
 * @pram  array   $app_vars      List of application variables with types
 * @param array   $fields        List of application repository fields with types
 * @param array   $models        List of application models with types
 * @param array   $excluded      List of excluded words to skip expressions
 * @param string  $err_mode      Error mode to performe the test (DEV|EXE)
 */
function prg_check($f_prf, $app_vars, $fields, $models, $excluded, $error_mode) {

    $prg_name    = pathinfo($f_prf, PATHINFO_FILENAME);
    $prf_code    = file_get_contents($f_prf);
    $prg_pars    = prg_get_parameters($f_prf, $models);
    $prg_vars    = prg_get_variables($prf_code, $models);
    $prg_selects = prg_get_selects($prf_code, $fields, $models);
    $prg_exps    = prg_get_exps($prg_name, $prf_code);
    // _______________________ Context for application and program in including script ___
    $code = "<?php\n".
            "ini_set('max_execution_time', 10);\n".
            '$GLOBALS["jxt-context"] = array("A" => '.var_export($app_vars, 1).','.
                                            '"P" => '.var_export($prg_pars, 1).','.
                                            '"V" => '.var_export($prg_vars, 1).','.
                                            '"S" => '.var_export($prg_selects, 1).");\n".
            error_mode($error_mode)."\n".
            'include "'.__DIR__.DIRECTORY_SEPARATOR.'lib'.
                                DIRECTORY_SEPARATOR.'jxtest.inc";'."\n".
            substr($prf_code, strpos($prf_code, 'function '.$prg_name.'_exp_'));
    $inc  = dirname($f_prf).DIRECTORY_SEPARATOR.'jxtest.inc';
    file_put_contents($inc, $code);
    $error_count = 0;
    foreach ($prg_exps as $exp => $body) {
        // ________________________________ Skip expressions containing excluded words ___
        if ($body === str_ireplace($excluded, 'JXTEST', $body))  {
            // ________________________________________________ test single expression ___
            $error_count+= test_exp($prg_name, $exp, $inc);
            }
        }
    return $error_count;

    }


/**
 * Test expression against application and program context
 *
 * @param  string $prg_name   Name of the program
 * @param  string $exp        Expression function name
 * @param  string $inc        File path to include to get application and program context
 * @return integer            "1" if error is logged, "0" if error is skippeed
 */
function test_exp($prg_name, $exp, $inc) {

    exec(PHP_BINARY.' -r '.escapeshellarg('include "'.$inc.'"; '.$exp.'();').' 2>&1',
         $output, $ret);
    $output = implode("\n", $output);
    // __________________________________________________________________ Fatal errors ___
    if ($ret) {
        // _________________________________________________________ Skip fatal errors ___
        }
    // __________________________________________________ Handable errors and warnings ___
    elseif (strpos($output, 'PHP') !== false) {
        $msg = substr($output, strpos($output, 'PHP'));
        $msg = substr($msg, 0, strpos($msg, "\n"));
        if (strpos($msg, ' in '.$inc)) {
            $msg = substr($msg, 0, strpos($msg, ' in '.$inc));
            }
        return test_log($msg, $prg_name, $exp);
        }
    return 0;

    }

?>