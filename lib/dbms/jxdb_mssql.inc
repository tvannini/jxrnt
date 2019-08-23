<?php

/**
 * Janox MSSQL Database Gateway
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
 * This script contains Janox data functions set for MSSQL database via SQLSRV PDO driver.
 * SQLSRV is available on Windows machines only!
 *
 * @name      jxmssql
 * @package   janox/dbms/jxdb_mssql.php
 * @version   2.6
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


if (extension_loaded("pdo_sqlsrv")) {
    $GLOBALS['dsn'] = "sqlsrv:server=";
    }
elseif (extension_loaded("pdo_dblib")) {
    $GLOBALS['dsn'] = "mssql:host=";
    }
else {
    throw new o2_exception("<b>MSSQL (neither SQLSRV nor FreeTDS) PDO</b> driver not ".
                           "loaded:<br />check your PHP configuration for ".
                           "<i>php_pdo_sqlsrv_*</i> or <i>php_pdo_dblib</i> extensions.",
                           o2error_DBCONNECT);
    }

define("o2_mssql_o", "[");
define("o2_mssql_c", "]");
$o2_mssql_conn  = array();
$o2_mssql_trans = array();
$o2_mssql_stms  = array();
$o2_mssql_error = array();


/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_mssql_normalize($string, $untrim = false) {

    return strtr(($untrim ? $string : trim($string)), array("'" => "''"));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_mssql_concat($strings) {

    return 'CONCAT('.implode(',', $strings).')';

    }


/**
 * Returns a database name fully qualified.
 * For example $tab_name is returned as [db].[schema].[tab_name]
 * If $name is omitted a fully qualified prefix will be returned, in the form
 * [db].[schema]
 *
 * @param  string $database
 * @param  string $owner
 * @param  string $string
 * @return string
 */
function o2_mssql_qualify($database, $owner, $name = "") {

    return o2_mssql_o.$database.o2_mssql_c.".".
           o2_mssql_o.$owner.o2_mssql_c.
           ($name ? ".".o2_mssql_o.$name.o2_mssql_c : "");

    }


/**
 * Return a mssql connection handle
 *
 * @param  string  $server
 * @param  string  $user
 * @param  string  $password
 * @param  boolean $trans
 * @return PDO
 */
function o2_mssql_connect($server, $user, $password, $trans) {

    $app = $_SESSION['o2_app'];
    // ___________________________________________________________ Prevent blank names ___
    if (!$server) {
        $server = "localhost";
        }
    // ___________________________________________________________ Connection encoding ___
    if ($app->chr_encoding) {
        switch (strtolower(str_replace(array("-", "_"), "", $app->chr_encoding))) {
            case "windows1252":
            case "cp1252":
                $utf8 = false;
                break;
            case "utf8":
                $utf8 = true;
                break;
            }
        }
    // ____________________________________ Isolated transaction: start new connection ___
    if ($app->isolated_trans) {
        $key  = $server.$user."jxext";
        $pers = false;
        }
    // _____________________________ In current transaction: use persistent connection ___
    else {
        $key  = $server.$user;
        $pers = true;
        }
    // _________________________________ Disable pers-conn due to crashes in sqlsrv3.0 ___
    $pers = false;
    // _____________________________________________________ Set connection attributes ___
    $attr = array(PDO::ATTR_PERSISTENT      => $pers,
                  PDO::ATTR_CASE            => PDO::CASE_UPPER,
                  PDO::SQLSRV_ATTR_ENCODING => ($utf8 ?
                                                PDO::SQLSRV_ENCODING_UTF8 :
                                                PDO::SQLSRV_ENCODING_SYSTEM),
				  PDO::ATTR_ERRMODE         => PDO::ERRMODE_EXCEPTION);
    // _________________________________________________ Check for existing connection ___
    if (!isset($GLOBALS['o2_mssql_conn'][$key])) {
        // _____________________________________________________ Create new connection ___
        if (!($conn_local = new PDO($GLOBALS['dsn'].$server,
                                    $user,
                                    $password,
                                    $attr))) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>MSSQL</b> server: <i>".$server."</i><hr />".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        // ______________________________________________ Create transaction if needed ___
        if ($trans) {
            if (!$conn_local->beginTransaction()) {
                $err = $conn_local->errorInfo();
                throw new o2_exception("<b>MSSQL</b> server: <i>".$server."</i><hr />".
                                       $err[2],
                                       o2error_DBCONNECT);
                }
            else {
                $GLOBALS['o2_mssql_trans'][$key] = true;
                }
            }
        // _________________________________________________ Force roll-back or errors ___
        $conn_local->exec("SET XACT_ABORT ON");
        // ___________________________________________________________ Save connection ___
        $GLOBALS['o2_mssql_conn'][$key] = $conn_local;
        }
    // _________________________________________________________ Check for transaction ___
    elseif ($trans && !isset($GLOBALS['o2_mssql_trans'][$key])) {
        $conn_local = $GLOBALS['o2_mssql_conn'][$key];
        // _________________________________ Create transaction on existing connection ___
        if (!$conn_local->beginTransaction()) {
            $err = $conn_local->errorInfo();
            throw new o2_exception("<b>MSSQL</b> server: <i>".$server."</i><hr />".
                                   $err[2],
                                   o2error_DBCONNECT);
            }
        else {
            $GLOBALS['o2_mssql_trans'][$key] = true;
            }
        }
    return $GLOBALS['o2_mssql_conn'][$key];

    }


