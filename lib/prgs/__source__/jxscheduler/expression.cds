��             �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � K                                                                                   	          
                                                                                                                                                               jxscheduler   < o2_scheduler::get_monitor("task_detail", o2par(1), o2par(2))jxscheduler    o2_reqpar(2)jxscheduler    "jxtoolbar"jxscheduler   
 "jximgbtn"jxscheduler    "<jx>/img/jobs/refresh.png"jxscheduler    "<jx>/img/jobs/separator.png"jxscheduler    "jxtree" jxscheduler   �   $img = "<jx>/img/jobs/";
if (o2_cron::is_registered()) {
   $img.= "cron_on24.png";
   }
else {
   $img.= "cron_off24.png";
   } $imgjxscheduler	    "<jx>/img/jobs/sched_add.png"jxscheduler
    "<jx>/img/jobs/run_job.png"jxscheduler    "<jx>/img/jobs/jobs24.png"jxscheduler    "tools/jxtask"jxscheduler    "tools/jxcron"jxscheduler    "tools/jxjobs"jxscheduler    "tools/jxprocesses"jxscheduler   
 'nowindow'jxscheduler    !o2par(1) && !o2par(2)jxscheduler    0 jxscheduler   �   if (o2par(2)) {
   $job_id = o2par(2);
   }
elseif (o2par(1)) {
   $job_id = o2_job::get_by_name(o2par(1))->id;
   }
else {
   $job_id = 0;
   } $job_id jxscheduler   8   $code = 'Host: '.o2_str_nice($_SESSION['o2_app']->host); $codejxscheduler    "<jx>/img/jobs/services.png"jxscheduler    'tools/jxservices'jxscheduler   	 'nav_big'jxscheduler    'hidden'jxscheduler    "jxtree jxtree_bottom"