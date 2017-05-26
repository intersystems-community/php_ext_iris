<?php
	/*
	ini_set('cache.shdir','/usr/lib/abadon/mgr');
	*/
	echo cach_set_dir('/usr/lib/abadon/mgr'),"\n";//Fix this
	echo cach_connect('test','test'),"\n";
	echo cach_global_set('ccc','new1','res1'),"\n";
	echo cach_global_set('ccc','new2','res2'),"\n";
	echo cach_global_set('ccc','new3','res3'),"\n";   
	echo cach_global_set('ccc','new'),"\n"; 
	echo cach_global_set('ccc','test','yep'),"\n"; 
	echo cach_global_set('ccc','sss',5),"\n";
	echo cach_global_get('ccc','test'),"\n"; 
	echo cach_global_get('ccc','sss'),"\n"; 
	echo cach_global_set('ccc','test','yep',5,'more','params',1337),"\n"; 
	echo cach_global_get('ccc','test','yep',5,'more','params'),"\n"; 
	echo cach_global_set('ccc','test','yep',5,'more','params',1337),"\n";
	echo cach_global_kill('ccc','test','yep',5,0),"\n"; // last params: 0 - kill tree, 1 - kill global 
	echo cach_global_order('ccc','new2',-1),"\n"; // Direction 1 for forward, -1 for reverse 
	echo cach_global_order('ccc','new2',1),"\n"; // Direction 1 for forward, -1 for reverse.
	//echo phpinfo();
?>