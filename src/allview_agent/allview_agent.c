
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

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* For bcopy */
#include <string.h>

/* For config file operations */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "cfg.h"
#include "log.h"
#include "sysinfo.h"
#include "security.h"
#include "allview_agent.h"

static	char	*CONFIG_HOSTS_ALLOWED	= NULL;
static	int	CONFIG_TIMEOUT		= AGENT_TIMEOUT;

void	signal_handler( int sig )
{
	if( SIGALRM == sig )
	{
		signal( SIGALRM, signal_handler );
	}
 
	if( SIGQUIT == sig || SIGINT == sig || SIGTERM == sig )
	{
	}
	exit( FAIL );
}

int	add_parameter(char *value)
{
	char	*value2;

	value2=strstr(value,",");
	if(NULL == value2)
	{
		return	FAIL;
	}
	value2[0]=0;
	value2++;
	add_user_parameter(value, value2);
	return	SUCCEED;
}

void    init_config(void)
{
	struct cfg_line cfg[]=
	{
/*               PARAMETER      ,VAR    ,FUNC,  TYPE(0i,1s),MANDATORY,MIN,MAX
*/
		{"Server",&CONFIG_HOSTS_ALLOWED,0,TYPE_STRING,PARM_MAND,0,0},
		{"Timeout",&CONFIG_TIMEOUT,0,TYPE_INT,PARM_OPT,1,30},
		{"UserParameter",0,&add_parameter,0,0,0,0},
		{0}
	};

	parse_cfg_file("/etc/allview/allview_agent.conf",cfg);
}

int	main()
{
	char	s[MAX_STRING_LEN];
	char	value[MAX_STRING_LEN];

#ifdef	TEST_PARAMETERS
	init_config();
	test_parameters();
	return	SUCCEED;
#endif

	signal( SIGINT,  signal_handler );
	signal( SIGQUIT, signal_handler );
	signal( SIGTERM, signal_handler );
	signal( SIGALRM, signal_handler );

/* Must be before init_config() */
	init_metrics();
	init_config();

/* Do not create debug files */
	allview_open_log(LOG_TYPE_SYSLOG,LOG_LEVEL_EMPTY,NULL);

	alarm(CONFIG_TIMEOUT);

	if(check_security(0,CONFIG_HOSTS_ALLOWED,0) == FAIL)
	{
		exit(FAIL);
	}

	fgets(s,MAX_STRING_LEN,stdin);
	process(s,value);

	printf("%s\n",value);

	fflush(stdout);

	alarm(0);

	return SUCCEED;
}
