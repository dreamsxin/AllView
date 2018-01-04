
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

#ifndef ALLVIEW_INTERFACES_H
#define ALLVIEW_INTERFACES_H

#define	MAX_INTERFACE	16

#define INTERFACE struct interface_type
INTERFACE
{
	char    *interface;
	int	clock[60*15];
	float	sent[60*15];
	float	received[60*15];
};

void	collect_stats_interfaces(FILE *outfile);

#endif
