<?php

/**
 * Janox Memory Database Gateway
 * PHP5
 *
 *
 * This file is part of Janox architecture.
 *
 * Janox is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * Janox is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This script contains Janox data functions set for Janox Memory Database
 *
 * @name      jxmdb
 * @package   janox/dbms/jxdb_jxmdb.php
 * @version   2.8
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


define('o2_jxmdb_o', '`');
define('o2_jxmdb_c', '`');
define('o2_jxmdb_s', '|-+-|');
$o2_jxmdb_conn  = null;
$o2_jxmdb_trans = array();
$o2_jxmdb_stms  = array();
$o2_jxmdb_error = array();


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_jxmdb_normalize($string, $untrim = false) {

    return strtr(($untrim ? $string : trim($string)), array("'" => "''"));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_jxmdb_concat($strings) {

    return o2_jxmdb_execute('concat'.o2_jxmdb_s.serialize($strings));

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as `dbtab_name`
 * If $name is omitted a fully qualified prefix will be returned, in the form `db
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $string
 * @return string
 */
function o2_jxmdb_qualify($database, $owner, $name = "") {

    return o2_jxmdb_o.$database.($name ? $name.o2_jxmdb_c : "");

    }


/**
 * Return a JXMDB socket
 *
 * @param  $server   Server to connect to, in the form <host>:<port>
 * @return Socket
 */
function o2_jxmdb_connect($server = false) {

    // _________________________________________________ Check for existing connection ___
    if (!isset($GLOBALS['o2_jxmdb_conn'])) {
        $app = $_SESSION['o2_app'];
        // ________________________________________________ Set standard host and port ___
        $host = '127.0.0.1';
        $port = 8111;
        if ($server) {
            list($h, $p) = explode(':', $server);
            if ($h) {
                $host = $h;
                }
            if ($p) {
                $port = $p;
                }
            }
        // _____________________________________________________ Create new connection ___
        if (!($conn_local = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) ||
            !socket_connect($conn_local, $host, $port)) {
            throw new o2_exception("<b>JXMDB</b> server: <i>".$host.':'.$port."</i><hr>".
                                   socket_strerror(socket_last_error()),
                                   o2error_DBCONNECT);
            }
        // ______________________________________________ Create transaction if needed ___
        /*
        if ($trans) {
            if (!$conn_local->beginTransaction()) {
                $err = $conn_local->errorInfo();
                throw new o2_exception("<b>JXMDB</b> server: <i>".$location."</i><hr>".
                                       $err[2],
                                       o2error_DBCONNECT);
                }
            else {
                $GLOBALS['o2_jxmdb_trans'][$location] = true;
                }
            }
        */
        $GLOBALS['o2_jxmdb_conn'] = $conn_local;
        }
    // _________________________________________________________ Check for transaction ___
    /*
    elseif ($trans && !isset($GLOBALS['o2_jxmdb_trans'][$location])) {
        $conn_local = $GLOBALS['o2_jxmdb_conn'][$location];
        // _________________________________ Create transaction on existing connection ___
        if (!$conn_local->beginTransaction()) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>JXMDB</b> server: <i>".$location."</i><hr>".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        else {
            $GLOBALS['o2_jxmdb_trans'][$location] = true;
            }
        }
    */
    return $GLOBALS['o2_jxmdb_conn'];

    }


/**
 * Execute a query on the given SQLite server.
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
 * For queries executing commands returns TRUE for correct execution
 *
 * @param  string  $query      Query to be executed
 * @param  string  $server     Host to connect to
 * @param  string  $user       Connecion user name
 * @param  string  $password   Connection user password
 * @param  boolean $only_exe   Execution only, no dataset returned
 * @param  boolean $trans      Transaction needed (insert, update, delete, ... queries)
 * @param  integer $limit      Number of records to fetch
 * @param  boolean $stm        Statement to fetch on
 * @return mix
 */
function o2_jxmdb_execute($cmd_str, $server = false) {

    $app = $_SESSION['o2_app'];
    // _____________________________________________________________________ SQL trace ___
    if ($app->sqltrace) {
        o2log($cmd_str);
        }
    // ______________________________________________________ Get connection to server ___
    $conn = o2_jxmdb_connect($server);
    if ($conn) {
        // ______________________________________________________ On execution failure ___
        if (socket_write($conn, $cmd_str, strlen($cmd_str)) === false) {
            throw new o2_exception("<b>JXMDB</b> server:<hr>".
                                   "query: <code>".$cmd_str."</code><hr>".
                                   socket_strerror(socket_last_error($conn)),
                                   o2error_DBEXECUTE);
            return false;
            }
        }
    else {
        throw new o2_exception("<b>JXMDB</b> server:<hr>Can't connect to server: <code>".
                               $cmd_str."</code>",
                               o2error_DBCONNECT);
        }
    // _____________________________________________ Execution only, no dataset return ___
    if ($only_exe) {
        return true;
        }
    // ________________________________________________________ Queries returning data ___
    else {
        $end = o2_jxmdb_s."\n".o2_jxmdb_s;
        $len = -strlen($end);
        $res = '';
        while ($out = socket_read($conn, 2048)) {
            if (substr($out, $len) === $end) {
                $res.= substr($out, 0, $len);
                break;
                }
            else {
                $res.= $out;
                }
            }
        $res = unserialize($res);
        // ______________________________________________ Error on remote server error ___
        if (is_array($res) && isset($res['!#ERROR'])) {
            throw new o2_exception("<b>JXMDB</b> gateway:<hr>".$res['!#ERROR'],
                                   o2error_DBEXECUTE);
            return false;
            }
        return $res;
        }

    }


