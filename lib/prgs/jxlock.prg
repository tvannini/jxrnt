<?php
//2.6
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxlock", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("options", False, "", "", 1);
o2def::view("sessions", False, "", "", 1);
o2def::act("close");
o2def::act("delete_sess");
o2def::act("destroy_sess");
o2def::act("do_batch");
o2def::act("read_status");
o2def::act("set_lock_off");
o2def::act("set_lock_on");
o2def::act("start");
o2def::act("toggle_lock");
o2def::form("jxlock", "", False, "true", false);
o2def::par(1, "batch", "_o2logical");
o2def::par(2, "new_status", "_o2logical");

?>
