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

#	imageid

#	Header( "Content-type:  text/html"); 
	Header( "Content-type:  image/png"); 
	Header( "Expires:  Mon, 17 Aug 1998 12:51:50 GMT");

	check_authorisation();

	$result=DBselect("select image from images where imageid=".$_GET["imageid"]);
	$row=DBfetch($result);
	$image=ImageCreateFromString($row["image"]);

	ImageOut($image);
	ImageDestroy($image);
?>
