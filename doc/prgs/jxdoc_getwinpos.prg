<?php
//1.3
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxdoc_getwinpos", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::view("winpos", False, "", "");
o2def::act("start", "");
o2def::par(1, "prg_name","program_name");
o2def::par(2, "form_name","form_name");
o2def::par(3, "is_open","_o2logical");
o2def::par(4, "value_x","_o2number");
o2def::par(5, "value_y","_o2number");
o2def::par(6, "value_w","_o2number");
o2def::par(7, "value_h","_o2number");
?>
