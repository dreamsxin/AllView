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

	$page["title"] = S_IT_SERVICES;
	$page["file"] = "services.php";

	show_header($page["title"],0,0);
	insert_confirm_javascript();
?>

<?php
	if(!check_anyright("Service","U"))
	{
		show_table_header("<font color=\"AA0000\">".S_NO_PERMISSIONS."</font>");
		show_footer();
		exit;
	}
?>

<?php
	if(isset($_GET["register"]))
	{
		if($_GET["register"]=="update")
		{
			$result=@update_service($_GET["serviceid"],$_GET["name"],$_GET["triggerid"],$_GET["linktrigger"],$_GET["algorithm"],$_GET["showsla"],$_GET["goodsla"],$_GET["sortorder"]);
			show_messages($result, S_SERVICE_UPDATED, S_CANNOT_UPDATE_SERVICE);
		}
		if($_GET["register"]=="add")
		{
			$result=@add_service($_GET["serviceid"],$_GET["name"],$_GET["triggerid"],$_GET["linktrigger"],$_GET["algorithm"],$_GET["showsla"],$_GET["goodsla"],$_GET["goodsla"]);
			show_messages($result, S_SERVICE_ADDED, S_CANNOT_ADD_SERVICE);
		}
		if($_GET["register"]=="add server")
		{
			$result=add_host_to_services($_GET["hostid"],$_GET["serviceid"]);
			show_messages($result, S_TRIGGER_ADDED, S_CANNOT_ADD_TRIGGER);
		}
		if($_GET["register"]=="add link")
		{
			if(!isset($_GET["softlink"]))
			{
				$_GET["softlink"]=0;
			}
			else
			{
				$_GET["softlink"]=1;
			}
			$result=add_service_link($_GET["servicedownid"],$_GET["serviceupid"],$_GET["softlink"]);
			show_messages($result, S_LINK_ADDED, S_CANNOT_ADD_LINK);
		}
		if($_GET["register"]=="delete")
		{
			$result=delete_service($_GET["serviceid"]);
			show_messages($result, S_SERVICE_DELETED, S_CANNOT_DELETE_SERVICE);
			unset($_GET["serviceid"]);
		}
		if($_GET["register"]=="delete_link")
		{
			$result=delete_service_link($_GET["linkid"]);
			show_messages($result, S_LINK_DELETED, S_CANNOT_DELETE_LINK);
		}
	}
?>

<?php
	show_table_header(S_IT_SERVICES_BIG);

	$now=time();
	$result=DBselect("select serviceid,name,algorithm from services order by sortorder,name");
	echo "<table border=0 width=100% bgcolor='#AAAAAA' cellspacing=1 cellpadding=3>";
	echo "<tr bgcolor='#CCCCCC'>";
	echo "<td><b>".S_SERVICE."</b></td>";
	echo "<td width=20%><b>".S_STATUS_CALCULATION."</b></td>";
	echo "</tr>";

	$col=0;
	if(isset($_GET["serviceid"]))
	{
		echo "<tr bgcolor=#EEEEEE>";
		$service=get_service_by_serviceid($_GET["serviceid"]);
		echo "<td><b><a href=\"services.php?serviceid=".$service["serviceid"]."#form\">".$service["name"]."</a></b></td>";
		if($service["algorithm"] == SERVICE_ALGORITHM_NONE)
		{
			echo "<td>".S_NONE."</td>";
		}
		else if($service["algorithm"] == SERVICE_ALGORITHM_MAX)
		{
			echo "<td>".S_MAX_OF_CHILDS."</td>";
		}
		else if($service["algorithm"] == SERVICE_ALGORITHM_MIN)
		{
			echo "<td>".S_MIN_OF_CHILDS."</td>";
		}
		else
		{
			echo "<td>".S_UNKNOWN."</td>";
		}
		echo "</tr>"; 
		$col++;
	}
	while($row=DBfetch($result))
	{
		if(!isset($_GET["serviceid"]) && service_has_parent($row["serviceid"]))
		{
			continue;
		}
		if(isset($_GET["serviceid"]) && service_has_no_this_parent($_GET["serviceid"],$row["serviceid"]))
		{
			continue;
		}
		if(isset($_GET["serviceid"])&&($_GET["serviceid"]==$row["serviceid"]))
		{
			echo "<tr bgcolor=#99AABB>";
		}
		else
		{
			if($col++%2==0)	{ echo "<tr bgcolor=#EEEEEE>"; }
			else		{ echo "<tr bgcolor=#DDDDDD>"; }
		}
		$childs=get_num_of_service_childs($row["serviceid"]);
		if(isset($_GET["serviceid"]))
		{
			echo "<td> - <a href=\"services.php?serviceid=".$row["serviceid"]."#form\">".$row["name"]." [$childs]</a></td>";
		}
		else
		{
			echo "<td><a href=\"services.php?serviceid=".$row["serviceid"]."#form\">".$row["name"]." [$childs]</a></td>";
		}
		if($row["algorithm"] == SERVICE_ALGORITHM_NONE)
		{
			echo "<td>".S_NONE."</td>";
		}
		else if($row["algorithm"] == SERVICE_ALGORITHM_MAX)
		{
			echo "<td>".S_MAX_OF_CHILDS."</td>";
		}
		else if($row["algorithm"] == SERVICE_ALGORITHM_MIN)
		{
			echo "<td>".S_MIN_OF_CHILDS."</td>";
		}
		else
		{
			echo "<td>".S_UNKNOWN."</td>";
		}
		echo "</tr>";
	}
	echo "</table>";
