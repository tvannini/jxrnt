<?php
//2.9
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting_user", "start", "", __FILE__);

o2def::view('prg�_�var');
o2def::view("users", false, "", "", 1, 0);
o2def::view("role_per_user", false, "", "", 1, 0);
o2def::view("model_roles", false, "", "", 1, 0);
o2def::view("model_users", false, "", "", 1, 0);
o2def::act("area_code_select");
o2def::act("cancel");
o2def::act("change_admin");
o2def::act("change_type");
o2def::act("check_name");
o2def::act("clear_pwd_history");
o2def::act("copy_role");
o2def::act("copy_roles");
o2def::act("create_new");
o2def::act("free_area_code");
o2def::act("plugin_area");
o2def::act("plugin_profile");
o2def::act("save");
o2def::act("start");
o2def::act("user_exists");
o2def::act("user_name_missing");
o2def::act("user_write");
o2def::form("user", "", False, "true", false);
o2def::par(1, "user_name", "jxuser");
o2def::par(2, "asp_area", "_o2alpha");
o2def::par(3, "expire_date", "_o2date");
o2def::par(4, "login_type", "o2sys_login_type");
o2def::par(5, "pwds_history", "o2sys_long_str");
o2def::par(6, "force_pwd_change", "_o2logical");
o2def::par(7, "no_pwd_change", "_o2logical");
o2def::par(8, "admin", "_o2logical");
o2def::par(9, "poweruser", "_o2logical");
o2def::par(10, "hidden", "_o2logical");
o2def::par(11, "create_ok", "_o2logical");

?>
