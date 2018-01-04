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
// GLOBALS
	$USER_DETAILS	="";
	$ERROR_MSG	="";
// END OF GLOBALS

	include_once 	"include/defines.inc.php";
	include_once 	"include/local_en.inc.php";
	include_once 	"include/db.inc.php";
	include_once 	"include/html.inc.php";

	include_once 	"include/audit.inc.php";
	include_once 	"include/hosts.inc.php";
	include_once 	"include/users.inc.php";
	include_once 	"include/graphs.inc.php";
	include_once 	"include/items.inc.php";
	include_once 	"include/screens.inc.php";
	include_once 	"include/triggers.inc.php";
	include_once 	"include/actions.inc.php";
	include_once 	"include/services.inc.php";
	include_once 	"include/maps.inc.php";

	function getmicrotime()
	{
		list($usec, $sec) = explode(" ",microtime()); 
		return ((float)$usec + (float)$sec); 
	} 

	function	iif($bool,$a,$b)
	{
		if($bool)
		{
			return $a;
		}
		else
		{
			return $b;
		}
	}

	function	iif_echo($bool,$a,$b)
	{
		echo iif($bool,$a,$b);
	}

	function	convert_units($value,$units)
	{
// Special processing for seconds
		if($units=="s")
		{
			$ret="";

			$t=floor($value/(365*24*3600));
			if($t>0)
			{
				$ret=$t."y";
				$value=$value-$t*(365*24*3600);
			}
			$t=floor($value/(30*24*3600));
			if($t>0)
			{
				$ret=$ret.$t."m";
				$value=$value-$t*(30*24*3600);
			}
			$t=floor($value/(24*3600));
			if($t>0)
			{
				$ret=$ret.$t."d";
				$value=$value-$t*(24*3600);
			}
			$t=floor($value/(3600));
			if($t>0)
			{
				$ret=$ret.$t."h";
				$value=$value-$t*(3600);
			}
			$t=floor($value/(60));
			if($t>0)
			{
				$ret=$ret.$t."m";
				$value=$value-$t*(60);
			}
			$ret=$ret.$value."s";
		
			return $ret;	
		}

		$u="";

// Special processing for bits (kilo=1000, not 1024 for bits)
		if( ($units=="b") || ($units=="bps"))
		{
			$abs=abs($value);

			if($abs<1000)
			{
				$u="";
			}
			else if($abs<1000*1000)
			{
				$u="K";
				$value=$value/1000;
			}
			else if($abs<1000*1000*1000)
			{
				$u="M";
				$value=$value/(1000*1000);
			}
			else
			{
				$u="G";
				$value=$value/(1000*1000*1000);
			}
	
			if(round($value)==$value)
			{
				$s=sprintf("%.0f",$value);
			}
			else
			{
				$s=sprintf("%.2f",$value);
			}

			return "$s $u$units";
		}


		if($units=="")
		{
			if(round($value)==$value)
			{
				return sprintf("%.0f",$value);
			}
			else
			{
				return sprintf("%.2f",$value);
			}
		}

		$abs=abs($value);

		if($abs<1024)
		{
			$u="";
		}
		else if($abs<1024*1024)
		{
			$u="K";
			$value=$value/1024;
		}
		else if($abs<1024*1024*1024)
		{
			$u="M";
			$value=$value/(1024*1024);
		}
		else
		{
			$u="G";
			$value=$value/(1024*1024*1024);
		}

		if(round($value)==$value)
		{
			$s=sprintf("%.0f",$value);
		}
		else
		{
			$s=sprintf("%.2f",$value);
		}

		return "$s $u$units";
	}

	function	get_template_permission_str($num)
	{
		$str="&nbsp;";
		if(($num&1)==1)	$str=$str.S_ADD."&nbsp;";
		if(($num&2)==2)	$str=$str.S_UPDATE."&nbsp;";
		if(($num&4)==4)	$str=$str.S_DELETE."&nbsp;";
		return $str;
	}
	
	function	get_media_count_by_userid($userid)
	{
		$sql="select count(*) as cnt from media where userid=$userid";
		$result=DBselect($sql);
		$row=DBfetch($result);
		return $row["cnt"]; 
	}
	
	function	get_action_count_by_triggerid($triggerid)
	{
		$cnt=0;

		$sql="select count(*) as cnt from actions where triggerid=$triggerid and scope=0";
		$result=DBselect($sql);
		$row=DBfetch($result);

		$cnt=$cnt+$row["cnt"];

		$sql="select count(*) as cnt from actions where scope=2";
		$result=DBselect($sql);
		$row=DBfetch($result);

		$cnt=$cnt+$row["cnt"];

		$sql="select distinct h.hostid from hosts h,items i,triggers t,functions f where h.hostid=i.hostid and i.itemid=f.itemid and f.triggerid=t.triggerid and t.triggerid=$triggerid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$sql="select count(*) as cnt from actions a,hosts h,items i,triggers t,functions f where h.hostid=i.hostid and i.itemid=f.itemid and f.triggerid=t.triggerid and a.triggerid=".$row["hostid"]." and a.scope=1";
			$result2=DBselect($sql);
			$row2=DBfetch($result2);
			$cnt=$cnt+$row2["cnt"];
		}

		return $cnt; 
	}

	function	check_anyright($right,$permission)
	{
		global $USER_DETAILS;

		$sql="select permission from rights where name='Default permission' and userid=".$USER_DETAILS["userid"];
		$result=DBselect($sql);

		$default_permission="H";
		if(DBnum_rows($result)>0)
		{
			$default_permission="";
			while($row=DBfetch($result))
			{
				$default_permission=$default_permission.$row["permission"];
			}
		}
# default_permission

		$sql="select permission from rights where name='$right' and id!=0 and userid=".$USER_DETAILS["userid"];
		$result=DBselect($sql);

		$all_permissions="";
		if(DBnum_rows($result)>0)
		{
			while($row=DBfetch($result))
			{
				$all_permissions=$all_permissions.$row["permission"];
			}
		}
# all_permissions

//		echo "$all_permissions|$default_permission<br>";

		switch ($permission) {
			case 'A':
				if(strstr($all_permissions,"A"))
				{
					return 1;
				}
				if(strstr($default_permission,"H"))
				{
					return 0;
				}
				else if(strstr($default_permission,"A"))
				{
					return 1;
				}
				break;
			case 'R':
				if(strstr($all_permissions,"R"))
				{
					return 1;
				}
				else if(strstr($all_permissions,"U"))
				{
					return 1;
				}
				if(strstr($default_permission,"H"))
				{
					return 0;
				}
				else if(strstr($default_permission,"R"))
				{
					return 1;
				}
				else if(strstr($default_permission,"U"))
				{
					return 1;
				}
				break;
			case 'U':
				if(strstr($all_permissions,"U"))
				{
					return 1;
				}
				if(strstr($default_permission,"H"))
				{
					return 0;
				}
				else if(strstr($default_permission,"U"))
				{
					return 1;
				}
				break;
			default:
				return 0;
		}
		return 0;
	}

	function	check_right($right,$permission,$id)
	{
		global $USER_DETAILS;

		$sql="select permission from rights where name='Default permission' and userid=".$USER_DETAILS["userid"];
		$result=DBselect($sql);

		$default_permission="H";
		if(DBnum_rows($result)>0)
		{
			$default_permission="";
			while($row=DBfetch($result))
			{
				$default_permission=$default_permission.$row["permission"];
			}
		}
# default_permission

		$sql="select permission from rights where name='$right' and id=0 and userid=".$USER_DETAILS["userid"];
		$result=DBselect($sql);

		$group_permission="";
		if(DBnum_rows($result)>0)
		{
			while($row=DBfetch($result))
			{
				$group_permission=$group_permission.$row["permission"];
			}
		}
# group_permission

		$id_permission="";
		if($id!=0)
		{
			$sql="select permission from rights where name='$right' and id=$id and userid=".$USER_DETAILS["userid"];
			$result=DBselect($sql);
			if(DBnum_rows($result)>0)
			{
				while($row=DBfetch($result))
				{
					$id_permission=$id_permission.$row["permission"];
				}
			}
		}
# id_permission
//		echo "$id_permission|$group_permission|$default_permission<br>";

		switch ($permission) {
			case 'A':
				if(strstr($id_permission,"H"))
				{
					return 0;
				}
				else if(strstr($id_permission,"A"))
				{
					return 1;
				}
				if(strstr($group_permission,"H"))
				{
					return 0;
				}
				else if(strstr($group_permission,"A"))
				{
					return 1;
				}
				if(strstr($default_permission,"H"))
				{
					return 0;
				}
				else if(strstr($default_permission,"A"))
				{
					return 1;
				}
				break;
			case 'R':
				if(strstr($id_permission,"H"))
				{
					return 0;
				}
				else if(strstr($id_permission,"R"))
				{
					return 1;
				}
				else if(strstr($id_permission,"U"))
				{
					return 1;
				}
				if(strstr($group_permission,"H"))
				{
					return 0;
				}
				else if(strstr($group_permission,"R"))
				{
					return 1;
				}
				else if(strstr($group_permission,"U"))
				{
					return 1;
				}
				if(strstr($default_permission,"H"))
				{
					return 0;
				}
				else if(strstr($default_permission,"R"))
				{
					return 1;
				}
				else if(strstr($default_permission,"U"))
				{
					return 1;
				}
				break;
			case 'U':
				if(strstr($id_permission,"H"))
				{
					return 0;
				}
				else if(strstr($id_permission,"U"))
				{
					return 1;
				}
				if(strstr($group_permission,"H"))
				{
					return 0;
				}
				else if(strstr($group_permission,"U"))
				{
					return 1;
				}
				if(strstr($default_permission,"H"))
				{
					return 0;
				}
				else if(strstr($default_permission,"U"))
				{
					return 1;
				}
				break;
			default:
				return 0;
		}
		return 0;
	}


/*	function	check_right($right,$permission,$id)
	{
		global $USER_DETAILS;

		if($id!=0)
		{
			$sql="select * from rights where name='$right' and permission in ('H') and id=$id and userid=".$USER_DETAILS["userid"];
			$result=DBselect($sql);
			if(DBnum_rows($result)>0)
			{
				return	0;
			}
		}

		$sql="select permission from rights where name='Default permission' and userid=".$USER_DETAILS["userid"];
		$result=DBselect($sql);

		$default_permission="H";
		if(DBnum_rows($result)>0)
		{
			$default_permission="";
			while($row=DBfetch($result))
			{
				$default_permission=$default_permission.$row["permission"];
			}
		}

		if($permission=='R')
		{
			$cond="'R','U'";
		}
		else
		{
			$cond="'".$permission."'";
		}

		$sql="select * from rights where name='$right' and permission in ($cond) and (id=$id or id=0) and userid=".$USER_DETAILS["userid"];
//		echo $sql;

		$result=DBselect($sql);

		if(DBnum_rows($result)>0)
		{
			return	1;
		}
		else
		{
			if(strstr($default_permission,"A")&&($permission=="A"))
			{
				return 1;
			}
			if(strstr($default_permission,"R")&&($permission=="R"))
			{
				return 1;
			}
			if(strstr($default_permission,"U")&&($permission=="R"))
			{
				return 1;
			}
			if(strstr($default_permission,"U")&&($permission=="U"))
			{
				return 1;
			}
			return	0;
		}
	}
*/

	function	get_scope_description($scope)
	{
		$desc="Unknown";
		if($scope==2)
		{
			$desc="All";
		}
		elseif($scope==1)
		{
			$desc="Host";
		}
		elseif($scope==0)
		{
			$desc="Trigger";
		}
		return $desc;
	}