/**
 * Execute a query on the given mssql server.
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
function o2_mssql_execute($query,
                          $server,
                          $user,
                          $password,
                          $only_exe = false,
                          $trans    = true,
                          $limit    = 0,
                          &$stm     = false) {

    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // _____________________________________________ Execution only, no dataset return ___
    if ($only_exe) {
		try {
            // ______________________________________________ Get connection to server ___
            $conn = o2_mssql_connect($server, $user, $password, $trans);
			$res = $conn->exec($query);
			}
        // ____________________________________________________ On execution exception ___
		catch (Exception $o2e) {
            $err = $conn->errorInfo();
            $conn->rollBack && @$conn->rollBack();
			$res = false;
            throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                   "</i> (as Exception)<br>query: <code>".$query.
                                   "</code><hr>".$err[2],
                                   o2error_DBEXECUTE);
            return false;
			}
        // _______________________________________ On other execution failure (NEVER!) ___
        if ($res === false) {
            $err = $conn->errorInfo();
            $conn->rollBack && @$conn->rollBack();
            throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                   "</i> (as Error)<br>query: <code>".$query.
                                   "</code><hr>".$err[2],
                                   o2error_DBEXECUTE);
            return false;
            }
        return $res;
        }
    // ________________________________________________________ Queries returning data ___
    else {
        $loop = ($stm || ($stm === 0));
        if (!$stm) {
            try {
                // __________________________________________ Get connection to server ___
                $conn = o2_mssql_connect($server, $user, $password, $trans);
                // _____________ Create statement and check execution failure (NEVER!) ___
                if (!($stm = $conn->query($query))) {
                    $err = $conn->errorInfo();
                    $conn->rollBack && @$conn->rollBack();
                    throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                           "</i> (as Error)<br>query: <code>".$query.
                                           "</code><hr>".$err[2],
                                           o2error_DBDATAQUERY);
                    return false;
                    }
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $err = $conn->errorInfo();
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                       "</i> (as Exception)<br>query: <code>".$query.
                                       "</code><hr>".$err[2],
                                       o2error_DBEXECUTE);
                return false;
                }
            }
        // ____________________________________________________________ Return dataset ___
        if ($loop) {
            $set = array();
            $num = 0;
            try {
    			// _________________________________ Needed for some stored procedures ___
                while($stm->columnCount() === 0 && $stm->nextRowset()) {
                    // _____________ Advance rowset until we get to a rowset with data ___
                    }
                while ((!$limit || $num < $limit) &&
                       ($row = $stm->fetch(PDO::FETCH_ASSOC))) {
                    $num++;
                    $set[] = $row;
                    }
                if ($limit && $num < $limit) {
                    $stm->closeCursor();
                    $stm = null;
                    }
                return $set;
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $err = $conn->errorInfo();
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                       "</i> (as Exception)<br>query: <code>".$query.
                                       "</code><hr>".$err[2],
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
        else {
            try {
                // _________________________________ Needed for some stored procedures ___
                while($stm->columnCount() === 0 && $stm->nextRowset()) {
                    // _____________ Advance rowset until we get to a rowset with data ___
                    }
                $data  = $stm->fetchAll(PDO::FETCH_ASSOC);
    			$err_c = $stm->errorCode();
                // __ Error on statement (during fetch) (NEVER: catched as exception!) ___
                if ($err_c != '00000') {
                    $err = $stm->errorInfo();
                    $conn->rollBack && @$conn->rollBack();
                    throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                           "</i> (as Error)<br>query: <code>".$query.
                                           "</code><hr>".$err[2],
                                           o2error_DBDATAQUERY);
                    }
                $stm->closeCursor();
                $stm = null;
                return $data;
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $err = $conn->errorInfo();
                $stm->closeCursor();
                $stm = null;
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                                       "</i> (as Exception)<br>query: <code>".$query.
                                       "</code><hr>".$err[2],
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
        }

    }


/**
 * Retrieves user tables list from a mssql database
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_mssql_tables($server, $user, $password, $database, $owner) {

    $query = "USE ".o2_mssql_o.$database.o2_mssql_c;
    o2_mssql_execute($query, $server, $user, $password, false, false);
    $query = "EXEC sp_tables @table_qualifier = '".$database."', ".
                                  "@table_owner = '".$owner."'";
    $arr = array();
    if ($res = o2_mssql_execute($query, $server, $user, $password, false, false)) {
        foreach ($res as $table) {
            $arr[] = $table['TABLE_NAME'];
            }
        }
    return $arr;

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
function o2_mssql_tabexists($server, $user, $password, $database, $owner, $table) {

    $query = "SELECT 1 AS IT_EXISTS FROM ".o2_mssql_o.$database.o2_mssql_c.".".
                                           o2_mssql_o.$owner.o2_mssql_c.
             ".sysobjects WHERE (type='U' OR type='V') AND name='".$table."'";
    $res   = o2_mssql_execute($query, $server, $user, $password, false, false);
    return ($res[0]["IT_EXISTS"] ? true : false);

    }


/**
 * Return an array containing informations about fileds of a mssql table:
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
function o2_mssql_tablefields($server, $user, $password, $database, $owner, $table) {

    $arr   = array();
    $query = "USE ".o2_mssql_o.$database.o2_mssql_c.";\n";
    o2_mssql_execute($query, $server, $user, $password, false, false);
    $query = "EXEC sp_columns @table_qualifier = '".$database."', ".
                             "@table_owner = '".$owner."', ".
                             "@table_name = '".$table."'\n";
    if (!($res = o2_mssql_execute($query, $server, $user, $password, false, false))) {
        throw new o2_exception("<b>MSSQL</b> server: <i>".$server."</i>",
                               o2error_DBTABLEINFO);
        }
    else {
        foreach ($res as $field) {
            $arr[] = array('field'   => $field['COLUMN_NAME'],
                           'type'    => $field['TYPE_NAME'].
                                        ($field['TYPE_NAME'] != "text" ?
                                         "(".$field['LENGTH'].")" : ""),
                           'default' => str_replace("(", "",
                                        str_replace(")", "",
                                        str_replace("'", "",
                                        $field['COLUMN_DEF']))));
            }
        }
    return $arr;

    }


/**
 * Return an array containing informations about indexes of a mssql table:
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
function o2_mssql_tableindexes($server, $user, $password, $database, $owner, $table) {

    $ret_val = array();
    $query   = "USE ".o2_mssql_o.$database.o2_mssql_c;
    o2_mssql_execute($query, $server, $user, $password, true, false);
    $query = "EXEC sp_helpindex ".o2_mssql_o.$table.o2_mssql_c;
    try {
        $res_local = o2_mssql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $res_local = array();
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    foreach ($res_local as $single_key) {
        if (strtoupper($single_key['INDEX_NAME']) != "JXPK_".strtoupper($table)) {
            $seq_no = 0;
            $ret_val[$single_key['INDEX_NAME']] = array();
            foreach (explode(",", $single_key['INDEX_KEYS']) as $single_segment) {
                if (strpos($single_segment, "(-)")) {
                    $single_segment = trim(str_replace("(-)", "", $single_segment));
                    $dir_local      = "D";
                    }
                else {
                    $single_segment = trim($single_segment);
                    $dir_local      = "A";
                    }
                $ret_val[$single_key['INDEX_NAME']]+=
                    array($seq_no => array('column' => $single_segment,
                                           'dir'    => $dir_local));
                $seq_no++;
                }
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
function o2_mssql_insertfrom($server,
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
    $query = "INSERT INTO ".o2_mssql_o.$database_to.o2_mssql_c.".".
                            o2_mssql_o.$owner_to.o2_mssql_c.".".
                            o2_mssql_o.$table_to.o2_mssql_c.
             " (".$list_to.") SELECT ".$list_from.
             " FROM ".o2_mssql_o.$database_from.o2_mssql_c.".".
                      o2_mssql_o.$owner_from.o2_mssql_c.".".
                      o2_mssql_o.$table_from.o2_mssql_c;
    if ($where) {
       $query.= " WHERE ".$where;
       }
    try {
        $aff_rows = o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLECOPY);
        throw $o2e;
        return false;
        }
    return $aff_rows;

    }


/**
 * Phisically removes a mssql table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_mssql_droptable($server, $user, $password, $database, $owner, $table) {

    $query = "DROP TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                           o2_mssql_o.$owner.o2_mssql_c.".".
                           o2_mssql_o.$table.o2_mssql_c."\n";
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEDROP);
        throw $o2e;
        }
    o2_mssql_commit($server, $user, $password, $database);
    return true;

    }


/**
 * Phisically renames a mssql table
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
function o2_mssql_renametable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $new_name) {

    try {
        // ______________________________________________________ Get existing indexes ___
        $query = "USE ".o2_mssql_o.$database.o2_mssql_c."\n";
        o2_mssql_execute($query, $server, $user, $password, true, false);
        $query = "EXEC sp_helpindex ".o2_mssql_o.$table.o2_mssql_c;
        $res   = o2_mssql_execute($query, $server, $user, $password, false, false);
        // ______________________________________________________________ Rename table ___
        $query = "EXEC sp_rename '".$database.".".$owner.".".$table."', "."'".
                                    $new_name."'\n";
        o2_mssql_execute($query, $server, $user, $password, true, true);
        // ________________________________________________________ Rename primary key ___
        foreach ($res as $single_key) {
            if (strtoupper($single_key['INDEX_NAME']) == "JXPK_".strtoupper($table)) {
                $query = "EXEC sp_rename '".$single_key['INDEX_NAME']."','JXPK_".
                                            $new_name."','OBJECT'";
                o2_mssql_execute($query, $server, $user, $password, true, true);
                break;
                }
            }

        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEREBUILD);
        throw $o2e;
        }
    o2_mssql_commit($server, $user, $password, $database);
    return true;

    }


/**
 * If $execute is passed TRUE function phisically creates a MSSQL table, else it returns
 * the mssql creation script for the table.
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
function o2_mssql_createtable($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $structure,
                              $execute = true) {

    $query = "CREATE TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                                   o2_mssql_o.$owner.o2_mssql_c.".".
                                   o2_mssql_o.$table.o2_mssql_c." (\n";
    $type        = "";
    $default     = "";
    foreach ($structure["fields"] as $field_name => $field_prop) {
        $query.= o2_mssql_field_create($field_name,
                                       $field_prop['type'],
                                       $field_prop['size'],
                                       $field_prop['int'],
                                       $field_prop['dec']).",\n";
        }
    $query = substr($query, 0, -2).")\n";
    $query.= "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                            o2_mssql_o.$owner.o2_mssql_c.".".
                            o2_mssql_o.$table.o2_mssql_c."\n";
    // ___________________________________________________________________ PRIMARY KEY ___
    $arr_local = array();
    foreach ($structure["primary"] as $pk_name => $pk_segments) {
        break;
        }
    $query.= "ADD CONSTRAINT JXPK_".$table." PRIMARY KEY CLUSTERED (".
             o2_mssql_indexfields_create($pk_segments).") ON [PRIMARY]\n";
    // _______________________________________________________________________ INDEXES ___
    foreach ($structure["keys"] as $index_name => $index_segs) {
        $query.= o2_mssql_index_create($index_name,
                                       $index_segs,
                                       $table,
                                       $database,
                                       $owner)."\n";
        }
    if ($execute) {
        try {
            o2_mssql_execute($query, $server, $user, $password, true, false);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        o2_mssql_commit($server, $user, $password, $database);
        return true;
        }
    else {
        return $query;
        }

    }


/**
 * Returns mssql definition for single field by Janox-type
 *
 * @param  string  $name         Field name
 * @param  string  $type         Field type (A|N|L|D|T|S)
 * @param  integer $size         Total size for alpha
 * @param  integer $int          Total size or integer part
 * @param  integer $dec          Number of decimals digits (only numbers)
 * @param  boolean $for_change   If script is requested for ALTER COLUMN syntax
 * @return string                SQL code to create field
 */
