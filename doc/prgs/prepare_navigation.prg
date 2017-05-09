<?php
//2.1
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("prepare_navigation", "prepare", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("links", False, "", "");
o2def::view("sub_nodes", False, "", "");
o2def::view("nodes_ref", False, "", "");
o2def::view("this", False, "", "");
o2def::act("add_subnode", "");
o2def::act("loop_subnodes", "");
o2def::act("prepare", "");
o2def::act("write_links", "");
o2def::act("write_ref", "");
o2def::par(1, "node_id","node_id");
o2def::par(2, "break_level","_o2number");
o2def::par(3, "parent_id","node_id");
o2def::par(4, "prev_id","node_id");
o2def::par(5, "nesting_level","_o2number");
o2def::par(6, "parent_ref","_o2text");
?>
