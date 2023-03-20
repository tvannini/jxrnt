<?php
//2.9
//o2def::module("");
//o2def::folder("triggers");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_start", "start", "", __FILE__);

o2def::view('prg�_�var');
o2def::view("users", false, "", "", 1, 0);
o2def::view("sessions", false, "", "", 1, 0);
o2def::view("root_lookup", false, "", "", 1, 0);
o2def::view("options", false, "", "", 1, 0);
o2def::view("roles", false, "", "", 1, 0);
o2def::view("rights_per_role", false, "", "", 1, 0);
o2def::view("logged_tabs", false, "", "", 1, 0);
o2def::view("jxsys_lookup", false, "", "", 1, 0);
o2def::view("terminals", false, "", "", 0, 0);
o2def::view("otp", false, "", "", 0, 0);
o2def::act("check_session_terminal");
o2def::act("db_session");
o2def::act("insert_jxsys");
o2def::act("insert_root");
o2def::act("ldap_with_domain");
o2def::act("load_logged");
o2def::act("load_options");
o2def::act("load_rights");
o2def::act("load_role_rights");
o2def::act("login");
o2def::act("login_correct");
o2def::act("login_correct_ldap");
o2def::act("login_failed");
o2def::act("login_ldap");
o2def::act("login_std");
o2def::act("session");
o2def::act("start");
o2def::act("terminal_id");
o2def::par(1, "user_roles", "_o2structure");
o2def::par(2, "user_rights", "_o2structure");
o2def::par(3, "app_options", "_o2structure");
o2def::par(4, "user_options", "_o2structure");
o2def::par(5, "logged_tabs", "_o2structure");
o2def::par(6, "password_change", "_o2number");
?>
