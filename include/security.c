
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


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include <errno.h>

/* config.h is required for socklen_t (undefined under Solaris) */
#include "config.h"
#include "common.h"
#include "log.h"

int	check_security(int sockfd, char *ip_list, int allow_if_empty)
{
	char	*sname;
	struct	sockaddr_in name;
	int	i;
	char	*s;

	char	tmp[MAX_STRING_LEN];

        allview_log( LOG_LEVEL_DEBUG, "In check_security()");

	if( (1 == allow_if_empty) && (strlen(ip_list)==0) )
	{
		return SUCCEED;
	}

	i=sizeof(name);

/*	if(getpeername(sockfd,  (struct sockaddr *)&name, (size_t *)&i) == 0)*/
	if(getpeername(sockfd,  (struct sockaddr *)&name, (socklen_t *)&i) == 0)
	{
		i=sizeof(struct sockaddr_in);

		sname=inet_ntoa(name.sin_addr);

		allview_log( LOG_LEVEL_DEBUG, "Connection from [%s]. Allowed servers [%s] ",sname, ip_list);

		strscpy(tmp,ip_list);
        	s=(char *)strtok(tmp,",");
		while(s!=NULL)
		{
			if(strcmp(sname, s)==0)
			{
				return	SUCCEED;
			}
                	s=(char *)strtok(NULL,",");
		}
	}
	else
	{
		allview_log( LOG_LEVEL_WARNING, "Error getpeername [%s]",strerror(errno));
		allview_log( LOG_LEVEL_WARNING, "Connection rejected");
		return FAIL;
	}
	allview_log( LOG_LEVEL_WARNING, "Connection from [%s] rejected. Allowed server is [%s] ",sname, ip_list);
	return	FAIL;
}
