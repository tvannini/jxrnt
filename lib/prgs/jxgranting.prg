<?php
//2.7
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxgranting", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("users", False, "", "", 1);
o2def::view("rights", False, "", "", 1);
o2def::view("roles", False, "", "", 1);
o2def::view("roles_per_user", False, "", "", 1);
o2def::view("rights_per_role", False, "", "", 1);
o2def::view("rights_per_user", False, "", "", 1);
o2def::view("users_per_role", False, "", "", 1);
o2def::view("roles_per_right", False, "", "", 1);
o2def::view("users_per_right", False, "", "", 1);
o2def::view("right_vis_profiling", False, "", "", 1);
o2def::view("right_enable_profiling", False, "", "", 1);
o2def::view("role_desc", False, "", "", 1);
o2def::view("right_desc", False, "", "", 1);
o2def::view("user_role_desc", False, "", "", 1);
o2def::view("user_right_desc", False, "", "", 1);
o2def::view("role_right_desc", False, "", "", 1);
o2def::view("right_role_desc", False, "", "", 1);
o2def::act("add_single_right2role");
o2def::act("change_login_type");
o2def::act("close_srv_windows");
o2def::act("edit_right_descr");
o2def::act("edit_role_descr");
o2def::act("fix");
o2def::act("goto_right");
o2def::act("goto_role");
o2def::act("goto_user");
o2def::act("password_edit");
o2def::act("right_create");
o2def::act("right_delete");
o2def::act("right_delete_enabprof");
o2def::act("right_delete_role");
o2def::act("right_delete_visprof");
o2def::act("right_write");
o2def::act("role_add_right");
o2def::act("role_create");
o2def::act("role_delete");
o2def::act("role_delete_right");
o2def::act("role_delete_user");
o2def::act("role_right_write");
o2def::act("role_right_write_multiple");
o2def::act("role_right_write_single");
o2def::act("role_write");
o2def::act("set_expire");
o2def::act("start");
o2def::act("switch_asp_folding");
o2def::act("user_add_role");
o2def::act("user_create");
o2def::act("user_delete");
o2def::act("user_delete_role");
o2def::act("user_detail");
o2def::act("user_role_write");
o2def::act("user_write");
o2def::form("jxgranting", "", False, "true", false);
o2def::par(1, "asp_code", "_o2alpha");
o2def::par(2, "user_name", "_o2alpha");
o2def::par(3, "role_name", "o2sys_role");
o2def::par(4, "right_name", "o2sys_right");

?>
