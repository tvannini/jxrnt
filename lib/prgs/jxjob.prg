<?php
//2.6
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxjob", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("close");
o2def::act("create_job");
o2def::act("read_job");
o2def::act("read_params");
o2def::act("save_job");
o2def::act("set_param");
o2def::act("set_value");
o2def::act("start");
o2def::form("jxjob_1", "", False, "true", false);
o2def::par(1, "job_id", "jxjob_id");

?>
