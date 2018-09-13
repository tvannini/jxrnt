��       ,      �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � �                                                                                   	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +          ,         jxinspector    "jxtree"jxinspector    array('[root]' => array(0, ''))jxinspector   - o2val('prg�_�var','element_type') == "string"jxinspector    true jxinspector   u   if (o2exp(22)) {
   $tabs = array("" => "") + o2_inspector::get_app_tabs();
   }
else {
   $tabs = array();
   } $tabs jxinspector   �   if (o2exp(8)) {
   $tab_name = o2val('prg�_�var','tab_name');
   $fields   = array("" => "") + o2_inspector::get_tab_fields($tab_name);
   }
else {
   $fields = array();
   } $fields jxinspector   �   $type = o2val('prg�_�var','element_type');
$cond = $type == "field"  ||
        $type == "update" ||
        $type == "param"; $condjxinspector	   * o2val('prg�_�var','element_type') == "key" jxinspector
      if (o2val('prg�_�var','element_type') == "key") {
   $tab_name = o2val('prg�_�var','tab_name');
   $keys     = array("" => "", "[exp]" => "[By expression]") +
               o2_inspector::get_tab_keys($tab_name);
   }
else {
   $keys = array();
   } $keys jxinspector   �	  $model_name = o2val('prg�_�var','data_model');
$app_var    = o2val('prg�_�var','app_var');
$tab_name   = o2val('prg�_�var','tab_name');
$tab_field  = o2val('prg�_�var','tab_field_name');
$tab_key    = o2val('prg�_�var','tab_key_name');
$prg_name   = o2val('prg�_�var','program');
$par_id     = o2val('prg�_�var','par_id');
$view       = o2val('prg�_�var','prg_view');
$prg_act    = o2val('prg�_�var','prg_act');
$prg_field  = o2val('prg�_�var','prg_field');
$exp_id     = o2val('prg�_�var','exp_id');
$string     = o2val('prg�_�var','string');
$list       = array();
$insp       = new o2_inspector();
switch (o2val('prg�_�var','element_type')) {
   case "model":
      $list = $insp->find_model($model_name);
      break;
   case "appvar":
      $list = $insp->find_app_var($app_var);
      break;
   case "tab":
      $list = $insp->find_table($tab_name);
      break;
   case "post":
      $list = $insp->find_table_event($tab_name, "SAVE");
      break;
   case "insert":
      $list = $insp->find_table_event($tab_name, "INSERT");
      break;
   case "delete":
      $list = $insp->find_table_event($tab_name, "DELETE");
      break;
   case "field":
      $list = $insp->find_table_field($tab_name, $tab_field);
      break;
   case "key":
      $list = $insp->find_table_key($tab_name, $tab_key);
      break;
   case "update":
      $list = $insp->find_act_field_update($tab_name, $tab_field);
      break;
   case "param":
      $list = $insp->find_field_as_param($tab_name, $tab_field);
      break;
   case "prg":
      $list = $insp->find_prg($prg_name);
      break;
   case "prg_par":
      $list = $insp->find_prg_par($prg_name, $par_id);
      break;
   case "prg_act":
      $list = $insp->find_prg_act($prg_name, $prg_act);
      break;
   case "prg_field":
      $list = $insp->find_prg_field($prg_name, $view, $prg_field);
      break;
   case "prg_unused_exps":
      $list = $insp->find_prg_unused_exps($prg_name);
      break;
   case "prg_duplic_exps":
      $list = $insp->find_prg_duplicated_exps($prg_name);
      break;
   case "prg_exp":
      $list = $insp->find_prg_exp($prg_name, $exp_id);
      break;
   case "string":
      $list = $insp->find_string($string);
      break;
   case "duplic":
      $list = $insp->find_duplicated_exps();
      break;
   }
if ($list == array()) {
   $list["[info]"] = array(0, "No valid search");
   }
o2tree_fold('result', 1); $listjxinspector    ""jxinspector    "<jx>/img/jobs/separator.png"jxinspector    o2val('prg�_�var','result')jxinspector   
 "jximgbtn"jxinspector    "<jx>/img/tools/tree/fold.png"jxinspector     "<jx>/img/tools/tree/unfold.png" jxinspector      o2tree_fold("result"); true jxinspector      o2tree_unfold("result"); true jxinspector   I   o2_send(o2_path(o2tree_export("result", "Inspection_".date("Ymd_His")))); truejxinspector     "<jx>/img/tools/tree/export.png" jxinspector     $type = o2val('prg�_�var','element_type');
$cond = $type == "tab"    ||
        $type == "post"   ||
        $type == "insert" ||
        $type == "delete" ||
        $type == "field"  ||
        $type == "param"  ||
        $type == "key"    ||
        $type == "update"; $cond jxinspector   �   $prgs = o2_inspector::get_app_prgs();
