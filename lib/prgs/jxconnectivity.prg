<?php
//2.6
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("jxconnectivity", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("logged_table", False, "", "", 1);
o2def::view("constraints", False, "", "", 1);
o2def::view("references", False, "", "", 1);
o2def::view("new_constraint", False, "", "", 1);
o2def::view("extra_indexes", False, "", "", 1);
o2def::view("new_extidx", False, "", "", 1);
o2def::act("activate_db_cons", "");
o2def::act("activate_single_cons", "");
o2def::act("activate_tab_cons", "");
o2def::act("area_change", "");
o2def::act("asp_clear", "_internal");
o2def::act("asp_clear_all", "_internal");
o2def::act("asp_clone", "");
o2def::act("asp_clone_all", "");
o2def::act("browse_table", "");
o2def::act("check_cons_validate", "");
o2def::act("check_create_new_constr", "");
o2def::act("check_create_new_exidx", "");
o2def::act("check_db_cons", "");
o2def::act("check_db_cons_off", "");
o2def::act("check_db_cons_validate", "");
o2def::act("check_index_name", "");
o2def::act("check_single_cons", "");
o2def::act("clear_operation", "_internal");
o2def::act("close_constr_details", "");
o2def::act("close_exidx_details", "");
o2def::act("constraint_add", "");
o2def::act("constraint_remove", "");
o2def::act("crea", "");
o2def::act("create_new_constr", "");
o2def::act("create_new_idx", "");
o2def::act("deactivate_db_cons", "");
o2def::act("deactivate_tab_cons", "");
o2def::act("define_treeview", "");
o2def::act("do_clone", "");
o2def::act("do_create_new_constr", "");
o2def::act("do_create_new_idx", "");
o2def::act("do_tree_select", "");
o2def::act("drop", "");
o2def::act("drop_all", "_internal");
o2def::act("drop_unmapped", "");
o2def::act("export", "");
o2def::act("export_all", "_internal");
o2def::act("export_db", "");
o2def::act("export_import", "_internal");
o2def::act("export_import_all", "");
o2def::act("export_import_tab", "");
o2def::act("extra_index_remove", "");
o2def::act("get_definition", "");
o2def::act("import", "");
o2def::act("import_all", "_internal");
o2def::act("index_add", "");
o2def::act("info_constraint", "");
o2def::act("info_db", "");
o2def::act("info_extra_index", "");
o2def::act("info_field", "");
o2def::act("info_reference", "");
o2def::act("info_server", "");
o2def::act("info_tab", "");
o2def::act("load_constraint", "");
o2def::act("load_extraindexes", "");
o2def::act("load_files", "");
o2def::act("load_folders", "");
o2def::act("load_references", "");
o2def::act("log_list", "");
o2def::act("log_off", "");
o2def::act("log_on", "");
o2def::act("log_table", "");
o2def::act("match", "");
o2def::act("new_c_add_link_field", "");
o2def::act("new_c_add_main_field", "");
o2def::act("new_c_remove_link_field", "");
o2def::act("new_c_remove_main_field", "");
o2def::act("new_c_tab_change", "");
o2def::act("new_ei_add_field", "");
o2def::act("new_ei_remove_field", "");
o2def::act("new_ei_switch_dir", "");
o2def::act("query_table_cons", "");
o2def::act("rebuild", "_internal");
o2def::act("rebuild_all", "_internal");
o2def::act("rebuild_dialog", "");
o2def::act("rebuild_errors", "");
o2def::act("result_select", "");
o2def::act("set_file_name", "");
o2def::act("set_table", "");
o2def::act("start", "");
o2def::act("switch_cons_status", "");
o2def::act("switch_idx_status", "");
o2def::act("switch_lock_asp_codes", "");
o2def::act("tables_check", "_internal");
o2def::act("toggle_sort_tree", "");
o2def::act("tree_fold_all", "");
o2def::act("tree_select", "");
o2def::act("tree_unfold_all", "");
o2def::act("unmapped_tabs", "");
o2def::act("validate_constraint", "");
o2def::act("validate_db_cons", "");
o2def::act("validate_tab_cons", "");
o2def::act("view_log", "");
o2def::form("gestione", "", False, "true", false);
o2def::form("m_expimp_dialog", "", False, "jxconnectivity_exp_69()", false);
o2def::form("n_rebuild_dialog", "", False, "jxconnectivity_exp_70()", false);
o2def::form("aspclone", "", False, "jxconnectivity_exp_133()", false);
o2def::form("getdef", "", False, "jxconnectivity_exp_233()", false);
o2def::form("constr_add", "", False, "jxconnectivity_exp_244()", false);
o2def::form("eixdx_add", "", False, "jxconnectivity_exp_307()", false);







?>
