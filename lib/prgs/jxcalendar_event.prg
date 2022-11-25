<?php
//2.8
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxcalendar_event", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("event", false, "", "", 0, 0);
o2def::view("event_id", false, "", "", 0, 0);
o2def::act("cancel");
o2def::act("create");
o2def::act("delete");
o2def::act("prepare_new_event");
o2def::act("save");
o2def::act("set_editable");
o2def::act("start");
o2def::act("whole_day");
o2def::form("jxcalendar_event", "", False, "true", false);
o2def::par(1, "event_id", "jxcal_event_id");
o2def::par(2, "event_date", "_o2date");

?>
