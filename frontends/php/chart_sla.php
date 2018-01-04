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

#	PARAMETERS:
	
#	itemid
#	period
#	from

	$sizeX=200;
	$sizeY=15;

//	Header( "Content-type:  text/html"); 
	Header( "Content-type:  image/png"); 
	Header( "Expires:  Mon, 17 Aug 1998 12:51:50 GMT"); 

	check_authorisation();

	$im = imagecreate($sizeX,$sizeY); 
  
	$red=ImageColorAllocate($im,255,0,0); 
	$darkred=ImageColorAllocate($im,150,0,0); 
	$green=ImageColorAllocate($im,0,255,0); 
	$darkgreen=ImageColorAllocate($im,0,150,0); 
	$blue=ImageColorAllocate($im,0,0,255); 
	$yellow=ImageColorAllocate($im,255,255,0); 
	$cyan=ImageColorAllocate($im,0,255,255); 
	$black=ImageColorAllocate($im,0,0,0); 
	$gray=ImageColorAllocate($im,150,150,150); 
	$white=ImageColorAllocate($im,255,255,255); 

	ImageFilledRectangle($im,0,0,$sizeX,$sizeY,ImageColorAllocate($im,120,200,120));

	$now=time(NULL);
	$period_start=$now-7*24*3600;
	$period_end=$now;
	$service=get_service_by_serviceid($_GET["serviceid"]);
	$stat=calculate_service_availability($_GET["serviceid"],$period_start,$period_end);
		
	$problem=$stat["problem"];
	$ok=$stat["ok"];

//	echo $problem," ",$ok;

// for test
//	$problem=81;
//	$service["goodsla"]=81;

//	$p=min(100-$problem,20);
	$p=min($problem,20);
	$g=max($service["goodsla"]-80,0);

	ImageFilledRectangle($im,$sizeX-$sizeX*$p/20,1,$sizeX-2,$sizeY-2,ImageColorAllocate($im,200,120,120));
	ImageLine($im,$sizeX*$g/20,1,$sizeX*$g/20,$sizeY-1,$yellow);

	ImageRectangle($im,0,0,$sizeX-1,$sizeY-1,$black);

	$s=sprintf("%2.2f%%",$ok);
	ImageString($im, 2,1,1, $s , $white);
	$s=sprintf("%2.2f%%", $problem);
	ImageString($im, 2,$sizeX-45,1, $s , $white);
	ImageOut($im); 
	ImageDestroy($im); 
?>
