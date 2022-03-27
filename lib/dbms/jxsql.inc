<?php

/**
 * Janox SQL
 * PHP5
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
 * This script defines Janox SQL Parser class and related functions.
 * JXSQL is used to write portable (engine-independent) SQL code.
 *
 *
 * @name      jxsql
 * @package   janox/dbms/jxsql.inc
 * @version   2.7
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
* Parses JXSQL code and returns SQL command to execute for specific database engine
*
*/
class JXSQLparser {

    public    $dbengine              = 'sqlite3';
    protected $databasename          = '';
    protected $dbserver              = '';
    protected $db_phys_name          = '';
    protected $qualifier_o           = '"';
    protected $qualifier_c           = '"';
    protected $fully_qualify_columns = false;
    protected $aliases               = array();

    protected $JXTAB                 = 'JXTAB';
    protected $JXCOL                 = 'JXCOL';
    protected $JXCONCAT              = 'JXCONCAT';
    protected $JXSUBSTRING           = 'JXSUBSTRING';


    public function __construct($dbname, $fq_columns = false) {

        $this->databasename          = $dbname;
        $this->dbserver              = o2db_server($dbname);
        $this->dbengine              = o2dbs_engine($this->dbserver);
        $this->db_phys_name          = o2db_name($dbname);
        $this->qualifier_o           = constant("o2_".$this->dbengine."_o");
        $this->qualifier_c           = constant("o2_".$this->dbengine."_c");
        $this->fully_qualify_columns = $fq_columns;
        $this->tab_qprefix           = o2db_qprefix($dbname);

        }


    private function startsWith($str, $startsWith) {

        if (trim($str) === '') {
            return false;
            }
        else {
            return strpos(strtolower($str), strtolower($startsWith)) === 0;
            }

        }


    private function endsWith($str, $endsWith) {

        if (trim($str) === '') {
            return false;
            }
        else {
            return substr(strtolower($str), -strlen($endsWith)) === strtolower($endsWith);
            }

        }


    /**
     * Parse JXSQL command
     */
    public function parseSQL($sql, $stage) {

        $sres    = null;
        $c       = 0;
        $len     = strlen($sql);
        $ch      = null;
        $sql_buf = '';
        // ______________________________________ Parse command character by character ___
        while ($c < $len) {
            // _________________________________________________________ Get character ___
            $ch = $sql{$c};
            // _________________________________________________ Check for parenthesis ___
            if ($ch == '(') {
                $jxsql_function = null;
                // ___________ Check if the parenthesis is related to a JXSQL function ___
                if ($stage == 1 && $this->endsWith($sql_buf, $this->JXTAB)) {
                    $jxsql_function = $this->JXTAB;
                    }
                elseif ($stage == 2 && $this->endsWith($sql_buf, $this->JXCOL)) {
                    $jxsql_function = $this->JXCOL;
                    }
                elseif ($stage == 3 && $this->endsWith($sql_buf, $this->JXCONCAT)) {
                    $jxsql_function = $this->JXCONCAT;
                    }
                elseif ($stage == 3 && $this->endsWith($sql_buf, $this->JXSUBSTRING)) {
                    $jxsql_function = $this->JXSUBSTRING;
                    }
                // ___________________________________________ If JXSQL function found ___
                if ($jxsql_function != null) {
                    $func_len = strlen($jxsql_function);
                    // _______________ Append buffer contents minus JX function length ___
                    $sres    .= substr($sql_buf, 0, (strlen($sql_buf) - $func_len));
                    // _________________________ Find corresponding closed parenthesis ___
                    $end_par  = $this->find_corresponding_parenthesis($sql, $c);
                    // ____ If closed parenthesis was found (so command properly ends) ___
                    if ($end_par > 0) {
                        // _________________ Calculate JXSQL function start and length ___
                        $start_pos      = $c - $func_len;
                        $length         = $end_par - $start_pos + 1;
                        // ___________________________________ Get JXSQL function call ___
                        $jx_sql_command = substr($sql, $start_pos, $length);
                        // ____________________________________ Execute JXSQL function ___
                        switch ($jxsql_function) {
                            case $this->JXTAB:
                                $sres.= $this->replace_JXTAB(strtolower($jx_sql_command));
                                break;
                            case $this->JXCOL:
                                $sres.= $this->replace_JXCOL(strtolower($jx_sql_command));
                                break;
                            case $this->JXCONCAT:
                                $sres.=
                                     $this->replace_JXCONCAT(strtolower($jx_sql_command));
                                break;
                            case $this->JXSUBSTRING:
                                $sres.=
                                  $this->replace_JXSUBSTRING(strtolower($jx_sql_command));
                                break;
                            }
                        // __________________________ Resuming from closed parenthesis ___
                        $c = $end_par;
                        }
                    else {
                        $sres.= '--MISSING_CLOSE_PARENTHESIS--';
                        }
                    }
                // If not a JXSQL function related parenthesis then empty buffer and go on
                else {
                    $sres.= $sql_buf.'(';
                    }
                $sql_buf = '';
                }
            else {
                $sql_buf.= $ch;
                }
            $c++;
            }
        if (strlen($sql_buf) > 0) {
            $sres.= $sql_buf;
            }
        return $sres;

        }


