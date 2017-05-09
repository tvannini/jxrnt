o2jse.menu.appMainMenu='base'
o2jse.menu.appMenu = function() {
o2jse.menu.addMenu('base');
with (o2jse.menu.menuList['base']) {
addItem("M","settings","Settings","","");
addItem("M","actions","Actions","","");
}
with (o2jse.menu.menuList['settings']) {
addItem("P","options","Options","settings_admin","");
addItem("P","uses","Uses","uses_select","");
}
with (o2jse.menu.menuList['actions']) {
addItem("P","produce","Produce HTML","produce_html","");
addItem("P","view","View","jxdoc_start","");
}
};
o2jse.menu.admMenu = function() {
with (o2jse.menu.menuList['o2admin']) {
addItem("P","o2grant","Granting","tools/jxgranting","/janox/img/users.gif");
addItem("P","jxsessadmin","Manage sessions","tools/o2sys_sessions_admin","/janox/img/sessions.png");
addItem("P","o2fs","Resources","tools/o2dev_fs_browser","/janox/img/fs.png");
addItem("P","o2tables","Connectivity","tools/o2sys_tables_admin","/janox/img/tables.gif");
addItem("M","o2options","Options","","/janox/img/options.gif");
addItem("P","o2jobs_admin","Manage jobs","tools/jxjobs","/janox/img/jobs.png");
}
with (o2jse.menu.menuList['o2options']) {
addItem("P","o2config","Advanced configuration","tools/o2sys_iniconf","/janox/img/options.gif");
addItem("S","o2optsep","","","/janox/img/");
addItem("P","o2preferences","Preferences","tools/o2sys_options_admin","/janox/img/preferences.gif");
addItem("P","o2keyb","Keyboard mapping","tools/o2sys_key_mapping","/janox/img/keyb.png");
}
};
