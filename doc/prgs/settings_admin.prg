<?php
//2.1
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("settings_admin", "set_defaults", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("settings", False, "", "confirm");
o2def::act("confirm", "");
o2def::act("set_default", "");
o2def::act("set_defaults", "");
o2def::act("settings_undo", "");
o2def::form("unique", "", False, "true", false);

?>