function o2_mssql_field_create($name, $type, $size, $int, $dec, $for_change = false) {

      switch ($type) {
          case "A":
              if ($size <= 8000) {
                  $ftype = "VARCHAR(".$size.")";
                  }
              else {
                  $ftype = "VARCHAR(max)";
                  }
              $default = "('')";
              break;
          case "N":
              if ($dec) {
                  $ftype = "DECIMAL(".($int + $dec).", ".$dec.")";
                  }
              elseif ($int < 5) {
                  $ftype = "SMALLINT";
                  }
              elseif ($int < 10) {
                  $ftype = "INT";
                  }
              else {
                  $ftype = "BIGINT";
                  }
              $default = "(0)";
              break;
          case "L":
              $ftype    = "CHAR(1)";
              $default = "('0')";
              break;
          case "D":
              $ftype    = "CHAR(8)";
              $default = "('00000000')";
              break;
          case "T":
              $ftype    = "CHAR(6)";
              $default = "('000000')";
              break;
          case "S":
              $ftype    = "VARCHAR(max)";
              $default = "('')";
              break;
          }
    if ($for_change) {
        return $name." ".$ftype." NOT NULL";
        }
    else {
        return $name." ".$ftype." NOT NULL DEFAULT ".$default;
        }

    }


/**
 * Returns mssql definition for single index by Janox-type.
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
function o2_mssql_index_create($key_name,
                               $key_segs,
                               $table,
                               $database,
                               $owner,
                               $unique = true) {

    return 'CREATE'.($unique ? ' UNIQUE' : '').
           ' NONCLUSTERED INDEX '.o2_mssql_o.$key_name.o2_mssql_c.
                           ' ON '.o2_mssql_o.$database.o2_mssql_c.'.'.
                                  o2_mssql_o.$owner.o2_mssql_c.'.'.
                                  o2_mssql_o.$table.o2_mssql_c.
                             ' ('.o2_mssql_indexfields_create($key_segs).')';

    }


/**
 * Return ORDER_BY clausole for all the fields of an index in a mssql context.
 * List has to be comma separated and rightly qualified.
 *
 * $fields array parameter is passed in the form:
 *    $fields[field_name] = direction [D/A]
 *
 * @param  array  $fields
 * @return string
 */
