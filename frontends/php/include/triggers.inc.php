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
	function	update_trigger_comments($triggerid,$comments)
	{
		global	$ERROR_MSG;

		if(!check_right("Trigger comment","U",$triggerid))
		{
			$ERROR_MSG="Insufficient permissions";
			return	0;
		}

		$comments=addslashes($comments);
		$sql="update triggers set comments='$comments' where triggerid=$triggerid";
		return	DBexecute($sql);
	}

	# Update Trigger status

	function	update_trigger_status($triggerid,$status)
	{
		global	$ERROR_MSG;

		if(!check_right_on_trigger("U",$triggerid))
		{
                        $ERROR_MSG="Insufficient permissions";
                        return 0;
		}
		add_alarm($triggerid,2);
		$sql="update triggers set status=$status where triggerid=$triggerid";
		return	DBexecute($sql);
	}

	# "Processor load on %s is 5" to "Processor load on www.sf.net is 5"
	function	expand_trigger_description_simple($triggerid)
	{
		$sql="select distinct t.description,h.host from triggers t,functions f,items i,hosts h where t.triggerid=$triggerid and f.triggerid=t.triggerid and f.itemid=i.itemid and i.hostid=h.hostid";
//		echo $sql;
		$result=DBselect($sql);
		$row=DBfetch($result);

//		$description=str_replace("%s",$row["host"],$row["description"]);

		$search=array("{HOSTNAME}");
		$replace=array($row["host"]);
//		$description = str_replace($search, $replace,$row["description"]);
		$description = str_replace("{HOSTNAME}", $row["host"],$row["description"]);

		return $description;
	}

	# "Processor load on %s is 5" to "Processor load on www.sf.net is 5"
	function	expand_trigger_description($triggerid)
	{
		$description=expand_trigger_description_simple($triggerid);
		$description=stripslashes(htmlspecialchars($description));

		return $description;
	}

	function	update_trigger_value_to_unknown_by_hostid($hostid)
	{
		$sql="select distinct t.triggerid from hosts h,items i,triggers t,functions f where f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and h.hostid=$hostid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			$sql="update triggers set value=2 where triggerid=".$row["triggerid"];
			DBexecute($sql);
		}
	}

	function	add_trigger_dependency($triggerid,$depid)
	{
		$result=insert_dependency($triggerid,$depid);;
		if(!$result)
		{
			return $result;
		}
		add_additional_dependencies($triggerid,$depid);
		return $result;
	}

	# Add Trigger definition

	function	add_trigger($expression,$description,$priority,$status,$comments,$url)
	{
		global	$ERROR_MSG;

//		if(!check_right("Trigger","A",0))
//		{
//			$ERROR_MSG="Insufficient permissions";
//			return	0;
//		}

#		$description=addslashes($description);
		$sql="insert into triggers  (description,priority,status,comments,url,value) values ('$description',$priority,$status,'$comments','$url',2)";
#		echo $sql,"<br>";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
 
		$triggerid=DBinsert_id($result,"triggers","triggerid");
#		echo $triggerid,"<br>";
		add_alarm($triggerid,2);
 
		$expression=implode_exp($expression,$triggerid);
		$sql="update triggers set expression='$expression' where triggerid=$triggerid";
#		echo $sql,"<br>";
		DBexecute($sql);
		reset_items_nextcheck($triggerid);
		return $triggerid;
	}

	# Delete Trigger definition

	function	delete_trigger($triggerid)
	{
		global	$ERROR_MSG;

		$sql="select count(*) from trigger_depends where triggerid_down=$triggerid or triggerid_up=$triggerid";
		$result=DBexecute($sql);
		if(DBget_field($result,0,0)>0)
		{
			$ERROR_MSG="Delete dependencies first";
			return	FALSE;
		}

		$result=delete_function_by_triggerid($triggerid);
		if(!$result)
		{
			return	$result;
		}
		$result=delete_alarms_by_triggerid($triggerid);
		if(!$result)
		{
			return	$result;
		}
		$result=delete_actions_by_triggerid($triggerid);
		if(!$result)
		{
			return	$result;
		}
		$result=delete_services_by_triggerid($triggerid);
		if(!$result)
		{
			return	$result;
		}

		$sql="update sysmaps_links set triggerid=NULL where triggerid=$triggerid";
		DBexecute($sql);

		$sql="delete from triggers where triggerid=$triggerid";
		return	DBexecute($sql);
	}

	# Update Trigger definition

	function	update_trigger($triggerid,$expression,$description,$priority,$status,$comments,$url)
	{
		global	$ERROR_MSG;

		if(!check_right_on_trigger("U",$triggerid))
		{
                        $ERROR_MSG="Insufficient permissions";
                        return 0;
		}

		$result=delete_function_by_triggerid($triggerid);
		if(!$result)
		{
			return	$result;
		}

		$expression=implode_exp($expression,$triggerid);
		add_alarm($triggerid,2);
//		$sql="update triggers set expression='$expression',description='$description',priority=$priority,status=$status,comments='$comments',url='$url' where triggerid=$triggerid";
		reset_items_nextcheck($triggerid);
		$sql="update triggers set expression='$expression',description='$description',priority=$priority,status=$status,comments='$comments',url='$url',value=2 where triggerid=$triggerid";
		return	DBexecute($sql);
	}

	function	check_right_on_trigger($permission,$triggerid)
	{
                $sql="select distinct h.hostid from functions f,items i,hosts h
where h.hostid=i.hostid and i.itemid=f.itemid and f.triggerid=$triggerid";
                $result=DBselect($sql);
                $ok=0;
		while($row=DBfetch($result))
		{
			if(check_right("Host",$permission,$row["hostid"]))
			{
				$ok=1;
			}
		}
		return	$ok;
	}

	function	get_trigger_by_triggerid($triggerid)
	{
		global	$ERROR_MSG;

		$sql="select triggerid,expression,description,status,priority,lastchange,dep_level,comments,url,value from triggers where triggerid=$triggerid";
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No trigger with triggerid=[$triggerid]";
		}
		return	$trigger;
	}

	function	delete_trigger_dependency($triggerid_down,$triggerid_up)
	{
// Why this was here?
//		$sql="select count(*) from trigger_depends where triggerid_down=$triggerid_up and triggerid_up=$triggerid_down";
//		$result=DBexecute($sql);
//		if(DBget_field($result,0,0)>0)
//		{
//			return	FALSE;
//		}

		$sql="select triggerid_down,triggerid_up from trigger_depends where triggerid_up=$triggerid_up and triggerid_down=$triggerid_down";
		$result=DBexecute($sql);
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$down=DBget_field($result,$i,0);
			$up=DBget_field($result,$i,1);
			$sql="update triggers set dep_level=dep_level-1 where triggerid=$up";
			DBexecute($sql);
		}

		$sql="delete from trigger_depends where triggerid_up=$triggerid_up and triggerid_down=$triggerid_down";
		DBexecute($sql);

		return	TRUE;
	}

	function	insert_dependency($triggerid_down,$triggerid_up)
	{
		$sql="insert into trigger_depends (triggerid_down,triggerid_up) values ($triggerid_down,$triggerid_up)";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		$sql="update triggers set dep_level=dep_level+1 where triggerid=$triggerid_up";
		$result=DBexecute($sql);
		return	$result;
	}

	// If 1 depends on 2, and 2 depends on 3, then add dependency 1->3
	function	add_additional_dependencies($triggerid_down,$triggerid_up)
	{
		$sql="select triggerid_down from trigger_depends where triggerid_up=$triggerid_down";
		$result=DBselect($sql);
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$triggerid=DBget_field($result,$i,0);
			insert_dependency($triggerid,$triggerid_up);
			add_additional_dependencies($triggerid,$triggerid_up);
		}
		$sql="select triggerid_up from trigger_depends where triggerid_down=$triggerid_up";
		$result=DBselect($sql);
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$triggerid=DBget_field($result,$i,0);
			insert_dependency($triggerid_down,$triggerid);
			add_additional_dependencies($triggerid_down,$triggerid);
		}
	}

	function	delete_function_by_triggerid($triggerid)
	{
		$sql="delete from functions where triggerid=$triggerid";
		return	DBexecute($sql);
	}

	function	delete_actions_by_triggerid($triggerid)
	{
		$sql="delete from actions where triggerid=$triggerid and scope=0";
		return	DBexecute($sql);
	}

	function	delete_alarms_by_triggerid($triggerid)
	{
		$sql="delete from alarms where triggerid=$triggerid";
		return	DBexecute($sql);
	}

	function	delete_triggers_by_itemid($itemid)
	{
		$sql="select triggerid from functions where itemid=$itemid";
		$result=DBselect($sql);
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			if(!delete_trigger(DBget_field($result,$i,0)))
			{
				return FALSE;
			}
		}
		$sql="delete from functions where itemid=$itemid";
		return	DBexecute($sql);
	}

	# Delete Service definitions by triggerid

	function	delete_services_by_triggerid($triggerid)
	{
		$sql="select serviceid from services where triggerid=$triggerid";
		$result=DBselect($sql);
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			delete_service(DBget_field($result,$i,0));
		}
		return	TRUE;
	}

	# Add item to hardlinked hosts

	function	add_trigger_to_templates($triggerid)
	{
		if($triggerid<=0)
		{
			return;
		}

		$trigger=get_trigger_by_triggerid($triggerid);

		$sql="select distinct h.hostid from hosts h,functions f, items i where i.itemid=f.itemid and h.hostid=i.hostid and f.triggerid=$triggerid";
		$result=DBselect($sql);
		if(DBnum_rows($result)!=1)
		{
			return;
		}

		$row=DBfetch($result);

		$hostid=$row["hostid"];

		$sql="select hostid,templateid,items from hosts_templates where templateid=$hostid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			if($row["triggers"]&1 == 0)	continue;

			$sql="insert into triggers  (description,priority,status,comments,url,value,expression) values ('".addslashes($trigger["description"])."',".$trigger["priority"].",".$trigger["status"].",'".addslashes($trigger["comments"])."','".addslashes($trigger["url"])."',2,'".$trigger["expression"]."')";
			$result4=DBexecute($sql);
			$triggerid_new=DBinsert_id($result4,"triggers","triggerid");


			$sql="select i.key_,f.parameter,f.function,f.functionid from functions f,items i where i.itemid=f.itemid and f.triggerid=$triggerid";
			$result2=DBselect($sql);
			while($row2=DBfetch($result2))
			{
				$sql="select itemid from items where key_=\"".$row2["key_"]."\" and hostid=".$row["hostid"];
				$result3=DBselect($sql);
				if(DBnum_rows($result3)!=1)
				{
					$sql="delete from triggers where triggerid=$triggerid_new";
					DBexecute($sql);
					$sql="delete from functions where triggerid=$triggerid_new";
					DBexecute($sql);
					break;
				}
				$row3=DBfetch($result3);

				$item=get_item_by_itemid($row3["itemid"]);

				$sql="insert into functions (itemid,triggerid,function,parameter) values (".$item["itemid"].",$triggerid_new,'".$row2["function"]."','".$row2["parameter"]."')";
				$result4=DBexecute($sql);
				$functionid=DBinsert_id($result4,"functions","functionid");

				$sql="update triggers set expression='".$trigger["expression"]."' where triggerid=$triggerid_new";
				DBexecute($sql);
				$expression=str_replace("{".$row2["functionid"]."}","{".$functionid."}",$trigger["expression"]);
				$sql="update triggers set expression='$expression' where triggerid=$triggerid_new";
				DBexecute($sql);
			}
		}
	}

	function	delete_trigger_from_templates($triggerid)
	{
		if($triggerid<=0)
		{
			return;
		}

		$trigger=get_trigger_by_triggerid($triggerid);

		$sql="select distinct h.hostid from hosts h,functions f, items i where i.itemid=f.itemid and h.hostid=i.hostid and f.triggerid=$triggerid";
		$result=DBselect($sql);
		if(DBnum_rows($result)!=1)
		{
			return;
		}

		$row=DBfetch($result);

		$hostid=$row["hostid"];

		$sql="select hostid,templateid,triggers from hosts_templates where templateid=$hostid";
		$result=DBselect($sql);
		while($row=DBfetch($result))
		{
			if($row["triggers"]&4 == 0)	continue;

			$sql="select distinct f.triggerid from functions f,items i,triggers t where t.description='".addslashes($trigger["description"])."' and t.triggerid=f.triggerid and i.itemid=f.itemid and i.hostid=".$row["hostid"];
			$result2=DBselect($sql);
			while($row2=DBfetch($result2))
			{
				delete_trigger($row2["triggerid"]);
			}

		}
	}
?>