    /**
    * Find corresponding parenthesis starting from "startpos" position.
    * Any other parenthesis in the middle will be skipped.
    * Returns corresponding parenthesis position
    */
    private function find_corresponding_parenthesis($mystr, $startpos) {

        $c         = $startpos + 1;
        $ch        = '';
        $open_pars = 0;
        $ret       = 0;
        $len       = strlen($mystr);
        while ($c < $len) {
            // _________________________________________________________ Get character ___
            $ch = $mystr{$c};
            // __________________________________ If there is another open parenthesis ___
            if ($ch == '(') {
                // __________________________________________________ Up for one level ___
                $open_pars++;
                }
            // _______________________________________ If there is a close parenthesis ___
            elseif ($ch == ')') {
                // _______________________________________ If we are on a higher level ___
                if ($open_pars > 0) {
                    // ____________________________________________ Down for one level ___
                    $open_pars--;
                    }
                else {
                    // _____________________________________________________ We got it ___
                    $ret = $c;
                    break;
                    }
                }
            $c++;
            }
        return $ret;

        }


    /**
     * Replace JXSQL table name
     */
    private function replace_JXTAB($sql) {

        // _______________________________ Extract string in the middle of parenthesis ___
        preg_match('/\((.*?)\)/', $sql, $match);
        // _______________________________________________ Split table name from alias ___
        list($tabname, $tabalias) = explode(':', $match[1]);
        // _____________________ If alias was not specified, assume same name as table ___
        if (!$tabalias) {
            $tabalias = $tabname;
            }
        // ___________ If table is defined then it is qualified by table informations  ___
        if ($real_tab_name = o2tab_is_defined($tabname)) {
            // ______________________________________________________ Store alias name ___
            $this->aliases[$tabalias] = $real_tab_name;
            // _____________________________ Table is qualified by table informations  ___
            $qname = o2tab_qname($real_tab_name);
            }
        // _________________________ Else it is qualified by JXSQL-parser informations ___
        else {
            // ______________________________________________________ Store alias name ___
            $this->aliases[$tabalias] = $tabname;
            // _______________________ Table is qualified by JXSQL-parser informations ___
            $qname = o2_gateway::qualify($this->dbengine,
                                         $this->db_phys_name,
                                         o2db_schema($this->databasename),
                                         $tabname);
            }
        return $qname.($tabname != $tabalias ? ' '.$tabalias : '');

        }


    /**
     * Replace JXSQL column name
     */
    private function replace_JXCOL($sql) {

        $sret = '';
        // _______________________________ Extract string in the middle of parenthesis ___
        preg_match('/\((.*?)\)/', $sql, $match);
        if (count($match) > 0) {
            // _____________________________________ Split table name from column name ___
            $stok = explode(':', $match[1]);
            if (count($stok) == 2) {
                // ________________________________________________________ Alias name ___
                $aliasname = $stok[0];
                // _______________________________________________ Logical column name ___
                $colname   = $stok[1];
                // _________________________________________ Get table name from alias ___
                $tabname   = $this->aliases[$aliasname];
                // __________________________________________ Get column physical name ___
                if ($colname != '*') {
                    $col_phys_name = o2field_name($tabname, $colname);
                    /**
                     * Lowercasing O2ASPID column otherwise if used with delimitators
                     * doesn't work in query execution
                     */
                    if ($col_phys_name == 'O2ASPID') {
                        $col_phys_name = 'o2aspid';
                        }
                    // ___________________________ Raise error if column was not found ___
                    elseif ($col_phys_name == '') {
                        $col_phys_name = '--WRONG-COL-NAME--';
                        }
                    $sret = (($tabname != $aliasname) || $this->fully_qualify_columns ?
                             $aliasname.'.' : '').
                            $this->qualifier_o.$col_phys_name.$this->qualifier_c;
                    }
                else {
                    $sret = $aliasname.'.*';
                    }
                }
            // ____________________ Alias is missing: simply add delimiters to content ___
            else {
                $colname = $stok[0];
                $sret    = $this->qualifier_o.$colname.$this->qualifier_c;
                }
            }
        else {
            $sret = '--PARSE_ERROR--';
            }
        return $sret;

        }