$list = array_keys($prgs);
$list = array("" => "", "[exp]" => "[By expression]") + array_combine($list, $list); $listjxinspector   6 (o2exp(33) ? o2val('prg�_�var','prgs_list') : array()) jxinspector   J  $list                    = array();
$list["model"]           = array(0, "Data type");
$list["appvar"]          = array(0, "Application variable");
$list["tab"]             = array(0, "Table");
$list["key"]             = array("tab", "Index");
$list["post"]            = array("tab", "Save");
$list["insert"]          = array("tab", "Insert");
$list["delete"]          = array("tab", "Delete");
$list["field"]           = array("tab", "Field");
$list["update"]          = array("field", "Update");
$list["param"]           = array("field", "As parameter");
$list["prg"]             = array(0, "Program");
$list["prg_par"]         = array("prg", "Parameter");
$list["prg_field"]       = array("prg", "Field or variable");
$list["prg_act"]         = array("prg", "Action");
$list["prg_unused_exps"] = array("prg", "Unused expressions");
$list["prg_duplic_exps"] = array("prg", "Duplicated expressions");
$list["prg_exp"]         = array("prg", "Expression uses");
$list["string"]          = array(0, "String in exps");
$list["duplic"]          = array(0, "Exps duplicated in prgs"); $listjxinspector   , o2val('prg�_�var','element_type') == "model" jxinspector   �   if (o2val('prg�_�var','element_type') == "model") {
   $mods = array("" => "", "[exp]" => "[By expression]") + o2_inspector::get_app_models();
   }
else {
   $mods = array();
   } $mods jxinspector   U  $json  = "{";
$nodes = o2val('prg�_�var','result');
foreach ($nodes as $node_id => $info) {
   $json.= '"'.$node_id.'":["'.$info[0].'","'.addslashes($info[1]).'","'.$info[2].'","'.$info[3].'"],';
   }
$json = substr($json, 0, -1)."}";
$tmp  = tempnam(o2app_dir_tmp(), "json_");
file_put_contents($tmp, $json);
o2_send(o2_path($tmp)); truejxinspector   . o2val('prg�_�var','element_type') == "prg_exp" jxinspector    �   if (o2val('prg�_�var','element_type') == "prg_exp") {
   $prg_name = o2val('prg�_�var','program');
   $exps     = array("" => "") + o2_inspector::get_prg_exps_list($prg_name);
   }
else {
   $exps = array();
   } $exps jxinspector!     $type = o2val('prg�_�var','element_type');
$cond = $type == "prg"  ||
        $type == "prg_par" ||
        $type == "prg_act" ||
        $type == "prg_field" ||
        $type == "prg_unused_exps" ||
        $type == "prg_duplic_exps" ||
        $type == "prg_exp"; $condjxinspector"   . o2val('prg�_�var','element_type') == "prg_par" jxinspector#   f  if (o2val('prg�_�var','element_type') == "prg_par") {
   $prg_name = o2val('prg�_�var','program');
   $list     = o2_inspector::get_prg_pars_list($prg_name);
   $pars     = array(0 => "");
   $par_id   = 0;
   foreach ($list as $par_name => $model) {
      $par_id++;
      $pars[$par_id] = $par_name;
      }
   }
else {
   $pars = array();
   } $pars jxinspector$   K   $type = o2val('prg�_�var','element_type');
$cond = ($type == "prg_field"); $cond jxinspector%   �   $prg_name = o2val('prg�_�var','program');
if ($prg_name && o2exp(36)) {
   $views = array("" => "") + o2_inspector::get_prg_views_list($prg_name);
   }
else {
   $views = array();
   } $views jxinspector&   I   $type = o2val('prg�_�var','element_type');
$cond = ($type == "prg_act"); $cond jxinspector'     $prg_name  = o2val('prg�_�var','program');
$view_name = o2val('prg�_�var','prg_view');
if ($prg_name && $view_name && o2exp(36)) {
   $fields = array("" => "") + o2_inspector::get_prg_view_fields_list($prg_name, $view_name);
   }
else {
   $fields = array();
   } $fields jxinspector(   �   $prg_name = o2val('prg�_�var','program');
if ($prg_name && o2exp(38)) {
   $acts = array("" => "") + o2_inspector::get_prg_acts_list($prg_name);
   }
else {
   $acts = array();
   } $actsjxinspector)   - o2val('prg�_�var','element_type') == "appvar" jxinspector*   %  if (o2val('prg�_�var','element_type') == "appvar") {
   $app_vars = o2_inspector::get_app_vars();
   $vars     = array("" => "");
   foreach ($app_vars as $var_name => $var_model) {
       $vars[$var_name] = $var_name.' ['.$var_model.']';
       }
   }
else {
   $vars = array();
   } $varsjxinspector+    'hidden'jxinspector,   
 'nowindow'jxinspector-   	 'nav_big'jxinspector.   # !o2zero('prg�_�var','element_type')jxinspector    "jxtree jxtree_bottom_left"