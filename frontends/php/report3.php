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
	$page["title"] = "IT Services availability report";
	$page["file"] = "report3.php";
	show_header($page["title"],0,0);
?>

<?php
//	if(!check_right("Host","R",0))
//	{
//		show_table_header("<font color=\"AA0000\">No permissions !</font>");
//		show_footer();
//		exit;
//	}
?>

<?php
	if(!isset($_GET["serviceid"]))
	{
		show_table_header("<font color=\"AA0000\">Undefined serviceid !</font>");
		show_footer();
		exit;
	}
	$service=get_service_by_serviceid($_GET["serviceid"]);
?>

<?php
	show_table_header_begin();
	echo "IT SERVICES AVAILABILITY REPORT";
	echo "<br>";
	echo "<a href=\"srv_status.php?serviceid=".$service["serviceid"]."\">",$service["name"],"</a>";;

	show_table_v_delimiter();

	$result=DBselect("select h.hostid,h.host from hosts h,items i where h.status=".HOST_STATUS_MONITORED." and h.hostid=i.hostid group by h.hostid,h.host order by h.host");

	$year=date("Y");
	for($year=date("Y")-2;$year<=date("Y");$year++)
	{
		if( isset($_GET["year"]) && ($_GET["year"] == $year) )
		{
			echo "<b>[";
		}
		echo "<a href='report3.php?serviceid=".$_GET["serviceid"]."&year=$year'>".$year."</a>";
		if(isset($_GET["year"]) && ($_GET["year"] == $year) )
		{
			echo "]</b>";
		}
		echo " ";
	}

	show_table_header_end();
?>

<?php


	echo "<br>";
	table_begin();
	table_header(array(S_FROM,S_TILL,S_OK,S_PROBLEMS,S_PERCENTAGE,S_SLA));
	$col=0;
	$year=date("Y");
	for($year=date("Y")-2;$year<=date("Y");$year++)
	{
		if( isset($_GET["year"]) && ($_GET["year"] != $year) )
		{
			continue;
		}
		$start=mktime(0,0,0,1,1,$year);

		$wday=date("w",$start);
		if($wday==0) $wday=7;
		$start=$start-($wday-1)*24*3600;

		for($i=0;$i<53;$i++)
		{
			$period_start=$start+7*24*3600*$i;
			$period_end=$start+7*24*3600*($i+1);
			if($period_start>time())
			{
				break;
			}
			$stat=calculate_service_availability($service["serviceid"],$period_start,$period_end);

			$from=date(S_DATE_FORMAT_YMD,$period_start);
			$till=date(S_DATE_FORMAT_YMD,$period_end);
	
			$t=sprintf("%2.2f%%",$stat["problem"]);
			$t_time=sprintf("%dd %dh %dm",$stat["problem_time"]/(24*3600),($stat["problem_time"]%(24*3600))/3600,($stat["problem_time"]%(3600))/(60));
			$f=sprintf("%2.2f%%",$stat["ok"]);
			$f_time=sprintf("%dd %dh %dm",$stat["ok_time"]/(24*3600),($stat["ok_time"]%(24*3600))/3600,($stat["ok_time"]%(3600))/(60));

			$ok=array("value"=>$f_time,"class"=>"off");
			$problems=array("value"=>$t_time,"class"=>"on");
			$percentage=array("value"=>$f,"class"=>"off");

			if($service["showsla"]==1)
			{
				if($stat["ok"]>=$service["goodsla"])
				{
					$sla=array("value"=>$service["goodsla"],"class"=>"off");
				}
				else
				{
					$sla=array("value"=>$service["goodsla"],"class"=>"on");
				}
			}
			else
			{
				$sla="-";
			}
		
			table_row(array(
				$from,
				$till,
				$ok,
				$problems,
				$percentage,
				$sla
				),$col++);
		}
	}
	table_end();

	show_footer();
?>
