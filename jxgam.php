<?php

/**
 * Janox Global Application Modifier
 * PHP7/8
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
 * This script is a multifunctional modifier for Janox developed application.
 *
 *
 * @name      jxgam
 * @package   janox/bin/jxgam.php
 * @version   2.9
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

$info  = <<<JANOX_SCRIPT_HEAD

               Janox Global Application Modifier


   This file is part of Janox.

    - LICENSE -

   Janox is free software; you can redistribute  it  and/or
   modify it under the  terms  of  the  GNU  Lesser General
   Public License  as  published   by   the   Free Software
   Foundation; either version 3 of the License, or (at your
   option) any later version.

   Janox is distributed in the hope that it will be useful,
   but WITHOUT  ANY  WARRANTY;  without  even  the  implied
   warranty of MERCHANTABILITY or FITNESS FOR A  PARTICULAR
   PURPOSE. See the  GNU Lesser General Public License  for
   more details.

   You should  have  received  a  copy  of  the  GNU Lesser
   General Public License along with this program.
   If not, see <http://www.gnu.org/licenses/>.

    - DESCRIPTION -

   This script provides functions to massively modify Janox
   programs script files, inside an application.

    - USAGE -

   You can run jxgam.php using a working PHP  installation,
   typing at the command line:

     <path_to_php>/php <jxbin>/jxgam.php <app_main_file>
       [ctrl [<type>|all]
        [where:"<prop1_name>=<prop1_value>;
                <prop2_name>=<prop2_value>;..."]
        [set:"<prop1_name>=<prop1_value>;
              <prop2_name>=<prop2_value>;..."]]|
       [exp where:"<search_string>" [set:"<set_string>"]]|
       [text where:"<search_string>" [set:"<set_string>"]]|
       [unused [patch]]|
       [repexp [patch]]

   Where:
    <path_to_php> is the path to  the  PHP  executable,  as
                  needed.
    <jxbin> is the path where this file is located.
    <app_main_file> is the analyzing application main  file
                    full path.
    <type> is the control type (label|edit|listcombo|...)
    <propn_name> & <propn_value> are name and value  for  a
                                 control property.

   First  parameter  is  always  the  main  file  for   the
   application to analyze.

   Second parameter is a word  indicating the  function  to
   execute on application [ctrl|exp|text|unused|repexp].

   ctrl: looks for controls definitions matching criteria:
          <type>|all: matches controls of  type  <type>  or
                      all control types;
          where:"": declares matching criteria for  control
                    properties:  matches  controls   having
                    <propn_name>=<propn_value>;
          set:"": when the set word is  specified,  matched
                  controls  will  be   modified   to   have
                  <propn_name>=<propn_value>, else matching
                  list is returned.

   exp: looks for text inside expressions scripts:
         where:"": declares the string to look for;
         set:"": when the set  word  is  specified,  search
                 text will be replaced with the  set  text,
                 else matching list is returned.

   text: looks for text inside the whole programs scripts:
         where:"": declares the string to look for;
         set:"": when the set  word  is  specified,  search
                 text will be replaced with the  set  text,
                 else matching list is returned.

   unused: looks for unused elements in programs: when  the
           "patch" word is specified, unused elements  will
           be removed, else matching list is returned.

   repexp: looks  for  duplicated  expressions:  when   the
           "patch"   word    is    specified,    duplicated
           expressions will be removed and the first  match
           used for all occurrences, else matching list  is
           returned.


JANOX_SCRIPT_HEAD;

$app_name   = "";
$app_dir    = null;
$result     = array();
$result_txt = "";
$ctrl_types = array("label",
                    "line",
                    "navigator",
                    "button",
                    "edit",
                    "text",
                    "checkbox",
                    "listcombo",
                    "file",
                    "img",
                    "tab",
                    "doc",
                    "html",
                    "multipage");

// _________________________________________ If aplication file is passed as parameter ___
if ($_SERVER['argc'] > 1) {
    // _____________________________________________________ Get application main file ___
    $app_main = realpath($_SERVER['argv'][1]);
    // _____________________________________________ If file exists and it is readable ___
    if (is_file($app_main) && file_exists($app_main) &&
        $main_text = @file_get_contents($app_main)) {
        // _______________________________________ Get application name from main file ___
        $app_name = preg_replace("/\..*$/i", "", basename($app_main));
        // _________________________________________________ Get application directory ___
        $app_dir  = new dir_descriptor(realpath(dirname($app_main).DIRECTORY_SEPARATOR.
                                                "..".DIRECTORY_SEPARATOR));
        print "\n\nJanox Global Application Modifier\n".
                  "=================================\n\nProcessing application ".
              $app_name." (".$app_dir.")\n\n";
        // ----------------------------- ALL STUFF HERE ----------------------------------
        process_commands();
        output_result();
        }
    // _________________________________ If file does not exists or it is not readable ___
    else {
        die("\n\nJanox Global Application Modifier\n".
                "=================================\n\nError opening file ".
            $_SERVER['argv'][1]."\n\n");
        }
    }
// _________________________________________________________ If no parameter is passed ___
else {
    // ___________________________________________ Output formatted JXGAM informations ___
    die("\n\n".$info."\n\n");
    }


/**
 * Filesystem element (file or folder)
 *
 */
