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
	function	add_service($serviceid,$name,$triggerid,$linktrigger,$algorithm,$showsla,$goodsla,$sortorder)
	{
		if( isset($showsla)&&($showsla=="on") )
		{
			$showsla=1;
		}
		else
		{
			$showsla=0;
		}
		if( isset($linktrigger)&&($linktrigger=="on") )
		{
//			$trigger=get_trigger_by_triggerid($triggerid);
//			$description=$trigger["description"];
//			if( strstr($description,"%s"))
//			{
				$description=expand_trigger_description($triggerid);
//			}
			$description=addslashes($description);
			$sql="insert into services (name,triggerid,status,algorithm,showsla,goodsla,sortorder) values ('$description',$triggerid,0,$algorithm,$showsla,$goodsla,$sortorder)";
		}
		else
		{
			$sql="insert into services (name,status,algorithm,showsla,goodsla,sortorder) values ('$name',0,$algorithm,$showsla,$goodsla,$sortorder)";
		}
		$result=DBexecute($sql);
		if(!$result)
		{
			return FALSE;
		}
		$id=DBinsert_id($result,"services","serviceid");
		if(isset($serviceid))
		{
			add_service_link($id,$serviceid,0);
		}
		return $id;
	}

	function	add_host_to_services($hostid,$serviceid)
	{
		$sql="select distinct t.triggerid,t.description from triggers t,hosts h,items i,functions f where h.hostid=$hostid and h.hostid=i.hostid and i.itemid=f.itemid and f.triggerid=t.triggerid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$serviceid2=add_service($serviceid,$row["description"],$row["triggerid"],"on",0,"off",99,0);
//			add_service_link($serviceid2,$serviceid,0);
		}
		return	1;
	}

	function	is_service_hardlinked($serviceid)
	{
		$sql="select count(*) from services_links where servicedownid=$serviceid and soft=0";
		$result=DBselect($sql);
		if(DBget_field($result,0,0)>0)
		{
			return	TRUE;
		}
		return	FALSE;
	}

	function	delete_service_link($linkid)
	{
		$sql="delete from services_links where linkid=$linkid";
		return DBexecute($sql);
	}

	function	delete_service($serviceid)
	{
		$sql="delete from services_links where servicedownid=$serviceid or serviceupid=$serviceid";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		$sql="delete from services where serviceid=$serviceid";
		return DBexecute($sql);
	}

	# Return TRUE if triggerid is a reason why the service is not OK
	# Warning: recursive function
	function	does_service_depend_on_the_service($serviceid,$serviceid2)
	{
#		echo "Serviceid:$serviceid Triggerid:$serviceid2<br>";
		$service=get_service_by_serviceid($serviceid);
#		echo "Service status:".$service["status"]."<br>";
		if($service["status"]==0)
		{
			return	FALSE;
		}
		if($serviceid==$serviceid2)
		{
			if($service["status"]>0)
			{
				return TRUE;
			}
			
		}

		$sql="select serviceupid from services_links where servicedownid=$serviceid2 and soft=0";
#		echo $sql."<br>";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			if(does_service_depend_on_the_service($serviceid,$row["serviceupid"]) == TRUE)
			{
				return	TRUE;
			}
		}
		return	FALSE;
	}

	function	service_has_parent($serviceid)
	{
		$sql="select count(*) from services_links where servicedownid=$serviceid";
		$result=DBselect($sql);
		if(DBget_field($result,0,0)>0)
		{
			return	TRUE;
		}
		return	FALSE;
	}

	function	service_has_no_this_parent($parentid,$serviceid)
	{
		$sql="select count(*) from services_links where serviceupid=$parentid and servicedownid=$serviceid";
		$result=DBselect($sql);
		if(DBget_field($result,0,0)>0)
		{
			return	FALSE;
		}
		return	TRUE;
	}

	function	update_service($serviceid,$name,$triggerid,$linktrigger,$algorithm,$showsla,$goodsla,$sortorder)
	{
		if( isset($linktrigger)&&($linktrigger=="on") )
		{
			// No mistake here
			$triggerid=$triggerid;
		}
		else
		{
			$triggerid='NULL';
		}
		if( isset($showsla)&&($showsla=="on") )
		{
			$showsla=1;
		}
		else
		{
			$showsla=0;
		}
		$sql="update services set name='$name',triggerid=$triggerid,status=0,algorithm=$algorithm,showsla=$showsla,goodsla=$goodsla,sortorder=$sortorder where serviceid=$serviceid";
		return	DBexecute($sql);
	}

	function	add_service_link($servicedownid,$serviceupid,$softlink)
	{
		global	$ERROR_MSG;

		if( ($softlink==0) && (is_service_hardlinked($servicedownid)==TRUE) )
		{
			return	FALSE;
		}

		if($servicedownid==$serviceupid)
		{
			$ERROR_MSG="Cannot link service to itself.";
			return	FALSE;
		}

		$sql="insert into services_links (servicedownid,serviceupid,soft) values ($servicedownid,$serviceupid,$softlink)";
		return	DBexecute($sql);
	}

	function	get_last_service_value($serviceid,$clock)
	{
	       	$sql="select count(*),max(clock) from service_alarms where serviceid=$serviceid and clock<=$clock";
//		echo " $sql<br>";
		
	        $result=DBselect($sql);
		if(DBget_field($result,0,0)>0)
		{
	       		$sql="select value from service_alarms where serviceid=$serviceid and clock=".DBget_field($result,0,1);
		        $result2=DBselect($sql);
// Assuring that we get very latest service value. There could be several with the same timestamp
//			$value=DBget_field($result2,0,0);
			for($i=0;$i<DBnum_rows($result2);$i++)
			{
				$value=DBget_field($result2,$i,0);
			}
		}
		else
		{
			$value=0;
		}
		return $value;
	}

	function	calculate_service_availability($serviceid,$period_start,$period_end)
	{
	       	$sql="select count(*),min(clock),max(clock) from service_alarms where serviceid=$serviceid and clock>=$period_start and clock<=$period_end";
		
		$sql="select clock,value from service_alarms where serviceid=$serviceid and clock>=$period_start and clock<=$period_end";
		$result=DBselect($sql);

// -1,0,1
		$state=get_last_service_value($serviceid,$period_start);
		$problem_time=0;
		$ok_time=0;
		$time=$period_start;
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$clock=DBget_field($result,$i,0);
			$value=DBget_field($result,$i,1);

			$diff=$clock-$time;

			$time=$clock;
#state=0,1 (OK), >1 PROBLEMS 

			if($state<=1)
			{
				$ok_time+=$diff;
				$state=$value;
			}
			else
			{
				$problem_time+=$diff;
				$state=$value;
			}
		}
