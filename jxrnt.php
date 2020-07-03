<?php

/**
 * Janox Runtime Script
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
 * This script is the Janox runtime reference file.
 * Here Janox runtime is started. This script can be called directly.
 *
 *
 * @name      jxrnt
 * @package   janox/jxrnt.php
 * @version   2.6
 * @copyright Tommaso Vannini (tvannini@janox.it) 2007
 * @author    Tommaso Vannini (tvannini@janox.it)
 */

/**
 * Janox release string
 *
 * @global string $jxrel
 */
$jxrel = "2.6.02";

/**
 * Janox built date string
 *
 * @global string $jxbuilt
 */
$jxbuilt = "20200703";

/**
 * Start execution time
 *
 * @global float $jxtime
 */
$jxtime = microtime(true);

/**
 * Script directly passed to PHP engine
 *
 * @global string $jxsrcname
 */
$jxsrcname = basename($_SERVER['argv'][0]);

/**
 * If runtime script is called directly from command-line.
 *
 * @global boolean $jxdirect
 */
$jxdirect = ($_SERVER['argc'] > 0 && $jxsrcname == basename(__FILE__));

// _____________________________________________________ Janox runtime for application ___
include_once 'lib/jxapp.inc';

?>
