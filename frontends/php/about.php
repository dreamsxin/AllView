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
	$page["title"] = S_ABOUT_ALLVIEW;
	$page["file"] = "about.php";
	show_header($page["title"],0,0);
?>

<?php
	show_table_header(S_INFORMATION_ABOUT_ALLVIEW);
?>

<?php
	table_begin();
	table_row(array("<a href=\"http://www.myleftstudio.com\">".S_HOMEPAGE_OF_ALLVIEW."</a>", S_HOMEPAGE_OF_ALLVIEW_DETAILS),0);
	table_row(array("<a href=\"http://www.myleftstudio.com/manual.php\">".S_LATEST_ALLVIEW_MANUAL."</a>", S_LATEST_ALLVIEW_MANUAL_DETAILS),1);
	table_row(array("<a href=\"http://sourceforge.net/project/showfiles.php?group_id=23494&release_id=40630\">".S_DOWNLOADS."</a>", S_DOWNLOADS_DETAILS),2);
	table_row(array("<a href=\"http://sourceforge.net/tracker/?atid=378686&group_id=23494&func=browse\">".S_FEATURE_REQUESTS."</a>", S_FEATURE_REQUESTS_DETAILS), 3);
	table_row(array("<a href=\"http://www.myleftstudio.com/forum\">".S_FORUMS."</a>", S_FORUMS_DETAILS),4);
	table_row(array("<a href=\"http://sourceforge.net/tracker/?group_id=23494&atid=378683\">".S_BUG_REPORTS."</a>", S_BUG_REPORTS_DETAILS),5);
	table_row(array("<a href=\"http://sourceforge.net/mail/?group_id=23494\">".S_MAILING_LISTS."</a>", S_MAILING_LISTS_DETAILS),6);
	table_end();
?>

<?php
	show_footer();
?>
