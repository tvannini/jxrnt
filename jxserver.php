<?php

/**
 * Janox Mini WEB Server
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
 * JXServer script
 * ===============
 *
 * This script works both as a service controller and as a router script.
 *
 * To serve Janox Demo application from a standard Janox folder structure, just type at
 * the command line:
 *
 *    <path-to>/php <path-to>/jxserver.php
 *
 * If you want to serve other Janox application or from different standard paths see
 * SETTINGS session below.
 *
 *
 * @name      jxserver.php
 * @package   janox/jxserver.php
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
$jxrnt_path = __DIR__.DIRECTORY_SEPARATOR.'jxrnt'.DIRECTORY_SEPARATOR;


/**
 * REGISTERED APPLICATIONS
 * =======================
 *
 * To register an application to be served by Janox Mini WEB Server just add it to the
 * array below, in the form:
 *
 * $apps = ['app-name1' => 'app-web-root1',
 *          'app-name2' => 'app-web-root2',
 *          ...
 *          'app-nameN' => 'app-web-rootN'];
 *
 * Application WEB root is folder htdocs under application main folder (<app>/htdocs).
 * Application name will be used as application alias too, so that application main URL
 * will be in the form:
 *
 *    http://localhost:8333/app_name/app_name.php
 *
 * @var array $apps   Provides a list of served applications with name and folder
 */
$apps = ['jxdemo'   => __DIR__.DIRECTORY_SEPARATOR.'demo'.DIRECTORY_SEPARATOR.'htdocs'.DIRECTORY_SEPARATOR,
         'jxstudio' => __DIR__.DIRECTORY_SEPARATOR.'studio'.DIRECTORY_SEPARATOR.'htdocs'.DIRECTORY_SEPARATOR];


/**
 * Default port is 8333, but if you want to listen on a different port just set $port
 * variable value.
 *
 * Server will respond on http://localhost:<port>.
 *
 * @var integer $port   HTTP port to listen on
 */
$port = 8333;

// ____________________________________ Set down error reporting to avoid braking HTML ___
error_reporting(E_ALL & ~E_WARNING & ~E_DEPRECATED & ~E_STRICT & ~E_NOTICE);
ini_set('display_errors', false);
ini_set('log_errors', true);


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
 * Start Janox Mini Web Server.
 *
 */
function srvstart() {

    if (!srvcheck()) {
        $rnt_obj = $GLOBALS['o2_runtime'];
        if ($rnt_obj && (!$rnt_obj->php_engine || !file_exists($rnt_obj->php_engine))) {
            $rnt_obj->find_php_exe();
            }
        if ($rnt_obj && $rnt_obj->php_engine) {
            $php_exe_path = $rnt_obj->php_engine;
            }
        else {
            print "Sorry, can't find Janox runtime to run Janox Mini WEB Server.\n".
                  "Please set \$jxrnt_path variable in this file (".__file__.").\n";
            die();
            }
        $list_old = $rnt_obj->proc_list(true);
        $rnt_obj->batch_exec($php_exe_path.' -S localhost:'.$GLOBALS['port'].' '.
                             __FILE__);
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
            // _______________________________________ Write PID-file with running PID ___
            file_put_contents(pid_file(), $running);
            print "Janox Mini WEB Server started.\nWith PID ".$running.
                  "\nListening on http://localhost:".$GLOBALS['port'].
                  "\nServing applications:\n";
            foreach ($GLOBALS['apps'] as $app_name => $app_dir) {
                print ' '.str_pad($app_name, 20, '.').' from '.$app_dir."\n";
                }
            }
        // _______________________________________________________________ Not started ___
        else {
            print "Sorry, unable to start Janox Mini WEB Server.\n";
            }
        }

    }


/**
 * Stop Janox Mini Web Server.
 *
 */
function srvstop() {

    // _________________________________________________________ Check for running PID ___
    if ($r_pid = srvcheck()) {
        $rnt_obj = $GLOBALS['o2_runtime'];
        $list    = $rnt_obj->proc_list(true);
        // _____________________ Check if running PID in the list of running processes ___
        foreach ($list as $pid => $exe_name) {
            if ($pid == $r_pid) {
                $rnt_obj->kill($pid);
                o2file_delete(pid_file());
                print "Janox Mini WEB Server stopped.\n";
                break;
                }
            }
        }

    }


