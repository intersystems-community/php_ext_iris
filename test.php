<?php
	/*
	ini_set('cache.shdir','/usr/lib/abadon/mgr');
	echo ini_get('cache.shdir');
	*/
	/*echo phpinfo();*/
	echo cach_set_dir('/usr/lib/abadon/mgr'),"\n";
	echo cach_connect("test","1234"),"\n";

	/*$start = microtime(true);
	for ($i=0;$i<10000000;$i++) {
		cach_set('^time',1);
	}
	$time = microtime(true) - $start;
	echo 'Скрипт выполнялся: ', $time," секунд\n";*/

	echo cach_set('^forKill','global',1),"\n";
	echo cach_set('^forKill','tree',1,1,'value'),"\n";
	echo cach_set('^forKill','tree',1,2,'value'),"\n";
	echo cach_set('^forKill','tree',1,3,'value'),"\n";
	echo cach_set('^forKill','tree',2,1,'value'),"\n";
	echo cach_set('^forKill','tree',2,2,'value'),"\n";
	echo cach_set('^forKill','tree',2,3,'value'),"\n";
	echo cach_set('^ccc','new1','res1'),"\n";
	echo cach_set('^ccc','new2','res1',5),"\n";
	echo cach_set('^ccc','new2','res1',5,4),"\n";
	echo cach_set('^ccc','new2','res1',5,4,6),"\n";
	echo cach_set('^ccc','new2','res1',5,4,6,7),"\n";
	echo cach_set('^ccc','new2','res1',5,4,6,7,8),"\n";
	echo cach_set('^ccc','new2','res2',6),"\n";
	echo cach_set('^ccc','new2','res3',7),"\n";
	echo cach_set('^ccc','new3','res3'),"\n";   
	echo cach_set('^ccc','new'),"\n"; 
	echo cach_set('^ccc','test','yep'),"\n"; 
	echo cach_set('^ccc','sss',5),"\n";
	echo cach_get('^ccc','test'),"\n"; 
	echo cach_get('^ccc','sss'),"\n"; 
	echo cach_set('^ccc','test','yep','agw','morez','params',1337),"\n"; 
	echo cach_set('^ccc','test','yep','agw','morez','params',1337,5),"\n";
	echo cach_get('^ccc','test','yep','agw','morez','params'),"\n"; 
	echo cach_set('^ccc','test','yep','agw','morez',1337),"\n";
	echo cach_order('^ccc','new2','res2'),"\n";
	echo cach_order_rev('^ccc','new2','res2'),"\n";
	echo cach_kill('^forKill','global'),"\n";
	echo cach_kill_tree('^forKill','tree'),"\n";

	echo "\norder:",$i="","\n";
	$i = cach_order('^ccc',$i);
	for (;$i<>"";) {
		echo $i,"\n";
		$i = cach_order('^ccc',$i);
	}

	echo "\nquery:\n";
	for ($q=cach_query('^ccc','new2');$q !== NULL;$q=cach_query($q)) {
		echo cach_get($q),"\n";
		cach_set($q,23);
		print_r($q);
	}

	echo "\nexec:\n";
	echo cach_exec("set ^global(5)=\"value\""),"\n";
	echo cach_exec("set ^global(6)=\"value\""),"\n";
	echo cach_exec("set ^global(7)=\"value\""),"\n";
	echo cach_exec("kill ^global(6)"),"\n";
?>