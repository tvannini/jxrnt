<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxmlt_harvest", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("prgs", False, "", "", 0);
o2def::act("check_update", "");
o2def::act("harvest_menu", "");
o2def::act("harvest_prg", "");
o2def::act("harvest_update", "");
o2def::act("start", "");
o2def::par(1, "mode", "o2sys_generic_opt");
o2def::par(2, "item", "_o2alpha");
o2def::par(3, "descendant_prgs", "_o2logical");
o2def::par(4, "descend_to_level", "_o2number");
o2def::par(5, "lang", "o2sys_language");
?>