//	The hash has form <md5sum of triggerid>,<sum of priorities>
	function	calc_trigger_hash()
	{
		$priorities=0;
		for($i=0;$i<=5;$i++)
		{
	        	$result=DBselect("select count(*) from triggers t,hosts h,items i,functions f  where t.value=1 and f.itemid=i.itemid and h.hostid=i.hostid and t.triggerid=f.triggerid and i.status=0 and t.priority=$i");
//			$priorities+=(1000^$i)*DBget_field($result,0,0);
			$priorities+=pow(100,$i)*DBget_field($result,0,0);
//			echo "$i $priorities ",DBget_field($result,0,0),"<br>";
//			echo pow(100,5)*13;
		}
		$triggerids="";
	       	$result=DBselect("select t.triggerid from triggers t,hosts h,items i,functions f  where t.value=1 and f.itemid=i.itemid and h.hostid=i.hostid and t.triggerid=f.triggerid and i.status=0");
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$triggerids="$triggerids,".DBget_field($result,$i,0);
		}
		$md5sum=md5($triggerids);

		return	"$priorities,$md5sum";
	}

	function	get_group_by_groupid($groupid)
	{
		global	$ERROR_MSG;

		$sql="select * from groups where groupid=$groupid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No groups with groupid=[$groupid]";
		}
		return	$result;
	}

	function	get_action_by_actionid($actionid)
	{
		global	$ERROR_MSG;

		$sql="select * from actions where actionid=$actionid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No action with actionid=[$actionid]";
		}
		return	$result;
	}

	function	get_usergroup_by_usrgrpid($usrgrpid)
	{
		global	$ERROR_MSG;

		$result=DBselect("select usrgrpid,name from usrgrp where usrgrpid=$usrgrpid"); 
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No user group with usrgrpid=[$usrgrpid]";
		}
		return	$result;
	}

	function	get_image_by_name($imagetype,$name)
	{
		$sql="select * from images where imagetype=$imagetype and name='$name'"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			return 0;
		}
	}

	function	get_function_by_functionid($functionid)
	{
		global	$ERROR_MSG;

		$sql="select * from functions where functionid=$functionid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);
		}
		else
		{
			$ERROR_MSG="No function with functionid=[$functionid]";
		}
		return	$item;
	}

	function	select_config()
	{
		global	$ERROR_MSG;

//		$sql="select smtp_server,smtp_helo,smtp_email,alarm_history,alert_history from config";
		$sql="select alarm_history,alert_history from config";
		$result=DBselect($sql);

		if(DBnum_rows($result) == 1)
		{
			$config["alarm_history"]=DBget_field($result,0,0);
			$config["alert_history"]=DBget_field($result,0,1);
//			$config["smtp_server"]=DBget_field($result,0,0);
//			$config["smtp_helo"]=DBget_field($result,0,1);
//			$config["smtp_email"]=DBget_field($result,0,2);
//			$config["alarm_history"]=DBget_field($result,0,3);
//			$config["alert_history"]=DBget_field($result,0,4);
		}
		else
		{
			$ERROR_MSG="Unable to select configuration";
		}
		return	$config;
	}

	function	show_messages($bool,$msg,$errmsg)
	{
		global	$ERROR_MSG;

		if(!$bool)
		{
			$msg="ERROR:".$errmsg;
			$color="#AA0000";
		}
		else
		{
			$color="#223344";
		}
		echo "<p align=center>";
//		echo "<font size=+1 color='$color'>";
		echo "<font color='$color'>";
		if($ERROR_MSG=="")
		{
			echo "<b>[$msg]</b>";
		}
		else
		{
			echo "<b>[$msg. $ERROR_MSG]</b>";
		}
		echo "</font>";
		echo "</p>";
	}

	function	show_message($msg)
	{
		show_messages(TRUE,$msg,'');
	}

	function	show_error_message($msg)
	{
		show_messages(FALSE,'',$msg);
	}

	function	validate_float($str)
	{
//		echo "Validating float:$str<br>";
		if (eregi('^([0-9]+)((\.)?)([0-9]*[KMG]{0,1})$', $str, &$arr)) 
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

// Does expression match server:key.function(param) ?
	function	validate_simple_expression($expression)
	{
		global	$ERROR_MSG;

//		echo "Validating simple:$expression<br>";
// Before str()
// 		if (eregi('^\{([0-9a-zA-Z[.-.]\_\.]+)\:([]\[0-9a-zA-Z\_\/\.\,]+)\.((diff)|(min)|(max)|(last)|(prev))\(([0-9\.]+)\)\}$', $expression, &$arr)) 
//		if (eregi('^\{([0-9a-zA-Z[.-.]\_\.]+)\:([]\[0-9a-zA-Z\_\/\.\,]+)\.((diff)|(min)|(max)|(last)|(prev)|(str))\(([0-9a-zA-Z\.\_\/\,]+)\)\}$', $expression, &$arr)) 
 		if (eregi('^\{([0-9a-zA-Z\_\.-]+)\:([]\[0-9a-zA-Z\_\/\.\,\:\(\) -]+)\.([a-z]{3,9})\(([0-9a-zA-Z\_\/\.\,]+)\)\}$', $expression, &$arr)) 
		{
			$host=$arr[1];
			$key=$arr[2];
			$function=$arr[3];
			$parameter=$arr[4];

//			echo $host,"<br>";
//			echo $key,"<br>";
//			echo $function,"<br>";
//			echo $parameter,"<br>";

			$sql="select count(*) from hosts h,items i where h.host='$host' and i.key_='$key' and h.hostid=i.hostid";
			$result=DBselect($sql);
			if(DBget_field($result,0,0)!=1)
			{
				$ERROR_MSG="No such host ($host) or monitored parameter ($key)";
				return -1;
			}

			if(	($function!="last")&&
				($function!="diff")&&
				($function!="min") &&
				($function!="max") &&
				($function!="avg") &&
				($function!="sum") &&
				($function!="count") &&
				($function!="prev")&&
				($function!="delta")&&
				($function!="change")&&
				($function!="abschange")&&
				($function!="nodata")&&
				($function!="time")&&
				($function!="dayofweek")&&
				($function!="date")&&
				($function!="now")&&
				($function!="str"))
			{
				$ERROR_MSG="Unknown function [$function]";
				return -1;
			}


			if(( $function!="str") && (validate_float($parameter)!=0) )
			{
				$ERROR_MSG="[$parameter] is not a float";
				return -1;
			}
		}
		else
		{
			$ERROR_MSG="Expression [$expression] does not match to [server:key.func(param)]";
			return -1;
		}
		return 0;
	}

	function	validate_expression($expression)
	{
		global	$ERROR_MSG;

//		echo "Validating expression: $expression<br>";

		$ok=0;
// Replace all {server:key.function(param)} with 0
		while($ok==0)
		{
//			echo "Expression:$expression<br>";
			$arr="";
			if (eregi('^((.)*)(\{((.)*)\})((.)*)$', $expression, &$arr)) 
			{
//				for($i=0;$i<20;$i++)
//				{
//					if($arr[$i])
//						echo "  $i: ",$arr[$i],"<br>";
//				}
				if(validate_simple_expression($arr[3])!=0)
				{
					return -1;
				}
				$expression=$arr[1]."0".$arr[6];
	                }
			else
			{
				$ok=1;
			}
		}
//		echo "Result:$expression<br><hr>";

		$ok=0;
		while($ok==0)
		{
// 	Replace all <float> <sign> <float> <K|M|G> with 0
//			echo "Expression:$expression<br>";
			$arr="";
			if (eregi('^((.)*)([0-9\.]+[A-Z]{0,1})([\&\|\>\<\=\+\-\*\/\#]{1})([0-9\.]+[A-Z]{0,1})((.)*)$', $expression, &$arr)) 
			{
//				echo "OK<br>";
//				for($i=0;$i<50;$i++)
//				{
//					if($arr[$i]!="")
//						echo "  $i: ",$arr[$i],"<br>";
//				}
				if(validate_float($arr[3])!=0)
				{
					$ERROR_MSG="[".$arr[3]."] is not a float";
					return -1;
				}
				if(validate_float($arr[5])!=0)
				{
					$ERROR_MSG="[".$arr[5]."] is not a float";
					return -1;
				}
				$expression=$arr[1]."(0)".$arr[6];
	                }
			else
			{
				$ok=1;
			}


// 	Replace all (float) with 0
//			echo "Expression2:[$expression]<br>";
			$arr="";
			if (eregi('^((.)*)(\(([0-9\.]+)\))((.)*)$', $expression, &$arr)) 
			{
//				echo "OK<br>";
//				for($i=0;$i<30;$i++)
//				{
//					if($arr[$i]!="")
//						echo "  $i: ",$arr[$i],"<br>";
//				}
				if(validate_float($arr[4])!=0)
				{
					$ERROR_MSG="[".$arr[4]."] is not a float";
					return -1;
				}
				$expression=$arr[1]."0".$arr[5];
				$ok=0;
	                }
			else
			{
				$ok=1;
			}



		}
//		echo "Result:$expression<br><hr>";

		if($expression=="0")
		{
			return 0;
		}

		return 1;
	}

	function	cr()
	{
		echo "\n";
	}

	function	check_authorisation()
	{
		global	$page;
		global	$PHP_AUTH_USER,$PHP_AUTH_PW;
		global	$USER_DETAILS;
		global	$_COOKIE;
		global	$_GET;
//		global	$sessionid;

		if(isset($_COOKIE["sessionid"]))
		{
			$sessionid=$_COOKIE["sessionid"];
		}
		else
		{
			unset($sessionid);
		}

		if(isset($sessionid))
		{
			$sql="select u.userid,u.alias,u.name,u.surname from sessions s,users u where s.sessionid='$sessionid' and s.userid=u.userid and s.lastaccess+900>".time();
			$result=DBselect($sql);
			if(DBnum_rows($result)==1)
			{
//				setcookie("sessionid",$sessionid,time()+3600);
				setcookie("sessionid",$sessionid);
				$sql="update sessions set lastaccess=".time()." where sessionid='$sessionid'";
				DBexecute($sql);
				$USER_DETAILS["userid"]=DBget_field($result,0,0);
				$USER_DETAILS["alias"]=DBget_field($result,0,1);
				$USER_DETAILS["name"]=DBget_field($result,0,2);
				$USER_DETAILS["surname"]=DBget_field($result,0,3);
				return;
			}
			else
			{
				setcookie("sessionid",$sessionid,time()-3600);
				unset($sessionid);
			}
		}

                $sql="select u.userid,u.alias,u.name,u.surname from users u where u.alias='guest'";
                $result=DBselect($sql);
                if(DBnum_rows($result)==1)
                {
                        $USER_DETAILS["userid"]=DBget_field($result,0,0);
                        $USER_DETAILS["alias"]=DBget_field($result,0,1);
                        $USER_DETAILS["name"]=DBget_field($result,0,2);
                        $USER_DETAILS["surname"]=DBget_field($result,0,3);
			return;
		}

		if($page["file"]!="index.php")
		{
			echo "<meta http-equiv=\"refresh\" content=\"0; url=index.php\">";
		}
		show_special_header("Login",0,1,1);
		show_error_message("Login name or password is incorrect");
		insert_login_form();
		show_footer();
		exit;
	}

	# Header for HTML pages

	function	show_header($title,$refresh,$nomenu)
	{
		show_special_header($title,$refresh,$nomenu,0);
	}


	function	show_special_header($title,$refresh,$nomenu,$noauth)
	{
		global $page;
		global $USER_DETAILS;

		if($noauth!=1)
		{
			check_authorisation();
		}

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta name="Author" content="Allview team ">
<link rel="stylesheet" href="css.css">

<?php
	if($USER_DETAILS['alias']=='guest')
	{
		$refresh=2*$refresh;
	}
	if($refresh!=0)
	{
		echo "<meta http-equiv=\"refresh\" content=\"$refresh\">\n";
		echo "<title>$title [refreshed every $refresh sec]</title>\n";
	}
	else
	{
		echo "<title>$title</title>\n";
	}

echo "</head>";
?>


<body>
<?php
		if($nomenu == 0)
		{
?>

<?php
	$menu=array(
		"view"=>array(
				"label"=>"View",
				"pages"=>array("overview.php","latest.php","tr_status.php","queue.php","latestalarms.php","alerts.php","maps.php","charts.php","screens.php","srv_status.php","alarms.php","history.php","tr_comments.php","report3.php"),
				"level2"=>array(
					array("label"=>"Overview","url"=>"overview.php"),
					array("label"=>"Latest data","url"=>"latest.php"),
					array("label"=>"Triggers","url"=>"tr_status.php?onlytrue=true&amp;noactions=true&amp;compact=true"),
					array("label"=>"Queue","url"=>"queue.php"),
					array("label"=>"Events","url"=>"latestalarms.php"),
					array("label"=>"Actions","url"=>"alerts.php"),
					array("label"=>"Maps","url"=>"maps.php"),
					array("label"=>"Graphs","url"=>"charts.php"),
					array("label"=>"Screens","url"=>"screens.php"),
					array("label"=>"IT Services","url"=>"srv_status.php")
					)
				),
		"reports"=>array(
				"label"=>"Reports",
				"pages"=>array("report1.php","report2.php"),
				"level2"=>array(
					array("label"=>"Status of ALLVIEW","url"=>"report1.php"),
					array("label"=>"Availability report","url"=>"report2.php")
					)
				),
		"configuration"=>array(
				"label"=>"Configuration",
				"pages"=>array("config.php","users.php","audit.php","hosts.php","items.php","triggers.php","sysmaps.php","graphs.php","screenconf.php","services.php","sysmap.php","media.php","screenedit.php","actions.php","graph.php"),
				"level2"=>array(
					array("label"=>"General","url"=>"config.php"),
					array("label"=>"Users","url"=>"users.php"),
					array("label"=>"Audit","url"=>"audit.php"),
					array("label"=>"Hosts","url"=>"hosts.php"),
					array("label"=>"Items","url"=>"items.php"),
					array("label"=>"Triggers","url"=>"triggers.php"),
					array("label"=>"Maps","url"=>"sysmaps.php"),
					array("label"=>"Graphs","url"=>"graphs.php"),
					array("label"=>"Screens","url"=>"screenconf.php"),
					array("label"=>"IT Services","url"=>"services.php")
					)
				),
		"login"=>array(
				"label"=>"Login",
				"pages"=>array("index.php"),
				"level2"=>array(
					array("label"=>"Login","url"=>"index.php"),
					)
				),
		);
?>

<table border=0 cellspacing=0 cellpadding=5 width="100%" bgcolor="#FFFFFF">
<tr>
<td width="118" height="31" class="top_header_left"><img width="118" height="31" src="images/general/allview.png" border="0" alt="ALLVIEW"></td>
<td width="662" class="top_header_right"><a href="http://www.myleftstudio.com/manual/v1.1/index.php" class="small_font">|Help|</a></td>
</tr>
</table>

<table class="menu" cellspacing=0 cellpadding=5>
<tr>
<?php
	$i=0;
	foreach($menu as $label=>$sub)
	{
// Check permissions
		if($label=="configuration")
		{
			if(	!check_anyright("Configuration of Allview","U")
				&&!check_anyright("User","U")
				&&!check_anyright("Host","U")
				&&!check_anyright("Graph","U")
				&&!check_anyright("Screen","U")
				&&!check_anyright("Network map","U")
				&&!check_anyright("Service","U")
			)
			{
				continue;
			}
			if(	!check_anyright("Default permission","R")
				&&!check_anyright("Host","R")
			)
			{
				continue;
			}

		}
// End of check permissions
		$active=0;
		foreach($sub["pages"] as $label2)
		{
			if($page["file"]==$label2)
			{
				$active=1;
				$active_level1=$label;
			}
		}
		if($active==1)
			echo "<td class=\"horizontal_menu\" height=24 colspan=9><b><a href=\"".$sub["level2"][0]["url"]."\" class=\"highlight\">".$sub["label"]."</a></b></td>";
		else
			echo "<td class=\"horizontal_menu_n\" height=24 colspan=9><b><a href=\"".$sub["level2"][0]["url"]."\" class=\"highlight\">".$sub["label"]."</a></b></td>";
		$i++;
	}
?>
</tr>
</table>
<table class="menu" width="100%" cellspacing=0 cellpadding=5>
<tr><td class="horizontal_menu" height=24 colspan=9><b>
<?php
	$i=0;
	if(isset($active_level1))
	foreach($menu[$active_level1]["level2"] as $label=>$sub)
	{
// Check permissions
		if(($sub["url"]=="latest.php")&&!check_anyright("Host","R"))							continue;
		if(($sub["url"]=="overview.php")&&!check_anyright("Host","R"))							continue;
		if(($sub["url"]=="tr_status.php?onlytrue=true&noactions=true&compact=true")&&!check_anyright("Host","R"))	continue;
		if(($sub["url"]=="queue.php")&&!check_anyright("Host","R"))							continue;
		if(($sub["url"]=="latestalarms.php")&&!check_anyright("Default permission","R"))				continue;
		if(($sub["url"]=="alerts.php")&&!check_anyright("Default permission","R"))					continue;
		if(($sub["url"]=="maps.php")&&!check_anyright("Network map","R"))						continue;
		if(($sub["url"]=="charts.php")&&!check_anyright("Graph","R"))							continue;
		if(($sub["url"]=="screens.php")&&!check_anyright("Screen","R"))							continue;
		if(($sub["url"]=="srv_status.php")&&!check_anyright("Service","R"))						continue;
		if(($sub["url"]=="about.php")&&!check_anyright("Default permission","R"))					continue;
		if(($sub["url"]=="report1.php")&&!check_anyright("Default permission","R"))					continue;
		if(($sub["url"]=="report2.php")&&!check_anyright("Host","R"))							continue;
		if(($sub["url"]=="config.php")&&!check_anyright("Configuration of Allview","U"))					continue;
		if(($sub["url"]=="users.php")&&!check_anyright("User","U"))							continue;
		if(($sub["url"]=="media.php")&&!check_anyright("User","U"))							continue;
		if(($sub["url"]=="audit.php")&&!check_anyright("Audit","U"))							continue;
		if(($sub["url"]=="hosts.php")&&!check_anyright("Host","U"))							continue;
		if(($sub["url"]=="items.php")&&!check_anyright("Host","U"))							continue;
		if(($sub["url"]=="triggers.php")&&!check_anyright("Host","U"))							continue;
		if(($sub["url"]=="actions.php")&&!check_anyright("Host","U"))							continue;
		if(($sub["url"]=="sysmaps.php")&&!check_anyright("Network map","U"))						continue;
		if(($sub["url"]=="sysmap.php")&&!check_anyright("Network map","U"))						continue;
		if(($sub["url"]=="graphs.php")&&!check_anyright("Graph","U"))							continue;
		if(($sub["url"]=="graph.php")&&!check_anyright("Graph","U"))							continue;
		if(($sub["url"]=="screenedit.php")&&!check_anyright("Screen","U"))						continue;
		if(($sub["url"]=="screenconf.php")&&!check_anyright("Screen","U"))						continue;
		if(($sub["url"]=="services.php")&&!check_anyright("Service","U"))						continue;
// End of check permissions
		if($i==0)
			echo "<a href=\"".$sub["url"]."\" class=\"highlight\">".$sub["label"]."</a><span class=\"divider\">&nbsp;&nbsp;|&nbsp;&nbsp;</span>";
		else
			echo "<a href=\"".$sub["url"]."\" class=\"highlight\">".$sub["label"]."</a><span class=\"divider\">&nbsp;&nbsp;|&nbsp;&nbsp;</span>";
		$i++;
	}
?>
</b></td>
</table>
<p align=center>


<?php
		}
	}

	# Show screen cell containing plain text values
	function	show_screen_plaintext($itemid)
	{
		$item=get_item_by_itemid($itemid);
		if($item["value_type"]==0)
		{
			$sql="select clock,value from history where itemid=$itemid order by clock desc limit 25";
		}
		else
		{
			$sql="select clock,value from history_str where itemid=$itemid order by clock desc limit 25";
		}
                $result=DBselect($sql);

		table_begin();
		table_header(array(S_CLOCK,$item["description"]));
		$col=0;
		while($row=DBfetch($result))
		{
			table_row(array(
				date(S_DATE_FORMAT_YMDHMS,$row["clock"]),
				$row["value"],
				),$col++);
		}
		table_end();
	}

	# Show values in plain text

	function	show_plaintext($itemid, $from, $till)
	{
		$item=get_item_by_itemid($itemid);
		if($item["value_type"]==0)
		{
			$sql="select clock,value from history where itemid=$itemid and clock>$from and clock<$till order by clock";
		}
		else
		{
			$sql="select clock,value from history_str where itemid=$itemid and clock>$from and clock<$till order by clock";
		}
                $result=DBselect($sql);

		echo "<PRE>\n";
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$clock=DBget_field($result,$i,0);
			$value=DBget_field($result,$i,1);
			echo date("Y-m-d H:i:s",$clock);
			echo "\t$clock\t$value\n";
		}
	}
 

	# Translate {10}>10 to something like localhost:procload.last(0)>10

	function	explode_exp ($expression, $html)
	{
#		echo "EXPRESSION:",$expression,"<Br>";

		$functionid='';
		$exp='';
		$state='';
		for($i=0;$i<strlen($expression);$i++)
		{
			if($expression[$i] == '{')
			{
				$functionid='';
				$state='FUNCTIONID';
				continue;
			}
			if($expression[$i] == '}')
			{
				$state='';
				$sql="select h.host,i.key_,f.function,f.parameter,i.itemid from items i,functions f,hosts h where functionid=$functionid and i.itemid=f.itemid and h.hostid=i.hostid";
				$res1=DBselect($sql);
				if($html == 0)
				{
					$exp=$exp."{".DBget_field($res1,0,0).":".DBget_field($res1,0,1).".".DBget_field($res1,0,2)."(".DBget_field($res1,0,3).")}";
				}
				else
				{
					$item=get_item_by_itemid(DBget_field($res1,0,4));
					if($item["value_type"] ==0) 
					{
						$exp=$exp."{<A HREF=\"history.php?action=showhistory&itemid=".DBget_field($res1,0,4)."\">".DBget_field($res1,0,0).":".DBget_field($res1,0,1)."</A>.<B>".DBget_field($res1,0,2)."(</B>".DBget_field($res1,0,3)."<B>)</B>}";
					}
					else
					{
						$exp=$exp."{<A HREF=\"history.php?action=showvalues&period=3600&itemid=".DBget_field($res1,0,4)."\">".DBget_field($res1,0,0).":".DBget_field($res1,0,1)."</A>.<B>".DBget_field($res1,0,2)."(</B>".DBget_field($res1,0,3)."<B>)</B>}";
					}
				}
				continue;
			}
			if($state == "FUNCTIONID")
			{
				$functionid=$functionid.$expression[$i];
				continue;
			}
			$exp=$exp.$expression[$i];
		}
#		echo "EXP:",$exp,"<Br>";
		return $exp;
	}

	# Translate localhost:procload.last(0)>10 to {12}>10

	function	implode_exp ($expression, $triggerid)
	{
//		echo "Expression:$expression<br>";
		$exp='';
		$state="";
		for($i=0;$i<strlen($expression);$i++)
		{
			if($expression[$i] == '{')
			{
				if($state=="")
				{
					$host='';
					$key='';
					$function='';
					$parameter='';
					$state='HOST';
					continue;
				}
			}
			if( ($expression[$i] == '}')&&($state=="") )
			{
//				echo "HOST:$host<BR>";
//				echo "KEY:$key<BR>";
//				echo "FUNCTION:$function<BR>";
//				echo "PARAMETER:$parameter<BR>";
				$state='';
		
				$sql="select i.itemid from items i,hosts h where i.key_='$key' and h.host='$host' and h.hostid=i.hostid";
#				echo $sql,"<Br>";
				$res=DBselect($sql);

				$itemid=DBget_field($res,0,0);
#				echo "ITEMID:$itemid<BR>";
	
#				$sql="select functionid,count(*) from functions where function='$function' and parameter=$parameter group by 1";
#				echo $sql,"<Br>";
#				$res=DBselect($sql);
#
#				if(DBget_field($res,0,1)>0)
#				{
#					$functionid=DBget_field($res,0,0);
#				}
#				else
#				{
					$sql="insert into functions (itemid,triggerid,function,parameter) values ($itemid,$triggerid,'$function','$parameter')";
#					echo $sql,"<Br>";
					$res=DBexecute($sql);
					if(!$res)
					{
#						echo "ERROR<br>";
						return	$res;
					}
					$functionid=DBinsert_id($res,"functions","functionid");
#				}
#				echo "FUNCTIONID:$functionid<BR>";

				$exp=$exp.'{'.$functionid.'}';

				continue;
			}
			if($expression[$i] == '(')
			{
				if($state == "FUNCTION")
				{
					$state='PARAMETER';
					continue;
				}
			}
			if($expression[$i] == ')')
			{
				if($state == "PARAMETER")
				{
					$state='';
					continue;
				}
			}
			if(($expression[$i] == ':') && ($state == "HOST"))
			{
				$state="KEY";
				continue;
			}
			if($expression[$i] == '.')
			{
				if($state == "KEY")
				{
					$state="FUNCTION";
					continue;
				}
				// Support for '.' in KEY
				if($state == "FUNCTION")
				{
					$state="FUNCTION";
					$key=$key.".".$function;
					$function="";
					continue;
				}
			}
			if($state == "HOST")
			{
				$host=$host.$expression[$i];
				continue;
			}
			if($state == "KEY")
			{
				$key=$key.$expression[$i];
				continue;
			}
			if($state == "FUNCTION")
			{
				$function=$function.$expression[$i];
				continue;
			}
			if($state == "PARAMETER")
			{
				$parameter=$parameter.$expression[$i];
				continue;
			}
			$exp=$exp.$expression[$i];
		}
		return $exp;
	}

	# Update Host status

	function	update_host_status($hostid,$status)
	{
                global  $ERROR_MSG;
                if(!check_right("Host","U",0))
                {
                        $ERROR_MSG="Insufficient permissions";
                        return 0;
                }

		$sql="select status from hosts where hostid=$hostid";
		$result=DBselect($sql);
		$old_status=DBget_field($result,0,0);
		if($status != $old_status)
		{
			update_trigger_value_to_unknown_by_hostid($hostid);
			$sql="update hosts set status=$status where hostid=$hostid and status!=".HOST_STATUS_DELETED;
			return	DBexecute($sql);
		}
		else
		{
			return 1;
		}
	}

	function	add_image($name,$imagetype,$files)
	{
		global	$ERROR_MSG;

		if(isset($files))
		{
			if($files["image"]["error"]==0)
			if($files["image"]["size"]<1024*1024)
			{
				$image=addslashes(fread(fopen($files["image"]["tmp_name"],"r"),filesize($files["image"]["tmp_name"])));
				$sql="insert into images (name,imagetype,image) values ('$name',$imagetype,'$image')";
				return	DBexecute($sql);
			}
			else
			{
				$ERROR_MSG="Image size must be less than 1Mb";
				return FALSE;
			}
		}
		else
		{
			$ERROR_MSG="Select image to download";
			return FALSE;
		}
	}

	function	delete_image($imageid)
	{
		$sql="delete from images where imageid=$imageid";
		return	DBexecute($sql);
	}

	# Delete Alert by actionid

	function	delete_alert_by_actionid( $actionid )
	{
		$sql="delete from alerts where actionid=$actionid";
		return	DBexecute($sql);
	}

	function	delete_rights_by_userid($userid )
	{
		$sql="delete from rights where userid=$userid";
		return	DBexecute($sql);
	}

	# Delete from History

	function	delete_history_by_itemid( $itemid )
	{
		$sql="delete from history_str where itemid=$itemid";
		DBexecute($sql);
		$sql="delete from history where itemid=$itemid";
		return	DBexecute($sql);
	}

	# Delete from Trends

	function	delete_trends_by_itemid( $itemid )
	{
		$sql="delete from trends where itemid=$itemid";
		return	DBexecute($sql);
	}

	# Add alarm

	function	add_alarm($triggerid,$value)
	{
		$sql="select max(clock) from alarms where triggerid=$triggerid";
		$result=DBselect($sql);
		$row=DBfetch($result);
		if($row[0]!="")
		{
			$sql="select value from alarms where triggerid=$triggerid and clock=".$row[0];
			$result=DBselect($sql);
			if(DBnum_rows($result) == 1)
			{
				$row=DBfetch($result);
				if($row["value"] == $value)
				{
					return 0;
				}
			}
		}

		$now=time();
		$sql="insert into alarms(triggerid,clock,value) values($triggerid,$now,$value)";
		return	DBexecute($sql);
	}

	# Reset nextcheck for related items

	function	reset_items_nextcheck($triggerid)
	{
		$sql="select itemid from functions where triggerid=$triggerid";
		$result=DBselect($sql);
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$itemid=DBget_field($result,$i,0);
			$sql="update items set nextcheck=0 where itemid=$itemid";
			DBexecute($sql);
		}
	}

	# Add host-template linkage
	function add_template_linkage($hostid,$templateid,$items,$triggers,$actions,$graphs,$screens)
	{
		$sql="insert into hosts_templates (hostid,templateid,items,triggers,actions,graphs,screens) values ($hostid,$templateid,$items,$triggers,$actions,$graphs,$screens)";
		return	DBexecute($sql);
	}

	# Update host-template linkage
	function update_template_linkage($hosttemplateid,$hostid,$templateid,$items,$triggers,$actions,$graphs,$screens)
	{
		$sql="update hosts_templates set hostid=$hostid,templateid=$templateid,items=$items,triggers=$triggers,actions=$actions,graphs=$graphs,screens=$screens where hosttemplateid=$hosttemplateid";
		return	DBexecute($sql);
	}

	# Delete host-template linkage
	function delete_template_linkage($hosttemplateid)
	{
		$sql="delete from hosts_templates where hosttemplateid=$hosttemplateid";
		return	DBexecute($sql);
	}

	# Add everything based on host_templateid

	function	add_using_host_template($hostid,$host_templateid)
	{
		global	$ERROR_MSG;

		if(!isset($host_templateid)||($host_templateid==0))
		{
			$ERROR_MSG="Select template first";
			return 0;
		}

		$host=get_host_by_hostid($hostid);
		$sql="select itemid from items where hostid=$host_templateid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$item=get_item_by_itemid($row["itemid"]);
			$itemid=add_item($item["description"],$item["key_"],$hostid,$item["delay"],$item["history"],$item["status"],$item["type"],$item["snmp_community"],$item["snmp_oid"],$item["value_type"],"",$item["snmp_port"],$item["units"],$item["multiplier"],$item["delta"],$item["snmpv3_securityname"],$item["snmpv3_securitylevel"],$item["snmpv3_authpassphrase"],$item["snmpv3_privpassphrase"],$item["formula"],$item["trends"]);

			$sql="select distinct t.triggerid from triggers t,functions f where f.itemid=".$row["itemid"]." and f.triggerid=t.triggerid";
			$result2=DBselect($sql);
			while($row2=DBfetch($result2))
			{
				$trigger=get_trigger_by_triggerid($row2["triggerid"]);
// Cannot use add_trigger here
				$description=$trigger["description"];
#				$description=str_replace("%s",$host["host"],$description);	
				$sql="insert into triggers  (description,priority,status,comments,url,value) values ('".addslashes($description)."',".$trigger["priority"].",".$trigger["status"].",'".addslashes($trigger["comments"])."','".addslashes($trigger["url"])."',2)";
				$result4=DBexecute($sql);
				$triggerid=DBinsert_id($result4,"triggers","triggerid");

				$sql="select functionid from functions where triggerid=".$row2["triggerid"]." and itemid=".$row["itemid"];
				$result3=DBselect($sql);
				while($row3=DBfetch($result3))
				{
					$function=get_function_by_functionid($row3["functionid"]);
					$sql="insert into functions (itemid,triggerid,function,parameter) values ($itemid,$triggerid,'".$function["function"]."','".$function["parameter"]."')";
					$result4=DBexecute($sql);
					$functionid=DBinsert_id($result4,"functions","functionid");
					$sql="update triggers set expression='".$trigger["expression"]."' where triggerid=$triggerid";
					DBexecute($sql);
					$trigger["expression"]=str_replace("{".$row3["functionid"]."}","{".$functionid."}",$trigger["expression"]);
					$sql="update triggers set expression='".$trigger["expression"]."' where triggerid=$triggerid";
					DBexecute($sql);
				}
				# Add actions
				$sql="select actionid from actions where scope=0 and triggerid=".$row2["triggerid"];
				$result3=DBselect($sql);
				while($row3=DBfetch($result3))
				{
					$action=get_action_by_actionid($row3["actionid"]);
					$userid=$action["userid"];
					$scope=$action["scope"];
					$severity=$action["severity"];
					$good=$action["good"];
					$delay=$action["delay"];
					$subject=addslashes($action["subject"]);
					$message=addslashes($action["message"]);
					$recipient=$action["recipient"];
					$sql="insert into actions (triggerid, userid, scope, severity, good, delay, subject, message,recipient) values ($triggerid,$userid,$scope,$severity,$good,$delay,'$subject','$message',$recipient)";
//					echo "$sql<br>";
					$result4=DBexecute($sql);
					$actionid=DBinsert_id($result4,"actions","actionid");
				}
			}
		}

		return TRUE;
	}

	function	add_group_to_host($hostid,$newgroup)
	{
		$sql="insert into groups (groupid,name) values (NULL,'$newgroup')";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		
		$groupid=DBinsert_id($result,"groupd","groupid");

		$sql="insert into hosts_groups (hostid,groupid) values ($hostid,$groupid)";
		$result=DBexecute($sql);

		return $result;
	}

	function	update_host_groups_by_groupid($groupid,$hosts)
	{
		$count=count($hosts);

		$sql="delete from hosts_groups where groupid=$groupid";
		DBexecute($sql);

		for($i=0;$i<$count;$i++)
		{
			$sql="insert into hosts_groups (hostid,groupid) values (".$hosts[$i].",$groupid)";
			DBexecute($sql);
		}
	}

	function	update_host_groups($hostid,$groups)
	{
		$count=count($groups);

		$sql="delete from hosts_groups where hostid=$hostid";
		DBexecute($sql);

		for($i=0;$i<$count;$i++)
		{
			$sql="insert into hosts_groups (hostid,groupid) values ($hostid,".$groups[$i].")";
			DBexecute($sql);
		}
	}

	function	add_host_group($name,$hosts)
	{
		global	$ERROR_MSG;

//		if(!check_right("Host","A",0))
//		{
//			$ERROR_MSG="Insufficient permissions";
//			return 0;
//		}

		$sql="select * from groups where name='$name'";
		$result=DBexecute($sql);
		if(DBnum_rows($result)>0)
		{
			$ERROR_MSG="Group '$name' already exists";
			return 0;
		}

		$sql="insert into groups (name) values ('$name')";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		
		$groupid=DBinsert_id($result,"groups","groupid");

		update_host_groups_by_groupid($groupid,$hosts);

		return $result;
	}

	function	add_user_group($name,$users)
	{
		global	$ERROR_MSG;

		if(!check_right("Host","A",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

		$sql="select * from usrgrp where name='$name'";
		$result=DBexecute($sql);
		if(DBnum_rows($result)>0)
		{
			$ERROR_MSG="Group '$name' already exists";
			return 0;
		}

		$sql="insert into usrgrp (name) values ('$name')";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		
		$usrgrpid=DBinsert_id($result,"usrgrp","usrgrpid");

		update_user_groups($usrgrpid,$users);

		return $result;
	}

	function	update_host_group($groupid,$name,$users)
	{
		global	$ERROR_MSG;

//		if(!check_right("Host","U",0))
//		{
//			$ERROR_MSG="Insufficient permissions";
//			return 0;
//		}

		$sql="select * from groups where name='$name' and groupid<>$groupid";
		$result=DBexecute($sql);
		if(DBnum_rows($result)>0)
		{
			$ERROR_MSG="Group '$name' already exists";
			return 0;
		}

		$sql="update groups set name='$name' where groupid=$groupid";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		
		update_host_groups_by_groupid($groupid,$users);

		return $result;
	}

	function	update_user_group($usrgrpid,$name,$users)
	{
		global	$ERROR_MSG;

		if(!check_right("Host","U",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

		$sql="select * from usrgrp where name='$name' and usrgrpid<>$usrgrpid";
		$result=DBexecute($sql);
		if(DBnum_rows($result)>0)
		{
			$ERROR_MSG="Group '$name' already exists";
			return 0;
		}

		$sql="update usrgrp set name='$name' where usrgrpid=$usrgrpid";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		
		update_user_groups($usrgrpid,$users);

		return $result;
	}
		
	# Sync host with hard-linked templates
	function	sync_host_with_templates($hostid)
	{
		$sql="select * from hosts_templates where hostid=$hostid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			sync_host_with_template($hostid,$row["templateid"],$row["items"],$row["triggers"],$row["actions"],
                                                $row["graphs"],$row["screens"]);
		}
	}

	# Sync host with hard-linked template
	function	sync_host_with_template($hostid,$templateid,$items,$triggers,$actions,$graphs,$screens)
	{
		global	$ERROR_MSG;

		if(!isset($templateid)||($templateid==0))
		{
			$ERROR_MSG="Select template first";
			return 0;
		}

		$host=get_host_by_hostid($hostid);
		$sql="select itemid from items where hostid=$templateid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$item=get_item_by_itemid($row["itemid"]);
			$itemid=add_item($item["description"],$item["key_"],$hostid,$item["delay"],$item["history"],$item["status"],$item["type"],$item["snmp_community"],$item["snmp_oid"],$item["value_type"],"",$item["snmp_port"],$item["units"],$item["multiplier"],$item["delta"],$item["snmpv3_securityname"],$item["snmpv3_securitylevel"],$item["snmpv3_authpassphrase"],$item["snmpv3_privpassphrase"],$item["formula"],$item["trends"]);

			$sql="select distinct t.triggerid from triggers t,functions f where f.itemid=".$row["itemid"]." and f.triggerid=t.triggerid";
			$result2=DBselect($sql);
			while($row2=DBfetch($result2))
			{
				$trigger=get_trigger_by_triggerid($row2["triggerid"]);
// Cannot use add_trigger here
				$description=$trigger["description"];
#				$description=str_replace("%s",$host["host"],$description);	
				$sql="insert into triggers  (description,priority,status,comments,url,value) values ('".addslashes($description)."',".$trigger["priority"].",".$trigger["status"].",'".addslashes($trigger["comments"])."','".addslashes($trigger["url"])."',2)";
				$result4=DBexecute($sql);
				$triggerid=DBinsert_id($result4,"triggers","triggerid");

				$sql="select functionid from functions where triggerid=".$row2["triggerid"]." and itemid=".$row["itemid"];
				$result3=DBselect($sql);
				while($row3=DBfetch($result3))
				{
					$function=get_function_by_functionid($row3["functionid"]);
					$sql="insert into functions (itemid,triggerid,function,parameter) values ($itemid,$triggerid,'".$function["function"]."','".$function["parameter"]."')";
					$result4=DBexecute($sql);
					$functionid=DBinsert_id($result4,"functions","functionid");
					$sql="update triggers set expression='".$trigger["expression"]."' where triggerid=$triggerid";
					DBexecute($sql);
					$trigger["expression"]=str_replace("{".$row3["functionid"]."}","{".$functionid."}",$trigger["expression"]);
					$sql="update triggers set expression='".$trigger["expression"]."' where triggerid=$triggerid";
					DBexecute($sql);
				}
				# Add actions
				$sql="select actionid from actions where scope=0 and triggerid=".$row2["triggerid"];
				$result3=DBselect($sql);
				while($row3=DBfetch($result3))
				{
					$action=get_action_by_actionid($row3["actionid"]);
					$userid=$action["userid"];
					$scope=$action["scope"];
					$severity=$action["severity"];
					$good=$action["good"];
					$delay=$action["delay"];
					$subject=addslashes($action["subject"]);
					$message=addslashes($action["message"]);
					$recipient=$action["recipient"];
					$sql="insert into actions (triggerid, userid, scope, severity, good, delay, subject, message,recipient) values ($triggerid,$userid,$scope,$severity,$good,$delay,'$subject','$message',$recipient)";
//					echo "$sql<br>";
					$result4=DBexecute($sql);
					$actionid=DBinsert_id($result4,"actions","actionid");
				}
			}
		}

		# Add graphs
		$sql="select distinct g.graphid from graphs g,items i, hosts h,graphs_items gi where h.hostid=$templateid and i.hostid=h.hostid and gi.itemid=i.itemid and g.graphid=gi.graphid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$graph=get_graph_by_graphid($row["graphid"]);
			$graphid=add_graph($graph["name"],$graph["width"],$graph["height"],$graph["yaxistype"],$graph["yaxismin"],$graph["yaxismax"]);
			$sql="select distinct gi.gitemid from graphs_items gi,graphs g,items i,hosts h  where gi.itemid=i.itemid and h.hostid=$templateid and i.hostid=h.hostid and g.graphid=gi.graphid and gi.itemid=i.itemid and g.graphid=".$graph["graphid"];
			$result2=DBselect($sql);
			while($row=DBfetch($result2))
			{
				$gitem=get_graphitem_by_gitemid($row["gitemid"]);
				$item=get_item_by_itemid($gitem["itemid"]);
				$sql="select * from items where key_='".$item["key_"]."' and hostid=$hostid";
				$result3=DBselect($sql);
				if(DBnum_rows($result3)==1)
				{
					$row2=DBfetch($result3);
					add_item_to_graph($graphid,$row2["itemid"],$gitem["color"],$gitem["drawtype"],$gitem["sortorder"]);
				}
			}
		}

		return TRUE;
	}

	# Delete Media definition by mediatypeid

	function	delete_media_by_mediatypeid($mediatypeid)
	{
		$sql="delete from media where mediatypeid=$mediatypeid";
		return	DBexecute($sql);
	}

	# Delete alrtes by mediatypeid

	function	delete_alerts_by_mediatypeid($mediatypeid)
	{
		$sql="delete from alerts where mediatypeid=$mediatypeid";
		return	DBexecute($sql);
	}

	function	get_mediatype_by_mediatypeid($mediatypeid)
	{
		global	$ERROR_MSG;

		$sql="select * from media_type where mediatypeid=$mediatypeid";
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No media type with with mediatypeid=[$mediatypeid]";
		}
		return	$item;
	}

	# Delete media type

	function	delete_mediatype($mediatypeid)
	{

		delete_media_by_mediatypeid($mediatypeid);
		delete_alerts_by_mediatypeid($mediatypeid);
		$sql="delete from media_type where mediatypeid=$mediatypeid";
		return	DBexecute($sql);
	}

	# Update media type

	function	update_mediatype($mediatypeid,$type,$description,$smtp_server,$smtp_helo,$smtp_email,$exec_path)
	{
		$description=addslashes($description);
		$sql="update media_type set type=$type,description='$description',smtp_server='$smtp_server',smtp_helo='$smtp_helo',smtp_email='$smtp_email',exec_path='$exec_path' where mediatypeid=$mediatypeid";
		return	DBexecute($sql);
	}

	# Add Media type

	function	add_mediatype($type,$description,$smtp_server,$smtp_helo,$smtp_email,$exec_path)
	{
		$description=addslashes($description);
		$sql="insert into media_type (type,description,smtp_server,smtp_helo,smtp_email,exec_path) values ($type,'$description','$smtp_server','$smtp_helo','$smtp_email','$exec_path')";
		return	DBexecute($sql);
	}

	# Add Media definition

	function	add_media( $userid, $mediatypeid, $sendto, $severity, $active)
	{
		$c=count($severity);
		$s=0;
		for($i=0;$i<$c;$i++)
		{
			$s=$s|pow(2,(int)$severity[$i]);
		}
		$sql="insert into media (userid,mediatypeid,sendto,active,severity) values ($userid,'$mediatypeid','$sendto',$active,$s)";
		return	DBexecute($sql);
	}

	# Update Media definition

	function	update_media($mediaid, $userid, $mediatypeid, $sendto, $severity, $active)
	{
		$c=count($severity);
		$s=0;
		for($i=0;$i<$c;$i++)
		{
			$s=$s|pow(2,(int)$severity[$i]);
		}
		$sql="update media set userid=$userid, mediatypeid=$mediatypeid, sendto='$sendto', active=$active,severity=$s where mediaid=$mediaid";
		return	DBexecute($sql);
	}

	# Delete Media definition

	function	delete_media($mediaid)
	{
		$sql="delete from media where mediaid=$mediaid";
		return	DBexecute($sql);
	}

	# Delete Media definition by userid

	function	delete_media_by_userid($userid)
	{
		$sql="delete from media where userid=$userid";
		return	DBexecute($sql);
	}

	function	delete_profiles_by_userid($userid)
	{
		$sql="delete from profiles where userid=$userid";
		return	DBexecute($sql);
	}

	# Update configuration

