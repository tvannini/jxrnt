��       I      �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � �                                                                                   	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +          ,          -          .          /          0          1          2          3          4          5          6          7          8          9          :          ;          <          =          >          ?          @          A          B          C          D          E          F          G          H          I   -       o2sys_fs_browser   K	  $row_list = o2dir_list(o2val('prg�_�var','folder_path'),
                       o2val('prg�_�var','files_filter'));
$counter = -1;
$list    = array();
foreach ($row_list as $s_file) {
   $counter++;
   $list[$counter]   = array('fullname' => $s_file,
                             'name'     => o2file_basename($s_file).
                                           (!o2file_ext($s_file) ?
                                            "" :
                                            ".".o2file_ext($s_file)),
                             'type'     => o2file_type($s_file),
                             'dim'      => o2file_size($s_file),
                             'date'     => o2file_date($s_file),
                             'time'     => o2file_time($s_file));
   $bytype[$counter] = $list[$counter]['type'];
   $byname[$counter] = strtolower($list[$counter]['name']);
   $bysize[$counter] = $list[$counter]['dim'];
   $bydate[$counter] = $list[$counter]['date'];
   $bytime[$counter] = $list[$counter]['time'];
   }
if ($counter > -1) {
   switch (o2val('prg�_�var','sort_by')) {
      case "size":
         array_multisort($bytype, SORT_ASC, SORT_STRING,
                         $bysize, SORT_ASC, SORT_NUMERIC,
                         $list);
         break;
      case "sizedesc":
         array_multisort($bytype, SORT_ASC, SORT_STRING,
                         $bysize, SORT_DESC, SORT_NUMERIC,
                         $list);
         break;
      case "time":
         array_multisort($bytype, SORT_ASC, SORT_STRING,
                         $bydate, SORT_ASC, SORT_NUMERIC,
                         $bytime, SORT_ASC, SORT_NUMERIC,
                         $list);
         break;
      case "timedesc":
         array_multisort($bytype, SORT_ASC, SORT_STRING,
                         $bydate, SORT_DESC, SORT_NUMERIC,
                         $bytime, SORT_DESC, SORT_NUMERIC,
                         $list);
         break;
      case "namedesc":
         array_multisort($bytype, SORT_ASC, SORT_STRING,
                         $byname, SORT_DESC, SORT_STRING,
                         $list);
         break;
      default:
         array_multisort($bytype, SORT_ASC, SORT_STRING,
                         $byname, SORT_ASC, SORT_STRING,
                         $list);
         break;
      }
   } $listo2sys_fs_browser    urldecode($_REQUEST['extp_1'])o2sys_fs_browser    trueo2sys_fs_browser    (!o2par(2) ? "*" : o2par(2)) o2sys_fs_browser   V  if (o2par(1) != "") {
   if (is_dir(o2par(1))) {
      $folder = o2par(1);
      }
   else {
      $folder = dirname(o2par(1));
      }
   }
elseif (o2app_runlevel() == "DEV") {
   $folder = o2app_dir_home();
   }
elseif (o2app_user() != "root") {
   $folder = o2app_dir_user();
   }
else {
   $folder = o2app_dir_home();
   } $folder o2sys_fs_browser   �  $text = '<table cellspacing="0" cellpadding="0" class="fsb_tab"><tr><th class="fsb_header">&nbsp;</th>'.
         '<th class="fsb_header" style="cursor:pointer;" title="Click to sort by name" onClick="'.o2_act4js(0, "order_by", "name").'">Name</th>'.
         '<th class="fsb_header" style="cursor:pointer;" title="Click to sort by size" onClick="'.o2_act4js(0, "order_by", "size").'">Size</th>'.
         '<th class="fsb_header" style="cursor:pointer;" title="Click to sort by date and time" onClick="'.o2_act4js(0, "order_by", "time").'">Last modified</th><th class="fsb_header">&nbsp</th></tr>';

