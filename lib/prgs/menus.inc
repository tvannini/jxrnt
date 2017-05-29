<?php
// Standard menu items
o2def::menu_item("", "M", "o2admin", "Administrator", "", True, True);
o2def::menu_item("o2admin", "P", "o2grant", "Granting", "tools/jxgranting", True, True, "users.gif");
o2def::menu_item("o2admin", "P", "jxsessadmin", "Sessions", "tools/o2sys_sessions_admin", True, True, "sessions.png");
o2def::menu_item("o2admin", "P", "o2fs", "Resources", "tools/o2dev_fs_browser", True, True, "fs.png");
o2def::menu_item("o2admin", "P", "o2tables", "Connectivity", "tools/o2sys_tables_admin", True, True, "tables.gif");
o2def::menu_item("o2admin", "M", "o2options", "Options", "", True, True, "options.gif");
o2def::menu_item("o2options", "P", "o2config", "Advanced configuration", "tools/o2sys_iniconf", True, True, "options.gif");
o2def::menu_item("o2options", "S", "o2optsep", "", "", True, True, "");
o2def::menu_item("o2options", "P", "o2preferences", "Preferences", "tools/o2sys_options_admin", True, True, "preferences.gif");
o2def::menu_item("o2options", "P", "o2keyb", "Keyboard mapping", "tools/o2sys_key_mapping", True, True, "keyb.png");
o2def::menu_item("o2admin", "P", "o2jobs_admin", "Jobs", "tools/jxjobs", True, True, "jobs.png");
?>