/**
 * Check if Janox Mini Web Server is running.
 *
 */
function srvcheck() {

    $rnt_obj  = $GLOBALS['o2_runtime'];
    $pid_file = pid_file();
    $running  = false;
    // ______________________________________________________ Check if PID-file exists ___
    if (o2file_exists($pid_file)) {
        // ___________________________________________________________ Get running PID ___
        $r_pid = file_get_contents($pid_file);
        $list  = $rnt_obj->proc_list(true);
        // _____________________ Check if running PID in the list of running processes ___
        foreach ($list as $pid => $exe_name) {
            if ($pid == $r_pid) {
                $running = $pid;
                break;
                }
            }
        // __________________________________________ Janox Mini Web Server is running ___
        if ($running) {
            print "Janox Mini WEB Server is running.\nWith PID ".$running.
                  "\nListening on http://localhost:".$GLOBALS['port'].
                  "\nServing applications:\n";
            foreach ($GLOBALS['apps'] as $app_name => $app_dir) {
                print ' '.str_pad($app_name, 20, '.').' from '.$app_dir."\n";
                }
            return $running;
            }
        else {
            print "Janox Mini WEB Server is not running.\n";
            o2file_delete($pid_file);
            return false;
            }
        }
    else {
        print "Janox Mini WEB Server is not running.\n";
        return false;
        }

    }


/**
 * RUN AS ROUTER
 * =============
 *
 * When called from inside a PHP internal server session this script works like a router.
 *
 * Requests are routed by prefixed alias so that files are served from within matching
 * registered application WEB root.
 *
 * If alias is "janox" or alias is not matching to any registered application then
 * requested resources are served from Janox WEB root (jxrnt/htdocs/).
 *
 */
if (php_sapi_name() == 'cli-server') {
    // ______________________________________________________ Look for requested alias ___
    $matches = array();
    preg_match('/^(\/(\w*)\/)?([^?]*)\?*(.*)/i', $_SERVER["REQUEST_URI"], $matches);
    $alias     = $matches[2];
    $file_path = $matches[3];
    $routed    = false;
    // ___________________________________________________ Application alias requested ___
    if ($alias) {
        foreach ($apps as $app_name => $app_dir) {
            // __________________________________________________ Found matching alias ___
            if ($app_name == $alias) {
                chdir($app_dir);
                $file_path = $app_dir.$file_path;
                $routed    = true;
                break;
                }
            }
        }
    // ______________________________________________________ Janox resource requested ___
    if (!$routed) {
        if (!$file_path || $file_path == '/') {
            $file_path = $jxrnt_path.'htdocs'.DIRECTORY_SEPARATOR.'index.html';
            }
        elseif ($alias != 'janox') {
            $file_path = $jxrnt_path.'htdocs'.DIRECTORY_SEPARATOR.$alias.
                                              DIRECTORY_SEPARATOR.$file_path;
            }
        else {
            $file_path = $jxrnt_path.'htdocs'.DIRECTORY_SEPARATOR.$file_path;
            }
        chdir($jxrnt_path.'htdocs'.DIRECTORY_SEPARATOR);
        }
    // ______________________________________________________ Serve requested resource ___
    header('Content-Type: '.o2_mime_content($file_path));
    include $file_path;

    }

/**
 * RUN AS SERVICE CONTROLLER
 * =========================
 *
 * When called from command line or by CLI, this script works as a controller for the
 * Janox Mini WEB Server service.
 *
 * If called without parameters or with command "START" it will try to start the Janox
 * Mini WEB Server.
 *
 * You can use command "CHECK" to check server status and command "STOP" to stop running
 * server, if any.
 *
 */
else {
    switch (strtolower(trim($_SERVER['argv'][1]))) {
        // _______________________________________________________________ Stop server ___
        case 'stop':
            srvstop();
            break;
        // _______________________________________________________ Check server status ___
        case 'check':
            srvcheck();
            break;
        // ______________________________________________________________ Start server ___
        default:
            srvstart();
            break;
        }
    }

?>