?>

<?php
	if(isset($_GET["serviceid"]))
	{
		show_table_header("LINKS");
		echo "<table border=0 width=100% bgcolor='#AAAAAA' cellspacing=1 cellpadding=3>";
		echo "<tr bgcolor='#CCCCCC'>";
		echo "<td><b>".S_SERVICE_1."</b></td>";
		echo "<td><b>".S_SERVICE_2."</b></td>";
		echo "<td><b>".S_SOFT_HARD_LINK."</b></td>";
		echo "<td><b>".S_ACTIONS."</b></td>";
		echo "</tr>";
		$sql="select linkid,servicedownid,serviceupid,soft from services_links where serviceupid=".$_GET["serviceid"]." or servicedownid=".$_GET["serviceid"];
		$result=DBselect($sql);
		$col=0;
		while($row=DBfetch($result))
		{
			if($col++%2==0)	{ echo "<tr bgcolor=#EEEEEE>"; }
			else		{ echo "<tr bgcolor=#DDDDDD>"; }
			$service=get_service_by_serviceid($row["serviceupid"]);
			echo "<td>".$service["name"]."</td>";
			$service=get_service_by_serviceid($row["servicedownid"]);
			echo "<td>".$service["name"]."</td>";
			if($row["soft"] == 0)
			{	
				echo "<td>".S_HARD."</td>";
			}
			else
			{
				echo "<td>".S_SOFT."</td>";
			}
			echo "<td><a href=\"services.php?register=delete_link&serviceid=".$_GET["serviceid"]."&linkid=".$row["linkid"]."\">".S_DELETE."</a></td>";
			echo "</tr>";
		}
		echo "</table>";
	}
?>

<?php
	if(isset($_GET["serviceid"]))
	{
		$result=DBselect("select serviceid,triggerid,name,algorithm,showsla,goodsla,sortorder from services where serviceid=".$_GET["serviceid"]);
		$triggerid=DBget_field($result,0,1);
		$name=DBget_field($result,0,2);
		$algorithm=DBget_field($result,0,3);
		$showsla=DBget_field($result,0,4);
		$goodsla=DBget_field($result,0,5);
		$sortorder=DBget_field($result,0,6);
	}
	else
	{
		$name="";
		$showsla=0;
		$goodsla=99.05;
		$sortorder=0;
		unset($triggerid);
	}

	echo "<a name=\"form\"></a>";
	show_form_begin("services.service");
	echo S_SERVICE;
	$col=0;

	show_table2_v_delimiter($col++);
	echo "<form method=\"get\" action=\"services.php\">";
	if(isset($_GET["serviceid"]))
	{
		echo "<input class=\"biginput\" name=\"serviceid\" type=\"hidden\" value=".$_GET["serviceid"].">";
	}
	echo S_NAME;
	show_table2_h_delimiter();
	echo "<input class=\"biginput\" name=\"name\" value=\"$name\" size=32>";

	show_table2_v_delimiter($col++);
	echo nbsp(S_STATUS_CALCULATION_ALGORITHM);
	show_table2_h_delimiter();
	echo "<select class=\"biginput\" name=\"algorithm\" size=1>";
