<?php

/**
 * Janox Deployment Script
 * PHP5
 *
 * See $info variable for details.
 *
 *
 * @name      jxhttpd
 * @package   janox/bin/jxhttpd.php
 * @version   2.3
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

$jxrel = "2.3.00";
$info  = <<<JANOX_SCRIPT_HEAD

                    Janox Deployment Tool


   This file is part of Janox.

    - LICENSE -

   Janox is free software; you can redistribute  it  and/or
   modify it under  the  terms  of  the  GNU Lesser General
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

   This script contains utilities  for  Janox  runtime  and
   applications deployment, in an Apache WEB Server:
    * Runtime alias creation
    * Runtime  settings file  (janox.ini)  modification for
      adding PHP executable file and runtime WEB alias
    * Application alias creation
    * Application   main   script   (/htdocs/<app_name>.php)
      modification for correct runtime inclusion

    - USAGE -

   You can run jxhttpd.php using a working PHP installation,
   typing at the command line:

     <path_to_php>/php <jxroot>/jxhttpd.php <params>

   where <params> is a list of  parameter/values  pairs,  in
   the form "par1=value1 par2=value2 ...".  Valid parameters
   are:

    conf=<httpdconf>
       This is the Apache configuration  file  (httpd.conf).
       This parameter is needed  for  creating  runtime  and
       applications WEB alias:  if  it  is  not passed  only
       Janox settings file ("janox.ini") will be changed.
    php=<phpexe>
       This is the PHP executable file name (with full path,
       if needed).  If this parameter is passed, information
       is stored in the janox.ini file.
    rnt=<jxrnt>
       This is the Janox runtime core  script  ("jxrnt.php")
       file name  with  full  path.  If  this  parameter  is
       passed, its value is updated in "janox.ini" file  and
       in the deployed application main script.
    alias=<jxalias>
       This is the name to set to Janox runtime WEB alias on
       the local server,  leading to  Janox WEB root  folder
       ("<janox>/jxrnt/htdocs/").  Janox WEB resources  will
       be reachable on "http://localhost/<jxalias>/".
       If "rnt" parameter is not  passed  as  well,  runtime
       alias will not be created.
    app=<appscript>
       This  parameter  is  used  for  deploying   a   Janox
       application. Passed value is the fully qualified main
       script file  ("<app_root>/htdocs/<app_name>.php")  of
       the  application  you  want  to  deploy.   When  this
       parameter is passed, a WEB alias is created, with the
       same name of the application, leading to applicaition
       WEB root folder ("<app_root>/htdocs/").
       If "rnt" parameter is  passed  as  well,  application
       main script will be updated to  include  the  correct
       Janox runtime.

    NOTE: Spaces  are boundary  for  parameter/value  pairs:
          if  you  need   to   pass   values   with   spaces
          double-quote the whole pair, example:
          ... jxhttpd.php "rnt=/path to my/jxrnt/jxrnt.php"

JANOX_SCRIPT_HEAD;

// _________________________________________ If aplication file is passed as parameter ___
if ($_SERVER['argc'] > 1) {
    // __________________________________________________________ Set global variables ___
    $return_txt = "                    Janox Deployment Tool\n";
    // ______________________________________________ Retrieve command line parameters ___
    $params = get_cmd_params($return_txt);
    // _____________________________________________________________ Missing arguments ___
    if (!isset($params['conf']) && !isset($params['rnt'])) {
        die("\n\n".$info."\n".str_repeat("=", 60).$return_txt.
            "\n\n   ERROR: Missing arguments\n\n");
        }
    else {
        // __________________________________________________________ Change janox.ini ___
        if (isset($params['rnt'])) {
            if (isset($params['php']) || isset($params['alias'])) {
                $jxdir = new dir_descriptor(dirname($params['rnt']).DIRECTORY_SEPARATOR);
                if ($jxdir->exists("janox.ini")) {
                    $return_txt.= change_ini($jxdir."janox.ini",
                                             $params['php'],
                                             $params['alias']);
                    }
                // _________________________________________________ Missing janox.ini ___
                else {
                    die("\n\n   ERROR: Cannot find janox.ini in ".$jxdir."\n\n");
                    }
                }
            if (isset($params['app'])) {
                $jxrnt      = new file_descriptor($params['rnt']);
                $appscript  = new file_descriptor($params['app']);
                $return_txt.= change_app_script($appscript->full_name, $jxrnt->full_name);
                }
            }
        // _________________________________________________________ Change httpd.conf ___
        if (isset($params['conf']) &&
            (isset($params['alias']) || isset($params['app']))) {
            $httpdconf = new file_descriptor($params['conf']);
            if ($httpdconf->exists()) {
                $jxalias   = false;
                $jxhtdocs  = false;
                $appalias  = false;
                $apphtdocs = false;
                if (isset($params['rnt']) && isset($params['alias'])) {
                    $jxdir    = new dir_descriptor(dirname($params['rnt']).
                                                   DIRECTORY_SEPARATOR);
                    $jxalias  = $params['alias'];
                    $jxhtdocs = $jxdir."htdocs".DIRECTORY_SEPARATOR;
                    }
                if (isset($params['app'])) {
                    $appscript = new file_descriptor($params['app']);
                    $appalias  = $appscript->name;
                    $apphtdocs = $appscript->path;
                    }
                $return_txt.= change_conf($httpdconf,
                                          $jxalias,
                                          $jxhtdocs,
                                          $appalias,
                                          $apphtdocs);
                }
            // ____________________________________________________ Missing httpd.conf ___
            else {
                die("\n\n   ERROR: Cannot find ".$httpdconf."\n\n");
                }
            }
        }
    print $return_txt;
    }
// _________________________________________________________ If no parameter is passed ___
else {
    // __________________________________________ Output formatted JXConv informations ___
    die("\n\n".$info."\n\n");
    }


/**
 * Retrieves and returns command line parameters
 *
 */
