<?php
//2.7
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_sessions_admin", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("sessions", False, "", "", 1);
o2def::act("access_log");
o2def::act("app_lock");
o2def::act("refresh");
o2def::act("sessions_delete");
o2def::act("start");
o2def::form("sessions_admin", "", False, "true", false);

?>
