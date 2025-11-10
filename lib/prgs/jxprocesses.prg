<?php
//3.0
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxprocesses", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("btn_click");
o2def::act("detail");
o2def::act("get_attach");
o2def::act("goto_jobs");
o2def::act("goto_scheduler");
o2def::act("keep_alive");
o2def::act("refresh");
o2def::act("start");
o2def::form("jxprocesses_1", "", False, "true", false);
o2def::par(1, "show_all", "_o2logical");
o2def::par(2, "deny_goto_jobs", "_o2logical");
o2def::par(3, "deny_goto_sched", "_o2logical");
o2def::par(4, "job_name_filter", "jxjob_name");

?>
