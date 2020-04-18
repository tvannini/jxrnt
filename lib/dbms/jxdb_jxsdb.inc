<?php

/**
 * Janox Database As A Service Gateway
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
 * This script contains Janox data functions set for Janox Database As A Service
 * A Janox application can expose a database as a service (RPC mode).
 * This gateway allows applications to use a remote db exposed as a service.
 *
 *
 * @name      jxsdb
 * @package   janox/dbms/jxdb_jxsdb.php
 * @version   2.6
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


define('o2_jxsdb_o', '#jxdo#');
define('o2_jxsdb_c', '#jxdc#');
//$_SESSION['o2_jxsdb_conn'] = null;


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_jxsdb_normalize($string, $untrim = false) {

    return strtr(($untrim ? $string : trim($string)), array("'" => "''"));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_jxsdb_concat($strings) {

    return o2_jxsdb_execute('concat', json_encode($strings));

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as "db"."schema"."tab_name"
 * If $name is omitted a fully qualified prefix will be returned, in the form
 * "db"."schema"
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $name
 * @return string
 */
function o2_jxsdb_qualify($database, $owner, $name = "") {

    return o2_jxsdb_o.$database.o2_jxsdb_c.".".
           o2_jxsdb_o.$owner.o2_jxsdb_c.
           ($name ? ".".o2_jxsdb_o.$name.o2_jxsdb_c : "");

    }


/**
 * Return a jxsdb connection
 *
 * @param  string  $server     Server to connect to, as an URL
 * @param  string  $user       Connecion user name
 * @param  string  $password   Connection user password
 * @return Socket
 */
function o2_jxsdb_connect($server, $user, $password) {

    if (substr($server, 0, 7) == 'http://') {
        $url    = $server;
        $server = substr($server, 7);
        }
    elseif (substr($server, 0, 8) == 'https://') {
        $url    = $server;
        $server = substr($server, 8);
        }
    else {
        $url = 'http://'.$server;
        }
    $key = $server.$user;
    // _________________________________________________ Check for existing connection ___
    if (!isset($_SESSION['o2_app']->o2_jxsdb_conn[$key])) {
        if (!($conn = new JXHttpConn($url, $user, $password))) {
            throw new o2_exception("<b>jxsdb</b> server: <i>".$server."</i><hr>".
                                   "Can't connect to server",
                                   o2error_DBCONNECT);
            }
        $_SESSION['o2_app']->o2_jxsdb_conn[$key] = $conn;
        }
    return $_SESSION['o2_app']->o2_jxsdb_conn[$key];

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
 * @param  string  $server     Host to connect to
 * @param  string  $user       Connecion user name
 * @param  string  $password   Connection user password
 * @param  string  $cmd        Command to be executed
 * @param  mixed   ...         Further parameters needed by command
 * @return mix
 */
function o2_jxsdb_execute($server, $user, $password, $cmd) {

    $app  = $_SESSION['o2_app'];
    $data = array('c' => $cmd);
    $pars = func_get_args();
    $p    = 1;
    // ________________________________________ Shifts $server, $user, $password, $cmd ___
    array_splice($pars, 0, 4);
    // _________________________________________________________________ Packet params ___
    foreach ($pars as $par) {
        $data['p'.$p] = $par;
        $p++;
        }
    // _____________________________________________________________________ SQL trace ___
    if ($app->sqltrace) {
        o2log($cmd.'<br>'.nl2br(print_r($data, 1)));
        }
    // ______________________________________________________ Get connection to server ___
    if ($conn = o2_jxsdb_connect($server, $user, $password)) {
//      $data['dev'] = 'tommaso';
//      $data['jxtrace'] = '1';
        $res = $conn->get($data);
        // ______________________________________________________ On execution failure ___
        if ($res === false) {
            throw new o2_exception("<b>jxsdb</b> server:<hr>query: <code>".$cmd."<br>".
                                   nl2br(print_r($data, 1))."</code><hr>",
                                   o2error_DBEXECUTE);
            return false;
            }
        }
    else {
        throw new o2_exception("<b>jxsdb</b> server:<hr>Can't connect to server: <code>".
                               $server."</code>",
                               o2error_DBCONNECT);
        }
    $res = json_decode($res, true);
    // __________________________________________________ Error on remote server error ___
    if (is_array($res) && isset($res['!#ERROR'])) {
        throw new o2_exception("<b>jxsdb</b> gateway:<hr>".$res['!#ERROR'],
                               o2error_DBEXECUTE);
        return false;
        }
    return $res;

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
function o2_jxsdb_tables($server, $user, $password, $database, $owner) {

    return o2_jxsdb_execute($server, $user, $password, 'tables');

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
function o2_jxsdb_tabexists($server, $user, $password, $database, $owner, $table) {

    return o2_jxsdb_execute($server, $user, $password, 'tabexists', $table);

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
function o2_jxsdb_tablefields($server, $user, $password, $database, $owner, $table) {

    return o2_jxsdb_execute($server, $user, $password, 'tablefields', $table);

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
function o2_jxsdb_tableindexes($server, $user, $password, $database, $owner, $table) {

    return o2_jxsdb_execute($server, $user, $password, 'tableindexes', $table);

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
function o2_jxsdb_insertfrom($server,
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

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'insertfrom',
                            $table_from,
                            $table_to,
                            json_encode($values),
                            $where);

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
function o2_jxsdb_droptable($server, $user, $password, $database, $owner, $table) {

    return o2_jxsdb_execute($server, $user, $password, 'droptable', $table);

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
function o2_jxsdb_renametable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $new_name) {

    return o2_jxsdb_execute($server, $user, $password, 'renametable', $table, $new_name);

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
function o2_jxsdb_createtable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $structure,
                              $execute = true) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'createtable',
                            $table,
                            json_encode($structure),
                            $execute);

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
function o2_jxsdb_aggregate($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where,
                            $functions,
                            $links = null) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'aggregate',
                            $table,
                            $table_alias,
                            $where,
                            json_encode($functions),
                            json_encode($links));

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
function o2_jxsdb_verifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'verifyrec',
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by);

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
function o2_jxsdb_modifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $sets,
                            $where) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'modifyrec',
                            $table,
                            $table_alias,
                            json_encode($sets),
                            $where);

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
function o2_jxsdb_insertrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $fields,
                            $values) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'insertrec',
                            $table,
                            $table_alias,
                            json_encode($fields),
                            json_encode($values));

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
function o2_jxsdb_deleterec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'deleterec',
                            $table,
                            $table_alias,
                            $where);

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
function o2_jxsdb_count($server,
                        $user,
                        $password,
                        $database,
                        $owner,
                        $table,
                        $table_alias,
                        $where,
                        $links = null) {

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'count',
                            $table,
                            $table_alias,
                            $where,
                            json_encode($links));

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
function o2_jxsdb_recordset($server,
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

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'recordset',
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by,
                            $recs,
                            json_encode($links),
                            $lock,
                            $page_where,
                            false);

    }


