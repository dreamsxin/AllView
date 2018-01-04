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

	$page["title"] = S_QUEUE_BIG;
	$page["file"] = "queue.php";
	show_header($page["title"],10,0);
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
	if(!isset($_GET["show"]))
	{
		$_GET["show"]=0;
	}

	$h1=S_QUEUE_OF_ITEMS_TO_BE_UPDATED_BIG;

#	$h2=S_GROUP."&nbsp;";
	$h2="";
	$h2=$h2."<select class=\"biginput\" name=\"show\" onChange=\"submit()\">";
	$h2=$h2."<option value=\"0\" ".iif($_GET["show"]==0,"selected","").">".S_OVERVIEW;
	$h2=$h2."<option value=\"1\" ".iif($_GET["show"]==1,"selected","").">".S_DETAILS;
	$h2=$h2."</select>";

	show_header2($h1, $h2, "<form name=\"selection\" method=\"get\" action=\"queue.php\">", "</form>");
?>

<?php
	$now=time();

	$result=DBselect("select i.itemid, i.nextcheck, i.description, h.host,h.hostid from items i,hosts h where i.status=0 and i.type not in (2) and ((h.status=".HOST_STATUS_MONITORED." and h.available!=".HOST_AVAILABLE_FALSE.") or (h.status=".HOST_STATUS_MONITORED." and h.available=".HOST_AVAILABLE_FALSE." and h.disable_until<=$now)) and i.hostid=h.hostid and i.nextcheck<$now and i.key_ not in ('status','icmpping','icmppingsec','allview[log]') order by i.nextcheck");
	table_begin();
	if($_GET["show"]==0)
	{
		$sec_5=0;
		$sec_10=0;
		$sec_30=0;
		$sec_60=0;
		$sec_300=0;
		$sec_rest=0;
		while($row=DBfetch($result))
		{
			if(!check_right("Host","R",$row["hostid"]))
			{
				continue;
			}
			if($now-$row["nextcheck"]<=5)		$sec_5++;
			elseif($now-$row["nextcheck"]<=10)	$sec_10++;
			elseif($now-$row["nextcheck"]<=30)	$sec_30++;
			elseif($now-$row["nextcheck"]<=60)	$sec_60++;
			elseif($now-$row["nextcheck"]<=300)	$sec_300++;
			else					$sec_rest++;

		}
		table_header(array(S_DELAY,S_COUNT));
		$elements=array(S_5_SECONDS,$sec_5);
		table_row($elements,$col++);
		$elements=array(S_10_SECONDS,$sec_10);
		table_row($elements,$col++);
		$elements=array(S_30_SECONDS,$sec_30);
		table_row($elements,$col++);
		$elements=array(S_1_MINUTE,$sec_60);
		table_row($elements,$col++);
		$elements=array(S_5_MINUTES,$sec_300);
		table_row($elements,$col++);
		$elements=array(S_MORE_THAN_5_MINUTES,$sec_rest);
		table_row($elements,$col++);
	}
	else
	{
		table_header(array(S_NEXT_CHECK,S_HOST,S_DESCRIPTION));
		$col=0;
		while($row=DBfetch($result))
		{
			if(!check_right("Host","R",$row["hostid"]))
			{
				continue;
			}
			$elements=array(date("m.d.Y H:i:s",$row["nextcheck"]),$row["host"],$row["description"]);
			table_row($elements,$col++);
		}
		iif_echo(DBnum_rows($result)==0,
			"<TR BGCOLOR=#EEEEEE><TD COLSPAN=3 ALIGN=CENTER>".S_THE_QUEUE_IS_EMPTY."</TD><TR>",
			"");
	}

	table_end();
?>
<?php
	show_table_header(S_TOTAL.":$col");
?>

<?php
	show_footer();
?>
