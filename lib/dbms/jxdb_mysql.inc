<?php

/**
 * Janox MySQL Database Gateway
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
 * This script contains Janox data functions set for MySQL database
 *
 * @name      jxmysql
 * @package   janox/dbms/jxdb_mysql.php
 * @version   2.5
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


if (!extension_loaded("pdo_mysql")) {
    throw new o2_exception("<b>MySQL PDO</b> driver not loaded:<br />".
                           "check your PHP configuration for ".
                           "<i>pdo</i> and <i>pdo_mysql</i> extensions.",
                           o2error_DBCONNECT);
    }

define("o2_mysql_o", "`");
define("o2_mysql_c", "`");
$o2_mysql_conn  = array();
$o2_mysql_trans = array();


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_mysql_normalize($string, $untrim = false) {

    return addslashes($untrim ? $string : rtrim($string));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_mysql_concat($strings) {

    return 'CONCAT('.implode(',', $strings).')';

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as `db`.`tab_name`
 * If $name is omitted a fully qualified prefix will be returned, in the form `db`
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $string
 * @return string
 */
function o2_mysql_qualify($database, $owner, $name = "") {

    return o2_mysql_o.$database.o2_mysql_c.
           ($name ? ".".o2_mysql_o.$name.o2_mysql_c : "");

    }


/**
 * Return a MySQL connection handle
 *
 * NOTE: Isolated transactions are not managed because MYISAM always works in autocommit
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  boolean $trans
 * @return PDO
 */
function o2_mysql_connect($server, $user, $password, $trans) {

    $app = $_SESSION['o2_app'];
    $key = $server.$user;
    // ___________________________________________________________ Prevent blank names ___
    if (!$server) {
        $server = "localhost";
        }
    // _________________________________________________ Check for existing connection ___
    if (!isset($GLOBALS['o2_mysql_conn'][$key])) {
        $opt = array(PDO::ATTR_PERSISTENT => true);
        // _______________________________________________________ Connection encoding ___
        if ($app->chr_encoding) {
            switch (strtolower(str_replace(array("-", "_"), "", $app->chr_encoding))) {
                case "windows1252":
                case "cp1252":
                    $opt[PDO::MYSQL_ATTR_INIT_COMMAND] = 'SET NAMES latin1';
                    break;
                case "utf8":
                    $opt[PDO::MYSQL_ATTR_INIT_COMMAND] = 'SET NAMES utf8';
                    break;
                }
            }
        // _____________________________________________________ Create new connection ___
        if (!($conn_local = new PDO("mysql:host=".$server, $user, $password, $opt))) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>MySQL</b> server: <i>".$server."</i><hr />".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        // ______________________________________________ Create transaction if needed ___
        if ($trans) {
            if (!$conn_local->beginTransaction()) {
                $err = $conn_local->errorInfo();
                throw new o2_exception("<b>MySQL</b> server: <i>".$server."</i><hr />".
                                       $err[2],
                                       o2error_DBCONNECT);
                }
            else {
                $GLOBALS['o2_mysql_trans'][$key] = true;
                }
            }
        $GLOBALS['o2_mysql_conn'][$key] = $conn_local;
        }
    // _________________________________________________________ Check for transaction ___
    elseif ($trans && !isset($GLOBALS['o2_mysql_trans'][$key])) {
        $conn_local = $GLOBALS['o2_mysql_conn'][$key];
        // _________________________________ Create transaction on existing connection ___
        if (!$conn_local->beginTransaction()) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>MySQL</b> server: <i>".$server."</i><hr />".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        else {
            $GLOBALS['o2_mysql_trans'][$key] = true;
            }
        }
    return $GLOBALS['o2_mysql_conn'][$key];

    }


/**
 * Execute a query on the given MySQL server.
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
 * For queries in the form 'SELECT [exp] AS COMPUTED' returns a numeric value
 *
 * For queries executing commands returns TRUE for correct execution
 *
 * @param  string  $query      Query to be executed
 * @param  string  $server     Host to connect to
 * @param  string  $user       Connecion user name
 * @param  string  $password   Connection user password
 * @param  boolean $only_exe   Execution only, no dataset returned
 * @param  boolean $trans      Transaction needed (insert, update, delete, ... queries)
 * @return mix
 */
