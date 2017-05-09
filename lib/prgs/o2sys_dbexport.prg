<?php
//2.2
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_dbexport", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("export", "_internal");
o2def::act("quit", "");
o2def::act("select_target", "");
o2def::act("start", "");
o2def::form("select_db", "", False, "o2sys_dbexport_exp_3()", false);
o2def::form("confirm_export", "", False, "o2sys_dbexport_exp_7()", false);
o2def::par(1, "source_db","_o2alpha");


?>
