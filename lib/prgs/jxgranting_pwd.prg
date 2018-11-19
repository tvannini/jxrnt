<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_pwd", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("user", False, "", "", 0);
o2def::act("cancel", "");
o2def::act("ok", "");
o2def::act("save", "");
o2def::act("start", "");
o2def::act("wrong_history", "");
o2def::act("wrong_old_pwd", "");
o2def::act("wrong_repeat", "");
o2def::form("password", "", False, "true", false);
o2def::par(1, "user_name", "_o2alpha");
o2def::par(2, "old_password", "_o2logical");
o2def::par(3, "set_get_result", "_o2logical");

?>
