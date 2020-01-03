<?php
// ==============  SERVERS ================
o2def::server("default", "sqlite3", "", "", "", 0);
o2def::server("jxsess_srv", "sqlite3", ":session:", "", "", 0);
// ==============  DATABASES ================
o2def::db("default", "default", "", "", false, true);
o2def::db("jxsess_db", "jxsess_srv", "", "", false, true);
?>
