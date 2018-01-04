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
	$page["file"] = "tr_status.php";
	$page["title"] = S_STATUS_OF_TRIGGERS;
?>
<?php
	$tr_hash=calc_trigger_hash();
	setcookie("triggers_hash",$tr_hash,time()+1800);

	if(!isset($_COOKIE["triggers_hash"]))
	{
		$triggers_hash="0,0";
	}
	else
	{
		$triggers_hash=$_COOKIE["triggers_hash"];
	}

	$new=explode(",",$tr_hash);
	$old=explode(",",$triggers_hash);
	setcookie("triggers_hash",$tr_hash,time()+1800);

	if(!isset($_COOKIE["triggers_hash"]))
	{
		$triggers_hash="0,0";
	}
	else
	{
		$triggers_hash=$_COOKIE["triggers_hash"];
	}

	$new=explode(",",$tr_hash);
	$old=explode(",",$triggers_hash);

//	Number of trigger decreased
//	echo $new[0]," ",$old[0];
	if(($old[1]!=$new[1])&&($new[0]<$old[0]))
	{
		$audio="warning_off.wav";
	}
//	Number of trigger increased
	if(($old[1]!=$new[1])&&($new[0]>=$old[0]))
	{
// DISASTER
		if(($new[0]-$old[0])/pow(10,5)>=1)
		{
			$audio="disaster_on.wav";
		}
		else
		{
			$audio="warning_on.wav";
		}
	}

//	echo "$tr_hash<br>$triggers_hash<br>".$old[1]."<br>".$new[1];
?>
<?php
	$refresh=10;
	if(!isset($_GET["onlytrue"])||isset($_GET["txt_select"]))
	{
		$refresh=0;
	}
	if(isset($_GET["fullscreen"]))
	{
		show_header($page["title"],$refresh,1);
	}
	else
	{
		show_header($page["title"],$refresh,0);
	}
?>
<?php
	if(!check_anyright("Host","R"))
	{
		show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
		show_footer();
		exit;
	}
	if(isset($_GET["hostid"])&&!check_right("Host","R",$_GET["hostid"]))
	{
		show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
		show_footer();
		exit;
	}
?>
<?php
	if(isset($audio))
	{
//		echo "AUDIO [$audio] [".$old[1].":".$new[1]."] [$triggers_hash] [$tr_hash]";
		echo "<BGSOUND src=\"audio/$audio\" loop=0>";
	}
?>                                                                                                             

<?php
 
	if(!isset($_GET["sort"]))
	{
		$sort='priority';
	}
	else
	{
		$sort=$_GET["sort"];
	}
	if(!isset($_GET["onlytrue"]))
	{
		$onlytrue='false';
	}
	else
	{
		$onlytrue=$_GET["onlytrue"];
	}
	if(isset($_GET["noactions"])&&($_GET["noactions"]!='true'))
	{
		$noactions='false';
	}
	else
	{
		$noactions='true';
	}
	if(isset($_GET["compact"])&&($_GET["compact"]!='true'))
	{
		$compact='false';
	}
	else
	{
		$compact='true';
	}
?>

<?php
	if(!isset($_GET["select"]))
	{
		$select="";
	}
	else
	{
		$select=$_GET["select"];
	}

	if(!isset($_GET["txt_select"]))
	{
		$txt_select="";
	}
	else
	{
		$txt_select=$_GET["txt_select"];
	}

	if(isset($_GET["btnSelect"])&&($_GET["btnSelect"]=="Inverse select"))
	{
		$select_cond="not like '%$txt_select%'";
	}
	else
	{
		$select_cond="like '%$txt_select%'";
	}
?>





<?php
	if(isset($_GET["groupid"])&&($_GET["groupid"]==0))
	{
		unset($_GET["groupid"]);
	}
?>

