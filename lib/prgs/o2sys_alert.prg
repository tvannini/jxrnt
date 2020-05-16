<?php
//2.6
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_alert", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("close");
o2def::act("start");
o2def::form("sys_alert", "", False, "true", false);
o2def::par(1, "msg_txt", "_o2text");

?>
