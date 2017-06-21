<?php
	echo cach_set_dir('/InterSystems/Cache/mgr'),"<br>";
	echo cach_connect("root","pFg4Fn4AMiHTB24wj99s"),"<br>";

	/*$start = microtime(true);
	for ($i=0;$i<10000000;$i++) {
		cach_set('^time',1);
	}
	$time = microtime(true) - $start;
	echo 'Скрипт выполнялся: ', $time," секунд<br>";*/

	echo cach_set('^forKill','tree',1,1,'value'),"<br>";
	echo cach_set('^forKill','tree',1,2,'value'),"<br>";
	echo cach_set('^forKill','tree',1,3,'value'),"<br>";
	echo cach_set('^forKill','tree',2,1,'value'),"<br>";
	echo cach_set('^forKill','tree',2,2,'value'),"<br>";
	echo cach_set('^forKill','tree',2,3,'value'),"<br>";
	echo cach_set('^ccc','new1','res1'),"<br>";
	echo cach_set('^ccc','new2','res1',5),"<br>";
	echo cach_set('^ccc','new2','res1',5,4),"<br>";
	echo cach_set('^ccc','new2','res1',5,4,6),"<br>";
	echo cach_set('^ccc','new2','res1',5,4,6,7),"<br>";
	echo cach_set('^ccc','new2','res1',5,4,6,7,8),"<br>";
	echo cach_set('^ccc','new2','res2',6),"<br>";
	echo cach_set('^ccc','new2','res3',7),"<br>";
	echo cach_set('^ccc','new3','res3'),"<br>";   
	echo cach_set('^ccc','new'),"<br>"; 
	echo cach_set('^ccc','test','yep'),"<br>"; 
	echo cach_set('^ccc','sss',5),"<br>";
	echo cach_get('^ccc','test'),"<br>"; 
	echo cach_get('^ccc','sss'),"<br>"; 
	echo cach_set('^ccc','test','yep','agw','morez','params',1337),"<br>"; 
	echo cach_set('^ccc','test','yep','agw','morez','params',1337,5),"<br>";
	echo cach_get('^ccc','test','yep','agw','morez','params'),"<br>"; 
	echo cach_set('^ccc','test','yep','agw','morez',1337),"<br>";
	echo cach_zkill('^forKill','global'),"<br>";
	echo cach_kill('^forKill','tree'),"<br>";

	echo "<br>order:",$i="","<br>";
	$i = cach_order('^ccc',$i);
	for (;$i;) {
		echo $i,"<br>";
		$i = cach_order('^ccc',$i);
	}

	echo "<br>query:<br>";
	for ($q=cach_query('^ccc','new2');$q !== NULL;$q=cach_query($q)) {
		echo cach_get($q),"<br>";
		cach_set($q,23);
		print_r($q);
	}

	echo "<br>exec:<br>";
	echo cach_exec("set ^global(5)=\"value\""),"<br>";
	echo cach_exec("set ^global(6)=\"value\""),"<br>";
	echo cach_exec("set ^global(7)=\"value\""),"<br>";
	echo cach_exec("kill ^global(6)"),"<br>";
	echo cach_exec("set ^aa = \$znspace"),"<br>";
	var_dump(cach_get("^aa"));
	echo cach_exec("znspace \"SAMPLES\""),"<br>";
	echo cach_exec("set ^aa = \$znspace"),"<br>";
	var_dump(cach_get("^aa"));
	var_dump(cach_get("^notfnd"));
?>
