��       W      �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG �                                                                                   	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +          ,          -          .          /          0          1          2          3          4          5          6          7          8          9          :          ;          <          =          >          ?          @          A          B          C          D          E          F          G          H          I          J          K          L          M          N          O          P          Q          R          S          T          U          V          W          jxjobs   	  $structure = o2_job::get_for_tree(0, 
                                  50, 
                                  o2val('prg�_�var','group_tree'), 
                                  o2val('prg�_�var','sort_tree'));
o2tree_def("jobs", $structure, 1, "node_detail"); true jxjobs       $code = o2tree_get_code("jobs"); $codejxjobs    "jxtree" jxjobs   �   $sele = o2tree_get_selection("jobs");
if ($sele) {
   $list = o2tree_get_nodes("jobs");
   $job  = o2_job::get_by_id($list[$sele][4]);
   } $job jxjobs   6   $job = o2val('prg�_�var','job_obj');
$id  = $job->id; $id jxjobs   :   $job  = o2val('prg�_�var','job_obj');
$name = $job->name; $name jxjobs   :   $job  = o2val('prg�_�var','job_obj');
$desc = $job->desc; $desc jxjobs   8   $job  = o2val('prg�_�var','job_obj');
$prg = $job->prg; $prg jxjobs	   @   $job    = o2val('prg�_�var','job_obj');
$params = $job->params; $paramsjxjobs
    "jxtoolbar"jxjobs   
 "jximgbtn"jxjobs    "<jx>/img/jobs/fold.png"jxjobs    "<jx>/img/jobs/unfold.png"jxjobs   \ (o2val('prg�_�var','sort_tree') ? "Sort by definition order" : "Sort by alphabetical order")jxjobs    !o2val('prg�_�var','sort_tree') jxjobs   8   o2tree_filter("jobs", o2val('prg�_�var','filter_tree')); truejxjobs    truejxjobs   M "<jx>/img/jobs/".(o2val('prg�_�var','sort_tree') ? "unsort.png" : "sort.png") jxjobs      o2tree_fold("jobs"); true jxjobs      o2tree_unfold("jobs"); true jxjobs   �  $raw_list = o2_run_job::get_by_job(o2val('prg�_�var','job_id'));
if ($raw_list !== false) {
   $rnt   = $GLOBALS['o2_runtime'];
   $imgs  = $rnt->alias."img/jobs/";
   $code  = '<table style="width:100%">';
   $blank = "&nbsp;&nbsp;&nbsp;&nbsp;- - -"; 
   foreach ($raw_list as $run_job) {
      $run    = $run_job->is_running();
      $wait   = ($run_job->start > (time() - 3)) && ($run_job->progress < 1);
      $queued = ($run_job->status == 'Q' && $run_job->progress < 1);
      $error  = ($run_job->error || 
                 (!$queued &&
                  ($run_job->progress < 100 || $run_job->status == 'R') && 
                  !$run) ? true : false);
      $status = ($wait ? 'Waiting' : 
                 ($queued ? 'Queued' : 
                  ($error ? 'Error' : o2format($run_job->status, 'jxjob_status'))));
      $ico    = $imgs.($wait ? 'scheduled.png' :
                       ($error ? 'failed.png' : 
                        ($run_job->status == 'C' ? 'executed.png' : 
                         ($run_job->status == 'Q' ? 'scheduled.png' :
                          'running.png'))));
      $bar    = $run_job->get_bar('node_detail', 'node_detail');
      $code  .= '<tr><td><img class="jximgbtn" src="'.$ico.
                '" onclick="'.o2_act4js(0, "run_job_detail", $run_job->id).
                '"></td><td>'.($run_job->start > 0 ? date("d/m/y H:i", $run_job->start) : $blank).
                "</td><td>".($run_job->end > 0 ? date("d/m/y H:i", $run_job->end) : $blank).
                "</td><td>".$status.
                "</td><td style='width:100px;height:18px'>".$bar.
                "</td></tr>\n";
      }
   $code.= '</table>';
   }
else {
   $code = 'Never';
   } $codejxjobs   " o2val('prg�_�var','run_jobs_list')jxjobs    "o2_ctrl_text_dis"jxjobs    falsejxjobs   # o2val('prg�_�var','sched_job_list') jxjobs   '  $raw_list = o2_scheduler::get_by_job(o2val('prg�_�var','job_id'));
if ($raw_list !== false) {
   $rnt  = $GLOBALS['o2_runtime'];
   $imgs = $rnt->alias."img/jobs/";
   $code = '<table style="width:100%">';
   foreach ($raw_list as $id => $sched_job) {
      $ico   = $imgs.($sched_job["active"] ? "scheduled.png" : "paused.png"); 
      $code .= '<tr><td><img src="'.$ico.'" title="Edit schedule" class="jximgbtn" onclick="'. 
               o2_act4js(0, "sched_detail", $id).'"></td><td>'.
               $sched_job["desc"].'</td><td style="width:20px"><img src="'.$imgs.
               'cancel.png" title="Delete schedule" class="jximgbtn" onclick="'. 
               o2_act4js(0, "delete_sched", $id)."\"></td></tr>\n";
      }
   $code.= "</table>";
   }
