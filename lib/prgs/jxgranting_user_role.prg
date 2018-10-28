<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_user_role", "", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("roles", False, "", "", 1);
o2def::act("add", "");
o2def::act("cancel", "");
o2def::form("user_role", "", False, "true", false);
o2def::par(1, "user_name", "_o2alpha");
o2def::par(2, "role_name", "_o2alpha");
o2def::par(3, "add_role_ok", "_o2logical");

?>
