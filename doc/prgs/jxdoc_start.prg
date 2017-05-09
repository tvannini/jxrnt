<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxdoc_start", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("nodes", False, "", "");
o2def::view("sub_nodes", False, "", "");
o2def::view("app_links_ctrl", False, "", "");
o2def::view("app_links_form", False, "", "");
o2def::view("app_link_prg", False, "", "");
o2def::view("nodes_id", False, "", "");
o2def::view("container", False, "", "");
o2def::view("p_container", False, "", "");
o2def::view("prg_forms", False, "", "");
o2def::act("add_all", "");
o2def::act("add_gparent", "");
o2def::act("add_parent", "");
o2def::act("add_this", "");
o2def::act("assign_new_id", "");
o2def::act("by_node_id", "");
o2def::act("by_prg_name", "");
o2def::act("by_selection", "");
o2def::act("insert_container", "");
o2def::act("insert_ctrl", "");
o2def::act("insert_form", "");
o2def::act("insert_p_container", "");
o2def::act("insert_prg", "");
o2def::act("no_doc", "");
o2def::act("show_this", "");
o2def::act("start", "");
o2def::act("verify_links", "");
o2def::form("jxdoc_start", "", False, "jxdoc_start_exp_71()", false);
o2def::par(1, "node_id","node_id");
o2def::par(2, "prg_name","program_name");
o2def::par(3, "form_name","form_name");
o2def::par(4, "ctrl_name","ctrl_name");
o2def::par(5, "parent_name","ctrl_name");
o2def::par(6, "parent_info","ctrl_name");
o2def::par(7, "gparent_name","ctrl_name");
o2def::par(8, "gparent_info","ctrl_name");

?>
