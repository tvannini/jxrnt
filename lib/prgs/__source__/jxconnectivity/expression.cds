��       �     prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � �                                                                                  	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +          ,          -          .          /          0          1          2          3          4          5          6          7          8          9          :          ;          <          =          >          ?          @          A          B          C          D          E          F          G          H          I          J          K          L          M          N          O          P          Q          R          S          T          U          V          W          X          Y          Z          [          \          ]          ^          _          `          a          b          c          d          e          f          g          h          i          j          k          l          m          n          o          p          q          r          s          t          u          v          w          x          y          z          {          |          }          ~                    �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �          �                                                                                            	         
                                                                                                                                                                                                                !         "         #         $         %         &         '         (         )         *         +         ,         -         .         /         0         1         2         3         4         5         6         7         8         9         :         ;         <         =         >         ?         @         A         B         C         D         E         F         G         H         I         J         K         L         M         N         O         P         Q         R         S         T         U         V         W         X         Y         Z         [         \         ]         ^         _         `         a         b         c         d         e         f         g         h         i         j         k         l         m         n         o         p         q         r         s         t         u         v         w         x         y         z         {         |         }         ~                  �         �         �         �         �         �         �         �         �         �         �         �        jxconnectivity   $ "jximgbtn".(o2exp(72) ? "" : "_dis") jxconnectivity     $list         = o2val('prg�_�var','tab_extra_indexes');
$index_fields = o2val('extra_indexes','index_fields');
$c_name       = o2val('extra_indexes','index_name');
$unique       = o2val('extra_indexes','index_unique');
$list[$index_fields] = array($c_name, $index_fields, $unique); $list jxconnectivity   Q  $list    = array();
$imgpath = "<jx>/img/tabadmin/tree/";
// ________________________________________________________ Get servers list ___
foreach (o2app_servers() as $single_server) {
   $list["jxsrv_".$single_server] = array(0, $single_server, $imgpath."server.png", "server");
   }
// ______________________________________________________ Get databases list ___
foreach (o2app_databases() as $single_db) {
   $list["jxdb_".$single_db] = array("jxsrv_".o2db_server($single_db), $single_db, $imgpath."db.png", "db");
   }
// _________________________________________________________ Get tables list ___
$tabs = o2app_tables();
foreach ($tabs as $single_tab) {
   $list[$single_tab] = array("jxdb_".o2tab_db($single_tab), $single_tab, $imgpath."tab.png", "table");
   }
// ___________________________________________________ Sort list if required ___
if (o2val('prg�_�var','sort_tree')) {
   uksort($list, 'strnatcasecmp');
   }
// __________________________________________________________________ FILTER ___
$needle = strtolower(o2val('prg�_�var','tab_name_filter'));
if ($needle) {
   $f_list = array();
   foreach ($list as $node_id => $node) {
      if (($node[3] == "table") && (strpos(strtolower($node[1]), $needle) !== false)) {
         $f_list[$node_id] = $node;
         $child = $node;
         while (true) {
            if ($child[0]) {
               $f_list[$child[0]] = $list[$child[0]];
               $child = $list[$child[0]];
               }
            else {
               break;
               }
            }
         }
      }
   if (count($f_list) < 1) {
      $f_list["res"] = array(0, "No result", $imgpath."folder.png");
      }
   o2tree_def("tables", $f_list, 1, "tree_select");
   o2tree_fold("tables");
   $list = $f_list;
   }
else {
   o2tree_def("tables", $list, 1, "tree_select");
   } $list jxconnectivity   )   o2tab_create(o2val('prg�_�var','table')); true jxconnectivity   '  $arr_phys  = o2val('prg�_�var','fields_physical');
$arr_def   = o2val('prg�_�var','fields_o2def');
$arr_match = array();
foreach ($arr_phys as $i => $singolo_campo) {
         $arr_match[$arr_phys[$i]] = $arr_def[$i];
         }
$ok = o2tab_rebuild(o2val('prg�_�var','table'), $arr_match); $okjxconnectivity   ! o2val('prg�_�var','fields_o2def')jxconnectivity   $ o2val('prg�_�var','fields_physical')jxconnectivity   " o2val('prg�_�var','only_in_o2def')jxconnectivity	   % o2val('prg�_�var','only_in_physical')jxconnectivity
   m o2val('prg�_�var','table') && o2val('prg�_�var','phys_structure', 'exist') && !o2val('prg�_�var','operation')jxconnectivity   $ o2val('prg�_�var','fields_physical') jxconnectivity   >   $arr_local = array_values(o2val('prg�_�var','only_in_o2def')); $arr_local[0]jxconnectivity   & o2tab_info(o2val('prg�_�var','table'))jxconnectivity    o2val('prg�_�var','table') jxconnectivity   �   $field_name = o2val('prg�_�var','only_in_physical', o2val('prg�_�var','field_from_physical'));
$list = o2val('prg�_�var','fields_physical') +
        array((count(o2val('prg�_�var','fields_physical')) + 1) => $field_name); $list jxconnectivity   �   $field_name = o2val('prg�_�var','only_in_o2def', o2val('prg�_�var','field_from_o2def'));
$list = o2val('prg�_�var','fields_o2def') +
        array((count(o2val('prg�_�var','fields_o2def')) + 1) => $field_name); $listjxconnectivity    o2tree_get_code("tables") jxconnectivity   A   $arr_local = array_values(o2val('prg�_�var','only_in_physical')); $arr_local[0]jxconnectivity    "R" jxconnectivity   4  $arr_local = array();
$field_out = o2val('prg�_�var','only_in_physical', o2val('prg�_�var','field_from_physical'));
foreach (o2val('prg�_�var','only_in_physical') as $single_field) {
        if ($single_field != $field_out) {
           $arr_local[$single_field] = $single_field;
           }
        }
 $arr_local jxconnectivity   +  $arr_local = array();
$field_out = o2val('prg�_�var','only_in_o2def', o2val('prg�_�var','field_from_o2def'));
foreach (o2val('prg�_�var','only_in_o2def') as $single_field) {
        if ($single_field != $field_out) {
           $arr_local[$single_field] = $single_field;
           }
        }
 $arr_localjxconnectivity    falsejxconnectivity   T o2val('prg�_�var','node_type') == "field" || o2val('prg�_�var','node_type') == "key" jxconnectivity   v   $list  = o2val('prg�_�var','tables_list');
$field = o2val('prg�_�var','field');
$tab   = $list[$list[$field][0]][0]; $tab jxconnectivity   '   o2tab_drop(o2val('prg�_�var','table')); truejxconnectivity    "jxtree"jxconnectivity    o2tree_get_selection("tables")jxconnectivity   # o2val('prg�_�var','asp_codes_list')jxconnectivity     "<jx>/img/tabadmin/dbexport.png"jxconnectivity   � o2val('prg�_�var','node_type') == "table" || o2val('prg�_�var','node_type') == "field" || o2val('prg�_�var','node_type') == "key" jxconnectivity   �  $list = array();
$root_folder = o2app_dir_home()."data".DIRECTORY_SEPARATOR;
$list[$root_folder] = $root_folder;
reset($list);
while ((list($folder_name, $folder_path) = each($list))) {
      foreach (o2dir_list($folder_path) as $single_element) {
              if (o2file_type($single_element) == "D") {
                 $list[$single_element] = $single_element.DIRECTORY_SEPARATOR;
                 }
              }
      }
$list_copy = $list;
$list = array();
foreach ($list_copy as $i => $single_element) {
    $name        = str_replace("\\", "/", $i);
    $list[$name] = substr($name, strlen($root_folder) - 1);
    } $listjxconnectivity     "jxtoolbar"jxconnectivity!   ! o2val('prg�_�var','folders_list') jxconnectivity"   F   $list = array_keys(o2val('prg�_�var','folders_list'));
$t = $list[0]; $tjxconnectivity#    "export" jxconnectivity$   #  if (count(o2val('prg�_�var','phys_structure','fields','more'))) {
   $list = @array_combine(array_keys(o2val('prg�_�var','phys_structure','fields','more')),
                          array_keys(o2val('prg�_�var','phys_structure','fields','more')));
   }
else {
   $list = array();
   } $listjxconnectivity%    "import"jxconnectivity&    "" jxconnectivity'   Z  $list         = array();
$folder_local = o2val('prg�_�var','folder').DIRECTORY_SEPARATOR;
$filter       = (o2exp(129) ? o2val('prg�_�var','file_name') : "*");
foreach (o2dir_list($folder_local, $filter) as $single_element) {
        if (o2file_type($single_element) != "D") {
            $single_element        = str_replace("\\", "/", $single_element);
            $list[$single_element] = o2file_basename($single_element).
                                     (o2file_ext($single_element) ?
                                      ".".o2file_ext($single_element) : "");
           }
        } $listjxconnectivity(    truejxconnectivity)   , basename(o2val('prg�_�var','selected_file')) jxconnectivity*      o2tree_fold("tables"); truejxconnectivity+    o2val('prg�_�var','files_list')jxconnectivity,   ! o2val('prg�_�var','table').".o2x"jxconnectivity-    "*.o2x"jxconnectivity.   t (o2val('prg�_�var','operation') == "export" || o2val('prg�_�var','operation') == "export_all" ? "Export" : "Import")jxconnectivity/   � o2tab_export(o2val('prg�_�var','table'), o2val('prg�_�var','folder').DIRECTORY_SEPARATOR.o2val('prg�_�var','file_name'), o2val('prg�_�var','imp_exp_asp'))jxconnectivity0   P !o2zero('prg�_�var','file_name') && (o2val('prg�_�var','operation') == "export")jxconnectivity1   � o2tab_import(o2val('prg�_�var','table'), o2val('prg�_�var','folder').DIRECTORY_SEPARATOR.o2val('prg�_�var','file_name'), o2val('prg�_�var','imp_exp_asp'))jxconnectivity2   P !o2zero('prg�_�var','file_name') && (o2val('prg�_�var','operation') == "import") jxconnectivity3     if (o2val('prg�_�var','operation') == "export") {
   $text = o2val('prg�_�var','affected_rows')." rows exported from table ".o2val('prg�_�var','table').
           "<br>to file: ".o2val('prg�_�var','folder').o2val('prg�_�var','file_name');
   }
elseif (o2val('prg�_�var','operation') == "import") {
   $text = o2val('prg�_�var','affected_rows')." rows imported from file ".
           o2val('prg�_�var','folder').o2val('prg�_�var','file_name').
           "<br>to table: ".o2val('prg�_�var','table');
   } $textjxconnectivity4   < o2val('prg�_�var','tab4check') != o2val('prg�_�var','table') jxconnectivity5      o2tree_unfold("tables"); truejxconnectivity6   ! "<jx>/img/tabadmin/tree/fold.png"jxconnectivity7   & !o2val('prg�_�var','unlock_asp_codes')jxconnectivity8   $ o2val('prg�_�var','node_type4check')jxconnectivity9   ( o2tab_browse(o2val('prg�_�var','table'))jxconnectivity:   # "<jx>/img/tabadmin/tree/unfold.png" jxconnectivity;   r   $name = o2val('prg�_�var','field');
$tab  = o2val('prg�_�var','table');
$name = substr($name, strlen($tab) + 1); $name jxconnectivity<   /  $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
switch ($field[5]) {
   case "M":
      $err = "MISSING";
      break;
   case "C":
      $err = "CHANGED";
      break;
   case "U":
      $err = "UNDEFINED";
      break;
   default:
      $err = "";
      break;
   } $errjxconnectivity=   ! o2field_model("o2_users", "area")jxconnectivity>    "em" jxconnectivity?   #  if (count(o2val('prg�_�var','phys_structure','fields','less'))) {
   $list = @array_combine(array_keys(o2val('prg�_�var','phys_structure','fields','less')),
                          array_keys(o2val('prg�_�var','phys_structure','fields','less')));
   }
