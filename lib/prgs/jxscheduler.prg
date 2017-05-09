<?php
//2.3
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxscheduler", "", "", __FILE__);

o2def::act("create_new", "");
o2def::act("cron", "");
o2def::act("goto_jobs", "");
o2def::act("goto_processes", "");
o2def::act("goto_services", "");
o2def::act("refresh", "");
o2def::act("task_detail", "");
o2def::form("jxscheduler_1", "", False, "true", false);
o2def::par(1, "job_name","jxjob_name");

?>
