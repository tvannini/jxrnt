��       7      prg I    WIDTH  d azione I    WIDTH  ( id      o2ref I    WIDTH  � deferred      	callparam K    SUBTYPE I  Text exp1      exp2      exp3      
operazione I   WIDTH   	tipologia I    WIDTH  ( service I    WIDTH  2 exp4      exp5      exp6      exp7      exp8       
CHANGE_LOG � �                                                                                   	          
                                                                                                                                                                                                                                       !          "          #          $          %          &          '          (          )          *          +          ,          -          .          /          0          1          2          3          4          5          6          7            �jxjobs
create_job   prg�_�var�job_id    0           Update 28^             �jxjobs
create_job   
[o2exp_55]       prg�_�var�job_id          Call program  ^            �jxjobs
create_job   set_tree_list::True::               Execute action  ^            �jxjobs
create_job        2          Execute script  ^            �jxjobs
create_job   node_detail::True::                Execute action  ^            �jxjobs
delete_job        /           Execute script  ^            �jxjobs
delete_job   prg�_�var�job_id    0           Update  ^            �jxjobs
delete_job   set_tree_list::True::                Execute action  ^            �jxjobsdelete_sched   prg�_�var�par_job_id    I           Update  ^            �jxjobsdelete_sched   prg�_�var�confirm    G           Confirm  ^            �jxjobsdelete_sched        5   H       Execute script  ^            �jxjobsdelete_sched   set_tree_list::True::                Execute action  ^            �jxjobsdelete_sched   node_detail::True::                Execute action  ^             �jxjobsedit_job   
[o2exp_55]    
   [o2exp_32]          Call program  ^            �jxjobsedit_job   set_tree_list::True::                Execute action  ^            �jxjobsedit_job   node_detail::True::                Execute action  ^            �jxjobsexe_job        ,           Execute script  ^            �jxjobsexe_job   node_detail::True::                Execute action  ^            �jxjobsfilter_tree   set_tree_list::True::                Execute action  ^            �jxjobsfilter_tree                   Execute script  ^            �jxjobs	fold_tree                   Execute script  ^            �jxjobsget_run_jobs_list   prg�_�var�run_jobs_list               Update  ^            �jxjobsget_sched_jobs_list   prg�_�var�sched_job_list               Update  ^            �jxjobsgoto_processes   
[o2exp_56]                Go toprogram ^            �jxjobs
goto_sched   
[o2exp_57]                Go toprogram ^            �jxjobsgoto_services   
[o2exp_84]                Go toprogram ^            �jxjobsnode_detail   prg�_�var�job_obj               Update  ^            �jxjobsnode_detail   prg�_�var�job_id               Update  ^            �jxjobsnode_detail   prg�_�var�job_name               Update  ^            �jxjobsnode_detail   prg�_�var�job_desc               Update  ^            �jxjobsnode_detail   prg�_�var�job_prg               Update  ^            �jxjobsnode_detail   prg�_�var�job_service    J           Update  ^            �jxjobsnode_detail   prg�_�var�job_params    	           Update  ^            �jxjobsnode_detail   prg�_�var�job_keep_days    <           Update  ^            �jxjobsnode_detail	   prg�_�var�job_max_instances    >           Update  ^            �jxjobsnode_detail
   prg�_�var�job_block_mode    @           Update  ^            �jxjobsnode_detail   prg�_�var�job_disabled    A           Update  ^            �jxjobsnode_detail   get_run_jobs_list::True::                Execute action  ^            �jxjobsnode_detail   get_sched_jobs_list::True::                Execute action  ^            �jxjobs	queue_job        Q           Execute script  ^            �jxjobs	queue_job   node_detail::True::                Execute action  ^             �jxjobsrun_job_detail   
[o2exp_59]    
   [o2exp_54]          Call program  ^            �jxjobsrun_job_detail   node_detail::True::                Execute action  ^             �jxjobs	sched_add   
[o2exp_58]       [o2exp_48]
[o2exp_32]          Call program  ^            �jxjobs	sched_add   set_tree_list::True::                Execute action  ^            �jxjobs	sched_add   node_detail::True::                Execute action  ^             �jxjobssched_detail   
[o2exp_58]    
   [o2exp_52]          Call program  ^            �jxjobssched_detail   node_detail::True::                Execute action  ^            �jxjobsset_tree_list                   Execute script  ^            �jxjobs	sort_tree   prg�_�var�sort_tree               Update  ^            �jxjobs	sort_tree   set_tree_list::True::                Execute action  ^            �jxjobsstart   set_tree_list::True::                Execute action  ^            �jxjobsswitch_tree_group   prg�_�var�group_tree    M           Update  ^            �jxjobsswitch_tree_group   set_tree_list::True::                Execute action  ^            �jxjobsunfold_tree                   Execute script  ^          