else {
   $list = array();
   } $list jxconnectivity@     $phys = array_keys(o2val('prg�_�var','phys_structure','fields','phys'));
$phys = @array_combine($phys, $phys);
$def  = array_keys(o2val('prg�_�var','phys_structure','fields','def'));
$def  = @array_combine($def, $def);
$list = array_intersect($phys, $def); $listjxconnectivityA   
 "jximgbtn"jxconnectivityB   = o2val('prg�_�var','table') && !o2val('prg�_�var','operation')jxconnectivityC   m o2val('prg�_�var','table') && !o2val('prg�_�var','phys_structure','exist') && !o2val('prg�_�var','operation')jxconnectivityD   $ "jximgbtn".(o2exp(10) ? "" : "_dis") jxconnectivityE   �   $op = o2val('prg�_�var','operation');
$if = $op == 'import' ||
      $op == 'export' ||
      $op == 'import_all' ||
      $op == 'export_all'; $ifjxconnectivityF   + o2val('prg�_�var','operation') == 'rebuild'jxconnectivityG   	 'rebuild'jxconnectivityH    !o2val('prg�_�var','operation')jxconnectivityI   = o2val('prg�_�var','table') && !o2val('prg�_�var','operation')jxconnectivityJ   _ o2val('prg�_�var','field_from_o2def') !== "" && o2val('prg�_�var','field_from_physical') !== ""jxconnectivityK   $ "jximgbtn".(o2exp(67) ? "" : "_dis") jxconnectivityL   �   $text = (o2val('prg�_�var','operation_res') ?
         "ASP code ".o2val('_o2SESSION','_area')." cleared in table ".o2val('prg�_�var','table') :
         "Nothing done for table ".o2val('prg�_�var','table')); $textjxconnectivityM   $ "jximgbtn".(o2exp(73) ? "" : "_dis") jxconnectivityN     $enab =  o2val('prg�_�var','node_type') == "table" &&
         o2val('prg�_�var','table') &&
         o2val('prg�_�var','phys_structure', 'exist') &&
         o2tab_asp(o2val('prg�_�var','table')) &&
        !o2val('prg�_�var','operation') &&
        !o2val('prg�_�var','is_view'); $enabjxconnectivityO    o2val('_o2SESSION','_area')jxconnectivityP    o2val('prg�_�var','area_local')jxconnectivityQ   D o2tab_asp(o2val('prg�_�var','table')) && o2val('_o2SESSION','_area')jxconnectivityR    "<jx>/img/tabadmin/unlock.png"jxconnectivityS   o (o2val('prg�_�var','node_type') == "table" && !o2zero('prg�_�var','table') ? o2val('prg�_�var','table') : null)jxconnectivityT   % "jximgbtn".(o2exp(155) ? "" : "_dis")jxconnectivityU    1000jxconnectivityV    o2view_rec("logged_table")jxconnectivityW   $ "jximgbtn".(o2exp(78) ? "" : "_dis") jxconnectivityX   r   $name = o2val('prg�_�var','field');
$tab  = o2val('prg�_�var','table');
$name = substr($name, strlen($tab) + 7); $namejxconnectivityY   % "jximgbtn".(o2exp(125) ? "" : "_dis")jxconnectivityZ   p o2val('prg�_�var','table') && o2view_rec("logged_table") && (substr(o2val('prg�_�var','table'), -6) != "_o2log")jxconnectivity[   $ "jximgbtn".(o2exp(90) ? "" : "_dis")jxconnectivity\   + o2tab_browselog(o2val('prg�_�var','table'))jxconnectivity]   ; "<jx>/img/tabadmin/browse".(o2exp(10) ? "" : "_dis").".png"jxconnectivity^   ; "<jx>/img/tabadmin/export".(o2exp(10) ? "" : "_dis").".png"jxconnectivity_   ; "<jx>/img/tabadmin/import".(o2exp(73) ? "" : "_dis").".png"jxconnectivity`   ; "<jx>/img/tabadmin/create".(o2exp(67) ? "" : "_dis").".png"jxconnectivitya   : "<jx>/img/tabadmin/drop".(o2exp(155) ? "" : "_dis").".png"jxconnectivityb   = "<jx>/img/tabadmin/aspclear".(o2exp(78) ? "" : "_dis").".png"jxconnectivityc   = "<jx>/img/tabadmin/rebuild".(o2exp(155) ? "" : "_dis").".png"jxconnectivityd   K (o2view_rec("logged_table") ? "Stop logging table" : "Start logging table")jxconnectivitye   � "<jx>/img/tabadmin/".(o2view_rec("logged_table") ? "logoff".(o2exp(125) ? "" : "_dis").".png" : "logon".(o2exp(125) ? "" : "_dis").".png")jxconnectivityf   > "<jx>/img/tabadmin/logbrowse".(o2exp(90) ? "" : "_dis").".png"jxconnectivityg   = "<jx>/img/tabadmin/checkall".(o2exp(72) ? "" : "_dis").".png"jxconnectivityh   ; "<jx>/img/tabadmin/export".(o2exp(72) ? "" : "_dis").".png"jxconnectivityi   ; "<jx>/img/tabadmin/import".(o2exp(72) ? "" : "_dis").".png"jxconnectivityj   : "<jx>/img/tabadmin/drop".(o2exp(123) ? "" : "_dis").".png"jxconnectivityk   = "<jx>/img/tabadmin/rebuild".(o2exp(123) ? "" : "_dis").".png"jxconnectivityl   < "<jx>/img/tabadmin/loglist".(o2exp(72) ? "" : "_dis").".png"jxconnectivitym   & o2val('prg�_�var','field_type') == "K"jxconnectivityn   & o2val('prg�_�var','field_type') == "F"jxconnectivityo   & !o2val('prg�_�var','unlock_asp_codes')jxconnectivityp   % o2val('prg�_�var','unlock_asp_codes')jxconnectivityq    "<jx>/img/tabadmin/lock.png" jxconnectivityr   �   $code        = o2asp_code_get();
$codes       = o2asp_codes();
$list        = array();
$list[""]    = "";
if (is_array($codes) && count($codes)) {
   $list[$code] = $code;
   $list       += array_combine($codes, $codes);
   } $listjxconnectivitys    !o2val('prg�_�var','node_type') jxconnectivityt   }   $text = "<center><h3>Janox Connectivity Manager</h3></center><br>Select an item from the treeview<br>to see its properties."; $textjxconnectivityu    "<jx>/img/jxpowered.png"jxconnectivityv    "disp_area"jxconnectivityw    "table"jxconnectivityx   Q "<jx>/img/tabadmin/".(o2val('prg�_�var','sort_tree') ? "unsort.png" : "sort.png")jxconnectivityy    !o2val('prg�_�var','sort_tree')jxconnectivityz   \ (o2val('prg�_�var','sort_tree') ? "Sort by definition order" : "Sort by alphabetical order")jxconnectivity{   X !o2val('prg�_�var','operation') && o2db_createtab(substr(o2val('prg�_�var','table'), 5)) jxconnectivity|   \   $tabs = o2val('prg�_�var','tables_list');
$type = $tabs[o2tree_get_selection("tables")][3]; $typejxconnectivity}   R o2val('prg�_�var','table') && (substr(o2val('prg�_�var','table'), -6) != "_o2log")jxconnectivity~   = "<jx>/img/tabadmin/aspclone".(o2exp(78) ? "" : "_dis").".png"jxconnectivity   % "jximgbtn".(o2exp(123) ? "" : "_dis")jxconnectivity�   = "<jx>/img/tabadmin/aspclear".(o2exp(72) ? "" : "_dis").".png"jxconnectivity�   X o2val('prg�_�var','operation') == "import" || o2val('prg�_�var','operation') == "export"jxconnectivity�   = "<jx>/img/tabadmin/aspclone".(o2exp(72) ? "" : "_dis").".png"jxconnectivity�   9 (o2exp(129) ? "Selected file name:" : "Selected folder:")jxconnectivity�    !o2exp(129)jxconnectivity�   Y o2val('prg�_�var','operation') == 'clone' || o2val('prg�_�var','operation') == 'cloneall' jxconnectivity�   �   $text = "Table ".o2val('prg�_�var','table').
        (o2val('prg�_�var','operation_res') ?
        " ASP data cloned to code ".o2val('prg�_�var','clone_target_area') :
        " skipped"); $textjxconnectivity�   ( !o2zero('prg�_�var','clone_target_area')jxconnectivity�    "clone"jxconnectivity�   
 "cloneall" jxconnectivity�   Y   $data = o2app_dir_data();
$dir = substr(o2val('prg�_�var','folder'), strlen($data) - 1); $dirjxconnectivity�   ) o2val('prg�_�var','operation') == 'clone'jxconnectivity�   , o2val('prg�_�var','operation') == 'cloneall'jxconnectivity�   ) o2val('prg�_�var','node_type') == "table"jxconnectivity�   & o2val('prg�_�var','node_type') == "db"jxconnectivity�    "export_all"jxconnectivity�   * o2val('prg�_�var','node_type') == "server" jxconnectivity�   T   $l = "ATTENTION: starting all tables check.\nOperation may take a while.\nProceed?"; $l jxconnectivity�   U   $l = "ATTENTION: starting all tables export.\nOperation may take a while.\nProceed?"; $l jxconnectivity�   \   $l = "ATTENTION: all ".o2val('_o2SESSION','_area')." ASP area data will be lost.\nProceed?"; $l jxconnectivity�   T   $l = "ATTENTION: starting ASP data cloning.\nOperation may take a while.\nProceed?"; $l jxconnectivity�   Z   $l = "ATTENTION: all listed tables will be fixed.\nOperation may take a while.\nProceed?"; $l jxconnectivity�   o   $l = "ATTENTION: all ".o2val('_o2SESSION','_area')." ASP area data\nwill be deleted from the table.\nProceed?"; $l jxconnectivity�   @   $l = "ATTENTION! Existing area data will be deleted.\nProceed?"; $ljxconnectivity�   E (o2db_createtab(o2tab_db(o2val('prg�_�var','table'))) ? false : true) jxconnectivity�   �   $enab =  o2val('prg�_�var','table') &&
        !o2val('prg�_�var','phys_structure','exist') &&
        !o2val('prg�_�var','operation') &&
        !o2val('prg�_�var','is_view'); $enabjxconnectivity�   ! "<jx>/img/tabadmin/separator.png" jxconnectivity�   �   $enab =  o2val('prg�_�var','table') &&
         o2val('prg�_�var','phys_structure', 'exist') &&
        !o2val('prg�_�var','operation') &&
        !o2val('prg�_�var','is_view'); $enabjxconnectivity�     !o2val('prg�_�var','log_active')jxconnectivity�    o2val('prg�_�var','log_active')jxconnectivity�    $_SESSION['o2_app']->tables_logjxconnectivity�    "tools/o2sys_dbexport" jxconnectivity�   0   o2ctrl_focus("n_rebuild_dialog", "btn_rebuild"); true jxconnectivity�   $   o2ctrl_focus("aspclone", "o2edit5"); true jxconnectivity�   +   o2ctrl_focus("m_expimp_dialog", "o2edit2"); truejxconnectivity�   + count(o2val('prg�_�var','files_list')) == 1 jxconnectivity�   I   $name = array_values(o2val('prg�_�var','files_list'));
$name = $name[0]; $namejxconnectivity�    "tools/o2sys_log_list" jxconnectivity�   $   o2ctrl_focus("gestione", "o2edit4"); truejxconnectivity�   e (o2val('prg�_�var','node_type') == "server" ? o2dbs_name(substr(o2val('prg�_�var','table'), 6)) : "")jxconnectivity�   e (o2val('prg�_�var','node_type') == "server" ? o2dbs_user(substr(o2val('prg�_�var','table'), 6)) : "") jxconnectivity�   �   $pwd = (o2val('prg�_�var','node_type') == "server" ? o2dbs_password(substr(o2val('prg�_�var','table'), 6)) : "");
if (!o2val('prg�_�var','show_password')) {
   if ($pwd) {
      $pwd = str_repeat("*", 10);
      }
   } $pwdjxconnectivity�   g (o2val('prg�_�var','node_type') == "server" ? o2dbs_engine(substr(o2val('prg�_�var','table'), 6)) : "")jxconnectivity�   Y (o2val('prg�_�var','node_type') == "server" ? substr(o2val('prg�_�var','table'), 6) : "")jxconnectivity�   U (o2val('prg�_�var','node_type') == "db" ? substr(o2val('prg�_�var','table'), 5) : "")jxconnectivity�   p (o2val('prg�_�var','node_type') == "db" ? o2dbs_engine(o2db_server(substr(o2val('prg�_�var','table'), 5))) : "")jxconnectivity�   ` (o2val('prg�_�var','node_type') == "db" ? o2db_name(substr(o2val('prg�_�var','table'), 5)) : "")jxconnectivity�   b (o2val('prg�_�var','node_type') == "db" ? o2db_schema(substr(o2val('prg�_�var','table'), 5)) : "") jxconnectivity�   �   $server = (o2val('prg�_�var','node_type') == "table" ||
           o2val('prg�_�var','node_type') == "field" ||
           o2val('prg�_�var','node_type') == "key" ?
           o2db_server(o2tab_db(o2val('prg�_�var','table'))) :
           ""); $server jxconnectivity�   �   $db = (o2val('prg�_�var','node_type') == "table" ||
       o2val('prg�_�var','node_type') == "field" ||
       o2val('prg�_�var','node_type') == "key" ?
       o2tab_db(o2val('prg�_�var','table')) :
       ""); $db jxconnectivity�     $engine = (o2val('prg�_�var','node_type') == "table" ||
           o2val('prg�_�var','node_type') == "field" ||
           o2val('prg�_�var','node_type') == "key" ?
           o2dbs_engine(o2db_server(o2tab_db(o2val('prg�_�var','table')))) :
           ""); $enginejxconnectivity�   b (o2val('prg�_�var','node_type') == "db" ? o2db_server(substr(o2val('prg�_�var','table'), 5)) : "") jxconnectivity�   �   $phys = (o2val('prg�_�var','node_type') == "table" ||
         o2val('prg�_�var','node_type') == "field" ||
         o2val('prg�_�var','node_type') == "key" ?
         o2tab_name(o2val('prg�_�var','table')) :
         ""); $phys jxconnectivity�   �   $exists = (o2val('prg�_�var','node_type') == "table" ||
           o2val('prg�_�var','node_type') == "field" ||
           o2val('prg�_�var','node_type') == "key" ?
           (o2tab_exists(o2val('prg�_�var','table')) ? "Yes" : "No") :
           ""); $exists jxconnectivity�   �   $data = (o2val('prg�_�var','node_type') == "table" ||
         o2val('prg�_�var','node_type') == "field" ||
         o2val('prg�_�var','node_type') == "key" ?
         (o2tab_data(o2val('prg�_�var','table')) ? "Yes" : "No") :
         ""); $data jxconnectivity�   U  $tab    = o2val('prg�_�var','table');
