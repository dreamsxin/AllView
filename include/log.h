
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

#ifndef ALLVIEW_LOG_H
#define ALLVIEW_LOG_H

#define LOG_LEVEL_EMPTY		0
#define LOG_LEVEL_CRIT		1
#define LOG_LEVEL_ERR		2
#define LOG_LEVEL_WARNING	3
#define LOG_LEVEL_DEBUG		4

#define LOG_TYPE_UNDEFINED	0
#define LOG_TYPE_SYSLOG		1
#define LOG_TYPE_FILE		2

/* Type - 0 (syslog), 1 - file */
int allview_open_log(int type,int level, const char *filename);
void allview_log(int level, const char *fmt, ...);
void allview_set_log_level(int level);

#endif