//		echo $problem_time,"-",$ok_time,"<br>";

		if(DBnum_rows($result)==0)
		{
			if(get_last_service_value($serviceid,$period_start)<=1)
			{
				$ok_time=$period_end-$period_start;
			}
			else
			{
				$problem_time=$period_end-$period_start;
			}
		}
		else
		{
			if($state<=1)
			{
				$ok_time=$ok_time+$period_end-$time;
			}
			else
			{
				$problem_time=$problem_time+$period_end-$time;
			}
		}

//		echo $problem_time,"-",$ok_time,"<br>";

		$total_time=$problem_time+$ok_time;
		if($total_time==0)
		{
			$ret["problem_time"]=0;
			$ret["ok_time"]=0;
			$ret["problem"]=0;
			$ret["ok"]=0;
		}
		else
		{
			$ret["problem_time"]=$problem_time;
			$ret["ok_time"]=$ok_time;
			$ret["problem"]=(100*$problem_time)/$total_time;
			$ret["ok"]=(100*$ok_time)/$total_time;
		}
		return $ret;
	}

	function	get_service_status_description($status)
	{
		$desc="<font color=\"#00AA00\">OK</a>";
		if($status==5)
		{
			$desc="<font color=\"#FF0000\">Disaster</a>";
		}
		elseif($status==4)
		{
			$desc="<font color=\"#FF8888\">Serious&nbsp;problem</a>";
		}
		elseif($status==3)
		{
			$desc="<font color=\"#AA0000\">Average&nbsp;problem</a>";
		}
		elseif($status==2)
		{
			$desc="<font color=\"#AA5555\">Minor&nbsp;problem</a>";
		}
		elseif($status==1)
		{
			$desc="<font color=\"#00AA00\">OK</a>";
		}
		return $desc;
	}

	function	get_num_of_service_childs($serviceid)
	{
		global	$ERROR_MSG;

		$sql="select count(*) from services_links where serviceupid=$serviceid";
		$result=DBselect($sql);
		return	DBget_field($result,0,0);
	}

	function	get_service_by_serviceid($serviceid)
	{
		global	$ERROR_MSG;

		$sql="select * from services where serviceid=$serviceid";
		$result=DBselect($sql);
		if(Dbnum_rows($result) == 1)
		{
			return	DBfetch($result);
		}
		else
		{
			$ERROR_MSG="No service with serviceid=[$serviceid]";
		}
		return	FALSE;
	}
?>
