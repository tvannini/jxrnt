��       +      �prg I    WIDTH  d idexp      expr K    SUBTYPE I  Text return I    WIDTH  �  
CHANGE_LOG � �                                                                                   	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +   	       
jxservices   U   $list = o2_service::get_for_tree();
o2tree_def('services', $list, 1, 'node_detail'); true
jxservices    o2tree_get_code('services')
jxservices   + o2_str_nice(o2val('prg�_�var','sele_host'))
jxservices   
 'jximgbtn'
jxservices    '<jx>/img/jobs/refresh.png'
jxservices    '<jx>/img/jobs/separator.png'
jxservices    'jxtoolbar'
jxservices    '<jx>/img/jobs/jobs24.png'
jxservices	    'jxtree'
jxservices
    'tools/jxjobs'
jxservices    'disp_area' 
jxservices   y   $text = "<center><h3>Janox Services Manager</h3></center><br>Select an item from the treeview<br>to see its properties."; $text
jxservices    '<jx>/img/jxpowered.png'
jxservices    false 
jxservices   �   $type = '';
$sele = o2tree_get_selection('services');
if ($sele) {
   $list = o2tree_get_nodes('services');
   $type = $list[$sele][3];
   } $type
jxservices   ( o2val('prg�_�var','sele_type') == 'host' 
jxservices   �   $host = '';
$sele = o2tree_get_selection('services');
if ($sele) {
   $list = o2tree_get_nodes('services');
   $host = $list[$sele][5];
   } $host
jxservices   ' o2val('prg�_�var','sele_type') == 'srv' 
jxservices   �   $srv  = '';
$sele = o2tree_get_selection('services');
if ($sele) {
   $list = o2tree_get_nodes('services');
   $srv  = $list[$sele][4];
   } $srv
jxservices   ( o2val('prg�_�var','sele_type') != 'host'
jxservices   ( o2val('prg�_�var','sele_type') == 'host'
jxservices    o2val('prg�_�var','nice_host')
jxservices   = o2_service::get_host_services(o2val('prg�_�var','sele_host'))
jxservices   " o2val('prg�_�var','host_services') 
jxservices   ?   o2tree_node_fold('services', o2tree_get_selection('services')); true
jxservices    '<jx>/img/jobs/srv_host.png'
jxservices   Y '<jx>/img/jobs/host_dele'.(o2val('prg�_�var','sele_type') == 'host' ? '' : '_dis').'.png' 
jxservices   c   o2_service::remove_host_service(o2val('prg�_�var','sele_host'), o2val('prg�_�var','sele_service')); true 
jxservices   �   $msg = 'Host '.o2val('prg�_�var','nice_host').' is going to be removed from service '.
       o2_str_nice(o2val('prg�_�var','sele_service'))."\nProceed?"; $msg
jxservices    ''
jxservices    array()
jxservices     'tools/jxservice_add_host' 
jxservices!   �   $img = "<jx>/img/jobs/";
if (o2_cron::is_registered()) {
   $img.= "cron_on24.png";
   }
else {
   $img.= "cron_off24.png";
   } $img
jxservices"    'tools/jxcron' 
jxservices#   8   $code = 'Host: '.o2_str_nice($_SESSION['o2_app']->host); $code
jxservices$    '<jx>/img/jobs/run_job.png'
jxservices%    '<jx>/img/jobs/scheduler.png'
jxservices&    'tools/jxprocesses'
jxservices'    'tools/jxscheduler'
jxservices(    'hidden'
jxservices)   
 'nowindow'
jxservices*   	 'nav_big'
jxservices	    'jxtree jxtree_bottom'