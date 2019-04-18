<?PHP
error_reporting(E_ALL);
ini_set('display_errors',1);

session_start();

$sError = false;

if (
  (!isset($_SESSION['connected']) || (isset($_SESSION['connected']) && !$_SESSION['connected'])) &&
  isset($_POST['submit']) && $_POST['submit'])
{
  $ok = cach_set_dir($_POST['path']);
  if ($ok) $ok = cach_connect($_POST['login'], $_POST['pass']);
  if ($ok) 
  {
    $_SESSION['connected'] = true;
    $_SESSION['path'] = $_POST['path'];
    $_SESSION['login'] = $_POST['login'];
    $_SESSION['pass'] = $_POST['pass'];

    cach_exec('set ^CacheVer = $zv');
    cach_exec('set ^CacheNS = $NAMESPACE');

    $_SESSION['CacheVer'] = cach_get('^CacheVer');
    $_SESSION['CacheNS'] = cach_get('^CacheNS');
  }

  if (!$ok)
    $sError = 'Error: '.cach_error().' Code: '.cach_errno();
}
elseif (isset($_POST['changeNS']) && $_POST['changeNS'] && isset($_POST['nmsp']) && $_POST['nmsp'])
{
  if (cach_exec('znspace "'.$_POST['nmsp'].'"'))
    $_SESSION['CacheNS'] = $_POST['nmsp'];
}
elseif (isset($_POST['logout']))
{
  unset(
    $_SESSION['connected'], $_SESSION['CacheVer'], $_SESSION['CacheNS'],
    $_SESSION['path'], $_SESSION['login'], $_SESSION['pass']
  );
  cach_quit();
}

if (isset($_SESSION['connected']) && $_SESSION['connected'])
{
  $ok = cach_set_dir($_SESSION['path']);
  if ($ok) $ok = cach_connect($_SESSION['login'], $_SESSION['pass']);
}

function gL($s)
{
  return $GLOBALS['_LANG'][$s];
}

function getGlobalFromPOST($sName, $i1=NULL, $i2=NULL)
{
  $a = array();
  
  if (isset($_POST[$sName]))
  {
    $sGlobal = trim($_POST[$sName]);
    if (strlen($sGlobal) == 0)
      return $a;

    $a[] = $sGlobal;

    if (isset($_POST[$i1]))
    {
      $sI1 = trim($_POST[$i1]);
      if (strlen($sI1) == 0)
        return $a;

      $a[] = $sI1;

      if (isset($_POST[$i2]))
      {
        $sI2 = trim($_POST[$i2]);
        if (strlen($sI2) == 0)
          return $a;

        $a[] = $sI2;
      }
    }
  }
  
  return $a;
}

function pRes($opt)
{
  $res = '';
  
  if (!isset($_POST['bSubmitted']))
    return '';
    
  if (!in_array($_POST['opt'], array('set', 'get', 'kill', 'zkill', 'order', 'order_rev', 'query', 'errno', 'error', 'exec', 'quit')))
    return '';
    
  if ($_POST['opt'] != $opt)
    return '';
  
  switch ($_POST['opt'])
  {
    case 'get':
    case 'kill':
    case 'zkill':
    case 'order':
    case 'order_rev':
    case 'query': 

      $a = getGlobalFromPOST('Global', 'I1', 'I2');

      break;

    case 'set':
      $a = getGlobalFromPOST('setGlobal', 'setI1', 'setI2');

  }

  switch ($_POST['opt'])
  {
    case 'get':
    case 'kill':
    case 'zkill':
    case 'order':
    case 'order_rev':
    case 'query': 
    case 'set':
      if (count($a)==0)
        return '<div class=error>'.gL('error_parameters').'</div>';
  }

  switch ($_POST['opt'])
  {
    case 'set':
      $res = cach_set($a, $_POST['setVal']);
      break;
    case 'get':
      $res = cach_get($a);
      break;
    case 'kill':
      $res = cach_kill($a);
      break;
    case 'zkill':
      $res = cach_zkill($a);
      break;
    case 'order':
      $res = cach_order($a);
      break;
    case 'order_rev':
      $res = cach_order_rev($a);
      break;
    case 'query': 
      $res = cach_query($a);
      break;
    case 'exec': 
      $res = cach_exec($_POST['cmd']);
      break;
    case 'errno':
      $res = cach_errno();
      break;
    case 'error':
      $res = cach_error();
      break;
    case 'quit':
      $res = cach_quit();
      break;
  }
  
  if ($res === false)
    $res = var_export($res, true).'<br>'.cach_error().'<br>'.cach_errno();
  else
    $res = var_export($res, true);
  
  return $res;
}