    /**
     * Replace JXCONCAT command
     */
    private function replace_JXCONCAT($sql) {

        $concatop = '';
        if ($this->dbengine == 'mssql') {
            $concatop = '+';
            }
        elseif ($this->dbengine == 'mysql') {
            $concatop = ',';
            }
        else {
            $concatop = '||';
            }
        $sret = '';
        // _______________________________ Extract string in the middle of parenthesis ___
        $start_pos      = strlen($this->JXCONCAT);
        $end_par        = $this->find_corresponding_parenthesis($sql, $start_pos);
        $length         = $end_par - $start_pos + 1;
        // ______________________ Get JXSQL function arguments (excluding parenthesis) ___
        $jx_sql_command = substr($sql, $start_pos + 1, $length - 2);
        if (strlen($jx_sql_command) > 0) {
            // _______________________________________________________ Split arguments ___
            $stok = explode(',', $jx_sql_command);
            $i    = 0;
            if ($this->dbengine == 'mysql') {
                $sret = 'CONCAT';
                }
            $sret.= '(';
            $tlen = count($stok);
            while ($i < $tlen) {
                // _________________________________________________________ Get token ___
                $argval = $stok[$i];
                if ($i > 0) {
                    $sret.= ' '.$concatop.' '.$argval;
                    }
                else {
                    $sret.= $argval;
                    }
                $i++;
                }
            $sret.= ')';
            }
        else {
            $sret = '--PARSE_ERROR--';
            }
        return $sret;

        }


    /**
     * Replace JXSUBSTRING command
     */
    private function replace_JXSUBSTRING($sql) {

        $sret = '';
        // _______________________________ Extract string in the middle of parenthesis ___
        $start_pos      = strlen($this->JXSUBSTRING);
        $end_par        = $this->find_corresponding_parenthesis($sql, $start_pos);
        $length         = $end_par - $start_pos + 1;
        // ______________________ Get JXSQL function arguments (excluding parenthesis) ___
        $jx_sql_command = substr($sql, $start_pos + 1, $length - 2);
        if (strlen($jx_sql_command) > 0) {
            // _______________________________________________________ Split arguments ___
            $stok       = explode(',', $jx_sql_command);
            $i          = 0;
            $index_from = 0;
            $index_to   = 0;
            // _____________________________________ SUBSTRING is used for all engines ___
            $sret       = 'SUBSTRING';
            $sret      .= '(';
            $tlen       = count($stok);
            while ($i < $tlen) {
                // _________________________________________________________ Get token ___
                $argval = $stok[$i];
                // _____________________________________ First parameter is the string ___
                if ($i == 0) {
                    $sret.= $argval;
                    }
                // _______________ Second parameter is substring index starting from 1 ___
                elseif ($i == 1) {
                    $index_from = $argval;
                    }
                elseif ($i == 2) {
                    $index_to = $argval;
                    if ($this->dbengine == 'postgres') {
                        $sret.= ' from '.$index_from.
                                ' for '.($index_to - $index_from + 1);
                        }
                    else {
                        $sret.= ','.$index_from.','.$index_to;
                        }
                    }
                $i++;
                }
            $sret.= ')';
            }
        else {
            $sret = '--PARSE_ERROR--';
            }
        return $sret;

        }

    }


/**
 * Function to parse JXSQL command to return an engine specific query.
 *
 * Parameter $fq_columns is used by runtime to fully qualify columns used in custom-where
 * expressions.
 *
 * @param  string  $dbname       Logical database to use as engine for the parser
 * @param  string  $sql          JXSQL query to parse
 * @param  boolean $fq_columns   Fully qualify columns names for custom-wheres
 * @return string
 */
function jxsql_parse($dbname, $sql, $fq_columns = false) {

    $parser = new JXSQLparser($dbname, $fq_columns);
    if (strtolower($parser->dbengine) != 'jxsdb') {
        // _________________________________________ STAGE1: parse all JXTAB functions ___
        $sql    = $parser->parseSQL($sql, 1);
        // _________________________________________ STAGE2: parse all JXCOL functions ___
        $sql    = $parser->parseSQL($sql, 2);
        // __________________________________________ STAGE3: parse other JX functions ___
        $sql    = $parser->parseSQL($sql, 3);
        }
    // _____________________________________________________________ Return parsed SQL ___
    return $sql;

    }


/**
 * Alias for function jxsql_parse()
 *
 */
function jxsql($dbname, $sql, $fq_columns = false) {

    return jxsql_parse($dbname, $sql, $fq_columns);

    }

?>