function o2_mssql_indexfields_create($fields) {

    $str = "";
    foreach ($fields as $field => $direction) {
        $str.= ($str ? "," : "").$field.($direction == "D" ? " DESC" : " ASC");
        }
    return $str;

    }


/**
 * Alter a MSSQL table to add a column
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
function o2_mssql_field_add($server,
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

    $query = "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                            o2_mssql_o.$owner.o2_mssql_c.".".
                            o2_mssql_o.$table.o2_mssql_c.
                    " ADD ".o2_mssql_field_create(o2_mssql_o.$field_name.o2_mssql_c,
                                                  $field_type,
                                                  $field_size,
                                                  $field_int,
                                                  $field_dec);
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a mssql table to remove a column
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
function o2_mssql_field_remove($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $field_name) {


    $query = "select o.[NAME] from ".
             o2_mssql_o.$database.o2_mssql_c.".sys.sysobjects o inner join ".
             o2_mssql_o.$database.o2_mssql_c.
             ".sys.syscolumns c on o.id = c.cdefault inner join ".
             o2_mssql_o.$database.o2_mssql_c.
             ".sys.sysobjects t on c.id = t.id where o.xtype = 'D' and t.name = '".$table.
             "' and c.name = '".$field_name."'";
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    if ($res[0]['NAME']) {
        $query = "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                                o2_mssql_o.$owner.o2_mssql_c.".".
                                o2_mssql_o.$table.o2_mssql_c.
                 " DROP CONSTRAINT ".o2_mssql_o.$res[0]['NAME'].o2_mssql_c;
        try {
            $res = o2_mssql_execute($query, $server, $user, $password, false, false);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLEALTER);
            throw $o2e;
            return false;
            }
        }
    $query =  "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                             o2_mssql_o.$owner.o2_mssql_c.".".
                             o2_mssql_o.$table.o2_mssql_c.
             " DROP COLUMN ".o2_mssql_o.$field_name.o2_mssql_c;
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a mssql table to change a column definition
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
function o2_mssql_field_change($server,
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

    // ______________________________________________________ Get actual default value ___
    $query = "SELECT COLUMN_DEFAULT AS [DEFAULT] FROM ".o2_mssql_o.$database.o2_mssql_c.
             ".INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='".$owner.
             "' AND TABLE_NAME = '".$table.
             "' AND COLUMN_NAME = '".$field_name."'";

    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    if ($res[0]['DEFAULT']) {
        $default = '('.ltrim(rtrim($res[0]['DEFAULT'], ')'), '(').')';
        }
    else {
        $default = false;
        }
    // _______________________________________________________ Get actual default name ___
    $query = "select o.[NAME] from ".
             o2_mssql_o.$database.o2_mssql_c.".sys.sysobjects o inner join ".
             o2_mssql_o.$database.o2_mssql_c.
             ".sys.syscolumns c on o.id = c.cdefault inner join ".
             o2_mssql_o.$database.o2_mssql_c.
             ".sys.sysobjects t on c.id = t.id where o.xtype = 'D' and t.name = '".$table.
             "' and c.name = '".$field_name."'";
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    // __________________________________________ Remove default constraint from field ___
    if ($res[0]['NAME']) {
        $const = $res[0]['NAME'];
        $query = "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                                o2_mssql_o.$owner.o2_mssql_c.".".
                                o2_mssql_o.$table.o2_mssql_c.
                 " DROP CONSTRAINT ".o2_mssql_o.$const.o2_mssql_c;
        try {
            o2_mssql_execute($query, $server, $user, $password, true, true);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLEALTER);
            throw $o2e;
            return false;
            }
        }
    // __________________________________________________________________ Alter column ___
    $query =   "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                              o2_mssql_o.$owner.o2_mssql_c.".".
                              o2_mssql_o.$table.o2_mssql_c.
             " ALTER COLUMN ".o2_mssql_field_create(o2_mssql_o.$field_name.o2_mssql_c,
                                                    $field_type,
                                                    $field_size,
                                                    $field_int,
                                                    $field_dec,
                                                    true);
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }
    // ___________________________________________ Restore default constraint on field ___
    if ($default !== false) {
        $query = "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                                o2_mssql_o.$owner.o2_mssql_c.".".
                                o2_mssql_o.$table.o2_mssql_c.
                 " ADD CONSTRAINT ".o2_mssql_o.$const.o2_mssql_c.
                 " DEFAULT ".$default." FOR ".o2_mssql_o.$field_name.o2_mssql_c;
        try {
            o2_mssql_execute($query, $server, $user, $password, true, true);
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLEALTER);
            throw $o2e;
            return false;
            }
        }

    }


/**
 * Alter a mssql table to rename a column
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
function o2_mssql_field_rename($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $old_name,
                               $new_name) {

    $query = "EXEC sp_rename '".$database.".".
                                $owner.".".
                                $table.".".
                                $old_name."', '".
                                $new_name."', 'COLUMN'";
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a mssql table to add an index
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
function o2_mssql_index_add($server,
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
            $quoted_segs[o2_mssql_o.$field_name.o2_mssql_c] = $direction;
            }
        else {
            $quoted_segs[$field_name] = $direction;
            }
        }
    $query = o2_mssql_index_create($index_name,
                                   $quoted_segs,
                                   $table,
                                   $database,
                                   $owner,
                                   $unique);
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Alter a mssql table to remove an existing index
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
function o2_mssql_index_remove($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $index_name) {

    $query = "DROP INDEX ".o2_mssql_o.$index_name.o2_mssql_c.
                    " ON ".o2_mssql_o.$database.o2_mssql_c.".".
                           o2_mssql_o.$owner.o2_mssql_c.".".
                           o2_mssql_o.$table.o2_mssql_c;
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        }

    }


/**
 * Return an array of calculated mssql aggragate functions for the table.
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
function o2_mssql_aggregate($server,
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
        $functions_list.= ($functions_list == "" ? "" : ",").
                          $single_func["func"]."(".$single_func["field"].") ".
                          "AS ".$func_name;
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_mssql_o.$database.o2_mssql_c.".".
                 o2_mssql_o.$owner.o2_mssql_c.".".
                 o2_mssql_o.$table.o2_mssql_c;
        }
    $query = "SELECT ".$functions_list.
             " FROM ".$table." AS ".o2_mssql_o.$table_alias.o2_mssql_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            $query.= " LEFT JOIN ".$linktab." ON ".$linkon;
            }
        }
    if ($where) {
        $query.= " WHERE ".$where;
        }
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
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
function o2_mssql_verifyrec($server,
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
        $table = o2_mssql_o.$database.o2_mssql_c.".".
                 o2_mssql_o.$owner.o2_mssql_c.".".
                 o2_mssql_o.$table.o2_mssql_c;
        }
    $query = "SELECT TOP 1 ".$select_str." FROM ".$table.
             " AS ".o2_mssql_o.$table_alias.o2_mssql_c;
    if ($where) {
        $query.= " WHERE ".$where;
        }
    if ($order_by) {
        $query.= " ORDER BY ".$order_by;
        }
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
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
 * Modifies mssql record, uniquely identified by $where clause, with values in $sets.
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
function o2_mssql_modifyrec($server,
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
        $upd_str.= ($upd_str != "" ? "," : "").
                   o2_mssql_o.$database.o2_mssql_c.".".
                   o2_mssql_o.$owner.o2_mssql_c.".".
                   o2_mssql_o.$table.o2_mssql_c.".".$field." = ".$value;
        }
    $query = "UPDATE ".o2_mssql_o.$database.o2_mssql_c.".".
                       o2_mssql_o.$owner.o2_mssql_c.".".
                       o2_mssql_o.$table.o2_mssql_c.
             " SET ".$upd_str.
             " FROM ".o2_mssql_o.$database.o2_mssql_c.".".
                      o2_mssql_o.$owner.o2_mssql_c.".".
                      o2_mssql_o.$table.o2_mssql_c.
             " AS ".o2_mssql_o.$table_alias.o2_mssql_c.
             " WHERE ".$where;
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECUPDATE);
        throw $o2e;
        }
    return true;

    }


/**
 * Insert passed record fields in a mssql table.
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
function o2_mssql_insertrec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $fields,
                            $values) {

    $query = "INSERT INTO ".o2_mssql_o.$database.o2_mssql_c.".".
                            o2_mssql_o.$owner.o2_mssql_c.".".
                            o2_mssql_o.$table.o2_mssql_c.
             " (".implode(",", $fields).") VALUES (".implode(",", $values).")";
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECINSERT);
        throw $o2e;
        }
    return true;

    }


/**
 * Delete record of a mssql table for the passed $where clause.
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
function o2_mssql_deleterec($server,
                            $user,
                            $password,
                            $database,
                            $owner,
                            $table,
                            $table_alias,
                            $where) {

    $query = "DELETE ".o2_mssql_o.$database.o2_mssql_c.".".
                       o2_mssql_o.$owner.o2_mssql_c.".".
                       o2_mssql_o.$table.o2_mssql_c.
             " FROM ".o2_mssql_o.$database.o2_mssql_c.".".
                      o2_mssql_o.$owner.o2_mssql_c.".".
                      o2_mssql_o.$table.o2_mssql_c;
    if ($where) {
       $query.= " AS ".o2_mssql_o.$table_alias.o2_mssql_c." WHERE ".$where;
       }
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
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
function o2_mssql_count($server,
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
        $table = o2_mssql_o.$database.o2_mssql_c.".".
                 o2_mssql_o.$owner.o2_mssql_c.".".
                 o2_mssql_o.$table.o2_mssql_c;
        }
    $query = "SELECT COUNT(*) AS COMPUTED FROM ".$table.
             " AS ".o2_mssql_o.$table_alias.o2_mssql_c;
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
    $conn = o2_mssql_connect($server, $user, $password, false);
    // ______________________________________________________ On execution failure ___
    if (!($res = $conn->query($query))) {
        $err = $conn->errorInfo();
        $rb  = $conn->rollBack;
        $rb && @$rb();
        throw new o2_exception("<b>MSSQL</b> server: <i>".$server.
                               "</i><br />Error counting dataset.<hr><code>".
                               $query."</code><br><br>".$err[2],
                               o2error_DBCOUNT);
        return false;
        }
    $dset = $res->fetchAll(PDO::FETCH_ASSOC);
    // _____________________________________ Return integer value stored in "COMPUTED" ___
    return intval($dset[0]['COMPUTED']);

    }


/**
 * Returns a set of $recs records from a mssql table for a passed $where clause.
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
function o2_mssql_recordset($server,
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

    // _____________ Disabled because of problems of cursors persistence across commit ___
    $stm_id = false;
    // ____________________________________________ Statement recall in case of paging ___
    if ($page_where) {
        if ($stm_id && isset($GLOBALS['o2_mssql_stms'][$stm_id])) {
            return o2_mssql_execute(false,
                                    false,
                                    false,
                                    false,
                                    false,
                                    false,
                                    $recs,
                                    $GLOBALS['o2_mssql_stms'][$stm_id]);
            }
        $where = $page_where.$where;
        }
    elseif ($stm_id && isset($GLOBALS['o2_mssql_stms'][$stm_id])) {
        if ($GLOBALS['o2_mssql_stms'][$stm_id]) {
            $GLOBALS['o2_mssql_stms'][$stm_id]->closeCursor();
            }
        $GLOBALS['o2_mssql_stms'][$stm_id] = null;
        unset($GLOBALS['o2_mssql_stms'][$stm_id]);
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_mssql_o.$database.o2_mssql_c.".".
                 o2_mssql_o.$owner.o2_mssql_c.".".
                 o2_mssql_o.$table.o2_mssql_c;
        }
    $query = 'SELECT ';
    if ($stm_id) {
        $stm = 0;
        }
    else {
        $stm = false;
        if ((strtoupper(substr($select_str, 0, 8)) != 'DISTINCT') &&
            (!isset($GLOBALS['jxviewsql']) ||
             (strpos($GLOBALS['jxviewsql'], 'L') !== false))) {
            $query.= 'TOP '.$recs.' ';
            }
        }
    $query.= $select_str.' FROM '.$table.' AS '.o2_mssql_o.$table_alias.o2_mssql_c;
    if ($lock) {
        $query.= ' WITH (UPDLOCK)';
        }
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
    // _________________________________________ Return SQL query instead of executing ___
    if (isset($GLOBALS['jxviewsql'])) {
        $GLOBALS['jxviewsql'] = $query;
        return array();
        }
    try {
        $res = o2_mssql_execute($query,
                                $server,
                                $user,
                                $password,
                                false,
                                $lock,
                                $recs,
                                $stm);
        if ($stm_id) {
            if ($stm) {
                $GLOBALS['o2_mssql_stms'][$stm_id] = $stm;
                }
            elseif (isset($GLOBALS['o2_mssql_stms'][$stm_id])) {
                $GLOBALS['o2_mssql_stms'][$stm_id] = null;
                unset($GLOBALS['o2_mssql_stms'][$stm_id]);
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
 * Alter MSSQL table to create a foreign key.
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
function o2_mssql_fkeyadd($server,
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

    $query = "USE ".o2_mssql_o.$main_db.o2_mssql_c;
    o2_mssql_execute($query, $server, $user, $password, false, false);
    $main_list = array();
    $ref_list  = array();
    foreach ($main_fields as $field) {
        if ($field != "O2ASPID") {
            $main_list[] = o2_mssql_o.$field.o2_mssql_c;
            }
        else {
            $main_list[] = $field;
            }
        }
    foreach ($ref_fields as $field) {
        if ($field != "O2ASPID") {
            $ref_list[] = o2_mssql_o.$field.o2_mssql_c;
            }
        else {
            $ref_list[] = $field;
            }
        }
    $main_list = implode(",", $main_list);
    $ref_list  = implode(",", $ref_list);
    $query     = "ALTER TABLE ".o2_mssql_o.$main_db.o2_mssql_c.".".
                                o2_mssql_o.$main_owner.o2_mssql_c.'.'.
                                o2_mssql_o.$main_table.o2_mssql_c.
                 " WITH NOCHECK ADD CONSTRAINT ".o2_mssql_o.$key_name.o2_mssql_c.
                 " FOREIGN KEY (".$main_list.
                 ") REFERENCES ".o2_mssql_o.$ref_db.o2_mssql_c.".".
                                 o2_mssql_o.$ref_owner.o2_mssql_c.'.'.
                                 o2_mssql_o.$ref_table.o2_mssql_c.
                 " (".$ref_list.")";
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Alter MSSQL table to remove a foreign key.
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
function o2_mssql_fkeyremove($server,
                             $user,
                             $password,
                             $database,
                             $owner,
                             $table,
                             $key_name) {

    $query = "USE ".o2_mssql_o.$database.o2_mssql_c;
    o2_mssql_execute($query, $server, $user, $password, false, false);
    $query = "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                            o2_mssql_o.$owner.o2_mssql_c.".".
                            o2_mssql_o.$table.o2_mssql_c.
             " DROP CONSTRAINT ".o2_mssql_o.$key_name.o2_mssql_c;
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Validate a foreign key in MSSQL table against existing data.
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
function o2_mssql_fkeyvalidate($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

    $query = "USE ".o2_mssql_o.$database.o2_mssql_c;
    o2_mssql_execute($query, $server, $user, $password, false, false);
    $query = "ALTER TABLE ".o2_mssql_o.$database.o2_mssql_c.".".
                            o2_mssql_o.$owner.o2_mssql_c.".".
                            o2_mssql_o.$table.o2_mssql_c.
             " WITH CHECK CHECK CONSTRAINT ".o2_mssql_o.$key_name.o2_mssql_c;
    try {
        o2_mssql_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEALTER);
        throw $o2e;
        return false;
        }
    return true;

    }


/**
 * Returns the list of existing foreign keys for MSSQL table
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @param  string $table
 * @return boolean
 */