/**
 * Alter jxsdb table to create a foreign key.
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
function o2_jxsdb_fkeyadd($server,
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

    return o2_jxsdb_execute($server,
                            $user,
                            $password,
                            'fkeyadd',
                            $main_table,
                            json_encode($main_fields),
                            $ref_table,
                            json_encode($ref_fields),
                            $key_name);

    }


/**
 * Alter jxsdb table to remove a foreign key.
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
function o2_jxsdb_fkeyremove($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $key_name) {

    return o2_jxsdb_execute($server, $user, $password, 'fkeyremove', $table, $key_name);

    }


/**
 * Validate a foreign key in jxsdb table against existing data.
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
function o2_jxsdb_fkeyvalidate($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

    return o2_jxsdb_execute($server, $user, $password, 'fkeyvalidate', $table, $key_name);

    }


/**
 * Returns the list of existing foreign keys for jxsdb table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_jxsdb_fkeystablist($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table) {

    return o2_jxsdb_execute($server, $user, $password, 'fkeystablist', $table);

    }


/**
 * Returns the list of existing foreign keys for jxsdb db-schema and some informations
 * about constraints
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_jxsdb_fkeysinfo($server,
                            $user,
                            $password,
                            $database,
                            $owner) {

    return o2_jxsdb_execute($server, $user, $password, 'fkeysinfo');

    }


/**
 * Commit open transaction on jxsdb database
 *
 * @param string  $server
 * @param string  $user
 * @param string  $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_jxsdb_commit($server, $user, $password, $end = false) {

//    return o2_jxsdb_execute($server, $user, $password, 'commit', $end);
    return true;

    }

?>
