<?php

/**
 * Janox SQLITE 3 Database Gateway
 * PHP7/8
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
 * This script contains Janox data functions set for SQLite database
 *
 * @name      jxsqlite
 * @package   janox/dbms/jxdb_sqlite3.php
 * @version   3.0
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007-2025
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


if (!extension_loaded('pdo_sqlite')) {
    throw new o2_exception('<b>SQLite3 PDO</b> driver not loaded:<br>'.
                           'check your PHP configuration for '.
                           '<i>pdo</i> and <i>pdo_sqlite</i> extensions.',
                           o2error_DBCONNECT);
    }

define('o2_sqlite3_o', '`');
define('o2_sqlite3_c', '`');
$GLOBALS['o2_sqlite3_conn']  = array();
$GLOBALS['o2_sqlite3_trans'] = array();
$GLOBALS['o2_sqlite3_stms']  = array();
$GLOBALS['o2_sqlite3_error'] = array();

/**
 * Normalize a string for db writing escaping special chars
 *
 * @param  string  $string
 * @param  boolean $untrim
 * @return string
 */
function o2_sqlite3_normalize($string, $untrim = false) {

    return strtr(($untrim ? $string : trim($string)), array("'" => "''"));

    }


/**
 * Concatenate two or more strings and/or fields
 *
 * @param  array $strings
 * @return string
 */
