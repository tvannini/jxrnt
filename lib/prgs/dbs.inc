<?php
// ==============  SERVERS ================
o2def::server("default", "sqlite3", "", "", "", 0);
o2def::server("jx_sysdb", "sqlite3", "".$GLOBALS['o2_runtime']->dir_data.'sys.db'."", "", "", 0);
// ==============  DATABASES ================
o2def::db("default", "default", "", "", false, true);
o2def::db("jx_sysdb", "jx_sysdb", "", "", false, true);
?>