//	if(isset($_GET["algorithm"]))
	if(isset($algorithm))
	{
//		if($_GET["algorithm"] == SERVICE_ALGORITHM_NONE)
		if($algorithm == SERVICE_ALGORITHM_NONE)
		{
			echo "<OPTION VALUE='0' SELECTED>".S_DO_NOT_CALCULATE;
			echo "<OPTION VALUE='1'>".S_MAX_BIG;
			echo "<OPTION VALUE='2'>".S_MIN_BIG;
		}
//		else if($_GET["algorithm"] == SERVICE_ALGORITHM_MAX)
		else if($algorithm == SERVICE_ALGORITHM_MAX)
		{
			echo "<OPTION VALUE='0'>".S_DO_NOT_CALCULATE;
			echo "<OPTION VALUE='1' SELECTED>".S_MAX_BIG;
			echo "<OPTION VALUE='2'>".S_MIN_BIG;
		}
		else if($_GET["algorithm"] == SERVICE_ALGORITHM_MIN)
		{
			echo "<OPTION VALUE='0'>".S_DO_NOT_CALCULATE;
			echo "<OPTION VALUE='1'>".S_MAX_BIG;
			echo "<OPTION VALUE='2' SELECTED>".S_MIN_BIG;
		}
	}
	else
	{
		echo "<OPTION VALUE='0'>".S_DO_NOT_CALCULATE;
		echo "<OPTION VALUE='1' SELECTED>".S_MAX_BIG;
		echo "<OPTION VALUE='2'>".S_MIN_BIG;
	}
	echo "</SELECT>";

        show_table2_v_delimiter($col++);
        echo nbsp(S_SHOW_SLA);
        show_table2_h_delimiter();
	if($showsla==1)
	{
   //     	echo "<INPUT class=\"biginput\" TYPE=\"CHECKBOX\" NAME=\"showsla\" VALUE=\"true\" CHECKED>";
        	echo "<INPUT class=\"biginput\" TYPE=\"CHECKBOX\" NAME=\"showsla\" VALUE=\"on\" CHECKED>";
	}
	else
	{
        	echo "<INPUT class=\"biginput\" TYPE=\"CHECKBOX\" NAME=\"showsla\">";
	}

	show_table2_v_delimiter($col++);
	echo nbsp(S_ACCEPTABLE_SLA_IN_PERCENT);
	show_table2_h_delimiter();
	echo "<input class=\"biginput\" name=\"goodsla\" value=\"$goodsla\" size=6>";

        show_table2_v_delimiter($col++);
        echo nbsp(S_LINK_TO_TRIGGER_Q);
        show_table2_h_delimiter();
	if(isset($triggerid)&&($triggerid!=""))
	{
        	echo "<INPUT class=\"biginput\" TYPE=\"CHECKBOX\" NAME=\"linktrigger\" VALUE=\"on\" CHECKED>";
	}
	else
	{
        	echo "<INPUT class=\"biginput\" TYPE=\"CHECKBOX\" NAME=\"linktrigger\">";
	}

	show_table2_v_delimiter($col++);
	echo S_TRIGGER;
	show_table2_h_delimiter();
        $result=DBselect("select triggerid,description from triggers order by description");
        echo "<select class=\"biginput\" name=\"triggerid\" size=1>";
        for($i=0;$i<DBnum_rows($result);$i++)
        {
                $triggerid_=DBget_field($result,$i,0);
//                $description_=DBget_field($result,$i,1);
//		if( strstr($description_,"%s"))
//		{
			$description_=expand_trigger_description($triggerid_);
//		}
//		if(isset($_GET["triggerid"]) && ($_GET["triggerid"]==$triggerid_))
		if(isset($triggerid) && ($triggerid==$triggerid_))
                {
                        echo "<OPTION VALUE='$triggerid_' SELECTED>$description_";
                }
                else
                {
                        echo "<OPTION VALUE='$triggerid_'>$description_";
                }
        }
        echo "</SELECT>";

	show_table2_v_delimiter($col++);
	echo nbsp(S_SORT_ORDER_0_999);
	show_table2_h_delimiter();
	echo "<input class=\"biginput\" name=\"sortorder\" value=\"$sortorder\" size=3>";


	show_table2_v_delimiter2();
	if(!isset($triggerid)||($triggerid==""))
	{
		echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"add\">";
	}
	if(isset($_GET["serviceid"]))
	{
		echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"update\">";
		echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"delete\"  onClick=\"return Confirm('".S_DELETE_SERVICE_Q."');\">";
	}

	show_table2_header_end();
