<?php
o2def::tab("jxdoc_nodes", "jxdoc", "docnodes", "id");

	o2def::field("id", "id", "node_id");
	o2def::field("description", "description", "node_desc");
	o2def::field("title", "title", "node_title");
	o2def::field("node_text", "node_text", "node_text");

	o2def::index("id", "id","A");
	o2def::index("description", "description","A", "id","A");
	o2def::index("title", "title","A", "id","A");

o2def::tab("jxdoc_uses", "jxdoc", "docuses", "id");

	o2def::field("id", "id", "use_id");
	o2def::field("description", "description", "use_desc");
	o2def::field("prefix", "prefix", "node_text");
	o2def::field("suffix", "suffix", "node_text");
	o2def::field("incremental", "incremental", "_o2logical");
	o2def::field("show_title", "show_title", "_o2logical");
	o2def::field("title_level", "title_level", "title_level");
	o2def::field("tech_level", "tech_level", "_o2number");

	o2def::index("description", "description","A", "id","A");
	o2def::index("id", "id","A");

o2def::tab("jxdoc_sub_nodes", "jxdoc", "docsubnodes", "child");

	o2def::field("parent", "parent", "node_id");
	o2def::field("use", "use", "use_id");
	o2def::field("child", "child", "use_id");
	o2def::field("position", "position", "_o2number");

	o2def::index("position", "parent","A", "position","A");
	o2def::index("child", "child","A", "parent","A", "position","A");
	o2def::index("use", "use","A", "parent","A", "position","A");

o2def::tab("jxdoc_key_words", "jxdoc", "dockeywords", "node");

	o2def::field("key_word", "key_word", "key_word");
	o2def::field("node", "node", "node_id");

	o2def::index("word", "key_word","A", "node","A");
	o2def::index("node", "node","A", "key_word","A");

o2def::tab("jxdoc_links", "jxdoc", "doclinks", "node");

	o2def::field("node", "node", "node_id");
	o2def::field("link", "link", "node_id");

	o2def::index("node", "node","A", "link","A");
	o2def::index("link", "link","A", "node","A");

o2def::tab("jxdoc_settings", "jxdoc", "docsettings", "key");

	o2def::field("key", "key", "setting_key");
	o2def::field("value", "value", "_o2text");
	o2def::field("description", "description", "_o2text");

	o2def::index("key", "key","A");

o2def::tab("jxdoc_navigation_links", "jxdoc", "docnavlinks", "id");

	o2def::field("node_id", "node_id", "node_id");
	o2def::field("node_parent", "node_parent", "node_id");
	o2def::field("node_prev", "node_prev", "node_id");
	o2def::field("node_next", "node_next", "node_id");

	o2def::index("id", "node_id","A");

o2def::tab("jxdoc_nodes_ref", "jxdoc", "docnodesref", "id");

	o2def::field("node_id", "node_id", "node_id");
	o2def::field("node_ref", "node_ref", "node_href");

	o2def::index("id", "node_id","A");

o2def::tab("jxdoc_app_links", "jxdoc", "docapplinks", "node_id");

	o2def::field("node_id", "node_id", "node_id");
	o2def::field("prg_name", "prg_name", "program_name");
	o2def::field("form_name", "form_name", "form_name");
	o2def::field("ctrl_name", "ctrl_name", "ctrl_name");

	o2def::index("node_id", "node_id","A");
	o2def::index("app_link", "prg_name","A", "form_name","A", "ctrl_name","A");

o2def::tab("jxdoc_winpos", "jxdoc", "docwinpos", "id");

	o2def::field("user_name", "user_name", "_o2alpha");
	o2def::field("prg_name", "prg_name", "program_name");
	o2def::field("form_name", "form_name", "form_name");
	o2def::field("is_open", "is_open", "_o2logical");
	o2def::field("value_x", "value_x", "_o2number");
	o2def::field("value_y", "value_y", "_o2number");
	o2def::field("value_w", "value_w", "_o2number");
	o2def::field("value_h", "value_h", "_o2number");

	o2def::index("id", "user_name","A", "prg_name","A", "form_name","A");

?>
