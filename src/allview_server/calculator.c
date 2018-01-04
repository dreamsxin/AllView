
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/wait.h>

#include <string.h>

#ifdef HAVE_NETDB_H
	#include <netdb.h>
#endif

/* Required for getpwuid */
#include <pwd.h>

#include <signal.h>
#include <errno.h>

#include <time.h>

#include "common.h"
#include "cfg.h"
#include "db.h"
#include "functions.h"
#include "log.h"

#include "calculator.h"

int calculator_loop()
{
	char	sql[MAX_STRING_LEN];

	int	i,res;

	struct	sigaction phan;

	DB_RESULT	*result;
	DB_ALERT	alert;
	DB_MEDIATYPE	mediatype;

	for(;;)
	{
#ifdef HAVE_FUNCTION_SETPROCTITLE
		setproctitle("connecting to the database");
#endif
		DBconnect(CONFIG_DBHOST, CONFIG_DBNAME, CONFIG_DBUSER, CONFIG_DBPASSWORD, CONFIG_DBSOCKET);

		snprintf(sql,sizeof(sql)-1,"select a.alertid,a.mediatypeid,a.sendto,a.subject,a.message,a.status,a.retries,mt.mediatypeid,mt.type,mt.description,mt.smtp_server,mt.smtp_helo,mt.smtp_email,mt.exec_path from alerts a,media_type mt where a.status=0 and a.retries<3 and a.mediatypeid=mt.mediatypeid order by a.clock");
		result = DBselect(sql);

		for(i=0;i<DBnum_rows(result);i++)
		{
			alert.alertid=atoi(DBget_field(result,i,0));
			alert.mediatypeid=atoi(DBget_field(result,i,1));
			alert.sendto=DBget_field(result,i,2);
			alert.subject=DBget_field(result,i,3);
			alert.message=DBget_field(result,i,4);
			alert.status=atoi(DBget_field(result,i,5));
			alert.retries=atoi(DBget_field(result,i,6));

			mediatype.mediatypeid=atoi(DBget_field(result,i,7));
			mediatype.type=atoi(DBget_field(result,i,8));
			mediatype.description=DBget_field(result,i,9);
			mediatype.smtp_server=DBget_field(result,i,10);
			mediatype.smtp_helo=DBget_field(result,i,11);
			mediatype.smtp_email=DBget_field(result,i,12);
			mediatype.exec_path=DBget_field(result,i,13);

			phan.sa_handler = &signal_handler;
			sigemptyset(&phan.sa_mask);
			phan.sa_flags = 0;
			sigaction(SIGALRM, &phan, NULL);

			/* Hardcoded value */
			alarm(10);
			res=send_alert(&alert,&mediatype);
			alarm(0);

			if(res==SUCCEED)
			{
				allview_log( LOG_LEVEL_DEBUG, "Alert ID [%d] was sent successfully", alert.alertid);
				snprintf(sql,sizeof(sql)-1,"update alerts set status=1 where alertid=%d", alert.alertid);
				DBexecute(sql);
			}
			else
			{
				allview_log( LOG_LEVEL_ERR, "Error sending alert ID [%d]", alert.alertid);
				snprintf(sql,sizeof(sql)-1,"update alerts set retries=retries+1 where alertid=%d", alert.alertid);
				DBexecute(sql);
			}

		}
		DBfree_result(result);

		DBclose();
#ifdef HAVE_FUNCTION_SETPROCTITLE
		setproctitle("sender [sleeping for %d seconds]", CONFIG_SENDER_FREQUENCY);
#endif
		sleep(CONFIG_SENDER_FREQUENCY);
	}
}