//	function	update_config($smtp_server,$smtp_helo,$smtp_email,$alarm_history,$alert_history)
	function	update_config($alarm_history,$alert_history)
	{
		global	$ERROR_MSG;

		if(!check_right("Configuration of Allview","U",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return	0;
		}


//		$sql="update config set smtp_server='$smtp_server',smtp_helo='$smtp_helo',smtp_email='$smtp_email',alarm_history=$alarm_history,alert_history=$alert_history";
		$sql="update config set alarm_history=$alarm_history,alert_history=$alert_history";
		return	DBexecute($sql);
	}


	# Activate Media

	function	activate_media($mediaid)
	{
		$sql="update media set active=0 where mediaid=$mediaid";
		return	DBexecute($sql);
	}

	# Disactivate Media

	function	disactivate_media($mediaid)
	{
		$sql="update media set active=1 where mediaid=$mediaid";
		return	DBexecute($sql);
	}

	# Delete User permission

	function	delete_permission($rightid)
	{
		$sql="delete from rights where rightid=$rightid";
		return DBexecute($sql);
	}

	function	delete_user_group($usrgrpid)
	{
		global	$ERROR_MSG;

		$sql="delete from users_groups where usrgrpid=$usrgrpid";
		DBexecute($sql);
		$sql="delete from usrgrp where usrgrpid=$usrgrpid";
		return DBexecute($sql);
	}

	# Delete User definition

	function	delete_user($userid)
	{
		global	$ERROR_MSG;

		$sql="select * from users where userid=$userid and alias='guest'";
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			$ERROR_MSG="Cannot delete user 'guest'";
			return	0;
		}


		delete_media_by_userid($userid);
		delete_actions_by_userid($userid);
		delete_rights_by_userid($userid);
		delete_profiles_by_userid($userid);

		$sql="delete from users_groups where userid=$userid";
		DBexecute($sql);
		$sql="delete from users where userid=$userid";
		return DBexecute($sql);
	}

	function	show_table_h_delimiter()
	{
//		echo "</font>";
		cr();
		echo "</td>";
		cr();
		echo "<td colspan=1 bgcolor=FFFFFF align=center valign=\"top\">";
		cr();
//		echo "	<font size=2>";
		cr();
	}

	function	show_table2_h_delimiter()
	{
//		echo "</font>";
		cr();
		echo "</td>";
		cr();
//		echo "<td colspan=1 bgcolor=CCCCCC align=left valign=\"top\">";
		echo "<td class=\"form_row_r\" height=24>";
		cr();
//		echo "	<font size=-1>";
		cr();
	}

	function	show_table3_h_delimiter($width=10)
	{
?>
        </td><td class="sub_menu" height=24 colspan=9 nowrap="nowrap" width="<?php echo $width;?>%">
<?php
/*
		cr();
		echo "</td>";
		cr();
		echo "<td width=$width% colspan=1 bgcolor=6d88ad align=right valign=\"top\">";
		cr();
		cr();
*/
	}


	function	show_table_v_delimiter($colspan=1)
	{
//		echo "</font>";
		cr();
		echo "</td>";
		cr();
		echo "</tr>";
		cr();
		echo "<tr>";
		cr();
		echo "<td colspan=$colspan bgcolor=FFFFFF align=center valign=\"top\">";
		cr();
//		echo "<font size=2>";
		cr();
	}

	function	show_table2_v_delimiter($rownum=0)
	{
//		echo "</font>";
		cr();
		echo "</td>";
		cr();
		echo "</tr>";
		cr();
		if($rownum%2 == 1)	{ echo "<TR BGCOLOR=\"#DFDFDF\">"; }
		else			{ echo "<TR BGCOLOR=\"#D8D8D8\">"; }
		cr();
//		echo "<td colspan=1 bgcolor=CCCCCC align=left valign=\"top\">";
		echo "<td class=\"form_row_l\" height=24>";
		cr();
//		echo "<font size=-1>";
		cr();
	}

	function	show_table3_v_delimiter()
	{
?>
        </td><td class="sub_nemu" height=24 colspan=9>

<?php
/*		cr();
		echo "</td>";
		cr();
		echo "</tr>";
		cr();
		echo "<tr>";
		cr();
		echo "<td colspan=1 bgcolor=#6d88ad align=left valign=\"top\">";
		cr();
		cr();*/
	}


	function	show_table2_v_delimiter2()
	{
//		echo "</font>";
		cr();
		echo "</td>";
		cr();
		echo "</tr>";
		cr();
		echo "<tr>";
		cr();
//		echo "<td colspan=2 bgcolor=\"99AABB\" align=right valign=\"top\">";
		echo "<td class=\"form_row_last\" colspan=2 align=right>";
		cr();
//		echo "<font size=-1>";
		cr();
	}


