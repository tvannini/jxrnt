��       4      �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � �                                                                                   	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +          ,          -          .          /          0          1          2          3          4          	jxprocess    o2_run_job::get_by_id(o2par(1)) 	jxprocess   6   $obj = o2val('prg�_�var','run_obj');
$val = $obj->id; $val 	jxprocess   7   $obj = o2val('prg�_�var','run_obj');
$val = $obj->job; $val 	jxprocess   7   $obj = o2val('prg�_�var','run_obj');
$val = $obj->prg; $val 	jxprocess   �   $obj  = o2val('prg�_�var','run_obj');
$list = array();
foreach ($obj->params as $name => $value) {
   $list[] = $name.": ".$value;
   } $list 	jxprocess   8   $obj = o2val('prg�_�var','run_obj');
$val = $obj->user; $val 	jxprocess   L   $obj = o2val('prg�_�var','run_obj');
$val = date("d/m/Y H:i", $obj->start); $val 	jxprocess   H   $obj = o2val('prg�_�var','run_obj');
$vis = ($obj->end ? true : false); $vis 	jxprocess	   J   $obj = o2val('prg�_�var','run_obj');
$val = date("d/m/Y H:i", $obj->end); $val 	jxprocess
   �   $img    = "<jx>/img/jobs/";
$status = o2val('prg�_�var','run_status');
if ($status == "Running") {
   $img.= "pause24.png"; 
   }
else {
   $img.= "pause_dis24.png"; 
   } $img 	jxprocess   9   $obj = o2val('prg�_�var','run_obj');
$val = $obj->aspid; $val 	jxprocess     $obj    = o2val('prg�_�var','run_obj');
$status = $obj->status;
$active = $obj->is_running();
switch ($obj->status) {
   case "Q":
      $val = "Queued";
      break;
   case "R":
      if ($active) {
         $val = "Running";
         }
      else {
         $val = "Error";
         }
      break;
   case "C":
      if ($obj->error || $obj->progress < 100) {
         $val = "Error";
         }
      else {
         $val = "Completed";
         }
      break;
   default:
      $val = "Error";
      break;
   } $val 	jxprocess   7   $obj = o2val('prg�_�var','run_obj');
$val = $obj->pid; $val 	jxprocess   <   $obj = o2val('prg�_�var','run_obj');
$val = $obj->progress; $val 	jxprocess   9   $obj = o2val('prg�_�var','run_obj');
$val = $obj->files; $val	jxprocess    o2val('prg�_�var','run_params') 	jxprocess   L   $job = o2_job::get_by_id(o2val('prg�_�var','run_job')); 
$val = $job->name; $val	jxprocess    false 	jxprocess   ^   $run  = o2_run_job::get_by_id(o2val('prg�_�var','run_id'));
$code = $run->get_bar('refresh'); $code 	jxprocess   a  switch (o2val('prg�_�var','run_status')) {
   case "Running":
      $img = "running32.png";
      break;
   case "Queued":
      $img = "scheduled32.png";
      break;
   case "Completed":
      $img = "executed32.png";
      break;
   case "Error":
   default:
      $img = "failed32.png";
      break;
   }
$img = "<jx>/img/jobs/".$img; $img	jxprocess    "jxtoolbar"	jxprocess    "<jx>/img/jobs/separator.png"	jxprocess    "<jx>/img/jobs/refresh.png" 	jxprocess   �   $img    = "<jx>/img/jobs/";
$status = o2val('prg�_�var','run_status');
if ($status == "Running" || $status == "Queued") {
   $img.= "stop24.png"; 
   }
else {
   $img.= "stop_dis24.png"; 
   } $img	jxprocess   [ o2val('prg�_�var','run_status') == "Running" || o2val('prg�_�var','run_status') == "Queued" 	jxprocess   �   $params = o2val('prg�_�var','run_files');
$img    = "<jx>/img/jobs/";
if (is_array($params) && count($params)) {
   $img.= "attach24.png"; 
   }
else {
   $img.= "attach_dis24.png"; 
   } $img 	jxprocess   Y   $params = o2val('prg�_�var','run_files');
$cond   = is_array($params) && count($params); $cond 	jxprocess   �   $img = "<jx>/img/jobs/";
if (o2zero('prg�_�var','run_error')) {
   $img.= "error_dis24.png"; 
   }
else {
   $img.= "error24.png"; 
   } $img	jxprocess     !o2zero('prg�_�var','run_error') 	jxprocess   9   $obj = o2val('prg�_�var','run_obj');
$val = $obj->error; $val	jxprocess    o2val('prg�_�var','show_error')	jxprocess     true	jxprocess!    o2val('prg�_�var','show_files') 	jxprocess"   P  $code = '<table style="width:100%"><tr><td>';
foreach (o2val('prg�_�var','run_files') as $path) {
   $file  = basename($path);
   $style = 'style="background-color:#ffffff;border:1px solid #bbbbbb;cursor:pointer;padding:3px;"';
   $div   = '<div '.$style.' onclick="'.o2_act4js(0, "show_file", $path).'">'.$file.'</div>';
   $dwnl  = '<div><img class="jximgbtn" onclick="'.o2_act4js(0, "download_file", $path).
            '" title="Download" src="'.o2rnt_alias().'img/jobs/attach24.png"></div>';  
   $code .= '<tr><td>'.$div.'</td><td>'.$dwnl.'</td></tr>';
   }
$code.= '</table>'; $code 	jxprocess#   ;   $run_job = o2val('prg�_�var','run_obj');
$run_job->kill(); true	jxprocess$   
 "jximgbtn"	jxprocess%   @ (o2val('prg�_�var','run_status') == "Running" ? "jximgbtn" : "")	jxprocess&    (o2exp(27) ? "jximgbtn" : "")	jxprocess'    (o2exp(29) ? "jximgbtn" : "")	jxprocess(    "tools/jxvisor"	jxprocess)    o2_reqpar(1) 	jxprocess*   :   $obj  = o2val('prg�_�var','run_obj');
$host = $obj->host; $host	jxprocess+   C '<jx>/img/jobs/reexe'.(o2app_user() == 'root' ? '' : '_dis').'.png' 	jxprocess,   3  $run_job = o2val('prg�_�var','run_obj');
$new_job = o2_run_job::queue(o2_job::get_by_id($run_job->job), 
                             $run_job->params, 
                             $run_job->aspid,                              
                             $run_job->service);
$new_id  = $new_job->id; $new_id	jxprocess-   6 o2_run_job::get_by_id(o2val('prg�_�var','new_run_id'))	jxprocess.    o2app_user() == 'root'	jxprocess/   * (o2app_user() == 'root' ? 'jximgbtn' : '') 	jxprocess0   =   $obj = o2val('prg�_�var','run_obj');
$val = $obj->developer; $val	jxprocess1    'disp_area'	jxprocess2   3 o2app_user() == 'root' || o2app_runlevel() == 'DEV' 	jxprocess3   "   o2_send(o2_path(o2_reqpar(1), 1)); true 	jxprocess4   �  $zip  = new ZipArchive(); 
$name = o2app_dir_tmp().o2val('prg�_�var','job_name').'-'.o2val('prg�_�var','run_id').'.zip';
o2file_delete($name);
$zip->open($name, ZipArchive::CREATE);
foreach (o2val('prg�_�var','run_files') as $path) {
   $file  = basename($path);
   /*   Add file to archive   */
   $zip->addFile($path, $file);
   }
$zip->close();
o2_send(o2_path($name, 1)); true