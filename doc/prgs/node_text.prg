<?php
//2.7
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("node_text", "compose", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("key_words", False, "", "", 1);
o2def::view("sub_nodes", False, "", "", 1);
o2def::view("links", False, "", "", 1);
o2def::view("node", False, "", "", 1);
o2def::act("add_index");
o2def::act("add_keyword");
o2def::act("add_link");
o2def::act("add_section");
o2def::act("collect_keywords");
o2def::act("collect_links");
o2def::act("collect_sections");
o2def::act("compose");
o2def::act("return_as_link");
o2def::act("return_as_text");
o2def::par(1, "node_id", "node_id");
o2def::par(2, "node_text", "node_text");
o2def::par(3, "nesting_str", "_o2text");
o2def::par(4, "development", "_o2logical");
o2def::par(5, "break_level", "_o2number");
o2def::par(6, "css", "_o2text");
o2def::par(7, "html_folder", "_o2text");
o2def::par(8, "starting_id", "node_id");
o2def::par(9, "index_text", "_o2text");
o2def::par(10, "prg_exe_id", "_o2number");
?>
