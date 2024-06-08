<?php
//2.9
//o2def::module("");
//o2def::folder("triggers");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_request", "verify_session", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("sessions", false, "", "", 1, 0);
o2def::act("session_error");
o2def::act("update_db_session");
o2def::act("verify_db_session");
o2def::act("verify_session");
?>
