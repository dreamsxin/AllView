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
	$page["title"] = S_IT_SERVICES;
	$page["file"] = "srv_status.php";
	show_header($page["title"],30,0);
?>
 
<?php
	show_table_header(S_IT_SERVICES_BIG);

	if(isset($_GET["serviceid"])&&isset($_GET["showgraph"]))
	{
		echo "<TABLE BORDER=0 COLS=4 align=center WIDTH=100% BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=3>";
		echo "<TR BGCOLOR=#EEEEEE>";
		echo "<TR BGCOLOR=#DDDDDD>";
		echo "<TD ALIGN=CENTER>";
		echo "<IMG SRC=\"chart5.php?serviceid=".$_GET["serviceid"]."\" border=0>";
		echo "</TD>";
		echo "</TR>";
		echo "</TABLE>";
		show_footer();
		exit;
	}

	$now=time();
	$result=DBselect("select serviceid,name,triggerid,status,showsla,goodsla from services order by sortorder,name");
	table_begin();
	table_header(array(S_SERVICE,S_STATUS,S_REASON,S_SLA_LAST_7_DAYS,nbsp(S_PLANNED_CURRENT_SLA),S_GRAPH));
	$col=0;
	if(isset($_GET["serviceid"]))
	{
		echo "<tr bgcolor=#EEEEEE>";
		$service=get_service_by_serviceid($_GET["serviceid"]);
		echo "<td><b><a href=\"srv_status.php?serviceid=".$service["serviceid"]."\">".$service["name"]."</a></b></td>";
		echo "<td>".get_service_status_description($service["status"])."</td>";
		echo "<td>&nbsp;</td>";
		if($service["showsla"]==1)
		{
			echo "<td><img src=\"chart_sla.php?serviceid=".$service["serviceid"]."\"></td>";
		}
		else
		{
			echo "<td>-</td>";
		}
		if($service["showsla"]==1)
		{
			$now=time(NULL);
			$period_start=$now-7*24*3600;
			$period_end=$now;
			$stat=calculate_service_availability($service["serviceid"],$period_start,$period_end);

			if($service["goodsla"]>$stat["ok"])
			{
				$color="AA0000";
			}
			else
			{
				$color="00AA00";
			}
			printf ("<td><font color=\"00AA00\">%.2f%%</font><b>/</b><font color=\"%s\">%.2f%%</font></td>",$service["goodsla"],$color,$stat["ok"]);
		}
		else
		{
			echo "<td>-</td>";
		}
		echo "<td><a href=\"srv_status.php?serviceid=".$service["serviceid"]."&showgraph=1\">Show</a></td>";
		echo "</tr>"; 
		$col++;
	}
	while($row=DBfetch($result))
	{
		if(!isset($_GET["serviceid"]) && service_has_parent($row["serviceid"]))
		{
			continue;
		}
		if(isset($_GET["serviceid"]) && service_has_no_this_parent($_GET["serviceid"],$row["serviceid"]))
		{
			continue;
		}
		if(isset($row["triggerid"])&&!check_right_on_trigger("R",$row["triggerid"]))
		{
			continue;
		}
		if(isset($_GET["serviceid"])&&($_GET["serviceid"]==$row["serviceid"]))
		{
			echo "<tr bgcolor=#99AABB>";
		}
		else
		{
			if($col++%2==0)	{ echo "<tr bgcolor=#EEEEEE>"; }
			else		{ echo "<tr bgcolor=#DDDDDD>"; }
		}
		$childs=get_num_of_service_childs($row["serviceid"]);
		if(isset($row["triggerid"]))
		{
			$description=nbsp(expand_trigger_description($row["triggerid"]));
			$description="[<a href=\"alarms.php?triggerid=".$row["triggerid"]."\">".S_TRIGGER_BIG."</a>] $description";
		}
		else
		{
			$trigger_link="";
			$description=$row["name"];
		}
		if(isset($_GET["serviceid"]))
		{
			if($childs == 0)
			{
				echo "<td> - $description</td>";
			}
			else
			{
				echo "<td> - <a href=\"srv_status.php?serviceid=".$row["serviceid"]."\">$description</a></td>";
			}
		}
		else
		{
			if($childs == 0)
			{
				echo "<td>$description</td>";
			}
			else
			{
				echo "<td><a href=\"srv_status.php?serviceid=".$row["serviceid"]."\"> $description</a></td>";
			}
		}
		echo "<td>".get_service_status_description($row["status"])."</td>";
		if($row["status"]==0)
		{
			echo "<td>-</td>";
		}
		else
		{
			echo "<td>";
			echo "<ul>";
			$sql="select s.triggerid,s.serviceid from services s, triggers t where s.status>0 and s.triggerid is not NULL and t.triggerid=s.triggerid order by s.status desc,t.description";
			$result2=DBselect($sql);
			while($row2=DBfetch($result2))
			{
				if(does_service_depend_on_the_service($row["serviceid"],$row2["serviceid"]))
				{
					$description=nbsp(expand_trigger_description($row2["triggerid"]));
					echo "<li class=\"itservices\"><a href=\"alarms.php?triggerid=".$row2["triggerid"]."\">$description</a></li>";
				}
			}
			echo "</ul>";
			echo "</td>";
		}

		if($row["showsla"]==1)
		{
			echo "<td><a href=\"report3.php?serviceid=".$row["serviceid"]."&year=".date("Y")."\"><img src=\"chart_sla.php?serviceid=".$row["serviceid"]."\" border=0></td>";
		}
		else
		{
			echo "<td>-</td>";
		}

		if($row["showsla"]==1)
		{
			$now=time(NULL);
			$period_start=$now-7*24*3600;
			$period_end=$now;
			$stat=calculate_service_availability($row["serviceid"],$period_start,$period_end);

			if($row["goodsla"]>$stat["ok"])
			{
				$color="AA0000";
			}
			else
			{
				$color="00AA00";
			}
			printf ("<td><font color=\"00AA00\">%.2f%%</font><b>/</b><font color=\"%s\">%.2f%%</font></td>",$row["goodsla"],$color,$stat["ok"]);
		}
		else
		{
			echo "<td>-</td>";
		}



		echo "<td><a href=\"srv_status.php?serviceid=".$row["serviceid"]."&showgraph=1\">Show</a></td>";
		echo "</tr>"; 
	}
	table_end();
?>

<?php
	show_footer();
?>
