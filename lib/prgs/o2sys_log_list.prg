<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_log_list", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("loglist", False, "", "save");
o2def::act("close", "");
o2def::act("loglist_delete", "");
o2def::act("save", "");
o2def::act("start", "");
o2def::act("view_log", "");
o2def::form("o2sys_log_list", "", False, "true", false);
o2def::par(1, "db_filter","_o2alpha");

?>
