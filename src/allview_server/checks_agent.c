
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

#include "checks_agent.h"

int	get_value_agent(double *result,char *result_str,DB_ITEM *item,char *error,int max_error_len)
{
	int	s;
	int	len;
	char	c[MAX_STRING_LEN];
	char	*e;

	struct hostent *hp;

	struct sockaddr_in servaddr_in;

	struct linger ling;

	allview_log( LOG_LEVEL_DEBUG, "get_value_agent: host[%s] ip[%s] key [%s]", item->host, item->ip, item->key );

	servaddr_in.sin_family=AF_INET;
	if(item->useip==1)
	{
		hp=gethostbyname(item->ip);
	}
	else
	{
		hp=gethostbyname(item->host);
	}

	if(hp==NULL)
	{
		allview_log( LOG_LEVEL_WARNING, "gethostbyname() failed [%s]", hstrerror(h_errno));
		snprintf(error,max_error_len-1,"gethostbyname() failed [%s]", hstrerror(h_errno));
		return	NETWORK_ERROR;
	}

	servaddr_in.sin_addr.s_addr=((struct in_addr *)(hp->h_addr))->s_addr;

	servaddr_in.sin_port=htons(item->port);

	s=socket(AF_INET,SOCK_STREAM,0);

	if(CONFIG_NOTIMEWAIT == 1)
	{
		ling.l_onoff=1;
		ling.l_linger=0;
		if(setsockopt(s,SOL_SOCKET,SO_LINGER,&ling,sizeof(ling))==-1)
		{
			allview_log(LOG_LEVEL_WARNING, "Cannot setsockopt SO_LINGER [%s]", strerror(errno));
		}
	}
	if(s == -1)
	{
		allview_log(LOG_LEVEL_WARNING, "Cannot create socket [%s]",
				strerror(errno));
		snprintf(error,max_error_len-1,"Cannot create socket [%s]", strerror(errno));
		return	FAIL;
	}
 
	if( connect(s,(struct sockaddr *)&servaddr_in,sizeof(struct sockaddr_in)) == -1 )
	{
		switch (errno)
		{
			case EINTR:
				allview_log( LOG_LEVEL_WARNING, "Timeout while connecting to [%s]",item->host);
				snprintf(error,max_error_len-1,"Timeout while connecting to [%s]",item->host);
				break;
			case EHOSTUNREACH:
				allview_log( LOG_LEVEL_WARNING, "No route to host [%s]",item->host);
				snprintf(error,max_error_len-1,"No route to host [%s]",item->host);
				break;
			default:
				allview_log( LOG_LEVEL_WARNING, "Cannot connect to [%s] [%s]",item->host, strerror(errno));
				snprintf(error,max_error_len-1,"Cannot connect to [%s] [%s]",item->host, strerror(errno));
		} 
		close(s);
		return	NETWORK_ERROR;
	}

	snprintf(c,sizeof(c)-1,"%s\n",item->key);
	allview_log(LOG_LEVEL_DEBUG, "Sending [%s]", c);
	if( write(s,c,strlen(c)) == -1 )
	{
		switch (errno)
		{
			case EINTR:
				allview_log( LOG_LEVEL_WARNING, "Timeout while sending data to [%s]",item->host );
				snprintf(error,max_error_len-1,"Timeout while sending data to [%s]",item->host);
				break;
			default:
				allview_log( LOG_LEVEL_WARNING, "Error while sending data to [%s] [%s]",item->host, strerror(errno));
				snprintf(error,max_error_len-1,"Error while sending data to [%s] [%s]",item->host, strerror(errno));
		} 
		close(s);
		return	FAIL;
	} 

	memset(c,0,MAX_STRING_LEN);
	len=read(s,c,MAX_STRING_LEN);
	if(len == -1)
	{
		switch (errno)
		{
			case 	EINTR:
					allview_log( LOG_LEVEL_WARNING, "Timeout while receiving data from [%s]",item->host );
					snprintf(error,max_error_len-1,"Timeout while receiving data from [%s]",item->host);
					break;
			case	ECONNRESET:
					allview_log( LOG_LEVEL_WARNING, "Connection reset by peer. Host [%s] Parameter [%s]",item->host, item->key);
					snprintf(error,max_error_len-1,"Connection reset by peer.");
					close(s);
					return	NETWORK_ERROR;
			default:
				allview_log( LOG_LEVEL_WARNING, "Error while receiving data from [%s] [%s]",item->host, strerror(errno));
				snprintf(error,max_error_len-1,"Error while receiving data from [%s] [%s]",item->host, strerror(errno));
		} 
		close(s);
		return	FAIL;
	}

	if( close(s)!=0 )
	{
		allview_log(LOG_LEVEL_WARNING, "Problem with close [%s]", strerror(errno));
	}
	allview_log(LOG_LEVEL_DEBUG, "Got string:[%d] [%s]", len, c);
	if(len>0)
	{
		c[len-1]=0;
	}

	*result=strtod(c,&e);

	/* The section should be improved */
	if( (*result==0) && (c==e) && (item->value_type==0) && (strcmp(c,"ALV_NOTSUPPORTED") != 0) && (strcmp(c,"ALV_ERROR") != 0) )
	{
		allview_log( LOG_LEVEL_WARNING, "Got empty string from [%s] IP [%s] Parameter [%s]", item->host, item->ip, item->key);
		allview_log( LOG_LEVEL_WARNING, "Assuming that agent dropped connection because of access permissions");
		snprintf(error,max_error_len-1,"Got empty string from [%s] IP [%s] Parameter [%s]", item->host, item->ip, item->key);
		return	NETWORK_ERROR;
	}

	/* Should be deleted in Allview 1.0 stable */
	if( cmp_double(*result,NOTSUPPORTED) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "NOTSUPPORTED1 [%s]", c );
		return NOTSUPPORTED;
	}
	if( strcmp(c,"ALV_NOTSUPPORTED") == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "NOTSUPPORTED2 [%s]", c );
		snprintf(error,max_error_len-1,"Not supported by ALLVIEW agent");
		return NOTSUPPORTED;
	}
	if( strcmp(c,"ALV_ERROR") == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "AGENT_ERROR [%s]", c );
		snprintf(error,max_error_len-1,"ALLVIEW agent non-critical error");
		return AGENT_ERROR;
	}

	strcpy(result_str,c);

	allview_log(LOG_LEVEL_DEBUG, "RESULT_STR [%s]", c );

	return SUCCEED;
}
