<?php
o2def::tab("o2_dbrelease", "default", "o2sys_dbrelease", "relcode");

	o2def::field("release_code", "release_code", "_o2alpha");

	o2def::index("relcode", "release_code","A");

o2def::tab("o2_users", "default", "o2sys_users", "o2user");

	o2def::field("o2user", "o2user", "_o2alpha");
	o2def::field("o2password", "o2password", "_o2sid");
	o2def::field("area", "asp_area", "_o2alpha");
	o2def::field("expire_date", "expire_date", "_o2date");
	o2def::field("last_date", "last_date", "_o2date");
	o2def::field("last_time", "last_time", "_o2time");
	o2def::field("login_type", "login_type", "o2sys_login_type");
	o2def::field("last_pwd_date", "last_pwd_date", "_o2date");
	o2def::field("force_pwd_change", "force_pwd_change", "_o2logical");
	o2def::field("pwds_history", "pwds_history", "o2sys_long_str");
	o2def::field("no_pwd_change", "no_pwd_change", "_o2logical");
	o2def::field("creation_date", "creation_date", "_o2date");

	o2def::index("o2user", "o2user","A");
	o2def::index("asp_area", "area","A", "o2user","A");

o2def::tab("o2_sessions", "default", "o2sys_sessions", "sid");

	o2def::field("sid", "sid", "_o2sid");
	o2def::field("o2user", "o2user", "_o2alpha");
	o2def::field("c_date", "c_date", "_o2date");
	o2def::field("c_time", "c_time", "_o2time");
	o2def::field("a_date", "a_date", "_o2date");
	o2def::field("a_time", "a_time", "_o2time");
	o2def::field("e_date", "e_date", "_o2date");
	o2def::field("e_time", "e_time", "_o2time");
	o2def::field("terminal_id", "terminal_id", "jxterminal_id");
	o2def::field("app_name", "app_name", "_o2alpha");
	o2def::field("run_mode", "run_mode", "o2sys_app_run_mode");
	o2def::field("host", "host", "jxhost");

	o2def::index("sid", "sid","A");
	o2def::index("expire", "e_date","A", "e_time","A", "sid","A");
	o2def::index("creation", "c_date","A", "c_time","A", "sid","A");
	o2def::index("lastupdate", "a_date","A", "a_time","A", "sid","A");
	o2def::index("upd_desc", "a_date","D", "a_time","D", "sid","A");

o2def::tab("o2_options", "default", "o2sys_options", "id");

	o2def::field("o2user", "o2user", "_o2alpha");
	o2def::field("code", "code", "_o2alpha");
	o2def::field("value", "value", "_o2sid");

	o2def::index("id", "o2user","A", "code","A");

o2def::tab("o2_rights", "default", "o2sys_rights", "right_code");

	o2def::field("right_code", "right_code", "o2sys_right");

	o2def::index("right_code", "right_code","A");

o2def::tab("o2_roles", "default", "o2sys_roles", "role_name");

	o2def::field("role_name", "role_name", "o2sys_role");

	o2def::index("role_name", "role_name","A");

o2def::tab("o2_rights_per_role", "default", "o2sys_rights_per_role", "role_name");

	o2def::field("role_name", "role_name", "o2sys_role");
	o2def::field("right_code", "right_code", "o2sys_right");

	o2def::index("role_name", "role_name","A", "right_code","A");
	o2def::index("right_code", "right_code","A", "role_name","A");

o2def::tab("o2_roles_per_user", "default", "o2sys_roles_per_user", "o2user");

	o2def::field("o2user", "o2user", "_o2alpha");
	o2def::field("role_name", "role_name", "o2sys_role");

	o2def::index("o2user", "o2user","A", "role_name","A");
	o2def::index("role_name", "role_name","A", "o2user","A");

o2def::tab("o2_rights_roles_desc", "default", "o2sys_rights_roles_desc", "unique_name");

	o2def::field("element_type", "element_type", "o2sys_right_role");
	o2def::field("element_name", "element_name", "o2sys_right");
	o2def::field("element_desc", "element_desc", "_o2text");

	o2def::index("unique_name", "element_type","A", "element_name","A");

o2def::tab("o2_tabslog", "default", "o2sys_tabslog", "log_tabname");

	o2def::field("log_tabname", "log_tabname", "_o2alpha");
	o2def::field("log_level", "log_level", "o2sys_loglevel");
	o2def::field("log_operations", "log_operations", "_o2number");

	o2def::index("log_tabname", "log_tabname","A");

o2def::tab("o2_profiling", "default", "o2sys_profiling", "obj");

	o2def::field("obj_type", "obj_type", "o2sys_obj_type");
	o2def::field("obj_module", "obj_module", "_o2alpha");
	o2def::field("obj_name", "obj_name", "_o2alpha");
	o2def::field("right_vis", "right_vis", "o2sys_right");
	o2def::field("right_enable", "right_enable", "o2sys_right");

	o2def::index("obj", "obj_type","A", "obj_module","A", "obj_name","A", "right_vis","A", "right_enable","A");

