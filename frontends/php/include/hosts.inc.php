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
	# Add Host definition

	function	add_host($host,$port,$status,$useip,$ip,$host_templateid,$newgroup,$groups)
	{
		global	$ERROR_MSG;

		if(!check_right("Host","A",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

 		if (!eregi('^([0-9a-zA-Z\_\.-]+)$', $host, &$arr)) 
		{
			$ERROR_MSG="Hostname should contain 0-9a-zA-Z_.- characters only";
			return 0;
		}

		$sql="select * from hosts where host='$host'";
		$result=DBexecute($sql);
		if(DBnum_rows($result)>0)
		{
			$ERROR_MSG="Host '$host' already exists";
			return 0;
		}

		if( isset($useip) && ($useip=="on") )
		{
			$useip=1;
		}
		else
		{
			$useip=0;
		}


		$sql="insert into hosts (host,port,status,useip,ip,disable_until,available) values ('$host',$port,$status,$useip,'$ip',0,".HOST_AVAILABLE_UNKNOWN.")";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		
		$hostid=DBinsert_id($result,"hosts","hostid");

		if($host_templateid != 0)
		{
			add_templates_to_host($hostid,$host_templateid);
//			$result=add_using_host_template($hostid,$host_templateid);
			sync_host_with_templates($hostid);
		}
		update_host_groups($hostid,$groups);
		if($newgroup != "")
		{
			add_group_to_host($hostid,$newgroup);
		}

		update_profile("HOST_PORT",$port);
		
		return	$result;
	}

	function	update_host($hostid,$host,$port,$status,$useip,$ip,$newgroup,$groups)
	{
		global	$ERROR_MSG;

		if(!check_right("Host","U",$hostid))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

 		if (!eregi('^([0-9a-zA-Z\_\.-]+)$', $host, &$arr)) 
		{
			$ERROR_MSG="Hostname should contain 0-9a-zA-Z_.- characters only";
			return 0;
		}

		$sql="select * from hosts where host='$host' and hostid<>$hostid";
		$result=DBexecute($sql);
		if(DBnum_rows($result)>0)
		{
			$ERROR_MSG="Host '$host' already exists";
			return 0;
		}


		if($useip=="on")
		{
			$useip=1;
		}
		else
		{
			$useip=0;
		}
		$sql="update hosts set host='$host',port=$port,useip=$useip,ip='$ip' where hostid=$hostid";
		$result=DBexecute($sql);


		update_host_status($hostid, $status);
		update_host_groups($hostid,$groups);
		if($newgroup != "")
		{
			add_group_to_host($hostid,$newgroup);
		}
		return	$result;
	}

	# Add templates linked to template host to the host

	function	add_templates_to_host($hostid,$host_templateid)
	{
		$sql="select * from hosts_templates where hostid=$host_templateid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			add_template_linkage($hostid,$row["templateid"],$row["items"],$row["triggers"],$row["actions"],
						$row["graphs"],$row["screens"]);
		}
	}

	function	delete_groups_by_hostid($hostid)
	{
		$sql="select groupid from hosts_groups where hostid=$hostid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$sql="delete from hosts_groups where hostid=$hostid and groupid=".$row["groupid"];
			DBexecute($sql);
			$sql="select count(*) as count from hosts_groups where groupid=".$row["groupid"];
			$result2=DBselect($sql);
			$row2=DBfetch($result2);
			if($row2["count"]==0)
			{
				$sql="delete from groups where groupid=".$row["groupid"];
				DBexecute($sql);
			}
		}
	}

	# Delete Host

	function	delete_host($hostid)
	{
		global $DB_TYPE;

		if($DB_TYPE=="MYSQL")
		{
			$sql="update hosts set status=".HOST_STATUS_DELETED.",host=concat(host,\" [DELETED]\") where hostid=$hostid";
		}
		else
		{
			$sql="update hosts set status=".HOST_STATUS_DELETED.",host=host||' [DELETED]' where hostid=$hostid";
		}
		return	DBexecute($sql);
	}

	function	delete_host_group($groupid)
	{
		global	$ERROR_MSG;

		$sql="delete from hosts_groups where groupid=$groupid";
		DBexecute($sql);
		$sql="delete from groups where groupid=$groupid";
		return DBexecute($sql);
	}

	function	get_host_by_hostid($hostid)
	{
		global	$ERROR_MSG;

		$sql="select hostid,host,useip,ip,port,status from hosts where hostid=$hostid";
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			$host["hostid"]=DBget_field($result,0,0);
			$host["host"]=DBget_field($result,0,1);
			$host["useip"]=DBget_field($result,0,2);
			$host["ip"]=DBget_field($result,0,3);
			$host["port"]=DBget_field($result,0,4);
			$host["status"]=DBget_field($result,0,5);
		}
		else
		{
			$ERROR_MSG="No host with hostid=[$hostid]";
		}
		return	$host;
	}
?>