if (is_array(o2val('prg�_�var','file_list'))) {
   foreach (o2val('prg�_�var','file_list') as $file) {
      $selection  = o2val('prg�_�var','file_name') == $file['fullname'];
      $select_btn = '';
      $ren_btn    = '';
      $del_btn    = '';
      /* ___________________ If interface is for file or folder selection ___ */
      if ((o2par(4) && $file['type'] != 'D') || (o2par(5) && $file['type'] == 'D')) {
         $select_btn = '<img src="'.$GLOBALS['o2_runtime']->alias.'img/ctrl_select.png"'.
                             ' title="Select '.($file['type'] != "D" ? 'file' : 'folder').'"'.
                             ' onClick="'.o2_act4js(0,
                                                    'file_select',
                                                    $file['fullname']).'">';
         }
      /* ______________________________________________ If item is a file ___ */
      if (o2par(6)) {
         $del_btn =    '<img src="'.$GLOBALS['o2_runtime']->alias.
                                    'img/ctrl_delete.png"'.
                             ' title="Delete '.($file['type'] != "D" ?
                                                'file' :
                                                'folder').'"'.
                             ' onClick="'.o2_act4js(0,
                                                    ($file['type'] != "D" ?
                                                     'file_del' :
                                                     'folder_del'),
                                                    $file['fullname']).'return true;">';
         }
      $text.= '<tr style="cursor:pointer;" class="'.
                 ($selection ? 'fsb_sel' : 'fsb_row').
                 '" title="'.$file['name'].'"'.
                 ($file['type'] != "D" ?
                  (o2par(7) ? ' onClick="'.o2_act4js(0, "file_get", $file['fullname']).'"' : "") :
                  ' onClick="'.o2_act4js(0, "set_folder", $file['fullname']).'"'
                 ).
                 '><td class="fsb_icon"><img src="'.o2file_ico($file['fullname']).
                                           '"></td><td class="fsb_body">'.
                 $file['name'].'</td><td class="fsb_body" style="text-align:right;">'.
                 ($file['dim'] ?
                  o2format($file['dim'], "o2sys_full_number") :
                  '&nbsp;').'</td><td class="fsb_body" style="text-align:right;">'.
                  o2format($file['date'], "_o2date").'&nbsp;'.
                  o2format($file['time'], "_o2time").
                 '</td><td class="fsb_tools">'.
                 ($select_btn.$ren_btn.$del_btn ?
                  $select_btn.
                  $ren_btn.
                  $del_btn :
                  "&nbsp;").
                 '</td></tr>';
      }
   }
$text.= "</table>"; $texto2sys_fs_browser   , o2file_dir(o2val('prg�_�var','folder_path'))o2sys_fs_browser   9 (!o2par(3) ? o2val('prg�_�var','folder_path') : o2par(3)) o2sys_fs_browser	   �   $str = "> ".
       str_replace(o2file_dir(o2val('prg�_�var','folder_limit')), "", o2val('prg�_�var','folder_path')).
       " &nbsp;&nbsp;&nbsp; | &nbsp;&nbsp;&nbsp; ".count(o2val('prg�_�var','file_list'))." items listed"; $str o2sys_fs_browser
   
  $url = o2val('prg�_�var','action_path');
$msg = (@unlink($url) ?
       "File ".o2file_basename($url).(!o2file_ext($url) ? "" : ".".o2file_ext($url))." deleted" :
       "Cannot delete file ".o2file_basename($url).(!o2file_ext($url) ? "" : ".".o2file_ext($url))); $msgo2sys_fs_browser    o2par(7)o2sys_fs_browser    o2par(6)o2sys_fs_browser    o2par(8)o2sys_fs_browser     o2val('prg�_�var','form_newdir') o2sys_fs_browser   �  if (!o2val('prg�_�var','newdir_name')) {
   $error = 1;
   }
else {
   $error = (strcspn(trim(o2val('prg�_�var','newdir_name')), '\/:*?"<>|') == strlen(trim(o2val('prg�_�var','newdir_name'))) ? 0 : 1);
   }
