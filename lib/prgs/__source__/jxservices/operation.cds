��             �prg I    WIDTH  d azione I    WIDTH  ( id      o2ref I    WIDTH  � deferred      	callparam K    SUBTYPE I  Text exp1      exp2      exp3      
operazione I   WIDTH   	tipologia I    WIDTH  ( service I    WIDTH  2 exp4      exp5      exp6      exp7      exp8       
CHANGE_LOG � H                                                                                   	          
                                                                                                                                                        �
jxservicesadd_host_service   
[o2exp_32]                Call program               �
jxservicesadd_host_service   create_tree::True::                Execute action               �
jxservicescreate_tree                   Execute script               �
jxservicescreate_tree   prg�_�var�sele_type               Update               �
jxservicescreate_tree   prg�_�var�sele_host               Update               �
jxservicescreate_tree   prg�_�var�sele_service               Update               �
jxservicescreate_tree   prg�_�var�nice_host               Update               �
jxservicescreate_tree   prg�_�var�host_services               Update               �
jxservicescron   
[o2exp_34]                Call program               �
jxservices	dele_host                   Execute script               �
jxservices	dele_host   create_tree::True::                Execute action               �
jxservices	goto_jobs   
[o2exp_10]                Go toprogram              �
jxservicesgoto_processes   
[o2exp_38]                Go toprogram              �
jxservices
goto_sched   
[o2exp_39]                Go toprogram              �
jxservicesnode_detail   prg�_�var�sele_type               Update               �
jxservicesnode_detail   sele_host::True::               Execute action               �
jxservicesnode_detail   sele_service::True::               Execute action               �
jxservices	sele_host   prg�_�var�sele_host               Update               �
jxservices	sele_host   prg�_�var�nice_host               Update               �
jxservices	sele_host   prg�_�var�host_services               Update               �
jxservices	sele_host   prg�_�var�sele_service               Update               �
jxservicessele_service   prg�_�var�sele_service               Update               �
jxservicessele_service                   Execute script               �
jxservicesstart   create_tree::True::                Execute action             