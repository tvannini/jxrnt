<?php

/**
 * Janox Cron Server
 * PHP7/8
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
 * JXCron script
 * =============
 *
 * This script works both as a cron/task service and as a service controller.
 *
 *
 * @name      jxcron.php
 * @package   janox/jxcron.php
 * @version   3.0
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007-2025
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * SETTINGS
 * ========
 *
 * @var string $jxrnt_path   path to Janox runtime folder
 */
$jxrnt_path = __DIR__.DIRECTORY_SEPARATOR;


/**
 * REGISTERED APPLICATIONS
 * =======================
 *
 * To register an application to be served by Janox Cron Server just add it to the array
 * below, in the form:
 *
 * $apps = ['app-name1' => 'app-root1',
 *          'app-name2' => 'app-root2',
 *          ...
 *          'app-nameN' => 'app-rootN'];
 *
 * Paths can be absolute or relative to Janox Cron Server script location.
 *
 * SAMPLE:
 *
 * $apps = ['jxdemo' => __DIR__.DIRECTORY_SEPARATOR.'demo'.DIRECTORY_SEPARATOR,
 *         'myapp '  => <path-to-myapp>.DIRECTORY_SEPARATOR.'myapp'.DIRECTORY_SEPARATOR];
 *
 *
 * @var array $apps   Provides a list of served applications with name and folder
 */
$apps = [];


/**
 * CRON FREQUENCY
 * ==============
 *
 * Set frequency for Janox Cron Server to run processes.
 *
 * @var int $freq   Frequency in seconds for Janox Cron Server
 */
$freq = 10;


// __________________________________________________________ Set down error reporting ___
error_reporting(E_ALL & ~E_WARNING & ~E_DEPRECATED & ~E_NOTICE);
ini_set('display_errors', false);
ini_set('log_errors', true);


/**
 * Script start
 */
print "Janox Cron Server\n=================\n";


/**
 * Configuration as changed from last run
 * ======================================
 */
$conf_changed = false;



/**
 * Load configuration from INI file
 * ================================
 *
 * If file "jxcron.ini" exists in the same folder as this script, configuration settings
 * will load from it, overriding default settings.
 *
 */
if (file_exists(__DIR__.DIRECTORY_SEPARATOR.basename(__FILE__, '.php').'.ini')) {
    $conf = parse_ini_file(__DIR__.DIRECTORY_SEPARATOR.basename(__FILE__, '.php').'.ini',
                           true);
    print 'Configuration loaded from INI file: '.
          __DIR__.DIRECTORY_SEPARATOR.basename(__FILE__, '.php').".ini\n";
    // ____________________________________________ Override default frequency setting ___
    if (isset($conf['frequency']) && (intval($conf['frequency']) > 0)) {
        $freq = intval($conf['frequency']);
        }
    // ______________________________________________ Override default runtime setting ___
    if (isset($conf['jxrnt']) && (trim($conf['jxrnt']) != '')) {
        $jxrnt_path = trim($conf['jxrnt']);
        // ________________________________ Accept both path to runtime file or folder ___
        if (substr($jxrnt_path, -9) != 'jxrnt.php') {
            if (substr($jxrnt_path, -1) != '/' &&
                substr($jxrnt_path, -1) != '\\') {
                $jxrnt_path .= DIRECTORY_SEPARATOR;
                }
            }
        else {
            $jxrnt_path = dirname($jxrnt_path).DIRECTORY_SEPARATOR;
            }
        if (substr($jxrnt_path, 0, 1) == '.') {
            $jxrnt_path = realpath(__DIR__.DIRECTORY_SEPARATOR.$jxrnt_path).
                          DIRECTORY_SEPARATOR;
            }
        }
    // __________________________________________ Override served applications setting ___
    if (isset($conf['APPS']) && count($conf['APPS']) > 0) {
        $apps = [];
        foreach ($conf['APPS'] as $app_name => $app_dir) {
            $app_dir = trim($app_dir);
            if (substr($app_dir, 0, 1) == '.') {
                $app_dir = realpath(__DIR__.DIRECTORY_SEPARATOR.$app_dir).
                           DIRECTORY_SEPARATOR;
                }
            $apps[$app_name] = $app_dir;
            }
        }
    }


// _____________________________________________________________ Include Janox runtime ___
include_once $jxrnt_path.'jxrnt.php';


/**
 * Returns PID-file Name
 *
 */
function pid_file() {

    return __DIR__.DIRECTORY_SEPARATOR.'.'.o2file_basename(__FILE__);

    }


/**
 * Start Janox Cron Server.
 *
 */
