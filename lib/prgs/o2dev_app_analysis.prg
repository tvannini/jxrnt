<?php
//2.9
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2dev_app_analysis", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("a_build_tabs_relations");
o2def::act("a_check_exps");
o2def::act("a_check_links");
o2def::act("a_check_locks");
o2def::act("a_check_tabs_relations");
o2def::act("a_count_calls");
o2def::act("a_duplicated_fileds");
o2def::act("a_missing_catch");
o2def::act("a_nuidxs_to_rep");
o2def::act("a_report_alias");
o2def::act("a_session");
o2def::act("a_sql_formulas");
o2def::act("a_unused_fields");
o2def::act("a_unused_keys");
o2def::act("a_unused_models");
o2def::act("a_unused_progs");
o2def::act("a_unused_tables");
o2def::act("a_views_in_loop");
o2def::act("batch");
o2def::act("do_analysis");
o2def::act("export");
o2def::act("on_model_change");
o2def::act("start");
o2def::form("o2dev_app_analysis", "", False, "o2dev_app_analysis_exp_47()", false);
o2def::par(1, "model", "_o2alpha");

?>
