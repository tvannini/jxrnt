<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxtranslate", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("base_lang", False, "", "", 0);
o2def::view("translation", False, "", "", 0);
o2def::view("default_translation", False, "", "", 0);
o2def::act("harvest", "");
o2def::act("process_base_string", "");
o2def::act("produce", "");
o2def::act("receive", "");
o2def::act("start", "");
o2def::act("string_blank", "");
o2def::act("string_blank_default", "");
o2def::act("string_changed", "");
o2def::act("string_new", "");
o2def::act("string_new_default", "");
o2def::act("string_unchanged", "");
o2def::form("jxtranslate_1", "", False, "true", false);

?>
