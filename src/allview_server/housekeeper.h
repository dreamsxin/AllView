
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

#ifndef ALLVIEW_HOUSEKEEPER_H
#define ALLVIEW_HOUSEKEEPER_H

extern	int	CONFIG_DISABLE_HOUSEKEEPING;
extern	int	CONFIG_HOUSEKEEPING_FREQUENCY;
extern	char	*CONFIG_DBHOST;
extern	char	*CONFIG_DBNAME;
extern	char	*CONFIG_DBUSER;
extern	char	*CONFIG_DBPASSWORD;
extern	char	*CONFIG_DBSOCKET;
extern	char	*CONFIG_FPING_LOCATION;

#ifdef ALLVIEW_THREADS
void *main_housekeeper_loop();
#else
int main_housekeeper_loop();
#endif

#endif
