
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

#ifndef ALLVIEW_ALERTER_H
#define ALLVIEW_ALERTER_H

extern	int	CONFIG_SENDER_FREQUENCY;
extern	char	*CONFIG_DBHOST;
extern	char	*CONFIG_DBNAME;
extern	char	*CONFIG_DBUSER;
extern	char	*CONFIG_DBPASSWORD;
extern	char	*CONFIG_DBSOCKET;
extern	char	*CONFIG_ALERT_SCRIPTS_PATH;

extern	void	signal_handler( int sig );

#ifdef ALLVIEW_THREADS
void *main_alerter_loop();
#else
int main_alerter_loop();
#endif

#endif
