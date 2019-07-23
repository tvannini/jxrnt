<?php

/**
 * Janox Memory Database server
 * PHP7
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
 * JXMDB script
 * ============
 *
 * This script works both as a service controller and as a socket server.
 *
 * To start Janox Memory Database just run:
 *
 *    <path-to>/php <path-to>/jxmdb.php
 *
 * JXMD accepts commands:
 *
 *  - start (or none parameter):        starts server
 *  - check | status:                   prints out server status informations
 *  - exit | quit | shutdown | stop:    stops server
 *
 *
 * To configure JXMDB server set values for global variables in SETTINGS section.
 *
 *
 * @name      jxmdb.php
 * @package   janox/jxmdb.php
 * @version   2.5
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * Settings
 * ========
 *
 * @var string $jxrnt_path   Path to Janox runtime folder
 */
$jxrnt_path = __DIR__.DIRECTORY_SEPARATOR.'jxrnt'.DIRECTORY_SEPARATOR;

/*
 * @var string $host   Host on which server will listen
 */
$host = '127.0.0.1';

/*
 * @var string $port   Port on which server will listen
 */
$port = 8111;

/*
 * @var string $db_engine   Database engine used by server
 */
$db_engine = 'sqlite3';

/*
 * @var string $db_host   Database server host
 */
$db_host = ':memory:';

/*
 * @var string $db_user   User name to connect to database server
 */
$db_user = '';

/*
 * @var string $db_password   User password to connect to database server
 */
$db_password = '';

/*
 * @var string $log_file   File to log out to
 */
$log_file = __DIR__.DIRECTORY_SEPARATOR.'jxmdb.log';


/**
 * Janox Memory Database server
 *
 */
class JXMDB {

    // __________________________________________________________ Server configuration ___
    private $host;
    private $port;
    private $log_file;
    // ______________________________________________________________ Database gateway ___
    private $db_engine;
    private $db_host;
    private $db_user;
    private $db_pwd;
    // ___________________________________________________________ Internal properties ___
    private $separator = '|-+-|';
    private $socket    = null;
    private $clients   = [];
    private $changed   = [];


    function __construct() {

        if (!extension_loaded('sockets')) {
            die("ERROR: The sockets extension is not loaded!");
            }
        // ______________________________________________________ Load server settings ___
        $this->host      = $GLOBALS['host'];
        $this->port      = $GLOBALS['port'];
        $this->log_file  = $GLOBALS['log_file'];
        $this->db_engine = $GLOBALS['db_engine'];
        $this->db_host   = $GLOBALS['db_host'];
        $this->db_user   = $GLOBALS['db_user'];
        $this->db_pwd    = $GLOBALS['db_password'];
        // ________________________________________________________ Load Janox runtime ___
        require_once($GLOBALS['jxrnt_path'].'jxrnt.php');
        // ___________________________________________________________ Load DB gateway ___
        require_once($GLOBALS['jxrnt_path'].'lib'.DIRECTORY_SEPARATOR.
                     'dbms'.DIRECTORY_SEPARATOR.'jxdb_'.$this->db_engine.'.inc');
        // _____________________________________________________________ Create server ___
        set_time_limit(0);
        $this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        socket_set_option($this->socket, SOL_SOCKET, SO_REUSEADDR, 1);
        // ____________________________________ bind socket to specified host and port ___
        socket_bind($this->socket, $this->host, $this->port);
        // ____________________________________________________________ Listen to port ___
        socket_listen($this->socket);

        }


    function __destruct() {

        foreach($this->clients as $client) {
            socket_close($client);
            }
        socket_close($this->socket);

        }


    function log($e) {

        if ($this->log_file) {
            file_put_contents($this->log_file, $e->to_raw_text(), FILE_APPEND);
            }

        }


    function run() {

        // _______________________________________________________________________ Run ___
        while(true) {
            $this->waitForChange();
            $this->checkNewClients();
            $this->checkMessageRecieved();
            $this->checkDisconnect();
            }

        }


    function checkDisconnect() {

        foreach ($this->changed as $changed_socket) {
            // _____________________________________________ Check disconnected client ___
            if (@socket_read($changed_socket, 1024, PHP_NORMAL_READ) === false) {
                // _________________________________ Remove client from $clients array ___
                unset($this->clients[array_search($changed_socket, $this->clients)]);
                }
            }

        }