function getmicrotime()
{
    list($usec, $sec) = explode(" ",microtime());
    return ((float)$usec + (float)$sec);
}

function pTest($opt)
{
  $res = '';

  if (!isset($_POST['bSubmitted']))
    return '';

  if (!in_array($_POST['opt'], array('test_povtor', 'test_cicle')))
    return '';

  if ($_POST['opt'] != $opt)
    return '';

  $a = array();

  switch ($_POST['opt'])
  {
    case 'test_povtor':
      $a = getGlobalFromPOST('povtorGlobal', 'povtorI1', 'povtorI2');

      $c = intval($_POST['povtorCount']);

      break;
      
    case 'test_cicle':
      $a = getGlobalFromPOST('cicleGlobal');
      
      $c = intval($_POST['cicleCount']);
  }
  
  if (count($a)==0)
    return '<div class=error>'.gL('error_parameters').'</div>';
    
  if ($c > 1000000)
        return '<div class=error>'.gL('error_very_big_count').'</div>';

  
  $time_start = getmicrotime();
  
  switch ($_POST['opt'])
  {
    case 'test_povtor':
    
      for ($i=0; $i < $c; $i++)
        if (cach_set($a, $i) === false)
          break;
          
      break;
      
    case 'test_cicle':
      $a = $a[0];
      
      for ($i=0; $i < $c; $i++)
        if (cach_set($a, $i, $i) === false)
          break;
  }    
  $res = (getmicrotime() - $time_start).' sec.';
  
  return $res;
}
  
function getAuthForm()
{
  $s = '<title>Authentication in InterSystems Cache from PHP</title>';
  
  if ($GLOBALS['sError'])
    $s .= '<div class=error>'.$GLOBALS['sError'].'</div>';

  $s .= '<form method="POST" class="auth">
    '.gL('db_path'). ': <input name="path" type="text" value="/InterSystems/Cache/mgr"><br>
    '.gL('db_login').': <input name="login" type="text" value="'.(isset($_POST['login'])?$_POST['login']:'').'"><br>
    '.gL('db_pass'). ': <input name="pass" type="password" value=""><br>
    <button name="submit" type="submit" value="1">'.gL('btnEnter').'</button>
  </form>';
  return $s;
}

function getHeader()
{
   $res = '<title>Module for work with InterSystems Cache from PHP</title>
    Module PHP version: '.phpversion('cache_ext').'<br>
    PHP version: '.phpversion().'<br>
    Cache version: '.$_SESSION['CacheVer'].'<br>';
  
  return $res;
}

function getInputField($s)
{
  return '<input name="'.$s.'" value="'.(isset($_POST[$s])?$_POST[$s]:'').'">';
}

function getFunctionTable()
{
   $res = '
   <H2>'.gL('main_table_caption').'</H2>
     <table class=main_table>
     <tr><th>'.gL('th_php_function').'</th><th colspan=2>'.gL('th_param').'</th><th>'.gL('th_res').'</th></tr>
     <tr><td>cach_set</td>
     <td><button name="cmd" value="set" >'.gL('btRun').'</button></td>
         <td>
           '.gl('caption_global').' '.getInputField('setGlobal').'<br>
           '.gl('caption_i1').' '.getInputField('setI1').'<br>
           '.gl('caption_i2').' '.getInputField('setI2').'<br>
           '.gl('caption_value').' '.getInputField('setVal').'
         </td>
         <td>'.pRes('set').'</td>
     </tr>
     <tr><td>cach_get</td>
     <td><button name="cmd" value="get" >'.gL('btRun').'</button></td>
         <td rowspan=6>
           '.gl('caption_global').' '.getInputField('Global').'<br>
           '.gl('caption_i1').' '.getInputField('I1').'<br>
           '.gl('caption_i2').' '.getInputField('I2').'<br>
         </td>
         <td>'.pRes('get').'</td>
     </tr>
     <tr><td>cach_kill</td>
         <td><button name="cmd" value="kill" >'.gL('btRun').'</button></td>
         <td>'.pRes('kill').'</td>
     </tr>
     <tr><td>cach_zkill</td>
         <td><button name="cmd" value="zkill" >'.gL('btRun').'</button></td>
         <td>'.pRes('zkill').'</td>
     </tr>
     <tr><td>cach_order</td>
         <td><button name="cmd" value="order" >'.gL('btRun').'</button></td>
         <td>'.pRes('order').'</td>
     </tr>
     <tr><td>cach_order_rev</td>
         <td><button name="cmd" value="order_rev" >'.gL('btRun').'</button></td>
         <td>'.pRes('order_rev').'</td>
     </tr>
     <tr><td>cach_query</td>
         <td><button name="cmd" value="query" >'.gL('btRun').'</button></td>
         <td>'.pRes('query').'</td>
     </tr>
     <tr><td>cach_exec</td>
         <td><button name="cmd" value="exec">'.gL('btRun').'</button></td>
         <td>
           '.gL('caption_cmd_string').' '.getInputField('cmd').'
         </td>
         <td>'.pRes('exec').'</td>
     </tr>
     <tr><td>cach_errno</td>
     <td><button name="cmd" value="errno" >'.gL('btRun').'</button></td>
         <td></td>  
         <td>'.pRes('errno').'</td>
     </tr>
     <tr><td>cach_error</td>
         <td><button name="cmd" value="error" >'.gL('btRun').'</button></td>
         <td></td>
         <td>'.pRes('error').'</td>
     </tr>
     <tr><td>cach_quit</td>
     <td><button name="cmd" value="quit" >'.gL('btRun').'</button></td>
         <td></td>   
         <td>'.pRes('quit').'</td>
     </tr>
     </table>';
     
     return $res;
}