class file_descriptor {

    /*     ===== PROPERTIES =========================================================== */
    public $full_name  = ""; /* Complete element name (path/name.extension)             */
    public $short_name = ""; /* Element name without path (name.extension)              */
    public $name       = ""; /* Element base name without path and extension            */
    public $path       = ""; /* Path (for folder is equal to $full_name)                */
    public $ext        = ""; /* Extension                                               */
    public $type       = ""; /* Type [F] = file, [D] = directory                        */
    public $http_mime  = ""; /* Upload files mime type                                  */
    public $mod_date   = ""; /* Last modification date                                  */
    public $mod_time   = ""; /* Last modification time                                  */
    public $size       = 0;  /* File size                                               */


    /**
     * Constructor
     *
     * @param  string  $file
     * @return file_descriptor
     */
    function __construct($file) {

        $this->full_name  = (realpath($file) ? realpath($file) : $file);
        $parts            = pathinfo($this->full_name);
        $this->ext        = (isset($parts['extension']) ? $parts['extension'] : "");
        $this->short_name = $parts['basename'];
        $this->name       = ($this->ext ?
                             substr($this->short_name, 0, - strlen($this->ext) - 1) :
                             $this->short_name);
        $this->path       = $parts['dirname'].DIRECTORY_SEPARATOR;
        if (is_dir($this->full_name)) {
            $this->type = "D";
            }
        else {
            $this->type = "F";
            $this->size = @filesize($this->full_name);
            }
        $timestamp_local = @filemtime($this->full_name);
        $this->mod_date  = date("Ymd", $timestamp_local);
        $this->mod_time  = date("His", $timestamp_local);

        }


    /**
     * Verifica l'esistenza del file
     *
     * @return boolean
     */
    function exists() {

        return file_exists($this->full_name);

        }


    /**
     * Copy filesystem element to passed target.
     *
     * @param  string $target
     * @return boolean
     */
    function copy_to($target) {

        $target = new file_descriptor($target);
        if ($target->type != "D") {
            return copy($this->full_name, $target);
            }
        else {
            return copy($this->full_name, $target.$this->short_name);
            }

        }


   public function __toString() {

       return $this->full_name;

       }

    }


/**
 * Filesystem folder
 *
 */
class dir_descriptor extends file_descriptor {

    /*     ===== PROPERTIES =========================================================== */
    public $match  = "*";   /* Filesystem match criteria                                */


    /**
     * Costruttore
     *
     * @param  string $dir
     * @param  string $match
     * @return dir_descriptor
     */
    function __construct($dir, $match = "*") {

        $this->type       = "D";
        $this->match      = ($match != "" ? $match : "*");
        $dir              = preg_replace("/\\\/", "/", rtrim($dir, " \\/"));
        $dir              = preg_replace("/\/+/", DIRECTORY_SEPARATOR, $dir);
        $parts            = pathinfo($dir);
        $this->ext        = (isset($parts['extension']) ? $parts['extension'] : "");
        $this->short_name = $parts['basename'];
        $this->name       = ($this->ext ?
                             substr($parts['basename'], 0, - strlen($this->ext) - 1) :
                             $parts['basename']);
        $this->path       = $parts['dirname'].DIRECTORY_SEPARATOR;
        $this->full_name  = $dir.DIRECTORY_SEPARATOR;
        $timestamp        = @filemtime($this->full_name);
        $this->mod_date   = date("Ymd", $timestamp);
        $this->mod_time   = date("His", $timestamp);

        }


    /**
     * Crea la cartella e ritorna true se la creazione ha successo
     *
     * @return boolean
     */
    function create() {

        return ($this->exists() || mkdir($this->full_name));

        }


    /**
     * Ritorna l'elenco di tutti gli elementi di filesystem contenuti nella folder e che
     * soddisfano la maschera $match
     *
     * @param  string $match
     * @return array
     */
    function get_elements($match = "") {

        if (!$match) {
            $match = $this->match;
            }
        $elements       = array();
        $elements_names = glob($this->full_name.$match);
        if ($elements_names) {
            foreach ($elements_names as $element) {
                if (is_dir($element)) {
                    $elements[$element] = new dir_descriptor($element);
                    }
                else {
                    $elements[$element] = new file_descriptor($element);
                    }
                }
            }
        return $elements;

        }


