<?php
//2.1
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxtask", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("jobs_combo", False, "", "");
o2def::act("change_desc", "");
o2def::act("clear_interval", "");
o2def::act("clear_once", "");
o2def::act("clear_recurrence", "");
o2def::act("create", "");
o2def::act("read_task", "");
o2def::act("save", "");
o2def::act("start", "");
o2def::act("type_change", "");
o2def::form("jxtask_1", "", False, "true", false);
o2def::par(1, "sched_id","jxscheduler_id");
o2def::par(2, "job","jxjob_id");

?>
