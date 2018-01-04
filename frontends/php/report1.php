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
	$page["title"] = S_STATUS_OF_ALLVIEW;
	$page["file"] = "report1.php";
	show_header($page["title"],0,0);
?>

<?php
	show_table_header(S_STATUS_OF_ALLVIEW_BIG);

	table_begin();

	table_header(array(S_PARAMETER,S_VALUE));

	$stats=get_stats();

	$col=0;
	$str=array("value"=>S_NO,"class"=>"on");
	if( (exec("ps -ef|grep allview_server|grep -v grep|wc -l")>0) || (exec("ps -ax|grep allview_server|grep -v grep|wc -l")>0) )
	{
		$str=array("value"=>S_YES,"class"=>"off");
	}
	table_row(array(S_ALLVIEW_SERVER_IS_RUNNING,$str),$col++);

	table_row(array(S_NUMBER_OF_VALUES_STORED,$stats["history_count"]),$col++);
	table_row(array(S_NUMBER_OF_TRENDS_STORED,$stats["trends_count"]),$col++);
	table_row(array(S_NUMBER_OF_ALARMS,$stats["alarms_count"]),$col++);
	table_row(array(S_NUMBER_OF_ALERTS,$stats["alerts_count"]),$col++);
	table_row(array(S_NUMBER_OF_TRIGGERS_ENABLED_DISABLED,$stats["triggers_count"]."(".$stats["triggers_count_enabled"]."/".$stats["triggers_count_disabled"].")"),$col++);
	table_row(array(S_NUMBER_OF_ITEMS_ACTIVE_TRAPPER,$stats["items_count"]."(".$stats["items_count_active"]."/".$stats["items_count_trapper"]."/".$stats["items_count_not_active"]."/".$stats["items_count_not_supported"].")"),$col++);
	table_row(array(S_NUMBER_OF_USERS,$stats["users_count"]),$col++);
	table_row(array(S_NUMBER_OF_HOSTS_MONITORED,$stats["hosts_count"]."(".$stats["hosts_count_monitored"]."/".$stats["hosts_count_not_monitored"]."/".$stats["hosts_count_template"].")"),$col++);

	table_end();
?>

<?php
	show_footer();
?>
