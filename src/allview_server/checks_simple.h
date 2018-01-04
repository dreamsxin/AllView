
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

#ifndef ALLVIEW_CHECKS_SIMPLE_H
#define ALLVIEW_CHECKS_SIMPLE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "db.h"
#include "log.h"

#include "../allview_agent/sysinfo.h"

extern	int	get_value_simple(double *result,char *result_str,DB_ITEM *item,char *error, int max_error_len);

#endif
