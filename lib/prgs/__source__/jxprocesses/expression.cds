��             �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � Z                                                                                   	          
                                                                                                                                                                                                                  jxprocesses   "  $user   = o2app_user();
$user   = ($user == 'root' || o2par(1) ? '' : $user);
$job    = (o2par(4) ? o2par(4) : o2val('prg�_�var','flt_job'));
$status = o2val('prg�_�var','flt_status');
$days   = o2val('prg�_�var','days_filter'); 
$code   = o2_run_job::get_monitor($user, 
                                  'refresh', 
                                  'refresh', 
                                  'btn_click',
                                  $job,
                                  $status,
                                  $days); $codejxprocesses    "jxtree"jxprocesses    "tools/jxprocess_files"jxprocesses    "jxtoolbar"jxprocesses   
 "jximgbtn"jxprocesses    "<jx>/img/jobs/refresh.png"jxprocesses    "<jx>/img/jobs/separator.png" jxprocesses   L   $run_job = o2_run_job::get_by_id(o2_reqpar(2));
$files   = $run_job->files; $filesjxprocesses	    o2_reqpar(1) == "show"jxprocesses
    o2_reqpar(1) == "attach"jxprocesses   	 !o2par(4) jxprocesses   r  $app      = $_SESSION['o2_app'];
$aspid    = $app->vars['_area']->valore;
$tab      = $app->get_table(o2_run_job::$run_table);
$db       = $tab->db;
$server   = $db->server;
$co       = constant('o2_'.$server->type.'_o');
$cc       = constant('o2_'.$server->type.'_c');
$select   = 'DISTINCT '.$tab->campi['job_id']->nome_fisico.' '.$co.'JOB_ID'.$cc;            
$aspfld   = $tab->campi['run_aspid']->nome_fisico;
$where    = ($aspid ?
             $aspfld."='' OR ".
             $aspfld." IS NULL OR ".
             $aspfld."='".$aspid."' OR ".
             $aspfld."='NOASP'" :
             '');
$run_jobs = o2_gateway::recordset($server->type,
                                  $server->server,
                                  $server->user,
                                  $server->password,
                                  $db->nome,
                                  $db->proprietario,
                                  $tab->nome,
                                  'jx_run_jobs',
                                  $select,
                                  $where,
                                  '',
                                  1000);
$list = array('' => '');
if (count($run_jobs)) {
   foreach ($run_jobs as $run_rec) {
      $job = o2_job::get_by_id($run_rec['JOB_ID']);
      $list[$job->name] = $job->name;
      }
   }
asort($list); $listjxprocesses    o2_reqpar(2)jxprocesses    "<jx>/img/jobs/jobs24.png"jxprocesses    "<jx>/img/jobs/scheduler.png"jxprocesses    "tools/jxprocess"jxprocesses    "tools/jxjobs"jxprocesses    "tools/jxscheduler"jxprocesses   	 !o2par(2)jxprocesses   	 !o2par(3) jxprocesses   �   $list = array('' => '',
              'Q' => 'Queued',
              'R' => 'Running',
              'C' => 'Completed',
              'K' => 'Killed',
              'E' => 'Error'); $listjxprocesses    truejxprocesses    'Filter by job name'jxprocesses    'Filter by process status'jxprocesses    3jxprocesses    o2val('prg�_�var','jobs_names')jxprocesses   	 'nav_big'jxprocesses    'hidden'jxprocesses   
 'nowindow'jxprocesses    "jxtree jxtree_bottom"