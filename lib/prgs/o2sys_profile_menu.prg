<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_profile_menu", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("obj_rights", False, "", "");
o2def::view("app_rights", False, "", "");
o2def::act("exit", "");
o2def::act("obj_rights_delete", "");
o2def::act("obj_rights_insert", "");
o2def::act("obj_rights_post", "");
o2def::act("obj_rights_undo", "");
o2def::act("start", "");
o2def::form("o2sys_profile_menu_1", "", False, "true", false);
o2def::par(1, "menu_id","_o2alpha");

?>