o2def::tab("o2_tmp_where", "default", "o2sys_tmp_where", "id");

	o2def::field("session_profile", "session_profile", "_o2sid");
	o2def::field("pos_id", "pos_id", "_o2number");
	o2def::field("field_name", "field_name", "o2sys_long_str");
	o2def::field("field_title", "field_title", "o2sys_long_str");
	o2def::field("field_mask", "field_mask", "o2sys_long_str");
	o2def::field("min_exp", "min_exp", "o2sys_long_str");
	o2def::field("max_exp", "max_exp", "o2sys_long_str");
	o2def::field("not_exp", "not_exp", "o2sys_long_str");
	o2def::field("like_exp", "like_exp", "o2sys_long_str");

	o2def::index("id", "session_profile","A", "pos_id","A");

o2def::tab("o2_custom_grids", "default", "o2sys_custom_grids", "p_col");

	o2def::field("o2user", "o2user", "_o2alpha");
	o2def::field("prg", "prg", "_o2alpha");
	o2def::field("obj", "obj", "_o2alpha");
	o2def::field("p_type", "p_type", "o2sys_custom_grid_prop");
	o2def::field("p_col", "p_col", "_o2alpha");
	o2def::field("p_value", "p_value", "o2sys_negative");

	o2def::index("p_col", "o2user","A", "prg","A", "obj","A", "p_type","A", "p_col","A");

o2def::tab("o2_custom_export", "default", "o2sys_custom_export", "p_col");

	o2def::field("o2user", "o2user", "_o2alpha");
	o2def::field("prg", "prg", "_o2alpha");
	o2def::field("obj", "obj", "_o2alpha");
	o2def::field("p_type", "p_type", "o2sys_custom_export_prop");
	o2def::field("p_col", "p_col", "_o2alpha");

	o2def::index("p_col", "o2user","A", "prg","A", "obj","A", "p_type","A", "p_col","A");

o2def::tab("o2_constraints", "default", "o2sys_constraints", "main_constr");

	o2def::field("main_table", "main_table", "o2sys_constraint_tab");
	o2def::field("main_fields", "main_fields", "o2sys_constraint_key");
	o2def::field("linked_table", "linked_table", "o2sys_constraint_tab");
	o2def::field("linked_fields", "linked_fields", "o2sys_constraint_key");
	o2def::field("constraint_name", "constraint_name", "o2sys_constraint_key");

	o2def::index("main_constr", "main_table","A", "main_fields","A");
	o2def::index("ref_contr", "linked_table","A", "linked_fields","A", "main_table","A", "main_fields","A");
	o2def::index("constr_name", "constraint_name","A");

o2def::tab("o2_extraindexes", "default", "o2sys_extraindexes", "index_def");

	o2def::field("index_table", "index_table", "o2sys_extraidx_tab");
	o2def::field("index_fields", "index_fields", "o2sys_extraidx_key");
	o2def::field("index_name", "index_name", "o2sys_extraidx_key");
	o2def::field("index_unique", "index_unique", "_o2logical");

	o2def::index("index_def", "index_table","A", "index_fields","A");
	o2def::index("index_name", "index_name","A");

o2def::tab("jx_jobs", "default", "jx_jobs", "job_id");

	o2def::field("job_id", "job_id", "jxjob_id");
	o2def::field("job_name", "job_name", "jxjob_name");
	o2def::field("job_desc", "job_desc", "jxjob_desc");
	o2def::field("job_prg", "job_prg", "jxjob_prg");
	o2def::field("job_params", "job_params", "_o2text");
	o2def::field("job_keep_days", "job_keep_days", "_o2number");
	o2def::field("job_max_instances", "job_max_instances", "_o2number");
	o2def::field("job_block_mode", "job_block_mode", "jxjob_block_mode");
	o2def::field("job_disabled", "job_disabled", "_o2logical");
	o2def::field("job_service", "job_service", "jxservice");

	o2def::index("job_id", "job_id","A");
	o2def::index("job_desc", "job_desc","A", "job_id","A");
	o2def::index("job_prg", "job_prg","A", "job_id","A");
	o2def::index("job_name", "job_name","A", "job_id","A");

