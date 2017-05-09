<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_table_set_log", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("log_tables", False, "", "");
o2def::act("create_logtab", "");
o2def::act("create_record", "");
o2def::act("set_level_change", "");
o2def::act("set_level_delete", "");
o2def::act("set_level_existence", "");
o2def::act("set_level_insert", "");
o2def::act("set_level_modify", "");
o2def::act("set_level_record", "");
o2def::act("start", "");
o2def::act("unset_log", "");
o2def::par(1, "table_name","o2sys_long_str");
o2def::par(2, "log_level","o2sys_loglevel");
?>
