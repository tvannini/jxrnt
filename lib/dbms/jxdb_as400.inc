<?php

/**
 * Janox AS400 (ser.4-5) Database Gateway
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
 * This script contains Janox data functions set for AS400 database series 4 & 5
 *
 * @name      jxas400
 * @package   janox/dbms/jxdb_as400.php
 * @version   2.9
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


define("o2_as400_sep", ",");
define("o2_as400_o", '"');
define("o2_as400_c", '"');
$GLOBALS['o2_as400_conn'] = array();


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_as400_normalize($string, $untrim = false) {

    return strtr(($untrim ? $string : trim($string)), array("\\" => "\\\\"));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_as400_concat($strings) {

    return '('.implode(' CONCAT ', $strings).')';

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as "db"."tab_name"
 * If $name is omitted a fully qualified prefix will be returned, in the form "db"
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $string
 * @return string
 */
function o2_as400_qualify($database, $owner, $name = "") {

    return o2_as400_o.$database.o2_as400_c.
           ($name ? ".".o2_as400_o.$name.o2_as400_c : "");

    }


/**
 * Return a db2 connection handle
 *
 * TODO: Manage isolated transaction
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @return resource
 */
function o2_as400_connect($server, $user, $password) {

    if (!isset($GLOBALS['o2_as400_conn'][$server])) {
        if (!($conn_local = @odbc_pconnect($server, $user, $password))) {
            throw new o2_exception("<b>AS400</b> server: <i>".$server.
                                   "</i><br>user: <i>".$user.
                                   "</i><hr>".odbc_errormsg($conn_local),
                                   o2error_DBCONNECT);
            }
        $GLOBALS['o2_as400_conn'][$server] = $conn_local;
//      odbc_exec($conn_local, "BEGIN TRANSACTION;");
        }
    return $GLOBALS['o2_as400_conn'][$server];

    }


/**
 * Execute a query on the given db2 server.
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
 * @param  string $query
 * @param  string $server
 * @return mix
 */
function o2_as400_execute($query, $server, $user, $password) {

//  print "<h2>".$query."</h2>";

    $connection = o2_as400_connect($server, $user, $password);
    if (!($res_local = @odbc_exec($connection, $query ))) {
        throw new o2_exception("<b>AS400</b> server: <i>".$server."</i><br>user: <i>".
                               $user."</i><br>query: <code>".$query."</code><hr>".
                               odbc_errormsg($connection),
                               o2error_DBEXECUTE);
        }
    elseif (is_resource($res_local)) {
        $arr_return = array();
        while($row = @odbc_fetch_array($res_local)) {
            $arr_local = array();
            foreach ($row as $field => $value) {
                $arr_local[$field] = rtrim($value);
                }
            $arr_return[] = $arr_local;
            }
        // For functions like COUNT(*) or @@FETCH_ROWS and aggregate functions
        if (isset($arr_return[0]['COMPUTED'])) {
            return intval($arr_return[0]['COMPUTED']);
            }
        return $arr_return;
        }
    else {
        return true;
        }

    }


/**
 * Retrieves user tables list from a db2 database
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_as400_tables($server, $user, $password, $database, $owner) {

    $connection = o2_as400_connect($server, $user, $password) ;
    if (!($res_local = @odbc_tables($connection,
                                    "%",
                                    $database,
                                    '%',
                                    "TABLE,VIEW"))) {
        throw new o2_exception("<b>AS400</b> server: <i>".$server."</i><br>user: <i>".
                               $user."</i><hr>".odbc_errormsg($connection),
                               o2error_DBTABLES);
        }
    else {
        $arr_local = array();
        while (odbc_fetch_row($res_local)) {
            $arr_local[] = odbc_result($res_local, 3);
            }
        }
    return $arr_local;

    }


/**
 * Return an array containing informations about fields of a db2 table:
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
function o2_as400_tablefields($server, $user, $password, $database, $owner, $table) {

    $connection = o2_as400_connect($server, $user, $password);
    if (!($res_local = odbc_columns($connection,
                                    '%',
                                    o2_as400_o.$database.o2_as400_c,
                                    o2_as400_o.$table.o2_as400_c,
                                    '%'))) {
        throw new o2_exception("<b>AS400</b> server: <i>".$server."</i><br>user: <i>".
                               $user."</i><hr>".odbc_errormsg($connection),
                               o2error_DBTABLEINFO);
        }
    else {
        $arr_local = array();
        while (odbc_fetch_row($res_local)) {
            $arr_local[] = array('field'   => odbc_result($res_local, 4),
                                 'type'    => odbc_result($res_local, 6)."(".
                                              odbc_result($res_local, 7).")",
                                 'default' => odbc_result($res_local, 9));
            }
        }
    return $arr_local;

    }


/**
 * Return an array containing informations about table keys of a db2 table:
 *
 *    $arr[key_name] = array(0 => array('Field' => column1_name),
 *                                      'Dir'   => column1_direction),
 *                           1 => array('Field' => column2_name),
 *                                      'Dir'   => column2_direction),
 *                           ...)
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return array
 */
