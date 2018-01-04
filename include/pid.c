
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


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <time.h>

#include "log.h"
#include "common.h"

int	create_pid_file(const char *pidfile)
{
	FILE	*f;

/* Check if PID file already exists */
	f = fopen(pidfile, "r");
	if(f != NULL)
	{
		fprintf(stderr, "File [%s] exists. Is this process already running ?\n",
			pidfile);
		allview_log( LOG_LEVEL_CRIT, "File [%s] exists. Is this process already running ?",
			pidfile);
		if(fclose(f) != 0)
		{
			fprintf(stderr, "Cannot close file [%s] [%m]", pidfile);
			allview_log( LOG_LEVEL_WARNING, "Cannot close file [%s] [%m]",
				pidfile);
		}

		return FAIL;
	}

	f = fopen(pidfile, "w");

	if( f == NULL)
	{
		fprintf(stderr, "Cannot create PID file [%s] [%m]\n",
			pidfile);
		allview_log( LOG_LEVEL_CRIT, "Cannot create PID file [%s] [%m]",
			pidfile);

		return FAIL;
	}

	fprintf(f,"%d",(int)getpid());
	if(fclose(f) != 0)
	{
		fprintf(stderr, "Cannot close file [%s] [%m]", pidfile);
		allview_log( LOG_LEVEL_WARNING, "Cannot close file [%s] [%m]",
			pidfile);
	}

	return SUCCEED;
}
