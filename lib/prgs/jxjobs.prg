<?php
//2.6
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxjobs", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("create_job");
o2def::act("delete_job");
o2def::act("delete_sched");
o2def::act("edit_job");
o2def::act("exe_job");
o2def::act("filter_tree");
o2def::act("fold_tree");
o2def::act("get_run_jobs_list");
o2def::act("get_sched_jobs_list");
o2def::act("goto_processes");
o2def::act("goto_sched");
o2def::act("goto_services");
o2def::act("node_detail");
o2def::act("queue_job");
o2def::act("run_job_detail");
o2def::act("sched_add");
o2def::act("sched_detail");
o2def::act("set_tree_list");
o2def::act("sort_tree");
o2def::act("start");
o2def::act("switch_tree_group");
o2def::act("unfold_tree");
o2def::form("jxjobs_1", "", False, "true", false);

?>
