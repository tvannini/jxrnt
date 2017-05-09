<?php
//2.1
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("uses_edit", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("uses", False, "", "");
o2def::view("uses_id", False, "", "");
o2def::act("delete", "");
o2def::act("save", "");
o2def::act("set_title", "");
o2def::act("start", "");
o2def::act("undo", "");
o2def::form("unic", "", False, "true", false);
o2def::par(1, "use_id","use_id");

?>
