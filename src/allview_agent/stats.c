
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

#include "config.h"

#include <netdb.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

/* No warning for bzero */
#include <string.h>
#include <strings.h>

/* For config file operations */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* For setpriority */
#include <sys/time.h>
#include <sys/resource.h>

/* Required for getpwuid */
#include <pwd.h>

#include "common.h"
#include "sysinfo.h"
#include "security.h"
#include "allview_agent.h"

#include "log.h"
#include "cfg.h"
#include "stats.h"

/*INTERFACE interfaces[MAX_INTERFACE]=
{
	{0}
};*/

void	collect_statistics()
{
	FILE	*file;

	for(;;)
	{
		file=fopen("/tmp/allview_agentd.tmp2","w");
		if(NULL == file)
		{
			fprintf(stderr, "Cannot open file [%s] [%m]\n","/tmp/allview_agentd.tmp2");
		}
		else
		{
			/* Here is list of functions to call periodically */
			collect_stats_interfaces(file);
			collect_stats_diskdevices(file);
			collect_stats_cpustat(file);

			fclose(file);
			rename("/tmp/allview_agentd.tmp2","/tmp/allview_agentd.tmp");
		}
		sleep(1);
	}
}
