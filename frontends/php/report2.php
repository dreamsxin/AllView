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
	$page["title"] = S_AVAILABILITY_REPORT;
	$page["file"] = "report2.php";
	show_header($page["title"],0,0);
?>

<?php
	if(!check_anyright("Host","R"))
	{
		show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
		show_footer();
		exit;
	}
?>

<?php
	if(isset($_GET["groupid"])&&($_GET["groupid"]==0))
	{
		unset($_GET["groupid"]);
	}

	$h1="&nbsp;".S_AVAILABILITY_REPORT_BIG;

	$h2=S_GROUP."&nbsp;";
	$h2=$h2."<select class=\"biginput\" name=\"groupid\" onChange=\"submit()\">";
	$h2=$h2."<option value=\"0\" ".iif(!isset($_GET["groupid"]),"selected","").">".S_ALL_SMALL;
	$result=DBselect("select groupid,name from groups order by name");
	while($row=DBfetch($result))
	{
// Check if at least one host with read permission exists for this group
		$result2=DBselect("select h.hostid,h.host from hosts h,items i,hosts_groups hg where h.status=".HOST_STATUS_MONITORED." and h.hostid=i.hostid and hg.groupid=".$row["groupid"]." and hg.hostid=h.hostid group by h.hostid,h.host order by h.host");
		$cnt=0;
		while($row2=DBfetch($result2))
		{
			if(!check_right("Host","R",$row2["hostid"]))
			{
				continue;
			}
			$cnt=1; break;
		}
		if($cnt!=0)
		{
			$h2=$h2."<option value=\"".$row["groupid"]."\" ".iif(isset($_GET["groupid"])&&($_GET["groupid"]==$row["groupid"]),"selected","").">".$row["name"];
		}
	}
	$h2=$h2."</select>";

	$h2=$h2."&nbsp;".S_HOST."&nbsp;";
	$h2=$h2."<select class=\"biginput\" name=\"hostid\" onChange=\"submit()\">";
	$h2=$h2."<option value=\"0\"".iif(!isset($_GET["hostid"])||($_GET["hostid"]==0),"selected","").">".S_SELECT_HOST_DOT_DOT_DOT;

	if(isset($_GET["groupid"]))
	{
		$sql="select h.hostid,h.host from hosts h,items i,hosts_groups hg where h.status=".HOST_STATUS_MONITORED." and h.hostid=i.hostid and hg.groupid=".$_GET["groupid"]." and hg.hostid=h.hostid group by h.hostid,h.host order by h.host";
	}
	else
	{
		$sql="select h.hostid,h.host from hosts h,items i where h.status=".HOST_STATUS_MONITORED." and h.hostid=i.hostid group by h.hostid,h.host order by h.host";
	}

	$result=DBselect($sql);
	while($row=DBfetch($result))
	{
		if(!check_right("Host","R",$row["hostid"]))
		{
			continue;
		}
		$h2=$h2."<option value=\"".$row["hostid"]."\"".iif(isset($_GET["hostid"])&&($_GET["hostid"]==$row["hostid"]),"selected","").">".$row["host"];
	}
	$h2=$h2."</select>";

	show_header2($h1, $h2, "<form name=\"form2\" method=\"get\" action=\"report2.php\">", "</form>");
?>

<?php
	if(isset($_GET["hostid"])&&!isset($_GET["triggerid"]))
	{
		echo "<br>";
		$result=DBselect("select host from hosts where hostid=".$_GET["hostid"]);
		$row=DBfetch($result);
		show_table_header($row["host"]);

		$result=DBselect("select distinct h.hostid,h.host,t.triggerid,t.expression,t.description,t.value from triggers t,hosts h,items i,functions f where f.itemid=i.itemid and h.hostid=i.hostid and t.status=0 and t.triggerid=f.triggerid and h.hostid=".$_GET["hostid"]." and h.status=".HOST_STATUS_MONITORED." and i.status=0 order by h.host, t.description");
		table_begin();
		table_header(array(S_DESCRIPTION,S_TRUE,S_FALSE,S_UNKNOWN,S_GRAPH));
		$col=0;
		while($row=DBfetch($result))
		{
			if(!check_right_on_trigger("R",$row["triggerid"])) 
			{
				continue;
			}
			$lasthost=$row["host"];

			$description=expand_trigger_description($row["triggerid"]);
			$description="<a href=\"alarms.php?triggerid=".$row["triggerid"]."\">$description</a>";
	
			$availability=calculate_availability($row["triggerid"],0,0);

			$true=array("value"=>sprintf("%.4f%%",$availability["true"]), "class"=>"on");
			$false=array("value"=>sprintf("%.4f%%",$availability["false"]), "class"=>"off");
			$unknown=array("value"=>sprintf("%.4f%%",$availability["unknown"]), "class"=>"unknown");
			$actions="<a href=\"report2.php?hostid=".$_GET["hostid"]."&triggerid=".$row["triggerid"]."\">".S_SHOW."</a>";

			table_row(array(
				$description,
				$true,
				$false,
				$unknown,
				$actions
				),$col++);
		}
		table_end();
	}
?>

<?php
	if(isset($_GET["triggerid"]))
	{
		echo "<TABLE BORDER=0 COLS=4 align=center WIDTH=100% BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=3>";
		echo "<TR BGCOLOR=#EEEEEE>";
		echo "<TR BGCOLOR=#DDDDDD>";
		echo "<TD ALIGN=CENTER>";
		echo "<IMG SRC=\"chart4.php?triggerid=".$_GET["triggerid"]."\" border=0>";
		echo "</TD>";
		echo "</TR>";
		echo "</TABLE>";
	}
?>


<?php
	show_footer();
?>