function o2_as400_tableindexes($server, $user, $password, $database, $owner, $table) {

    $query_local = "SELECT * FROM QSYS2.SYSKEYS, QSYS2.SYSINDEXES ".
                   "WHERE QSYS2.SYSKEYS.INDEX_NAME = QSYS2.SYSINDEXES.INDEX_NAME AND ".
                   "QSYS2.SYSINDEXES.TABLE_NAME = '".strtoupper($table)."'";
    try {
        $res_local = o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    foreach ($res_local as $single_segment) {
        $key_name = str_replace($table."_", "", $single_segment['INDEX_NAME']);
        if (!isset($ret_val[$key_name])) {
            $ret_val[$key_name] = array();
            }
        $ret_val[$key_name]+= array(($single_segment['ORDINAL_POSITION'] - 1) =>
            array('column' => $single_segment['COLUMN_NAME'],
                  'dir'    => strtoupper($single_segment['ORDERING'])));
        ksort($ret_val[$key_name]);
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
function o2_as400_insertfrom($server,
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
        $sep = ($list_from ? o2_as400_sep : "");
        if ($field_from == "@o2CloneArea") {
            $list_from.= $sep.$field_to;
            $list_to  .= $sep."O2ASPID";
            }
        else {
            $list_from.= $sep.o2_as400_o.$table_from.o2_as400_c.".".
                              o2_as400_o.$field_from.o2_as400_c;
            $list_to  .= $sep.o2_as400_o.$field_to.o2_as400_c;
            }
        }
    $query_local = "INSERT INTO ".o2_as400_o.$database_to.o2_as400_c.".".
                                  o2_as400_o.$table_to.o2_as400_c.
                   " (".$list_to.") SELECT ".$list_from.
                   " FROM ".o2_as400_o.$database_from.o2_as400_c.".".
                            o2_as400_o.$table_from.o2_as400_c." AS ".
                            o2_as400_o.$table_from.o2_as400_c."\n";
    if ($where) {
       $query_local.= " WHERE ".$where;
       }
    try {
        $aff_rows = o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLECOPY);
        throw $o2e;
        return false;
        }
    return $aff_rows;

    }


/**
 * Phisically removes a db2 table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_as400_droptable($server, $user, $password, $database, $owner, $table) {

    $query_local = "DROP TABLE ".o2_as400_o.$database.o2_as400_c.".".
                                 o2_as400_o.$table.o2_as400_c."\n";
    try {
        o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEDROP);
        throw $o2e;
        }
    o2_as400_commit($server, $user, $password, $database);
    return true;

    }


/**
 * Phisically renames a db2 table
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
function o2_as400_renametable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $new_name) {

    $query_local = "RENAME TABLE ".o2_as400_o.$database.o2_as400_c.".".
                                   o2_as400_o.$table.o2_as400_c.
                   " TO ".o2_as400_o.$new_name.o2_as400_c."\n";
    try {
        o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEREBUILD);
        throw $o2e;
        }
    o2_as400_commit($server, $user, $password, $database);
    return true;

    }


/**
 * If $execute is passed TRUE function phisically creates a db2 table, else it returns
 * the db2 creation script for the table.
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
function o2_as400_createtable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $structure,
                              $execute = true) {

    $query_local = "CREATE TABLE ".o2_as400_o.$database.o2_as400_c.".".
                                   o2_as400_o.$table.o2_as400_c." (\n";
    $type        = "";
    $default     = "";
    foreach ($structure["fields"] as $field_name => $field_prop) {
        switch ($field_prop["type"]) {
            case "A":
                if ($field_prop["size"] <= 255) {
                    $type = "character(".$field_prop["size"].")";
                    }
                elseif ($field_prop["size"] <= 32735) {
                    $type = "varchar(".$field_prop["size"].")";
                    }
                else {
                    $type = "long varchar";
                    }
                $default = "''";
                break;
            case "N":
                if ($field_prop["dec"]) {
                    $type = "double";
                    }
                else {
                    if ($field_prop["size"] < 5) {
                        $type = "smallint";
                        }
                    elseif ($field_prop["size"] < 10) {
                        $type = "integer";
                        }
                    else {
                        $type = "bigint";
                        }
                    }
                $default = "0";
                break;
            case "L":
                $type    = "character(1)";
                $default = "'0'";
                break;
            case "D":
                $type    = "character(8)";
                $default = "'00000000'";
                break;
            case "T":
                $type    = "character(6)";
                $default = "'000000'";
                break;
            case "S":
                $type    = "long varchar";
                $default = "''";
                break;
            }
        $query_local.= $field_name." ".$type.
                       " NOT NULL WITH DEFAULT ".$default.o2_as400_sep."\n";
        }
    $query_local = substr($query_local, 0, -2).")\n";
    if ($execute) {
        try {
            o2_as400_execute($query_local, $server, $user, $password);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        $query_local = "";
        }
    // ___________________________________________________________________ PRIMARY KEY ___
    $arr_local = array();
    foreach ($structure["primary"] as $pk_name => $pk_segments) {
        foreach ($pk_segments as $field_name => $direction) {
            $arr_local[] = $field_name;
            }
        break;
        }
    $query_local.= "ALTER TABLE ".o2_as400_o.$database.o2_as400_c.".".
                                  o2_as400_o.$table.o2_as400_c.
                   " ADD PRIMARY KEY (".implode(o2_as400_sep, $arr_local).")\n";
    if ($execute) {
        try {
            o2_as400_execute($query_local, $server, $user, $password);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        $query_local = "";
        }
    // _______________________________________________________________________ INDEXES ___
    foreach ($structure["keys"] as $index_name => $index_segs) {
        $arr_local = array();
        foreach ($index_segs as $field_name => $direction) {
            $arr_local[] = $field_name.($direction == "D" ? " DESC" : " ASC");
            }
        $query_local.= "CREATE UNIQUE INDEX ".o2_as400_o.$database.o2_as400_c.".".
                       o2_as400_o.$table."_".$index_name.o2_as400_c.
                       " ON ".o2_as400_o.$database.o2_as400_c.".".
                              o2_as400_o.$table.o2_as400_c.
                       " (".implode(o2_as400_sep, $arr_local).")\n";
        if ($execute) {
            try {
                o2_as400_execute($query_local, $server, $user, $password);
                }
            catch (o2_exception $o2e) {
                $o2e->set_error_class(o2error_DBTABLECREATE);
                throw $o2e;
                }
            $query_local = "";
            }
        }
    if ($execute) {
        return true;
        }
    else {
        return $query_local;
        }

    }


/**
 * Return an array of calculated db2 aggragate functions for the table.
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
 * @return array
 */
function o2_as400_aggregate($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where,
                            $functions) {

    $functions_list = "";
    foreach ($functions as $func_name => $single_func) {
        $functions_list.= ($functions_list ? o2_as400_sep : "").
                           $single_func["func"]."(".$single_func["field"].") AS ".
                           o2_as400_o.$func_name.o2_as400_c;
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_as400_o.$database.o2_as400_c.".".o2_as400_o.$table.o2_as400_c;
        }
    $query_local = "SELECT ".$functions_list.
                   " FROM ".$table." AS ".o2_as400_o.$table_alias.o2_as400_c;
    if ($where) {
        $query_local.= " WHERE ".$where;
        }
    try {
        $res_local = o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBAGGREGATE);
        throw $o2e;
        }
    return $res_local[0];

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
function o2_as400_verifyrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by) {

    $query_local = "SELECT ".$select_str.
                   " FROM ".o2_as400_o.$database.o2_as400_c.".".
                            o2_as400_o.$table.o2_as400_c.
                   " AS ".o2_as400_o.$table_alias.o2_as400_c;
    if ($where) {
        $query_local.= " WHERE ".$where;
        }
    if ($order_by) {
        $query_local.= " ORDER BY ".$order_by;
        }
    $query_local.=" FETCH FIRST 1 ROWS ONLY";
    try {
        $res_local = o2_as400_execute($query_local, $server, $user, $password);
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
 * Modifies db2 record, uniquely identified by $where clause, with values in $sets.
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
function o2_as400_modifyrec($server,
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
        $upd_str.= ($upd_str != "" ? o2_as400_sep : "").
                   o2_as400_o.$table_alias.o2_as400_c.".".$field." = ".$value;
        }
    $query_local = "UPDATE ".o2_as400_o.$database.o2_as400_c.".".
                             o2_as400_o.$table.o2_as400_c.
                   " AS ".o2_as400_o.$table_alias.o2_as400_c.
                   " SET ".$upd_str.
                   " WHERE ".$where;
    try {
        o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECUPDATE);
        throw $o2e;
        }
    return true;

    }


