<?php
//1.5
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("get_settings", "settings", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("settings", False, "", "");
o2def::act("add_setting", "");
o2def::act("settings", "");
o2def::par(1, "settings","_o2structure");
?>