$logged = (o2val('prg�_�var','node_type') == "table" ||
           o2val('prg�_�var','node_type') == "field" ||
           o2val('prg�_�var','node_type') == "key" ?
           (o2tab_log($tab) ?
            o2format(o2tab_log_level($tab), "o2sys_loglevel") :
            "None") :
           ""); $loggedjxconnectivity�    "import_all"jxconnectivity�    "<jx>/img/alert.png"jxconnectivity�   . o2val('prg�_�var','operation') == "export_all"jxconnectivity�   . o2val('prg�_�var','operation') == "import_all"jxconnectivity�    "disp_area" jxconnectivity�   '9  $tab_strct        = o2val('prg�_�var','phys_structure');
$phys_fields      = $tab_strct['fields']['phys']; /* Existing physical fields */
$def_fields       = $tab_strct['fields']['def'];  /* Defined Janox fields */
$only_phys_fields = $tab_strct['fields']['more']; /* Physical fields missing in definition */
$only_def_fields  = $tab_strct['fields']['less']; /* Defined fields missing in physical structure*/
$phys_keys        = $tab_strct['keys']['phys'];   /* Existing physical keys */
$def_keys         = $tab_strct['keys']['def'];    /* Defined Janox keys */
$only_phys_keys   = $tab_strct['keys']['more'];   /* Physical keys missing in definition */
$only_def_keys    = $tab_strct['keys']['less'];   /* Defined keys missing in physical structure*/
$constraints      = o2val('prg�_�var','tab_constraints');
$active_cons      = o2val('prg�_�var','cons_read_list');
$extra_idx        = o2val('prg�_�var','tab_extra_indexes');
$tab_name         = o2val('prg�_�var','table');
$errors           = false;
$list             = o2val('prg�_�var','tables_list');
// __________________________________________________ Sort lists if required ___
if (o2val('prg�_�var','sort_tree')) {
   uksort($phys_fields, 'strnatcasecmp');
   uksort($def_fields, 'strnatcasecmp');
   uksort($only_phys_fields, 'strnatcasecmp');
   uksort($only_def_fields, 'strnatcasecmp');
   uksort($phys_keys, 'strnatcasecmp');
   uksort($def_keys, 'strnatcasecmp');
   uksort($only_phys_keys, 'strnatcasecmp');
   uksort($only_def_keys, 'strnatcasecmp');
   uksort($constraints, 'strnatcasecmp');
   uksort($extra_idx, 'strnatcasecmp');
   }
/*
 * $list record:
 *  0. parent
 *  1. text
 *  2. icon
 *  3. node type ([ ]|field|key|constraint|reference|extra_index)
 *  4. physical name | linked table
 *  5. status ([ ]|[M]issing|[C]hanged|[U]ndefined) | linked fields
 *  6. meta definition / index-segments | referenced fields | index fields
 *  7. type (fields & keys only) | Constraint name (constraint & reference only)
 *  8. size (fields & keys only)
 *  9. old type (changed fields only)
 * 10. old size (changed fields only)
 */
o2tree_node_remove("tables", $tab_name."_jxfields");
o2tree_node_remove("tables", $tab_name."_jxkeys");
o2tree_node_remove("tables", $tab_name."_jxintc");
o2tree_node_remove("tables", $tab_name."_jxextc");
foreach ($def_fields as $def_name => $def_field) {
    $status  = "";
    $icon    = "<jx>/img/tabadmin/tree/field.png";
    $size    = "";
    $oldtype = "";
    $oldsize = "";
    $text    = $def_field['name'];
    // ___________________________________________ Missing in physical table ___
    if ($tab_strct['exist'] && in_array($def_name, array_keys($only_def_fields))) {
        $status = "M";
        $icon   = "<jx>/img/tabadmin/tree/error.png";
        $text  .= " <i>(missing)</i>";
        $errors = true;
        }
    // _____________________________________________________________ Changed ___
    if (in_array($def_name, array_keys($tab_strct['fields']['var']))) {
        $status  = "C";
        $icon    = "<jx>/img/tabadmin/tree/error.png";
        $oldtype = $phys_fields[$def_name]['type'];
        if ($phys_fields[$def_name]['type'] != "number" &&
            $phys_fields[$def_name]['type'] != "structure") {
            $oldsize = $phys_fields[$def_name]['dim'];
            }
        $text  .= " <i>(changed)</i>";
        $errors = true;
        }
    // ________________________________________________________________ Size ___
    if ($def_field['type'] != "number" && $def_field['type'] != "structure") {
        if ($def_field['dim']) {
            $size = $def_field['dim'];
            }
        }
    // __________________________________________________ Add fields to list ___
    $list[$tab_name."_".$def_field['name']] = array($tab_name."_jxfields",
                                                    $text,
                                                    $icon,
                                                    "field",
                                                    $def_name,
                                                    $status,
                                                    $def_field['meta'],
                                                    $def_field['type'],
                                                    $size,
                                                    $oldtype,
                                                    $oldsize);

    }
// _______________________________________________ Undefined physical fields ___
foreach ($only_phys_fields as $only_phys_name => $only_phys_field) {
    $size   = "";
    $errors = true;
    if ($only_phys_field['type'] != "number" && 
        $only_phys_field['type'] != "structure") {
        $size = $only_phys_field['dim'];
        }
    // ________________________________________ Add undefined fields to list ___
    $list[$tab_name."_".$only_phys_name] = array($tab_name."_jxfields",
                                                 $only_phys_name.
                                                 " <i>(undefined)</i>",
                                                 "<jx>/img/tabadmin/tree/error.png",
                                                 "field",
                                                 $only_phys_name,
                                                 "U",
                                                 "",
                                                 $only_phys_field['type'],
                                                 $size);
    }
// _______________________________________________________ Add fields folder ___
$list[$tab_name."_jxfields"] = array($tab_name,
                                     "Fields",
                                     "<jx>/img/tabadmin/tree/".
                                     ($errors ? "folder_error" : "folder").".png",
                                     "",
                                     "",
                                     "");
