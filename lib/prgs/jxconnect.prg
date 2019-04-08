<?php
//2.4
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("");
*/

o2def::prg("jxconnect", "start", "", __FILE__);

o2def::view('prg§_§var');
o2def::act("change_conn_type", "");
o2def::act("check_connection", "");
o2def::act("from_engine2detail", "");
o2def::act("quit", "");
o2def::act("save_connection", "");
o2def::act("save_postgres", "");
o2def::act("select_dbfile", "");
o2def::act("start", "");
o2def::form("as400", "", False, "jxconnect_exp_8()", false);
o2def::form("db2", "", False, "jxconnect_exp_9()", false);
o2def::form("mssql", "", False, "jxconnect_exp_10()", false);
o2def::form("mysql", "", False, "jxconnect_exp_11()", false);
o2def::form("oracle", "", False, "jxconnect_exp_12()", false);
o2def::form("postgres", "", False, "jxconnect_exp_13()", false);
o2def::form("sqlite", "", False, "jxconnect_exp_14()", false);
o2def::form("engine", "", False, "jxconnect_exp_15()", false);
o2def::par(1, "jxconnection", "_o2structure");
o2def::par(2, "check_connection", "_o2logical");
o2def::par(3, "extra_data", "_o2logical");
o2def::par(4, "title", "_o2alpha");








?>
