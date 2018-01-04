
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

#ifndef ALLVIEW_DISKDEVICES_H
#define ALLVIEW_DISKDEVICES_H

#define	MAX_DISKDEVICES	8

#define DISKDEVICE struct diskdevice_type
DISKDEVICE
{
	char    *device;
	int	major;
	int	diskno;
	int	clock[60*15];
	float	read_io_ops[60*15];
	float	blks_read[60*15];
	float	write_io_ops[60*15];
	float	blks_write[60*15];
};

void	collect_stats_diskdevices(FILE *outfile);

#endif
