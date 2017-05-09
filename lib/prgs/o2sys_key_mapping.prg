<?php
//2.1
//o2def::module("");
//o2def::folder("options");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_key_mapping", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("options", False, "", "");
o2def::act("load_options", "");
o2def::act("save_options", "");
o2def::act("set_html_default", "");
o2def::act("start", "");
o2def::form("o2sys_key_mapping_1", "", False, "true", false);

?>
