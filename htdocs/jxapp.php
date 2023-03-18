<?php

/**
 * Janox Application Main Script
 * PHP5
 *
 * This PHP script  file  is  part  of  an  application  realized  under
 * Janox architecture, the full cross-tech development suite.
 *
 * For  learning  how  to  develop   full  featured,  everywhere-running
 * applications and other informations please refer to
 * www.janox.it or mail to info@janox.it.
 *
 * This application can be run as a Server Side  Script,  within  a  WEB
 * Server, and surfed with a standard browser.
 *
 * For executing this application you need  to  refer  to  an  installed
 * Janox Runtime script (jxrnt/jxrnt.php).  That can be  done  in  three
 * different ways, evaluated in this order:
 *  - (Only BATCH  execution)  Passing  the  [jxrnt]  parameter  at  the
 *    command line, in the form:
 *       php.exe this_script.php jxrnt=path/to/jxrnt.php
 *  - Setting the value of the variable $jxrnt below in this script
 *  - Setting the environment variable [JXRNT]
 */

// _________________ Use this variable to set Janox Runtime location ___
$jxrnt = "";


// __________________________________ Command line parameter [jxrnt] ___
if ($_SERVER["argc"] > 1) {
 foreach ($_SERVER["argv"] as $single_param) {
  list($par_name, $par_val) = explode("=", $single_param, 2);
  if ($par_name == "jxrnt") (@include $par_val) ||
                             die("Wrong Janox Runtime ".$par_val."\n");
  }
 }
// __ $o2_runtime is defined when called from inside a Janox context ___
if (!$o2_runtime) {
 // ___________________________________ Script coded variable $jxrnt ___
 if ($jxrnt) (@include $jxrnt) || die("Wrong Janox Runtime ".$jxrnt."\n");
 // ___________________________________ Environment variable [jxrnt] ___
 elseif (getenv("JXRNT")) (@include getenv("JXRNT")) ||
                           die("Wrong Janox Runtime ".getenv("JXRNT")."\n");
 // __________________________________________ Missing Janox Runtime ___
 else die("Missing Janox Runtime\n");
 }


// ______________________________________ Application instance start ___
o2def::app("2.9");

?>