function o2_mysql_execute($query,
                          $server,
                          $user,
                          $password,
                          $only_exe = false,
                          $trans    = true) {

    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // ______________________________________________________ Get connection to server ___
    $conn = o2_mysql_connect($server, $user, $password, $trans);
    // _____________________________________________ Execution only, no dataset return ___
    if ($only_exe) {
        $res = $conn->exec($query);
        // ______________________________________________________ On execution failure ___
        if ($res === false) {
            $err = $conn->errorInfo();
            $rb  = $conn->rollBack;
            $rb && @$rb();
            throw new o2_exception("<b>MySQL</b> server: <i>".$server.
                                   "</i><br />query: <code>".$query."</code><hr />".
                                   $err[2],
                                   o2error_DBEXECUTE);
            return false;
            }
        return $res;
        }
    // ________________________________________________________ Queries returning data ___
    else {
        // ______________________________________________________ On execution failure ___
        if (!($res = $conn->query($query))) {
            $err = $conn->errorInfo();
            $rb  = $conn->rollBack;
            $rb && @$rb();
            throw new o2_exception("<b>MySQL</b> server: <i>".$server.
                                   "</i><br />query: <code>".$query."</code><hr />".
                                   $err[2],
                                   o2error_DBDATAQUERY);
            return false;
            }
        // ____________________________________________________________ Return dataset ___
        return $res->fetchAll(PDO::FETCH_ASSOC);
        }

    }


