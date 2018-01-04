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
	include "include/config.inc.php";
	include "include/classes.inc.php";

	$graph=new Graph();
	if(isset($_GET["period"]))
	{
		$graph->setPeriod($_GET["period"]);
	}
	if(isset($_GET["from"]))
	{
		$graph->setFrom($_GET["from"]);
	}
	if(isset($_GET["stime"]))
	{
		$graph->setSTime($_GET["stime"]);
	}
	if(isset($_GET["border"]))
	{
		$graph->setBorder(0);
	}

	$result=DBselect("select name,width,height,yaxistype,yaxismin,yaxismax from graphs where graphid=".$_GET["graphid"]);

	$name=DBget_field($result,0,0);
	if(isset($_GET["width"])&&$_GET["width"]>0)
	{
		$width=$_GET["width"];
	}
	else
	{
		$width=DBget_field($result,0,1);
	}
	if(isset($_GET["height"])&&$_GET["height"]>0)
	{
		$height=$_GET["height"];
	}
	else
	{
		$height=DBget_field($result,0,2);
	}

	$graph->setWidth($width);
	$graph->setHeight($height);
	$graph->setHeader(DBget_field($result,0,0));
	$graph->setYAxisType(DBget_field($result,0,3));
	$graph->setYAxisMin(DBget_field($result,0,4));
	$graph->setYAxisMax(DBget_field($result,0,5));

	$result=DBselect("select gi.itemid,i.description,gi.color,h.host,gi.drawtype from graphs_items gi,items i,hosts h where gi.itemid=i.itemid and gi.graphid=".$_GET["graphid"]." and i.hostid=h.hostid order by gi.sortorder");

	for($i=0;$i<DBnum_rows($result);$i++)
	{
		$graph->addItem(DBget_field($result,$i,0));
		$graph->setColor(DBget_field($result,$i,0), DBget_field($result,$i,2));
		$graph->setDrawtype(DBget_field($result,$i,0), DBget_field($result,$i,4));
	}

	$graph->Draw();
?>

