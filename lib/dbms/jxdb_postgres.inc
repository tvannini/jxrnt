<?php

/**
 * Janox POSTGRES Database Gateway
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
 * This script contains Janox data functions set for POSTGRES database
 *
 * @name      jxpostgres
 * @package   janox/dbms/jxdb_postgres.php
 * @version   2.6
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


if (!extension_loaded("pdo_pgsql")) {
    throw new o2_exception("<b>PostgreSQL PDO</b> driver not loaded:<br>".
                           "check your PHP configuration for <i>pdo_pgsql</i> extension.",
                           o2error_DBCONNECT);
    }

define("o2_postgres_o", '"');
define("o2_postgres_c", '"');
$o2_postgres_conn  = array();
$o2_postgres_trans = array();
$o2_postgres_stms  = array();
$o2_postgres_error = array();


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_postgres_normalize($string, $untrim = false) {

    return strtr(($untrim ? $string : trim($string)), array("'" => "''"));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_postgres_concat($strings) {

    return 'CONCAT('.implode(',', $strings).')';

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as "db"."schema"."tab_name"
 * If $name is omitted a fully qualified prefix will be returned, in the form
 * "db"."schema"
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $string
 * @return string
 */
function o2_postgres_qualify($database, $owner, $name = "") {

    return o2_postgres_o.$database.o2_postgres_c.".".
           o2_postgres_o.$owner.o2_postgres_c.
           ($name ? ".".o2_postgres_o.$name.o2_postgres_c : "");

    }


/**
 * Return a postgres connection handle
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  boolean $trans
 * @return PDO
 */
function o2_postgres_connect($server, $user, $password, $trans) {

    $app = $_SESSION['o2_app'];
    // ____________________________________ Isolated transaction: start new connection ___
    if ($app->isolated_trans) {
        $key  = $server.$user."jxext";
        $attr = array();
        }
    // _____________________________ In current transaction: use persistent connection ___
    else {
        $key  = $server.$user;
        $attr = array(PDO::ATTR_PERSISTENT => true);
        }
    // _________________________________________________ Check for existing connection ___
    if (!isset($GLOBALS['o2_postgres_conn'][$key])) {
        // __________________________________________ Create new persistent connection ___
        if (!($conn_local = new PDO("pgsql:".$server, $user, $password, $attr))) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>Postgres</b> server: <i>".$server."</i><hr>".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        // _______________________________________________________ Connection encoding ___
        if ($app->chr_encoding) {
            switch (strtolower(str_replace(array("-", "_"), "", $app->chr_encoding))) {
                case "windows1252":
                case "cp1252":
                    $conn_local->exec("SET NAMES 'WIN1252'");
                    break;
                case "utf8":
                    $conn_local->exec("SET NAMES 'UTF8'");
                    break;
                }
            }
        // ______________________________________________ Create transaction if needed ___
        if ($trans) {
            if (!$conn_local->beginTransaction()) {
                $err = $conn_local->errorInfo();
                throw new o2_exception("<b>Postgres</b> server: <i>".$server."</i><hr>".
                                       $err[2],
                                       o2error_DBCONNECT);
                }
            else {
                $GLOBALS['o2_postgres_trans'][$key] = true;
                }
            }
        $GLOBALS['o2_postgres_conn'][$key] = $conn_local;
        // _________________________________________ Try to set column names UPPERCASE ___
        $conn_local->setAttribute(PDO::ATTR_CASE, 1);
        }
    // _________________________________________________________ Check for transaction ___
    elseif ($trans && !isset($GLOBALS['o2_postgres_trans'][$key])) {
        $conn_local = $GLOBALS['o2_postgres_conn'][$key];
        // _________________________________ Create transaction on existing connection ___
        if (!$conn_local->beginTransaction()) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>Postgres</b> server: <i>".$server."</i><hr>".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        else {
            $GLOBALS['o2_postgres_trans'][$key] = true;
            }
        }
    return $GLOBALS['o2_postgres_conn'][$key];

    }


/**
 * Execute a query on the given PostgreSQL server.
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
 * @param  integer $limit      Number of records to fetch
 * @param  boolean $stm        Statement to fetch on
 * @return mix
 */