    /**
     * Removes folder and all its content
     *
     * @return boolean
     */
    function remove() {

        $res = false;
        if ($this->exists()) {
            $res = $this->make_empty();
            if ($res) {
                $res = @rmdir($this->full_name);
                }
            }
        return $res;

        }


    /**
     * Removes all folder content
     *
     * @return boolean
     */
    function make_empty() {

        clearstatcache();
        $res            = true;
        $elements_names = glob($this->full_name."*");
        if ($elements_names) {
            foreach ($elements_names as $single_element) {
                $element = new file_descriptor($single_element);
                if ($element->type == "D") {
                    $folder = new dir_descriptor($element->full_name.DIRECTORY_SEPARATOR);
                    $res    = $res_val && $folder->remove();
                    unset($folder);
                    }
                else {
                    $res = $res && @unlink($element->full_name);
                    }
                }
            }
        return $res;

        }


    /**
     * Copy filesystem element to passed target.
     *
     * @param  string $target
     * @return boolean
     */
    function copy_to($target) {

        $target = new dir_descriptor($target);
        if ($target->exists()) {
            $target->make_empty();
            }
        else {
            $target->create();
            }
        foreach ($this->get_elements() as $element_name => $element) {
            $element->copy_to($target.$element_name);
            }

        }


    /**
     * Ritorna l'oggetto file_descriptor dell'elemento di filesystem richiesto
     *
     * @param  string $file
     * @return file_descriptor
     */
    function get_element($file) {

        return new file_descriptor($this->full_name.basename($file));

        }


    /**
     * Verifica l'esistenza della cartella (con $file == null) o di $file all'interno
     * della cartella
     *
     * @param  string $file
     * @return boolean
     */
    function exists($file = null) {

        if ($file) {
            return (count($this->get_elements(basename($file))) > 0);
            }
        else {
            return file_exists($this->full_name);
            }

        }

    }


/**
 * Outputs result array
 *
 */
function output_result() {

    global $result;
    $prgs_n    = 0;
    $matches_n = 0;
    foreach ($result as $prg => $matches) {
        $prgs_n++;
        print $prg."\n";
        foreach ($matches as $match) {
            $matches_n++;
            print " - ".$match."\n";
            }
        }
    print "\n".$matches_n." matches found in ".$prgs_n." programs.\n\n";

    }


/**
 * Processes command string and calls requested functions
 *
 */
function process_commands() {

    if ($_SERVER['argv'][2]) {
        switch ($_SERVER['argv'][2]) {
            case "ctrl":
                loop_all("ctrl", ctrl_params());
                break;
            case "exp":
                die("\nSorry, command ".$_SERVER['argv'][2]." not implemented yet.\n\n");
                break;
            case "text":
                die("\nSorry, command ".$_SERVER['argv'][2]." not implemented yet.\n\n");
                break;
            case "unused":
                die("\nSorry, command ".$_SERVER['argv'][2]." not implemented yet.\n\n");
                break;
            case "repexp":
                die("\nSorry, command ".$_SERVER['argv'][2]." not implemented yet.\n\n");
                break;
            default:
                die("\nSorry, command ".$_SERVER['argv'][2]." not implemented yet.\n\n");
                break;
            }
        }
    else {
        die("\nError: missing command\nUse ctrl, exp, text, unused or repexp\n\n");
        }

    }


/**
 * Processes and executes "ctrl" command and parameters
 *
 * @return array
 */
function ctrl_params() {

    global $ctrl_types;
    $ctrl_type = "all";
    $where_str = "";
    $set_str   = "";
    $where     = array();
    $set       = array();
    // _______________________ Check all parameters to get TYPE, WHERE and SET strings ___
    for ($i = $_SERVER['argc'] - 1; $i > 2; $i--) {
        $word = $_SERVER['argv'][$i];
        if (in_array(strtolower($word), $ctrl_types) || strtolower($word) == "all") {
            $ctrl_type = strtolower($word);
            }
        elseif (substr(strtolower($word), 0, 6) == "where:") {
            $where_str = substr($word, 6);
            }
        elseif (substr(strtolower($word), 0, 4) == "set:") {
            $set_str = substr($word, 4);
            }
        else {
            die("\nError: unknown parameter ".$word."\n\n");
            }
        }
    // _________________________________________________________ Parse WHERE condition ___
    $prop  = "";
    $value = "";
    foreach (explode(';', $where_str) as $prop_cond) {
        if ($prop_cond) {
            list($prop, $value) = explode("=", $prop_cond);
            $where[$prop]       = $value;
            }
        }
    // ____________________________________________________________ Parse SET commands ___
    $prop  = "";
    $value = "";
    foreach (explode(';', $set_str) as $prop_cond) {
        if ($prop_cond) {
            list($prop, $value) = explode("=", $prop_cond);
            $set[$prop]         = $value;
            }
        }
    return array("type" => $ctrl_type, "where" => $where, "set" => $set);

    }


