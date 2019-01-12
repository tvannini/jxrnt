<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("o2sys_table_filter", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("tmp_where", False, "", "tmp_where_post", 1);
o2def::act("add_filter", "");
o2def::act("apply", "");
o2def::act("clear_all", "");
o2def::act("close", "");
o2def::act("start", "");
o2def::act("tmp_where_add", "");
o2def::act("tmp_where_clear", "");
o2def::act("tmp_where_delete", "");
o2def::act("tmp_where_fill", "");
o2def::act("tmp_where_post", "");
o2def::act("tmp_where_undo", "");
o2def::form("filters", "", False, "true", false);
o2def::par(1, "prgid", "_o2number");
o2def::par(2, "form_name", "_o2alpha");
o2def::par(3, "table_name", "_o2alpha");

?>