/**
 * Retrieves user tables list from a MySQL database
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_mysql_tables($server, $user, $password, $database, $owner) {

    $arr_local   = array();
    $conn_local  = o2_mysql_connect($server, $user, $password, false);
    $query_local = "SHOW TABLES FROM ".o2_mysql_o.$database.o2_mysql_c;
    $res_local   = $conn_local->query($query_local);
    if ($res_local === false) {
        $err = $conn_local->errorInfo();
        throw new o2_exception("<b>MySQL</b> server: <i>".$server."</i><hr />".$err[2],
                               o2error_DBTABLES);
        }
    while ($single_tab = $res_local->fetchColumn()) {
        $arr_local[] = $single_tab;
        }
    return $arr_local;

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
function o2_mysql_tabexists($server, $user, $password, $database, $owner, $table) {

    $conn_local  = o2_mysql_connect($server, $user, $password, false);
    $query_local = "SHOW TABLES FROM ".o2_mysql_o.$database.o2_mysql_c.
                   " LIKE '".$table."'";
    $res_local   = $conn_local->query($query_local);
    if ($res_local === false) {
        $err = $conn_local->errorInfo();
        throw new o2_exception("<b>MySQL</b> server: <i>".$server."</i><hr />".$err[2],
                               o2error_DBTABEXISTS);
        }
    return ($res_local->fetchColumn() ? true : false);

    }


/**
 * Return an array containing informations about fileds of a MySQL table:
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
function o2_mysql_tablefields($server, $user, $password, $database, $owner, $table) {

    $fields_list = array();
    $query_local = "SHOW FIELDS FROM ".o2_mysql_o.$database.o2_mysql_c.".".
                                       o2_mysql_o.$table.o2_mysql_c;
    try {
        $res_local = o2_mysql_execute($query_local,
                                      $server,
                                      $user,
                                      $password,
                                      false,
                                      false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    foreach ($res_local as $single_field) {
        $single_field  = array_change_key_case($single_field);
        $fields_list[] = array('field'   => $single_field['field'],
                               'type'    => $single_field['type'],
                               'default' => $single_field['default']);
        }
    return $fields_list;

    }


/**
 * Return an array containing informations about indexes of a MySQL table:
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
function o2_mysql_tableindexes($server, $user, $password, $database, $owner, $table) {

    $ret_val     = array();
    $query_local = "SHOW KEYS FROM ".o2_mysql_o.$database.o2_mysql_c.".".
                                     o2_mysql_o.$table.o2_mysql_c;
    try {
        $res_local = o2_mysql_execute($query_local,
                                      $server,
                                      $user,
                                      $password,
                                      false,
                                      false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    foreach ($res_local as $single_segment) {
        if ($single_segment['Key_name'] != "PRIMARY") {
            if (!isset($ret_val[$single_segment['Key_name']])) {
                $ret_val[$single_segment['Key_name']] = array();
                }
            $ret_val[$single_segment['Key_name']]+=
                array(($single_segment['Seq_in_index'] - 1) =>
                      array('column' => $single_segment['Column_name'],
                            'dir'    => ""));
            }
        }
    return $ret_val;

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
function o2_mysql_insertfrom($server,
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

    $list_from = "";
    $list_to   = "";
    foreach ($values as $field_from => $field_to) {
        $sep = ($list_from ? "," : "");
        if ($field_from == "@o2CloneArea") {
            $list_from.= $sep.$field_to;
            $list_to  .= $sep."O2ASPID";
            }
        else {
            $list_from.= $sep.$field_from;
            $list_to  .= $sep.$field_to;
            }
        }
    $query_local = "INSERT INTO ".o2_mysql_o.$database_to.o2_mysql_c.".".
                                  o2_mysql_o.$table_to.o2_mysql_c." (".$list_to.
                   ") SELECT ".$list_from." FROM ".o2_mysql_o.$database_from.o2_mysql_c.
                                               ".".o2_mysql_o.$table_from.o2_mysql_c;
    if ($where) {
       $query_local.= " WHERE ".$where;
       }
    try {
        $aff_rows = o2_mysql_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLECOPY);
        throw $o2e;
        return false;
        }
    return $aff_rows;

    }


/**
 * Phisically removes a mysql table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_mysql_droptable($server, $user, $password, $database, $owner, $table) {

    $query_local = "DROP TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                                 o2_mysql_o.$table.o2_mysql_c;
    try {
        o2_mysql_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEDROP);
        throw $o2e;
        }
    o2_mysql_commit($server, $user, $password, $database);
    return true;

    }


/**
 * Phisically renames a mysql table
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
function o2_mysql_renametable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $new_name) {

    $query_local = "RENAME TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                                   o2_mysql_o.$table.o2_mysql_c." TO ".
                                   o2_mysql_o.$database.o2_mysql_c.".".
                                   o2_mysql_o.$new_name.o2_mysql_c;
    try {
        o2_mysql_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEREBUILD);
        throw $o2e;
        }
    o2_mysql_commit($server, $user, $password, $database);
    return true;

    }


/**
 * If $execute is passed TRUE function phisically creates a mysql table, else it returns
 * the MySQL creation script for the table.
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
function o2_mysql_createtable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $structure,
                              $execute = true) {

    $query = "CREATE TABLE IF NOT EXISTS ".o2_mysql_o.$database.o2_mysql_c.".".
                                           o2_mysql_o.$table.o2_mysql_c." (\n";
    $type        = "";
    $default     = "";
    foreach ($structure["fields"] as $field_name => $field_prop) {
        $query.= o2_mysql_field_create($field_name,
                                       $field_prop['type'],
                                       $field_prop['size'],
                                       $field_prop['int'],
                                       $field_prop['dec']).",\n";
        }
    // ___________________________________________________________________ PRIMARY KEY ___
    $arr_local = array();
    foreach ($structure["primary"] as $pk_name => $pk_segments) {
        break;
        }
    $query.= "PRIMARY KEY (".o2_mysql_indexfields_create($pk_segments)."),\n";
    // _______________________________________________________________________ INDEXES ___
    foreach ($structure["keys"] as $index_name => $index_segs) {
        $query.= o2_mysql_index_create($index_name, $index_segs).",\n";
        }
    $query = substr($query, 0, -2).")\nENGINE = MYISAM";
    if ($execute) {
        try {
            o2_mysql_execute($query, $server, $user, $password, true, true);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        o2_mysql_commit($server, $user, $password, $database);
        return true;
        }
    else {
        return $query;
        }

    }


/**
 * Returns MySQL definition for single field by Janox-type
 *
 * @param  string  $name         Field name
 * @param  string  $type         Field type (A|N|L|D|T|S)
 * @param  integer $size         Total size for alpha
 * @param  integer $int          Total size or integer part
 * @param  integer $dec          Number of decimals digits (only numbers)
 * @param  boolean $for_change   If script is requested for ALTER COLUMN syntax
 * @return string                SQL code to create field
 */