function o2_mssql_fkeystablist($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table) {

    $query = "USE ".o2_mssql_o.$database.o2_mssql_c;
    o2_mssql_execute($query, $server, $user, $password, false, false);
    $query = "SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS ".
             "WHERE TABLE_CATALOG='".$database.
             "' AND TABLE_SCHEMA='".$owner.
             "' AND TABLE_NAME='".$table.
             "' AND CONSTRAINT_TYPE='FOREIGN KEY'";
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        $res = array();
        }
    $list = array();
    foreach ($res as $c_name) {
        $list[] = $c_name["CONSTRAINT_NAME"];
        }
    return $list;

    }


/**
 * Returns the list of existing foreign keys for MSSQL db-schema and some informations
 * about constraints
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_mssql_fkeysinfo($server,
                            $user,
                            $password,
                            $database,
                            $owner) {

    $query = "USE ".$database."; ".
             "SELECT c.CONSTRAINT_NAME AS ".o2_mssql_o."CN".o2_mssql_c.", ".
             "c.TABLE_NAME AS ".o2_mssql_o."ON".o2_mssql_c.", ".
             "COL_NAME(fc.parent_object_id, fc.parent_column_id) AS ".
                                                          o2_mssql_o."SK".o2_mssql_c.", ".
             "OBJECT_NAME (f.referenced_object_id) AS ".o2_mssql_o."TO".o2_mssql_c.", ".
             "COL_NAME(fc.referenced_object_id, fc.referenced_column_id) AS ".
                                                           o2_mssql_o."TK".o2_mssql_c." ".
             "FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS AS c ".
             "INNER JOIN sys.foreign_keys AS f ".
             "ON c.CONSTRAINT_NAME = f.name ".
             "INNER JOIN sys.foreign_key_columns AS fc ".
             "ON f.OBJECT_ID = fc.constraint_object_id ".
             "WHERE c.CONSTRAINT_CATALOG='".$database."' AND ".
                   "c.CONSTRAINT_SCHEMA='".$owner."'";
    try {
        $res = o2_mssql_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    $list = array();
    foreach ($res as $c) {
        if (!isset($list[$c["CN"]])) {
            $list[$c["CN"]] = array("ON" => $c["ON"],
                                    "TO" => $c["TO"],
                                    "SK" => array(),
                                    "TK" => array());
            }
        $list[$c["CN"]]["SK"][] = $c["SK"];
        $list[$c["CN"]]["TK"][] = $c["TK"];
        }
    return $list;

    }


/**
 * Commit open transaction on mssql database
 *
 * @param string  $server
 * @param string  $user
 * @param string  $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_mssql_commit($server, $user, $password, $end = false) {

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
    if ($GLOBALS['o2_mssql_trans'][$key]) {
        // ___________________________________ Check for errors on current transaction ___
        if ($GLOBALS['o2_mssql_error'][$key]) {
            $GLOBALS['o2_mssql_conn'][$key]->rollBack();
            unset($GLOBALS['o2_mssql_error'][$key]);
            }
        // ________________________________________________ Commit current transaction ___
        else {
            // ___________________________________ Close open statements before commit ___
            if ($end) {
                if (is_array($GLOBALS['o2_mssql_stms'])) {
                    foreach ($GLOBALS['o2_mssql_stms'] as $id => $stm) {
                        if ($GLOBALS['o2_mssql_stms'][$id]) {
                            $GLOBALS['o2_mssql_stms'][$id]->closeCursor();
                            }
                        $GLOBALS['o2_mssql_stms'][$id] = null;
                        $stm                           = null;
                        }
                    }
                unset($GLOBALS['o2_mssql_stms']);
                }
            if (!$GLOBALS['o2_mssql_conn'][$key]->commit()) {
                $err = $GLOBALS['o2_mssql_conn'][$key]->errorInfo();
                throw new o2_exception("<b>MSSQL</b> server: <i>".$server."</i><hr />".
                                       $err[2],
                                       o2error_DBCOMMIT);
                }
            }
        unset($GLOBALS['o2_mssql_trans'][$key]);
        }
    // _________________________________________________ Check for existing connection ___
    if ($end && $GLOBALS['o2_mssql_conn'][$key]) {
        unset($GLOBALS['o2_mssql_conn'][$key]);
        }

    }

?>
