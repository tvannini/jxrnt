<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_pwd", "", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("cancel", "");
o2def::act("ok", "");
o2def::act("save", "");
o2def::act("wrong", "");
o2def::form("password", "", False, "true", false);
o2def::par(1, "user_name","_o2alpha");
o2def::par(2, "password","_o2alpha");
o2def::par(3, "save_pwd","_o2logical");

?>