    function checkMessageRecieved() {

        foreach ($this->changed as $key => $socket) {
            $buffer = null;
            if (socket_recv($socket, $buffer, 65535, 0) >= 1) {
                // ==================================================== ALL STUFF HERE ===
                switch (trim($buffer)) {
                    // _____________________________________________ Skip empty buffer ___
                    case '':
                        break;
                    // ___________________________________________ Check server status ___
                    case 'check':
                    case 'status':
                        $this->sendMessage($socket,
                                           "Janox Memory Database server\n".
                                           '['.$this->host.':'.$this->port.']'.
                                           "\nPID: ".getmypid().
                                           "\nRAM: ".memory_get_usage().'/'.
                                                     memory_get_peak_usage().
                                           "\nClients: ".count($this->clients)."\n");
                        break;
                    // _______________________________________________ Shutdown server ___
                    case 'exit':
                    case 'quit':
                    case 'stop':
                    case 'shutdown':
                        $this->sendMessageAll("Janox Memory Database server\n".
                                              '['.$this->host.':'.$this->port.']'.
                                              "\nShutting down...\nBye bye\n");
                        exit("Bye bye\n");
                        break;
                    // _______________________________________________ Process request ___
                    default:
                        try {
                            $this->sendMessage($socket,
                                               $this->processRequest($buffer).
                                               $this->separator."\n".$this->separator);
                            }
                        catch (exception $e) {
                            $this->log($e);
                            $this->sendMessage($socket,
                                               serialize(array('!#ERROR' =>
                                                               $e->getmessage())).
                                               $this->separator."\n".$this->separator);
                            unset($this->changed[$key]);
                            return false;
                            }
                        break;
                    }
                unset($this->changed[$key]);
                }
            }

        }


    function waitForChange() {

        // _____________________________________________________ Reset changed sockets ___
        $this->changed = array_merge([$this->socket], $this->clients);
        $empty         = null;
        try {
            // ____ This next part is blocking so that we dont run away with cpu (???) ___
            if (socket_select($this->changed, $empty, $empty, null) === false) {
                print socket_strerror(socket_last_error())."\n";
                }
            }
        catch (exception $e) {
            $this->log($e);
            }

        }


    function checkNewClients() {

        if (!in_array($this->socket, $this->changed)) {
            return; // ________________________________________________ No new clients ___
            }
        // _________________________________________________________ Accept new socket ___
        $this->clients[] = socket_accept($this->socket);
        unset($this->changed[0]);

        }


    function sendMessage($client, $msg) {

        socket_write($client, $msg, strlen($msg));
        return true;

        }


    function sendMessageAll($msg) {

        $msg.= "\n";
        foreach($this->clients as $client) {
            @socket_write($client, $msg, strlen($msg));
            }
        return true;

        }


