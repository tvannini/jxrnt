<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_log_browser", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("log_view", False, "", "", 1);
o2def::view("users", False, "", "", 1);
o2def::view("logged_tables", False, "", "", 1);
o2def::view("source_tab", False, "", "", 1);
o2def::act("check_range", "");
o2def::act("close_all", "");
o2def::act("filter", "");
o2def::act("log_level", "");
o2def::act("restore_log", "");
o2def::act("save_log_level", "");
o2def::act("start", "");
o2def::form("o2sys_log_browser", "", False, "true", false);
o2def::form("o2sys_log_browser_2", "", False, "o2sys_log_browser_exp_25()", false);
o2def::par(1, "table_name","_o2text");
o2def::par(2, "source_tab","_o2text");


?>
