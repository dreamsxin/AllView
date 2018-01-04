
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


#ifndef ALLVIEW_EXPRESSION_H
#define ALLVIEW_EXPRESSION_H

#include "common.h"
#include "db.h"

int	cmp_double(double a,double b);
int	find_char(char *str,char c);
int	substitute_functions(char *exp);
int	substitute_macros(DB_TRIGGER *trigger, DB_ACTION *action, char *exp);
int     evaluate_expression (int *result,char *expression);

#ifdef ALLVIEW_THREADS
int	substitute_macros_thread(MYSQL *database, DB_TRIGGER *trigger, DB_ACTION *action, char *exp);
int	evaluate_expression_thread(MYSQL *database, int *result,char *expression);
#endif
	
#endif
