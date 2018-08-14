<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_export_csv", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("user_options", False, "", "", 1);
o2def::view("custom_cols", False, "", "", 1);
o2def::view("tmp", False, "", "", 1);
o2def::view("default_options", False, "", "", 0);
o2def::act("add_column", "");
o2def::act("cancel", "");
o2def::act("check_custom_col", "");
o2def::act("close_sele_cols", "");
o2def::act("export", "");
o2def::act("load_columns", "");
o2def::act("preset_default", "");
o2def::act("read_options", "");
o2def::act("select_columns", "");
o2def::act("start", "");
o2def::act("write_custom_col", "");
o2def::act("write_custom_cols", "");
o2def::act("write_options", "");
o2def::form("o2sys_export_csv", "", False, "true", false);
o2def::form("o2sys_export_csv_2", "", False, "o2sys_export_csv_exp_44()", false);
o2def::par(1, "view_name","o2sys_long_str");


?>
