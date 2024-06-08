<?php
//2.9
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxaccesslog_browser", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("access_log", false, "", "", 0, 0);
o2def::act("load_lists");
o2def::act("refresh");
o2def::act("start");
o2def::form("jxaccesslog_browser_1", "", False, "true", false);

?>
