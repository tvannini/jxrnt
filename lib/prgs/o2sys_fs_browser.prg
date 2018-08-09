<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_fs_browser", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("close_form_fileget", "");
o2def::act("close_form_fileupload", "");
o2def::act("close_form_newdir", "");
o2def::act("create_dir", "");
o2def::act("file_add", "");
o2def::act("file_del", "");
o2def::act("file_get", "");
o2def::act("file_ren", "");
o2def::act("file_select", "");
o2def::act("folder_del", "");
o2def::act("folder_up", "");
o2def::act("load_file_list", "");
o2def::act("load_folder_list", "");
o2def::act("load_from_folder_limit", "");
o2def::act("loop_from_folder_limit", "");
o2def::act("order_by", "");
o2def::act("req_newdir_name", "");
o2def::act("send_file", "");
o2def::act("set_folder", "");
o2def::act("set_folder_by_tree", "");
o2def::act("start", "");
o2def::form("browser", "", False, "o2sys_fs_browser_exp_69()", false);
o2def::form("add_folder", "", False, "o2sys_fs_browser_exp_14()", false);
o2def::form("add_file", "", False, "o2sys_fs_browser_exp_23()", false);
o2def::form("visor", "", False, "o2sys_fs_browser_exp_28()", "o2sys_fs_browser_exp_30()");
o2def::par(1, "start_folder_path","o2sys_long_str");
o2def::par(2, "start_filter","o2sys_long_str");
o2def::par(3, "folder_limit","o2sys_long_str");
o2def::par(4, "for_file_selection","_o2logical");
o2def::par(5, "for_folder_selection","_o2logical");
o2def::par(6, "allow_mod","_o2logical");
o2def::par(7, "allow_get","_o2logical");
o2def::par(8, "allow_upload","_o2logical");
o2def::par(9, "window_title","o2sys_long_str");




?>
