<?php

/*
  +------------------------------------------------------------------------+
  | AllView                                                                |
  +------------------------------------------------------------------------+
  | Copyright (c) 2001-2008 Myleft Studio (http://www.myleftstudio.com)    |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License.                    |
  +------------------------------------------------------------------------+
  | Authors: Myleft Studio <dreamsxin@qq.com>                              |
  |          Harald Holzer <hholzer@may.co.at>                             |
  |          Dirk Datzert <dirk@datzert.de>                                |
  +------------------------------------------------------------------------+
*/
?>
<?php
	$page["title"]="ALLVIEW main page";
	$page["file"]="index.php";

	include "include/config.inc.php";
	include "include/forms.inc.php";

	if(isset($_POST["password"]))
	{
		$password=$_POST["password"];
	}
	else
	{
		unset($password);
	}
	if(isset($_POST["name"]))
	{
		$name=$_POST["name"];
	}
	else
	{
		unset($name);
	}
	if(isset($_POST["register"]))
	{
		$register=$_POST["register"];
	}
	else
	{
		unset($register);
	}
	if(isset($_GET["reconnect"]))
	{
		$reconnect=$_GET["reconnect"];
	}
	else
	{
		unset($reconnect);
	}
	if(isset($_COOKIE["sessionid"]))
	{
		$sessionid=$_COOKIE["sessionid"];
	}
	else
	{
		unset($sessionid);
	}


	if(isset($reconnect))
	{
		$sql="delete from sessions where sessionid='$sessionid'";
		DBexecute($sql);
		setcookie("sessionid",$sessionid,time()-3600);
		unset($sessionid);
	}

	if(isset($register)&&($register=="Enter"))
	{
		$password=md5($password);
		$sql="select u.userid,u.alias,u.name,u.surname,u.url from users u where u.alias='$name' and u.passwd='$password'";
		$result=DBselect($sql);
		if(DBnum_rows($result)==1)
		{
			$USER_DETAILS["userid"]=DBget_field($result,0,0);
			$USER_DETAILS["alias"]=DBget_field($result,0,1);
			$USER_DETAILS["name"]=DBget_field($result,0,2);
			$USER_DETAILS["surname"]=DBget_field($result,0,3);
			$USER_DETAILS["url"]=DBget_field($result,0,4);
			$sessionid=md5(time().$password.$name.rand(0,10000000));
			setcookie("sessionid",$sessionid,time()+3600);
// Required !
			$_COOKIE["sessionid"]=$sessionid;
			$sql="insert into sessions (sessionid,userid,lastaccess) values ('$sessionid',".$USER_DETAILS["userid"].",".time().")";
			DBexecute($sql);

			if($USER_DETAILS["url"] != '')
			{
				echo "<HTML><HEAD>";
        			echo "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=".$USER_DETAILS["url"]."\">";
				echo "</HEAD></HTML>";
				return;
			}
		}
	}

	show_header($page["title"],0,0);
?>

<?php
	if(!isset($sessionid))
	{
//		echo "-",$_COOKIE["sessionid"],"-<br>";
		insert_login_form();
	}
	else
	{
		echo "<div align=center>";
		echo "Press <a href=\"index.php?reconnect=1\">here</a> to disconnect/reconnect";
		echo "</div>";
	}	
?>

<?php
	show_footer();
?>
