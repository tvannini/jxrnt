<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_right", "", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("rights", False, "", "", 1);
o2def::act("cancel", "");
o2def::act("check", "");
o2def::act("create", "");
o2def::form("right", "", False, "true", false);
o2def::par(1, "right_code", "_o2alpha");
o2def::par(2, "right_ok", "_o2logical");
o2def::par(3, "right_desc", "_o2text");

?>
