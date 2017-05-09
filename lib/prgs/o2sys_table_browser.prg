<?php
//2.3
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_table_browser", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("browser", False, "", "", 1);
o2def::act("browser_delete", "");
o2def::act("browser_insert", "");
o2def::act("browser_post", "");
o2def::act("browser_undo", "");
o2def::act("close", "");
o2def::act("delete", "");
o2def::act("filter", "");
o2def::act("new", "");
o2def::act("refresh", "");
o2def::act("save", "");
o2def::act("start", "");
o2def::act("undo", "");
o2def::form("table_browser", "", False, "true", false);
o2def::par(1, "table_name","_o2text");
o2def::par(2, "table_index","_o2text");

?>
