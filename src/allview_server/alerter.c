
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
#include "zlog.h"
#include "email.h"

#include "alerter.h"

void	signal_handler2( int sig )
{
	allview_log( LOG_LEVEL_DEBUG, "Got signal [%d]", sig);
}

/* Send alert */
int send_alert(DB_ALERT	*alert,DB_MEDIATYPE *mediatype, char *error, int max_error_len)
{
	int res=FAIL;
	struct	sigaction phan;
	int	pid;

	char	full_path[MAX_STRING_LEN];

	allview_log( LOG_LEVEL_DEBUG, "In send_alert()");

	if(mediatype->type==ALERT_TYPE_EMAIL)
	{
		res = send_email(mediatype->smtp_server,mediatype->smtp_helo,mediatype->smtp_email,alert->sendto,alert->subject,
			alert->message, error, max_error_len);
	}
	else if(mediatype->type==ALERT_TYPE_EXEC)
	{
/*		if(-1 == execl(CONFIG_ALERT_SCRIPTS_PATH,mediatype->exec_path,alert->sendto,alert->subject,alert->message))*/
		allview_log( LOG_LEVEL_DEBUG, "Before execl([%s],[%s])",CONFIG_ALERT_SCRIPTS_PATH,mediatype->exec_path);

		phan.sa_handler = &signal_handler2;
		phan.sa_handler = SIG_IGN;
/*		signal( SIGCHLD, SIG_IGN );*/

		sigemptyset(&phan.sa_mask);
		phan.sa_flags = 0;
		sigaction(SIGCHLD, &phan, NULL);

/*		if(-1 == execl("/home/allview/bin/lmt.sh","lmt.sh",alert->sendto,alert->subject,alert->message,(char *)0))*/

		pid=fork();
		if(0 != pid)
		{
			waitpid(pid,NULL,0);
		}
		else
		{
			strscpy(full_path,CONFIG_ALERT_SCRIPTS_PATH);
			strncat(full_path,"/",MAX_STRING_LEN);
			strncat(full_path,mediatype->exec_path,MAX_STRING_LEN);
			allview_log( LOG_LEVEL_DEBUG, "Before executing [%s] [%m]", full_path);
			if(-1 == execl(full_path,mediatype->exec_path,alert->sendto,alert->subject,alert->message,(char *)0))
			{
				allview_log( LOG_LEVEL_ERR, "Error executing [%s] [%s]", full_path, strerror(errno));
				allview_syslog("Error executing [%s] [%s]", full_path, strerror(errno));
				snprintf(error,max_error_len-1,"Error executing [%s] [%s]", full_path, strerror(errno));
				res = FAIL;
			}
			else
			{
				res = SUCCEED;
			}
			/* In normal case the program will never reach this point */
			allview_log( LOG_LEVEL_DEBUG, "After execl()");
			exit(0);
		}
		res = SUCCEED;
	}
	else
	{
		allview_log( LOG_LEVEL_ERR, "Unsupported media type [%d] for alert ID [%d]", mediatype->type,alert->alertid);
		allview_syslog("Unsupported media type [%d] for alert ID [%d]", mediatype->type,alert->alertid);
		snprintf(error,max_error_len-1,"Unsupported media type [%d]", mediatype->type);
		res=FAIL;
	}

	allview_log( LOG_LEVEL_DEBUG, "End of send_alert()");

	return res;
}

#ifdef ALLVIEW_THREADS
void *main_alerter_loop()
#else
int main_alerter_loop()
#endif
{
	char	sql[MAX_STRING_LEN];
	char	error[MAX_STRING_LEN];
	char	error_esc[MAX_STRING_LEN];

	int	i,res;

	struct	sigaction phan;

#ifdef ALLVIEW_THREADS
	DB_HANDLE	database;
#endif

	DB_RESULT	*result;
	DB_ALERT	alert;
	DB_MEDIATYPE	mediatype;

#ifdef ALLVIEW_THREADS
	my_thread_init();
#endif

	for(;;)
	{
#ifdef HAVE_FUNCTION_SETPROCTITLE
		setproctitle("connecting to the database");
#endif

#ifdef	ALLVIEW_THREADS
		DBconnect_thread(&database);
#else
		DBconnect();
#endif

		snprintf(sql,sizeof(sql)-1,"select a.alertid,a.mediatypeid,a.sendto,a.subject,a.message,a.status,a.retries,mt.mediatypeid,mt.type,mt.description,mt.smtp_server,mt.smtp_helo,mt.smtp_email,mt.exec_path from alerts a,media_type mt where a.status=0 and a.retries<3 and a.mediatypeid=mt.mediatypeid order by a.clock");
#ifdef	ALLVIEW_THREADS
		result = DBselect_thread(&database, sql);
#else
		result = DBselect(sql);
#endif

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
			res=send_alert(&alert,&mediatype,error,sizeof(error));
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
				allview_syslog("Error sending alert ID [%d]", alert.alertid);
				DBescape_string(error,error_esc,MAX_STRING_LEN);
				snprintf(sql,sizeof(sql)-1,"update alerts set retries=retries+1,error='%s' where alertid=%d", error_esc, alert.alertid);
#ifdef	ALLVIEW_THREADS
				DBexecute_thread(&database,sql);
#else
				DBexecute(sql);
#endif
			}

		}
		DBfree_result(result);

#ifdef	ALLVIEW_THREADS
		DBclose_thread(&database);
#else
		DBclose();
#endif
#ifdef HAVE_FUNCTION_SETPROCTITLE
		setproctitle("sender [sleeping for %d seconds]", CONFIG_SENDER_FREQUENCY);
#endif

		sleep(CONFIG_SENDER_FREQUENCY);
	}
}