//	function	show_table2_header_begin()
	function	show_form_begin($help="")
	{
?>
<p align=center>
<table class="form" width="50%" cellspacing=0 cellpadding=1>
<tr>
<td class="form_row_first" height=24 colspan=2>
<?php
		if($help!="")
		{
			echo "<a style=\"float:right\" href=\"http://www.myleftstudio.com/manual/v1.1/web.$help.php\"><img src=\"images/general/help.gif\" border=0 alt=\"?\"></a>";
		}
	}

	function	show_table_header_begin()
	{
		echo "<table border=0 align=center cellspacing=0 cellpadding=0 width=100% bgcolor=000000>";
		cr();
		echo "<tr>";
		cr();
		echo "<td valign=\"top\">";
		cr();
		echo "<table width=100% border=0 cellspacing=1 cellpadding=3>";
		cr();
		echo "<tr>";
		cr();
//		echo "<td colspan=1 bgcolor=99AABB align=center valign=\"top\">";
		echo "<td colspan=1 bgcolor=6d88ad align=center valign=\"top\">";
		cr();
//		echo "	<font size=+1>";
		cr();
	}

	function	show_header2($h1, $h2, $h2_form1, $h2_form2)
	{
?>
	<?php echo $h2_form1; ?>
	<table class="menu" cellspacing=0 cellpadding=1 width="100%">
	<tr>
	<td class="sub_menu" height=24 align=left>
	<?php echo $h1; ?>
	</td>
	<td class="sub_menu" height=24 align=right>
	<?php echo $h2; ?>
	</td>
	</tr>
	</table>
	<?php echo $h2_form2; ?>
<?php
	}

	function	show_table3_header_begin()
	{
?>
	<table class="menu" cellspacing=0 cellpadding=1 width="100%">
	<tr>
<?php
		echo "<td class=\"sub_menu\" height=24 colspan=9 nowrap=\"nowrap\">";
	}


	function	show_table2_header_end()
	{
//		cr();
//		echo "</td>";
//		cr();
//		echo "</tr>";
//		cr();
//		echo "</table>";
		cr();
		echo "</td>";
		cr();
		echo "</tr>";
		cr();
		echo "</table>";
//		echo "</p>";
		cr();
	}

	function	show_table_header_end()
	{
/*		cr();
		echo "</td>";
		cr();
		echo "</tr>";
		cr();
		echo "</table>";
		cr();*/
		echo "</td>";
		echo "</tr>";
		echo "</table>";
	}

	function	show_table_header($title)
	{
?>
<table class="menu" width="100%" cellspacing=0 cellpadding=1>
<tr>
<td class="sub_menu" height=24 colspan=9><?php echo $title; ?></td>
</tr>
</table>
<?php
	}

	function	insert_time_navigator($itemid,$period,$from)
	{
		$descr=array("January","February","March","April","May","June",
			"July","August","September","October","November","December");
		$sql="select min(clock),max(clock) from history where itemid=$itemid";
		$result=DBselect($sql);

		if(DBnum_rows($result) == 0)
		{
			$min=time(NULL);
			$max=time(NULL);
		}
		else
		{
			$min=DBget_field($result,0,0);
			$max=DBget_field($result,0,1);
		}

		$now=time()-3600*$from-$period;

		$year_min=date("Y",$min);   
		$year_max=date("Y",$max);

		$year_now=date("Y",$now);
		$month_now=date("m",$now);
		$day_now=date("d",$now);
		$hour_now=date("H",$now);

		echo "<form method=\"put\" action=\"history.php\">";
		echo "<input name=\"itemid\" type=\"hidden\" value=$itemid size=8>";
		echo "<input name=\"action\" type=\"hidden\" value=\"showhistory\" size=8>";

		echo "Year";
		echo "<select name=\"year\">";
	        for($i=$year_min;$i<=$year_max;$i++)
	        {
			if($i==$year_now)
			{	
	               		echo "<option value=\"$i\" selected>$i";
			}
			else
			{
	               		echo "<option value=\"$i\">$i";
			}
	        }
		echo "</select>";

		echo "Month";
		echo "<select name=\"month\">";
	        for($i=1;$i<=12;$i++)
	        {
			if($i==$month_now)
			{	
	               		echo "<option value=\"$i\" selected>".$descr[$i-1];
			}
			else
			{
	               		echo "<option value=\"$i\">".$descr[$i-1];
			}
	        }
		echo "</select>";

		echo "Day";
		echo "<select name=\"day\">";
	        for($i=1;$i<=31;$i++)
	        {
			if($i==$day_now)
			{	
	               		echo "<option value=\"$i\" selected>$i";
			}
			else
			{
	               		echo "<option value=\"$i\">$i";
			}
	        }
		echo "</select>";

		echo "Hour";
		echo "<select name=\"hour\">";
	        for($i=0;$i<=23;$i++)
	        {
			if($i==$hour_now)
			{	
	               		echo "<option value=\"$i\" selected>$i";
			}
			else
			{
	               		echo "<option value=\"$i\">$i";
			}
	        }
		echo "</select>";

		echo "Period:";
		echo "<select name=\"period\">";
		if($period==3600)
		{
			echo "<option value=\"3600\" selected>1 hour";
		}
		else
		{
			echo "<option value=\"3600\">1 hour";
		}
		if($period==10800)
		{
			echo "<option value=\"10800\" selected>3 hours";
		}
		else
		{
			echo "<option value=\"10800\">3 hours";
		}
		if($period==21600)
		{
			echo "<option value=\"21600\" selected>6 hours";
		}
		else
		{
			echo "<option value=\"21600\">6 hours";
		}
		echo "</select>";

		echo "<input class=\"button\" type=\"submit\" name=\"action\" value=\"showhistory\">";

		echo "</form>";
	}

	# Show History Graph

	function	show_history($itemid,$from,$period,$diff)
	{
		if (!isset($from))
		{
			$from=0;
			$till="NOW";
		}
		else
		{
			$till=time(NULL)-$from*3600;
			$till=date(S_DATE_FORMAT_YMDHMS,$till);   
		}

		if (!isset($period))
		{ 
			$period=3600;
			show_table_header("TILL $till (LAST HOUR)");
		}
		else
		{
			$tmp=$period/3600;
			show_table_header("TILL $till ($tmp HOURs)");
		}
//		echo("<hr>");
		echo "<center>";
		echo "<TABLE BORDER=0 WIDTH=100% BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=3>";
		echo "<TR BGCOLOR=#EEEEEE>";
		echo "<TR BGCOLOR=#DDDDDD>";
		echo "<TD ALIGN=CENTER>";

		if($diff==0)
		{
			echo "<script language=\"JavaScript\">";
			echo "if (navigator.appName == \"Microsoft Internet Explorer\")";
			echo "{";
			echo " document.write(\"<IMG SRC='chart.php?itemid=$itemid&period=$period&from=$from&width=\"+(document.body.clientWidth-108)+\"'>\")";
			echo "}";
			echo "else if (navigator.appName == \"Netscape\")";
			echo "{";
			echo " document.write(\"<IMG SRC='chart.php?itemid=$itemid&period=$period&from=$from&width=\"+(document.width-108)+\"'>\")";
			echo "}";
			echo "else";
			echo "{";
			echo " document.write(\"<IMG SRC='chart.php?itemid=$itemid&period=$period&from=$from'>\")";
			echo "}";
			echo "</script>";
		}
		else
		{
			echo "<script language=\"JavaScript\">";
			echo "if (navigator.appName == \"Microsoft Internet Explorer\")";
			echo "{";
			echo " document.write(\"<IMG SRC='chart_diff.php?itemid=$itemid&period=$period&from=$from&width=\"+(document.body.clientWidth-108)+\"'>\")";
			echo "}";
			echo "else if (navigator.appName == \"Netscape\")";
			echo "{";
			echo " document.write(\"<IMG SRC='chart_diff.php?itemid=$itemid&period=$period&from=$from&width=\"+(document.width-108)+\"'>\")";
			echo "}";
			echo "else";
			echo "{";
			echo " document.write(\"<IMG SRC='chart_diff.php?itemid=$itemid&period=$period&from=$from'>\")";
			echo "}";
			echo "</script>";
		}
		echo "</TD>";
		echo "</TR>";
		echo "</TABLE>";
		echo "</center>";
		echo("<hr>");
		insert_time_navigator($itemid,$period,$from);
		echo("<hr>");
	}

	# Show history
	function	show_freehist($itemid,$period)
	{
		show_form_begin("history.period");
		echo "Choose period";

		show_table2_v_delimiter();
		echo "<form method=\"get\" action=\"history.php\">";
		echo "<input name=\"itemid\" type=\"hidden\" value=$itemid size=8>";
		echo "Period in seconds";
		show_table2_h_delimiter();
		echo "<input name=\"period\" value=\"7200\" size=8>";

		show_table2_v_delimiter();
		echo "From (in hours)";
		show_table2_h_delimiter();
		echo "<input name=\"from\" value=\"24\" size=8>";

		show_table2_v_delimiter2();
		echo "Press ";
		echo "<input class=\"button\" type=\"submit\" name=\"action\" value=\"showvalues\"> to see values in plain text";

		show_table2_header_end();

		show_footer();
	}

	# Show in plain text
	function	show_plaintxt($itemid,$period)
	{
		show_form_begin("history.plaim");
		echo "Data in plain text format";

		show_table2_v_delimiter();
		echo "<form method=\"get\" action=\"history.php\">";
		echo "<input name=\"itemid\" type=\"Hidden\" value=$itemid size=8>";
		echo "<input name=\"itemid\" type=\"Hidden\" value=$itemid size=8>";
		echo "From: (yyyy/mm/dd - HH:MM)";
		show_table2_h_delimiter();
		echo "<input name=\"fromyear\" value=\"",date("Y"),"\" size=5>/";
		echo "<input name=\"frommonth\" value=\"",date("m"),"\" size=3>/";
		echo "<input name=\"fromday\" value=\"",date("d"),"\" size=3> - ";
		echo "<input name=\"fromhour\" value=\"0\" size=3>:";
		echo "<input name=\"frommin\" value=\"00\" size=3>";

		show_table2_v_delimiter();
		echo "Till: (yyyy/mm/dd - HH:MM)";
		show_table2_h_delimiter();
		echo "<input name=\"tillyear\" value=\"",date("Y"),"\" size=5>/";
		echo "<input name=\"tillmonth\" value=\"",date("m"),"\" size=3>/";
		echo "<input name=\"tillday\" value=\"",date("d"),"\" size=3> - ";
		echo "<input name=\"tillhour\" value=\"23\" size=3>:";
		echo "<input name=\"tillmin\" value=\"59\" size=3>";

		show_table2_v_delimiter2();
		echo "Press to see data in ";
		echo "<input class=\"button\" type=\"submit\" name=\"action\" value=\"plaintext\">";

		show_table2_header_end();

		show_footer();
	}

	function	show_footer()
	{
		global $USER_DETAILS;

?>
<p>
<table class="menu" width="100%" cellspacing=0 cellpadding=5>
<tr>
<td class="horizontal_menu" height=24 colspan=9 align=center><b><?php echo "<a href=\"http://www.myleftstudio.com\" class=\"highlight\">".S_ALLVIEW_VER."</a>&nbsp;".S_COPYRIGHT_BY."<a href=\"mailto:allview@gobbo.caves.lv\" class=\"highlight\">".S_ALLVIEW_TEAM."</a>"; ?></b></td>
<td class="horizontal_menu" height=24 colspan=9 align=right><b><span class="divider">&nbsp;&nbsp;|&nbsp;&nbsp;</span><?php echo " ".S_CONNECTED_AS."&nbsp;".$USER_DETAILS["alias"];?></b></td>
</tr>
</table>

</body>
</html>
<?php
	}

	function	get_stats()
	{
	        $result=DBselect("select count(*) from history");
		$stat["history_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from trends");
		$stat["trends_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from alarms");
		$stat["alarms_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from alerts");
		$stat["alerts_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from triggers");
		$stat["triggers_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from triggers where status=0");
		$stat["triggers_count_enabled"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from triggers where status=1");
		$stat["triggers_count_disabled"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from items");
		$stat["items_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from items where status=0");
		$stat["items_count_active"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from items where status=1");
		$stat["items_count_not_active"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from items where status=3");
		$stat["items_count_not_supported"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from items where type=2");
		$stat["items_count_trapper"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from hosts");
		$stat["hosts_count"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from hosts where status=".HOST_STATUS_MONITORED);
		$stat["hosts_count_monitored"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from hosts where status!=".HOST_STATUS_MONITORED);
		$stat["hosts_count_not_monitored"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from hosts where status=".HOST_STATUS_DELETED);
		$stat["hosts_count_template"]=DBget_field($result,0,0);

	        $result=DBselect("select count(*) from users");
		$stat["users_count"]=DBget_field($result,0,0);


		return $stat;
	}

	// If $period_start=$period_end=0, then take maximum period
	function	calculate_availability($triggerid,$period_start,$period_end)
	{
		if(($period_start==0)&&($period_end==0))
		{
	        	$sql="select count(*),min(clock),max(clock) from alarms where triggerid=$triggerid";
		}
		else
		{
	        	$sql="select count(*),min(clock),max(clock) from alarms where triggerid=$triggerid and clock>=$period_start and clock<=$period_end";
		}
//		echo $sql,"<br>";

		
	        $result=DBselect($sql);
		if(DBget_field($result,0,0)>0)
		{
			$min=DBget_field($result,0,1);
			$max=DBget_field($result,0,2);
		}
		else
		{
			if(($period_start==0)&&($period_end==0))
			{
				$max=time();
				$min=$max-24*3600;
			}
			else
			{
				$ret["true_time"]=0;
				$ret["false_time"]=0;
				$ret["unknown_time"]=0;
				$ret["true"]=0;
				$ret["false"]=0;
				$ret["unknown"]=100;
				return $ret;
			}
		}

		$sql="select clock,value from alarms where triggerid=$triggerid and clock>=$min and clock<=$max";
//		echo " $sql<br>";
		$result=DBselect($sql);

//		echo $sql,"<br>";

// -1,0,1
		$state=-1;
		$true_time=0;
		$false_time=0;
		$unknown_time=0;
		$time=$min;
		if(($period_start==0)&&($period_end==0))
		{
			$max=time();
		}
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$clock=DBget_field($result,$i,0);
			$value=DBget_field($result,$i,1);

			$diff=$clock-$time;

			$time=$clock;

			if($state==-1)
			{
				$state=$value;
				if($state == 0)
				{
					$false_time+=$diff;
				}
				if($state == 1)
				{
					$true_time+=$diff;
				}
				if($state == 2)
				{
					$unknown_time+=$diff;
				}
			}
			else if($state==0)
			{
				$false_time+=$diff;
				$state=$value;
			}
			else if($state==1)
			{
				$true_time+=$diff;
				$state=$value;
			}
			else if($state==2)
			{
				$unknown_time+=$diff;
				$state=$value;
			}
		}

		if(DBnum_rows($result)==0)
		{
			$false_time=$max-$min;
		}
		else
		{
			if($state==0)
			{
				$false_time=$false_time+$max-$time;
			}
			elseif($state==1)
			{
				$true_time=$true_time+$max-$time;
			}
			elseif($state==3)
			{
				$unknown_time=$unknown_time+$max-$time;
			}

		}
//		echo "$true_time $false_time $unknown_time";

		$total_time=$true_time+$false_time+$unknown_time;
		if($total_time==0)
		{
			$ret["true_time"]=0;
			$ret["false_time"]=0;
			$ret["unknown_time"]=0;
			$ret["true"]=0;
			$ret["false"]=0;
			$ret["unknown"]=100;
		}
		else
		{
			$ret["true_time"]=$true_time;
			$ret["false_time"]=$false_time;
			$ret["unknown_time"]=$unknown_time;
			$ret["true"]=(100*$true_time)/$total_time;
			$ret["false"]=(100*$false_time)/$total_time;
			$ret["unknown"]=(100*$unknown_time)/$total_time;
		}
		return $ret;
	}

	function	get_resource_name($permission,$id)
	{
		$res="-";
		if($permission=="Graph")
		{
			if(isset($id)&&($id!=0))
			{
				$host=get_graph_by_graphid($id);
				$res=$host["name"];
			}
			else
			{
				$res="All graphs";
			}
		}
		else if($permission=="Host")
		{
			if(isset($id)&&($id!=0))
			{
				$host=get_host_by_hostid($id);
				$res=$host["host"];
			}
			else
			{
				$res="All hosts";
			}
		}
		else if($permission=="Screen")
		{
			if(isset($id)&&($id!=0))
			{
				$screen=get_screen_by_screenid($id);
				$res=$screen["name"];
			}
			else
			{
				$res="All hosts";
			}
		}
		else if($permission=="Item")
		{
			if(isset($id)&&($id!=0))
			{
				$item=get_item_by_itemid($id);
				$host=get_host_by_hostid($item["hostid"]);
				$res=$host["host"].":".$item["description"];
			}
			else
			{
				$res="All items";
			}
		}
		else if($permission=="User")
		{
			if(isset($id)&&($id!=0))
			{
				$user=get_user_by_userid($id);
				$res=$user["alias"];
			}
			else
			{
				$res="All users";
			}
		}
		else if($permission=="Network map")
		{
			if(isset($id)&&($id!=0))
			{
				$user=get_map_by_sysmapid($id);
				$res=$user["name"];
			}
			else
			{
				$res="All maps";
			}
		}
		return $res;
	}

	function	get_profile($idx,$default_value)
	{
		global $USER_DETAILS;

		if($USER_DETAILS["alias"]=="guest")
		{
			return $default_value;
		}

		$sql="select value from profiles where userid=".$USER_DETAILS["userid"]." and idx='$idx'";
		$result=DBselect($sql);

		if(DBnum_rows($result)==0)
		{
			return $default_value;
		}
		else
		{
			$row=DBfetch($result);
			return $row["value"];
		}
	}

	function	update_profile($idx,$value)
	{
		global $USER_DETAILS;

		if($USER_DETAILS["alias"]=="guest")
		{
			return;
		}

		$sql="select value from profiles where userid=".$USER_DETAILS["userid"]." and idx='$idx'";
		$result=DBselect($sql);

		if(DBnum_rows($result)==0)
		{
			$sql="insert into profiles (userid,idx,value) values (".$USER_DETAILS["userid"].",'$idx','$value')";
			DBexecute($sql);
		}
		else
		{
			$row=DBfetch($result);
			$sql="update profiles set value='$value' where userid=".$USER_DETAILS["userid"]." and idx='$idx'";
			DBexecute($sql);
		}
	}

        function get_drawtype_description($drawtype)
        {
		if($drawtype==0)
			return "Line";
		if($drawtype==1)
			return "Filled region";
		if($drawtype==2)
			return "Bold line";
		if($drawtype==3)
			return "Dot";
		if($drawtype==4)
			return "Dashed line";
		return "Unknown";
        }

	function insert_confirm_javascript()
	{
		echo "<SCRIPT LANGUAGE=\"JavaScript\">";

		echo "function Confirm(msg)";
		echo "{";
		echo "	if(confirm( msg))";
		echo "	{";
		echo "		return true;";
		echo "	}";
		echo "	else";
		echo "	{";
		echo "		return false;";
		echo "	}";
		echo "}";
		echo "</SCRIPT>";
	}

/* Use ImageSetStyle+ImageLIne instead of bugged ImageDashedLine */
	if(function_exists("imagesetstyle"))
	{
		function DashedLine($image,$x1,$y1,$x2,$y2,$color)
		{
// Style for dashed lines
//			$style = array($color, $color, $color, $color, IMG_COLOR_TRANSPARENT, IMG_COLOR_TRANSPARENT, IMG_COLOR_TRANSPARENT, IMG_COLOR_TRANSPARENT);
			$style = array($color, $color, IMG_COLOR_TRANSPARENT, IMG_COLOR_TRANSPARENT);
			ImageSetStyle($image, $style);
			ImageLine($image,$x1,$y1,$x2,$y2,IMG_COLOR_STYLED);
		}
	}
	else
	{
		function DashedLine($image,$x1,$y1,$x2,$y2,$color)
		{
			ImageDashedLine($image,$x1,$y1,$x2,$y2,$color);
		}
	}


	function time_navigator($resource="graphid",$id)
	{
	echo "<TABLE BORDER=0 align=center COLS=2 WIDTH=100% BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=1>";
	echo "<TR BGCOLOR=#FFFFFF>";
	echo "<TD ALIGN=LEFT>";

	echo "<div align=left>";
	echo "<b>".S_PERIOD.":</b>&nbsp;";

	$hour=3600;
		
		$a=array(S_1H=>3600,S_2H=>2*3600,S_4H=>4*3600,S_8H=>8*3600,S_12H=>12*3600,
			S_24H=>24*3600,S_WEEK_SMALL=>7*24*3600,S_MONTH_SMALL=>31*24*3600,S_YEAR_SMALL=>365*24*3600);
		foreach($a as $label=>$sec)
		{
			echo "[";
			if($_GET["period"]>$sec)
			{
				$tmp=$_GET["period"]-$sec;
				echo("<A HREF=\"charts.php?period=$tmp".url_param($resource).url_param("stime").url_param("from").url_param("keep").url_param("fullscreen")."\">-</A>");
			}
			else
			{
				echo "-";
			}

			echo("<A HREF=\"charts.php?period=$sec".url_param($resource).url_param("stime").url_param("from").url_param("keep").url_param("fullscreen")."\">");
			echo($label."</A>");

			$tmp=$_GET["period"]+$sec;
			echo("<A HREF=\"charts.php?period=$tmp".url_param($resource).url_param("stime").url_param("from").url_param("keep").url_param("fullscreen")."\">+</A>");

			echo "]&nbsp;";
		}

		echo("</div>");

	echo "</TD>";
	echo "<TD BGCOLOR=#FFFFFF WIDTH=15% ALIGN=RIGHT>";
	echo "<b>".nbsp(S_KEEP_PERIOD).":</b>&nbsp;";
		if($_GET["keep"] == 1)
		{
			echo("[<A HREF=\"charts.php?keep=0".url_param($resource).url_param("from").url_param("period").url_param("fullscreen")."\">".S_ON_C."</a>]");
		}
		else
		{
			echo("[<A HREF=\"charts.php?keep=1".url_param($resource).url_param("from").url_param("period").url_param("fullscreen")."\">".S_OFF_C."</a>]");
		}
	echo "</TD>";
	echo "</TR>";
	echo "<TR BGCOLOR=#FFFFFF>";
	echo "<TD>";
	if(isset($_GET["stime"]))
	{
		echo "<div align=left>" ;
		echo "<b>".S_MOVE.":</b>&nbsp;" ;

		$day=24;
// $a already defined
//		$a=array("1h"=>1,"2h"=>2,"4h"=>4,"8h"=>8,"12h"=>12,
//			"24h"=>24,"week"=>7*24,"month"=>31*24,"year"=>365*24);
		foreach($a as $label=>$hours)
		{
			echo "[";

			$stime=$_GET["stime"];
			$tmp=mktime(substr($stime,8,2),substr($stime,10,2),0,substr($stime,4,2),substr($stime,6,2),substr($stime,0,4));
			$tmp=$tmp-3600*$hours;
			$tmp=date("YmdHi",$tmp);
			echo("<A HREF=\"charts.php?stime=$tmp".url_param($resource).url_param("period").url_param("keep").url_param("fullscreen")."\">-</A>");

			echo($label);

			$stime=$_GET["stime"];
			$tmp=mktime(substr($stime,8,2),substr($stime,10,2),0,substr($stime,4,2),substr($stime,6,2),substr($stime,0,4));
			$tmp=$tmp+3600*$hours;
			$tmp=date("YmdHi",$tmp);
			echo("<A HREF=\"charts.php?stime=$tmp".url_param($resource).url_param("period").url_param("keep").url_param("fullscreen")."\">+</A>");

			echo "]&nbsp;";
		}
		echo("</div>");
	}
	else
	{
		echo "<div align=left>";
		echo "<b>".S_MOVE.":</b>&nbsp;";

		$day=24;
// $a already defined
//		$a=array("1h"=>1,"2h"=>2,"4h"=>4,"8h"=>8,"12h"=>12,
//			"24h"=>24,"week"=>7*24,"month"=>31*24,"year"=>365*24);
		foreach($a as $label=>$hours)
		{
			echo "[";
			$tmp=$_GET["from"]+$hours;
			echo("<A HREF=\"charts.php?from=$tmp".url_param($resource).url_param("period").url_param("keep").url_param("fullscreen")."\">-</A>");

			echo($label);

			if($_GET["from"]>=$hours)
			{
				$tmp=$_GET["from"]-$hours;
				echo("<A HREF=\"charts.php?from=$tmp".url_param($resource).url_param("period").url_param("keep").url_param("fullscreen")."\">+</A>");
			}
			else
			{
				echo "+";
			}

			echo "]&nbsp;";
		}
		echo("</div>");
	}
	echo "</TD>";
	echo "<TD BGCOLOR=#FFFFFF WIDTH=15% ALIGN=RIGHT>";
//		echo("<div align=left>");
		echo "<form method=\"put\" action=\"charts.php\">";
		echo "<input name=\"graphid\" type=\"hidden\" value=\"".$_GET[$resource]."\" size=12>";
		echo "<input name=\"period\" type=\"hidden\" value=\"".(9*3600)."\" size=12>";
		if(isset($_GET["stime"]))
		{
			echo "<input name=\"stime\" class=\"biginput\" value=\"".$_GET["stime"]."\" size=12>";
		}
		else
		{
			echo "<input name=\"stime\" class=\"biginput\" value=\"yyyymmddhhmm\" size=12>";
		}
		echo "&nbsp;";
		echo "<input class=\"button\" type=\"submit\" name=\"action\" value=\"go\">";
		echo "</form>";
//		echo("</div>");
	echo "</TD>";
	echo "</TR>";
	echo "</TABLE>";
	}

	function ImageOut($image)
	{
//		ImageJPEG($image);
		ImagePNG($image);
	}

	function	update_user_groups($usrgrpid,$users)
	{
		$count=count($users);

		$sql="delete from users_groups where usrgrpid=$usrgrpid";
		DBexecute($sql);

		for($i=0;$i<$count;$i++)
		{
			$sql="insert into users_groups (usrgrpid,userid) values ($usrgrpid,".$users[$i].")";
			DBexecute($sql);
		}
	}
?>
