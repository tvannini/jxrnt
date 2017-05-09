<!DOCTYPE HTML>
<html>
	<head>
		<title>Janox :: info - Runtime details</title>
        <style type="text/css">
        <!--
        @font-face {
            font-family: 'Gudea';
            font-style: normal;
            font-weight: 400;
            src: local('Gudea'), url(css/fonts/gudea.woff) format('woff');
            }
        * { font-family: "Gudea", sans-serif; color: #000000; font-size: 14px; box-sizing: border-box; }
        body {
         color: #666666;
         margin: 0;
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
<body>
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
    o2info_item("version",
                $GLOBALS['o2_runtime']->release());
    o2info_item("root",
                $GLOBALS['o2_runtime']->root->nome_completo);
    o2info_item("web root",
                $GLOBALS['o2_runtime']->alias);
    o2info_item("php executable",
                $GLOBALS['o2_runtime']->php_engine);
    o2info_item("multisession",
               ($GLOBALS['o2_runtime']->multisession ? "On" : "Off"));
    o2info_item("default data chunk",
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
    o2info_item("dbms", $dbms_list);
    o2info_item("services", $services_list);
?>

	</table>
</body>
</html>


