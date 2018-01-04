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
	include "include/forms.inc.php";
	$page["title"] = S_CONFIGURATION_OF_GRAPHS;
	$page["file"] = "graphs.php";
	show_header($page["title"],0,0);
	insert_confirm_javascript();
?>

<?php
	show_table_header(S_CONFIGURATION_OF_GRAPHS_BIG);
	echo "<br>";
?>

<?php
	if(!check_anyright("Graph","U"))
	{
		show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
		show_footer();
		exit;
	}
?>

<?php
	if(isset($_GET["register"]))
	{
		if($_GET["register"]=="add")
		{
			$result=add_graph($_GET["name"],$_GET["width"],$_GET["height"],$_GET["yaxistype"],$_GET["yaxismin"],$_GET["yaxismax"]);
			if($result)
			{
				add_audit(AUDIT_ACTION_ADD,AUDIT_RESOURCE_GRAPH,"Graph [".addslashes($_GET["name"])."]");
			}
			show_messages($result, S_GRAPH_ADDED, S_CANNOT_ADD_GRAPH);
		}
		if($_GET["register"]=="update")
		{
			$result=update_graph($_GET["graphid"],$_GET["name"],$_GET["width"],$_GET["height"],$_GET["yaxistype"],$_GET["yaxismin"],$_GET["yaxismax"]);
			if($result)
			{
				add_audit(AUDIT_ACTION_ADD,AUDIT_RESOURCE_GRAPH,"Graph ID [".$_GET["graphid"]."] Graph [".addslashes($_GET["name"])."]");
			}
			show_messages($result, S_GRAPH_UPDATED, S_CANNOT_UPDATE_GRAPH);
		}
		if($_GET["register"]=="delete")
		{
			$graph=get_graph_by_graphid($_GET["graphid"]);
			$result=delete_graph($_GET["graphid"]);
			if($result)
			{
				add_audit(AUDIT_ACTION_DELETE,AUDIT_RESOURCE_GRAPH,"Graph [".addslashes($graph["name"])."]");
			}
			show_messages($result, S_GRAPH_DELETED, S_CANNOT_DELETE_GRAPH);
			unset($_GET["graphid"]);
		}
	}
?>

<?php
	show_table_header("GRAPHS");
	table_begin();
	table_header(array(S_ID,S_NAME,S_WIDTH,S_HEIGHT,S_ACTIONS));

	$result=DBselect("select g.graphid,g.name,g.width,g.height from graphs g order by g.name");
	$col=0;
	while($row=DBfetch($result))
	{
		if(!check_right("Graph","U",$row["graphid"]))
		{
			continue;
		}
	
		table_row(array(
			$row["graphid"],
			"<a href=\"graph.php?graphid=".$row["graphid"]."\">".$row["name"]."</a>",
			$row["width"],
			$row["height"],
			"<A HREF=\"graphs.php?graphid=".$row["graphid"]."#form\">Change</A>"
			),$col++);
	}
	if(DBnum_rows($result)==0)
	{
		echo "<TR BGCOLOR=#EEEEEE>";
		echo "<TD COLSPAN=5 ALIGN=CENTER>".S_NO_GRAPHS_DEFINED."</TD>";
		echo "<TR>";
	}
	table_end();
?>

<?php
	echo "<a name=\"form\"></a>";

	insert_graph_form();

?>

<?php
	show_footer();
?>
