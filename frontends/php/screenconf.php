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
	$page["title"] = S_SCREENS;
	$page["file"] = "screenconf.php";
	show_header($page["title"],0,0);
	insert_confirm_javascript();
?>

<?php
	show_table_header(S_CONFIGURATION_OF_SCREENS_BIG);
	echo "<br>";
?>

<?php
	if(!check_right("Screen","U",0))
	{
//		show_table_header("<font color=\"AA0000\">No permissions !</font>");
//		show_footer();
//		exit;
	}
?>

<?php
	if(isset($_GET["register"]))
	{
		if($_GET["register"]=="add")
		{
			$result=add_screen($_GET["name"],$_GET["cols"],$_GET["rows"]);
			show_messages($result,S_SCREEN_ADDED,S_CANNOT_ADD_SCREEN);
		}
		if($_GET["register"]=="update")
		{
			$result=update_screen($_GET["screenid"],$_GET["name"],$_GET["cols"],$_GET["rows"]);
			show_messages($result, S_SCREEN_UPDATED, S_CANNOT_UPDATE_SCREEN);
		}
		if($_GET["register"]=="delete")
		{
			$result=delete_screen($_GET["screenid"]);
			show_messages($result, S_SCREEN_DELETED, S_CANNOT_DELETE_SCREEN);
			unset($_GET["screenid"]);
		}
	}
?>

<?php
	show_table_header("SCREENS");
	table_begin();
	table_header(array(S_ID,S_NAME,S_COLUMNS,S_ROWS,S_ACTIONS));

	$result=DBselect("select screenid,name,cols,rows from screens order by name");
	$col=0;
	while($row=DBfetch($result))
	{
		if(!check_right("Screen","R",$row["screenid"]))
		{
			continue;
		}
		table_row(array(
			$row["screenid"],
			"<a href=\"screenedit.php?screenid=".$row["screenid"]."\">".$row["name"]."</a>",
			$row["cols"],
			$row["rows"],
			"<A HREF=\"screenconf.php?screenid=".$row["screenid"]."#form\">".S_CHANGE."</A>"
			),$col++);
	}
	if(DBnum_rows($result)==0)
	{
			echo "<TR BGCOLOR=#EEEEEE>";
			echo "<TD COLSPAN=5 ALIGN=CENTER>".S_NO_SCREENS_DEFINED."</TD>";
			echo "<TR>";
	}
	table_end();
?>

<?php
	echo "<a name=\"form\"></a>";

	if(isset($_GET["screenid"]))
	{
		$result=DBselect("select screenid,name,cols,rows from screens g where screenid=".$_GET["screenid"]);
		$row=DBfetch($result);
		$name=$row["name"];
		$cols=$row["cols"];
		$rows=$row["rows"];
	}
	else
	{
		$name="";
		$cols=1;
		$rows=1;
	}

	show_form_begin("screenconf.screen");
	echo S_SCREEN;
	$col=0;

	show_table2_v_delimiter($col++);
	echo "<form method=\"get\" action=\"screenconf.php\">";
	if(isset($_GET["screenid"]))
	{
		echo "<input class=\"biginput\" name=\"screenid\" type=\"hidden\" value=".$_GET["screenid"].">";
	}
	echo S_NAME;
	show_table2_h_delimiter();
	echo "<input class=\"biginput\" name=\"name\" value=\"$name\" size=32>";

	show_table2_v_delimiter($col++);
	echo S_COLUMNS;
	show_table2_h_delimiter();
	echo "<input class=\"biginput\" name=\"cols\" size=5 value=\"$cols\">";

	show_table2_v_delimiter($col++);
	echo S_ROWS;
	show_table2_h_delimiter();
	echo "<input class=\"biginput\" name=\"rows\" size=5 value=\"$rows\">";

	show_table2_v_delimiter2();
	echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"add\">";
	if(isset($_GET["screenid"]))
	{
		echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"update\">";
		echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"delete\" onClick=\"return Confirm('".S_DELETE_SCREEN_Q."');\">";
	}

	show_table2_header_end();
?>

<?php
	show_footer();
?>