?>

<?php
	if(isset($_GET["serviceid"]))
	{
		$result=DBselect("select serviceid,triggerid,name from services where serviceid=".$_GET["serviceid"]);
		$triggerid=DBget_field($result,0,1);
		$name=DBget_field($result,0,2);
	}
	else
	{
		$name="";
		unset($_GET["triggerid"]);
	}

	show_form_begin("services.link");
	echo nbsp(S_LINK_TO);
	$col=0;

	show_table2_v_delimiter($col++);
	echo "<form method=\"post\" action=\"services.php\">";
	if(isset($_GET["serviceid"]))
	{
		echo "<input name=\"serviceid\" type=\"hidden\" value=".$_GET["serviceid"].">";
		echo "<input name=\"serviceupid\" type=\"hidden\" value=".$_GET["serviceid"].">";
	}
	echo S_NAME;
	show_table2_h_delimiter();
	$result=DBselect("select serviceid,triggerid,name from services order by name");
        echo "<select class=\"biginput\" name=\"servicedownid\" size=1>";
        for($i=0;$i<DBnum_rows($result);$i++)
        {
                $servicedownid_=DBget_field($result,$i,0);
//                $name_=DBget_field($result,$i,2);
//		if( strstr($name_,"%s"))
//		{
			if(DBget_field($result,$i,1)>0)
			{
				$name_=expand_trigger_description(DBget_field($result,$i,1));
			}
			else
			{
				$name_=DBget_field($result,$i,2);
			}
//		}
		echo "<OPTION VALUE='$servicedownid_'>$name_";
        }
        echo "</SELECT>";

        show_table2_v_delimiter($col++);
        echo nbsp(S_SOFT_LINK_Q);
        show_table2_h_delimiter();
//	if(isset($_GET["softlink"])&&($_GET["triggerid"]!=""))
//	{
//      	echo "<INPUT TYPE=\"CHECKBOX\" NAME=\"softlink\" VALUE=\"true\">";
//	}
//	else
//	{
//       	echo "<INPUT TYPE=\"CHECKBOX\" NAME=\"softlink\">";
//	}
	echo "<INPUT class=\"biginput\" TYPE=\"CHECKBOX\" NAME=\"softlink\" VALUE=\"true\" checked>";

	show_table2_v_delimiter2();
	echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"add link\">";

	show_table2_header_end();
?>

<?php
	if(isset($_GET["serviceid"]))
	{

	show_form_begin("services.server");
	echo nbsp(S_ADD_SERVER_DETAILS);
	$col=0;

	show_table2_v_delimiter($col++);
	echo "<form method=\"post\" action=\"services.php\">";
	if(isset($_GET["serviceid"]))
	{
		echo "<input name=\"serviceid\" type=\"hidden\" value=".$_GET["serviceid"].">";
	}
	echo S_SERVER;
	show_table2_h_delimiter();
	$result=DBselect("select hostid,host from hosts order by host");
        echo "<select class=\"biginput\" name=\"hostid\" size=1>";
        while($row=DBfetch($result))
        {
		echo "<OPTION VALUE='".$row["hostid"]."'>".$row["host"];
        }
        echo "</SELECT>";

	show_table2_v_delimiter2();
	echo "<input class=\"button\" type=\"submit\" name=\"register\" value=\"add server\">";

	show_table2_header_end();
	}

?>

<?php
	show_footer();
?>
