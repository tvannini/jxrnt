<?php

/*
    Expected parameters:
    dbtype = database engine (blank for SQLite)
    dbname = database server name (host name or IP)
    dbuser = database user ID
    dbpswd = database user password
    server = server name
    srname = service name
    srengs = number of engines per service
    action = action to execute (start, stop, end)
*/
chdir(dirname(__FILE__));
include_once(dirname(__FILE__).DIRECTORY_SEPARATOR.
             "..".DIRECTORY_SEPARATOR.
             "..".DIRECTORY_SEPARATOR.
             "jxrnt.php");

$params_local = array();
foreach ($_SERVER['argv'] as $index => $single_arg) {
    list($key_local, $val_local) = explode("=", $single_arg, 2);
    if ($index) {
        $params_local[strtolower($key_local)] = $val_local;
        }
    }

$params_local['dbtype'] = ($params_local['dbtype'] ? $params_local['dbtype'] : "sqlite3");
$_SESSION['o2_app']     = new o2_app($GLOBALS['o2_runtime']->release);
$_SESSION['o2_app']->db['o2_sysdb']->server->type     = $params_local['dbtype'];
$_SESSION['o2_app']->db['o2_sysdb']->server->server   = $params_local['dbname'];
$_SESSION['o2_app']->db['o2_sysdb']->server->user     = $params_local['dbuser'];
$_SESSION['o2_app']->db['o2_sysdb']->server->password = $params_local['dbpswd'];
$_SESSION['o2_app']->load_gateways();

switch ($params_local['action']) {
    case "stop":
        $action_local = "S";
        break;
    case "end":
        $action_local = "E";
        break;
    default:
        $action_local = "A";
        break;
    }
$return_params = $_SESSION['o2_app']->intcall("tools/o2sys_set_service_status",
                                             $params_local['srname'],
                                             $params_local['server'],
                                             $action_local,
                                             $params_local['srengs']);

?>
