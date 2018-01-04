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
	function add_audit($action,$resource,$details)
	{
		global $USER_DETAILS;

		$details=addslashes($details);
		$userid=$USER_DETAILS["userid"];
		$clock=time();
		$sql="insert into audit (userid,clock,action,resource,details) values ($userid,$clock,$action,$resource,'$details')";
		return DBexecute($sql);
	}
?>
