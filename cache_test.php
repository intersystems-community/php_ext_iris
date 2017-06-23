<?php
	session_start();
	$uid_session = session_id();
	echo '<div align="center">';
	echo '<h1>Cache extension</h1>';
	if ((!isset($_POST['submit']))&&(!isset($_POST['sbmNmsp'])&&(!isset($_POST['sbset']))&&(!isset($_POST['sbget']))&&(!isset($_POST['sbkill']))&&(!isset($_POST['sbzkill']))&&(!isset($_POST['sbord']))&&(!isset($_POST['sbordv']))&&(!isset($_POST['sbqur']))&&(!isset($_POST['sbexec']))&&(!isset($_POST['sbCset']))&&(!isset($_POST['sberrno']))&&(!isset($_POST['sberror']))&&(!isset($_POST['sbquit'])))) {
		echo '<title>Authentication in InterSystems Cache from PHP</title><form method="POST">Path: <input style="margin-top:15" name="path" type="text" value="/InterSystems/Cache/mgr"><br>User Name: <input style="margin-top:15" name="login" type="text" value="test_php"><br>Password: <input style="margin-top:15" name="pass" type="text" value="123456"><br><input style="margin-top:25" name="submit" type="submit" value="Log in"></form>';
		if (isset($_SESSION['error'])) {
			echo '<p style="color:#FF0000">'.$_SESSION['error'].'<p>';
			unset($_SESSION['error']);
		}
	} else {
		if (isset($_POST['submit'])) {
			$_SESSION['path'] = $_POST['path'];
			$_SESSION['login'] = $_POST['login'];
			$_SESSION['pass'] = $_POST['pass'];
		}
		if (isset($_POST['sbmNmsp'])) {
			$_SESSION['nmsp'] = $_POST['nmsp'];
		}
		echo "<title>Module for work with InterSystems Cache from PHP</title>";
		echo "Module PHP version: ".phpversion('cache_ext')."<br>";
		echo "PHP version: ".phpversion()."<br>";
		$res = cach_exec('set ^CacheV3rs10n("'.$uid_session.'") = $zv');
		if ((!isset($_SESSION['connected']))||(!$res)) {
			if (cach_set_dir($_SESSION['path'])) {
				if (cach_connect($_SESSION['login'],$_SESSION['pass'])) {
					$_SESSION['connected'] = 1;
					cach_exec('set ^CacheV3rs10n("'.$uid_session.'") = $zv');
				} else {
					$_SESSION['error'] = "Authentication error<br>";
					echo '<br><meta http-equiv="refresh" content="0; url=cache_test.php">';
					exit();
				}
			} else {
				$_SESSION['error'] = "Set path error<br>";
				echo '<br><meta http-equiv="refresh" content="0; url=cache_test">';
				exit();
			}
		}
		if ($temp = cach_get("^CacheV3rs10n",$uid_session)) {
			cach_exec('kill ^CacheV3rs10n("'.$uid_session.'")');
			if(isset($_SESSION['nmsp'])) {
				if (!cach_exec('znspace "'.$_SESSION['nmsp'].'"')) {
					$_SESSION['error'] = '"Get" return error code: '.cach_errno()."<br>";
				}
			}
			if (isset($_POST['sbset'])) {
				if ((empty($_POST['stGlNm']))||(empty($_POST['stVal']))) {
					$_SESSION['res_cach_set'] = false;
				} else {
					if (empty($_POST['stInx1'])) {
						if (!$_SESSION['res_cach_set'] = cach_set($_POST['stGlNm'],$_POST['stVal'])) {
							$_SESSION['error'] = '"Set" return error code: '.cach_errno()."<br>";
						}
					} else 
					if (empty($_POST['stInx2'])) {
						if (!$_SESSION['res_cach_set'] = cach_set($_POST['stGlNm'],$_POST['stInx1'],$_POST['stVal'])) {
							$_SESSION['error'] = '"Set" return error code: '.cach_errno()."<br>";
						}						
					} else {
						if (!$_SESSION['res_cach_set'] = cach_set($_POST['stGlNm'],$_POST['stInx1'],$_POST['stInx2'],$_POST['stVal'])) {
							$_SESSION['error'] = '"Set" return error code: '.cach_errno()."<br>";
						}
					}
				}
				$_SESSION['stGlNm']=$_POST['stGlNm'];
				$_SESSION['stInx1']=$_POST['stInx1'];
				$_SESSION['stInx2']=$_POST['stInx2'];
				$_SESSION['stVal']=$_POST['stVal'];
			} else
			if (isset($_POST['sbget'])) {
				if (empty($_POST['gtGlNm'])) {
					$_SESSION['res_cach_get'] = false;
				} else {
					if (empty($_POST['gtInx1'])) {
						if (false === $_SESSION['res_cach_get'] = cach_get($_POST['gtGlNm'])) {
							$_SESSION['error'] = '"Set" return error code: '.cach_errno()."<br>";
						}
					} else 
					if (empty($_POST['gtInx2'])) {
						if (false === $_SESSION['res_cach_get'] = cach_get($_POST['gtGlNm'],$_POST['gtInx1'])) {
							$_SESSION['error'] = '"Set" return error code: '.cach_errno()."<br>";
						}						
					} else {
						if (false === $_SESSION['res_cach_get'] = cach_get($_POST['stGlNm'],$_POST['stInx1'],$_POST['stInx2'])) {
							$_SESSION['error'] = '"Set" return error code: '.cach_errno()."<br>";
						}
					}
				}
				$_SESSION['gtGlNm']=$_POST['gtGlNm'];
				$_SESSION['gtInx1']=$_POST['gtInx1'];
				$_SESSION['gtInx2']=$_POST['gtInx2'];
				$_SESSION['gtVal']=$_POST['gtVal'];
			} else
			if (isset($_POST['sbkill'])) {
				if (!$_SESSION['res_cach_kill'] = cach_kill($_POST['klGlNm'],$_POST['klInx1'],$_POST['klInx2'])) {
					$_SESSION['error'] = '"Kill" return error code: '.cach_errno()."<br>";
				}
				$_SESSION['klGlNm']=$_POST['klGlNm'];
				$_SESSION['klInx1']=$_POST['klInx1'];
				$_SESSION['klInx2']=$_POST['klInx2'];
			} else
			if (isset($_POST['sbzkill'])) {
				if (!$_SESSION['res_cach_zkill'] = cach_kill($_POST['zklGlNm'],$_POST['zklInx1'],$_POST['zklInx2'])) {
					$_SESSION['error'] = '"ZKill" return error code: '.cach_errno()."<br>";
				}
				$_SESSION['zklGlNm']=$_POST['zklGlNm'];
				$_SESSION['zklInx1']=$_POST['zklInx1'];
				$_SESSION['zklInx2']=$_POST['zklInx2'];
			} else
			if (isset($_POST['sbord'])) {
				if (false === $_SESSION['res_cach_ord'] = cach_order($_POST['ordGlNm'],$_POST['ordInx1'],$_POST['ordInx2'])) {
					$_SESSION['error'] = '"Order" return error code: '.cach_errno()."<br>";
				}
				$_SESSION['ordGlNm']=$_POST['ordGlNm'];
				$_SESSION['ordInx1']=$_POST['ordInx1'];
				$_SESSION['ordInx2']=$_POST['ordInx2'];
			} else
			if (isset($_POST['sbordv'])) {
				if (false === $_SESSION['res_cach_ordv'] = cach_order_rev($_POST['ordvGlNm'],$_POST['ordvInx1'],$_POST['ordvInx2'])) {
					$_SESSION['error'] = '"Order_rev" return error code: '.cach_errno()."<br>";
				}
				$_SESSION['ordvGlNm']=$_POST['ordvGlNm'];
				$_SESSION['ordvInx1']=$_POST['ordvInx1'];
				$_SESSION['ordvInx2']=$_POST['ordvInx2'];
			} else
			if (isset($_POST['sbqur'])) {
				if (false === $_SESSION['res_cach_qur'] = cach_query($_POST['qurGlNm'],$_POST['qurInx1'])) {
					$_SESSION['error'] = '"Query" return error code: '.cach_errno()."<br>";
				}
				$_SESSION['qurGlNm']=$_POST['qurGlNm'];
				$_SESSION['qurInx1']=$_POST['qurInx1'];
			} else 
			if (isset($_POST['sbexec'])) {
				if (!$_SESSION['res_cach_exec'] = cach_exec($_POST['COS'])) {
					$_SESSION['error'] = '"Execute" return error code: '.cach_errno()."<br>";
				}
				$_SESSION['COS']=$_POST['COS'];
			} else
			if (isset($_POST['sbCset'])) {
				if ($_POST['stCount']>1000000) {
					$_SESSION['res_cach_cset'] = 'The maximum number of<br> repetitions is one million';
				} else {
					$start = microtime(true);
					for ($AlOuGdHvAbdndUs=0;$AlOuGdHvAbdndUs<$_POST['stCount'];$AlOuGdHvAbdndUs++) {
						cach_set($_POST['stCGlNm'],$_POST['stCInx1'],$_POST['stCInx2'],$_POST['stCVal']);
					}
					$time = microtime(true) - $start;
					$_SESSION['res_cach_cset'] = round($time,2,PHP_ROUND_HALF_UP); 
				}
				$_SESSION['stCount']=$_POST['stCount'];
				$_SESSION['stCGlNm']=$_POST['stCGlNm'];
				$_SESSION['stCInx1']=$_POST['stCInx1'];
				$_SESSION['stCInx2']=$_POST['stCInx2'];
				$_SESSION['stCVal']=$_POST['stCVal'];
			} else 
			if (isset($_POST['sberrno'])) {
				$_SESSION['res_cach_errno'] = cach_errno();
			} else
			if (isset($_POST['sberrno'])) {
				$_SESSION['res_cach_error'] = cach_error();
			} else
			if (isset($_POST['sbquit'])) {
				$_SESSION['res_cach_quit'] = cach_quit();
				echo '<br><meta http-equiv="refresh" content="0; url=auth.php">';
			}
			echo "Cache version: ".$temp."<br>";
			if (cach_exec('set ^CacheN4m3sp4s3("'.$uid_session.'") = $znspace')) {
				if ($temp = cach_get("^CacheN4m3sp4s3",$uid_session)) {
					cach_exec('kill ^CacheN4m3sp4s3("'.$uid_session.'")');
					echo '<form method="POST"> Namespace: <input style="margin-top:15" name="nmsp" type="text" value="'.$temp.'">';
					echo '<input name="sbmNmsp" type="submit" value="Change"><br></form>';
					echo '<form method="POST"><table border=1 width=900><caption>Functions added to PHP</caption>';
					if(isset($_SESSION['error'])) {
						echo '<p style="color:#FF0000">'.$_SESSION['error'].'<p>';
						unset($_SESSION['error']);
					}
					echo '<tr><th>PHP function</th><th>Parameters</th><th>Result</th></tr>';
					if (isset($_POST['sbset'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_set</td><td>Name: <input name="stGlNm" value="'.$_SESSION['stGlNm'].'"><br>Index1: <input name="stInx1" value="'.$_SESSION['stInx1'].'"><br>Index2: <input name="stInx2" value="'.$_SESSION['stInx2'].'"><br>Value: <input name="stVal" value="'.$_SESSION['stVal'].'"><br><input name="sbset" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_set']);
					echo'</td></tr>';
					if (isset($_POST['sbget'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_get</td><td>Name: <input name="gtGlNm" value="'.$_SESSION['gtGlNm'].'"><br>Index1: <input name="gtInx1" value="'.$_SESSION['gtInx1'].'"><br>Index2: <input name="gtInx2" value="'.$_SESSION['gtInx2'].'"><br><input name="sbget" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_get']);
					echo '</td></tr>';
					if (isset($_POST['sbkill'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_kill</td><td>Name: <input name="klGlNm" value="'.$_SESSION['klGlNm'].'"><br>Index1: <input name="klInx1" value="'.$_SESSION['klInx1'].'"><br>Index2: <input name="klInx2" value="'.$_SESSION['klInx2'].'"><br><input name="sbkill" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_kill']);
					echo '</td></tr>';
					if (isset($_POST['sbzkill'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_zkill</td><td>Name: <input name="zklGlNm" value="'.$_SESSION['zklGlNm'].'"><br>Index1: <input name="zklInx1" value="'.$_SESSION['zklInx1'].'"><br>Index2: <input name="zklInx2" value="'.$_SESSION['zklInx2'].'"><br><input name="sbzkill" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_zkill']);
					echo '</td></tr>';
					if (isset($_POST['sbord'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_order</td><td>Name: <input name="ordGlNm" value="'.$_SESSION['ordGlNm'].'"><br>Index1: <input name="ordInx1" value="'.$_SESSION['ordInx1'].'"><br>Index2: <input name="ordInx2" value="'.$_SESSION['ordInx2'].'"><br><input name="sbord" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_ord']);
					echo '</td></tr>';
					if (isset($_POST['sbordv'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_order_rev</td><td>Name: <input name="ordvGlNm" value="'.$_SESSION['ordvGlNm'].'"><br>Index1: <input name="ordvInx1" value="'.$_SESSION['ordvInx1'].'"><br>Index2: <input name="ordvInx2" value="'.$_SESSION['ordvInx2'].'"><br><input name="sbordv" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_ordv']);
					echo '</td></tr>';
					if (isset($_POST['sbqur'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_query</td><td>Name: <input name="qurGlNm" value="'.$_SESSION['qurGlNm'].'"><br>Index1: <input name="qurInx1" value="'.$_SESSION['qurInx1'].'"><br><input name="sbqur" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_qur']);
					echo '</td></tr>';
					if (isset($_POST['sbexec'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_exec</td><td>Cmd: <input name="COS" value="'.$_SESSION['COS'].'"><br><input name="sbexec" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_exec']);
					echo '</td></tr>';
					if (isset($_POST['sberrno'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_erno</td><td><br><input name="sberrno" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_errno']);
					echo '</td></tr>';
					if (isset($_POST['sberror'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>cach_error</td><td><br><input name="sberror" type="submit" value="Execute"></td><td>';
					var_dump($_SESSION['res_cach_error']);
					echo '</td></tr>';
					echo '<td>cach_quit</td><td><br><input name="sbquit" type="submit" value="Execute"></td><td>';
					echo '</td></tr>';
					echo '<tr><th>Test</th><th>Parameters</th><th>Result (sec.)</th></tr>';
					if (isset($_POST['sbCset'])) echo '<tr style="background-color:#6b8e23">'; else echo '<tr>';
					echo '<td>Time of execution <br>cach_set repeat N times</td><td>Number of repetitions: <input name="stCount" value="'.$_SESSION['stCount'].'"><br>Name: <input name="stCGlNm" value="'.$_SESSION['stCGlNm'].'"><br>Index1: <input name="stCInx1" value="'.$_SESSION['stCInx1'].'"><br>Index2: <input name="stCInx2" value="'.$_SESSION['stCInx2'].'"><br>Value: <input name="stCVal" value="'.$_SESSION['stCVal'].'"><br><input name="sbCset" type="submit" value="Execute"></td><td>'.$_SESSION['res_cach_cset'].'</td></tr></table></form>';
				}
			}
		}
	}
	echo '</div>';
?>