/**
 * Loops on application folder and subfolders, processes files and remove interface
 * working files (cds, __SUORCE__, o2bak, ...)
 *
 * @param string $cmd      Command string [ctrl|exp|text|...]
 * @param array  $params   Matching, setting and patching criteria as needed by function
 */
function loop_all($cmd, $params) {

    global $app_dir;
    $dir   = new dir_descriptor($app_dir."prgs/");
    $files = $dir->get_elements();
    // _____________________________________________________ Loop on folder files list ___
    while ($file = array_shift($files)) {
        // ________________________________________________________ Manage sub folders ___
        if ($file->type == "D") {
            // _______________________________________ Remove "__source__" directories ___
            if ($file->short_name == "__source__") {
//                $file->remove();
                }
            // ________________________________________________ Add all others to list ___
            else {
                $files+= $file->get_elements();
                }
            }
        // ____________________________________________________ Remove interface files ___
        elseif ($file->ext == "cds" || $file->ext == "o2bak" || $file->ext == "cache") {
//            @unlink($file->full_name);
            }
        // ____________________________________________ Get PRF content and process it ___
        elseif ($file->ext == "prf") {
            $prf = $file->full_name;
            if (is_readable($prf)) {
                $prf_txt  = file_get_contents($prf);
                $prf_file = (substr($file->path, -5, -1) !== "prgs" ?
                             substr($file->path,
                                    strrpos($file->path, DIRECTORY_SEPARATOR, -2) + 1,
                                    -1).DIRECTORY_SEPARATOR :
                             "").$file->name;
                // _______________________________________ Process PRF text by command ___
                switch ($cmd) {
                    case "ctrl":
                        // ________________________________ Filter controls in program ___
                        $is_in = look_for_ctrls($prf_file,
                                                $prf_txt,
                                                $params["type"],
                                                $params["where"]);
                        // __________________________________ Modify filtered controls ___
                        if (count($params["set"]) && $is_in) {
                            $prf_txt = patch_ctrls($prf_file, $prf_txt, $params["set"]);
                            file_put_contents($prf, $prf_txt);
                            }
                        break;
                    }
                }
            else {
                print "\nError: can't read file ".$prf."\n\n";
                }
            }

        }

    }


/**
 * Performes researches on PRF content to get controls list matching criteria
 *
 * @param  string $prf_name   Program name
 * @param  string $prf_txt    PRF content
 * @param  string $type       Controls type filter
 * @param  array  $where      Controls matching criteria
 * @return boolean
 */
function look_for_ctrls($prf_name, $prf_txt, $type, $where) {

    global $result;
    global $result_txt;
    $parts = array();
    // _________________________ Get all controls defined in program, filtered by type ___
    preg_match_all('/\-\>ctrldef\("(\w*)",\s*"('.($type == "all" ? '\w*' : $type).')",/',
                   $prf_txt, $parts);
    foreach ($parts[1] as $index => $ctrl_name) {
        $by_prop = true;
        // ______________________________________________ Filter by control properties ___
        foreach ($where as $prop => $value) {
            $subparts = array();
            preg_match('/\$ctrl_'.$ctrl_name.'\-\>'.$prop.'\(["\']?'.
                       preg_quote($value, "/").'["\']?\)/i',
                       $prf_txt, $subparts);
            if (!count($subparts)) {
                $by_prop = false;
                break;
                }
            }
        if ($by_prop) {
            $result[$prf_name][] = $ctrl_name;
            }
        }
    return isset($result[$prf_name]);

    }


/**
 * Performes substitutions in controls definitions
 *
 * @param  string $prf_name   Program name
 * @param  string $prf_txt    PRF content
 * @param  array  $set        Controls properties changing criteria
 * @return string
 */
function patch_ctrls($prf_name, $prf_txt, $set) {

    global $result;
    global $result_txt;
    if ($result[$prf_name]) {
        foreach ($result[$prf_name] as $ctrl_name) {
            foreach ($set as $prop => $value) {
                $parts = array();
                // __________________________________ Check for quoted values (string) ___
                preg_match_all('/\$ctrl_'.$ctrl_name.'\-\>'.$prop.
                               '\((["\']?).*\);/i', $prf_txt, $parts);
                $quoted  = (trim($parts[1][0]) ? "$1" : "");
                $prf_txt = preg_replace('/\$ctrl_'.$ctrl_name.'\-\>'.$prop.
                                        '\((["\']?).*\);/i',
                                        "\$ctrl_".$ctrl_name."->".
                                        $prop."(".$quoted.$value.$quoted.");",
                                        $prf_txt);
                }
            }
        }
    return $prf_txt;

    }

?>
