<?php
echo "Setting dir:\n";
echo iris_set_dir('/usr/irissys/mgr'),"\n";

echo "Connecting:\n";

if (!iris_connect("php","burmur")) 
{
  echo "Error connecting: ";
  echo iris_error().': '.iris_errno(),"\n";
  die();
}
	/*$start = microtime(true);
	for ($i=0;$i<10000000;$i++) {
		iris_set('^time',1);
	}
	$time = microtime(true) - $start;
	echo 'Скрипт выполнялся: ', $time," секунд\n";*/
echo "Setting nodes:\n";
	echo iris_set('^forKill','tree',1,1,'value'),"\n";
	echo iris_set('^forKill','tree',1,2,'value'),"\n";
	echo iris_set('^forKill','tree',1,3,'value'),"\n";
	echo iris_set('^forKill','tree',2,1,'value'),"\n";
	echo iris_set('^forKill','tree',2,2,'value'),"\n";
	echo iris_set('^forKill','tree',2,3,'value'),"\n";
	echo iris_set('^ccc','new1','res1'),"\n";
	echo iris_set('^ccc','new2','res1',5),"\n";
	echo iris_set('^ccc','new2','res1',5,4.0),"\n";
	echo iris_set('^ccc','new2','res1',5,4,6),"\n";
	echo iris_set('^ccc','new2','res1',5,4,6,7),"\n";
	echo iris_set('^ccc','new2','res1',5,4,6,7,8),"\n";
	echo iris_set('^ccc','new2','res2',6),"\n";
	echo iris_set('^ccc','new2','res3',7),"\n";
	echo iris_set('^ccc','new3','res3'),"\n";   
	echo iris_set('^ccc','new'),"\n"; 
	echo iris_set('^ccc','test','yep'),"\n"; 
	echo iris_set('^ccc','sss',5.1E25),"\n";
        echo iris_set('^ccc', 4.2, 6.2),"\n";
        echo iris_set('^ccc', 3, 6.2),"\n";
echo "getting yep=\n";
	echo iris_get('^ccc','test'),"\n"; 
        echo "getting 5.3=\n";
	echo iris_get('^ccc','sss'),"\n"; 
	echo iris_set('^ccc','test','yep','agw','morez','params',1337),"\n"; 
	echo iris_set('^ccc','test','yep','agw','morez','params',1337,5),"\n";
echo "getting 1337=\n";
	echo iris_get('^ccc','test','yep','agw','morez','params'),"\n"; 
	echo iris_set('^ccc','test','yep','agw','morez',1337),"\n";
echo "ZKilling:";
	echo iris_zkill('^forKill','global'),"\n";
echo "Killing:";
	echo iris_kill('^forKill','tree'),"\n";

echo "\nOrder:\n";
/*
$i = iris_order('^ccc', '');
echo $i."\n";
$i = iris_order('^ccc',$i);

echo "$i\n";
$i = iris_order('^ccc',$i);

echo "$i\n";
$i = iris_order('^ccc',$i);
*/

for ($i = iris_order('^ccc', 'new2', 'res1', ''); ($i !== false) && ($i !== NULL); $i = iris_order('^ccc', 'new2', 'res1',$i))
{
  var_dump($i);
}

echo "\nRev Order:",$i="zzzzz","\n";

for ($i = iris_order_rev('^ccc',$i); $i !==NULL; $i = iris_order_rev('^ccc',$i))
{
  echo "$i\n";
}

echo "\nQuery:\n";

for ($q=array('^ccc'); $q !== NULL; $q=iris_query($q))
{
  //echo iris_get($q),"\n";
  //iris_set($q,23);
  echo join(',', $q), ' = '. iris_get($q)."\n";
}

die();
echo "\nExec:\n";
echo iris_exec("set ^global(5)=\"value\""),"\n";
echo iris_exec("set ^global(6)=\"value\""),"\n";
echo iris_exec("set ^global(7)=\"value\""),"\n";
echo iris_exec("kill ^global(6)"),"\n";
echo iris_exec("set ^aa = \$znspace"),"\n";
var_dump(iris_get("^aa"));
echo iris_exec("znspace \"SAMPLES\""),"\n";
echo iris_exec("set ^aa = \$znspace"),"\n";
var_dump(iris_get("^aa"));
var_dump(iris_get("^notfnd"));
?>