function srvstart() {

    if (!srvcheck('start')) {
        // _______________________________________________ Check applications to serve ___
        if (count($GLOBALS['apps']) < 1) {
            print "Janox Cron Server: no application to serve.\n".
                  "Please set \$apps variable in this file (".__file__.").\n";
            die();
            }
        // _______________________________________________________ Check Janox runtime ___
        if (isset($GLOBALS['o2_runtime']) && is_a($GLOBALS['o2_runtime'], 'o2_runtime')) {
            $rnt_obj = $GLOBALS['o2_runtime'];
            if (!$rnt_obj->php_engine || !file_exists($rnt_obj->php_engine)) {
                $rnt_obj->find_php_exe();
                }
            }
        else {
            print "Sorry, can't find Janox runtime to run Janox Cron Server.\n".
                  "Please set \$jxrnt_path variable in this file (".__file__.").\n";
            die();
            }
        // ___________________________________________________ Start Janox Cron Server ___
        $list_old = $rnt_obj->proc_list(true);
        $rnt_obj->batch_exec($rnt_obj->php_engine.' '.__FILE__.' run-sched');
        // __________________________________________________________ Check if started ___
        $list    = $rnt_obj->proc_list(true);
        $running = false;
        foreach (array_diff_key($list, $list_old) as $pid => $exe_name) {
            if (!strpos($exe_name, 'fpm')) {
                $running = $pid;
                break;
                }
            }
        //____________________________________________________________________ Started ___
        if ($running) {
            // _______________ Write PID-file with running PID and configuration check ___
            file_put_contents(pid_file(), $running.' '.md5(serialize($GLOBALS['apps'])));
            print "Janox Cron Server started with PID ".$running.
                  "\nServing applications:\n";
            foreach ($GLOBALS['apps'] as $app_name => $app_dir) {
                print ' '.str_pad($app_name, 20, '.').' from '.$app_dir."\n";
                }
            }
        // _______________________________________________________________ Not started ___
        else {
            print "Sorry, unable to start Janox Cron Server.\n";
            }
        }

    }


/**
 * Stop Janox Cron Server.
 *
 */
function srvstop() {

    // _________________________________________________________ Check for running PID ___
    if ($r_pid = srvcheck('stop')) {
        $rnt_obj = $GLOBALS['o2_runtime'];
        $list    = $rnt_obj->proc_list(true);
        // _____________________ Check if running PID in the list of running processes ___
        foreach ($list as $pid => $exe_name) {
            if ($pid == $r_pid) {
                $rnt_obj->kill($pid);
                o2file_delete(pid_file());
                print "Janox Cron Server stopped.\n";
                break;
                }
            }
        }

    }


/**
 * Check if Janox Cron Server is running.
 *
 */
function srvcheck($for = '') {

    $rnt_obj  = $GLOBALS['o2_runtime'];
    $pid_file = pid_file();
    $running  = false;
    // ______________________________________________________ Check if PID-file exists ___
    if (o2file_exists($pid_file)) {
        // _________________________________ Get running PID and configuration settings___
        list($r_pid, $md5_check) = explode(' ', file_get_contents($pid_file));
        // _______________________________ Check if MD5 matches configuration settings ___
        if ($md5_check != md5(serialize($GLOBALS['apps']))) {
            if ($for != 'stop') {
                print "Janox Cron Server configuration has changed since last run.\n".
                      "Please stop the server and start it again to apply new settings.\n";
                die();
                }
            }
        // _____________________ Check if running PID in the list of running processes ___
        $list  = $rnt_obj->proc_list(true);
        foreach ($list as $pid => $exe_name) {
            if ($pid == $r_pid) {
                $running = $pid;
                break;
                }
            }
        // ______________________________________________ Janox Cron Server is running ___
        if ($running) {
            print "Janox Cron Server is running with PID ".$running.
                  "\nServing applications:\n";
            foreach ($GLOBALS['apps'] as $app_name => $app_dir) {
                print ' '.str_pad($app_name, 20, '.').' from '.$app_dir."\n";
                }
            return $running;
            }
        else {
            print "Janox Cron Server is not running.\n";
            o2file_delete($pid_file);
            return false;
            }
        }
    else {
        print "Janox Cron Server is not running.\n";
        return false;
        }

    }


/**
 * RUN
 * ===
 *
 * Check command line parameter to run Janox Cron Server or to control it.
 *
 * Parameter "run-sched" is used internally to run the cron server process.
 *
 */
switch (strtolower(trim($_SERVER['argv'][1]))) {
    /**
     * RUN AS CRON SERVICE
     * ===================
     *
     */
    case 'run-sched':
        $rnt_obj = $GLOBALS['o2_runtime'];
        // _____________________________________________ Run Janox Cron Server process ___
        while (true) {
            foreach ($apps as $app_name => $app_dir) {
                $rnt_obj->batch_exec($rnt_obj->php_engine.' '.
                                  $app_dir.'htdocs'.DIRECTORY_SEPARATOR.$app_name.'.php '.
                                     'jxrnt='.$jxrnt_path.'jxrnt.php user=jxsys jxsched');
                }
            // ___________________________________________________ Wait for next cycle ___
            sleep($GLOBALS['freq']);
            }
        break;
    /**
     * RUN AS SERVICE CONTROLLER
     * =========================
     *
     * If called without parameters or with command "START" it will try to start the Janox
     * Cron Server.
     *
     * You can use command "CHECK" to check server status and command "STOP" to stop
     * running server, if any.
     *
     */
    // ___________________________________________________________________ Stop server ___
    case 'stop':
        srvstop();
        break;
    // ___________________________________________________________ Check server status ___
    case 'check':
        srvcheck('check');
        break;
    // __________________________________________________________________ Start server ___
    default:
        srvstart();
        break;
    }

?>