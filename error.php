<?php
	echo cach_set_dir('/InterSystems/Cache/mgr'),"\n";
	echo cach_connect("dev","dsdev"),"\n";
	echo cach_errno(),"\n";
	echo cach_error(),"\n";
	/*$start = microtime(true);
	for ($i=0;$i<10000000;$i++) {
		cach_set('^time',1);
	}
	$time = microtime(true) - $start;
	echo 'Скрипт выполнялся: ', $time," секунд\n";*/

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
	echo cach_zkill('^forKill','global'),"\n";
	echo cach_kill('^forKill','tree'),"\n";
?>