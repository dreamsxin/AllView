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
<?
	function	nbsp($str)
	{
		return str_replace(" ","&nbsp;",$str);;
	}

	function url1_param($parameter)
	{
		global $_GET;
	
		if(isset($_GET[$parameter]))
		{
			return "$parameter=".$_GET[$parameter];
		}
		else
		{
			return "";
		}
	}

	function url_param($parameter)
	{
		global $_GET;
	
		if(isset($_GET[$parameter]))
		{
			return "&$parameter=".$_GET[$parameter];
		}
		else
		{
			return "";
		}
	}

	function table_begin($class="tborder")
	{
		echo "<table class=\"$class\" border=0 width=\"100%\" bgcolor='#AAAAAA' cellspacing=1 cellpadding=3>";
		echo "\n";
	}

	function table_header($elements)
	{
		echo "<tr bgcolor='#CCCCCC'>";
		while(list($num,$element)=each($elements))
		{
			echo "<td><b>".$element."</b></td>";
		}
		echo "</tr>";
		echo "\n";
	}

	function table_row($elements, $rownum)
	{
		if($rownum%2 == 1)	{ echo "<TR BGCOLOR=\"#DDDDDD\">"; }
		else			{ echo "<TR BGCOLOR=\"#EEEEEE\">"; }

		while(list($num,$element)=each($elements))
		{
			if(is_array($element)&&isset($element["hide"])&&($element["hide"]==1))	continue;
			if(is_array($element))
			{
				if(isset($element["class"]))
					echo "<td class=\"".$element["class"]."\">".$element["value"]."</td>";
				else
					echo "<td>".$element["value"]."</td>";
			}
			else
			{
				echo "<td>".$element."</td>";
			}
		}
		echo "</tr>";
		echo "\n";
	}


	function table_end()
	{
		echo "</table>";
		echo "\n";
	}

	function table_td($text,$attr)
	{
		echo "<td $attr>$text</td>";
	}

	function table_nodata($text="...")
	{
		cr();
		echo "<TABLE BORDER=0 align=center WIDTH=\"100%\" BGCOLOR=\"#CCCCCC\" cellspacing=1 cellpadding=3>";
		echo "<TR BGCOLOR=\"#DDDDDD\">";
		echo "<TD ALIGN=CENTER>";
		echo $text;
		echo "</TD>";
		echo "</TR>";
		echo "</TABLE>";
		cr();
	}
?>
