<?php
//2.2
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxdbcopy", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("source", False, "", "", 1);
o2def::view("target", False, "", "", 1);
o2def::act("execute", "_internal");
o2def::act("export_all_aspids", "");
o2def::act("loop_on_aspid", "");
o2def::act("loop_on_rows", "");
o2def::act("loop_on_tabs", "");
o2def::act("start", "");
o2def::par(1, "action","_o2alpha");
o2def::par(2, "jxdb","_o2alpha");
o2def::par(3, "connect_type","_o2alpha");
o2def::par(4, "connect_host","_o2alpha");
o2def::par(5, "connect_user","_o2alpha");
o2def::par(6, "connect_password","_o2alpha");
o2def::par(7, "connect_schema","_o2alpha");
o2def::par(8, "connect_db","_o2alpha");
o2def::par(9, "aspid","_o2alpha");
?>