function o2_postgres_execute($query,
                             $server,
                             $user,
                             $password,
                             $only_exe = false,
                             $trans    = true,
                             $limit    = 0,
                             &$stm     = false) {

    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace && !$stm) {
        if ($stm === 0) {
            o2log('(Start fetching) '.$query);
            }
        else {
            o2log($query);
            }
        }
    // _____________________________________________ Execution only, no dataset return ___
    if ($only_exe) {
        // __________________________________________________ Get connection to server ___
        $conn = o2_postgres_connect($server, $user, $password, $trans);
        $res  = $conn->exec($query);
        // ______________________________________________________ On execution failure ___
        if ($res === false) {
            $err = $conn->errorInfo();
            $conn->rollBack && @$conn->rollBack();
            throw new o2_exception("<b>PostgreSQL</b> server: <i>".$server.
                                   "</i><br>query: <code>".$query."</code><hr>".
                                   $err[2],
                                   o2error_DBEXECUTE);
            return false;
            }
        return $res;
        }
    // ________________________________________________________ Queries returning data ___
    else {
        $loop = ($stm || ($stm === 0));
        if (!$stm) {
            // ______________________________________________ Get connection to server ___
            $conn = o2_postgres_connect($server, $user, $password, $trans);
            // __________________________ Create statement and check execution failure ___
            if (!($stm = $conn->query($query))) {
                $err = $conn->errorInfo();
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception("<b>PostgreSQL</b> server: <i>".$server.
                                       "</i><br>query: <code>".$query."</code><hr>".
                                       $err[2],
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
        // ____________________________________________________________ Return dataset ___
        if ($loop) {
            $set = array();
            $num = 0;
            while ((!$limit || $num < $limit) && ($row = $stm->fetch(PDO::FETCH_ASSOC))) {
                $num++;
                $set[] = $row;
                }
            if ($limit && $num < $limit) {
                $stm->closeCursor();
                $stm = null;
                }
            return $set;
            }
        else {
            $data  = $stm->fetchAll(PDO::FETCH_ASSOC);
            $err_c = $stm->errorCode();
            // _____________________________________ Error on statement (during fetch) ___
            if ($err_c != '00000') {
                $err = $stm->errorInfo();
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception("<b>PostgreSQL</b> server: <i>".$server.
                                       "</i><br>query: <code>".$query."</code><hr>".
                                       $err[2],
                                       o2error_DBDATAQUERY);
                }
            $stm->closeCursor();
            $stm = null;
            return $data;
            }
        }

    }


/**
 * Retrieves user tables list from a postgres database
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_postgres_tables($server, $user, $password, $database, $owner) {

    $query = "SELECT table_name FROM information_schema.tables WHERE ".
             "(table_type = 'BASE TABLE' OR table_type = 'VIEW') AND table_schema = '".
             $owner."'";
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLES);
        throw $o2e;
        }
    $tables = array();
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
function o2_postgres_tabexists($server, $user, $password, $database, $owner, $table) {

    $query = "SELECT 1 AS IT_EXISTS FROM information_schema.tables WHERE ".
             "(table_type = 'BASE TABLE' OR table_type = 'VIEW') AND table_schema = '".
             $owner."' AND table_name='".$table."'";
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABEXISTS);
        throw $o2e;
        }
    return ($res[0]["IT_EXISTS"] ? true : false);

    }


/**
 * Return an array containing informations about fileds of a postgres table:
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
function o2_postgres_tablefields($server, $user, $password, $database, $owner, $table) {

    $query = "SELECT * FROM information_schema.columns WHERE table_name='".
             $table."' AND TABLE_SCHEMA='".$owner."' ORDER BY ORDINAL_POSITION";
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    $list = array();
    foreach ($res as $field) {
        $default = explode("::", $field['COLUMN_DEFAULT'], 2);
        $list[]  = array('field'   => $field['COLUMN_NAME'],
                         'type'    => $field['DATA_TYPE'].
                                      "(".$field['CHARACTER_MAXIMUM_LENGTH'].")",
                         'default' => str_replace("(", "",
                                      str_replace(")", "",
                                      str_replace("'", "", $default[0]))));
        }
    return $list;

    }


/**
 * Return an array containing informations about indexes of a postgres table:
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
function o2_postgres_tableindexes($server, $user, $password, $database, $owner, $table) {

    $ret_val     = array();
    $query_local = "SELECT indexname, indexdef FROM pg_indexes WHERE schemaname='".$owner.
                   "' AND tablename='".$table."'";
    try {
        $res_local = o2_postgres_execute($query_local,
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
    foreach ($res_local as $single_key) {
        if ($single_key['INDEXNAME'] != $table."_pkey") {
            $index    = substr($single_key['INDEXNAME'], strlen($table) + 1);
            $segments = array();
            $unique   = stripos($single_key['INDEXDEF'], 'CREATE UNIQUE INDEX') !== false;
            preg_match('/\(([^(]*)\)/', $single_key['INDEXDEF'], $segments);
            $segments        = explode(",", $segments[1]);
            $ret_val[$index] = array('unique' => $unique, 'segments' => array());
            $seq             = 0;
            foreach ($segments as $seg) {
                list($seg, $dir) = explode(" ", trim($seg));
                if (trim($dir) == "DESC") {
                    $desc = true;
                    }
                else {
                    $desc = false;
                    }
                $seg = rtrim(ltrim($seg, o2_postgres_o), o2_postgres_c);
                $ret_val[$index]['segments']+= array($seq =>
                                                     array('column' => $seg,
                                                           'dir'    => ($desc ?
                                                                        "D" : "A")));
                $seq++;
                }
            }
        }
    return $ret_val;

    }


/**
 * Insert into table A data read from table B and return TRUE on success.
 * Matching fields are passed by array $values in the form:
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
function o2_postgres_insertfrom($server,
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
    $query = "INSERT INTO ".o2_postgres_o.$database_to.o2_postgres_c.".".
                            o2_postgres_o.$owner_to.o2_postgres_c.".".
                            o2_postgres_o.$table_to.o2_postgres_c.
             " (".$list_to.") SELECT ".$list_from.
             " FROM ".o2_postgres_o.$database_from.o2_postgres_c.".".
                      o2_postgres_o.$owner_from.o2_postgres_c.".".
                      o2_postgres_o.$table_from.o2_postgres_c;
    if ($where) {
       $query.= " WHERE ".$where;
       }
    try {
        $aff_rows = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLECOPY);
        throw $o2e;
        return false;
        }
    return $aff_rows;

    }


/**
 * Phisically removes a postgres table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_postgres_droptable($server, $user, $password, $database, $owner, $table) {

    $query = "DROP TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                           o2_postgres_o.$owner.o2_postgres_c.".".
                           o2_postgres_o.$table.o2_postgres_c."\n";
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEDROP);
        throw $o2e;
        }
    o2_postgres_commit($server, $user, $password, $database);
    return true;

    }


/**
 * Phisically renames a postgres table
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
function o2_postgres_renametable($server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table,
                                 $new_name) {

    try {
        // ______________________________________________________ Get existing indexes ___
        $query    = "SELECT indexname, indexdef FROM pg_indexes WHERE schemaname='".
                    $owner."' AND tablename='".$table."'";
        $idx_list = o2_postgres_execute($query,
                                        $server,
                                        $user,
                                        $password,
                                        false,
                                        false);
        // ______________________________________________________________ Rename table ___
        $query = "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                                o2_postgres_o.$owner.o2_postgres_c.".".
                                o2_postgres_o.$table.o2_postgres_c.
                 " RENAME TO ".o2_postgres_o.$new_name.o2_postgres_c."\n";
        o2_postgres_execute($query, $server, $user, $password, true, true);
        // ____________________________________________________________ Rename indexes ___
        foreach ($idx_list as $idx) {
            $new_idx = substr($idx['INDEXNAME'], strlen($table) + 1);
            $query   = "ALTER INDEX ".o2_postgres_o.$database.o2_postgres_c.".".
                                      o2_postgres_o.$owner.o2_postgres_c.".".
                                      o2_postgres_o.$idx['INDEXNAME'].o2_postgres_c.
                       " RENAME TO ".o2_postgres_o.$new_name."_".$new_idx.o2_postgres_c.
                       "\n";
            o2_postgres_execute($query, $server, $user, $password, true, true);
            }
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEREBUILD);
        throw $o2e;
        }
    o2_postgres_commit($server, $user, $password, $database);
    return true;

    }


/**
 * If $execute is passed TRUE function phisically creates a postgres table, else it
 * returns the postgres creation script for the table.
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
 *       "indexes" => array([idx_1]    => array([field_1] => A|D,
 *                                              [field_2] => A|D,
 *                                              [...]
 *                                              [field_n] => A|D),
 *                          [idx_2]    => array([field_1] => A|D,
 *                                              [field_2] => A|D,
 *                                              [...]
 *                                              [field_n] => A|D),
 *                          [...]
 *                          [idx_n]    => array([field_1] => A|D,
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
function o2_postgres_createtable($server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table,
                                 $structure,
                                 $execute = true) {

    $query   = "CREATE TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                               o2_postgres_o.$owner.o2_postgres_c.".".
                               o2_postgres_o.$table.o2_postgres_c." (\n";
    $type    = "";
    $default = "";
    foreach ($structure["fields"] as $field_name => $field_prop) {
        $query.= o2_postgres_field_create($field_name,
                                          $field_prop['type'],
                                          $field_prop['size'],
                                          $field_prop['int'],
                                          $field_prop['dec']).",\n";
        }
    $query  = substr($query, 0, -2).")\n";
    $script = array();
    // ___________________________________________________________________ PRIMARY KEY ___
    foreach ($structure["primary"] as $pk_name => $pk_segments) {
        break;
        }
    $script["P"] = "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                                  o2_postgres_o.$owner.o2_postgres_c.".".
                                  o2_postgres_o.$table.o2_postgres_c.
                   " ADD PRIMARY KEY (".
                   o2_postgres_indexfields_create($pk_segments, false).")";
    // ________________________________________________________________ UNIQUE INDEXES ___
    foreach ($structure["keys"] as $key_name => $key_segs) {
        $script[$key_name] = o2_postgres_index_create($key_name,
                                                      $key_segs,
                                                      $table,
                                                      $database,
                                                      $owner,
                                                      true);
        }
    // ____________________________________________________________ NOT UNIQUE INDEXES ___
    foreach ($structure["indexes"] as $idx_name => $idx_segs) {
        $script[$idx_name] = o2_postgres_index_create($idx_name,
                                                      $idx_segs,
                                                      $table,
                                                      $database,
                                                      $owner,
                                                      false);
        }
    if ($execute) {
        try {
            o2_postgres_execute($query, $server, $user, $password, true, true);
            foreach ($script as $key_create) {
                o2_postgres_execute($key_create, $server, $user, $password, true, true);
                }
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        o2_postgres_commit($server, $user, $password, $database);
        return true;
        }
    else {
        return $query.";\n".implode(";", $script);
        }

    }

/**
 * Returns Postgres definition for single field by Janox-type
 *
 * @param  string  $name         Field name
 * @param  string  $type         Field type (A|N|L|D|T|S)
 * @param  integer $size         Total size for alpha
 * @param  integer $int          Total size or integer part
 * @param  integer $dec          Number of decimals digits (only numbers)
 * @param  boolean $for_change   If script is requested for ALTER COLUMN syntax
 * @return string                SQL code to create field
 */
function o2_postgres_field_create($name, $type, $size, $int, $dec, $for_change = false) {

    switch ($type) {
        case "A":
            if ($size <= 255) {
                $fld_type = "varchar(".$size.")";
                }
            else {
                $fld_type = "text";
                }
            $fld_default = "''";
            break;
        case "N":
            if ($dec) {
                $fld_type = "numeric(".($int + $dec).", ".$dec.")";
                }
            elseif ($int < 5) {
                $fld_type = "SMALLINT";
                }
            elseif ($int < 10) {
                $fld_type = "INTEGER";
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
            $fld_default = "''";
            break;
        }
    if ($for_change) {
        return $name." TYPE ".$fld_type;
        }
    else {
        return $name." ".$fld_type." NOT NULL DEFAULT ".$fld_default;
        }

    }


/**
 * Returns Postgres definition for single index by Janox-type.
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
function o2_postgres_index_create($key_name,
                                  $key_segs,
                                  $table,
                                  $database,
                                  $owner,
                                  $unique = true) {

    return 'CREATE'.($unique ? ' UNIQUE' : '').
           ' INDEX '.o2_postgres_o.$table.'_'.$key_name.o2_postgres_c.
              ' ON '.o2_postgres_o.$database.o2_postgres_c.'.'.
                     o2_postgres_o.$owner.o2_postgres_c.'.'.
                     o2_postgres_o.$table.o2_postgres_c.
                ' ('.o2_postgres_indexfields_create($key_segs).')';

    }


/**
 * Return ORDER_BY clausole for all the fields of an index in a postgres context.
 * List has to be comma separated and rightly qualified.
 *
 * $fields array parameter is passed in the form:
 *    $fields[field_name] = direction [D/A]
 *
 * @param  array   $fields
 * @param  boolean $with_dir
 * @return string
 */
function o2_postgres_indexfields_create($fields, $with_dir = true) {

    $str = "";
    foreach ($fields as $field => $direction) {
        $str.= ($str ? "," : "").$field.
               ($with_dir ? ($direction == "D" ?
                             " DESC NULLS LAST" :
                             " ASC NULLS FIRST") : "");
        }
    return $str;

    }


/**
 * Alter a Postgres table to add a column
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
function o2_postgres_field_add($server,
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

    $query = "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                            o2_postgres_o.$owner.o2_postgres_c.".".
                            o2_postgres_o.$table.o2_postgres_c.
             " ADD COLUMN ".o2_postgres_field_create(o2_postgres_o.$field_name.
                                                     o2_postgres_c,
                                                     $field_type,
                                                     $field_size,
                                                     $field_int,
                                                     $field_dec);
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Postgres table to remove a column
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
function o2_postgres_field_remove($server,
                                  $user,
                                  $password,
                                  $database,
                                  $owner,
                                  $table,
                                  $field_name) {

    $query =  "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                             o2_postgres_o.$owner.o2_postgres_c.".".
                             o2_postgres_o.$table.o2_postgres_c.
             " DROP COLUMN ".o2_postgres_o.$field_name.o2_postgres_c;
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Postgres table to change a column definition
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
function o2_postgres_field_change($server,
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

    $query =   "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                              o2_postgres_o.$owner.o2_postgres_c.".".
                              o2_postgres_o.$table.o2_postgres_c.
             " ALTER COLUMN ".o2_postgres_field_create(o2_postgres_o.$field_name.
                                                       o2_postgres_c,
                                                       $field_type,
                                                       $field_size,
                                                       $field_int,
                                                       $field_dec,
                                                       true);
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Postgres table to rename a column
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
function o2_postgres_field_rename($server,
                                  $user,
                                  $password,
                                  $database,
                                  $owner,
                                  $table,
                                  $old_name,
                                  $new_name) {

    $query =    "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                               o2_postgres_o.$owner.o2_postgres_c.".".
                               o2_postgres_o.$table.o2_postgres_c.
             " RENAME COLUMN ".o2_postgres_o.$old_name.o2_postgres_c.
                        " TO ".o2_postgres_o.$new_name.o2_postgres_c;
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Postgres table to add an index
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
function o2_postgres_index_add($server,
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
            $quoted_segs[o2_postgres_o.$field_name.o2_postgres_c] = $direction;
            }
        else {
            $quoted_segs[$field_name] = $direction;
            }
        }
    $query = o2_postgres_index_create($index_name,
                                      $quoted_segs,
                                      $table,
                                      $database,
                                      $owner,
                                      $unique);
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a Postgres table to remove an existing index
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
function o2_postgres_index_remove($server,
                                  $user,
                                  $password,
                                  $database,
                                  $owner,
                                  $table,
                                  $index_name) {

    $query = "DROP INDEX ".o2_postgres_o.$database.o2_postgres_c.".".
                           o2_postgres_o.$owner.o2_postgres_c.".".
                           o2_postgres_o.$table.'_'.$index_name.o2_postgres_c;
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Return an array of calculated postgres aggragate functions for the table or view.
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
function o2_postgres_aggregate($server,
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
                           "AS ".o2_postgres_o.$func_name.o2_postgres_c;
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_postgres_o.$database.o2_postgres_c.".".
                 o2_postgres_o.$owner.o2_postgres_c.".".
                 o2_postgres_o.$table.o2_postgres_c;
        }
    $query = "SELECT ".$functions_list.
             " FROM ".$table." AS ".o2_postgres_o.$table_alias.o2_postgres_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
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
function o2_postgres_verifyrec($server,
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
        $table = o2_postgres_o.$database.o2_postgres_c.".".
                 o2_postgres_o.$owner.o2_postgres_c.".".
                 o2_postgres_o.$table.o2_postgres_c;
        }
    $query = "SELECT ".$select_str." FROM ".$table.
             " AS ".o2_postgres_o.$table_alias.o2_postgres_c;
    if ($where) {
        $query.= " WHERE ".$where;
        }
    if ($order_by) {
        $query.= " ORDER BY ".$order_by;
        }
    $query.= " LIMIT 1";
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
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
 * Modifies postgres record, uniquely identified by $where clause, with values in $sets.
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
function o2_postgres_modifyrec($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $table_alias,
                               $sets,
                               $where) {

    $upd_str = "";
    foreach ($sets as $field => $value) {
        $upd_str.= ($upd_str ? "," : "").$field." = ".$value;
        }
    $query = "UPDATE ".o2_postgres_o.$database.o2_postgres_c.".".
                       o2_postgres_o.$owner.o2_postgres_c.".".
                       o2_postgres_o.$table.o2_postgres_c.
             " AS ".o2_postgres_o.$table_alias.o2_postgres_c.
             " SET ".$upd_str." WHERE ".$where;
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECUPDATE);
        throw $o2e;
        }
    return true;

    }


/**
 * Insert passed record fields in a postgres table.
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
function o2_postgres_insertrec($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $table_alias,
                               $fields,
                               $values) {

    $query = "INSERT INTO ".o2_postgres_o.$database.o2_postgres_c.".".
                            o2_postgres_o.$owner.o2_postgres_c.".".
                            o2_postgres_o.$table.o2_postgres_c.
             " (".implode(",", $fields).") VALUES (".implode(",", $values).")";
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECINSERT);
        throw $o2e;
        }
    return true;

    }


/**
 * Delete record of a postgres table for the passed $where clause.
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
function o2_postgres_deleterec($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $table_alias,
                               $where) {

    $query = "DELETE FROM ".o2_postgres_o.$database.o2_postgres_c.".".
                            o2_postgres_o.$owner.o2_postgres_c.".".
                            o2_postgres_o.$table.o2_postgres_c;
    if ($where) {
       $query.= " AS ".o2_postgres_o.$table_alias.o2_postgres_c." WHERE ".$where;
       }
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
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
function o2_postgres_count($server,
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
        $table = o2_postgres_o.$database.o2_postgres_c.".".
                 o2_postgres_o.$owner.o2_postgres_c.".".
                 o2_postgres_o.$table.o2_postgres_c;
        }
    $query = "SELECT COUNT(*) AS COMPUTED FROM ".$table.
             " AS ".o2_postgres_o.$table_alias.o2_postgres_c;
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
    $conn = o2_postgres_connect($server, $user, $password, false);
    // __________________________________________________________ On execution failure ___
    if (!($res = $conn->query($query))) {
        $err = $conn->errorInfo();
        $rb  = $conn->rollBack;
        $rb && @$rb();
        throw new o2_exception("<b>PostgreSQL</b> server: <i>".$server.
                               "</i><br>Error counting dataset.<hr><code>".
                               $query."</code><br><br>".$err[2],
                               o2error_DBCOUNT);
        return false;
        }
    $dset = $res->fetchAll(PDO::FETCH_ASSOC);
    // _____________________________________ Return integer value stored in "COMPUTED" ___
    return intval($dset[0]['COMPUTED']);

    }


/**
 * Returns a set of $recs records from a postgres table for a passed $where clause.
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
function o2_postgres_recordset($server,
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

    // ____________________________________________ Statement recall in case of paging ___
    if ($page_where) {
        if ($stm_id && isset($GLOBALS['o2_postgres_stms'][$stm_id])) {
            return o2_postgres_execute(false,
                                       false,
                                       false,
                                       false,
                                       false,
                                       false,
                                       $recs,
                                       $GLOBALS['o2_postgres_stms'][$stm_id]);
            }
        $where = $page_where.$where;
        }
    elseif ($stm_id && isset($GLOBALS['o2_postgres_stms'][$stm_id])) {
        if ($GLOBALS['o2_postgres_stms'][$stm_id]) {
            $GLOBALS['o2_postgres_stms'][$stm_id]->closeCursor();
            }
        $GLOBALS['o2_postgres_stms'][$stm_id] = null;
        unset($GLOBALS['o2_postgres_stms'][$stm_id]);
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != '(') {
        $table = o2_postgres_o.$database.o2_postgres_c.'.'.
                 o2_postgres_o.$owner.o2_postgres_c.'.'.
                 o2_postgres_o.$table.o2_postgres_c;
        }
    $query = 'SELECT '.$select_str.' FROM '.$table.
             ' AS '.o2_postgres_o.$table_alias.o2_postgres_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= ' LEFT JOIN '.$linktab.' ON '.$linkon;
            }
        }
    if ($where) {
        $query.= ' WHERE '.$where;
        }
    if ($order_by &&
        (!isset($GLOBALS['jxviewsql']) ||
         (strpos($GLOBALS['jxviewsql'], 'O') !== false))) {
        $query.= ' ORDER BY '.$order_by;
        }
    if ($stm_id) {
        $stm = 0;
        }
    else {
        $stm   = false;
        if ((strtoupper(substr($select_str, 0, 8)) != 'DISTINCT') &&
            (!isset($GLOBALS['jxviewsql']) ||
             (strpos($GLOBALS['jxviewsql'], 'L') !== false))) {
            $query.= ' LIMIT '.$recs;
            }
        }
    // _________________________________________ Return SQL query instead of executing ___
    if (isset($GLOBALS['jxviewsql'])) {
        $GLOBALS['jxviewsql'] = $query;
        return array();
        }
    if ($lock) {
        $query.= ' FOR UPDATE OF '.o2_postgres_o.$table_alias.o2_postgres_c;
        }
    try {
        $res = o2_postgres_execute($query,
                                   $server,
                                   $user,
                                   $password,
                                   false,
                                   $lock,
                                   $recs,
                                   $stm);
        if ($stm_id) {
            if ($stm) {
                $GLOBALS['o2_postgres_stms'][$stm_id] = $stm;
                }
            elseif (isset($GLOBALS['o2_postgres_stms'][$stm_id])) {
                $GLOBALS['o2_postgres_stms'][$stm_id] = null;
                unset($GLOBALS['o2_postgres_stms'][$stm_id]);
                }
            }
        unset($stm);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBDATAQUERY);
        throw $o2e;
        }
    return $res;

    }


/**
 * Alter Postgres table to create a foreign key.
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
function o2_postgres_fkeyadd($server,
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
            $main_list[] = o2_postgres_o.$field.o2_postgres_c;
            }
        else {
            $main_list[] = $field;
            }
        }
    foreach ($ref_fields as $field) {
        if ($field != "O2ASPID") {
            $ref_list[] = o2_postgres_o.$field.o2_postgres_c;
            }
        else {
            $ref_list[] = $field;
            }
        }
    $main_list = implode(",", $main_list);
    $ref_list  = implode(",", $ref_list);
    $query     = "ALTER TABLE ".o2_postgres_o.$main_db.o2_postgres_c.".".
                                o2_postgres_o.$main_owner.o2_postgres_c.'.'.
                                o2_postgres_o.$main_table.o2_postgres_c.
                 " ADD CONSTRAINT ".o2_postgres_o.$key_name.o2_postgres_c.
                 " FOREIGN KEY (".$main_list.
                 ") REFERENCES ".o2_postgres_o.$ref_db.o2_postgres_c.".".
                                 o2_postgres_o.$ref_owner.o2_postgres_c.'.'.
                                 o2_postgres_o.$ref_table.o2_postgres_c.
                 " (".$ref_list.") MATCH FULL NOT VALID";
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Alter Postgres table to remove a foreign key.
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
function o2_postgres_fkeyremove($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $key_name) {

    $query = "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                            o2_postgres_o.$owner.o2_postgres_c.".".
                            o2_postgres_o.$table.o2_postgres_c.
             " DROP CONSTRAINT ".o2_postgres_o.$key_name.o2_postgres_c;
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Validate a foreign key in Postgres table against existing data.
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
function o2_postgres_fkeyvalidate($server,
                                  $user,
                                  $password,
                                  $database,
                                  $owner,
                                  $table,
                                  $key_name) {

    $query = "ALTER TABLE ".o2_postgres_o.$database.o2_postgres_c.".".
                            o2_postgres_o.$owner.o2_postgres_c.".".
                            o2_postgres_o.$table.o2_postgres_c.
             " VALIDATE CONSTRAINT ".o2_postgres_o.$key_name.o2_postgres_c;
    try {
        o2_postgres_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Returns the list of existing foreign keys for Postgres table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_postgres_fkeystablist($server,
                                  $user,
                                  $password,
                                  $database,
                                  $owner,
                                  $table) {

    $query = "SELECT conname ".o2_postgres_o."CN".o2_postgres_c.
             " FROM pg_catalog.pg_constraint WHERE conrelid='".
             o2_postgres_o.$database.o2_postgres_c.'.'.
             o2_postgres_o.$owner.o2_postgres_c.'.'.
             o2_postgres_o.$table.o2_postgres_c."'::regclass AND contype='f'";

    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
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
 * Returns the list of existing foreign keys for Postgres db-schema and some informations
 * about constraints
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_postgres_fkeysinfo($server,
                               $user,
                               $password,
                               $database,
                               $owner) {

    $query = "SELECT x.conname ".o2_postgres_o."CN".o2_postgres_c.",".
             " x.conkey ".o2_postgres_o."SK".o2_postgres_c.",".
             " x.confkey ".o2_postgres_o."TK".o2_postgres_c.",".
             " pk.relname ".o2_postgres_o."ON".o2_postgres_c.",".
             " fk.relname ".o2_postgres_o."TO".o2_postgres_c." ".
             "FROM pg_catalog.pg_constraint x ".
             "INNER JOIN pg_catalog.pg_class pk ON x.conrelid!=0 AND x.conrelid=pk.oid ".
             "INNER JOIN pg_catalog.pg_class fk ON x.confrelid!=0 AND x.confrelid=fk.oid".
             " INNER JOIN information_schema.table_constraints tc ON ".
                         "x.conname=tc.constraint_name ".
             "WHERE x.contype='f' AND ".
             "tc.constraint_catalog='".$database."' AND ".
             "tc.constraint_schema='".$owner."'";
    try {
        $res = o2_postgres_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    $list = array();
    foreach ($res as $c) {
        $list[$c["CN"]] = array("ON" => $c["ON"],
                                "TO" => $c["TO"],
                                "SK" => explode(',', substr($c["SK"], 1, -1)),
                                "TK" => explode(',', substr($c["TK"], 1, -1)));
        }
    return $list;

    }


/**
 * Commit open transaction on Postgres database
 *
 * @param string  $server
 * @param string  $user
 * @param string  $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_postgres_commit($server, $user, $password, $end = false) {

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
    if ($GLOBALS['o2_postgres_trans'][$key]) {
        // ___________________________________ Check for errors on current transaction ___
        if ($GLOBALS['o2_postgres_error'][$key]) {
            $GLOBALS['o2_postgres_conn'][$key]->rollBack();
            unset($GLOBALS['o2_postgres_error'][$key]);
            }
        // ________________________________________________ Commit current transaction ___
        else {
            // ___________________________________ Close open statements before commit ___
            if ($end) {
                if (is_array($GLOBALS['o2_postgres_stms'])) {
                    foreach ($GLOBALS['o2_postgres_stms'] as $id => $stm) {
                        if ($GLOBALS['o2_postgres_stms'][$id]) {
                            $GLOBALS['o2_postgres_stms'][$id]->closeCursor();
                            }
                        $GLOBALS['o2_postgres_stms'][$id] = null;
                        $stm                              = null;
                        }
                    }
                unset($GLOBALS['o2_postgres_stms']);
                }
            if (!$GLOBALS['o2_postgres_conn'][$key]->commit()) {
                $err = $GLOBALS['o2_postgres_conn'][$key]->errorInfo();
                throw new o2_exception("<b>Postgres</b> server: <i>".$server."</i><hr>".
                                       $err[2],
                                       o2error_DBCOMMIT);
                }
            }
        unset($GLOBALS['o2_postgres_trans'][$key]);
        }
    // _________________________________________________ Check for existing connection ___
    if ($end && $GLOBALS['o2_postgres_conn'][$key]) {
        unset($GLOBALS['o2_postgres_conn'][$key]);
        }

    }

?>
