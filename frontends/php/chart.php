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
	if(isset($_GET["width"]))
	{
		$graph->setWidth($_GET["width"]);
	}
	if(isset($_GET["height"]))
	{
		$graph->setHeight($_GET["height"]);
	}
	if(isset($_GET["border"]))
	{
		$graph->setBorder(0);
	}
	$graph->addItem($_GET["itemid"]);

	$graph->Draw();
?>

