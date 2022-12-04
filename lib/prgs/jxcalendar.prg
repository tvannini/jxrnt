<?php
//2.8
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxcalendar", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("cal_event_create");
o2def::act("cal_event_selection");
o2def::act("compose_html");
o2def::act("create_event");
o2def::act("festivities");
o2def::act("goto_date");
o2def::act("menu_selection");
o2def::act("next");
o2def::act("prev");
o2def::act("refresh_win");
o2def::act("set_size");
o2def::act("start");
o2def::act("switch_show_all");
o2def::act("today");
o2def::act("view_day");
o2def::act("view_month");
o2def::act("view_week");
o2def::act("view_year");
o2def::form("jxcalendar", "", False, "true", false);
o2def::par(1, "date", "_o2date");

?>
