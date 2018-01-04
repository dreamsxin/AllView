
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

#ifndef ALLVIEW_CHECKS_AGENT_H
#define ALLVIEW_CHECKS_AGENT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "config.h"

#ifdef HAVE_NETDB_H
	#include <netdb.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "db.h"
#include "log.h"
#include "expression.h"

extern  int     CONFIG_NOTIMEWAIT;

extern	int	get_value_agent(double *result,char *result_str,DB_ITEM *item,char *error,int max_error_len);

#endif