function get_cmd_params(&$return_txt) {

    $console_txt = "";
    $cmdline_params = array();
    for ($par_index = 1; $par_index < $_SERVER['argc']; $par_index++) {
        $single_par = $_SERVER['argv'][$par_index];
        if (strpos($single_par, "=")) {
            list ($par_name, $par_value) = explode("=", $single_par, 2);
            }
        else {
            $par_name  = $single_par;
            $par_value = true;
            }
        $console_txt.= "   ".strtolower($par_name)." = ".$par_value."\n";
        $cmdline_params[strtolower($par_name)] = $par_value;
        }
    if ($console_txt) {
        $return_txt.= "Input parameters:\n".$console_txt."\n";
        }
    return $cmdline_params;

    }


/**
 * Changes Janox settings file (janox.ini)
 *
 */
function change_ini($ini_file, $php_exe, $jxalias = "") {

    $return_txt  = "";
    $ini_content = file_get_contents($ini_file);
    if ($jxalias) {
        $jxalias = "/".trim(trim($jxalias, "/"))."/";
        if (preg_match('/^\s*o2_alias\s*=/m', $ini_content)) {
            $ini_content = preg_replace('/^\s*o2_alias.*$/m',
                                        'o2_alias = "'.$jxalias.'"', $ini_content);
            $return_txt.= "Janox runtime alias updated in janox.ini\n";
            }
        else {
            $ini_content.= "\no2_alias = \"".$jxalias."\"\n";
            $return_txt .= "Janox runtime alias added in janox.ini\n";
            }
        }
    if ($php_exe) {
        if (preg_match('/^\s*php_exe\s*=/m', $ini_content)) {
            $ini_content = preg_replace('/^\s*php_exe.*$/m',
                                        'php_exe = "'.$php_exe.'"', $ini_content);
            $return_txt.= "PHP executable updated in janox.ini\n";
            }
        else {
            $ini_content.= "\nphp_exe = \"".$php_exe."\"\n";
            $return_txt .= "PHP executable added in janox.ini\n";
            }
        }
    if (!@rename($ini_file, $ini_file.".".date("Y_m_d-H_i_s").".save") ||
        !@file_put_contents($ini_file, $ini_content)) {
        die("\n\n   ERROR: Cannot write to ".$ini_file."\n\n");
        }
    return $return_txt;

    }


/**
 * Changes Apache configuration file (httpd.conf)
 *
 */
