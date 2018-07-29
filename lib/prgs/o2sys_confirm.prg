<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_confirm", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("act_no", "");
o2def::act("act_yes", "");
o2def::act("start", "");
o2def::form("sys_confirm", "", False, "true", false);
o2def::par(1, "msg_txt","_o2text");
o2def::par(2, "return_value","_o2logical");

?>
