<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_role_right", "", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("rights4combo", False, "", "", 1);
o2def::view("rights4list", False, "", "", 1);
o2def::act("add", "");
o2def::act("add_multiple", "");
o2def::act("add_right_to_list", "");
o2def::act("add_single", "");
o2def::act("cancel", "");
o2def::act("select_all", "");
o2def::act("unselect_all", "");
o2def::form("jxgranting_role_right_1", "", False, "true", false);
o2def::par(1, "role_name", "_o2alpha");
o2def::par(2, "right_name", "_o2alpha");
o2def::par(3, "right_list", "_o2structure");
o2def::par(4, "right_ok", "_o2logical");

?>
