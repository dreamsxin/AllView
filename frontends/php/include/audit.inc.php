<?php
/*
** Allview
** Copyright (C) 2000,2001,2002,2003,2004 Allview team 
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**/
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
