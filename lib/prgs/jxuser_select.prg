<?php
//2.8
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxuser_select", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("users", false, "", "", 0, 0);
o2def::act("close");
o2def::act("select");
o2def::act("start");
o2def::form("jxuser_select_1", "", False, "true", false);
o2def::par(1, "user", "_o2alpha");
o2def::par(2, "show_admin", "_o2logical");
o2def::par(3, "show_all_areas", "_o2logical");

?>
