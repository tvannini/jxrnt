<?php
//2.1
//o2def::module("");
//o2def::folder("[root]");
/*
o2def::prgnotes("");
*/

o2def::prg("produce_html", "produce", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("produce", "");
o2def::act("produce_index", "");
o2def::par(1, "starting_node_id","node_id");
o2def::par(2, "html_dir","_o2text");
?>
