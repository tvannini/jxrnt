<?php
//2.8
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxcalendar", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("user_events", false, "", "", 0, 0);
o2def::view("festivities", false, "", "", 0, 0);
o2def::act("add_event");
o2def::act("add_festivity");
o2def::act("compose_html");
o2def::act("load_events");
o2def::act("load_festivities");
o2def::act("next_month");
o2def::act("prev_month");
o2def::act("set_size");
o2def::act("start");
o2def::act("today");
o2def::form("jxcalendar", "", False, "true", false);
o2def::par(1, "date", "_o2date");
o2def::par(2, "view", "jxcal_view");

?>
