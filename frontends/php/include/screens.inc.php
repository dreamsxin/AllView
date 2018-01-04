<?php
/*
** Allview
** Copyright (C) 2000,2001,2002,2003,2004 Allview team 
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**/
?>
<?php
        function        add_screen($name,$cols,$rows)
        {
                global  $ERROR_MSG;

                if(!check_right("Screen","A",0))
                {
                        $ERROR_MSG="Insufficient permissions";
                        return 0;
                }

                $sql="insert into screens (name,cols,rows) values ('$name',$cols,$rows)";
                return  DBexecute($sql);
        }

        function        update_screen($screenid,$name,$cols,$rows)
        {
                global  $ERROR_MSG;

                if(!check_right("Screen","U",0))
                {
                        $ERROR_MSG="Insufficient permissions";
                        return 0;
                }

                $sql="update screens set name='$name',cols=$cols,rows=$rows where screenid=$screenid";
                return  DBexecute($sql);
        }

        function        delete_screen($screenid)
        {
                $sql="delete from screens_items where screenid=$screenid";
                $result=DBexecute($sql);
                if(!$result)
                {
                        return  $result;
                }
                $sql="delete from screens where screenid=$screenid";
                return  DBexecute($sql);
        }

        function add_screen_item($resource,$screenid,$x,$y,$resourceid,$width,$height)
        {
                $sql="delete from screens_items where screenid=$screenid and x=$x and y=$y";
                DBexecute($sql);
                $sql="insert into screens_items (resource,screenid,x,y,resourceid,width,height) values ($resource,$screenid,$x,$y,$resourceid,$width,$height)";
                return  DBexecute($sql);
        }

        function update_screen_item($screenitemid,$resource,$resourceid,$width,$height)
        {
                $sql="update screens_items set resource=$resource,resourceid=$resourceid,width=$width,height=$height where screenitemid=$screenitemid";
                return  DBexecute($sql);
        }

        function delete_screen_item($screenitemid)
        {
                $sql="delete from screens_items where screenitemid=$screenitemid";
                return  DBexecute($sql);
        }

	function	get_screen_by_screenid($screenid)
	{
		global	$ERROR_MSG;

		$sql="select * from screens where screenid=$screenid"; 
		$result=DBselect($sql);
		if(DBnum_rows($result) == 1)
		{
			return	DBfetch($result);	
		}
		else
		{
			$ERROR_MSG="No screen with screenid=[$screenid]";
		}
		return	$result;
	}
?>