<?php
	$h1="&nbsp;".S_STATUS_OF_TRIGGERS_BIG;

	$h2="";
	$h2=$h2."<input name=\"onlytrue\" type=\"hidden\" value=\"".$_GET["onlytrue"]."\">";
	$h2=$h2."<input name=\"noactions\" type=\"hidden\" value=\"".$_GET["noactions"]."\">";
	$h2=$h2.S_GROUP."&nbsp;";
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
	$h2=$h2."<option value=\"0\"".iif(!isset($_GET["hostid"])||($_GET["hostid"]==0),"selected","").">Select host...";

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

	$h2=$h2.nbsp("  ");

	if(isset($_GET["select"])&&($_GET["select"]==""))
	{
		unset($_GET["select"]);
	}
	if(isset($_GET["select"]))
	{
  		$h2=$h2."<input class=\"biginput\" type=\"text\" name=\"select\" value=\"".$_GET["select"]."\">";
	}
	else
	{
 		$h2=$h2."<input class=\"biginput\" type=\"text\" name=\"select\" value=\"\">";
	}
	$h2=$h2.nbsp(" ");
  	$h2=$h2."<input class=\"button\" type=\"submit\" name=\"do\" value=\"select\">";
//	show_table_header_end();
	show_header2($h1, $h2, "<form name=\"form2\" method=\"get\" action=\"tr_status.php\">", "</form>");
?>

<?php
	if(!isset($_GET["fullscreen"]))
	{
/*		show_table_header_begin();
		echo S_STATUS_OF_TRIGGERS_BIG;
	        show_table_v_delimiter();

	        $result=DBselect("select hostid,host from hosts where status in (0,2) order by host");
	        if(isset($_GET["hostid"]))
	        {
	                echo "<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=$compact&sort=$sort\">".S_ALL_SMALL."</A>  ";
	        }
	        else
	        {
	                echo "<b>[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=$compact&sort=$sort\">".S_ALL_SMALL."</A>]</b>  ";
	        }
		while($row=DBfetch($result))
	        {
        		if(!check_right("Host","R",$row["hostid"]))
		        {
				continue;
			}
	                if(isset($_GET["hostid"]) && ($row["hostid"] == $_GET["hostid"]))
	                {
	                        echo "<b>[<A HREF=\"tr_status.php?hostid=".$row["hostid"]."&onlytrue=$onlytrue&noactions=$noactions&compact=$compact&sort=$sort\">".$row["host"]."</A>]</b>  ";
	                }
	                else
	                {
	                        echo "<A HREF=\"tr_status.php?hostid=".$row["hostid"]."&onlytrue=$onlytrue&noactions=$noactions&compact=$compact&sort=$sort\">".$row["host"]."</A>  ";
	                }
	        }*/
//		show_table_v_delimiter(2);
//		show_table_header_begin();
?>

<?php
		$h1="";
		if(isset($_GET["hostid"]))
		{
			$cond="&hostid=".$_GET["hostid"];
		}
		else
		{
			$cond="";
		}

		if($onlytrue!='true')
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=true&noactions=$noactions&compact=$compact&select=$select&txt_select=$txt_select&sort=$sort$cond\">".S_SHOW_ONLY_TRUE."</a>] ";
		}
		else
		{
			$h1=$h1."[<A HREF=\"tr_status.php?noactions=$noactions&compact=$compact&select=$select&txt_select=$txt_select&sort=$sort$cond\">".S_SHOW_ALL_TRIGGERS."</A>] ";
		}
		if($noactions!='true')
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=true&compact=$compact&select=$select&txt_select=$txt_select&sort=$sort$cond\">".S_HIDE_ACTIONS."</A>] ";
		}
		else
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=false&compact=$compact&select=$select&txt_select=$txt_select&sort=$sort$cond\">".S_SHOW_ACTIONS."</A>] ";
		}
		if($compact!='true')
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=true&select=$select&txt_select=$txt_select&sort=$sort$cond\">".S_HIDE_DETAILS."</A>] ";
		}
		else
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=false&select=$select&txt_select=$txt_select&sort=$sort$cond\">".S_SHOW_DETAILS."</A>] ";
		}
		
		if($select!='true')
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=$compact&select=true&txt_select=$txt_select&sort=$sort$cond\">".S_SELECT."</A>] ";
		}
		else
		{
			$h1=$h1."[<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=$compact&select=false&sort=$sort$cond\">".S_HIDE_SELECT."</A>] "; 
			$h1=$h1."<form name=\"form1\" method=\"get\" action=\"tr_status.php?select=true\">
  			<input class=\"biginput\" type=\"text\" name=\"txt_select\" value=\"$txt_select\">
  			<input class=\"button\" type=\"submit\" name=\"btnSelect\" value=\"Select\">
  			<input class=\"button\" type=\"submit\" name=\"btnSelect\" value=\"Inverse select\">
			<INPUT NAME=\"compact\" TYPE=\"HIDDEN\" value=\"$compact\">
			<INPUT NAME=\"onlytrue\" TYPE=\"HIDDEN\" value=\"$onlytrue\">
			<INPUT NAME=\"noactions\" TYPE=\"HIDDEN\" value=\"$noactions\">			
		        <INPUT NAME=\"select\" TYPE=\"HIDDEN\" value=\"$select\">
			</form>";
		}
		show_table_header($h1);