function o2_mysql_field_create($name, $type, $size, $int, $dec, $for_change = false) {

    switch ($type) {
        case "A":
            if ($size <= 255) {
                $fld_type    = "varchar(".$size.")";
                $fld_default = "''";
                }
            elseif ($size <= 65530) {
                $fld_type    = "text";
                $fld_default = false;
                }
            else {
                $fld_type    = "longtext";
                $fld_default = false;
                }
            break;
        case "N":
            if ($dec) {
                $fld_type = "decimal(".($int + $dec).", ".$dec.")";
                }
            elseif ($int < 3) {
                $fld_type = "TINYINT";
                }
            elseif ($int < 5) {
                $fld_type = "SMALLINT";
                }
            elseif ($int < 7) {
                $fld_type = "MEDIUMINT";
                }
            elseif ($int < 10) {
                $fld_type = "INT";
                }
            else {
                $fld_type = "BIGINT";
                }
            $fld_default = "0";
            break;
        case "L":
            $fld_type    = "char(1)";
            $fld_default = "'0'";
            break;
        case "D":
            $fld_type    = "char(8)";
            $fld_default = "'00000000'";
            break;
        case "T":
            $fld_type    = "char(6)";
            $fld_default = "'000000'";
            break;
        case "S":
            $fld_type    = "text";
            $fld_default = false;
            break;
        }
    return $name." ".$fld_type.($fld_default !== false ?
                                " NOT NULL DEFAULT ".$fld_default :
                                "");

    }


/**
 * Returns MySQL definition for single index by Janox-type.
 * $key_segs is a list of key segments, in the form <field-name> => <direction>,
 * where <direction> is A|D for ascending or descending.
 *
 * @param  string  $key_name   Index name
 * @param  array   $key_segs   Index segments as an array "name" => <direction>
 * @param  string  $table      Table physical name
 * @param  string  $database   Database name
 * @param  string  $owner      Database owner
 * @param  boolean $unique     Index is unique
 * @return string              SQL code to create index
 */
function o2_mysql_index_create($key_name,
                               $key_segs,
                               $table    = "",
                               $database = "",
                               $owner    = "",
                               $unique   = true) {

    return ($unique ? 'UNIQUE ' : 'INDEX ').o2_mysql_o.$key_name.o2_mysql_c.
           " (".o2_mysql_indexfields_create($key_segs).")";

    }


/**
 * Return ORDER_BY clausole for all the fields of an index in a mysql context.
 * List has to be comma separated and rightly qualified.
 *
 * $fields array parameter is passed in the form:
 *    $fields[field_name] = direction [D/A]
 *
 * @param  array  $fields
 * @return string
 */
function o2_mysql_indexfields_create($fields) {

    $str = "";
    foreach ($fields as $field => $direction) {
        $str.= ($str ? "," : "").$field." ".($direction == "D" ? "DESC" : "ASC");
        }
    return $str;

    }


/**
 * Alter a MySQL table to add a column
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  string  $database
 * @param  string  $owner
 * @param  string  $table
 * @param  string  $field_name
 * @param  string  $field_type
 * @param  integer $field_size
 * @param  integer $field_int
 * @param  integer $field_dec
 * @return boolean
 */
