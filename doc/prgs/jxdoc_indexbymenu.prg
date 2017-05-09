<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxdoc_indexbymenu", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("nodes", False, "", "");
o2def::view("subnodes", False, "", "");
o2def::view("last_node_id", False, "", "");
o2def::view("prg_node", False, "", "");
o2def::view("this_menu_node", False, "", "");
o2def::view("prg_as_subnode", False, "", "");
o2def::view("last_subnode_position", False, "", "");
o2def::act("add_item", "");
o2def::act("add_this_menu", "");
o2def::act("check_prg", "");
o2def::act("insert_menu", "");
o2def::act("insert_menu_node", "");
o2def::act("link_menu", "");
o2def::act("link_prg", "");
o2def::act("start", "");
o2def::par(1, "menu_id","program_name");
?>
