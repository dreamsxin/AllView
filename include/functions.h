
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


#ifndef ALLVIEW_FUNCTIONS_H
#define ALLVIEW_FUNCTIONS_H

#include "common.h"
#include "db.h"

#define	EVALUATE_FUNCTION_NORMAL	0
#define	EVALUATE_FUNCTION_SUFFIX	1

void    update_triggers (int itemid);
int	get_lastvalue(char *value,char *host,char *key,char *function,char *parameter);
int	process_data(int sockfd,char *server,char *key, char *value);
void	process_new_value(DB_ITEM *item,char *value);

#ifdef ALLVIEW_THREADS
void	update_triggers_thread(MYSQL *database, int itemid);
void	process_new_value_thread(MYSQL *database, DB_ITEM *item,char *value);
void	update_services_thread(MYSQL *database, int triggerid, int status);
void	update_serv_thread(MYSQL *database,int serviceid);
void	apply_actions_thread(MYSQL *database, DB_TRIGGER *trigger,int trigger_value);
void	send_to_user_thread(MYSQL *database, DB_TRIGGER *trigger,DB_ACTION *action);
void	update_functions_thread(MYSQL *database, DB_ITEM *item);
int	get_lastvalue_thread(MYSQL *database, char *value,char *host,char *key,char *function,char *parameter);
#endif

#endif
