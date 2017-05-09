<?php
//2.2
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_expire", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("cancel", "");
o2def::act("save", "");
o2def::act("start", "");
o2def::form("password", "", False, "true", false);
o2def::par(1, "user_name","_o2alpha");
o2def::par(2, "expire_date","_o2date");
o2def::par(3, "save_date","_o2logical");

?>
