
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


#ifndef ALLVIEW_EMAIL_H
#define ALLVIEW_EMAIL_H

int	send_email(char *smtp_server,char *smtp_helo,char *smtp_email,char *mailto,char *mailsubject,char *mailbody, char *error, int max_error_len);

#endif
