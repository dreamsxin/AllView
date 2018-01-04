
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

#ifndef ALLVIEW_CPUSTAT_H
#define ALLVIEW_CPUSTAT_H

#define CPUSTAT struct cpustat_type
CPUSTAT
{
	char    *device;
	int	major;
	int	diskno;
	int	clock[60*15];
	float	cpu_user[60*15];
	float	cpu_system[60*15];
	float	cpu_nice[60*15];
	float	cpu_idle[60*15];
};

void	collect_stats_cpustat(FILE *outfile);

#endif