//		echo "<br>";
	}

 	$time=date("[H:i:s]",time());
  	if(isset($_GET["fullscreen"]))
	{
		show_table_header("<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=$compact&sort=$sort\">".S_TRIGGERS_BIG." $time</A>");

		$cond="";
		if(isset($_GET["hostid"]))
		{
			$cond=" and h.hostid=".$_GET["hostid"]." ";
		}

		if($onlytrue=='true')
		{
			$sql="select t.priority,count(*) from triggers t,hosts h,items i,functions f  where t.value=1 and t.status=0 and f.itemid=i.itemid and h.hostid=i.hostid and h.status=".HOST_STATUS_MONITORED." and t.triggerid=f.triggerid and t.description $select_cond and i.status=0 $cond group by 1";
		}
		else
		{
			$sql="select t.priority,count(*) from triggers t,hosts h,items i,functions f  where f.itemid=i.itemid and h.hostid=i.hostid and t.triggerid=f.triggerid and t.status=0 and h.status=".HOST_STATUS_MONITORED." and t.description $select_cond and i.status=0 $cond group by 1";
		}
		$result=DBselect($sql);
		$p0=$p1=$p2=$p3=$p4=$p5=0;
		for($i=0;$i<DBnum_rows($result);$i++)
		{
			$priority=DBget_field($result,$i,0);
			$count=DBget_field($result,$i,1);
			if($priority==0) $p0=$count;
			if($priority==1) $p1=$count;
			if($priority==2) $p2=$count;
			if($priority==3) $p3=$count;
			if($priority==4) $p4=$count;
			if($priority==5) $p5=$count;
		}
		echo "\n<TABLE BORDER=0 align=center WIDTH=100% BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=0>";
		echo "<TR ALIGN=CENTER>";
		table_td("<B>".S_NOT_CLASSIFIED.": $p0</B>","");
		table_td("<B>".S_INFORMATION.": $p1</B>","");
		table_td("<B>".S_WARNING.": $p2</B>","");
		table_td("<B>".S_AVERAGE.": $p3</B>","BGCOLOR=#DDAAAA");
		table_td("<B>".S_HIGH.": $p4</B>","BGCOLOR=#FF8888");
		table_td("<B>".S_DISASTER.": $p5</B>","BGCOLOR=RED");
		echo "</TR>";
		echo "</TABLE>";
	}
	else
	{
//		show_table_header("<A HREF=\"tr_status.php?onlytrue=$onlytrue&noactions=$noactions&compact=$compact&fullscreen=1&sort=$sort\">".S_TRIGGERS_BIG." $time</A>");
	}

	table_begin();
	$header=array();
  
	echo "<TR ALIGN=CENTER BGCOLOR=\"#CCCCCC\">";
	if(isset($_GET["fullscreen"]))
	{
		$fullscreen="&fullscreen=1";
	}
	else
	{
		$fullscreen="";
	}
	if(isset($sort) && $sort=="description")
	{
		$description=S_DESCRIPTION_BIG;
	}
	else
	{
		if($select=="TRUE")
			$description="<A HREF=\"tr_status.php?sort=description&onlytrue=$onlytrue&noactions=$noactions&compact=$compact&select=$select&txt_select=$txt_select$fullscreen$cond\">".S_DESCRIPTION;
		else
			$description="<A HREF=\"tr_status.php?sort=description&onlytrue=$onlytrue&noactions=$noactions&compact=$compact$fullscreen$cond\">".S_DESCRIPTION."</a>";
	}
	if($compact!='true') {$description=$description."<BR><FONT SIZE=-1>".S_EXPRESSION."</FONT></B>";}
	$header=array_merge($header,array($description));
	$header=array_merge($header,array(S_STATUS));

	if(!isset($sort)||(isset($sort) && $sort=="priority"))
	{
		$header=array_merge($header,array(S_SEVERITY_BIG));
	}
	else
	{
		if($select=="TRUE")
			$header=array_merge($header,array("<A HREF=\"tr_status.php?sort=priority&onlytrue=$onlytrue&noactions=$noactions&compact=$compact&select=$select&txt_select=$txt_select$fullscreen$cond\">".S_SEVERITY."</a>"));
		else
			$header=array_merge($header,array("<A HREF=\"tr_status.php?sort=priority&onlytrue=$onlytrue&noactions=$noactions&compact=$compact$fullscreen$cond\">".S_SEVERITY."</a>"));
	}

	if(isset($sort) && $sort=="lastchange")
	{
		$header=array_merge($header,array(nbsp(S_LAST_CHANGE_BIG)));
	}
	else
	{
		if($select=="TRUE")
		{
			$header=array_merge($header,array("<A HREF=\"tr_status.php?sort=lastchange&onlytrue=$onlytrue&noactions=$noactions&compact=$compact&select=$select&txt_select=$txt_select$fullscreen$cond\">".nbsp(S_LAST_CHANGE)."</a>"));
		}
		else
		{
			$header=array_merge($header,array("<A HREF=\"tr_status.php?sort=lastchange&onlytrue=$onlytrue&noactions=$noactions&compact=$compact$fullscreen$cond\">".nbsp(S_LAST_CHANGE)."</a>"));
		}
	}
	echo "</TD>";
   
	if($noactions!='true')
	{
		$header=array_merge($header,array(S_ACTIONS));
	}
	$header=array_merge($header,array(S_COMMENTS));
	table_header($header);
	unset($header);

	if(isset($_GET["hostid"]))
	{
		$cond=" and h.hostid=".$_GET["hostid"]." ";
	}
	else
	{
		$cond="";
	}

	if(!isset($sort))
	{
		$sort="priority";
	}

	switch ($sort)
	{
		case "description":
			$sort="order by t.description";
			break;
		case "priority":
			$sort="order by t.priority desc, t.description";
			break;
		case "lastchange":
			$sort="order by t.lastchange desc, t.priority";
			break;
		default:
			$sort="order by t.priority desc, t.description";
	}

	if($onlytrue=='true')
	{
		$result=DBselect("select distinct t.triggerid,t.status,t.description,t.expression,t.priority,t.lastchange,t.comments,t.url,t.value from triggers t,hosts h,items i,functions f  where t.value=1 and t.status=0 and f.itemid=i.itemid and h.hostid=i.hostid and t.description $select_cond and t.triggerid=f.triggerid and i.status in (0,2) and h.status=".HOST_STATUS_MONITORED." $cond $sort");
	}
	else
	{
		$result=DBselect("select distinct t.triggerid,t.status,t.description,t.expression,t.priority,t.lastchange,t.comments,t.url,t.value from triggers t,hosts h,items i,functions f  where f.itemid=i.itemid and h.hostid=i.hostid and t.triggerid=f.triggerid and t.status=0 and t.description $select_cond and i.status in (0,2) and h.status=".HOST_STATUS_MONITORED." $cond $sort");
	}
	$col=0;
	while($row=DBfetch($result))
	{
		if(!check_right_on_trigger("R",$row["triggerid"]))
		{
			continue;
		}

// Check for dependencies

		$sql="select count(*) from trigger_depends d, triggers t where d.triggerid_down=".$row["triggerid"]." and d.triggerid_up=t.triggerid and t.value=1";
		$result2=DBselect($sql);

		if(DBget_field($result2,0,0)>0)
		{
			continue;
		}

		$elements=array();


		$description=expand_trigger_description($row["triggerid"]);

		if($row["url"] != "")
		{
			$description="<a href='".$row["url"]."'>$description</a>";
		}

		if($compact!='true')
		{
			$description=$description."<BR><FONT COLOR=\"#000000\" SIZE=-2>".explode_exp($row["expression"],1)."</FONT>";
		}

		if( (time(NULL)-$row["lastchange"])<300)
		{
			$blink1="<blink>";
			$blink2="</blink>";
		}
		else
		{
			$blink1="";
			$blink2="";
		}
		if($row["value"]==0)
				$value=array("value"=>"$blink1".S_FALSE_BIG."$blink2","class"=>"off");
		else if($row["value"]==2)
				$value=array("value"=>"$blink1".S_UNKNOWN_BIG."$blink2","class"=>"unknown");
		else
				$value=array("value"=>S_TRUE_BIG,"class"=>"on");

		if($row["priority"]==0)		$priority=S_NOT_CLASSIFIED;
		elseif($row["priority"]==1)	$priority=S_INFORMATION;
		elseif($row["priority"]==2)	$priority=S_WARNING;
		elseif($row["priority"]==3)	$priority=array("value"=>S_AVERAGE,"class"=>"average");
		elseif($row["priority"]==4)	$priority=array("value"=>S_HIGH,"class"=>"high");
		elseif($row["priority"]==5)	$priority=array("value"=>S_DISASTER,"class"=>"disaster");
		else				$priority=$row["priority"];

		$lastchange="<A HREF=\"alarms.php?triggerid=".$row["triggerid"]."\">".date(S_DATE_FORMAT_YMDHMS,$row["lastchange"])."</a>";

		$actions=array("hide"=>1);
		if($noactions!='true')
		{
			$actions="<A HREF=\"actions.php?triggerid=".$row["triggerid"]."\">".S_SHOW_ACTIONS."</A> - ";
			$actions=$actions."<A HREF=\"alarms.php?triggerid=".$row["triggerid"]."\">".S_HISTORY."</A> - ";
			if(isset($_GET["hostid"]))
			{
				$actions=$actions."<A HREF=\"triggers.php?hostid=".$_GET["hostid"]."&triggerid=".$row["triggerid"]."#form\">".S_CHANGE."</A>";
			}
			else
			{
				$actions=$actions."<A HREF=\"triggers.php?triggerid=".$row["triggerid"]."#form\">".S_CHANGE."</A>";
			}
		}
		$comments=array("hide"=>1);;
		if($row["comments"] != "")
		{
			$comments="<A HREF=\"tr_comments.php?triggerid=".$row["triggerid"]."\">".S_SHOW."</a>";
		}
		else
		{
			$comments="<A HREF=\"tr_comments.php?triggerid=".$row["triggerid"]."\">".S_ADD."</a>";
		}

		table_row(array(
				$description,
				$value,
				$priority,
				$lastchange,
				$actions,
				$comments
				),$col++);
	}
	table_end();

	show_table_header(S_TOTAL.":$col");
?>

<?php
	show_footer();
?>
