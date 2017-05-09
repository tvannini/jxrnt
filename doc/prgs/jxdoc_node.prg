<?php
//2.1
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("jxdoc_node", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("nodes", False, "", "");
o2def::view("nodes_id", False, "", "");
o2def::view("sections", False, "", "");
o2def::view("key_words", False, "", "");
o2def::view("links", False, "", "");
o2def::view("sections_order", False, "", "");
o2def::view("ancestors", False, "", "");
o2def::view("app_links", False, "", "");
o2def::view("program_node", False, "", "");
o2def::view("form_node", False, "", "");
o2def::act("close_all", "");
o2def::act("create_subnode", "");
o2def::act("create_subsection", "");
o2def::act("delete", "");
o2def::act("delete_ancestor", "");
o2def::act("delete_applink", "");
o2def::act("delete_link", "");
o2def::act("delete_section", "");
o2def::act("delete_word", "");
o2def::act("edit", "");
o2def::act("editor_vis", "");
o2def::act("get_winpos", "");
o2def::act("goto_ancestor", "");
o2def::act("goto_child", "");
o2def::act("goto_form", "");
o2def::act("goto_index", "");
o2def::act("goto_link", "");
o2def::act("goto_prg", "");
o2def::act("index_by_menu", "");
o2def::act("new", "");
o2def::act("new_link", "");
o2def::act("new_section", "");
o2def::act("new_word", "");
o2def::act("node_text", "");
o2def::act("produce_html", "");
o2def::act("save", "");
o2def::act("save_new", "");
o2def::act("save_section", "");
o2def::act("save_word", "");
o2def::act("select_node", "");
o2def::act("select_use", "");
o2def::act("sendto", "");
o2def::act("set_winpos", "");
o2def::act("start", "");
o2def::act("undo", "");
o2def::form("edit", "", False, "jxdoc_node_exp_71()", false);
o2def::form("visor", "", False, "true", false);
o2def::par(1, "node_id","node_id");
o2def::par(2, "top_node","node_id");


?>
