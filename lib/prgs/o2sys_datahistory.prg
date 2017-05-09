<?php
//2.3
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_datahistory", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("close", "");
o2def::act("start", "");
o2def::form("o2sys_datahistory", "", False, "true", false);
o2def::par(1, "log_set","_o2structure");
o2def::par(2, "show_record_data","_o2logical");

?>