if (!$error) {
   foreach (o2val('prg�_�var','file_list') as $s_file) {
      if ($s_file['name'] == o2val('prg�_�var','newdir_name')) {
         $error = 2;
         }
      }
   } $erroro2sys_fs_browser   ? "Cannot create directory!\\nPlease insert a valid folder name."o2sys_fs_browser   % o2val('prg�_�var','name_error') === 1 o2sys_fs_browser   ]   mkdir(o2val('prg�_�var','folder_path').DIRECTORY_SEPARATOR.o2val('prg�_�var','newdir_name')); trueo2sys_fs_browser     !o2val('prg�_�var','name_error')o2sys_fs_browser   l "Cannot create directory!\\nA file named [".o2val('prg�_�var','newdir_name')."] already exists in the path."o2sys_fs_browser   % o2val('prg�_�var','name_error') === 2o2sys_fs_browser   . o2dir_delete(o2val('prg�_�var','action_path'))o2sys_fs_browser   $ o2val('prg�_�var','form_fileupload')o2sys_fs_browser   - o2val('prg�_�var','uploaded_file') != array()o2sys_fs_browser    "*"o2sys_fs_browser   " o2zero('prg�_�var','files_filter') o2sys_fs_browser     if (file_exists(o2val('prg�_�var','uploaded_file','nome_completo'))) {
   rename(o2val('prg�_�var','uploaded_file','nome_completo'),
          o2val('prg�_�var','folder_path').DIRECTORY_SEPARATOR.o2val('prg�_�var','uploaded_file','nome_originale'));
   } trueo2sys_fs_browser   ! o2val('prg�_�var','form_fileget') o2sys_fs_browser   g   $text = '<a href="'.o2_path(o2val('prg�_�var','file_name')).'">'.o2val('prg�_�var','file_name').'</a>'; $texto2sys_fs_browser   ' o2_path(urldecode($_REQUEST['extp_1']))o2sys_fs_browser    "fsb_addfile"o2sys_fs_browser     "fsb_addfolder"o2sys_fs_browser!    "tools/o2sys_confirm" o2sys_fs_browser"   �   $full_path   = o2val('prg�_�var','action_path');
$folder_name = o2file_basename($full_path).
               (!o2file_ext($full_path) ? "" : ".".o2file_ext($full_path));
$msg = "Cannot delete folder ".$folder_name; $msgo2sys_fs_browser#     !o2val('prg�_�var','del_result')o2sys_fs_browser$   # o2val('prg�_�var','dialog_confirm')o2sys_fs_browser%   ( o2ctrl_focus("add_folder", "new_folder") o2sys_fs_browser&   �   $full_path   = o2val('prg�_�var','action_path');
$folder_name = o2file_basename($full_path).
               (!o2file_ext($full_path) ? "" : ".".o2file_ext($full_path));
$msg = "Delete folder ".$folder_name."?"; $msgo2sys_fs_browser'    urldecode($_REQUEST['extp_1'])o2sys_fs_browser(   + (o2par(9) ? o2par(9) : "Resources browser")o2sys_fs_browser)   	 'nav_big'o2sys_fs_browser*    o2tree_get_code("jxfsbrowser") o2sys_fs_browser+   q   $setto = urldecode($_REQUEST['extp_1']);
$sort = $setto.(o2val('prg�_�var','sort_by') !== $setto ? "" : "desc"); $sort o2sys_fs_browser,   �  $list = o2val('prg�_�var','folder_list');
$root = o2val('prg�_�var','folder_path');
if (isset($list["jxfsroot"])) {
   if ($root == $list["jxfsroot"][1]) {
      $root_node = "jxfsroot";
      }
   else {
      $root_node = $root;
      }
   }
else {
   $list             = array();
   $root_node        = "jxfsroot";
   $list[$root_node] = array(0, $root, o2file_ico($root, false, true));    
   }      
