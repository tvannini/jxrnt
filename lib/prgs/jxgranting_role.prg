<?php
//2.2
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_role", "", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("roles", False, "", "", 1);
o2def::view("model_roles", False, "", "", 1);
o2def::view("model_role_rights", False, "", "", 1);
o2def::view("model_role_users", False, "", "", 1);
o2def::view("role_rights", False, "", "", 1);
o2def::view("role_users", False, "", "", 1);
o2def::act("add_model_right", "");
o2def::act("add_model_user", "");
o2def::act("cancel", "");
o2def::act("check", "");
o2def::act("create", "");
o2def::act("on_change_model", "");
o2def::form("role", "", False, "true", false);
o2def::par(1, "role_name","_o2alpha");
o2def::par(2, "role_ok","_o2logical");
o2def::par(3, "role_desc","_o2text");

?>
