<?php
//2.8
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxcalendar_month", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("events", false, "", "", 0, 0);
o2def::view("festivities", false, "", "", 0, 0);
o2def::act("add_event");
o2def::act("add_festivity");
o2def::act("cal_event_create");
o2def::act("cal_event_selection");
o2def::act("compose_html");
o2def::act("load_events");
o2def::act("load_festivities");
o2def::act("return_code");
o2def::act("set_exe_id");
o2def::act("set_size");
o2def::act("start");
o2def::par(1, "date", "_o2date");
o2def::par(2, "size", "jxcal_size");
o2def::par(3, "code_for_embedding", "_o2text");
o2def::par(4, "exe_id", "_o2number");
o2def::par(5, "show_all", "_o2logical");
?>