// _________________________________________________________________ Indexes ___
$pkey   = o2tab_pkey(o2val('prg�_�var','table'));
$errors = false;          
foreach ($def_keys as $def_name => $def_key) {
    // ____________________________________________________ Standard indexes ___
    if (!$def_key['extra']) {
       $is_pkey = ($def_name == $pkey);
       $status  = "";
       $icon    = "<jx>/img/tabadmin/tree/".($is_pkey ? "p" : "")."key.png";
       $text    = $def_name;
       $segs    = array();
       // ________________________________________ Missing in physical table ___
       if ($tab_strct['exist'] &&
           in_array($def_name, array_keys($only_def_keys))) {
           $status = "M";
           $icon   = "<jx>/img/tabadmin/tree/error.png";
           $text  .= " <i>(missing)</i>";
           $errors = true;
           }
       // __________________________________________________________ Changed ___
       if (in_array($def_name, array_keys($tab_strct['keys']['var']))) {
           $status = "C";
           $icon   = "<jx>/img/tabadmin/tree/error.png";
           $text  .= " <i>(changed)</i>";
           $errors = true;
           }
          // ________________________________________________ Index segments ___
          foreach ($def_key['segments'] as $segm_index => $single_segm) {
              $segs[] = "<img src='".o2rnt_alias()."img/tabadmin/tree/".
              ($single_segm['dir'] == "D" ?
               "desc.png' title='Descending'" :
               "asc.png' title='Ascending'").
               " style='display:inline'>&nbsp; ".$single_segm['column'];
             }          
       // ______________________________________________________ Add to list ___
       $list[$tab_name."_jxkey_".$def_name] = array($tab_name."_jxkeys",
                                                    $text,
                                                    $icon,
                                                    "key",
                                                    $def_name,
                                                    $status,
                                                    $segs,
                                                    ($is_pkey ? "P" : "I"));
       }
    }
// ______________________________________________ Undefined physical indexes ___
$extra_list = array();
foreach ($extra_idx as $exidx) {
   $extra_list[] = $exidx[0];
   }
foreach ($only_phys_keys as $only_phys_name => $only_phys_key) {
    if (!in_array($only_phys_name, $extra_list)) {
       $segs = array();
       foreach ($only_phys_key as $segm_index => $single_segm) {
          $segs[] = "<img src='".o2rnt_alias()."img/tabadmin/tree/".
                    ($single_segm['dir'] == "D" ?
                    "desc.png' title='Descending'" :
                    "asc.png' title='Ascending'").
                    " style='display:inline'>&nbsp; ".$single_segm['column'];       
           }
       // ______________________________________________________ Add to list ___
       $list[$tab_name."_jxkey_".$def_field['name']] = array($tab_name."_jxkeys",
                                                             $only_phys_name.
                                                             " <i>(undefined)</i>",
                                                             "<jx>/img/tabadmin/tree/error.png",
                                                             "key",
                                                             $only_phys_name,
                                                             "U",
                                                             $segs,
                                                             ($is_pkey ? "P" : "I"));
       $errors = true;
       }
    }
// ___________________________________________________________ Extra indexes ___
foreach ($extra_idx as $exidx) {
   $text = $exidx[0];
   $segs = array();
   foreach (explode(',', $exidx[1]) as $single_segment) {
      list($seg_fld, $seg_dir) = explode('|', $single_segment);
      $segs[] = "<img src='".o2rnt_alias()."img/tabadmin/tree/".
                ($seg_dir == "D" ?
                "desc.png' title='Descending'" :
                "asc.png' title='Ascending'").
                " style='display:inline'>&nbsp; ".$seg_fld;       
      }
   if (in_array($exidx[0], array_keys($only_def_keys))) {
      $status = 'M';
      $icon   = '<jx>/img/tabadmin/tree/error.png';
      $text  .= ' <i>(missing)</i>';
      $errors = true;         
      }            
   elseif (in_array($exidx[0], array_keys($tab_strct['keys']['var']))) {
      $status = 'C';
      $icon   = '<jx>/img/tabadmin/tree/error.png';
      $text  .= ' <i>(changed)</i>';
      $errors = true;   
      }
   else {
      $status = '';
      $icon   = '<jx>/img/tabadmin/tree/ext_idx.png';         
      }
    // _________________________________________________________ Add to list ___
    $list[$tab_name."_jxeidx_".$exidx[0]] = array($tab_name.'_jxkeys',
                                                  $text,
                                                  $icon,
                                                  'extra_index',
                                                  $exidx[0],
                                                  $status,
                                                  $segs,
                                                  'I');
    }
// ______________________________________________________ Add indexes folder ___
$list[$tab_name."_jxkeys"] = array($tab_name,
                                   "Indexes",
                                   "<jx>/img/tabadmin/tree/".
                                   ($errors ? "folder_error" : "folder").".png",
                                   "",
                                   "",
                                   "");
// ____________________________________________________ Internal constraints ___
$done = false;
foreach ($constraints["C"] as $constraint) {
    $idx = $constraint[0]."|".$constraint[1];
    if (isset($active_cons[$constraint[3]])) {
       $icon = "<jx>/img/tabadmin/tree/constraint.png";
       }
    else {
       $icon = "<jx>/img/tabadmin/tree/constraint_dis.png";
       }
    // _________________________________________________________ Add to list ___
    $list[$tab_name."_jxintc_".$idx] = array($tab_name."_jxintc",
                                             $constraint[0],
                                             $icon,
                                             "constraint",
                                             $constraint[0],
                                             $constraint[1],
                                             $constraint[2],
                                             $constraint[3]);
    $done = true;
    }
// _________________________________________ Add internal constraints folder ___
if ($done) {
   $list[$tab_name."_jxintc"] = array($tab_name,
                                      "Constraints",
                                      "<jx>/img/tabadmin/tree/folder.png");
   }
// ____________________________________________________ External constraints ___
$icon = "<jx>/img/tabadmin/tree/reference.png";
$done = false;
foreach ($constraints["R"] as $constraint) {
    $idx = $constraint[0]."|".$constraint[1];
    // _________________________________________________________ Add to list ___
    $list[$tab_name."_jxextc_".$idx] = array($tab_name."_jxextc",
                                             $constraint[0],
                                             $icon,
                                             "reference",
                                             $constraint[0],
                                             $constraint[1],
                                             $constraint[2],
                                             $constraint[3]);
    $done = true;
    }
// _________________________________________ Add external constraints folder ___
if ($done) {
   $list[$tab_name."_jxextc"] = array($tab_name,
                                      "References",
                                      "<jx>/img/tabadmin/tree/folder.png");
   }
// _________________________________________________________ Define treeview ___
o2tree_def("tables", $list, 1, "tree_select");
o2tree_node_show("tables", $tab_name."_jxfields"); $listjxconnectivity�   A "All database tables exported to ".o2val('prg�_�var','file_name')jxconnectivity�   5 (o2val('prg�_�var','node_type') == "key" ? "K" : "F") jxconnectivity�     $vis  = false;
$list = o2val('prg�_�var','tables_list');
$tab  = o2val('prg�_�var','table');
foreach ($list as $field) {
   if (($field[0] == $tab."_jxfields" || $field[0] == $tab."_jxkeys") &&
       $field[5]) {
      $vis = true;
      break;
      }
   } $vis jxconnectivity�   Y   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
$phys = $field[4]; $physjxconnectivity�   C "All database tables imported from ".o2val('prg�_�var','file_name') jxconnectivity�   �   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
$meta  = (o2val('prg�_�var','field_type') == "F" ? $field[6] : ""); $meta jxconnectivity�   �   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
$type  = $field[7].($field[8] ? " (".$field[8].")" : ""); $type jxconnectivity�   �   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
$type  = $field[9].($field[10] ? " (".$field[10].")" : ""); $typejxconnectivity�   & o2val('prg�_�var','field_type') == "F" jxconnectivity�   �   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
if ($field[7] == "P") {
   $label = "Primary key:";
   }
else {
   $label = "Index name:";
   } $label jxconnectivity�   �   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
$cond  = (o2val('prg�_�var','field_type') == "F") && ($field[5] == "C"); $condjxconnectivity�   & o2val('prg�_�var','field_type') == "K" jxconnectivity�   �   $field = o2val('prg�_�var','tables_list',o2val('prg�_�var','field'));
$list  = (o2val('prg�_�var','field_type') == "K" ? implode("<br>", $field[6]) : ""); $list jxconnectivity�   �   if (o2val('prg�_�var','operation') == "import") {
   $txt = "Add ASP code column with value \"".o2asp_code_get()."\".";
   }
else {
   $txt = "Remove ASP code column in export.";
   } $txt jxconnectivity�   ^  $asp = (o2val('prg�_�var','node_type') == "db" ?
        o2db_asp(substr(o2val('prg�_�var','table'), 5)) :
        "");
switch ($asp) {
   case 'C':
      $asp = 'Column';
      break;
   case 'S':
      $asp = 'Schema';
      break;
   case 'D':
      $asp = 'Database';
      break;
   default:
      $asp = "Off";
      break;
   } $asp jxconnectivity�   �   $app    = $_SESSION['o2_app'];
$db     = $app->db[substr(o2val('prg�_�var','table'), 5)];
$create = ($db->create_tabs ? "On" : "Off"); $createjxconnectivity�   % substr(o2val('prg�_�var','table'), 5)jxconnectivity�   = "<jx>/img/tabadmin/extratab".(o2exp(72) ? "" : "_dis").".png"jxconnectivity�    "o2_ctrl_label" jxconnectivity�     $in       = (o2val('prg�_�var','node_type') == "db");
$db       = substr(o2val('prg�_�var','table'), 5);
$list     = array();
$img_path = "<jx>/img/tabadmin/tree/";
$errors   = 0;
if ($in) {
   foreach (o2app_tables() as $tab) {
      $ico    = "tab.png";
      $taberr = false;
      $type   = "error";
      if (o2tab_db($tab) == $db) {
         $tab_info = o2tab_info($tab);
         if ($tab_info['exist']) {
            if (count($tab_info['fields']['less'])) {
               $list[$tab."err1"] = array($tab,
                                          "Missing fields in physical table",
                                          $img_path."error.png");
               $taberr            = true;
               }
            if (count($tab_info['fields']['more'])) {
               $list[$tab."err2"] = array($tab,
                                          "Missing fields in table definition",
                                          $img_path."error.png");
               $taberr            = true;
               }
            if (count($tab_info['fields']['var'])) {
               $list[$tab."err3"] = array($tab,
                                          "Changed fields in table definition",
                                          $img_path."error.png");
               $taberr            = true;
               }
            if (count($tab_info['keys']['less'])) {
               $list[$tab."err4"] = array($tab,
                                          "Missing keys in physical table",
                                          $img_path."error.png");
               $taberr            = true;
               $type              = "index";
               }
            if (count($tab_info['keys']['more'])) {
               $list[$tab."err5"] = array($tab,
                                          "Missing keys in table definition",
                                          $img_path."error.png");
               $taberr            = true;
               $type              = "index";
               }
            if (count($tab_info['keys']['var'])) {
               $list[$tab."err6"] = array($tab,
                                          "Changed keys in table definition",
                                          $img_path."error.png");
               $taberr            = true;
               $type              = "index";
               }
            if (count($tab_info['fkeys']['less'])) {
               $list[$tab."err7"] = array($tab,
                                          "Unactive foreign keys",
                                          $img_path."error.png");
               $taberr            = true;
               $type              = "fkey";
               }
            if (count($tab_info['fkeys']['more'])) {
               $list[$tab."err8"] = array($tab,
                                          "Undefined foreign keys are active",
                                          $img_path."error.png");
               $taberr            = true;
               $type              = "fkey";
               }
            }
         else {
            $list[$tab."err0"] = array($tab,
                                       "Table does not exist on database",
                                       $img_path."error.png");
            $taberr            = true;
            }
         if ($taberr) {
            $errors++;
            $list[$tab] = array("jxresultset", $tab, $img_path."error.png", $type);
            }
         }
      }
   }
