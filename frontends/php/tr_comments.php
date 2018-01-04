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
	$page["title"] = S_TRIGGER_COMMENTS;
	$page["file"] = "tr_comments.php";
	show_header($page["title"],0,0);
?>

<?php
	if(!check_right("Trigger comment","R",$_GET["triggerid"]))
	{
		show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
		show_footer();
		exit;
	}
?>

<?php
	show_table_header(S_TRIGGER_COMMENTS_BIG);
?>

<?php
	if(isset($_GET["register"]) && ($_GET["register"]=="update"))
	{
		$result=update_trigger_comments($_GET["triggerid"],$_GET["comments"]);
		show_messages($result, S_COMMENT_UPDATED, S_CANNO_UPDATE_COMMENT);
	}
?>

<?php
	$result=DBselect("select comments from triggers where triggerid=".$_GET["triggerid"]);
	$comments=stripslashes(DBget_field($result,0,0));
?>

<?php
	show_form_begin("tr_comments.comments");
	echo S_COMMENTS;

	show_table2_v_delimiter();
	echo "<form method=\"get\" action=\"tr_comments.php\">";
	echo "<input name=\"triggerid\" type=\"hidden\" value=".$_GET["triggerid"].">";
	echo S_COMMENTS;
	show_table2_h_delimiter();
	echo "<textarea name=\"comments\" cols=100 ROWS=\"25\" wrap=\"soft\">$comments</TEXTAREA>";

	show_table2_v_delimiter2();
	echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"update\">";

	show_table2_header_end();
?>

<?php
	show_footer();
?>
