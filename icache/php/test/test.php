<?php
echo "Setting dir:\n";
echo cach_set_dir('/InterSystems/Cache/mgr'),"\n";

echo "Connecting:\n";

if (!cach_connect("habr_test","burmur#@8765")) 
{
  echo "Error connecting: ";
  echo cach_error().': '.cach_errno(),"\n";
  die();
}
	/*$start = microtime(true);
	for ($i=0;$i<10000000;$i++) {
		cach_set('^time',1);
	}
	$time = microtime(true) - $start;
	echo 'Скрипт выполнялся: ', $time," секунд\n";*/
echo "Setting nodes:\n";
	echo cach_set('^forKill','tree',1,1,'value'),"\n";
	echo cach_set('^forKill','tree',1,2,'value'),"\n";
	echo cach_set('^forKill','tree',1,3,'value'),"\n";
	echo cach_set('^forKill','tree',2,1,'value'),"\n";
	echo cach_set('^forKill','tree',2,2,'value'),"\n";
	echo cach_set('^forKill','tree',2,3,'value'),"\n";
	echo cach_set('^ccc','new1','res1'),"\n";
	echo cach_set('^ccc','new2','res1',5),"\n";
	echo cach_set('^ccc','new2','res1',5,4.0),"\n";
	echo cach_set('^ccc','new2','res1',5,4,6),"\n";
	echo cach_set('^ccc','new2','res1',5,4,6,7),"\n";
	echo cach_set('^ccc','new2','res1',5,4,6,7,8),"\n";
	echo cach_set('^ccc','new2','res2',6),"\n";
	echo cach_set('^ccc','new2','res3',7),"\n";
	echo cach_set('^ccc','new3','res3'),"\n";   
	echo cach_set('^ccc','new'),"\n"; 
	echo cach_set('^ccc','test','yep'),"\n"; 
	echo cach_set('^ccc','sss',5.1E25),"\n";
        echo cach_set('^ccc', 4.2, 6.2),"\n";
        echo cach_set('^ccc', 3, 6.2),"\n";
echo "getting yep=\n";
	echo cach_get('^ccc','test'),"\n"; 
        echo "getting 5.3=\n";
	echo cach_get('^ccc','sss'),"\n"; 
	echo cach_set('^ccc','test','yep','agw','morez','params',1337),"\n"; 
	echo cach_set('^ccc','test','yep','agw','morez','params',1337,5),"\n";
echo "getting 1337=\n";
	echo cach_get('^ccc','test','yep','agw','morez','params'),"\n"; 
	echo cach_set('^ccc','test','yep','agw','morez',1337),"\n";
echo "ZKilling:";
	echo cach_zkill('^forKill','global'),"\n";
echo "Killing:";
	echo cach_kill('^forKill','tree'),"\n";

echo "\nOrder:\n";
/*
$i = cach_order('^ccc', '');
echo $i."\n";
$i = cach_order('^ccc',$i);

echo "$i\n";
$i = cach_order('^ccc',$i);

echo "$i\n";
$i = cach_order('^ccc',$i);
*/

for ($i = cach_order('^ccc', 'new2', 'res1', ''); ($i !== false) && ($i !== NULL); $i = cach_order('^ccc', 'new2', 'res1',$i))
{
  var_dump($i);
}

echo "\nRev Order:",$i="zzzzz","\n";

for ($i = cach_order_rev('^ccc',$i); $i !==NULL; $i = cach_order_rev('^ccc',$i))
{
  echo "$i\n";
}

echo "\nQuery:\n";

for ($q=array('^ccc'); $q !== NULL; $q=cach_query($q))
{
  //echo cach_get($q),"\n";
  //cach_set($q,23);
  echo join(',', $q), ' = '. cach_get($q)."\n";
}

die();
echo "\nExec:\n";
echo cach_exec("set ^global(5)=\"value\""),"\n";
echo cach_exec("set ^global(6)=\"value\""),"\n";
echo cach_exec("set ^global(7)=\"value\""),"\n";
echo cach_exec("kill ^global(6)"),"\n";
echo cach_exec("set ^aa = \$znspace"),"\n";
var_dump(cach_get("^aa"));
echo cach_exec("znspace \"SAMPLES\""),"\n";
echo cach_exec("set ^aa = \$znspace"),"\n";
var_dump(cach_get("^aa"));
var_dump(cach_get("^notfnd"));
?>
