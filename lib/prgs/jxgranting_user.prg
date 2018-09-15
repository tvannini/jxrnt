<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_user", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("users", False, "", "", 1);
o2def::view("role_per_user", False, "", "", 1);
o2def::view("model_roles", False, "", "", 1);
o2def::view("model_users", False, "", "", 1);
o2def::act("area_code_select", "");
o2def::act("cancel", "");
o2def::act("check_name", "");
o2def::act("copy_role", "");
o2def::act("copy_roles", "");
o2def::act("create", "");
o2def::act("free_area_code", "");
o2def::act("password_wrong", "");
o2def::act("start", "");
o2def::act("user_exists", "");
o2def::act("user_name_missing", "");
o2def::act("user_write", "");
o2def::form("user", "", False, "true", false);
o2def::par(1, "user_name","_o2alpha");
o2def::par(2, "asp_area","_o2alpha");
o2def::par(3, "password","_o2alpha");
o2def::par(4, "expire_date","_o2date");
o2def::par(5, "login_type","o2sys_login_type");
o2def::par(6, "create_ok","_o2logical");

?>
