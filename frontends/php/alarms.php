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
	$page["title"] = S_ALARMS;
	$page["file"] = "alarms.php";
	show_header($page["title"],0,0);
?>

<?php
	if(!check_right_on_trigger("R",$_GET["triggerid"]))
        {
                show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
                show_footer();
                exit;
        }
?>

<?php
	show_table_header_begin();
	echo S_ALARMS_BIG;
 
	show_table_v_delimiter(); 

	if(!isset($_GET["triggerid"]))
	{
		echo "<div align=center><B>No triggerID!!!!</B><BR>Please Contact Server Adminstrator</div>";
		show_footer();
		exit;
	}
	else
	{
		$trigger=get_trigger_by_triggerid($_GET["triggerid"]);

		$Expression=$trigger["expression"];
		$Description=$trigger["description"];
//		$Priority=$trigger["priority"];
	}
?>

<?php
	if(isset($_GET["limit"]) && ($_GET["limit"]=="NO"))
	{
		echo "[<A HREF=\"alarms.php?triggerid=".$_GET["triggerid"]."&limit=30\">";
		echo S_SHOW_ONLY_LAST_100;
		echo "</A>]";
		$limit=" ";
	}
	else 
	{
		echo "[<A HREF=\"alarms.php?triggerid=".$_GET["triggerid"]."&limit=NO\">";
		echo S_SHOW_ALL;
		echo "</A>]";
		$limit=" limit 100";
	}

	show_table_header_end();
	echo "<br>";
?>


<?php
	$Expression=explode_exp($Expression,1);
//	if( strstr($Description,"%s"))
//	{
		$Description=expand_trigger_description($trigger["triggerid"]);
//	}
	show_table_header("$Description<BR><font size=-1>$Expression</font>");
?>

<FONT COLOR="#000000">
<?php
	$sql="select clock,value,triggerid from alarms where triggerid=".$_GET["triggerid"]." order by clock desc $limit";
	$result=DBselect($sql);

	table_begin();
	table_header(array(S_TIME,S_STATUS,S_DURATION,S_SUM,"%"));
	$truesum=0;
	$falsesum=0;
	$dissum=0;
	$clock=mktime();
	$col=0;
	while($row=DBfetch($result))
	{
		$lclock=$clock;
		$clock=$row["clock"];
		$leng=$lclock-$row["clock"];

//		if($row["value"]==0)		{ echo "<TR BGCOLOR=#EEFFEE>"; }
//		elseif($row["value"]==2)	{ echo "<TR BGCOLOR=#EEEEEE>"; }
//		else				{ echo "<TR BGCOLOR=#FFDDDD>"; }

//		table_td(date("Y.M.d H:i:s",$row["clock"]),"");
		if($row["value"]==1)
		{
			$istrue=S_TRUE_BIG;
			$truesum=$truesum+$leng;
			$sum=$truesum;
		}
		elseif($row["value"]==0)
		{
			$istrue=S_FALSE_BIG;
			$falsesum=$falsesum+$leng;
			$sum=$falsesum;
		}
		elseif($row["value"]==3)
		{
			$istrue=S_DISABLED_BIG;
			$dissum=$dissum+$leng;
			$sum=$dissum;
		}
		elseif($row["value"]==2)
		{
			$istrue=S_UNKNOWN_BIG;
			$dissum=$dissum+$leng;
			$sum=$dissum;
		}
	
		$proc=(100*$sum)/($falsesum+$truesum+$dissum);
		$proc=round($proc*100)/100;
		$proc="$proc%";
 
//		table_td("<B>$istrue</B>","");
		if($leng>60*60*24)
		{
			$leng= round(($leng/(60*60*24))*10)/10;
			$leng="$leng days";
		}
		elseif ($leng>60*60)
		{
			$leng= round(($leng/(60*60))*10)/10;
			$leng="$leng hours"; 
		}
		elseif ($leng>60)
		{
			$leng= round(($leng/(60))*10)/10;
			$leng="$leng mins";
		}
		else
		{
			$leng="$leng secs";
		}

		if($sum>60*60*24)
		{
			$sum= round(($sum/(60*60*24))*10)/10;
			$sum="$sum days";
		}
		elseif ($sum>60*60)
		{
			$sum= round(($sum/(60*60))*10)/10;
			$sum="$sum hours"; 
		}
		elseif ($sum>60)
		{
			$sum= round(($sum/(60))*10)/10;
			$sum="$sum mins";
		}
		else
		{
			$sum="$sum secs";
		}
  
//		table_td($leng,"");
//		table_td($sum,"");
//		table_td($proc,"");
//		echo "</TR>";
		table_row(array(
			date("Y.M.d H:i:s",$row["clock"]),
			$istrue,
			$leng,
			$sum,
			$proc
			),$col++);
	}
	table_end();
?>

<?php
	show_footer();
?>