$list["jxresultset"] = array(0,
                             ($errors ? $errors." tables have problems!" : "No problems detected"),
                             $img_path.($errors ? "folder_error.png" : "folder.png"));
o2tree_def("result", $list, 1, "result_select"); "Check" jxconnectivity�   �  // _____________________________________________________________ Janox table ___
$tables = o2val('prg�_�var','tables_list');
// __________________ Element selected in result list (JX-table o SQL-table) ___
$nodes  = o2tree_get_nodes("result");
$sele   = o2tree_get_selection("result");
// ______________________________________________ Only tables are selectable ___
if ($nodes[$sele][0] && $nodes[$sele][0] == "jxresultset") {
   // _________________________________________ DB selected in main treeview ___
   $db_name = substr(o2val('prg�_�var','table'), 5);
   if (isset($tables[$sele])) {
      o2tree_node_select("tables", $sele);
      }
   // ____________________________________________________________ SQL table ___
   else {
      // ___________________________ Get Janox definition code for SQL-table ___
      eval(o2_file::get_def($db_name, $sele));
      $tables[$sele] = array("jxdb_".$db_name,
                             $sele,
                             "<jx>/img/tabadmin/tree/tab_sql.png",
                             "table",
                             "SQL");
      o2tree_def("tables", $tables, 1, "tree_select");
      }
   o2tree_node_show("tables", $sele);
   } $tablesjxconnectivity�   C (o2val('prg�_�var','show_result') ? o2tree_get_code("result") : "")jxconnectivity�   + o2val('prg�_�var','show_result')." result:"jxconnectivity�     o2val('prg�_�var','show_result') jxconnectivity�   A  $in       = (o2val('prg�_�var','node_type') == "db");
$db       = substr(o2val('prg�_�var','table'), 5);
$list     = array();
$img_path = "<jx>/img/tabadmin/tree/";
$tabs     = 0;
if ($in) {
   foreach (o2app_tables() as $tab) {
      if (o2tab_db($tab) == $db) {
         if (o2tab_drop($tab)) {
            $list[$tab] = array("jxresultset", $tab, $img_path."tab.png");
            $tabs++;
            }
         }
      }
   }
$list["jxresultset"] = array(0, $tabs." tables dropped", $img_path."folder.png");
o2tree_def("result", $list, 1, "result_select"); "Drop" jxconnectivity�   D  $in       = (o2val('prg�_�var','node_type') == "db");
$db       = substr(o2val('prg�_�var','table'), 5);
$list     = array();
$img_path = "<jx>/img/tabadmin/tree/";
$tabs     = 0;
if ($in) {
   foreach (o2app_tables() as $tab) {
      if (o2tab_db($tab) == $db) {
         if (o2tab_rebuild($tab)) {
            $list[$tab] = array("jxresultset", $tab, $img_path."tab.png");
            $tabs++;
            }
         }
      }
   }
$list["jxresultset"] = array(0, $tabs." tables rebuilt", $img_path."folder.png");
o2tree_def("result", $list, 1, "result_select");	 "Rebuild" jxconnectivity�   �  $in        = (o2val('prg�_�var','node_type') == "db");
$db        = substr(o2val('prg�_�var','table'), 5);
$list      = array();
$img_path  = "<jx>/img/tabadmin/tree/";
$data_path = o2app_dir_data().ltrim(o2val('prg�_�var','file_name'), DIRECTORY_SEPARATOR);
$tabs      = 0;
if ($in) {
   foreach (o2app_tables() as $tab) {
      if (o2tab_db($tab) == $db) {
         o2tab_export($tab, $data_path.$tab.".o2x");
         $list[$tab] = array("jxresultset", $tab, $img_path."tab.png");
         $tabs++;
         }
      }
   }