function change_conf($conf_file, $jxalias, $jxhtdocs, $appalias = "", $apphtdocs = "") {

    $return_txt   = "";
    $changed      = false;
    $conf_content = file_get_contents($conf_file);
    if ($jxalias) {
        $jxalias  = trim(trim($jxalias, "/"));
        $jxhtdocs = str_replace("\\", "/", $jxhtdocs);
        if (preg_match('/^\s*Alias\s*\/'.$jxalias.'\s/im', $conf_content)) {
            $return_txt.= "Alias \"".$jxalias.
                          "\" already present in httpd.conf: left unchanged.\n";
            }
        else {
            $conf_content.= "\nAlias /".$jxalias." \"".$jxhtdocs."\"\n".
                            " <Directory \"".rtrim($jxhtdocs, "/")."\">\n".
                            "  Options Indexes MultiViews\n".
                            "  AllowOverride None\n".
                            "  Order allow,deny\n".
                            "  Allow from all\n".
                            " </Directory>\n";
            $return_txt  .= "Janox runtime alias \"".$jxalias."\" added in httpd.conf\n";
            $changed      = true;
            }
        }
    if ($appalias) {
        $appalias  = trim(trim($appalias, "/"));
        $apphtdocs = str_replace("\\", "/", $apphtdocs);
        if (preg_match('/^\s*Alias\s*\/'.$appalias.'\s/im', $conf_content)) {
            $return_txt.= "Alias \"".$appalias.
                          "\" already present in httpd.conf: left unchanged.\n";
            }
        else {
            $conf_content.= "\nAlias /".$appalias." \"".$apphtdocs."\"\n".
                            " <Directory \"".rtrim($apphtdocs, "/")."\">\n".
                            "  Options Indexes MultiViews\n".
                            "  AllowOverride None\n".
                            "  Order allow,deny\n".
                            "  Allow from all\n".
                            " </Directory>\n";
            $return_txt  .= "Application alias \"".$appalias."\" added in httpd.conf\n";
            $changed      = true;
            }
        }
    if ($changed) {
        if (!@rename($conf_file, $conf_file.".".date("Y_m_d-H_i_s").".save") ||
            !@file_put_contents($conf_file, $conf_content)) {
            die("\n\n   ERROR: Cannot write to ".$conf_file."\n\n");
            }
        $return_txt.= "*** Apache WEB Server restart is needed! ***\n";
        }
    return $return_txt;

    }


/**
 * Changes Application main file (<app_root>/htdocs/<app_name>.php) for including the
 * correct Janox runtime.
 *
 */
function change_app_script($app_script, $jxrnt) {

    $return_txt     = "";
    $script_content = file_get_contents($app_script);

    $script_content = preg_replace('/^\s*\$jxrnt\s*=.*$/m',
                                   '$jxrnt = "'.$jxrnt.'";',
                                   $script_content);
    $return_txt.= "Janox runtime updated in ".$app_script."\n";
    if (!@rename($app_script, $app_script.".".date("Y_m_d-H_i_s").".save") ||
        !@file_put_contents($app_script, $script_content)) {
        die("\n\n   ERROR: Cannot write to ".$app_script."\n\n");
        }
    return $return_txt;

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
     * @param string  $file
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
     * @return o2_dir
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
                    $elements[basename($element)] = new dir_descriptor($element);
                    }
                else {
                    $elements[basename($element)] = new file_descriptor($element);
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

        $res_val = false;
        if ($this->exists()) {
            $res_val = $this->make_empty();
            if ($res_val) {
                $res_val      = @rmdir($this->full_name);
                }
            }
        return $res_val;

        }


    /**
     * Removes all folder content
     *
     * @return boolean
     */
    function make_empty() {

        clearstatcache();
        $res_val              = true;
        $elements_names_local = glob($this->full_name."*");
        if ($elements_names_local) {
            foreach ($elements_names_local as $single_element) {
                $element_local = new file_descriptor($single_element);
                if ($element_local->type == "D") {
                    $folder_local = new dir_descriptor($element_local->full_name.
                                                       DIRECTORY_SEPARATOR);
                    $res_val      = $res_val && $folder_local->remove();
                    unset($folder_local);
                    }
                else {
                    $res_val = $res_val && @unlink($element_local->full_name);
                    }
                }
            }
        return $res_val;

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
     * Ritorna l'oggetto o2_fsitem dell'elemento di filesystem richiesto
     *
     * @param  string $file
     * @return o2_elemento_fs
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


?>
