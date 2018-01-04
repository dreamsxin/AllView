
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

#include "checks_simple.h"

int	get_value_simple(double *result,char *result_str,DB_ITEM *item,char *error, int max_error_len)
{
	char	*e,*t;
	char	c[MAX_STRING_LEN];
	char	s[MAX_STRING_LEN];
	int	ret = SUCCEED;

	/* The code is ugly. I would rewrite it. Allview team.	*/
	/* Assumption: host name does not contain '_perf'	*/
	if(NULL == strstr(item->key,"_perf"))
	{
		if(item->useip==1)
		{
			snprintf(c,sizeof(c)-1,"check_service[%s,%s]",item->key,item->ip);
		}
		else
		{
			snprintf(c,sizeof(c)-1,"check_service[%s,%s]",item->key,item->host);
		}
	}
	else
	{
		strscpy(s,item->key);
		t=strstr(s,"_perf");
		t[0]=0;
		
		if(item->useip==1)
		{
			snprintf(c,sizeof(c)-1,"check_service_perf[%s,%s]",s,item->ip);
		}
		else
		{
			snprintf(c,sizeof(c)-1,"check_service_perf[%s,%s]",s,item->host);
		}
	}


	process(c,result_str);

	if(strcmp(result_str,"ALV_NOTSUPPORTED\n") == 0)
	{
		allview_log( LOG_LEVEL_WARNING, "Simple check [%s] is not supported", c);
		snprintf(error,max_error_len-1,"Simple check [%s] is not supported", c);
		ret = NOTSUPPORTED;
	}
	else
	{
		*result=strtod(result_str,&e);
	}

	allview_log( LOG_LEVEL_DEBUG, "SIMPLE [%s] [%s] [%f] RET [%d]", c, result_str, *result, ret);
	return ret;
}
