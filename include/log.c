
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

static	char log_filename[MAX_STRING_LEN];

static	int log_type = LOG_TYPE_UNDEFINED;
static	int log_level;

int allview_open_log(int type,int level, const char *filename)
{
	FILE *log_file = NULL;
/* Just return if we do not want to write debug */
	log_level = level;
	if(level == LOG_LEVEL_EMPTY)
	{
		return	SUCCEED;
	}

	if(type == LOG_TYPE_SYSLOG)
	{
        	openlog("allview_suckerd",LOG_PID,LOG_USER);
        	setlogmask(LOG_UPTO(LOG_WARNING));
		log_type = LOG_TYPE_SYSLOG;
	}
	else if(type == LOG_TYPE_FILE)
	{
		log_file = fopen(filename,"a+");
		if(log_file == NULL)
		{
			fprintf(stderr, "Unable to open debug file [%s] [%m]\n", filename);
			return	FAIL;
		}
		log_type = LOG_TYPE_FILE;
		strscpy(log_filename,filename);
		fclose(log_file);
	}
	else
	{
/* Not supported logging type */
		fprintf(stderr, "Not supported loggin type [%d]\n", type);
		return	FAIL;
	}
	return	SUCCEED;
}

void allview_set_log_level(int level)
{
	log_level = level;
}

void allview_log(int level, const char *fmt, ...)
{
	FILE *log_file = NULL;

	char	str[MAX_STRING_LEN];
	char	str2[MAX_STRING_LEN];
	time_t	t;
	struct	tm	*tm;
	va_list ap;

	struct stat	buf;
	char	filename_old[MAX_STRING_LEN];

	if( (level>log_level) || (level == LOG_LEVEL_EMPTY))
	{
		return;
	}

	if(log_type == LOG_TYPE_SYSLOG)
	{
		va_start(ap,fmt);
		vsprintf(str,fmt,ap);
		strncat(str,"\n",MAX_STRING_LEN);
		str[MAX_STRING_LEN]=0;
		syslog(LOG_DEBUG,str);
		va_end(ap);
	}
	else if(log_type == LOG_TYPE_FILE)
	{
		t=time(NULL);
		tm=localtime(&t);
		snprintf(str2,sizeof(str2)-1,"%.6d:%.4d%.2d%.2d:%.2d%.2d%.2d ",(int)getpid(),tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

		va_start(ap,fmt);
		vsnprintf(str,MAX_STRING_LEN,fmt,ap);

		log_file = fopen(log_filename,"a+");
		if(log_file == NULL)
		{
			return;
		}
		fprintf(log_file,"%s",str2);
		fprintf(log_file,"%s",str);
		fprintf(log_file,"\n");
		fclose(log_file);
		va_end(ap);


		if(stat(log_filename,&buf) == 0)
		{
			if(buf.st_size>1024*1024)
			{
				strscpy(filename_old,log_filename);
				strncat(filename_old,".old",MAX_STRING_LEN);
				if(rename(log_filename,filename_old) != 0)
				{
/*					exit(1);*/
				}
			}
		}
	}
	else
	{
		/* Log is not opened */
	}	
        return;
}

