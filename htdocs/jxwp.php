<?php

/**
 * Janox WEB Print Server
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
 * This script contains Janox WEB Print Server and it returns the rusult file (PDF) for a
 * resources request.
 *
 * @name      jxwp
 * @package   janox/htdocs/jxwp.php
 * @version   2.3
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */


/**
 * Workin directory
 *
 * @var string   Existing folder where JXWP can work in
 */
$wd = "/var/tmp/tmp_jxwp/";

/**
 * Print executable
 *
 * @var string   Full path to Report Manager executable (note extension for OS)
 */
$pe = "/var/reporter/printreptopdf.sh";

/**
 * Execution log to file
 *
 * @var boolean
 */
$log = true;


// _______________________________________________________________ Running mode: Local ___
if (defined("STDIN")) {
    $text = "\n   Janox Print Service\n   ~~~~~~~~~~~~~~~~~~~\n".
            "   This URL is a service for the Janox Runtime.\n".
            "   Visit www.janox.it or mail to info@janox.it.\n\n";
    die($text);
    }
// _________________________________________________________ Running mode: WEB Service ___
else {
    wplog("START WEB SESSION in ".$wd);
    // __________________________________________ Create unique hash working directory ___
    $hwd = create_wd($wd);
    // ________________________________________________________________ Decode request ___
    foreach ($_REQUEST as $key => $data) {
        switch ($key) {
            case "__jxhm":
                // _________________________________ Get hash marker for relative path ___
                $hm = $data;
                wplog("Got hash marker ".$hm);
                break;
            case "__jxpc":
                // _________________________________________________ Get print command ___
                $cmd = $data;
                wplog("Got print command ".$cmd);
                break;
            case "__jxsrc":
                // ___________________________________ Get source (model) file content ___
                $src = $data;
                wplog("Got source model");
                break;
            case "__jxres":
                // _____________________________________ Result file (PDF) locqal path ___
                $res = $hwd.$data;
                wplog("Got result file ".$res);
                break;
            default:
                // __________ Dots "." are replaced with "|" to preserve throw request ___
                $key = str_replace("|", ".", $key);
                // ____________________________________________ Create single XML file ___
                file_put_contents($hwd.$key, $data);
                wplog("Got resource file ".$key);
                break;
            }
        }
    // _____________________________________________________ Called from Janox runtime ___
    if ($hm) {
        // ________________________________________________ Create source (model) file ___
        file_put_contents($hwd."jxwp_src", str_ireplace($hm, $hwd, $src));
        wplog("Source model recreated as jxwp_src");
        // ______________ Execute print command (replace local path in command string) ___
        $cmd = $pe." ".str_ireplace($hm, $hwd, $cmd);
        system($cmd);
        wplog("Print command executed: ".$cmd);
        // _________________________________________ Send produced file back to client ___
        send_file($res);
        wplog("Result file sent back");
        // ________________________________________________ Remove hash working folder ___
        remove_wd($hwd);
        wplog("Working directory removed: ".$hwd);
        }
    // _____________________________________________ Called from else where (browser?) ___
    else {
        $site = "http://www.janox.it";
        $mail = "mailto:info@janox.it";
        $text = "<h1>Janox Print Service</h1>".
                "<p>This URL is a service for the Janox Runtime.</p>".
                "<p>Visit <a href=\"".$site."\">www.janox.it</a> or mail to <a href=\"".
                $mail."\">info@janox.it</a>.</p>";
        print '<html><body style="font-family:Arial"><center><br><br><br>'.$text.
              '</center></body></html>';
        }
    }
wplog("~~~~~~~~~~ END ~~~~~~~~~~");


/**
 * Create unique hash directory to work in
 *
 * @param  string $wd   Global working directory
 * @return string
 */
function create_wd($wd) {

    $hwd = $wd."tmp".mt_rand(1111111, 9999999);
    clearstatcache();
    if (file_exists($hwd)) {
        return create_wd($wd);
        }
    else {
        mkdir($hwd);
        wplog("Working directory created: ".$hwd.DIRECTORY_SEPARATOR);
        return $hwd.DIRECTORY_SEPARATOR;
        }

    }


/**
 * Removes working directory and all its content
 *
 * @param  string   Full path to folder
 * @return boolean
 */
function remove_wd($hwd) {

    $res_val  = true;
    $elements = array_diff(scandir($hwd), Array(".", ".."));
    if ($elements) {
        foreach ($elements as $element) {
            if (is_dir($hwd.$element)) {
                $res_val = $res_val && remove_wd($hwd.$element);
                wplog("Sub-folder removed: ".$element);
                }
            else {
                $res_val = $res_val && unlink($hwd.$element);
                wplog("File removed: ".$element);
                }
            }
        }
    if ($res_val) {
        $res_val = rmdir($hwd);
        wplog("Folder removed: ".$element);
        }
    return $res_val;

    }


/**
 * Send resource file as a stream
 *
 * @param string $res   File to send
 */
function send_file($res) {

    if ($res && file_exists($res)) {
        if (is_readable($res)) {
            // _____________________________________________________________ Send file ___
            ob_clean();
            flush();
            readfile($res);
            // ____________________________________________________________ End stream ___
            wplog("File correctly sent back");
            }
        else {
            header($_SERVER["SERVER_PROTOCOL"]." 403 Forbidden");
            print "<h4>FORBIDDEN</h4>";
            wplog("--ERROR: result file is not readable");
            die();
            }
        }
    else {
        header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found");
        print "<h4>NOT FOUND</h4>";
        wplog("--ERROR: can't find result file");
        die();
        }

    }


/**
 * Log $log_txt to file
 *
 */
function wplog($log_txt) {

    global $log;
    global $wd;
    if ($log) {
        $f = fopen($wd."jxwp.log", 'a');
        fwrite($f, date("Y.m.d H:i:s> ").$log_txt."\n");
        fclose($f);
        }

    }

?>