else {
   $code = "Never";
   } $codejxjobs    "disp_area" jxjobs   u   $text = "<center><h3>Janox Jobs Manager</h3></center><br>Select an item from the treeview<br>to see its properties."; $textjxjobs    "<jx>/img/jxpowered.png"jxjobs    o2zero('prg�_�var','job_id')jxjobs    !o2zero('prg�_�var','job_id')jxjobs     o2val('prg�_�var','job_id')jxjobs!    o2val('prg�_�var','job_name') jxjobs"   �   $label = '<div class="o2_ctrl_label" style="overflow:hidden;white-space:normal;word-wrap:break-word">'.nl2br(o2val('prg�_�var','job_desc'))."</div>"; $labeljxjobs#    o2val('prg�_�var','job_prg')jxjobs$   
 "jximgbtn"jxjobs%    "<jx>/img/jobs/job_add.png"jxjobs&    "<jx>/img/jobs/job_del.png"jxjobs'   7 "ATTENTION! This job is going to be deleted.\nProceed?"jxjobs(    "<jx>/img/jobs/refresh.png"jxjobs)    "<jx>/img/jobs/separator.png"jxjobs*    "<jx>/img/jobs/sched_add.png"jxjobs+   L "<jx>/img/jobs/exe".(o2val('prg�_�var','job_disabled') ? "_dis" : "").".png" jxjobs,   u   $job = o2_job::get_by_id(o2val('prg�_�var','job_id'));
if ($job) {
   $job->run(o2val('_o2SESSION','_area'));
   } truejxjobs-    "<jx>/img/jobs/scheduler.png"jxjobs.    "<jx>/img/jobs/run_job.png" jxjobs/   Z   if ($job = o2_job::get_by_id(o2val('prg�_�var','job_id'))) {
   $job->delete(true);
   } truejxjobs0    0jxjobs1    "<jx>/img/jobs/job_edit.png" jxjobs2   A   o2tree_node_select("jobs", "jxjob_".o2val('prg�_�var','job_id')); truejxjobs3    $idjxjobs4    o2_reqpar(1) jxjobs5   G   $task = new o2_task(o2val('prg�_�var','par_job_id'));
$task->delete(); truejxjobs6    o2_reqpar(1)jxjobs7    "tools/jxjob"jxjobs8    "tools/jxprocesses"jxjobs9    "tools/jxscheduler"jxjobs:    "tools/jxtask"jxjobs;    "tools/jxprocess" jxjobs<   ?   $job  = o2val('prg�_�var','job_obj');
$keep = $job->keep_days; $keep jxjobs=   �   $days = o2val('prg�_�var','job_keep_days');
$keep = "<div style='float:right'>Run instances are preserved ".
        ($days ? "for ".$days." days" : "forever")."</div>"; $keep jxjobs>   B   $job  = o2val('prg�_�var','job_obj');
$max = $job->max_instances; $max jxjobs?   �   $dis = "";
if (o2val('prg�_�var','job_disabled')) {
   $dis.= "<div style='float:right;font-weight:bold;color:#993300'>DISABLED</div>";
   } $dis jxjobs@   @   $job  = o2val('prg�_�var','job_obj');
$mode = $job->block_mode; $mode jxjobsA   M   $job = o2val('prg�_�var','job_obj');
$dis = ($job->disabled ? true : false); $disjxjobsB   & o2val('prg�_�var','job_max_instances')jxjobsC   A o2format(o2val('prg�_�var','job_block_mode'), "jxjob_block_mode")jxjobsD   & o2val('prg�_�var','job_max_instances')jxjobsE   " !o2val('prg�_�var','job_disabled')jxjobsF   < "jximgbtn".(o2val('prg�_�var','job_disabled') ? "_dis" : "")jxjobsG   0 "Job schedule is going to be deleted.\nProceed?"jxjobsH    o2val('prg�_�var','confirm')jxjobsI    o2_reqpar(1) jxjobsJ   I   $job  = o2val('prg�_�var','job_obj');
$srv = o2_str_nice($job->service); $srvjxjobsK   N '<jx>/img/jobs/'.(o2val('prg�_�var','group_tree') ? 'byjob.png' : 'bysrv.png')jxjobsL   R (o2zero('prg�_�var','job_service') ? 'Default' : o2val('prg�_�var','job_service'))jxjobsM     !o2val('prg�_�var','group_tree') jxjobsN   J   $msg = (o2val('prg�_�var','group_tree') ? 'Ungroup' : 'Group by service'); $msgjxjobsO    'hidden'jxjobsP   N "<jx>/img/jobs/queue".(o2val('prg�_�var','job_disabled') ? "_dis" : "").".png" jxjobsQ   �   $job = o2_job::get_by_id(o2val('prg�_�var','job_id'));
if ($job) {
   o2_run_job::queue($job, null, o2val('_o2SESSION','_area'));
   } truejxjobsR    "<jx>/img/jobs/services.png" jxjobsS   8   $code = 'Host: '.o2_str_nice($_SESSION['o2_app']->host); $codejxjobsT    "tools/jxservices"jxjobsU   
 'nowindow'jxjobsV   	 'nav_big'jxjobs    "jxtree jxtree_bottom"