<?php
//2.5
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_profile_ctrl", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("vis_rights", False, "", "", 1);
o2def::view("app_rights", False, "", "", 1);
o2def::view("ena_rights", False, "", "", 1);
o2def::act("ena_rights_delete", "");
o2def::act("ena_rights_insert", "");
o2def::act("ena_rights_post", "");
o2def::act("ena_rights_undo", "");
o2def::act("exit", "");
o2def::act("start", "");
o2def::act("vis_rights_delete", "");
o2def::act("vis_rights_insert", "");
o2def::act("vis_rights_post", "");
o2def::act("vis_rights_undo", "");
o2def::form("o2sys_profile_ctrl_1", "", False, "true", false);
o2def::par(1, "prg_name", "_o2alpha");
o2def::par(2, "ctrl_name", "_o2alpha");

?>
