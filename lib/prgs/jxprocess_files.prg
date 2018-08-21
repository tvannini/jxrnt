<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxprocess_files", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("close", "");
o2def::act("download_all", "");
o2def::act("download_file", "");
o2def::act("show_file", "");
o2def::act("start", "");
o2def::form("jxprocess_files_1", "", False, "true", false);
o2def::par(1, "runjob_id","_o2sid");

?>
