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
 * ===============
 *
 * This script works both as a service controller and as socket server.
 *
 * To start Janox Memory Database just run:
 *
 *    <path-to>/php <path-to>/jxmdb.php
 *
 * JXMD script can be executed with extra parameters:
 *
 *  - start (or none parameter): starts server
 *  - check | status:            prints out server status informations
 *  - exit | quit | shutdown:    stops server
 *
 * @name      jxmdb.php
 * @package   janox/jxmdb.php
 * @version   2.5
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
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
 * Janox Memory Database server
 *
 */
class JXMDB {

    static  $host      = '127.0.0.1';
    static  $port      = 8111;
    private $socket    = null;
    private $clients   = [];
    private $changed   = [];
    private $db_engine = 'sqlite3';
    private $db_host   = ':memory:';
    private $db_user   = '';
    private $db_pwd    = '';
    private $separator = '|-+-|';



    function __construct() {

        if (!extension_loaded('sockets')) {
            die("ERROR: The sockets extension is not loaded!");
            }
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
        socket_bind($this->socket, self::$host, self::$port);
        // ____________________________________________________________ Listen to port ___
        socket_listen($this->socket);

        }


    function __destruct() {

        foreach($this->clients as $client) {
            socket_close($client);
            }
        socket_close($this->socket);

        }


    function run() {

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
                                           "Janox Memory Database".
                                           "\nPID: ".getmypid().
                                           "\nRAM: ".memory_get_usage().'/'.
                                                     memory_get_peak_usage().
                                           "\nClients: ".count($this->clients)."\n");
                        break;
                    // _______________________________________________ Shutdown server ___
                    case 'exit':
                    case 'quit':
                    case 'shutdown':
                        $this->sendMessageAll("Janox Memory Database shuttting down...\n".
                                              "Bye bye\n");
                        exit("Bye bye\n");
                        break;
                    // _______________________________________________ Process request ___
                    default:
                        $this->sendMessage($socket,
                                           $this->processRequest($buffer).
                                           $this->separator."\n".$this->separator);
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
        // ________ This next part is blocking so that we dont run away with cpu (???) ___
        if (socket_select($this->changed, $empty, $empty, null) === false) {
            print socket_strerror(socket_last_error())."\n";
            }

        }


    function checkNewClients() {

        if (!in_array($this->socket, $this->changed)) {
            return; // ________________________________________________ No new clients ___
            }
        // _________________________________________________________ Accept new socket ___
        $this->clients[] = socket_accept($this->socket);
//        $this->sendMessage($socket_new, "\nWelcome to Janox Memory Database");
//        $this->sendMessage($socket_new, '#'.((integer) $socket_new)."\n");
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
        $exec = 'o2_'.$this->db_engine.'_'.$pars[0];
        $n    = 0;
        return serialize($exec((substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n]),
                               (substr($pars[++$n], 0, 3) === '[S]' ?
                                unserialize(substr($pars[$n], 3)) : $pars[$n])));

        }

    }


// _________________________________________________________________ Server controller ___
switch (trim($_SERVER['argv'][1])) {
    // __________________________________________________ Run server (from controller) ___
    case 'JxStartBatch':
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
        $cmd = $php_exe_path.' '.__FILE__.' JxStartBatch';
        if (stripos(PHP_OS, "win") !== false) {
            // ____________________________________________ Batch execution on Windows ___
            (new COM("WScript.Shell"))->Run($cmd, 0, false);
            }
        else {
            // ______________________________________________ Batch execution on Linux ___
            system("(".$cmd.") > /dev/null &");
            }
        break;
    // ___________________________________________________________ Check server status ___
    case 'check':
    case 'status':
    // _______________________________________________________________ Shutdown server ___
    case 'stop':
    case 'exit':
    case 'quit':
    case 'shutdown':
        if (($conn = @socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) &&
            !@socket_connect($conn, JXMDB::$host, JXMDB::$port)) {
            die("Janox Memory Database: server is not running!\n");
            }
        else {
            socket_write($conn, $_SERVER['argv'][1], 6);
            print socket_read($conn, 2048);
            }
        break;
    // _______________________________________________________________ Process request ___
    default:
        print "Janox Memory Database: unknown command ".$_SERVER['argv'][1]."\n";
        break;
    }

?>