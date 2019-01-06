<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxmlt_produce", "produce", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("base_lang", False, "", "", 0);
o2def::view("translation", False, "", "", 0);
o2def::view("default_translation", False, "", "", 0);
o2def::view("prgs", False, "", "", 0);
o2def::act("check_update", "");
o2def::act("process_base_string", "");
o2def::act("produce", "");
o2def::act("produce_all", "");
o2def::act("produce_prg", "");
o2def::act("produce_update", "");
o2def::act("single_prg", "");
o2def::act("string_blank", "");
o2def::act("string_blank_default", "");
o2def::act("string_changed", "");
o2def::act("string_new", "");
o2def::act("string_new_default", "");
o2def::act("string_unchanged", "");
o2def::par(1, "mode", "o2sys_generic_opt");
o2def::par(2, "prg", "_o2alpha");
o2def::par(3, "lang_from", "o2sys_language");
o2def::par(4, "lang_to", "o2sys_language");
o2def::par(5, "use_default", "_o2logical");
?>
