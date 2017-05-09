<?php
// Voci menu
o2def::menu_item("", "M", "base", "", "", True, True);
o2def::menu_item("base", "M", "settings", "Settings", "", True, True);
o2def::menu_item("settings", "P", "options", "Options", "settings_admin", True, True);
o2def::menu_item("settings", "P", "uses", "Uses", "uses_select", True, True);
o2def::menu_item("base", "M", "actions", "Actions", "", True, True);
o2def::menu_item("actions", "P", "produce", "Produce HTML", "produce_html", True, True);
o2def::menu_item("actions", "P", "view", "View", "jxdoc_start", True, True);
?>
