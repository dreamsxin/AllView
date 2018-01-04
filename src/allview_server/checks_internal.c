
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

#include "common.h"
#include "checks_internal.h"

#ifdef ALLVIEW_THREADS
int	get_value_internal(double *result,char *result_str,DB_ITEM *item,char *error,int max_error_len)
{
	DB_HANDLE	database;

	DBconnect_thread(&database);

	if(strcmp(item->key,"allview[triggers]")==0)
	{
		*result=DBget_triggers_count_thread(&database);
	}
	else if(strcmp(item->key,"allview[items]")==0)
	{
		*result=DBget_items_count_thread(&database);
	}
	else if(strcmp(item->key,"allview[items_unsupported]")==0)
	{
		*result=DBget_items_unsupported_count_thread(&database);
	}
	else if(strcmp(item->key,"allview[history]")==0)
	{
		*result=DBget_history_count_thread(&database);
	}
	else if(strcmp(item->key,"allview[history_str]")==0)
	{
		*result=DBget_history_str_count_thread(&database);
	}
	else if(strcmp(item->key,"allview[trends]")==0)
	{
		*result=DBget_trends_count_thread(&database);
	}
	else if(strcmp(item->key,"allview[queue]")==0)
	{
		*result=DBget_queue_count_thread(&database);
	}
	else
	{
		DBclose_thread(&database);
		allview_log( LOG_LEVEL_WARNING, "Internal check [%s] is not supported", item->key);
		snprintf(error,max_error_len-1,"Internal check [%s] is not supported", item->key);
		return NOTSUPPORTED;
	}

	snprintf(result_str,MAX_STRING_LEN-1,"%f",*result);

	allview_log( LOG_LEVEL_DEBUG, "INTERNAL [%s] [%f]", result_str, *result);

	DBclose_thread(&database);

	return SUCCEED;
}

#else
int	get_value_internal(double *result,char *result_str,DB_ITEM *item,char *error,int max_error_len)
{
	if(strcmp(item->key,"allview[triggers]")==0)
	{
		*result=DBget_triggers_count();
	}
	else if(strcmp(item->key,"allview[items]")==0)
	{
		*result=DBget_items_count();
	}
	else if(strcmp(item->key,"allview[items_unsupported]")==0)
	{
		*result=DBget_items_unsupported_count();
	}
	else if(strcmp(item->key,"allview[history]")==0)
	{
		*result=DBget_history_count();
	}
	else if(strcmp(item->key,"allview[history_str]")==0)
	{
		*result=DBget_history_str_count();
	}
	else if(strcmp(item->key,"allview[trends]")==0)
	{
		*result=DBget_trends_count();
	}
	else if(strcmp(item->key,"allview[queue]")==0)
	{
		*result=DBget_queue_count();
	}
	else
	{
		allview_log( LOG_LEVEL_WARNING, "Internal check [%s] is not supported", item->key);
		snprintf(error,max_error_len-1,"Internal check [%s] is not supported", item->key);
		return NOTSUPPORTED;
	}

	snprintf(result_str,MAX_STRING_LEN-1,"%f",*result);

	allview_log( LOG_LEVEL_DEBUG, "INTERNAL [%s] [%f]", result_str, *result);
	return SUCCEED;
}
#endif