/**
 * Retrieves user tables list from a sqlite database
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_jxmdb_tables($server, $user, $password, $database, $owner) {

    return o2_jxmdb_execute('tables'.o2_jxmdb_s.$database.o2_jxmdb_s.$owner, $server);

    }


/**
 * Return TRUE if database cointains the specified table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_jxmdb_tabexists($server, $user, $password, $database, $owner, $table) {

    return o2_jxmdb_execute('tabexists'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table,
                            $server);

    }


/**
 * Return an array containing informations about fileds of a sqlite table:
 *    $arr[n] = array('Field'   => field,
 *                    'Type'    => type(dim),
 *                    'Default' => default)
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return array
 */
function o2_jxmdb_tablefields($server, $user, $password, $database, $owner, $table) {

    return o2_jxmdb_execute('tablefields'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table,
                            $server);

    }


/**
 * Return an array containing informations about indexes of a sqlite table:
 *    $arr[n] = array('Field'   => field,
 *                    'Type'    => type(dim),
 *                    'Default' => default)
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return array
 */
function o2_jxmdb_tableindexes($server, $user, $password, $database, $owner, $table) {

    return o2_jxmdb_execute('tableindexes'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table,
                            $server);

    }


/**
 * Insert into table A data read from table B and return TRUE on success.
 * Matching fileds are passed by array $values in the form:
 *    $values[field_from] = field_to
 *
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
 * @param  string $where           optional WHERE clause for filtering copied records
 * @return boolean
 */
function o2_jxmdb_insertfrom($server,
                             $user,
                             $password,
                             $database_from,
                             $owner_from,
                             $table_from,
                             $database_to,
                             $owner_to,
                             $table_to,
                             $values,
                             $where = "") {

    return o2_jxmdb_execute('insertfrom'.o2_jxmdb_s.
                            $database_from.o2_jxmdb_s.
                            $owner_from.o2_jxmdb_s.
                            $table_from.o2_jxmdb_s.
                            $database_to.o2_jxmdb_s.
                            $owner_to.o2_jxmdb_s.
                            $table_to.o2_jxmdb_s.
                            serialize($values).o2_jxmdb_s.
                            $where,
                            $server);

    }


/**
 * Phisically removes a sqlite table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_jxmdb_droptable($server, $user, $password, $database, $owner, $table) {

    return o2_jxmdb_execute('droptable'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table,
                            $server);

    }


/**
 * Phisically renames a sqlite table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @param  string $new_name
 * @return boolean
 */
function o2_jxmdb_renametable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $new_name) {

    return o2_jxmdb_execute('renametable'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $new_name,
                            $server);

    }


/**
 * If $execute is passed TRUE function phisically creates a sqlite table, else it returns
 * the sqlite creation script for the table.
 * Array $structure is an array descriptive of the table structure.
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
 *       "primary" => array([pkeyname] => array([field_1] => A|D,
 *                                              [field_2] => A|D,
 *                                              [...]
 *                                              [field_n] => A|D)))
 *
 * @param  string    $server
 * @param  string    $user
 * @param  string    $password
 * @param  string    $database
 * @param  string    $owner
 * @param  string    $table
 * @param  array     $structure
 * @param  boolean   $execute
 * @return boolean
 */
function o2_jxmdb_createtable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $structure,
                              $execute = true) {

    return o2_jxmdb_execute('createtable'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            serialize($structure).o2_jxmdb_s.
                            $execute,
                            $server);

    }


