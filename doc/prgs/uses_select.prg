<?php
//2.1
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("uses_select", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("uses", False, "", "");
o2def::act("delete", "");
o2def::act("edit", "");
o2def::act("new", "");
o2def::act("select", "");
o2def::act("start", "");
o2def::form("unic", "", False, "true", false);
o2def::par(1, "use_id","use_id");

?>