function o2_sqlite3_concat($strings) {

    return '('.implode('||', $strings).')';

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
function o2_sqlite3_qualify($database, $owner, $name = '') {

    return o2_sqlite3_o.$database.($name ? $name.o2_sqlite3_c : '');

    }


/**
 * Return a sqlite connection handle
 *
 * @param  string $location   SQLite database file path
 * @param  string $user       user (unused)
 * @param  string $password   password (unused)
 * @param  string $trans      open transaction
 * @return PDO
 */
function o2_sqlite3_connect($location, $user, $password, $trans) {

    $app = $_SESSION['o2_app'];
    // ______________________________________________ Prevent blank and relative names ___
    if (!$location) {
        $location = $app->dir_data.$app->nome.'.db';
        }
    // _________________________________________________________ Session physical file ___
    elseif (strpos(strtolower($location), ':session:') === 0) {
        if (strtolower($location) == ':session:') {
            $location = $app->dir_tmp.$app->nome.'.db';
            }
        else {
            $location = $app->dir_tmp.substr($location, 9);
            }
        }
    elseif (strpos(':memory:', strtolower($location)) === false &&
            basename($location) == $location) {
        $location = $app->dir_data.$location;
        }
    // _________________________________________________ Check for existing connection ___
    if (!isset($GLOBALS['o2_sqlite3_conn'][$location])) {
        // _____________________________________________________ Create new connection ___
        try {
            $conn_local = new PDO('sqlite:'.$location,
                                  null,
                                  null,
                                  array(PDO::ATTR_PERSISTENT => false,
                                        PDO::ATTR_ERRMODE    => PDO::ERRMODE_EXCEPTION));
            }
        catch (Exception $o2e) {
            throw new o2_exception('<b>SQLite3</b> server: <i>'.$location.'</i><hr>'.
                                   $o2e->getMessage(),
                                   o2error_DBCONNECT);
            }
        // ______________________________________________ Create transaction if needed ___
        if ($trans) {
            try {
                $conn_local->beginTransaction();
                }
            catch (Exception $o2e) {
                throw new o2_exception('<b>SQLite3</b> server: <i>'.$location.'</i><hr>'.
                                       $o2e->getMessage(),
                                       o2error_DBCONNECT);
                }
            $GLOBALS['o2_sqlite3_trans'][$location] = true;
            }
        // ___________________________________________________________ Save connection ___
        $GLOBALS['o2_sqlite3_conn'][$location] = $conn_local;
        }
    // _________________________________________________________ Check for transaction ___
    elseif ($trans && !isset($GLOBALS['o2_sqlite3_trans'][$location])) {
        $conn_local = $GLOBALS['o2_sqlite3_conn'][$location];
        // _________________________________ Create transaction on existing connection ___
        try {
            $conn_local->beginTransaction();
            }
        catch (Exception $o2e) {
            throw new o2_exception('<b>SQLite3</b> server: <i>'.$location.'</i><hr>'.
                                   $o2e->getMessage(),
                                   o2error_DBCONNECT);
            }
        $GLOBALS['o2_sqlite3_trans'][$location] = true;
        }
    return $GLOBALS['o2_sqlite3_conn'][$location];

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
 * @param  string  $query           Query to be executed
 * @param  string  $server          Host to connect to
 * @param  string  $user            Connecion user name
 * @param  string  $password        Connection user password
 * @param  boolean $only_exe        Execution only, no dataset returned
 * @param  boolean $trans           Transaction needed (insert, update, ... / queries)
 * @param  integer $limit           Number of records to fetch
 * @param  boolean $stm             Statement to fetch on or prepared statement
 * @param  array   $prepared_pars   Actual parameters for prepared statement
 * @return mix
 */
function o2_sqlite3_execute($query,
                            $server,
                            $user,
                            $password,
                            $only_exe      = false,
                            $trans         = true,
                            $limit         = 0,
                            &$stm          = false,
                            $prepared_pars = false) {

    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace && !$stm) {
        if ($stm === 0) {
            o2log('(Start fetching) '.$query);
            }
        else {
            if ($prepared_pars) {
                $ppars = "\nWith parameters: ".print_r($prepared_pars, 1);
                }
            o2log($query.$ppars);
            }
        }
    // _____________________________________________ Execution only, no dataset return ___
    if ($only_exe) {
        // __________________________________________________ Get connection to server ___
        $conn = o2_sqlite3_connect($server, $user, $password, $trans);
        try {
            $res = $conn->exec($query);
            }
        // ____________________________________________________ On execution exception ___
		catch (Exception $o2e) {
            $conn->rollBack && @$conn->rollBack();
            $res = false;
            throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                                   '</i><br>query: <code>'.$query.'</code><hr>'.
                                   $o2e->getMessage(),
                                   o2error_DBEXECUTE);
            return false;
            }
        return $res;
        }
    // ________________________________________________________ Queries returning data ___
    else {
        // __________________________________________________ Get connection to server ___
        $conn = o2_sqlite3_connect($server, $user, $password, $trans);
        $loop = ($stm || ($stm === 0));
        if (!$stm) {
            try {
                // _____________ Create prepared statement for reusing with parameters ___
                if ($prepared_pars) {
                    // __________________ Create statement and check execution failure ___
                    $stm = $conn->prepare($query);
                    }
                // _________________________ Create statement for reusing for fetching ___
                else {
                    // ______________________________________________ Create statement ___
                    $stm = $conn->query($query);
                    }
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                                       '</i><br>query: <code>'.$query.'</code><hr>'.
                                       $o2e->getMessage(),
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
        // _______________________ If statement is prepared execute it with parameters ___
        if ($prepared_pars) {
            try {
                $stm->execute($prepared_pars);
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $conn->rollBack && @$conn->rollBack();
				$q = ($stm ? '<br>query: <code>'.$stm->queryString.'</code>' : $query);
                throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                                       '</i>'.$q.'<hr>'.$o2e->getMessage(),
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
        // ____________________________________________________________ Return dataset ___
        if ($loop) {
            $set = array();
            $num = 0;
            try {
                while ((!$limit || $num < $limit) &&
                       ($row = $stm->fetch(PDO::FETCH_ASSOC))) {
                    $num++;
                    $set[] = $row;
                    }
                if ($limit && $num < $limit && !$prepared_pars) {
                    $stm->closeCursor();
                    $stm = null;
                    }
                return $set;
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $conn->rollBack && @$conn->rollBack();
				$q = ($stm ? '<br>query: <code>'.$stm->queryString.'</code>' : $query);
                throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                                       '</i>'.$q.'<hr>'.$o2e->getMessage(),
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
        else {
            try {
                $data  = $stm->fetchAll(PDO::FETCH_ASSOC);
                if (!$prepared_pars) {
                    $stm->closeCursor();
                    $stm = null;
                    }
                return $data;
                }
            // ________________________________________________ On execution exception ___
            catch (Exception $o2e) {
                $stm->closeCursor();
                $stm = null;
                $conn->rollBack && @$conn->rollBack();
                throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                                       '</i><br>query: <code>'.$query.
                                       '</code><hr>'.$o2e->getMessage(),
                                       o2error_DBDATAQUERY);
                return false;
                }
            }
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
function o2_sqlite3_tables($server, $user, $password, $database, $owner) {

    $query = 'SELECT name FROM sqlite_master '.
             "WHERE (type='table' OR type='view') AND name LIKE '".$database."%'";
    $conn  = o2_sqlite3_connect($server, $user, $password, false);
    try {
        $res = $conn->query($query)->fetchAll(PDO::FETCH_ASSOC);
        }
    catch (Exception $o2e) {
        throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                               '</i><hr>'.$o2e->getMessage(),
                               o2error_DBTABLES);
        }
    $tables = array();
    foreach ($res as $tab) {
        $tables[] = $tab['name'];
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
function o2_sqlite3_tabexists($server, $user, $password, $database, $owner, $table) {

    $query = 'SELECT 1 AS IT_EXISTS FROM sqlite_master WHERE '.
             "(type='table' OR type='view') AND name = '".$database.$table."'";
    $conn  = o2_sqlite3_connect($server, $user, $password, false);
    try {
        $res = $conn->query($query)->fetch(PDO::FETCH_ASSOC);
        }
    catch (Exception $o2e) {
        throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                               '</i><hr>'.$o2e->getMessage(),
                               o2error_DBTABEXISTS);
        }
    return (isset($res['IT_EXISTS']) && $res['IT_EXISTS'] ? true : false);

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
function o2_sqlite3_tablefields($server, $user, $password, $database, $owner, $table) {

    $query = 'PRAGMA table_info('.o2_sqlite3_o.$database.$table.o2_sqlite3_c.')';
    try {
        $res = o2_sqlite3_execute($query, $server, $user, $password, false, false);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEINFO);
        throw $o2e;
        }
    $arr = array();
    foreach ($res as $field) {
        $arr[] = array('field'   => $field['name'],
                       'type'    => $field['type'],
                       'default' => $field['dflt_value']);
        }
    return $arr;

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
function o2_sqlite3_tableindexes($server, $user, $password, $database, $owner, $table) {

    $ret_val     = array();
    $query_local = 'PRAGMA index_list('.o2_sqlite3_o.$database.$table.o2_sqlite3_c.')';
    try {
        $res_local = o2_sqlite3_execute($query_local,
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
        $key_name = $single_key['name'];
        if ($key_name != $database.$table.'_PK' &&
            !strpos(strtoupper($key_name), 'AUTOINDEX')) {
            if (!isset($ret_val[$key_name])) {
                $ret_val[str_replace($database.$table.'_', '', $key_name)] =
                          array('unique' => $single_key['unique'], 'segments' => array());
                }
            $query_local = 'PRAGMA index_info('.o2_sqlite3_o.$key_name.o2_sqlite3_c.')';
            $query_info  = 'SELECT '.o2_sqlite3_o.'sql'.o2_sqlite3_c.' '.
                                     o2_sqlite3_o.'SQL'.o2_sqlite3_c.
                           ' FROM '.o2_sqlite3_o.'sqlite_master'.o2_sqlite3_c.
                           ' WHERE '.o2_sqlite3_o.'type'.o2_sqlite3_c."='index' and ".
                           o2_sqlite3_o.'tbl_name'.o2_sqlite3_o."='".$table."' and ".
                           o2_sqlite3_o.'name'.o2_sqlite3_c."='".$key_name."'";
            try {
                $res_key  = o2_sqlite3_execute($query_local,
                                               $server,
                                               $user,
                                               $password,
                                               false,
                                               false);
                $key_info = o2_sqlite3_execute($query_info,
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
            foreach ($res_key as $single_segment) {
                $parts = array();
                preg_match('/'.o2_sqlite3_o.$single_segment['name'].o2_sqlite3_c.
                           ' (ASC|DESC)/',
                           $key_info[0]['SQL'],
                           $parts);
                $ret_val[str_replace($database.$table.'_', '', $key_name)]['segments']+=
                    array($single_segment['seqno'] =>
                          array('column' => $single_segment['name'],
                                'dir'    => $parts[1] == 'ASC' ? 'A' : 'D'));
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
function o2_sqlite3_insertfrom($server,
                               $user,
                               $password,
                               $database_from,
                               $owner_from,
                               $table_from,
                               $database_to,
                               $owner_to,
                               $table_to,
                               $values,
                               $where = '') {

    $list_from = '';
    $list_to   = '';
    foreach ($values as $field_from => $field_to) {
        $sep = ($list_from ? ',' : '');
        if ($field_from == '@o2CloneArea') {
            $list_from.= $sep.$field_to;
            $list_to  .= $sep.'O2ASPID';
            }
        else {
            $list_from.= $sep.$field_from;
            $list_to  .= $sep.$field_to;
            }
        }
    $query = 'INSERT INTO '.o2_sqlite3_o.$database_to.$table_to.o2_sqlite3_c.
             ' ('.$list_to.') SELECT '.$list_from.
             ' FROM '.o2_sqlite3_o.$database_from.$table_from.o2_sqlite3_c;
    if ($where) {
       $query.= ' WHERE '.$where;
       }
    try {
        $aff_rows = o2_sqlite3_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLECOPY);
        throw $o2e;
        return false;
        }
    return $aff_rows;

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
function o2_sqlite3_droptable($server, $user, $password, $database, $owner, $table) {

    $query_local = 'DROP TABLE '.o2_sqlite3_o.$database.$table.o2_sqlite3_c;
    try {
        o2_sqlite3_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEDROP);
        throw $o2e;
        }
    o2_sqlite3_commit($server, $user, $password, $database);
    return true;

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
function o2_sqlite3_renametable($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $new_name) {
    try {
        // _____________________________________________________ Copy existing indexes ___
        $keys_query = array();
        $keys       = o2_sqlite3_tableindexes($server,
                                              $user,
                                              $password,
                                              $database,
                                              $owner,
                                              $table);

        foreach ($keys as $keyname => $key) {
            $arr_local = array();
            foreach ($key['segments'] as $keystructure) {
                $arr_local[] = o2_sqlite3_o.$keystructure['column'].o2_sqlite3_c.
                               ($keystructure['dir'] == 'D' ? ' DESC' : ' ASC');
                }
            $keys_query[]= 'CREATE'.($key['unique'] ? ' UNIQUE' : '').' INDEX '.
                           o2_sqlite3_o.$database.$new_name."_".$keyname.o2_sqlite3_c.
                           ' ON '.o2_sqlite3_o.$database.$new_name.o2_sqlite3_c.' ('.
                           implode(',', $arr_local).")\n";
            }
        // _____________________________________________________ Drop existing indexes ___
        $query_local = 'PRAGMA index_list('.o2_sqlite3_o.$database.$table.o2_sqlite3_c.
                       ')';
        $res_local   = o2_sqlite3_execute($query_local,
                                          $server,
                                          $user,
                                          $password,
                                          false,
                                          false);
        foreach ($res_local as $single_key) {
            $key_name = $single_key['name'];
            if (stripos($key_name, 'sqlite_autoindex') === false) {
                o2_sqlite3_execute('DROP INDEX '.o2_sqlite3_o.$key_name.o2_sqlite3_c,
                                   $server,
                                   $user,
                                   $password,
                                   true,
                                   true);
                }
            }
        // _____________________________________ Rename table and recreate new indexes ___
        $query_local = 'ALTER TABLE '.o2_sqlite3_o.$database.$table.o2_sqlite3_c.
                       ' RENAME TO '.o2_sqlite3_o.$database.$new_name.o2_sqlite3_c;
        o2_sqlite3_execute($query_local, $server, $user, $password, true, true);
        foreach ($keys_query as $single_query) {
            o2_sqlite3_execute($single_query, $server, $user, $password, true, true);
            }
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBTABLEREBUILD);
        throw $o2e;
        }
    o2_sqlite3_commit($server, $user, $password, $database);
    return true;

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
function o2_sqlite3_createtable($server,
                                $user,
                                $password,
                                $database,
                                $owner,
                                $table,
                                $structure,
                                $execute = true) {

    $query_local = 'CREATE TABLE '.o2_sqlite3_o.$database.$table.o2_sqlite3_c." (\n";
    $type        = '';
    $default     = '';
    foreach ($structure['fields'] as $field_name => $field_prop) {
        switch ($field_prop['type']) {
            case 'A':
                if ($field_prop['size'] <= 255) {
                    $type = 'varchar('.$field_prop['size'].')';
                    }
                elseif ($field_prop['size'] <= 65530) {
                    $type = 'text';
                    }
                else {
                    $type = 'longtext';
                    }
                $default = "''";
                break;
            case 'N':
                $type    = ($field_prop['dec'] ?
                            'numeric('.($field_prop['int'] + $field_prop['dec']).', '.
                                        $field_prop['dec'].')' :
                            ($field_prop['int'] > 9 ?
                             'bigint('.$field_prop['int'].')' :
                             'integer'));
                $default = '0';
                break;
            case 'L':
                $type    = 'char(1)';
                $default = "'0'";
                break;
            case 'D':
                $type    = 'char(8)';
                $default = "'00000000'";
                break;
            case 'T':
                $type    = 'char(6)';
                $default = "'000000'";
                break;
            case 'S':
                $type    = 'text';
                $default = "''";
                break;
            }
        $query_local.= $field_name.' '.$type.' NOT NULL DEFAULT '.$default.",\n";
        }
    // ___________________________________________________________________ Primary key ___
    $arr_local = array();
    foreach ($structure['primary'] as $pk_name => $pk_segments) {
        foreach ($pk_segments as $field_name => $direction) {
            $arr_local[] = $field_name.($direction == 'D' ? ' DESC' : ' ASC');
            }
        break;
        }
    $query_local.= 'CONSTRAINT '.$database.$table.'_PK PRIMARY KEY ('.
                   implode(',', $arr_local)."))\n";
    $indexes_queries = array();
    // ________________________________________________________________ Unique indexes ___
    foreach ($structure['keys'] as $index_name => $index_segs) {
        $arr_local = array();
        foreach ($index_segs as $field_name => $direction) {
            $arr_local[] = $field_name.($direction == 'D' ? ' DESC' : ' ASC');
            }
        $indexes_queries[]= 'CREATE UNIQUE INDEX '.
                            o2_sqlite3_o.$database.$table.'_'.$index_name.o2_sqlite3_c.
                            ' ON '.o2_sqlite3_o.$database.$table.o2_sqlite3_c.' ('.
                            implode(',', $arr_local).")\n";
        }
    // ____________________________________________________________ Not unique indexes ___
    foreach ($structure['indexes'] as $index_name => $index_segs) {
        $arr_local = array();
        foreach ($index_segs as $field_name => $direction) {
            $arr_local[] = $field_name.($direction == 'D' ? ' DESC' : ' ASC');
            }
        $indexes_queries[]= 'CREATE INDEX '.
                            o2_sqlite3_o.$database.$table.'_'.$index_name.o2_sqlite3_c.
                            ' ON '.o2_sqlite3_o.$database.$table.o2_sqlite3_c.' ('.
                            implode(',', $arr_local).")\n";
        }
    if ($execute) {
        try {
            o2_sqlite3_execute($query_local, $server, $user, $password, true, true);
            foreach ($indexes_queries as $single_query) {
                o2_sqlite3_execute($single_query, $server, $user, $password, true, true);
                }
            }
        catch (o2_exception $o2e) {
            $o2e->set_error_class(o2error_DBTABLECREATE);
            throw $o2e;
            }
        o2_sqlite3_commit($server, $user, $password, $database);
        return true;
        }
    else {
        return $query_local;
        }

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
function o2_sqlite3_aggregate($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $where,
                              $functions,
                              $links = null) {

    $functions_list = '';
    foreach ($functions as $func_name => $single_func) {
        $functions_list.= ($functions_list ? ',' : '').
                           $single_func['func'].'('.$single_func['field'].
                          ') AS '.o2_sqlite3_o.$func_name.o2_sqlite3_c;
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != '(') {
        $table = o2_sqlite3_o.$database.$table.o2_sqlite3_c;
        }
    $query = 'SELECT '.$functions_list.
             ' FROM '.$table.' AS '.o2_sqlite3_o.$table_alias.o2_sqlite3_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            // _________________________________ Insert database in table name, if any ___
            if ($database) {
                $linktab = substr($linktab, 0, 1).$database.substr($linktab, 1);
                }
            $query.= ' LEFT JOIN '.$linktab.' ON '.$linkon;
            }
        }
    if ($where) {
        $query.= ' WHERE '.$where;
        }
    try {
        $res_local = o2_sqlite3_execute($query, $server, $user, $password, false);
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
function o2_sqlite3_verifyrec($server,
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
    if (substr($table, 0, 1) != '(') {
        $table = o2_sqlite3_o.$database.$table.o2_sqlite3_c;
        }
    $query = 'SELECT '.$select_str.' FROM '.$table.
             ' AS '.o2_sqlite3_o.$table_alias.o2_sqlite3_c;
    if ($where) {
        $query.= ' WHERE '.$where;
        }
    if ($order_by) {
        $query.= ' ORDER BY '.$order_by;
        }
    $query.= ' LIMIT 1';
    try {
        $res = o2_sqlite3_execute($query, $server, $user, $password, false, false);
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
function o2_sqlite3_modifyrec($server,
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
        $upd_str.= ($upd_str ? ',' : '').$field.' = '.$value;
        }
    // ____________________________ SQLite does not support alias for UPDATE statement ___
    $where = str_replace(o2_sqlite3_o.$table_alias.o2_sqlite3_c.'.', '', $where);
    $query = 'UPDATE '.o2_sqlite3_o.$database.$table.o2_sqlite3_c.
             ' SET '.$upd_str.' WHERE '.$where;
    try {
        o2_sqlite3_execute($query, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECUPDATE);
        throw $o2e;
        }
    return true;

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
function o2_sqlite3_insertrec($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $fields,
                              $values) {

    $query_local = 'INSERT INTO '.o2_sqlite3_o.$database.$table.o2_sqlite3_c.
                   ' ('.implode(',', $fields).') VALUES ('.implode(',', $values).')';
    try {
        o2_sqlite3_execute($query_local, $server, $user, $password, true, true);
        }
    catch (o2_exception $o2e) {
        $o2e->set_error_class(o2error_DBRECINSERT);
        throw $o2e;
        }
    return true;

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
function o2_sqlite3_deleterec($server,
                              $user,
                              $password,
                              $database,
                              $owner,
                              $table,
                              $table_alias,
                              $where) {

    $query = 'DELETE FROM '.o2_sqlite3_o.$database.$table.o2_sqlite3_c;
    if ($where) {
        // ________________________ SQLite does not support alias for DELETE statement ___
        $where = str_replace(o2_sqlite3_o.$table_alias.o2_sqlite3_c.'.', '', $where);
        $query.= ' WHERE '.$where;
        }
    // _________________________________________ Return SQL query instead of executing ___
    if (isset($GLOBALS['jxviewsql'])) {
        $GLOBALS['jxviewsql'] = $query;
        return true;
        }
    try {
        o2_sqlite3_execute($query, $server, $user, $password, true, true);
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
function o2_sqlite3_count($server,
                          $user,
                          $password,
                          $database,
                          $owner,
                          $table,
                          $table_alias,
                          $where,
                          $links = null) {

    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != '(') {
        $table = o2_sqlite3_o.$database.$table.o2_sqlite3_c;
        }
    $query = 'SELECT COUNT(*) AS COMPUTED FROM '.$table.
             ' AS '.o2_sqlite3_o.$table_alias.o2_sqlite3_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            // _________________________________ Insert database in table name, if any ___
            if ($database) {
                $linktab = substr($linktab, 0, 1).$database.substr($linktab, 1);
                }
            $query.= ' LEFT JOIN '.$linktab.' ON '.$linkon;
            }
        }
    if ($where) {
        $query.= ' WHERE '.$where;
        }
    // _____________________________________________________________________ SQL trace ___
    if ($_SESSION['o2_app']->sqltrace) {
        o2log($query);
        }
    // ______________________________________________________ Get connection to server ___
    $conn = o2_sqlite3_connect($server, $user, $password, false);
    try {
        $dset = $conn->query($query)->fetchAll(PDO::FETCH_ASSOC);
        }
    // ________________________________________________________ On execution exception ___
    catch (Exception $o2e) {
        $rb = $conn->rollBack;
        $rb && @$rb();
        throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                               '</i><br>Error counting dataset.<hr><code>'.
                               $query.'</code><br><br>'.$o2e->getMessage(),
                               o2error_DBCOUNT);
        return false;
        }
    // _____________________________________ Return integer value stored in "COMPUTED" ___
    return intval($dset[0]['COMPUTED']);

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
 * @param  array   $prepared_pars
 * @return array
*/
function o2_sqlite3_recordset($server,
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
                              $links         = null,
                              $lock          = false,
                              $page_where    = false,
                              $stm_id        = false,
                              $prepared_pars = false) {

    // ____________________________________________ Statement recall in case of paging ___
    if ($page_where) {
        if ($stm_id && isset($GLOBALS['o2_sqlite3_stms'][$stm_id])) {
            return o2_sqlite3_execute(false,
                                      $server,
                                      $user,
                                      $password,
                                      false,
                                      false,
                                      $recs,
                                      $GLOBALS['o2_sqlite3_stms'][$stm_id],
                                      $prepared_pars);
            }
        $where = $page_where.$where;
        }
    elseif ($stm_id && isset($GLOBALS['o2_sqlite3_stms'][$stm_id]) && !$prepared_pars) {
        if ($GLOBALS['o2_sqlite3_stms'][$stm_id]) {
            $GLOBALS['o2_sqlite3_stms'][$stm_id]->closeCursor();
            }
        $GLOBALS['o2_sqlite3_stms'][$stm_id] = null;
        unset($GLOBALS['o2_sqlite3_stms'][$stm_id]);
        }
    // __________________ If $table starts with a "(" then table is a sub-select query ___
    if (substr($table, 0, 1) != "(") {
        $table = o2_sqlite3_o.$database.$table.o2_sqlite3_c;
        }
    $query = 'SELECT '.$select_str.' FROM '.$table.
             ' AS '.o2_sqlite3_o.$table_alias.o2_sqlite3_c;
    if ($links) {
        foreach ($links as $linktab => $linkon) {
            // _________________________________ Insert database in table name, if any ___
            if ($database) {
                $linktab = substr($linktab, 0, 1).$database.substr($linktab, 1);
                }
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
    if ($stm_id && $prepared_pars && isset($GLOBALS['o2_sqlite3_stms'][$stm_id])) {
        $stm = $GLOBALS['o2_sqlite3_stms'][$stm_id];
        }
    elseif ($stm_id) {
        $stm = 0;
        }
    else {
        $stm = false;
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
    try {
        $res = o2_sqlite3_execute($query,
                                  $server,
                                  $user,
                                  $password,
                                  false,
                                  $lock,
                                  $recs,
                                  $stm,
                                  $prepared_pars);
        if ($stm_id) {
            if ($stm) {
                $GLOBALS['o2_sqlite3_stms'][$stm_id] = $stm;
                }
            elseif (isset($GLOBALS['o2_sqlite3_stms'][$stm_id])) {
                $GLOBALS['o2_sqlite3_stms'][$stm_id] = null;
                unset($GLOBALS['o2_sqlite3_stms'][$stm_id]);
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
 * Alter SQLite3 table to create a foreign key.
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
function o2_sqlite3_fkeyadd($server,
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
 * Alter SQLite3 table to remove a foreign key.
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
function o2_sqlite3_fkeyremove($server,
                               $user,
                               $password,
                               $database,
                               $owner,
                               $table,
                               $key_name) {

    return false;

    }


/**
 * Validate a foreign key in SQLite3 table against existing data.
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
function o2_sqlite3_fkeyvalidate($server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table,
                                 $key_name) {

    return false;

    }


/**
 * Returns the list of existing foreign keys for SQLite3 table
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
function o2_sqlite3_fkeystablist($server,
                                 $user,
                                 $password,
                                 $database,
                                 $owner,
                                 $table) {

    return array();

    }


/**
 * Returns the list of existing foreign keys for SQLite3 db-schema and some informations
 * about constraints
 *
 *  === NOT IMPLEMENTED YET ===
 *
 * @param  string $server
 * @param  string $user
 * @param  string $password
 * @param  string $database
 * @param  string $owner
 * @return array
 */
function o2_sqlite3_fkeysinfo($server,
                              $user,
                              $password,
                              $database,
                              $owner) {

    return array();

    }

/**
 * Commit open transaction on sqlite database
 *
 * @param string  $server
 * @param string  $user
 * @param string  $password
 * @param boolean $end        Close open statements and connection for script ending
 */
function o2_sqlite3_commit($server, $user, $password, $end = false) {

    $app = $_SESSION['o2_app'];
    // ______________________________________________ Prevent blank and relative names ___
    if (!$server) {
        $server = $app->dir_data.$app->nome.'.db';
        }
    // _________________________________________________________ Session physical file ___
    elseif (strpos(':session:', strtolower($server)) === 0) {
        if (strtolower($server) == ':session:') {
            $server = $app->dir_tmp.$app->nome.'.db';
            }
        else {
            $server = $app->dir_tmp.substr($server, 9);
            }
        }
    elseif (strpos(':memory:', strtolower($server)) === false &&
            basename($server) == $server) {
        $server = $app->dir_data.$server;
        }

    // ________________________________________________ Check for existing transaction ___
    if (isset($GLOBALS['o2_sqlite3_trans'][$server])) {
        // ___________________________________ Check for errors on current transaction ___
        if (isset($GLOBALS['o2_sqlite3_error'][$server])) {
            $GLOBALS['o2_sqlite3_conn'][$server]->rollBack();
            unset($GLOBALS['o2_sqlite3_error'][$server]);
            }
        // ________________________________________________ Commit current transaction ___
        else {
            // ___________________________________ Close open statements before commit ___
            if ($end) {
                if (is_array($GLOBALS['o2_sqlite3_stms'])) {
                    foreach ($GLOBALS['o2_sqlite3_stms'] as $id => $stm) {
                        if ($GLOBALS['o2_sqlite3_stms'][$id]) {
                            $GLOBALS['o2_sqlite3_stms'][$id]->closeCursor();
                            }
                        $GLOBALS['o2_sqlite3_stms'][$id] = null;
                        $stm                             = null;
                        }
                    }
                $GLOBALS['o2_sqlite3_stms'] = array();
                }
            try {
                $GLOBALS['o2_sqlite3_conn'][$server]->commit();
                }
            catch (Exception $o2e) {
                throw new o2_exception('<b>SQLite3</b> server: <i>'.$server.
                                       '</i><hr>'.$o2e->getMessage(),
                                       o2error_DBCOMMIT);
                }
            }
        unset($GLOBALS['o2_sqlite3_trans'][$server]);
        }
    // _________________________________________________ Check for existing connection ___
    if ($end && isset($GLOBALS['o2_sqlite3_conn'][$server])) {
        unset($GLOBALS['o2_sqlite3_conn'][$server]);
        }

    }

?>