o2def::tab("jx_running_jobs", "default", "jx_running_jobs", "run_id");

	o2def::field("run_id", "run_id", "_o2sid");
	o2def::field("job_id", "job_id", "jxjob_id");
	o2def::field("run_prg", "run_prg", "jxjob_prg");
	o2def::field("run_params", "run_params", "jxjob_params");
	o2def::field("run_user", "run_user", "_o2alpha");
	o2def::field("run_start_date", "run_start_date", "_o2date");
	o2def::field("run_start_time", "run_start_time", "_o2time");
	o2def::field("run_aspid", "run_aspid", "_o2alpha");
	o2def::field("run_status", "run_status", "jxjob_status");
	o2def::field("run_pid", "run_pid", "_o2number");
	o2def::field("run_progress", "run_progress", "jxjob_progress");
	o2def::field("run_files", "run_files", "_o2text");
	o2def::field("run_end_date", "run_end_date", "_o2date");
	o2def::field("run_end_time", "run_end_time", "_o2time");
	o2def::field("run_error", "run_error", "_o2text");
	o2def::field("run_service", "run_service", "jxservice");
	o2def::field("run_host", "run_host", "jxhost");
	o2def::field("run_at_date", "run_at_date", "_o2date");
	o2def::field("run_at_time", "run_at_time", "_o2time");
	o2def::field("run_developer", "run_developer", "_o2alpha");
	o2def::field("run_sessid", "run_sessid", "_o2sid");
	o2def::field("run_sched_id", "run_sched_id", "jxscheduler_id");

	o2def::index("run_id", "run_id","A");
	o2def::index("run_user", "run_user","A", "run_id","A");
	o2def::index("run_start", "run_start_date","D", "run_start_time","D", "run_id","A");
	o2def::index("run_end", "run_end_date","D", "run_end_time","D", "run_id","A");
	o2def::index("run_status", "run_status","A", "run_start_date","D", "run_start_time","D", "run_id","A");
	o2def::index("job_id", "job_id","A", "run_start_date","D", "run_start_time","D", "run_id","A");
	o2def::index("run_sessid", "run_sessid","A", "run_id","A");

o2def::tab("jx_scheduler", "default", "jx_scheduler", "sched_id");

	o2def::field("sched_id", "sched_id", "jxscheduler_id");
	o2def::field("sched_job", "sched_job", "jxjob_id");
	o2def::field("sched_user", "sched_user", "_o2alpha");
	o2def::field("sched_type", "sched_type", "jxscheduler_type");
	o2def::field("sched_interval", "sched_interval", "_o2number");
	o2def::field("sched_date", "sched_date", "_o2date");
	o2def::field("sched_time", "sched_time", "jxsched_time");
	o2def::field("sched_recurrence", "sched_recurrence", "jxsched_recurrence");
	o2def::field("sched_week_day", "sched_week_day", "jxsched_week_day");
	o2def::field("sched_month_day", "sched_month_day", "jxsched_month_day");
	o2def::field("sched_paused", "sched_paused", "_o2logical");
	o2def::field("sched_aspid", "sched_aspid", "_o2alpha");

	o2def::index("sched_id", "sched_id","A");
	o2def::index("sched_job", "sched_job","A", "sched_id","A");

o2def::tab("jx_services", "default", "jx_services", "srv_host");

	o2def::field("srv_service", "srv_service", "jxservice");
	o2def::field("srv_host", "srv_host", "jxhost");
	o2def::field("srv_start_date", "srv_start_date", "o2sys_long_date");
	o2def::field("srv_start_time", "srv_start_time", "o2sys_long_time");
	o2def::field("srv_last_date", "srv_last_date", "o2sys_long_date");
	o2def::field("srv_last_time", "srv_last_time", "o2sys_long_time");

	o2def::index("srv_host", "srv_service","A", "srv_host","A");
	o2def::index("srv_host", "srv_host","A", "srv_service","A");

o2def::tab("jx_hosts", "default", "jx_hosts", "host_host");

	o2def::field("host_host", "host_host", "jxhost");
	o2def::field("host_pids", "host_pids", "_o2text");

	o2def::index("host_host", "host_host","A");

o2def::tab("jx_translations", "default", "jx_translations", "lang_ctrl");

	o2def::field("mlt_lang", "mlt_lang", "o2sys_language");
	o2def::field("mlt_prg", "mlt_prg", "_o2alpha");
	o2def::field("mlt_ctrl", "mlt_ctrl", "_o2alpha");
	o2def::field("mlt_property", "mlt_property", "_o2alpha");
	o2def::field("mlt_prop_value", "mlt_prop_value", "_o2alpha");
	o2def::field("mlt_base_str", "mlt_base_str", "o2sys_full_str");
	o2def::field("mlt_lang_str", "mlt_lang_str", "o2sys_full_str");

	o2def::index("lang_ctrl", "mlt_lang","A", "mlt_prg","A", "mlt_ctrl","A", "mlt_property","A", "mlt_prop_value","A");
	o2def::index("lang_string", "mlt_lang","A", "mlt_base_str","A", "mlt_prg","A", "mlt_ctrl","A", "mlt_property","A", "mlt_prop_value","A");

?>
