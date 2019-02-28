<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_filter_ctrl", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("objs_rights", False, "", "", 1);
o2def::act("check_from_cache", "");
o2def::act("check_from_view", "");
o2def::act("exit", "");
o2def::act("start", "");
o2def::par(1, "form_name", "_o2alpha");
?>