    function processRequest($buffer) {

        // ____________________________________________ Explode buffer into parameters ___
        $pars = explode($this->separator, $buffer);
        // print_r($pars);
        // __________________________________________________ Engine method to execute ___
        switch ($pars[0]) {
            case 'concat':
                $exec = 'o2_'.$this->db_engine.'_concat';
                return serialize($exec(unserialize($pars[1])));
                break;
            case 'tables':
                $exec = 'o2_'.$this->db_engine.'_tables';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2]));
                break;
            case 'tabexists':
                $exec = 'o2_'.$this->db_engine.'_tabexists';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3]));
                break;
            case 'tablefields':
                $exec = 'o2_'.$this->db_engine.'_tablefields';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3]));
                break;
            case 'tableindexes':
                $exec = 'o2_'.$this->db_engine.'_tableindexes';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3]));
                break;
            case 'insertfrom':
                $exec = 'o2_'.$this->db_engine.'_insertfrom';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 $pars[5],
                                 $pars[6],
                                 unserialize($pars[7]),
                                 $pars[8]));
                break;
            case 'droptable':
                $exec = 'o2_'.$this->db_engine.'_droptable';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3]));
                break;
            case 'renametable':
                $exec = 'o2_'.$this->db_engine.'_renametable';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4]));
                break;
            case 'createtable':
                $exec = 'o2_'.$this->db_engine.'_createtable';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 unserialize($pars[4]),
                                 $pars[5]));
                break;
            case 'aggregate':
                $exec = 'o2_'.$this->db_engine.'_aggregate';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 $pars[5],
                                 unserialize($pars[6]),
                                 unserialize($pars[7])));
                break;
            case 'verifyrec':
                $exec = 'o2_'.$this->db_engine.'_verifyrec';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 $pars[5],
                                 $pars[6],
                                 $pars[7]));
                break;
            case 'modifyrec':
                $exec = 'o2_'.$this->db_engine.'_modifyrec';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 unserialize($pars[5]),
                                 $pars[6]));
                break;
            case 'insertrec':
                $exec = 'o2_'.$this->db_engine.'_insertrec';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 unserialize($pars[5]),
                                 unserialize($pars[6])));
                break;
            case 'deleterec':
                $exec = 'o2_'.$this->db_engine.'_deleterec';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 $pars[5]));
                break;
            case 'count':
                $exec = 'o2_'.$this->db_engine.'_count';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 $pars[5],
                                 unserialize($pars[6])));
                break;
            case 'recordset':
                $exec = 'o2_'.$this->db_engine.'_recordset';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4],
                                 $pars[5],
                                 $pars[6],
                                 $pars[7],
                                 $pars[8],
                                 unserialize($pars[9]),
                                 $pars[10],
                                 $pars[11],
                                 $pars[12]));
                break;
            case 'fkeyadd':
                $exec = 'o2_'.$this->db_engine.'_fkeyadd';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 unserialize($pars[4]),
                                 $pars[5],
                                 $pars[6],
                                 $pars[7],
                                 unserialize($pars[8]),
                                 $pars[9]));
                break;
            case 'fkeyremove':
                $exec = 'o2_'.$this->db_engine.'_fkeyremove';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4]));
                break;
            case 'fkeyvalidate':
                $exec = 'o2_'.$this->db_engine.'_fkeyvalidate';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3],
                                 $pars[4]));
                break;
            case 'fkeystablist':
                $exec = 'o2_'.$this->db_engine.'_fkeystablist';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2],
                                 $pars[3]));
                break;
            case 'fkeysinfo':
                $exec = 'o2_'.$this->db_engine.'_fkeysinfo';
                return serialize($exec($this->db_host,
                                 $this->db_user,
                                 $this->db_pwd,
                                 $pars[1],
                                 $pars[2]));
                break;
            case 'commit':
                $exec = 'o2_'.$this->db_engine.'_commit';
                return serialize($exec($this->db_host,
                                       $this->db_user,
                                       $this->db_pwd,
                                       $pars[1]));
                break;
            }

        }

    }


// =============================== SERVER CONTROLLER =====================================
$cmd = strtolower(trim($_SERVER['argv'][1]));
switch ($cmd) {
    // __________________________________________________ Run server (from controller) ___
    case 'jxstartbatch':
        (new JXMDB())->run();
        break;
    // __________________________________________________________________ Start server ___
    case '':
    case 'start':
        include($jxrnt_path.'jxrnt.php');
        $rnt_obj = $GLOBALS['o2_runtime'];
        if ($rnt_obj && (!$rnt_obj->php_engine || !file_exists($rnt_obj->php_engine))) {
            $rnt_obj->find_php_exe();
            }
        if ($rnt_obj && $rnt_obj->php_engine) {
            $php_exe_path = $rnt_obj->php_engine;
            }
        else {
            die("Sorry, can't find Janox runtime to run Janox Memory Database Server.\n".
                "Please set \$jxrnt_path variable in this file (".__file__.").\n");
            }
        $run = $php_exe_path.' '.__FILE__.' JxStartBatch';
        if (stripos(PHP_OS, "win") !== false) {
            // ____________________________________________ Batch execution on Windows ___
            (new COM("WScript.Shell"))->Run($run, 0, false);
            }
        else {
            // ______________________________________________ Batch execution on Linux ___
            system("(".$run.") > /dev/null &");
            }
        print "Janox Memory Database server\n".
              '['.$host.':'.$port."]\nStarted\n";
        break;
    // ___________________________________________________________ Check server status ___
    case 'check':
    case 'status':
    // _______________________________________________________________ Shutdown server ___
    case 'quit':
    case 'stop':
    case 'shutdown':
    case 'exit':
        if (($conn = @socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) &&
            !@socket_connect($conn, $host, $port)) {
            die("Janox Memory Database server\n".
                '['.$host.':'.$port."]\nServer is not running!\n");
            }
        else {
            socket_write($conn, $cmd, strlen($cmd));
            print socket_read($conn, 2048);
            }
        break;
    // _______________________________________________________________ Process request ___
    default:
        print "Janox Memory Database server\n".
              "[".$host.':'.$port."]\nUnknown command ".$cmd."\n";
        break;
    }

?>