<!DOCTYPE HTML>
<html>
	<head>
		<title>Janox :: info - Runtime details</title>
        <link href="index/index.css" rel="stylesheet" type="text/css">
        <style type="text/css">
        <!--
        * { font-family: "Gudea", sans-serif; color: #606060; font-size: 14px; box-sizing: border-box; }
        body {
         background-color: #fefefe;
         margin: 0;
         }
        .main_resize {
         border: none;
         max-width: 100%;
         }
        .label {
         color: #666666;
         padding-right: 1em;
         }
        .infodata {
         color: #414141;
         background-color: #f5f5f5;
         font: normal 0.78em/1.5em "Courier New", mono;
         border: none;
         padding: 0 2em 0 2em;
         }
         -->
        </style>
</head>
<body class="main_resize">
<br />
<?php
    include(dirname(__FILE__)."/../jxrnt.php");

    function o2info_item($label, $value) {

        print "<tr>\n".
              " <td class=\"label\">".$label.":</td>\n".
			  " <td class=\"infodata\">".
			  $value.
			  " </td>\n</tr>\n";

        }

     // ____________________________________________________________________ Test zone ___
     // __________________________________________________________________________________

?>

	<table align="center">

<?php
    o2info_item("Version",
                $GLOBALS['o2_runtime']->release());
    o2info_item("Root",
                $GLOBALS['o2_runtime']->root->nome_completo);
    o2info_item("WEB root",
                $GLOBALS['o2_runtime']->alias);
    o2info_item("PHP executable",
                $GLOBALS['o2_runtime']->php_engine);
    o2info_item("Multisession",
               ($GLOBALS['o2_runtime']->multisession ? "On" : "Off"));
    o2info_item("Default data chunk",
                number_format($GLOBALS['o2_runtime']->def_datachunk, 0, "", ".").
                " bytes");
    $dbms_list        = "";
    $present_gateways = array();
    $folder_local     = new o2_dir("../lib/dbms/");
    foreach ($folder_local->all_elements() as $fs_element) {
	   if (substr($fs_element->nome, 0, 5) == "jxdb_" &&
	              $fs_element->ext == "inc") {
		  $dbms_list.= substr($fs_element->nome, 5)."<br>";
		  }
	   }
    o2info_item("Dbms", $dbms_list);
?>

	</table>
</body>
</html>


