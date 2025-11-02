<?php
//3.0
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxlist_select", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("tmp", false, "", "", 1, 0);
o2def::act("add_item");
o2def::act("cancel");
o2def::act("check_custom_col");
o2def::act("columns_select_all");
o2def::act("columns_unselect_all");
o2def::act("done");
o2def::act("load_items");
o2def::act("start");
o2def::form("jxlist_select_2", "", False, "true", false);
o2def::par(1, "list", "_o2structure");
o2def::par(2, "win_title", "_o2alpha");

?>