/**
 * Return an array of calculated sqlite aggragate functions for the table.
 * Array $functions contains the list of functions to calculate and is passed in the form:
 *    $functions[o2aggrfunc_n] = array('func'  => aggr_function,
 *                                     'field' => on_field)
 *
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
function o2_jxmdb_aggregate($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where,
                            $functions,
                            $links = null) {

    return o2_jxmdb_execute('aggregate'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            $where.o2_jxmdb_s.
                            serialize($functions).o2_jxmdb_s.
                            serialize($links),
                            $server);

    }


/**
 * Verify if exists at last 1 record for given conditions. If it exists function returns
 * the record, else it returns FALSE.
 *
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
function o2_jxmdb_verifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by) {

    return o2_jxmdb_execute('verifyrec'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            $select_str.o2_jxmdb_s.
                            $where.o2_jxmdb_s.
                            $order_by,
                            $server);

    }


/**
 * Modifies sqlite record, uniquely identified by $where clause, with values in $sets.
 * Setting values are passed in array $sets in the form:
 *    array('field1' => value1,
 *          'field2' => value2,
 *          ...,
 *          'fieldn' => valuen)
 *
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
function o2_jxmdb_modifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $sets,
                            $where) {

    return o2_jxmdb_execute('modifyrec'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            serialize($sets).o2_jxmdb_s.
                            $where,
                            $server);

    }


/**
 * Insert passed record fields in a sqlite table.
 *
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
function o2_jxmdb_insertrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $fields,
                            $values) {

    return o2_jxmdb_execute('insertrec'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            serialize($fields).o2_jxmdb_s.
                            serialize($values),
                            $server);

    }


/**
 * Delete record of a sqlite table for the passed $where clause.
 *
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
function o2_jxmdb_deleterec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where) {

    return o2_jxmdb_execute('deleterec'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            $where,
                            $server);

    }


/**
 * Returns number of total records for passed $where clause.
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @param  string $table_alias
 * @param  string $where
 * @param  array  $links
 * @return integer
 */
function o2_jxmdb_count($server,
                        $user,
                        $password,
                        $database,
                        $owner,
                        $table,
                        $table_alias,
                        $where,
                        $links = null) {

    return o2_jxmdb_execute('count'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            $where.o2_jxmdb_s.
                            serialize($links),
                            $server);

    }


/**
 * Returns a set of $recs records from a sqlite table for a passed $where clause.
 *
 * Recordset is returned in the form:
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
 * @param  string  $stm_id
 * @return array
*/
function o2_jxmdb_recordset($server,
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
                            $links      = null,
                            $lock       = false,
                            $page_where = false,
                            $stm_id     = false) {

    return o2_jxmdb_execute('recordset'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $table_alias.o2_jxmdb_s.
                            $select_str.o2_jxmdb_s.
                            $where.o2_jxmdb_s.
                            $order_by.o2_jxmdb_s.
                            $recs.o2_jxmdb_s.
                            serialize($links).o2_jxmdb_s.
                            $lock.o2_jxmdb_s.
                            $page_where.o2_jxmdb_s.
                            $stm_id,
                            $server);

    }


/**
 * Alter jxmdb table to create a foreign key.
 * $main_fields and $ref_fields are list of filed names, from main and referenced
 * tables to create the key on.
 *
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
function o2_jxmdb_fkeyadd($server,
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

    return o2_jxmdb_execute('fkeyadd'.o2_jxmdb_s.
                            $main_db.o2_jxmdb_s.
                            $main_owner.o2_jxmdb_s.
                            $main_table.o2_jxmdb_s.
                            serialize($main_fields).o2_jxmdb_s.
                            $ref_db.o2_jxmdb_s.
                            $ref_owner.o2_jxmdb_s.
                            $ref_table.o2_jxmdb_s.
                            serialize($ref_fields).o2_jxmdb_s.
                            $key_name,
                            $server);

    }


/**
 * Alter jxmdb table to remove a foreign key.
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  string  $database
 * @param  string  $owner
 * @param  string  $table
 * @param  string  $key_name
 * @return boolean
 */
function o2_jxmdb_fkeyremove($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $key_name) {

    return o2_jxmdb_execute('fkeyremove'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $key_name,
                            $server);

    }


/**
 * Validate a foreign key in jxmdb table against existing data.
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  string  $database
 * @param  string  $owner
 * @param  string  $table
 * @param  string  $key_name
 * @return boolean
 */
function o2_jxmdb_fkeyvalidate($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

    return o2_jxmdb_execute('fkeyvalidate'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table.o2_jxmdb_s.
                            $key_name,
                            $server);

    }


/**
 * Returns the list of existing foreign keys for jxmdb table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return array
 */
function o2_jxmdb_fkeystablist($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table) {

    return o2_jxmdb_execute('fkeystablist'.o2_jxmdb_s.
                            $database.o2_jxmdb_s.
                            $owner.o2_jxmdb_s.
                            $table,
                            $server);

    }


/**
 * Returns the list of existing foreign keys for jxmdb db-schema and some informations
 * about constraints
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_jxmdb_fkeysinfo($server,
                            $user,
                            $password,
                            $database,
                            $owner) {

    return o2_jxmdb_execute('fkeysinfo'.o2_jxmdb_s.$database.o2_jxmdb_s.$owner, $server);

    }


/**
 * Commit open transaction on sqlite database
 *
 * @param string  $server
 * @param string  $user
 * @param string  $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_jxmdb_commit($server, $user, $password, $end = false) {

    // _______________________________________________ This is needed for memory db!!! ___
    $end = false;
    return o2_jxmdb_execute('commit'.o2_jxmdb_s.$end, $server);

    }

?>
