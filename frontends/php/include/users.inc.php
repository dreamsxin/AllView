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
	# Add User definition

	function	add_user($name,$surname,$alias,$passwd,$url)
	{
		global	$ERROR_MSG;

		if(!check_right("User","A",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}
		

		$passwd=md5($passwd);
		$sql="insert into users (name,surname,alias,passwd,url) values ('$name','$surname','$alias','$passwd','$url')";
		return DBexecute($sql);
	}

	# Update User definition

	function	update_user($userid,$name,$surname,$alias,$passwd, $url)
	{
		global	$ERROR_MSG;

		if(!check_right("User","U",$userid))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

		if($passwd=="")
		{
			$sql="update users set name='$name',surname='$surname',alias='$alias',url='$url' where userid=$userid";
		}
		else
		{
			$passwd=md5($passwd);
			$sql="update users set name='$name',surname='$surname',alias='$alias',passwd='$passwd',url='$url' where userid=$userid";
		}
		return DBexecute($sql);
	}

	# Add permission

	function	add_permission($userid,$right,$permission,$id)
	{
		$sql="insert into rights (userid,name,permission,id) values ($userid,'$right','$permission',$id)";
		return DBexecute($sql);
	}

	function	get_user_by_userid($userid)
	{
		global	$ERROR_MSG;

		$sql="select * from users where userid=$userid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No user with itemid=[$userid]";
		}
		return	$result;
	}
?>
