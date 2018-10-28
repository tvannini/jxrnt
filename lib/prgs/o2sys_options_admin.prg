<?php
//2.4
//o2def::module("");
//o2def::folder("options");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_options_admin", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("options", False, "", "", 1);
o2def::act("add_skin", "");
o2def::act("load_options", "");
o2def::act("load_skins", "");
o2def::act("mod_appmenu", "");
o2def::act("mod_grid_dblclick", "");
o2def::act("mod_grid_settings", "");
o2def::act("mod_grid_wheel", "");
o2def::act("mod_language", "");
o2def::act("mod_menu_help", "");
o2def::act("mod_menu_open_on", "");
o2def::act("mod_nav_effects", "");
o2def::act("mod_newsession", "");
o2def::act("mod_rowtools", "");
o2def::act("remove_skin", "");
o2def::act("reset", "");
o2def::act("save_options", "");
o2def::act("start", "");
o2def::form("options_admin", "", False, "true", false);

?>
