<?php
include_once '../lib/jxapp.inc';
// _____________________________________________________________________ Session start ___
session_name($_REQUEST["JXSESSNAME"]);
session_start();
// _____________________________________________ Close sessione to free other requests ___
session_write_close();
// ______________________________________________________ Recover application instance ___
if (isset($_SESSION['o2_app'])) {
    $app = $_SESSION['o2_app'];
    $res = $app->ext_resources;
    if (isset($_REQUEST["RID"])) {
        $res_id = $_REQUEST["RID"];
        }
    else {
        $res_id = 0;
        }
    $file_path = $res[$res_id][0];
    if ($file_path && file_exists($file_path)) {
        if (is_readable($file_path)) {
            $file_size  = filesize($file_path);
            $file_name  = basename($file_path);
            $file_type  = o2_mime_content($file_path);
            $file_disp  = ($res[$res_id][1] ? "attachment" : "inline");
            $file_cache = ($res[$res_id][2] ? true : false);
            // ___________________________________________________________ Set headers ___
            header('Pragma: public');
            header('Content-Type: '.$file_type."; charset=".$app->chr_encoding);
            header('Content-Disposition: '.$file_disp.'; filename="'.$file_name.'"');
            header('Content-Length: '.$file_size);
            // ______________________________________________________ Cache management ___
            if ($file_cache) {
                header('Cache-Control: max-age=3600');
                }
            else {
                header('Expires: 0');
                header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
                header('Cache-Control: private', false);
                }
            // _____________________________________________________________ Send file ___
            ob_clean();
            flush();
            readfile($file_path);
            // ____________________________________________________________ End stream ___
            die();
            }
        else {
            header($_SERVER["SERVER_PROTOCOL"]." 403 Forbidden");
            print "<h4>FORBIDDEN</h4>";
            die();
            }
        }
    else {
        header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found");
        print "<h4>NOT FOUND</h4>";
        die();
        }
    }
else {
    header($_SERVER["SERVER_PROTOCOL"]." 403 Forbidden");
    print "<h4>FORBIDDEN</h4>";
    die();
    }
?>