function o2_mysql_field_add($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $field_name,
                            $field_type,
                            $field_size,
                            $field_int,
                            $field_dec) {

    $query = "ALTER TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                            o2_mysql_o.$table.o2_mysql_c.
             " ADD COLUMN ".o2_mysql_field_create(o2_mysql_o.$field_name.o2_mysql_c,
                                                  $field_type,
                                                  $field_size,
                                                  $field_int,
                                                  $field_dec);
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a MySQL table to remove a column
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @param  string $field_name
 * @return boolean
 */
function o2_mysql_field_remove($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $field_name) {

    $query =  "ALTER TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                             o2_mysql_o.$table.o2_mysql_c.
             " DROP COLUMN ".o2_mysql_o.$field_name.o2_mysql_c;
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a MySQL table to change a column definition
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  string  $database
 * @param  string  $owner
 * @param  string  $table
 * @param  string  $field_name
 * @param  string  $field_type
 * @param  integer $field_size
 * @param  integer $field_int
 * @param  integer $field_dec
 * @return boolean
 */
function o2_mysql_field_change($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $field_name,
                               $field_type,
                               $field_size,
                               $field_int,
                               $field_dec) {

    $query =   "ALTER TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                              o2_mysql_o.$table.o2_mysql_c.
             " MODIFY ".o2_mysql_field_create(o2_mysql_o.$field_name.o2_mysql_c,
                                              $field_type,
                                              $field_size,
                                              $field_int,
                                              $field_dec,
                                              true);
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a MySQL table to rename a column
 *
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
function o2_mysql_field_rename($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $old_name,
                               $new_name) {

    $query   =    "ALTER TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                                 o2_mysql_o.$table.o2_mysql_c.
               " CHANGE COLUMN ".o2_mysql_o.$old_name.o2_mysql_c." ".
                                 o2_mysql_o.$new_name.o2_mysql_c;
    // _______________________________________ Get field definition to use with CHANGE ___
    $flds    = o2_mysql_tablefields($server, $user, $password, $database, $owner, $table);
    $fld_def = false;
    foreach ($flds as $fld) {
        if ($fld["field"] == $old_name) {
            $type = strtoupper($fld["type"]);
            if (strpos($type, "CHAR") !== false) {
                $default = " NOT NULL DEFAULT ''";
                }
            elseif (strpos($type, "TEXT") !== false) {
                $default = "";
                }
            else {
                $default = " NOT NULL DEFAULT 0";
                }
            $query  .= " ".$type.$default;
            $fld_def = true;
            break;
            }
        }
    if ($fld_def) {
        try {
            $res = o2_mysql_execute($query, $server, $user, $password, true, true);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLEALTER);
            throw $o2e;
            }
        }
    else {
        throw new o2_exception("Unknown field <i>".$old_name."</i> in table <i>".$table.
                               "</i>", o2error_DBTABLEALTER);
        }

    }


/**
 * Alter a MySQL table to add an index
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  string  $database
 * @param  string  $owner
 * @param  string  $table
 * @param  string  $index_name
 * @param  array   $key_segs
 * @param  boolean $unique
 * @return boolean
 */
function o2_mysql_index_add($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $index_name,
                            $key_segs,
                            $unique = true) {

    $quoted_segs = array();
    foreach ($key_segs as $field_name => $direction) {
        $quoted_segs[o2_mysql_o.$field_name.o2_mysql_c] = $direction;
        }
    $query = "ALTER TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                            o2_mysql_o.$table.o2_mysql_c.
                    " ADD ".o2_mysql_index_create($index_name,
                                                  $quoted_segs,
                                                  $table,
                                                  $database,
                                                  $owner,
                                                  $unique);
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a MySQL table to remove an existing index
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  string  $database
 * @param  string  $owner
 * @param  string  $table
 * @param  string  $index_name
 * @return boolean
 */
function o2_mysql_index_remove($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $index_name) {

    $query = "ALTER TABLE ".o2_mysql_o.$database.o2_mysql_c.".".
                            o2_mysql_o.$table.o2_mysql_c.
             " DROP INDEX ".o2_mysql_o.$index_name.o2_mysql_c;
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Return an array of calculated MySQL aggragate functions for the table.
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
function o2_mysql_aggregate($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where,
                            $functions,
                            $links = null) {

    $functions_list = "";
    foreach ($functions as $func_name => $single_func) {
        $functions_list.= ($functions_list ? "," : "").
                           $single_func["func"]."(".
                           $single_func["field"].") AS ".o2_mysql_o.$func_name.o2_mysql_c;
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_mysql_o.$database.o2_mysql_c.".".o2_mysql_o.$table.o2_mysql_c;
        }
    $query = "SELECT ".$functions_list." FROM ".$table.
             " AS ".o2_mysql_o.$table_alias.o2_mysql_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBAGGREGATE);
        throw $o2e;
        }
    return $res[0];

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
function o2_mysql_verifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by) {

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_mysql_o.$database.o2_mysql_c.".".o2_mysql_o.$table.o2_mysql_c;
        }
    $query_local = "SELECT ".$select_str." FROM ".$table.
                   " AS ".o2_mysql_o.$table_alias.o2_mysql_c;
    if (trim($where) != "") {
        $query_local.= " WHERE ".$where;
        }
    if (trim($order_by) != "") {
        $query_local.= " ORDER BY ".$order_by;
        }
    $query_local.=" LIMIT 1";
    try {
        $res_local = o2_mysql_execute($query_local,
                                      $server,
                                      $user,
                                      $password,
                                      false,
                                      false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECQUERY);
        throw $o2e;
        }
    if (!$res_local || $res_local === array()) {
        return false;
        }
    else {
        return $res_local[0];
        }

    }


/**
 * Modifies MySQL record, uniquely identified by $where clause, with values in $sets.
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
function o2_mysql_modifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $sets,
                            $where) {

    $upd_str = '';
    foreach ($sets as $field => $value) {
        $upd_str.= ($upd_str != "" ? "," : "").$field." = ".$value;
        }
    $query_local = "UPDATE ".o2_mysql_o.$database.o2_mysql_c.".".
                             o2_mysql_o.$table.o2_mysql_c.
                   " AS ".o2_mysql_o.$table_alias.o2_mysql_c.
                   " SET ".$upd_str." WHERE ".$where;
    try {
        o2_mysql_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECUPDATE);
        throw $o2e;
        }
    return true;

    }


/**
 * Insert passed record fields in a MySQL table.
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
function o2_mysql_insertrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $fields,
                            $values) {

    $query_local = "INSERT INTO ".o2_mysql_o.$database.o2_mysql_c.".".
                                  o2_mysql_o.$table.o2_mysql_c.
                   "(".implode(",", $fields).") VALUES (".implode(",", $values).")";
    try {
        o2_mysql_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECINSERT);
        throw $o2e;
        }
    return true;

    }


/**
 * Delete record of a MySQL table for the passed $where clause.
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
function o2_mysql_deleterec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where) {

    $query_local = "DELETE FROM ".o2_mysql_o.$database.o2_mysql_c.".".
                                  o2_mysql_o.$table.o2_mysql_c;
    if ($where) {
        // _________________________ MySQL does not support alias for DELETE statement ___
        $where       = str_replace(o2_mysql_o.$table_alias.o2_mysql_c.".", "", $where);
        $query_local.= " WHERE ".$where;
        }
    try {
        o2_mysql_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECDELETE);
        throw $o2e;
        }
    return true;

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
function o2_mysql_count($server,
                        $user,
                        $password,
                        $database,
                        $owner,
                        $table,
                        $table_alias,
                        $where,
                        $links = null) {

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_mysql_o.$database.o2_mysql_c.".".o2_mysql_o.$table.o2_mysql_c;
        }
    $query = "SELECT COUNT(*) AS COMPUTED FROM ".$table.
             " AS ".o2_mysql_o.$table_alias.o2_mysql_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // ______________________________________________________ Get connection to server ___
    $conn = o2_mysql_connect($server, $user, $password, false);
    // __________________________________________________________ On execution failure ___
    if (!($res = $conn->query($query))) {
        $err = $conn->errorInfo();
        $rb  = $conn->rollBack;
        $rb && @$rb();
        throw new o2_exception("<b>MySQL</b> server: <i>".$server.
                               "</i><br />Error counting dataset.<hr />".
                               $err[2],
                               o2error_DBCOUNT);
        return false;
        }
    $dset = $res->fetchAll(PDO::FETCH_ASSOC);
    // _____________________________________ Return integer value stored in "COMPUTED" ___
    return intval($dset[0]['COMPUTED']);

    }


/**
 * Returns a set of $recs records from a MySQL table for a passed $where clause.
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
 * @return array
 */
function o2_mysql_recordset($server,
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
                            $links = null,
                            $lock  = false) {

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_mysql_o.$database.o2_mysql_c.".".o2_mysql_o.$table.o2_mysql_c;
        }
    $query = "SELECT ".$select_str." FROM ".$table.
             " AS ".o2_mysql_o.$table_alias.o2_mysql_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    if ($order_by &&
        (!isset($GLOBALS['jxviewsql']) ||
         (strpos($GLOBALS['jxviewsql'], 'O') !== false))) {
        $query.= " ORDER BY ".$order_by;
        }
    if (!isset($GLOBALS['jxviewsql']) ||
        (strpos($GLOBALS['jxviewsql'], 'L') !== false)) {
        $query.= " LIMIT ".abs($recs);
        }
    // _________________________________________ Return SQL query instead of executing ___
    if (isset($GLOBALS['jxviewsql'])) {
        $GLOBALS['jxviewsql'] = $query;
        return array();
        }
    if ($lock) {
        $query.= " FOR UPDATE";
        }
    try {
        $res = o2_mysql_execute($query, $server, $user, $password, false, $lock);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBDATAQUERY);
        throw $o2e;
        }
    return $res;

    }