/**
 * Insert passed record fields in a db2 table.
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
function o2_as400_insertrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $fields,
                            $values) {

    $query_local = "INSERT INTO ".o2_as400_o.$database.o2_as400_c.".".
                                  o2_as400_o.$table.o2_as400_c.
                   " (".implode(o2_as400_sep, $fields).
                   ") VALUES (".implode(o2_as400_sep, $values).")";
    try {
        o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECINSERT);
        throw $o2e;
        }
    return true;

    }


/**
 * Delete record of a db2 table for the passed $where clause.
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
function o2_as400_deleterec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where) {

    $query_local = "DELETE FROM ".o2_as400_o.$database.o2_as400_c.".".
                                  o2_as400_o.$table.o2_as400_c.
                   " AS ".o2_as400_o.$table_alias.o2_as400_c;
    if ($where) {
       $query_local.= " WHERE ".$where;
       }
    // _________________________________________ Return SQL query instead of executing ___
    if (isset($GLOBALS['jxviewsql'])) {
        $GLOBALS['jxviewsql'] = $query_local;
        return true;
        }
    try {
        o2_as400_execute($query_local, $server, $user, $password);
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
 * @return integer
 */
function o2_as400_count($server,
                        $user,
                        $password,
                        $database,
                        $owner,
                        $table,
                        $table_alias,
                        $where) {

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_as400_o.$database.o2_as400_c.".".o2_as400_o.$table.o2_as400_c;
        }
    $query_local = "SELECT COUNT(*) AS COMPUTED FROM ".$table;
    if ($where) {
        $query_local.= " AS ".o2_as400_o.$table_alias.o2_as400_c." WHERE ".$where;
        }
    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query_local);
        }
    try {
       $res_val = o2_as400_execute($query_local, $server, $user, $password);
       }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBAGGREGATE);
        throw $o2e;
        }
    return $res_val;

    }


