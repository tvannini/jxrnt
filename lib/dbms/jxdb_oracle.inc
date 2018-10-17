<?php

/**
 * Janox Oracle Database Gateway
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
 * This script contains Janox data functions set for Oracle database
 *
 * @name      jxoracle
 * @package   janox/dbms/jxdb_oracle.php
 * @version   2.4
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


if (!extension_loaded("oci8")) {
    throw new o2_exception("<b>Oracle OCI8</b> driver not loaded:<br />".
                           "check your PHP configuration for <i>oci8</i> extension.",
                           o2error_DBCONNECT);
    }

define("o2_oracle_o", '"');
define("o2_oracle_c", '"');
$o2_oracle_conn  = array();
$o2_oracle_trans = array();


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string $string
 * @return string
 */
function o2_oracle_normalize($string) {

    return strtr(trim($string), array("'" => "''"));

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as "schema"."tab_name"
 * If $name is omitted a fully qualified prefix will be returned, in the form "schema"
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $string
 * @return string
 */
function o2_oracle_qualify($database, $owner, $name = "") {

    return o2_oracle_o.$owner.o2_oracle_c.
           ($name ? ".".o2_oracle_o.$name.o2_oracle_c : "");

    }


/**
 * Return a Oracle connection handle
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  boolean $trans
 * @return PDO
 */
function o2_oracle_connect($server, $user, $password, $trans) {

    $app = $_SESSION['o2_app'];
    // ____________________________________ Isolated transaction: start new connection ___
    if ($app->isolated_trans) {
        $key  = $server.$user."jxext";
        $func = "oci_new_connect";
        }
    // _____________________________ In current transaction: use persistent connection ___
    else {
        $key  = $server.$user;
        $func = "oci_pconnect";
        }
    // _________________________________________________ Check for existing connection ___
    if (!isset($GLOBALS['o2_oracle_conn'][$key])) {
        // _______________________________________________________ Connection encoding ___
        $chr_encoding = "";
        if ($app->chr_encoding) {
            switch (strtolower(str_replace(array("-", "_"), "", $app->chr_encoding))) {
                case "windows1252":
                case "cp1252":
                    $chr_encoding = "WE8MSWIN1252";
                    break;
                case "utf8":
                default:
                    $chr_encoding = "UTF8";
                    break;
                }
            }
        // _____________________________________________________ Create new connection ___
        if (!($conn_local = $func($user, $password, $server, $chr_encoding))) {
            $err = oci_error();
            throw new o2_exception("<b>Oracle</b> server: <i>".$server."</i><hr />".
                                   $err['message'],
                                   o2error_DBCONNECT);
            }
        $stmt = oci_parse($conn_local, "ALTER SESSION SET NLS_NUMERIC_CHARACTERS = '.,'");
        oci_execute($stmt, OCI_NO_AUTO_COMMIT);
        $GLOBALS['o2_oracle_conn'][$key] = $conn_local;
        }
    // __________________________________________________ Create transaction if needed ___
    if ($trans) {
        $GLOBALS['o2_oracle_trans'][$key] = true;
        }
    return $GLOBALS['o2_oracle_conn'][$key];

    }


/**
 * Execute a query on the given Oracle server.
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
function o2_oracle_execute($query,
                           $server,
                           $user,
                           $password,
                           $only_exe = false,
                           $trans    = true,
                           $limit    = 0) {

    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // ______________________________________________________ Get connection to server ___
    $conn = o2_oracle_connect($server, $user, $password, $trans);
    // _____________________________________________ Execution only, no dataset return ___
    if ($only_exe) {
        $stmt = oci_parse($conn, $query);
        // ______________________________________________________ On execution failure ___
        if (oci_execute($stmt, OCI_NO_AUTO_COMMIT) === false) {
            $err = oci_error($stmt);
            oci_rollback($conn);
            throw new o2_exception("<b>Oracle</b> server: <i>".$server.
                                   "</i><br>query: <code>".$err['sqltext'].
                                   "</code><hr>".$err['message'].
                                   ($err['offset'] ?
                                    "<br>At character ".$err['offset'] : ""),
                                   o2error_DBEXECUTE);
            return false;
            }
        return oci_num_rows($stmt);
        }
    // ________________________________________________________ Queries returning data ___
    else {
        $stmt = oci_parse($conn, $query);
        // ______________________________________________________ On execution failure ___
        if (!($res = oci_execute($stmt, OCI_NO_AUTO_COMMIT))) {
            $err = oci_error($stmt);
            oci_rollback($conn);
            throw new o2_exception("<b>Oracle</b> server: <i>".$server.
                                   "</i><br>query: <code>".$err['sqltext'].
                                   "</code><hr>".$err['message'].
                                   ($err['offset'] ?
                                    "<br>At character ".$err['offset'] : ""),
                                   o2error_DBEXECUTE);
            return false;
            }
        // ____________________________________________________________ Return dataset ___
        $arr_return = array();
        $counter    = 0;
        while ((!$limit || $counter < $limit) && ($row = @oci_fetch_assoc($stmt))) {
            $counter++;
            $arr_local = array();

            foreach ($row as $field => $value) {
                if (is_object($value) && get_class($value) == "OCI-Lob") {
                    while (!$value->eof()) {
                        $arr_local[$field].= $value->read(2000);
                        }
                    $arr_local[$field] = rtrim($arr_local[$field]);
                    }
                else {
                    $arr_local[$field] = rtrim($value);
                    }
                }
            $arr_return[] = $arr_local;
            }
        return $arr_return;
        }

    }


/**
 * Retrieves user tables list from a Oracle database
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_oracle_tables($server, $user, $password, $database, $owner) {

    $query = "SELECT TABLE_NAME FROM ALL_TABLES WHERE OWNER='".$owner."'";
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLES);
        throw $o2e;
        }
    $arr_local = array();
    $tables    = array();
    foreach ($res as $tab) {
        $tables[] = $tab['TABLE_NAME'];
        }
    return $tables;

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
function o2_oracle_tabexists($server, $user, $password, $database, $owner, $table) {

    $query = "SELECT 1 IT_EXISTS FROM ALL_TABLES WHERE ".
             "OWNER = '".$owner."' AND TABLE_NAME='".$table."'";
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABEXISTS);
        throw $o2e;
        }
    return ($res[0]["IT_EXISTS"] ? true : false);

    }


/**
 * Return ORDER_BY clausole for all the fields of an index in a Oracle context.
 * List has to be comma separated and rightly qualified.
 *
 * $fields array parameter is passed in the form:
 *    $fields[field_name] = direction [D/A]
 *
 * @param  array  $fields
 * @return string
 */
function o2_oracle_indexfields_create($fields, $with_dir = true) {

    $str = "";
    foreach ($fields as $field => $direction) {
        $str.= ($str ? "," : "").$field.
               ($with_dir ? ($direction == "D" ? " DESC" : " ASC") : "");
        }
    return $str;

    }


/**
 * Return an array containing informations about fileds of a Oracle table:
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
function o2_oracle_tablefields($server, $user, $password, $database, $owner, $table) {

    $query = "SELECT * FROM ALL_TAB_COLUMNS WHERE TABLE_NAME='".$table.
             "' AND OWNER='".$owner."'";
    try {
        $res = o2_oracle_execute($query, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    $list = array();
    foreach ($res as $field) {
        $list[] = array('field'   => $field['COLUMN_NAME'],
                        'type'    => ($field['DATA_TYPE'] == "CLOB" ?
                                      "TEXT" :
                                      $field['DATA_TYPE']."(".
                                      ($field['CHAR_LENGTH'] ?
                                       $field['CHAR_LENGTH'] :
                                       $field['DATA_LENGTH']).")"),
                        'default' => str_replace("(",
                                                 "",
                                                 str_replace(")",
                                                             "",
                                                             str_replace("'",
                                                                         "",
                                                               $field['DATA_DEFAULT']))));
       }
    return $list;

    }


/**
 * Return an array containing informations about indexes of a Oracle table:
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
function o2_oracle_tableindexes($server, $user, $password, $database, $owner, $table) {

    $ret_val = array();
    $query   = "SELECT * FROM ALL_IND_COLUMNS WHERE TABLE_NAME='".$table.
               "' AND TABLE_OWNER='".$owner."'";
    try {
        $res = o2_oracle_execute($query, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    foreach ($res as $single_segment) {
        $key_name = $single_segment['INDEX_NAME'];
        if (substr($key_name, 0, strlen($table)) == $table) {
            $key_name = substr($key_name, strlen($table));
            }
        if (!isset($ret_val[$key_name])) {
            $ret_val[$key_name] = array();
            }
        $segm_dir = ($single_segment['DESCEND'] != "DESC" ? "A" : "D");
        if ($segm_dir == "A") {
            $column_name = $single_segment['COLUMN_NAME'];
            }
        else {
            $query = "SELECT * FROM ALL_IND_EXPRESSIONS WHERE TABLE_NAME='".
                     $table."' AND TABLE_OWNER='".$owner.
                     "' AND COLUMN_POSITION=".$single_segment['COLUMN_POSITION'];
            try {
                $res2 = o2_oracle_execute($query, $server, $user, $password);
                }
            catch (o2_exception $o2e) {
                $o2e->set_error_class(o2error_DBTABLEINFO);
                throw $o2e;
                }
            $column_name = substr($res2[0]['COLUMN_EXPRESSION'], 1, -1);
            }
        $ret_val[$key_name]+= array(($single_segment['COLUMN_POSITION'] - 1) =>
                                    array('column' => $column_name,
                                          'dir'    => $segm_dir));
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
function o2_oracle_insertfrom($server,
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
    $query = "INSERT INTO ".o2_oracle_o.$owner_to.o2_oracle_c.".".
                            o2_oracle_o.$table_to.o2_oracle_c.
             " (".$list_to.") SELECT ".$list_from.
             " FROM ".o2_oracle_o.$owner_from.o2_oracle_c.".".
                      o2_oracle_o.$table_from.o2_oracle_c;
    if ($where) {
       $query.= " WHERE ".$where;
       }
    try {
        $aff_rows = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLECOPY);
        throw $o2e;
        return false;
        }
    return $aff_rows;

    }


/**
 * Phisically removes a Oracle table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_oracle_droptable($server, $user, $password, $database, $owner, $table) {

    $query = "DROP TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                           o2_oracle_o.$table.o2_oracle_c;
    try {
        o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEDROP);
        throw $o2e;
        }
    o2_oracle_commit($server, $user, $password, $database);
    return true;

    }


/**
 * Phisically renames a Oracle table
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
function o2_oracle_renametable($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $new_name) {

    try {
        // ______________________________________________________ Get existing indexes ___
        $query    = "SELECT DISTINCT(INDEX_NAME) FROM ALL_IND_COLUMNS WHERE TABLE_NAME='".
                    $table."' AND TABLE_OWNER='".$owner."'";
        $idx_list = o2_oracle_execute($query,
                                      $server,
                                      $user,
                                      $password,
                                      false,
                                      false);
        // ______________________________________________________________ Rename table ___
        $query = "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                                o2_oracle_o.$table.o2_oracle_c.
                 " RENAME TO ".o2_oracle_o.$new_name.o2_oracle_c;
        o2_oracle_execute($query, $server, $user, $password, true, true);
        // ____________________________________________________________ Rename indexes ___
        foreach ($idx_list as $idx) {
            $new_idx  = substr($idx['INDEX_NAME'], strlen($table));
            $full_idx = $new_name.$new_idx;
            if (strlen($full_idx) > 30) {
                $full_idx = $new_name.substr(md5($new_idx), strlen($new_name) - 30);
                }
            $query   = "ALTER INDEX ".o2_oracle_o.$owner.o2_oracle_c.".".
                                      o2_oracle_o.$idx['INDEX_NAME'].o2_oracle_c.
                       " RENAME TO ".o2_oracle_o.$full_idx.o2_oracle_c."\n";
            o2_oracle_execute($query, $server, $user, $password, true, true);
            }
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEREBUILD);
        throw $o2e;
        return false;
        }
    o2_oracle_commit($server, $user, $password, $database);
    return true;

    }


/**
 * If $execute is passed TRUE function phisically creates a Oracle table, else it
 * returns the Oracle creation script for the table.
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
function o2_oracle_createtable($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $structure,
                               $execute = true) {

    // _________________________________________ Block tables names over 24 characters ___
    if (strlen($table) > 24 && substr($table, -6) != '_o2log') {
        throw new o2_exception("<b>Oracle</b> Janox Gateway:<br>Tables names can't be ".
                               "longer than 24 characters.<br>Table name <i>".$table.
                               "</i> is too long.",
                               o2error_DBTABLECREATE);
        return false;
        }
    $query   = "CREATE TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                               o2_oracle_o.$table.o2_oracle_c." (\n";
    $type    = "";
    $default = "";
    foreach ($structure["fields"] as $field_name => $field_prop) {
        // _______________ Block columns names over 24 characters (26 with delimiters) ___
        if (strlen($field_name) > 26) {
            throw new o2_exception("<b>Oracle</b> Janox Gateway:<br>Columns names can't ".
                                   "be longer than 24 characters.<br>Column name <i>".
                                   $field_name."</i> in table <i>".$table.
                                   "</i> is too long (".strlen($field_name).
                                   " characters).",
                                   o2error_DBTABLECREATE);
            return false;
            }
        $query.= o2_oracle_field_create($field_name,
                                        $field_prop['type'],
                                        $field_prop['size'],
                                        $field_prop['int'],
                                        $field_prop['dec']).",\n";
        }
    $query  = substr($query, 0, -2).")\n";
    $script = array();
    // _______________________________________________________________________ INDEXES ___
    foreach ($structure["keys"] as $key_name => $key_segs) {
        $full_name = $table.$key_name;
        if (strlen($full_name) > 30) {
            $full_name = $table.substr(md5($key_name), strlen($table) - 30);
            }
        $script[$full_name] = o2_oracle_index_create($full_name,
                                                     $key_segs,
                                                     $table,
                                                     $database,
                                                     $owner);
        }
    // ___________________________________________________________________ PRIMARY KEY ___
    foreach ($structure["primary"] as $pk_name => $pk_segments) {
        break;
        }
    $full_name = $table.$pk_name."PK";
    if (strlen($full_name) > 30) {
        $full_name = $table.substr(md5($pk_name."PK"), strlen($table) - 30);
        }
    $script["P"] = "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                                  o2_oracle_o.$table.o2_oracle_c.
                   " ADD CONSTRAINT ".o2_oracle_o.$full_name.o2_oracle_c.
                   " PRIMARY KEY (".
                   o2_oracle_indexfields_create($pk_segments, false).")";
    if ($execute) {
        try {
            o2_oracle_execute($query, $server, $user, $password, true, true);
            foreach ($script as $key_create) {
                o2_oracle_execute($key_create, $server, $user, $password, true, true);
                }
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        o2_oracle_commit($server, $user, $password, $database);
        return true;
        }
    else {
        return $query.";\n".implode(";", $script);
        }

    }


/**
 * Returns Oracle definition for single field by Janox-type
 *
 * @param  string  $name         Field name
 * @param  string  $type         Field type (A|N|L|D|T|S)
 * @param  integer $size         Total size for alpha
 * @param  integer $int          Total size or integer part
 * @param  integer $dec          Number of decimals digits (only numbers)
 * @param  boolean $for_change   If script is requested for ALTER COLUMN syntax
 * @return string                SQL code to create field
 */
function o2_oracle_field_create($name, $type, $size, $int, $dec, $for_change = false) {

    switch ($type) {
        case "A":
            if ($size <= 4000) {
                $type    = "varchar2(".$size." CHAR)";
                $default = " DEFAULT (' ') NOT NULL";
                }
            else {
                $type    = "clob";
                $default = "";
                }
            break;
        case "N":
            $type    = "number(".($int + $dec).",".$dec.")";
            $default = " DEFAULT (0) NOT NULL";
            break;
        case "L":
            $type    = "char(1)";
            $default = " DEFAULT ('0') NOT NULL";
            break;
        case "D":
            $type    = "char(8)";
            $default = " DEFAULT ('00000000') NOT NULL";
            break;
        case "T":
            $type    = "char(6)";
            $default = " DEFAULT ('000000') NOT NULL";
            break;
        case "S":
            $type    = "clob";
            $default = "";
            break;
        }
    if ($for_change) {
        return $name." TYPE ".$type;
        }
    else {
        return $name." ".$type.$default;
        }

    }


/**
 * Returns Oracle definition for single index by Janox-type.
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
function o2_oracle_index_create($key_name,
                                $key_segs,
                                $table,
                                $database,
                                $owner,
                                $unique = true) {

    return 'CREATE'.($unique ? ' UNIQUE' : '').
           ' INDEX '.o2_oracle_o.$key_name.o2_oracle_c.
              ' ON '.o2_oracle_o.$owner.o2_oracle_c.'.'.
                     o2_oracle_o.$table.o2_oracle_c.
                ' ('.o2_oracle_indexfields_create($key_segs).')';


    }


/**
 * Alter a Oracle table to add a column
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
function o2_oracle_field_add($server,
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

    $query = "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                            o2_oracle_o.$table.o2_oracle_c.
                    " ADD ".o2_oracle_field_create(o2_oracle_o.$field_name.o2_oracle_c,
                                                   $field_type,
                                                   $field_size,
                                                   $field_int,
                                                   $field_dec);
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Oracle table to remove a column
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
function o2_oracle_field_remove($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $field_name) {

    $query =  "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                             o2_oracle_o.$table.o2_oracle_c.
             " DROP COLUMN ".o2_oracle_o.$field_name.o2_oracle_c;
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Oracle table to change a column definition
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
function o2_oracle_field_change($server,
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

    $query =   "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                              o2_oracle_o.$table.o2_oracle_c.
             " ALTER COLUMN ".o2_oracle_field_create(o2_oracle_o.$field_name.o2_oracle_c,
                                                     $field_type,
                                                     $field_size,
                                                     $field_int,
                                                     $field_dec,
                                                     true);
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Oracle table to rename a column
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
function o2_oracle_field_rename($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $old_name,
                                $new_name) {

    $query =    "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                               o2_oracle_o.$table.o2_oracle_c.
             " RENAME COLUMN ".o2_oracle_o.$old_name.o2_oracle_c.
                        " TO ".o2_oracle_o.$new_name.o2_oracle_c;
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Oracle table to add an index
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
function o2_oracle_index_add($server,
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
        if ($field_name != "O2ASPID") {
            $quoted_segs[o2_oracle_o.$field_name.o2_oracle_c] = $direction;
            }
        else {
            $quoted_segs[$field_name] = $direction;
            }
        }
    $query = o2_oracle_index_create($index_name,
                                    $quoted_segs,
                                    $table,
                                    $database,
                                    $owner,
                                    $unique);
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Oracle table to remove an existing index
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
function o2_oracle_index_remove($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $index_name) {

    $query = "DROP INDEX ".o2_oracle_o.$owner.o2_oracle_c.".".
                           o2_oracle_o.$index_name.o2_oracle_c;
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Return an array of calculated Oracle aggragate functions for the table.
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
function o2_oracle_aggregate($server,
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
                           $single_func["func"]."(".$single_func["field"].") ".
                           "AS ".o2_oracle_o.$func_name.o2_oracle_c;
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_oracle_o.$owner.o2_oracle_c.".".o2_oracle_o.$table.o2_oracle_c;
        }
    $query = "SELECT ".$functions_list.
                     " FROM ".$table." ".o2_oracle_o.$table_alias.o2_oracle_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, false, false);
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
function o2_oracle_verifyrec($server,
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
        $table = o2_oracle_o.$owner.o2_oracle_c.".".o2_oracle_o.$table.o2_oracle_c;
        }
    $query = "SELECT ".$select_str." FROM ".$table.
             " ".o2_oracle_o.$table_alias.o2_oracle_c;
    if ($where) {
        $query.= " WHERE ".$where;
        }
    if ($order_by) {
        $query.= " ORDER BY ".$order_by;
        }
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, false, false, 1);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECQUERY);
        throw $o2e;
        }
    if (!$res || $res === array()) {
        return false;
        }
    else {
        return $res[0];
        }

    }


/**
 * Modifies Oracle record, uniquely identified by $where clause, with values in $sets.
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
function o2_oracle_modifyrec($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $table_alias,
                             $sets,
                             $where) {

    $upd_str = '';
    $where   = str_replace(o2_oracle_o.$table_alias.o2_oracle_c.'.', "", $where);
    $lslist  = array();
    $lsidx   = -1;
    foreach ($sets as $field => $value) {
        // ___________________________________________________________ LOB field value ___
        if (strlen($value) > 4000) {
            $lsidx++;
            $upd_str.= ($upd_str ? "," : "").$field."=EMPTY_CLOB()";
            $lslist[$lsidx] = array($field,
                                    ($value == "''" ? " " : substr($value, 1, -1)));
            }
        // _____________________________________________ Standard (no LOB) field value ___
        else {
            $upd_str.= ($upd_str ? "," : "").$field." = ".
                       ($value == "''" ? "' '" : $value);
            }
        }
    $query = "UPDATE ".o2_oracle_o.$owner.o2_oracle_c.".".
                       o2_oracle_o.$table.o2_oracle_c." ".
             " SET ".$upd_str." WHERE ".$where;
    // ________________________ If LOBs are present add LOBs fields reference to query ___
    if ($lsidx > -1) {
        $fldstr = "";
        $valstr = "";
        $varstr = "";
        foreach ($lslist as $listidx => $keyval) {
            $fldstr.= $keyval[0].",";
            $varstr.= ":jxlongstr".$listidx.",";
            }
        $fldstr = substr($fldstr, 0, -1);
        $varstr = substr($varstr, 0, -1);
        $query.= " RETURNING ".$fldstr." INTO ".$varstr;
        }
    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // ________________________________________________ Try to catch connection errors ___
    try {
        $conn = o2_oracle_connect($server, $user, $password, true);
        $stmt = oci_parse($conn, $query);
        // _____________________ If LOBs are present bind LOBs variables to LOB fields ___
        if ($lsidx > -1) {
            $loblist = array();
            for ($listidx = 0; $listidx <= $lsidx; $listidx++) {
                $loblist[$listidx] = oci_new_descriptor($conn, OCI_D_LOB);
                oci_bind_by_name($stmt,
                                 ":jxlongstr".$listidx,
                                 $loblist[$listidx],
                                 -1,
                                 OCI_B_CLOB);
                }
            }
        // ________________________________________________________ If error on update ___
        if (oci_execute($stmt, OCI_NO_AUTO_COMMIT) === false) {
            $err = oci_error($stmt);
            oci_rollback($conn);
            throw new o2_exception("<b>Oracle</b> server: <i>".$server.
                                   "</i><br>query: <code>".$err['sqltext'].
                                   "</code><hr>".$err['message'].
                                   ($err['offset'] ?
                                    "<br>At character ".$err['offset'] : ""),
                                   o2error_DBRECUPDATE);
            return false;
            }
        // ___________________________________________________________ If update is OK ___
        else {
            // _________________________________ If LOBs are present write single LOBs ___
            if ($lsidx > -1) {
                foreach ($loblist as $listidx => $lob) {
                    $loblist[$listidx]->save($lslist[$listidx][1]);
                    }
                }
            }
        }
    // __________________________________________________ Rethrow connection exception ___
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECUPDATE);
        throw $o2e;
        }
    return true;

    }


/**
 * Insert passed record fields in a Oracle table.
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
function o2_oracle_insertrec($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $table_alias,
                             $fields,
                             $values) {

    $values_str = "";
    $lslist  = array();
    $lsidx   = -1;
    foreach ($values as $val_idx => $value) {
        // ___________________________________________________________ LOB field value ___
        if (strlen($value) > 4000) {
            $lsidx++;
            $values_str.= "EMPTY_CLOB(),";
            $lslist[$lsidx] = array($fields[$val_idx],
                                    ($value == "''" ? " " : substr($value, 1, -1)));
            }
        // _____________________________________________ Standard (no LOB) field value ___
        else {
            $values_str.= ($value == "''" ? "' '" : $value).",";
            }
        }
    $query = "INSERT INTO ".
             o2_oracle_o.$owner.o2_oracle_c.".".o2_oracle_o.$table.o2_oracle_c.
             " (".implode(",", $fields).") VALUES (".substr($values_str, 0, -1).")";
    // ________________________ If LOBs are present add LOBs fields reference to query ___
    if ($lsidx > -1) {
        $fldstr = "";
        $valstr = "";
        $varstr = "";
        foreach ($lslist as $listidx => $keyval) {
            $fldstr.= $keyval[0].",";
            $varstr.= ":jxlongstr".$listidx.",";
            }
        $fldstr = substr($fldstr, 0, -1);
        $varstr = substr($varstr, 0, -1);
        $query.= " RETURNING ".$fldstr." INTO ".$varstr;
        }
    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // ________________________________________________ Try to catch connection errors ___
    try {
        $conn = o2_oracle_connect($server, $user, $password, true);
        $stmt = oci_parse($conn, $query);
        // _____________________ If LOBs are present bind LOBs variables to LOB fields ___
        if ($lsidx > -1) {
            $loblist = array();
            for ($listidx = 0; $listidx <= $lsidx; $listidx++) {
                $loblist[$listidx] = oci_new_descriptor($conn, OCI_D_LOB);
                oci_bind_by_name($stmt,
                                 ":jxlongstr".$listidx,
                                 $loblist[$listidx],
                                 -1,
                                 OCI_B_CLOB);
                }
            }
        // ________________________________________________________ If error on insert ___
        if (oci_execute($stmt, OCI_NO_AUTO_COMMIT) === false) {
            $err = oci_error($stmt);
            oci_rollback($conn);
            // ___________________________________________ Remove existing transaction ___
            throw new o2_exception("<b>Oracle</b> server: <i>".$server.
                                   "</i><br>query: <code>".$err['sqltext'].
                                   "</code><hr>".$err['message'].
                                   ($err['offset'] ?
                                    "<br>At character ".$err['offset'] : ""),
                                   o2error_DBRECINSERT);
            return false;
            }
        // ___________________________________________________________ If insert is OK ___
        else {
            // _________________________________ If LOBs are present write single LOBs ___
            if ($lsidx > -1) {
                foreach ($loblist as $listidx => $lob) {
                    $loblist[$listidx]->save($lslist[$listidx][1]);
                    }
                }
            }
        }
    // __________________________________________________ Rethrow connection exception ___
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECINSERT);
        throw $o2e;
        }
    return true;

    }


/**
 * Delete record of a Oracle table for the passed $where clause.
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
function o2_oracle_deleterec($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $table_alias,
                             $where) {

    $query = "DELETE FROM ".o2_oracle_o.$owner.o2_oracle_c.".".
                            o2_oracle_o.$table.o2_oracle_c;
    if ($where) {
        $where = str_replace(o2_oracle_o.$table_alias.o2_oracle_c.'.', "", $where);
        $query.= " WHERE ".$where;
        }
    try {
        o2_oracle_execute($query, $server, $user, $password, true, true);
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
function o2_oracle_count($server,
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
        $table = o2_oracle_o.$owner.o2_oracle_c.".".o2_oracle_o.$table.o2_oracle_c;
        }
    $query = "SELECT COUNT(*) COMPUTED FROM ".$table.
             " ".o2_oracle_o.$table_alias.o2_oracle_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    // ______________________________________________________ Get connection to server ___
    $conn = o2_oracle_connect($server, $user, $password, false);
    $stmt = oci_parse($conn, $query);
    // __________________________________________________________ On execution failure ___
    if (!($res = oci_execute($stmt, OCI_NO_AUTO_COMMIT))) {
        $err = oci_error($stmt);
        oci_rollback($conn);
        throw new o2_exception("<b>Oracle</b> server: <i>".$server.
                               "</i><br>Error counting dataset.<br>Query: ".
                               $err['sqltext']."<hr />".$err['message'],
                               o2error_DBCOUNT);
        return false;
        }
    $dset = @oci_fetch_assoc($stmt);
    // _____________________________________ Return integer value stored in "COMPUTED" ___
    return intval($dset['COMPUTED']);

    }


/**
 * Returns a set of $recs records from a Oracle table for a passed $where clause.
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
function o2_oracle_recordset($server,
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

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_oracle_o.$owner.o2_oracle_c.".".o2_oracle_o.$table.o2_oracle_c;
        }
    $query = "SELECT ".$select_str." FROM ".$table.
             " ".o2_oracle_o.$table_alias.o2_oracle_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($page_where) {
        $where = $page_where.$where;
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    if ($order_by &&
        (!isset($GLOBALS['jxviewsql']) ||
         (strpos($GLOBALS['jxviewsql'], 'O') !== false))) {
        $query.= " ORDER BY ".$order_by;
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
        $res = o2_oracle_execute($query,
                                 $server,
                                 $user,
                                 $password,
                                 false,
                                 $lock,
                                 abs($recs));
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBDATAQUERY);
        throw $o2e;
        }
    return $res;

    }


/**
 * Alter Oracle table to create a foreign key.
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
function o2_oracle_fkeyadd($server,
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

    $main_list = array();
    $ref_list  = array();
    foreach ($main_fields as $field) {
        if ($field != "O2ASPID") {
            $main_list[] = o2_oracle_o.$field.o2_oracle_c;
            }
        else {
            $main_list[] = $field;
            }
        }
    foreach ($ref_fields as $field) {
        if (field != "O2ASPID") {
            $ref_list[] = o2_oracle_o.$field.o2_oracle_c;
            }
        else {
            $ref_list[] = $field;
            }
        }
    $main_list = implode(",", $main_list);
    $ref_list  = implode(",", $ref_list);
    $query     = "ALTER TABLE ".o2_oracle_o.$main_owner.o2_oracle_c.'.'.
                                o2_oracle_o.$main_table.o2_oracle_c.
                 " ADD CONSTRAINT ".o2_oracle_o.$key_name.o2_oracle_c.
                 " FOREIGN KEY (".$main_list.
                 ") REFERENCES ".o2_oracle_o.$ref_owner.o2_oracle_c.'.'.
                                 o2_oracle_o.$ref_table.o2_oracle_c.
                 " (".$ref_list.") ENABLE NOVALIDATE";
    try {
        o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Alter Oracle table to remove a foreign key.
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
function o2_oracle_fkeyremove($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $key_name) {

    $query = "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                            o2_oracle_o.$table.o2_oracle_c.
             " DROP CONSTRAINT ".o2_oracle_o.$key_name.o2_oracle_c;
    try {
        o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Validate a foreign key in Oracle table against existing data.
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
function o2_oracle_fkeyvalidate($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $key_name) {

    $query = "ALTER TABLE ".o2_oracle_o.$owner.o2_oracle_c.".".
                            o2_oracle_o.$table.o2_oracle_c.
             " MODIFY CONSTRAINT ".o2_oracle_o.$key_name.o2_oracle_c." VALIDATE";
    try {
        o2_oracle_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Returns the list of existing foreign keys for Oracle table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_oracle_fkeystablist($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table) {

    $query = "SELECT constraint_name ".o2_oracle_o."CN".o2_oracle_c.
             " FROM all_constraints WHERE owner='".$owner.
             "' AND table_name='".$table."' AND constraint_type='R'";
    try {
        $res = o2_oracle_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    $list = array();
    foreach ($res as $c_name) {
        $list[] = $c_name["CN"];
        }
    return $list;

    }


/**
 * Commit open transaction on Oracle database
 *
 * @param string $server
 * @param string $user
 * @param string $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_oracle_commit($server, $user, $password, $end = true) {

    $app = $_SESSION['o2_app'];
    // ____________________________________ Isolated transaction: start new connection ___
    if ($app->isolated_trans) {
        $key = $server.$user."jxext";
        }
    // _____________________________ In current transaction: use persistent connection ___
    else {
        $key = $server.$user;
        }
    // ________________________________________________ Check for existing transaction ___
    if ($GLOBALS['o2_oracle_trans'][$key]) {
        // ___________________________________ Check for errors on current transaction ___
        if ($GLOBALS['o2_oracle_error'][$key]) {
            oci_rollback($GLOBALS['o2_oracle_conn'][$key]);
            unset($GLOBALS['o2_oracle_error'][$key]);
            }
        // ________________________________________________ Commit current transaction ___
        elseif (!oci_commit($GLOBALS['o2_oracle_conn'][$key])) {
            $err = oci_error();
            throw new o2_exception("<b>Oracle</b> server: <i>".$server."</i><hr>".
                                   $err['message'],
                                   o2error_DBCOMMIT);
            }
        unset($GLOBALS['o2_oracle_trans'][$key]);
        }
    // _________________________________________________ Check for existing connection ___
    if ($end && $GLOBALS['o2_oracle_conn'][$key]) {
        unset($GLOBALS['o2_oracle_conn'][$key]);
        }

    }

?>