/**
 * Alter MySql table to create a foreign key.
 * $main_fields and $ref_fields are list of filed names, from main and referenced
 * tables to create the key on.
 *
 *  === NOT IMPLEMENTED YET ===
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
function o2_mysql_fkeyadd($server,
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

    return false;

    }


/**
 * Alter MySQL table to remove a foreign key.
 *
 *  === NOT IMPLEMENTED YET ===
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
function o2_mysql_fkeyremove($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $key_name) {

    return false;

    }


/**
 * Validate a foreign key in MySql table against existing data.
 *
 *  === NOT IMPLEMENTED YET ===
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
function o2_mysql_fkeyvalidate($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

    return false;

    }


/**
 * Returns the list of existing foreign keys for MySql table
 *
 *  === NOT IMPLEMENTED YET ===
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_mysql_fkeystablist($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table) {

    return array();

    }


/**
 * Commit open transaction on MySQL database
 *
 * @param string $server
 * @param string $user
 * @param string $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_mysql_commit($server, $user, $password, $end = false) {

    $key = $server.$user;
    // ___________________________________________________________ Prevent blank names ___
    if (!$server) {
        $server = "localhost";
        }
    // ________________________________________________ Check for existing transaction ___
    if ($GLOBALS['o2_mysql_trans'][$key]) {
        // ___________________________________ Check for errors on current transaction ___
        if ($GLOBALS['o2_mysql_error'][$key]) {
            $GLOBALS['o2_mysql_conn'][$key]->rollBack();
            unset($GLOBALS['o2_mysql_error'][$key]);
            }
        // ________________________________________________ Commit current transaction ___
        elseif (!$GLOBALS['o2_mysql_conn'][$key]->commit()) {
            $err = $GLOBALS['o2_mysql_conn'][$key]->errorInfo();
            throw new o2_exception("<b>MySQL</b> server: <i>".$server."</i><hr />".
                                   $err[2],
                                   o2error_DBCOMMIT);
            }
        unset($GLOBALS['o2_mysql_trans'][$key]);
        }
    // _________________________________________________ Check for existing connection ___
    if ($end && $GLOBALS['o2_mysql_conn'][$key]) {
        unset($GLOBALS['o2_mysql_conn'][$key]);
        }

    }

?>