/**
 * Returns a set of $recs records from a db2 table for a passed $where clause.
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
 * @return array
 */
function o2_as400_recordset($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $select_str,
                            $where,
                            $order_by,
                            $recs) {

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_as400_o.$database.o2_as400_c.".".o2_as400_o.$table.o2_as400_c;
        }
    $query_local = "SELECT ".$select_str.
                   " FROM ".$table." AS ".o2_as400_o.$table_alias.o2_as400_c;
    if ($where) {
        $query_local.= " WHERE ".$where;
        }
    if ($order_by) {
        $query_local.= " ORDER BY ".$order_by."\n";
        }
    $query_local.= " FETCH FIRST ".abs($recs)." ROWS ONLY";
    try {
        $res_local = o2_as400_execute($query_local, $server, $user, $password);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBDATAQUERY);
        throw $o2e;
        }
    return $res_local;

    }


/**
 * Alter DB2 table to create a foreign key.
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
function o2_as400_fkeyadd($server,
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
 * Alter DB2 table to remove a foreign key.
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
function o2_as400_fkeyremove($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $key_name) {

    return false;

    }


/**
 * Validate a foreign key in DB2 table against existing data.
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
function o2_as400_fkeyvalidate($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

    return false;

    }


/**
 * Returns the list of existing foreign keys for DB2 table
 *
 *  === NOT IMPLEMENTED YET ===
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return array
 */
function o2_as400_fkeystablist($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table) {

    return array();

    }


/**
 * Commit open transaction on db2 database
 *
 * @param string $server
 * @param string $user
 * @param string $password
 */
function o2_as400_commit($server, $user, $password) {

    if (isset($GLOBALS['o2_as400_conn'][$server])) {
/*      if (!@odbc_exec($GLOBALS['o2_as400_conn'][$server], "COMMIT TRANSACTION;")) {
            throw new o2_exception("<b>AS400</b> server: <i>".$server.
                                   "</i><br>user: <i>".$user."</i><hr>".
                                   odbc_errormsg($GLOBALS['o2_as400_conn'][$server]),
                                   o2error_DBCOMMIT);
            } */
        unset($GLOBALS['o2_as400_conn'][$server]);
        }

    }

?>