$raw_list = o2dir_list($root, o2val('prg�_�var','files_filter'));
foreach ($raw_list as $s_file) {
   if (o2file_type($s_file) == "D") {
      $f_name        = o2file_basename($s_file).(!o2file_ext($s_file) ? "" :
                                                 ".".o2file_ext($s_file));
      $f_name        = str_replace('\\', '/', $f_name);                                                                                                   
      $s_file        = str_replace('\\', '/', $s_file);                                                                                                   
      $list[$s_file] = array($root_node, $f_name, o2file_ico($s_file, false, true));
      }
   }   
o2tree_def("jxfsbrowser", $list, 1, "set_folder_by_tree"); $listo2sys_fs_browser-   
 "fsb_tree"o2sys_fs_browser.    isset($_REQUEST['extp_1']) o2sys_fs_browser/   �   $dir  = o2val('prg�_�var','folder_path');
$list = o2val('prg�_�var','folder_list');
if ($dir == $list["jxfsroot"][1]) {
   $dir = "jxfsroot";
   }
o2tree_node_select("jxfsbrowser", $dir); true o2sys_fs_browser0   �   $dir  = o2tree_get_selection("jxfsbrowser");
$list = o2val('prg�_�var','folder_list');
if ($dir == "jxfsroot") {
   $dir = $list["jxfsroot"][1];
   } $diro2sys_fs_browser1    o2val('prg�_�var','status_str')o2sys_fs_browser2   < "<jx>/img/fs/folder_up_btn".(o2exp(52) ? "" : "_dis").".png"o2sys_fs_browser3   ( (o2exp(52) ? "jximgbtn" : "o2_ctrl_img") o2sys_fs_browser4   �   $limit  = o2val('prg�_�var','folder_limit');
$path   = o2val('prg�_�var','folder_path');
$enable = $path != $limit && $path.DIRECTORY_SEPARATOR != $limit; $enable o2sys_fs_browser5     o2tree_fold("jxfsbrowser", 1);
$list = o2val('prg�_�var','folder_list');
$dir  = o2val('prg�_�var','folder_path');
if ($list["jxfsroot"][1] == $dir) {
   o2tree_node_select("jxfsbrowser", "jxfsroot");
   }
else {
   o2tree_node_select("jxfsbrowser", $dir);
   } trueo2sys_fs_browser6    "<jx>/img/fs/file_add_btn.png"o2sys_fs_browser7     "<jx>/img/fs/folder_add_btn.png"o2sys_fs_browser8   
 "jximgbtn"o2sys_fs_browser9   E o2val('prg�_�var','folder_limit') != o2val('prg�_�var','folder_path')o2sys_fs_browser:   ! o2val('prg�_�var','folder_limit')o2sys_fs_browser;   U strpos(o2val('prg�_�var','folder_limit'), o2val('prg�_�var','folder_path')) !== falseo2sys_fs_browser<     o2val('prg�_�var','folder_path')o2sys_fs_browser=   E o2val('prg�_�var','folder_limit') == o2val('prg�_�var','folder_path') o2sys_fs_browser>   �   $start = o2val('prg�_�var','folder_start');
$curr  = o2val('prg�_�var','folder_path');
$dir = substr($start, 0, strpos($start, DIRECTORY_SEPARATOR, strlen($curr)) + 1); $diro2sys_fs_browser?    'disp_area'o2sys_fs_browser@    0o2sys_fs_browserA   ! o2val('prg�_�var','folder_start')o2sys_fs_browserE   K !(o2val('prg�_�var','form_fileupload') || o2val('prg�_�var','form_newdir')) o2sys_fs_browserF   �   $url = o2val('prg�_�var','action_path');
$msg = 'Delete file '.o2file_basename($url).(!o2file_ext($url) ? "" : ".".o2file_ext($url)).'?'; $msgo2sys_fs_browserG    urldecode($_REQUEST['extp_1'])o2sys_fs_browserH    ''o2sys_fs_browserI   
 'nowindow'o2sys_fs_browserJ    'hidden'o2sys_fs_browser'   falseo2sys_fs_browser-    "fsb_tree jxtree_bottom"