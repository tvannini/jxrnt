<?php
//2.8
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxcalendar_fest", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("festivities", false, "", "", 0, 0);
o2def::view("fest_id", false, "", "", 0, 0);
o2def::view("fest_delete", false, "", "", 0, 0);
o2def::act("add_fest");
o2def::act("close");
o2def::act("create_fest");
o2def::act("delete_fest");
o2def::act("edit_new");
o2def::act("load_fest");
o2def::act("start");
o2def::form("jxcalendar_fest", "", False, "true", false);

?>
