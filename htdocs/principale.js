o2jse.menu.appMainMenu='principale'
o2jse.menu.appMenu = function() {
o2jse.menu.addMenu('principale');
with (o2jse.menu.menuList['principale']) {
addItem("M","azioni","собака","","img/act_folder.gif");
addItem("M","test","Test","","img/act_folder.gif");
addItem("S","test_separ4","","","");
addItem("M","sysmenu","System","","img/act_folder.gif");
addItem("P","leggi_classi_bar","Analizza classi","lancia_lettura_classi","img/read_classes.gif");
addItem("M","test_side_menu","Test Side Menù","","");
}
with (o2jse.menu.menuList['azioni']) {
addItem("P","leggi_classi","Analizza classi","lancia_lettura_classi","img/read_classes.gif");
addItem("P","gestisci_classi","Gestione classi","gestione_classi","");
addItem("P","stampa","Stampà","stampa_classi","img/print.gif");
addItem("P","visualizza","Visualizza pdf","pdf_visor","");
addItem("P","lancia","Lancia prg","srv_lanciatore","img/act_folder.gif");
}
with (o2jse.menu.menuList['test']) {
addItem("M","viewmenu","View","","img/test_folder.gif");
addItem("M","ctrlsmenu","GUI interface","","img/test_folder.gif");
addItem("M","actsmenu","Actions","","img/test_folder.gif");
addItem("M","climenu","Client mode","","img/test_folder.gif");
addItem("M","test_vari","Test vari","","img/test_folder.gif");
}
with (o2jse.menu.menuList['viewmenu']) {
addItem("P","simple_view","Simple view","simple_view","");
addItem("P","righe_refresh","Refresh righe","righe_refresh","");
addItem("P","gateway_test","Gateway test","test_gateway","");
addItem("P","viewlevel","View in prg level","views_prg_level","");
addItem("S","test_separ1","","","");
addItem("P","calcolati","Test calcolati","calcolati","");
addItem("P","combolist","Test combo list","test_combolist","");
addItem("P","viewalias","Test view alias","test_alias_viste","");
addItem("S","test_separ2","","","");
addItem("P","provamod","Prova modelli vista","prova_mod","");
addItem("P","test_link","Link","test_link","");
addItem("P","test_fixlink","Fixed link","test_fix_link","");
addItem("P","link_refresh","Link refresh","link_refresh","");
addItem("P","test_expkey","Index by expression","test_indexbyexp","");
addItem("P","test_one2many","One to many","one2many","");
addItem("P","viewtotal","View total","test_total_on_create","");
addItem("P","short_date_time","Short date/time","short_date_time","");
addItem("P","logicalrange","Range on formulas","test_logical_range","");
addItem("P","insertintofrom","Insert into from","test_insert_into_from","");
addItem("P","view_delete","View-delete","test_view_delete","");
addItem("P","view_selection","View selection","test_view_selection","");
addItem("P","snapshot","Snapshot","test_snapshot","");
addItem("P","virtual_flds","Virtual fields","test_virtual","");
addItem("P","prepared_stmts","Prepared statements","test_prepared","");
}
with (o2jse.menu.menuList['ctrlsmenu']) {
addItem("P","test_cui","GUI interface","test_gui","");
addItem("P","test_windows","Windows","test_windows","");
addItem("P","new_ctrls","Nuovi controlli","new_ctrls","");
addItem("S","test_separ3","","","");
addItem("P","onchange","On change events","test_onchange","img/test.gif");
addItem("P","test2","Test","test","img/test.gif");
addItem("P","test_interfaccia","Test interfaccia","prova_interfaccia","img/interface.gif");
addItem("S","test_separ11","","","");
addItem("P","problema","Problema","problema","");
addItem("P","multiline_tab","Multiline table","multiline_table","");
addItem("P","wysiwyg","Wysiwyg editor","test_wysiwyg","");
addItem("P","multipage","Multipage","multi_page","");
addItem("P","tabmultipage","Table in multipage","tab_in_multipage","");
addItem("P","counterarea","Counter on text area","js_on_field","");
addItem("P","array_treeview","Local treeview","test_array_treeview","");
addItem("P","set_size_pos","Controls size and position","ctrls_set_size_pos","");
addItem("P","new_size_mode","New size mode","test_expand","");
addItem("P","flowbox","Flowbox","flowbox","");
}
with (o2jse.menu.menuList['actsmenu']) {
addItem("P","testclick","Test click","test_click","");
addItem("P","lanciatore","Test loop","lanciatore","");
addItem("P","test_closeprg","Chiusure in cascata","test_closeprogram","");
addItem("P","exe_logic","Azioni in trigger di vista","execution_logic","");
addItem("P","goto_logic","Goto in cascata di call","test_goto","");
addItem("P","canconline","Cancel from online","test_canc_online","");
addItem("P","deepnest","Loop on deep nesting views","deep_nesting","");
addItem("P","endlesssfx","Endless suffix","endless_suffix","");
addItem("P","deferred","Deferred call","deferred_caller","");
addItem("P","closeinloop","Close prg in loop","test_close_in_loop","");
}
with (o2jse.menu.menuList['climenu']) {
addItem("P","clitest","Client table","test_cli_mode","");
addItem("P","ajaxcombo","Lookup control","test_ajaxcombo","");
addItem("P","ajaxcombo2","Test lookup in table","test_ajax_combo","");
addItem("P","fulltable","Full table","test_full_table","");
addItem("P","clionchange","Client on-change","cli_onchange","");
}
with (o2jse.menu.menuList['test_vari']) {
addItem("P","test_login_as","Login as other user","test_log_as_user","");
addItem("P","keyb","Keyboard","test_keyboard","img/keyb.gif");
addItem("P","test_nomi_lunghi","Test nomi lunghi","test_nomi_lunghi_mssql","");
addItem("P","test_numeri_lunghi","Test numeri lunghi","test_long_num","");
addItem("P","test_cache","Test cache","test_cache","");
addItem("P","test_map","Image map","test_img_map","");
addItem("P","screenform","Screen form","test_screen_form","");
addItem("P","invisibletab","Invisible table","invisible_table","");
addItem("P","confirm_msg","Confirm message","test_confirm","");
addItem("P","test_frame","Test frame","test_frame","");
addItem("P","test_logout","Test log-out","test_logout","");
addItem("P","test_dynamic","Controlli dinamici","controlli_dinamici","");
addItem("P","test_notify","Notification area","test_notify","");
addItem("P","test_popup","Popup on-over","test_popup","");
addItem("P","test_sqlview","View SQL","test_sql_view","");
addItem("P","test_isolated","Isolated transaction","test_isolated_transaction","");
addItem("M","bad_menu","Menu con separatori multipli","","");
addItem("P","test_multi_post_log","Log with multiple post","test_multi_post_log","");
addItem("P","embedded_calendar","Embedded calendar","embedded_calendar","");
}
with (o2jse.menu.menuList['bad_menu']) {
addItem("P","bad_menu1","Lettura classi","lancia_lettura_classi","");
addItem("S","bad_menu2","","","");
addItem("P","bad_menu4","Messaggio di conferma","test_confirm","");
addItem("S","bad_menu5","","","");
}
with (o2jse.menu.menuList['sysmenu']) {
addItem("P","userprofile","User profile","jxuser_profile","");
addItem("P","granting","Manage granting","tools/jxgranting","");
addItem("P","tab_admin","Administer databases","dbadmin","");
addItem("P","sess_admin","Manage sessions","tools/o2sys_sessions_admin","");
addItem("P","test_folder","File system browser","test_folder_browser","");
addItem("P","debug","Debugger","call_debug","");
addItem("P","doc","Documentor","doc/nodes_select","");
addItem("P","gtkdefer","GTK defer","tools/o2sys_gtk_defer","");
addItem("P","splitrep","Split tables rep","tools/o2dev_split_rep","");
addItem("P","prefs","Preferences","tools/o2sys_options_admin","");
addItem("P","iniconf","INI configuration","tools/o2sys_iniconf","");
addItem("U","exturl","Google (External URL)","http://www.google.com","");
addItem("P","srvcs","Services","tools/o2sys_services_admin","");
addItem("P","ologout","Logout","only_logout","");
addItem("P","translate","Translation manager","tools/jxtranslate","");
addItem("P","pwd_change","Password change","test_password_change","");
addItem("P","calendar","Calendar","tools/jxcalendar","");
}
with (o2jse.menu.menuList['test_side_menu']) {
addItem("M","tmenu1","Menu 1","","");
addItem("M","tmenu2","Menu 2","","");
addItem("P","titem1","Leggi classi","leggi_classi","");
}
with (o2jse.menu.menuList['tmenu1']) {
addItem("M","tmenu1_1","Sub menu 1 di 1","","");
addItem("P","titem1_2","Chiamato","chiamato","");
}
with (o2jse.menu.menuList['tmenu1_1']) {
addItem("P","titem1_1_1","Test interfaccia","prova_interfaccia","");
addItem("P","titem1_1_2","Gestione classi","gestione_classi","");
}
with (o2jse.menu.menuList['tmenu2']) {
addItem("P","titem2_1","Simple view","simple_view","");
addItem("P","titem_2_2","Test click","test_click","");
}
};
o2jse.menu.admMenu = function() {
with (o2jse.menu.menuList['']) {
}
};
