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
	$page["title"] = "User defined screens";
	$page["file"] = "screens.php";

	$nomenu=0;
	if(isset($_GET["fullscreen"]))
	{
		$nomenu=1;
	}
	if(isset($_GET["screenid"]))
	{
		show_header($page["title"],60,$nomenu);
	}
	else
	{
		show_header($page["title"],0,$nomenu);
	}
?>

<?php
		if(isset($_GET["screenid"])&&($_GET["screenid"]==0))
		{
			unset($_GET["screenid"]);
		}

		if(isset($_GET["screenid"]))
		{
			$result=DBselect("select name from screens where screenid=".$_GET["screenid"]);
			$map=DBget_field($result,0,0);
			$map=iif(isset($_GET["fullscreen"]),
				"<a href=\"screens.php?screenid=".$_GET["screenid"]."\">".$map."</a>",
				"<a href=\"screens.php?screenid=".$_GET["screenid"]."&fullscreen=1\">".$map."</a>");
		}
		else
		{
			$map=S_SELECT_SCREEN_TO_DISPLAY;
		}

		$h1=S_SCREENS_BIG.nbsp(" / ").$map;

		$h2="";
		if(isset($_GET["fullscreen"]))
		{
			$h2=$h2."<input name=\"fullscreen\" type=\"hidden\" value=".$_GET["fullscreen"].">";
		}

		$h2=$h2."<select class=\"biginput\" name=\"screenid\" onChange=\"submit()\">";
		$h2=$h2."<option value=\"0\" ".iif(!isset($_GET["screenid"]),"selected","").">".S_SELECT_SCREEN_DOT_DOT_DOT;

		$result=DBselect("select screenid,name from screens order by name");
		while($row=DBfetch($result))
		{
			if(!check_right("Screen","R",$row["screenid"]))
			{
				continue;
			}
			$h2=$h2."<option value=\"".$row["screenid"]."\" ".iif(isset($_GET["screenid"])&&($_GET["screenid"]==$row["screenid"]),"selected","").">".$row["name"];
		}
		$h2=$h2."</select>";

		show_header2($h1,$h2,"<form name=\"form2\" method=\"get\" action=\"screens.php\">","</form>");
?>

<?php
//	if(isset($_GET["screenid"]))
	if( isset($_GET["screenid"]) && check_right("Screen","R",$_GET["screenid"]))
	{
		$screenid=$_GET["screenid"];
		$result=DBselect("select name,cols,rows from screens where screenid=$screenid");
		$row=DBfetch($result);
/*		if(isset($_GET["fullscreen"]))
		{
			$map="<a href=\"screens.php?screenid=".$_GET["screenid"]."\">".$row["name"]."</a>";
		}
		else
		{
			$map="<a href=\"screens.php?screenid=".$_GET["screenid"]."&fullscreen=1\">".$row["name"]."</a>";
		}
	show_table_header($map);*/
          echo "<TABLE BORDER=1 COLS=".$row["cols"]." align=center WIDTH=100% BGCOLOR=\"#FFFFFF\"";
          for($r=0;$r<$row["rows"];$r++)
          {
          echo "<TR>";
          for($c=0;$c<$row["cols"];$c++)
          {
                echo "<TD align=\"center\" valign=\"top\">\n";

		$sql="select * from screens_items where screenid=$screenid and x=$c and y=$r";
		$iresult=DBSelect($sql);
		if(DBnum_rows($iresult)>0)
		{
			$irow=DBfetch($iresult);
			$screenitemid=$irow["screenitemid"];
			$resource=$irow["resource"];
			$resourceid=$irow["resourceid"];
			$width=$irow["width"];
			$height=$irow["height"];
		}

		if(DBnum_rows($iresult)>0)
		{
			if($resource == 0)
			{
				echo "<a href=charts.php?graphid=$resourceid><img src='chart2.php?graphid=$resourceid&width=$width&height=$height&period=3600&border=0' border=0></a>";
			}
			else if($resource == 1)
			{
				echo "<a href=history.php?action=showhistory&itemid=$resourceid><img src='chart.php?itemid=$resourceid&width=$width&height=$height&period=3600&border=0' border=0></a>";
			}
			else if($resource == 2)
			{
				echo get_map_imagemap($resourceid);
				echo "<img src='map.php?sysmapid=$resourceid&noedit=true&border=1' border=0 usemap=#links$resourceid>";
			}
			else if($resource == 3)
			{
				show_screen_plaintext($resourceid);
			}
		}
		else
		{
			echo "&nbsp;";
		}
		echo "</TD>";
          }
          echo "</TR>\n";
          }
          echo "</TABLE>";
	}
	else
	{
//		show_table_header(S_SELECT_SCREEN_TO_DISPLAY);		
		echo "<TABLE BORDER=0 align=center COLS=4 WIDTH=100% BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=3>";
		echo "<TR BGCOLOR=#DDDDDD>";
		echo "<TD ALIGN=CENTER>";
		echo "...";
		echo "</TD>";
		echo "</TR>";
		echo "</TABLE>";
	}
?>

<?php
	show_footer();
?>