$list["jxresultset"] = array(0, $tabs." tables exported to ".$data_path, $img_path."folder.png");
o2tree_def("result", $list, 1, "result_select"); "Export" jxconnectivity�   [  $in        = (o2val('prg�_�var','node_type') == "db");
$db        = substr(o2val('prg�_�var','table'), 5);
$server    = o2db_server($db);
$list      = array();
$img_path  = "<jx>/img/tabadmin/tree/";
$data_path = o2app_dir_data().ltrim(o2val('prg�_�var','file_name'), DIRECTORY_SEPARATOR);
$create    = o2db_createtab($db);
$tabs      = 0;
$errors    = 0;
if ($in) {
   foreach (o2app_tables() as $tab) {
      $desc = "";
      $ico  = "tab.png";
      if (o2tab_db($tab) == $db) {
         if (o2file_exists($data_path.$tab.".o2x")) {
            if (o2tab_exists($tab) || $create) {
               o2tab_import($tab, $data_path.$tab.".o2x");
               o2dbs_commit($server);
               }
            else {
               $list[$tab."err1"] = array($tab, "Table creation denied", $img_path."error.png");
               $ico               = "error.png";
               $errors ++;
               }
            }
         else {
            $list[$tab."err2"] = array($tab, "Can't find import file ".$data_path.$tab.".o2x", $img_path."error.png");
            $ico               = "error.png";
            $errors++;
            }
         o2tab_export($tab, $data_path.$tab.".o2x");
         $list[$tab] = array("jxresultset", $tab, $img_path.$ico);
         $tabs++;
         }
      }
   }
if ($errors) {
   $list["jxresultset"] = array(0, $errors." tables not imported!", $img_path."folder_error.png");
   }
else {
   $list["jxresultset"] = array(0, $tabs." tables imported from ".$data_path, $img_path."folder.png");
   }
o2tree_def("result", $list, 1, "result_select"); "Import"jxconnectivity�    'hidden' jxconnectivity�   �  $in       = (o2val('prg�_�var','node_type') == "db");
$db       = substr(o2val('prg�_�var','table'), 5);
$list     = array();
$img_path = "<jx>/img/tabadmin/tree/";
$skip     = false;
if ($in) {
   foreach (o2app_tables() as $tab) {
      if (o2tab_db($tab) == $db) {
         if (o2tab_aspclear($tab)) {
            $list[$tab] = array("jxresultset",
                                $tab,
                                $img_path."tab.png");
            }
         else {
            $list[$tab] = array("jxresultset",
                                $tab." (skipped)",
                                $img_path."error.png");
            $skip       = true;
            }
         }
      }
   }
$list["jxresultset"] = array(0,
                             ($skip ? "Some tables skipped" : "All tables ASP cleared"),
                             $img_path.($skip ? "folder_error.png" : "folder.png"));
o2tree_def("result", $list, 1, "result_select"); "ASP clear" jxconnectivity�   �  $in       = (o2val('prg�_�var','node_type') == "db");
$db       = substr(o2val('prg�_�var','table'), 5);
$list     = array();
$img_path = "<jx>/img/tabadmin/tree/";
$skip     = false;
$tarea    = o2val('prg�_�var','clone_target_area');
if ($in) {
   foreach (o2app_tables() as $tab) {
      if (o2tab_db($tab) == $db) {
         if (o2tab_aspclone($tab, o2val('prg�_�var','clone_target_area'))) {
            $list[$tab] = array("jxresultset",
                                $tab,
                                $img_path."tab.png");
            }
         else {
            $list[$tab] = array("jxresultset",
                                $tab." (skipped)",
                                $img_path."error.png");
            $skip       = true;
            }
         }
      }
   }
$list["jxresultset"] = array(0,
                             ($skip ?
                              "Some tables skipped" :
                              "All tables ASP cloned to code \"".$tarea."\""),
                             $img_path.($skip ? "folder_error.png" : "folder.png"));
o2tree_def("result", $list, 1, "result_select"); "ASP clone" jxconnectivity�   �  $in       = (o2val('prg�_�var','node_type') == "db");
$db       = substr(o2val('prg�_�var','table'), 5);
$list     = array();
$img_path = "<jx>/img/tabadmin/tree/";
$mapped   = array();
$tabs     = 0;
if ($in) {
   foreach (o2app_tables() as $tab) {
      $mapped[$tab] = o2tab_name($tab);
      }
   foreach (o2db_tables($db) as $tab) {
      if (substr($tab, -6) == "_o2log") {
         $tab_src = array_search(substr($tab, 0, -6), $mapped);
         if ($tab_src && !o2tab_log($tab_src)) {
            $list[$tab] = array("jxresultset", $tab, $img_path."tab.png");
            $tabs++;
            }
         }
      elseif (!in_array($tab, $mapped)) {
         $list[$tab] = array("jxresultset", $tab, $img_path."tab.png");
         $tabs++;
         }
      }
   }
$list["jxresultset"] = array(0,
                             $tabs." unmapped tables in database",
                             $img_path."folder.png");
o2tree_def("result", $list, 1, "result_select"); "Unmapped tables"jxconnectivity�   . "Table ".o2val('prg�_�var','table')." created"jxconnectivity�   . "Table ".o2val('prg�_�var','table')." dropped"jxconnectivity�   . "Table ".o2val('prg�_�var','table')." rebuilt" jxconnectivity�   �  if (o2val('prg�_�var','operation') == "export") {
   $text = o2val('prg�_�var','affected_rows')." rows exported from table ".o2val('prg�_�var','table').
           " to file: ".o2val('prg�_�var','folder').o2val('prg�_�var','file_name');
   }
elseif (o2val('prg�_�var','operation') == "import") {
   $text = o2val('prg�_�var','affected_rows')." rows imported from file ".
           o2val('prg�_�var','folder').o2val('prg�_�var','file_name').
           " to table: ".o2val('prg�_�var','table');
   } $textjxconnectivity�     !o2zero('prg�_�var','file_name')jxconnectivity�   * o2tab_aspclear(o2val('prg�_�var','table'))jxconnectivity�   R o2tab_aspclone(o2val('prg�_�var','table'), o2val('prg�_�var','clone_target_area'))jxconnectivity�    "<jx>/img/tabadmin/get_def.png" jxconnectivity�   c   $list = o2val('prg�_�var','tables_list');
$vis  = ($list[o2val('prg�_�var','table')][4] == "SQL"); $vis jxconnectivity�   Z   $table = o2val('prg�_�var','table');
$def   = o2_file::get_def(o2tab_db($table), $table); $defjxconnectivity�    "getdef"jxconnectivity�   * o2val('prg�_�var','operation') == 'getdef'jxconnectivity�   - "<pre>".o2val('prg�_�var','get_def')."</pre>" jxconnectivity�     $trace = "";
$tab   = o2val('prg�_�var','table');
if (o2val('prg�_�var','node_type') == "table" ||
    o2val('prg�_�var','node_type') == "field" ||
    o2val('prg�_�var','node_type') == "key") {
    $trace = o2tab_trace($tab);
    $trace = ($trace ? $trace : "None");
    } $tracejxconnectivity�   X (o2val('prg�_�var','show_result') == "Check") && (count(o2tree_get_nodes("result")) > 1) jxconnectivity�   2  $errors_tab = o2tree_get_nodes("result");
$in         = (o2val('prg�_�var','node_type') == "db");
$list       = array();
$img_path   = "<jx>/img/tabadmin/tree/";
$tabs       = 0;
if ($in) {
   foreach ($errors_tab as $tab) {
      if ($tab[0] === "jxresultset") {
         switch ($tab[3]) {
            case 'index':
               o2tab_indexes_setoff($tab[1]);
               if (o2tab_indexes_seton($tab[1])) {            
                  $list[$tab[1]] = array("jxresultset", $tab[1], $img_path."tab.png");
                  $tabs++;
                  }
               break;
            case 'fkey':
               o2tab_fkeys_setoff($tab[1]);
               if (o2tab_fkeys_seton($tab[1])) {            
                  $list[$tab[1]] = array("jxresultset", $tab[1], $img_path."tab.png");
                  $tabs++;
                  }
               break;
            case 'error':
            default:
               if (o2tab_rebuild($tab[1])) {
                  $list[$tab[1]] = array("jxresultset", $tab[1], $img_path."tab.png");
                  $tabs++;
                  }
               break;
            }
         }
      }
   }
$list["jxresultset"] = array(0, $tabs." tables with errors fixed", $img_path."folder.png");
o2tree_def("result", $list, 1, "result_select");	 "Rebuild"jxconnectivity�   b (o2val('prg�_�var','show_result') == "Unmapped tables") && (count(o2tree_get_nodes("result")) > 1) jxconnectivity�   �  $unmapped_tabs = o2tree_get_nodes("result");
$in            = (o2val('prg�_�var','node_type') == "db");
$list          = array();
$img_path      = "<jx>/img/tabadmin/tree/";
$tabs          = 0;
$app           = $_SESSION['o2_app'];
$db            = $app->db[substr(o2val('prg�_�var','table'), 5)];
if ($in) {
   foreach ($unmapped_tabs as $tab) {
      if ($tab[0] === "jxresultset") {
         o2_gateway::droptable($db->server->type,
                               $db->server->server,
                               $db->server->user,
                               $db->server->password,
                               $db->nome,
                               $db->proprietario,
                               $tab[1]);
         $list[$tab[1]] = array("jxresultset", $tab[1], $img_path."tab.png");
         $tabs++;
         }
      }
   }
$list["jxresultset"] = array(0, $tabs." unmapped tables dropped", $img_path."folder.png");
o2tree_def("result", $list, 1, "result_select"); "Drop" jxconnectivity�   A   $m = "ATTENTION: all unmapped tables will be dropped.\nProceed?"; $mjxconnectivity�   ' o2val('prg�_�var','new_c_new_main_tab')jxconnectivity�   * o2val('prg�_�var','new_c_new_main_fields')jxconnectivity�   5 "Add constraint to table ".o2val('prg�_�var','table')jxconnectivity�   ( o2val('prg�_�var','show_constr_details')jxconnectivity�   * o2val('prg�_�var','new_c_main_all_fields')jxconnectivity�   ) o2val('prg�_�var','new_c_main_all_field')jxconnectivity�   % o2val('prg�_�var','new_c_main_field')jxconnectivity�   & o2val('prg�_�var','new_c_main_fields')jxconnectivity�   & o2val('prg�_�var','new_c_link_fields')jxconnectivity�   ) o2val('prg�_�var','new_c_link_all_field')jxconnectivity�   % o2val('prg�_�var','new_c_link_field')jxconnectivity�   * o2val('prg�_�var','new_c_link_all_fields')jxconnectivity�    "jxwiz_cancel"jxconnectivity�    "jxwiz_save"jxconnectivity�    0jxconnectivity   ' count(o2val('prg�_�var','tables_list'))jxconnectivity  F o2val('prg�_�var','tables_index') >= o2val('prg�_�var','tables_total') jxconnectivity  @   o2tree_node_select("tables", o2val('prg�_�var','cons_loop_db')); truejxconnectivity  C o2val('prg�_�var','res_count')." constraints activated on database" jxconnectivity  u  if (o2val('prg�_�var','cons_loop') || o2val('prg�_�var','cons_loop_db')) {
   $idx = o2val('constraints','linked_table').'|'.o2val('constraints','main_fields');
   }
else {
   $list     = o2val('prg�_�var','tables_list');
   $node     = $list[o2val('prg�_�var','field')];
   $link_tab = $node[4];
   $fields   = $node[5];
   $idx      = $link_tab.'|'.$fields;
   } $idxjxconnectivity  " o2val('prg�_�var','res_count') + 1 jxconnectivity  `  $c_idx  = o2val('prg�_�var','cons_index');
$c_list = o2val('prg�_�var','tab_constraints');
$cons   = $c_list["C"][$c_idx];
$name   = $cons[3];
$on     = o2val('prg�_�var','cons_read_list', $name);
$msg    = "Constraint <i>".$name."</i> ".
          ($on ? "removed from" : "enabled on").
          " table <i>".o2val('prg�_�var','table')."</i>"; $msgjxconnectivity  " !o2val('prg�_�var','cons_loop_db')jxconnectivity  % array("C" => array(), "R" => array()) jxconnectivity	  �   $active_cons = o2val('prg�_�var','cons_read_list');
$is_active   = isset($active_cons[o2val('constraints','constraint_name')]);
$cond        = $is_active !== o2val('prg�_�var','cons_loop_active'); $cond jxconnectivity
  �   $int_c = count(o2val('prg�_�var','new_c_main_fields'));
$ext_c = count(o2val('prg�_�var','new_c_link_fields'));
$ok = $int_c && $int_c == $ext_c; $okjxconnectivity  ( o2val('prg�_�var','new_c_check_data_ok')jxconnectivity  ) !o2val('prg�_�var','new_c_check_data_ok') jxconnectivity  <   $msg = "Select the same number of fields from both tables!"; $msg jxconnectivity  '  $list     = o2val('prg�_�var','tables_list');
$idx      = o2val('prg�_�var','tables_index');
$key_list = array_keys($list);
$node     = $list[$key_list[$idx]];
if ($node[3] == "table" && $node[0] == o2val('prg�_�var','cons_loop_db')) {
   $tab = $node[1];
   }
else {
   $tab = "";
   } $tabjxconnectivity  % o2val('prg�_�var','tables_index') + 1 jxconnectivity  �   $fields = o2tab_fields(o2val('prg�_�var','table'));
$list   = array_combine($fields, $fields);
if (isset($list['O2ASPID'])) {
   unset($list['O2ASPID']);
   } $listjxconnectivity   array() jxconnectivity  3   $name = "jxc_".substr(md5(microtime(true)), 0, 26); $name jxconnectivity  �   $c_idx  = o2val('prg�_�var','cons_index');
$c_list = o2val('prg�_�var','tab_constraints');
$cons   = $c_list["C"][$c_idx];
$name   = $cons[3];
$cond   = o2val('prg�_�var','cons_read_list', $name); $cond jxconnectivity  T   $list = o2val('prg�_�var','tables_list');
unset($list[o2val('prg�_�var','field')]); $list jxconnectivity  =   $list = implode(",", o2val('prg�_�var','new_c_main_fields')); $listjxconnectivity   o2view_rec("new_constraint")jxconnectivity  + !o2val('prg�_�var','new_c_check_duplicate')jxconnectivity  * o2val('prg�_�var','new_c_check_duplicate') jxconnectivity  �   $msg = "Sorry, you can't create it.\n".
       "A constraint already exists for the selected set of fields of this table.\n".
       "Please remove the existing constraint before creating a new one."; $msgjxconnectivity  E o2val('prg�_�var','res_count')." constraints deactivated on database"jxconnectivity  # o2val('prg�_�var','new_c_link_tab')jxconnectivity  4 implode(",", o2val('prg�_�var','new_c_link_fields'))jxconnectivity   o2val('prg�_�var','new_c_name')jxconnectivity  . o2val('prg�_�var','node_type') == "constraint"jxconnectivity  - o2val('prg�_�var','node_type') == "reference"jxconnectivity    "C" jxconnectivity!  v   $list  = o2val('prg�_�var','tables_list');
$field = o2val('prg�_�var','field');
$tab   = $list[$list[$field][0]][0]; $tab jxconnectivity"  �   $list   = o2val('prg�_�var','tables_list');
$constr = o2val('prg�_�var','field');
$idx   = $list[$constr][4]."|".$list[$constr][5]; $idxjxconnectivity#  _ o2val('prg�_�var','tab4check') != o2val('prg�_�var','table') && !o2val('prg�_�var','cons_loop') jxconnectivity$  b  $list       = o2val('prg�_�var','tab_constraints');
$ext_tab    = o2val('constraints','linked_table');
$int_fields = o2val('constraints','main_fields');
$ext_fields = o2val('constraints','linked_fields');
$c_name     = o2val('constraints','constraint_name');
$list["C"][$ext_tab."|".$int_fields] = array($ext_tab, $int_fields, $ext_fields, $c_name); $list jxconnectivity%  \  $list       = o2val('prg�_�var','tab_constraints');
$ext_tab    = o2val('references','main_table');
$int_fields = o2val('references','main_fields');
$ext_fields = o2val('references','linked_fields');
$c_name     = o2val('references','constraint_name');
$list["R"][$ext_tab."|".$int_fields] = array($ext_tab, $int_fields, $ext_fields, $c_name); $list jxconnectivity&  �   $list = o2val('prg�_�var','new_c_link_fields');
$new_field = o2val('prg�_�var','new_c_link_all_field');
$list[$new_field] = $new_field; $list jxconnectivity'  m   $list = o2val('prg�_�var','new_c_link_all_fields');
unset($list[o2val('prg�_�var','new_c_link_all_field')]); $list jxconnectivity(  �   $list = o2val('prg�_�var','new_c_main_fields');
$new_field = o2val('prg�_�var','new_c_main_all_field');
$list[$new_field] = $new_field; $list jxconnectivity)  m   $list = o2val('prg�_�var','new_c_main_all_fields');
unset($list[o2val('prg�_�var','new_c_main_all_field')]); $list jxconnectivity*  �   $list = o2val('prg�_�var','new_c_link_all_fields');
$new_field = o2val('prg�_�var','new_c_link_field');
$list[$new_field] = $new_field; $list jxconnectivity+  e   $list = o2val('prg�_�var','new_c_link_fields');
unset($list[o2val('prg�_�var','new_c_link_field')]); $list jxconnectivity,  �   $list = o2val('prg�_�var','new_c_main_all_fields');
$new_field = o2val('prg�_�var','new_c_main_field');
$list[$new_field] = $new_field; $list jxconnectivity-  e   $list = o2val('prg�_�var','new_c_main_fields');
unset($list[o2val('prg�_�var','new_c_main_field')]); $list jxconnectivity.  �   $fields = o2tab_fields(o2val('prg�_�var','new_c_link_tab'));
$list = array_combine($fields, $fields);
if (isset($list['O2ASPID'])) {
   unset($list['O2ASPID']);
   } $list jxconnectivity/  �  $list     = array();
$table    = o2val('prg�_�var','table');
$phys_tab = o2tab_name($table);
$db       = o2tab_db($table);
$owner    = o2db_schema($db);
$server   = o2db_server($db);
$query    = "select constraint_name from all_constraints where owner='".$owner.
            "' and table_name='".$phys_tab."' and constraint_type='R'";
foreach (o2dbs_query($server, $query) as $c_name) {
   $list[$c_name["CONSTRAINT_NAME"]] = $c_name["CONSTRAINT_NAME"];
   } $list jxconnectivity0  �  $list     = array();
$table    = o2val('prg�_�var','table');
$phys_tab = o2tab_name($table);
$db       = o2tab_db($table);
$phys_db  = o2db_name($db);
$owner    = o2db_schema($db);
$server   = o2db_server($db);
$query    = "SELECT conname \"CN\" FROM pg_catalog.pg_constraint WHERE conrelid = '\"".
            $phys_db.'"."'.$owner.'"."'.$phys_tab."\"'::regclass AND contype = 'f'";
foreach (o2dbs_query($server, $query) as $c_name) {
   $list[$c_name["CN"]] = $c_name["CN"];
   } $list jxconnectivity1  F   $db = o2dbs_engine(o2db_server(o2tab_db(o2val('prg�_�var','table')))); $dbjxconnectivity2  8 "Create new index for table ".o2val('prg�_�var','table')jxconnectivity3  ' o2val('prg�_�var','show_exidx_details')jxconnectivity4   !o2val('prg�_�var','cons_loop') jxconnectivity5  `  $c_idx  = o2val('prg�_�var','cons_index');
$c_list = o2val('prg�_�var','tab_constraints');
$cons   = $c_list["C"][$c_idx];
$name   = $cons[3];
$on     = o2val('prg�_�var','cons_read_list', $name);
$msg    = "Constraint <i>".$name."</i> ".
          ($on ? "removed from" : "enabled on").
          " table <i>".o2val('prg�_�var','table')."</i>"; $msg jxconnectivity6  �  $c_idx    = o2val('prg�_�var','cons_index');
$c_list   = o2val('prg�_�var','tab_constraints');
$cons     = $c_list["C"][$c_idx];
$name     = $cons[3];
$table    = o2val('prg�_�var','table');
$phys_tab = o2tab_name($table);
$db       = o2tab_db($table);
$owner    = o2db_schema($db);
$server   = o2db_server($db);
$on       = o2val('prg�_�var','cons_read_list', $name);

/*   Switch constraint status   */
if ($on) {
   $query = 'ALTER TABLE "'.$owner.'"."'.$phys_tab.'" drop constraint "'.$name.'"';
   }
else {
   /*   Referenced table   */
   $ref_tab  = $cons[0];
   $ref_phys = o2tab_name($ref_tab);
   /*   Add O2ASPID if present in both tables   */
   $aspid = (o2tab_asp($table) && o2tab_asp($ref_tab) ? "O2ASPID" : "");
   /*   Internal constraint fields   */
   $main_fields = $aspid;
   foreach (explode(",", $cons[1]) as $field) {
      $main_fields.= ($main_fields ? "," : "").
                     '"'. o2field_name($table, $field).'"';
      }
   /*   External referenced fields   */
   $ref_fields = $aspid;
   foreach (explode(",", $cons[2]) as $field) {
      $ref_fields.= ($ref_fields ? "," : "").
                    '"'. o2field_name($ref_tab, $field).'"';
      }
   /*   Compose constraint creation command   */
   $query = 'ALTER TABLE "'.$owner.'"."'.$phys_tab.'" ADD CONSTRAINT "'.
            $name.'" FOREIGN KEY ('.$main_fields.') REFERENCES "'.
            $owner.'"."'.$ref_phys.'" ('.$ref_fields.') ENABLE NOVALIDATE';
   }
o2dbs_execute($server, $query); true jxconnectivity7  $  $c_idx    = o2val('prg�_�var','cons_index');
$c_list   = o2val('prg�_�var','tab_constraints');
$cons     = $c_list["C"][$c_idx];
$name     = $cons[3];
$table    = o2val('prg�_�var','table');
$phys_tab = o2tab_name($table);
$db       = o2tab_db($table);
$phys_db  = o2db_name($db);
$owner    = o2db_schema($db);
$server   = o2db_server($db);
$on       = o2val('prg�_�var','cons_read_list', $name);

/*   Switch constraint status   */
if ($on) {
   $query = 'ALTER TABLE "'.$phys_db.'"."'.$owner.'"."'.$phys_tab.'" drop constraint "'.$name.'"';
   }
else {
   /*   Referenced table   */
   $ref_tab  = $cons[0];
   $ref_phys = o2tab_name($ref_tab);
   /*   Add O2ASPID if present in both tables   */
   $aspid = (o2tab_asp($table) && o2tab_asp($ref_tab) ? "O2ASPID" : "");
   /*   Internal constraint fields   */
   $main_fields = $aspid;
   foreach (explode(",", $cons[1]) as $field) {
      $main_fields.= ($main_fields ? "," : "").
                     '"'. o2field_name($table, $field).'"';
      }
   /*   External referenced fields   */
   $ref_fields = $aspid;
   foreach (explode(",", $cons[2]) as $field) {
      $ref_fields.= ($ref_fields ? "," : "").
                    '"'. o2field_name($ref_tab, $field).'"';
      }
   /*   Compose constraint creation command   */
   $query = 'ALTER TABLE "'.$owner.'"."'.$phys_tab.'" ADD CONSTRAINT "'.
            $name.'" FOREIGN KEY ('.$main_fields.') REFERENCES "'.
            $owner.'"."'.$ref_phys.'" ('.$ref_fields.') MATCH FULL NOT VALID';
   }
o2dbs_execute($server, $query); true jxconnectivity8    $c_idx  = o2val('prg�_�var','cons_index');
$c_list = o2val('prg�_�var','tab_constraints');
$cons   = $c_list["C"][$c_idx];
$name   = $cons[3];
$msg    = "Constraint <i>".$name."</i> validated against records on table <i>".
          o2val('prg�_�var','table')."</i>"; $msg jxconnectivity9  |  $c_idx    = o2val('prg�_�var','cons_index');
$c_list   = o2val('prg�_�var','tab_constraints');
$cons     = $c_list["C"][$c_idx];
$name     = $cons[3];
$table    = o2val('prg�_�var','table');
$phys_tab = o2tab_name($table);
$db       = o2tab_db($table);
$owner    = o2db_schema($db);
$server   = o2db_server($db);
$on       = o2val('prg�_�var','cons_read_list', $name);

/*   Validate only active constraints   */
if ($on) {
   /*   Compose constraint validation command   */
   $query = 'ALTER TABLE "'.$owner.'"."'.$phys_tab.'" MODIFY CONSTRAINT "'.
            $name.'" VALIDATE';
   }
o2dbs_execute($server, $query); true jxconnectivity:  �  $c_idx    = o2val('prg�_�var','cons_index');
$c_list   = o2val('prg�_�var','tab_constraints');
$cons     = $c_list["C"][$c_idx];
$name     = $cons[3];
$table    = o2val('prg�_�var','table');
$phys_tab = o2tab_name($table);
$db       = o2tab_db($table);
$phys_db  = o2db_name($db);
$owner    = o2db_schema($db);
$server   = o2db_server($db);
$on       = o2val('prg�_�var','cons_read_list', $name);

/*   Validate only active constraints   */
if ($on) {
   /*   Compose constraint validation command   */
   $query = 'ALTER TABLE "'. $phys_db.'"."'.$owner.'"."'.$phys_tab.'" VALIDATE CONSTRAINT "'.
            $name.'"';
   }
o2dbs_execute($server, $query); true jxconnectivity;  @   o2tree_node_select("tables", o2val('prg�_�var','cons_loop_db')); truejxconnectivity<  C o2val('prg�_�var','res_count')." constraints validated on database"jxconnectivity=  & "<jx>/img/tabadmin/constraint_add.png"jxconnectivity>  ( "<jx>/img/tabadmin/constraint_seton.png"jxconnectivity?  ) "<jx>/img/tabadmin/constraint_setoff.png"jxconnectivity@  + "<jx>/img/tabadmin/constraint_validate.png" jxconnectivityA  h   $list = o2val('prg�_�var','tables_list');
$node = o2val('prg�_�var','field');
$name = $list[$node][7]; $name jxconnectivityB  h   $list = o2val('prg�_�var','tables_list');
$node = o2val('prg�_�var','field');
$name = $list[$node][1]; $name jxconnectivityC  �  $code = "";
if (o2val('prg�_�var','field_type') == "C") {
   $list = o2val('prg�_�var','tables_list');
   $node = $list[o2val('prg�_�var','field')];
   $intf = explode(",", $node[5]);
   $extf = explode(",", $node[6]);
   foreach ($intf as $fidx => $field) {
      if ($field) {
         $code.= $field."<img src='/janox/img/tabadmin/arrows.png' style='display:inline'>".$extf[$fidx]."<br>";
         }
      }
   } $codejxconnectivityD  & o2val('prg�_�var','field_type') == "R"jxconnectivityE  & o2val('prg�_�var','field_type') == "C"jxconnectivityF   "<jx>/img/tabadmin/drop.png" jxconnectivityG  x   $msg = "Constraint on table ".o2exp(322)." is going to be removed and its definition is going to be deleted.\nProceed?"; $msg jxconnectivityH  �   $list = o2val('prg�_�var','tables_list');
$node = o2val('prg�_�var','field');
$name = $list[$node][7];
$on  = o2val('prg�_�var','cons_read_list', $name);
$img = "<jx>/img/tabadmin/constraint_set".($on ? "off" : "on").".png"; $img jxconnectivityI  �   $list = o2val('prg�_�var','tables_list');
$node = o2val('prg�_�var','field');
$name = $list[$node][7];
$on  = o2val('prg�_�var','cons_read_list', $name);
$txt = ($on ? "Deactivate" : "Activate")." constraint"; $txt jxconnectivityJ  C   $fields = o2app_tables();
$list = array_combine($fields, $fields); $list jxconnectivityK  n   $list   = o2val('prg�_�var','tables_list');
$node   = $list[o2val('prg�_�var','field')];
$fields = $node[5]; $fields jxconnectivityL  r  $table  = o2val('prg�_�var','table');
$db     = o2tab_db($table);
$server = o2db_server($db);
if (o2tab_exists($table)) {
   $list   = o2_gateway::fkeystablist(o2dbs_engine($server),
                                      o2dbs_name($server),
                                      o2dbs_user($server),
                                      o2dbs_password($server),
                                      o2db_name($db),
                                      o2db_schema($db),
                                      o2tab_name($table));
   $list   = array_combine($list, $list);
   }
else {
   $list = array();
   } $list jxconnectivityM  ~  $c_idx  = o2val('prg�_�var','cons_index');
$c_list = o2val('prg�_�var','tab_constraints');
$cons   = $c_list["C"][$c_idx];
$name   = $cons[3];
$table  = o2val('prg�_�var','table');
$on     = o2val('prg�_�var','cons_read_list', $name);
/*   Switch constraint status   */
if ($on) {
   o2tab_fkey_setoff($table, $name);
   }
else {
   o2tab_fkey_seton($table, $name);
   } true jxconnectivityN    $c_idx  = o2val('prg�_�var','cons_index');
$c_list = o2val('prg�_�var','tab_constraints');
$cons   = $c_list['C'][$c_idx];
$name   = $cons[3];
$on     = o2val('prg�_�var','cons_read_list', $name);
$res    = false;
/*   Validate only active constraints   */
if ($on) {
   $table    = o2val('prg�_�var','table');
   $phys_tab = o2tab_name($table);
   $db       = o2tab_db($table);
   $phys_db  = o2db_name($db);
   $owner    = o2db_schema($db);
   $server   = o2db_server($db);
   $type     = o2dbs_engine($server);
   $res      = o2_gateway::fkeyvalidate($type,
                                        o2dbs_name($server),
                                        o2dbs_user($server),
                                        o2dbs_password($server),
                                        $phys_db,
                                        o2db_schema($db),
                                        $phys_tab,
                                        $name);
   if (!$res) {
      o2dbs_commit($server);
      }
   } true jxconnectivityO  5   $ret = o2tab_fkeys_seton(o2val('prg�_�var','table')); $retjxconnectivityP  d o2val('prg�_�var','res_count')." constraints enabled on table <i>".o2val('prg�_�var','table')."</i>" jxconnectivityQ  6   $ret = o2tab_fkeys_setoff(o2val('prg�_�var','table')); $retjxconnectivityR  f o2val('prg�_�var','res_count')." constraints removed from table <i>".o2val('prg�_�var','table')."</i>" jxconnectivityS  ?   $ret = o2db_fkeys_seton(substr(o2val('prg�_�var','table'), 5)); $ret jxconnectivityT  @   $ret = o2db_fkeys_setoff(substr(o2val('prg�_�var','table'), 5)); $retjxconnectivityU  / o2val('prg�_�var','node_type') == "extra_index"jxconnectivityV  & o2val('prg�_�var','new_ei_all_fields')jxconnectivityW  & o2val('prg�_�var','new_ei_new_fields') jxconnectivityX  h   $list = o2val('prg�_�var','tables_list');
$node = o2val('prg�_�var','field');
$name = $list[$node][4]; $name jxconnectivityY  �   $list = o2val('prg�_�var','tables_list');
$node = $list[o2val('prg�_�var','field')];
if (is_array($node[6])) {
   $code = implode("<br>", $node[6]);
   }
else {
   $code = '';
   } $code jxconnectivityZ  m   $on  = o2val('prg�_�var','index_is_on');
$img = "<jx>/img/tabadmin/ext_idx_set".($on ? "off" : "on").".png"; $imgjxconnectivity[  & o2val('prg�_�var','field_type') == "E"jxconnectivity\   "E" jxconnectivity]  b   $on  = o2val('prg�_�var','index_is_on');
$txt = ($on ? "Deactivate" : "Activate")." extra index"; $txt jxconnectivity^  �   $str = '';
foreach (o2val('prg�_�var','new_ei_fields') as $field) {
   list($dir, $name) = explode('| ', $field);
   $str.= ($str ? ',' : '').$name.'|'.$dir;
   } $strjxconnectivity_  & o2val('prg�_�var','new_ei_check_data')jxconnectivity`  ' !o2val('prg�_�var','new_ei_check_data') jxconnectivitya  �   $msg = "Sorry, you can't create it.\n".
       "An index already exists for the selected set of fields of this table.\n".
       "Please remove the existing index before creating a new one."; $msg jxconnectivityb  n   $list   = o2val('prg�_�var','tables_list');
$node   = $list[o2val('prg�_�var','field')];
$fields = $node[5]; $fields jxconnectivityc  �  $node   = o2val('prg�_�var','tables_list', o2val('prg�_�var','field'));
$fields = $node[5];
$stru   = o2val('prg�_�var','phys_structure');
$idxs   = $stru['keys']['phys'] + $stru['keys']['more'];
$is_on  = false;
foreach ($idxs as $idx_name => $segs) {
   $idx_str = '';
   foreach ($segs as $seg) {
      $idx_str.= ($idx_str ? ',' : '').$seg['column'].'|'.$seg['dir'];
      }
   if ($idx_str == $fields) {
      $is_on = true;
      }
   } $is_on jxconnectivityd  s   $msg = "Index on table ".o2exp(322)." is going to be removed and its definition is going to be deleted.\nProceed?"; $msg jxconnectivitye  �   $list  = o2val('prg�_�var','tables_list');
$field = o2val('prg�_�var','field');
$idx   = $list[$field][4]."|".$list[$field][5]; $idxjxconnectivityf    o2val('prg�_�var','index_is_on') jxconnectivityg  �  $table  = o2val('prg�_�var','table');
$node   = o2val('prg�_�var','tables_list', o2val('prg�_�var','field'));
$name   = $node[4];
$on     = o2val('prg�_�var','index_is_on');
$unique = o2val('prg�_�var','index_is_unique');
/*   Switch index status   */
if ($on) {
   o2tab_index_remove($table, $name);
   }
else {
   $segs = array();
   $fields = o2val('prg�_�var','phys_structure', 'keys', 'def', $name, 'segments');
   foreach ($fields as $seg) {
      $segs[$seg['column']] = $seg['dir'];
      }
   /*
   if (o2val('prg�_�var','field_type') == 'K') {
      $fields = o2val('prg�_�var','phys_structure', 'keys', 'def', $name, 'segments');
      foreach ($fields as $seg) {
         $segs[$seg['column']] = $seg['dir'];
         }
      }   
   else {
      foreach (explode(',', $node[5]) as $seg) {
         list($seg_name, $seg_dir) = explode('|', $seg);
         $segs[$seg_name] = $seg_dir;
         }
      }
   */
   o2tab_index_add($table, $name, $segs, $unique);
   } true jxconnectivityh  ,  $table = o2val('prg�_�var','table');
$node  = o2val('prg�_�var','tables_list', o2val('prg�_�var','field'));
$name  = $node[4];
$on     = o2val('prg�_�var','index_is_on');
$msg    = "Index <i>".$name."</i> ".
          ($on ? "removed from" : "enabled on").
          " table <i>".$table."</i>"; $msgjxconnectivityi  P o2val('prg�_�var','field_type') == 'K' || o2val('prg�_�var','field_type') == 'E' jxconnectivityj  �   $cond = o2val('prg�_�var','table') &&
        o2val('prg�_�var','phys_structure', 'exist') &&
        !o2val('prg�_�var','operation') &&
        o2val('prg�_�var','field_type') == 'E'; $condjxconnectivityk  % 'jximgbtn'.(o2exp(362) ? '' : '_dis')jxconnectivityl  : '<jx>/img/tabadmin/drop'.(o2exp(362) ? '' : '_dis').'.png' jxconnectivitym  :  /*if (o2val('prg�_�var','field_type') == 'K') { */
   $node  = o2val('prg�_�var','tables_list', o2val('prg�_�var','field'));
   $name  = $node[4];
   $idxs  = array_keys(o2val('prg�_�var','phys_structure', 'keys', 'phys'));
   $is_on = in_array($name, $idxs);
/*   
   }
else {
   $is_on = false;
   }
*/ $is_onjxconnectivityn  I !o2val('prg�_�var','cons_loop') && o2val('prg�_�var','field_type') == 'E'jxconnectivityo  I !o2val('prg�_�var','cons_loop') && o2val('prg�_�var','field_type') == 'K'jxconnectivityp  & o2val('prg�_�var','field_type') == 'K' jxconnectivityq  i   $node      = o2val('prg�_�var','tables_list', o2val('prg�_�var','field'));
$is_unique = $node[7] == 'U';
 $is_uniquejxconnectivityr  @ (o2val('prg�_�var','index_is_unique') ? 'UNIQUE' : 'NOT UNIQUE')jxconnectivitys  A "<jx>/img/tabadmin/ext_idx_add".(o2exp(155) ? "" : "_dis").".png"jxconnectivityt  % o2val('prg�_�var','new_ei_all_field')jxconnectivityu  ! o2val('prg�_�var','new_ei_field') jxconnectivityv  �   $list       = o2val('prg�_�var','new_ei_fields');
$new        = o2val('prg�_�var','new_ei_all_field');
$list[$new] = 'A| '.$new; $list jxconnectivityw  e   $list = o2val('prg�_�var','new_ei_all_fields');
unset($list[o2val('prg�_�var','new_ei_all_field')]); $list jxconnectivityx  �   $list = o2val('prg�_�var','new_ei_all_fields');
$new_field = o2val('prg�_�var','new_ei_field');
$list[$new_field] = $new_field; $list jxconnectivityy  ]   $list = o2val('prg�_�var','new_ei_fields');
unset($list[o2val('prg�_�var','new_ei_field')]); $listjxconnectivityz   'idx_'.o2uid()jxconnectivity{  " o2val('prg�_�var','new_ei_fields') jxconnectivity|  �   $list = o2val('prg�_�var','new_ei_fields');
$seg  = o2val('prg�_�var','new_ei_field');
list($dir, $name) = explode('| ', $list[$seg]);
if ($dir == 'D') {
   $label = 'Make ascending';
   }
else {
   $label = 'Make descending';
   } $labeljxconnectivity}  " o2val('prg�_�var','new_ei_unique')jxconnectivity~    o2val('prg�_�var','new_ei_name')jxconnectivity  & o2val('prg�_�var','new_ei_new_fields') jxconnectivity�    $ok = true;
$stru = o2val('prg�_�var','phys_structure');
$idxs = $stru['keys']['def'];
$new  = o2val('prg�_�var','new_ei_new_fields');
foreach ($idxs as $segs) {
   $str = '';
   foreach ($segs as $seg) {
      $str.= ($str ? ',' : '').$seg['column'].'|'.$seg['dir'];
      }
   if ($new == $str) {
      $ok = false;
      }
   }
$ok = $ok && !o2view_rec("new_extidx"); $ok jxconnectivity�  �   $list = o2val('prg�_�var','new_ei_fields');
$sele = o2val('prg�_�var','new_ei_field');
list($dir) = explode('| ', $list[$sele]);
if ($dir == 'D') {
   $list[$sele] = 'A| '.$sele;
   }
else {
   $list[$sele] = 'D| '.$sele;
   } $listjxconnectivity�  & o2val('prg�_�var','new_ei_check_data') jxconnectivity�  2  $ok   = true;
$name = o2val('prg�_�var','new_ei_name');
$stru = o2val('prg�_�var','phys_structure');
foreach (o2val('prg�_�var','tab_extra_indexes') as $idx) {
   if ($name == $idx[0]) {
      $ok = false;
      }
   }
if (in_array($name, array_keys($stru['keys']['def']))) {
   $ok = false;
   } $ok jxconnectivity�  �   $msg = 'An index with name "'.o2val('prg�_�var','new_ei_name').
       "\" already exists for this table.\nPlease change name."; $msgjxconnectivity�  ' !o2val('prg�_�var','new_ei_check_data') jxconnectivity�  (   o2ctrl_focus('eixdx_add', 'eixdx_name'); truejxconnectivity�  F o2val('prg�_�var','table') && o2tab_exists(o2val('prg�_�var','table')) jxconnectivity�  �   $tab_name = o2val('prg�_�var','table');
if ($table = $_SESSION['o2_app']->get_table($tab_name)) {
   $table->create_logtable(true);
   } truejxconnectivity�  
 'nowindow'jxconnectivity�  	 'nav_big'jxconnectivity    "jxtree jxtree_bottom"