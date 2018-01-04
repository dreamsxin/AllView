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
	# Add Graph

	function	add_graph($name,$width,$height,$yaxistype,$yaxismin,$yaxismax)
	{
		global	$ERROR_MSG;

		if(!check_right("Graph","A",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

		$sql="insert into graphs (name,width,height,yaxistype,yaxismin,yaxismax) values ('$name',$width,$height,$yaxistype,$yaxismin,$yaxismax)";
		$result=DBexecute($sql);
		return DBinsert_id($result,"graphs","graphid");
	}

	function	update_graph_item($gitemid,$itemid,$color,$drawtype,$sortorder)
	{
		$sql="update graphs_items set itemid=$itemid,color='$color',drawtype=$drawtype,sortorder=$sortorder where gitemid=$gitemid";
		return	DBexecute($sql);
	}

	function	add_item_to_graph($graphid,$itemid,$color,$drawtype,$sortorder)
	{
		$sql="insert into graphs_items (graphid,itemid,color,drawtype,sortorder) values ($graphid,$itemid,'$color',$drawtype,$sortorder)";
		return	DBexecute($sql);
	}

	# Update Graph

	function	update_graph($graphid,$name,$width,$height,$yaxistype,$yaxismin,$yaxismax)
	{
		global	$ERROR_MSG;

		if(!check_right("Graph","U",0))
		{
			$ERROR_MSG="Insufficient permissions";
			return 0;
		}

		$sql="update graphs set name='$name',width=$width,height=$height,yaxistype=$yaxistype,yaxismin=$yaxismin,yaxismax=$yaxismax where graphid=$graphid";
		return	DBexecute($sql);
	}

	function	delete_graphs_item($gitemid)
	{
		$sql="delete from graphs_items where gitemid=$gitemid";
		return	DBexecute($sql);
	}

	# Delete Graph

	function	delete_graph($graphid)
	{
		$sql="delete from graphs_items where graphid=$graphid";
		$result=DBexecute($sql);
		if(!$result)
		{
			return	$result;
		}
		$sql="delete from graphs where graphid=$graphid";
		return	DBexecute($sql);
	}

	function	get_graphitem_by_gitemid($gitemid)
	{
		global	$ERROR_MSG;

		$sql="select * from graphs_items where gitemid=$gitemid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No graph item with gitemid=[$gitemid]";
		}
		return	$result;
	}

	function	get_graph_by_graphid($graphid)
	{
		global	$ERROR_MSG;

		$sql="select * from graphs where graphid=$graphid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No graph with graphid=[$graphid]";
		}
		return	$result;
	}
?>
