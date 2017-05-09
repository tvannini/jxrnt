<?php

if (!file_exists("../jxrnt.php")) {
    $o2db_result = "<tr><td><i>Sorry, can't find Janox runtime.</i></td></tr>";
    $o2db_error  = true;
    }
else {
    include_once '../jxrnt.php';
    $o2db_type     = $_POST['o2dbadm_type'];
    $o2db_server   = stripcslashes($_POST['o2dbadm_server']);
    $o2db_user     = $_POST['o2dbadm_user'];
    $o2db_password = $_POST['o2dbadm_password'];
    $o2db_query    = stripslashes($_POST['o2dbadm_command']);
    $o2db_result   = "";
    $o2db_error    = false;
    $o2db_database = $_POST['o2dbadm_database'];
    $o2db_owner    = $_POST['o2dbadm_owner'];
    $o2db_table    = $_POST['o2dbadm_table'];
    $tables_list   = array();
    // __________________________________________________ Look up for present gateways ___
    $present_gateways = array();
    $folder_local     = new o2_dir("../lib/dbms/");
    foreach ($folder_local->all_elements() as $fs_element) {
        if (substr($fs_element->nome, 0, 5) == "jxdb_" &&
            $fs_element->ext == "inc") {
            $gateway_name = substr($fs_element->nome, 5);
            $present_gateways[$gateway_name] = ($gateway_name == $o2db_type);
            }
        }
    if (isset($_POST['o2dbadm_go'])) {
        if (!$_POST['o2dbadm_type']) {
            $o2db_result = "<tr><td><i>Please, select server type.</i></td></tr>";
            $o2db_error  = true;
            }
        else {
            if (!file_exists("../lib/dbms/jxdb_".$o2db_type.".inc")) {
                $o2db_result = "<tr><td><i>Sorry, can't find</i> <code>".
                               $o2db_type."</code> <i>gateway.</i></td></tr>";
                $o2db_error  = true;
                }
            else {
                include_once "../lib/dbms/jxdb_".$o2db_type.".inc";
                $connection_local = o2_gateway::connect($o2db_type,
                                                        $o2db_server,
                                                        $o2db_user,
                                                        $o2db_password);
                if (!$connection_local) {
                    $o2db_result = "<tr><td><i>Can't connect to the server!</i></td></tr>";
                    $o2db_error  = true;
                    }
                else {
                    // I look up for database tables
                    $tables_list = o2_gateway::tables($o2db_type,
                                                      $o2db_server,
                                                      $o2db_user,
                                                      $o2db_password,
                                                      $o2db_database,
                                                      $o2db_owner);
                    if (!$o2db_query) {
                        $o2db_result = "<tr><td class='res_col'>".
                                       "<i>Successfully connected</i></td></tr>";
                        }
                    else {
                        $exe_local = o2_gateway::execute($o2db_type,
                                                         $o2db_query,
                                                         $o2db_server,
                                                         $o2db_user,
                                                         $o2db_password);
                        if (isset($exe_local['o2error'])) {
                            $o2db_result = "<tr><td><i>Query error</i>. ".
                                           "Database returned:<br><code>".
                                           $exe_local['o2error']."</code></td></tr>";
                            $o2db_error  = true;
                            }
                        else {
						    o2_gateway::commit($o2db_type,
                                               $o2db_server,
                                               $o2db_user,
                                               $o2db_password,
                                               true);
                            $result_titles  = "";
                            $result_counter = 0;
                            $o2db_result    = "";
                            if (!is_array($exe_local) || !is_array($exe_local[0])) {
                                $o2db_result = "<tr><td class='res_col'>".
                                               "<i>Query executed</i></td></tr>";
                                }
                            else {
                                foreach ($exe_local as $single_record) {
                                    $result_counter++;
                                    $o2db_result.= "<tr><td class='count_col'>".
                                                   $result_counter."</td>";
                                    foreach ($single_record as $s_key => $s_value) {
                                        if ($result_counter < 2) {
                                            $result_titles.= "<th class='res_col'>".
                                                             htmlentities($s_key,
                                                                   ENT_COMPAT | ENT_HTML5,
                                                                          "")."</th>";
                                            }
                                        $o2db_result.= "<td class='res_col'>".
                                                       ($s_value ?
                                                        htmlentities($s_value,
                                                                   ENT_COMPAT | ENT_HTML5,
                                                                     "") : "&nbsp;").
                                                       "</td>";
                                        }
                                    $o2db_result.="</tr>\n";
                                    }
                                $o2db_result = "<tr><th class='res_col'>#</th>".
                                               $result_titles."</tr>\n".$o2db_result;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

?>

<html>
    <head>
        <title>o2 :: dbadmin - Database Server Administration</title>
        <link rel='icon' href='img/o2.ico' type='image/ico'>
        <style type="text/css">
        <!--
          body {
             color: #666666;
             background-color: #fefefe;
             font: normal 16px Arial, sans-serif;
             letter-spacing: 1px;
             margin: 0;
             }
            .label {
             color: #666666;
             font: normal 0.78em/1.5em Arial, sans-serif;
             letter-spacing: 1px;
             padding-right: 1em;
             padding-left: 1em;
                }
            .field {
                color: #666666;
             font: bold 0.78em Arial, sans-serif;
                border: 1px solid #dddddd;
                 width: 160px;
             height: 1.5em;
             padding-left: 3px;
                }
            .field_area {
                color: #414141;
             font: normal 0.78em/1.5em "Courier New", mono;
             border: 1px solid #dddddd;
             padding: 0 5px 0 5px;
                 width: 100%;
             }
         .tab_list {
             color: #666666;
             font: normal 0.78em/1.5em Arial, sans-serif;
             border: 1px solid #dddddd;
             width: 100%;
             padding: 0 5px 0 5px;
                }
            .result {
             color: #414141;
             font: normal 0.78em/1.2em "Courier New", mono;
             letter-spacing: 0;
             border: 1px solid #666666;
                border-right: none;
             padding: 0 5px 0 5px;
                }
            .count_col {
             color: #993300;
             font: normal 0.72em/1.2em "Courier New", mono;
             letter-spacing: 0;
             text-align: right;
                 border-right: 1px solid #666666;
                padding-right: 2px;
             padding-top: 2px;
                }
            .res_col {
                border-right: 1px solid #666666;
             padding: 0 5px 0 5px;
                white-space: nowrap;
                }
         .btn_app {
             color: #666666;
             background-color: #dddddd;
             font: bold 0.78em/1.5em Arial, sans-serif;
             letter-spacing: 1px;
             border: 1px solid;
             border-left-color: #eaeaea;
             border-top-color: #eaeaea;
             border-right-color: #b1b1b1;
             border-bottom-color: #b1b1b1;
             height: 1.5em;
             }
         .btn_app:hover {
             color: #993300;
             background-color: #dddddd;
             font: bold 0.75em/1.5em Arial, sans-serif;
             letter-spacing: 1px;
             border: 1px solid;
             border-left-color: #eaeaea;
             border-top-color: #eaeaea;
             border-right-color: #b1b1b1;
             border-bottom-color: #b1b1b1;
             height: 1.5em;
             cursor: pointer;
             }
        -->
        </style>
        <script>

            function showTab(tabList) {

                tabList.form.o2dbadm_command.value = "SELECT * FROM " + tabList.value;

                }

        </script>
    </head>
    <body>
     <br />
     <form method="POST" target="_self" action="<?php echo $_SERVER['PHP_SELF']; ?>">
      <table align="center" width="500px">
       <tr>
        <td class="label">Type:</td>
        <td>
         <select name="o2dbadm_type" class="field">
          <?php
            foreach ($present_gateways as $gateway_name => $gateway_selected) {
                print "<option value='".$gateway_name."' ".
                      ($gateway_selected ? "selected" : "").
                      ">".$gateway_name."</option>";
                }
          ?>
         </select>
        </td>
        <td class="label">Tables list:</td>
       </tr>
       <tr>
        <td class="label">Server:</td>
        <td>
         <INPUT type="text"
                name="o2dbadm_server"
                class="field"
                value="<?php echo $o2db_server; ?>">
         </td>
        <td rowspan="5">
         <select name="o2dbadm_table"
                 size="10"
                 class="tab_list"
                 ondblclick="showTab(this);"
                 title="Double click on table to select from">
          <?php
            foreach ($tables_list as $single_table) {
                print "<option value='".$single_table."' ".
                      ($single_table == $o2db_table ? "selected" : "").
                      ">".$single_table."</option>";
                }
          ?>
         </select>
        </td>
       </tr>
       <tr>
        <td class="label">User:</td>
        <td><INPUT type="text"
                   name="o2dbadm_user"
                   class="field"
                   value="<?php echo $o2db_user; ?>">
        </td>
       </tr>
       <tr>
        <td class="label">Password:</td>
        <td><INPUT type="password"
                   name="o2dbadm_password"
                   class="field"
                   value="<?php echo $o2db_password; ?>">
        </td>
       </tr>
       <tr>
        <td class="label">Database:</td>
        <td><input type="text"
                   name="o2dbadm_database"
                   class="field"
                   value="<?php echo $o2db_database; ?>">
        </td>
       </tr>
       <tr>
        <td class="label">Schema:</td>
        <td>
         <input type="text"
                name="o2dbadm_owner"
                class="field"
                value="<?php echo $o2db_owner; ?>">
        </td>
       </tr>
       <tr>
        <td colspan="3" height="100%">
         <textarea name="o2dbadm_command" rows="4" class="field_area"><?php
          print $o2db_query;
         ?></textarea>
        </td>
       </tr>
       <tr>
        <td colspan="2" valign="top" class="label">Result:</td>
        <td align="right">
         <INPUT name="o2dbadm_go" type="submit" value="Execute" class="btn_app">
        </td>
       </tr>
      </table>
      <table bgcolor="<?php echo ($o2db_error ? "#993300" : "#eeeeee"); ?>"
             class="result"
             cellpadding="0"
             cellspacing="0">
       <?php print $o2db_result; ?>
      </table>
     </form>
    </body>
</html>