function getTestTable()
{
  $res = '<H2>'.gL('test_table_caption').'</H2>
  <table class=main_table>
    <tr><th>'.gL('th_test_name').'</th><th colspan=2>'.gL('th_test_param').'</th><th>'.gL('th_test_res').'</th></tr>
    <tr><td>'.gL('caption_test_povtor').'</td>
         <td><button name="cmd" value="test_povtor" >'.gL('btRun').'</button></td>
         <td>
           '.gl('caption_global').' '.getInputField('povtorGlobal').'<br />
           '.gl('caption_i1').' '.getInputField('povtorI1').'<br />
           '.gl('caption_i2').' '.getInputField('povtorI2').'<br />
           '.gL('caption_povtor_count').' '.getInputField('povtorCount').'<br />
           <br />
         </td>
         <td>'.pTest('test_povtor').'</td>
    </tr>
    <tr><td>'.gL('caption_test_cicle').'</td>
         <td><button name="cmd" value="test_cicle" >'.gL('btRun').'</button></td>
         <td>
           '.gl('caption_global').' '.getInputField('cicleGlobal').'<br />
           '.gL('caption_cicle_count').' '.getInputField('cicleCount').'<br />
         </td>
         <td>'.pTest('test_cicle').'</td>
     </tr> 
  </table>';
     
  return $res;   
}

function getChangeNamespace()
{
  $res = 'Namespace: <input style="margin-top:15" name="nmsp" type="text" value="'.$_SESSION['CacheNS'].'">
  <input name="changeNS" type="submit" value="'.gL('change_namespace').'">';
  
  return $res;
}

function getDemo()
{
  $res = getHeader().'
  <form method="POST" name=main>'.
  getChangeNamespace().'<p>'.
  getFunctionTable().'<p>'.
  getTestTable().'
  <input type=hidden name=bSubmitted value=1 />
  <input type=hidden name=opt value="" />
  </form>
  <p>
  <form method="POST">
    <input type=submit name=logout value="Exit" />
  </form> 
  ';
  
  return $res;
}
?>

<html>
<head>
<style>
  body {
    text-align: center;
  }
  form.auth input[type=text] {
    margin-top: 15px;
  }
  
  form.auth input[type=submit] {
    margin-top: 25px;
  }
  .main_table {
    border: 2px solid;
    width: 900px;
    margin: auto;
    border-collapse: collapse;
  }
  .main_table td, .main_table th {
    border: 1px solid;
    padding: 6px 4px 6px 10px; 
    margin: 0px;
  }
  .error {
    color: red;
    font-weight: bold;
    text-align: center;
  }
</style>

</head>
<body>
  <h1>Cache extension</h1>

<?PHP

if (!isset($_SESSION['connected']) || !$_SESSION['connected'])
{
  echo getAuthForm();
}
else
{
  echo getDemo();
}

?>
<script language="JavaScript">
  var elements = document.getElementsByTagName('button');
  for(var i = 0, len = elements.length; i < len; i++)
  {
    elements[i].onclick = function (event)
    {
      document.forms['main'].elements['opt'].value = event.currentTarget.value;
    }
  }
</script>
</body>     
</html>
