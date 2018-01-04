
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

#include "pinger.h"

/* Could be improved */
int	is_ip(char *ip)
{
	int i;
	char c;
	int dots=0;
	int res = SUCCEED;

	allview_log( LOG_LEVEL_DEBUG, "In process_ip([%s])", ip);

	for(i=0;ip[i]!=0;i++)
	{
		c=ip[i];
		if( (c>='0') && (c<='9'))
		{
			continue;
		}
		else if(c=='.')
		{
			dots++;
		}
		else
		{
			res = FAIL;
			break;
		}
	}
	if( dots!=3)
	{
		res = FAIL;
	}
	allview_log( LOG_LEVEL_DEBUG, "End of process_ip([%d])", res);
	return res;
}

#ifdef ALLVIEW_THREADS
int	process_value(MYSQL *database, char *key, char *host, char *value)
#else
int	process_value(char *key, char *host, char *value)
#endif
{
	char	sql[MAX_STRING_LEN];

	DB_RESULT       *result;
	DB_ITEM	item;
	char	*s;

	allview_log( LOG_LEVEL_DEBUG, "In process_value()");

	/* IP address? */
	if(is_ip(host) == SUCCEED)
	{
		snprintf(sql,sizeof(sql)-1,"select i.itemid,i.key_,h.host,h.port,i.delay,i.description,i.nextcheck,i.type,i.snmp_community,i.snmp_oid,h.useip,h.ip,i.history,i.lastvalue,i.prevvalue,i.value_type,i.trapper_hosts,i.delta,i.units,i.multiplier,i.formula from items i,hosts h where h.status=%d and h.hostid=i.hostid and h.ip='%s' and i.key_='%s' and i.status=%d and i.type=%d", HOST_STATUS_MONITORED, host, key, ITEM_STATUS_ACTIVE, ITEM_TYPE_SIMPLE);
	}
	else
	{
		snprintf(sql,sizeof(sql)-1,"select i.itemid,i.key_,h.host,h.port,i.delay,i.description,i.nextcheck,i.type,i.snmp_community,i.snmp_oid,h.useip,h.ip,i.history,i.lastvalue,i.prevvalue,i.value_type,i.trapper_hosts,i.delta,i.units,i.multiplier,i.formula from items i,hosts h where h.status=%d and h.hostid=i.hostid and h.host='%s' and i.key_='%s' and i.status=%d and i.type=%d", HOST_STATUS_MONITORED, host, key, ITEM_STATUS_ACTIVE, ITEM_TYPE_SIMPLE);
	}
	allview_log( LOG_LEVEL_DEBUG, "SQL [%s]", sql);
#ifdef ALLVIEW_THREADS
	result = DBselect_thread(database, sql);
#else
	result = DBselect(sql);
#endif

	if(DBnum_rows(result) == 0)
	{
		DBfree_result(result);
		return  FAIL;
	}

	item.itemid=atoi(DBget_field(result,0,0));
	item.key=DBget_field(result,0,1);
	item.host=DBget_field(result,0,2);
	item.port=atoi(DBget_field(result,0,3));
	item.delay=atoi(DBget_field(result,0,4));
	item.description=DBget_field(result,0,5);
	item.nextcheck=atoi(DBget_field(result,0,6));
	item.type=atoi(DBget_field(result,0,7));
	item.snmp_community=DBget_field(result,0,8);
	item.snmp_oid=DBget_field(result,0,9);
	item.useip=atoi(DBget_field(result,0,10));
	item.ip=DBget_field(result,0,11);
	item.history=atoi(DBget_field(result,0,12));
	s=DBget_field(result,0,13);
	if(s==NULL)
	{
		item.lastvalue_null=1;
	}
	else
	{
		item.lastvalue_null=0;
		item.lastvalue_str=s;
		item.lastvalue=atof(s);
	}
	s=DBget_field(result,0,14);
	if(s==NULL)
	{
		item.prevvalue_null=1;
	}
	else
	{
		item.prevvalue_null=0;
		item.prevvalue_str=s;
		item.prevvalue=atof(s);
	}
	item.value_type=atoi(DBget_field(result,0,15));
	item.trapper_hosts=DBget_field(result,0,16);
	item.delta=atoi(DBget_field(result,0,17));

	item.units=DBget_field(result,0,18);
	item.multiplier=atoi(DBget_field(result,0,19));
	item.formula=DBget_field(result,0,20);

#ifdef ALLVIEW_THREADS
	process_new_value_thread(database,&item,value);
	update_triggers_thread(database,item.itemid);
#else
	process_new_value(&item,value);
	update_triggers(item.itemid);
#endif
 
	DBfree_result(result);

	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
int create_host_file(MYSQL *database)
#else
int create_host_file(void)
#endif
{
	char	sql[MAX_STRING_LEN];
	FILE	*f;
	int	i,now;

	DB_HOST	host;
	DB_RESULT	*result;

	allview_log( LOG_LEVEL_DEBUG, "In create_host_file()");

	f = fopen("/tmp/allview_suckerd.pinger", "w");

	if( f == NULL)
	{
		allview_log( LOG_LEVEL_ERR, "Cannot open file [%s] [%s]", "/tmp/allview_suckerd.pinger", strerror(errno));
		allview_syslog("Cannot open file [%s] [%s]", "/tmp/allview_suckerd.pinger", strerror(errno));
		return FAIL;
	}

	now=time(NULL);
	/* Select hosts monitored by IP */
	snprintf(sql,sizeof(sql)-1,"select distinct h.ip from hosts h,items i where i.hostid=h.hostid and (h.status=%d or (h.status=%d and h.available=%d and h.disable_until<=%d)) and (i.key_='%s' or i.key_='%s') and i.type=%d and i.status=%d and h.useip=1", HOST_STATUS_MONITORED, HOST_STATUS_MONITORED, HOST_AVAILABLE_FALSE, now, SERVER_ICMPPING_KEY, SERVER_ICMPPINGSEC_KEY, ITEM_TYPE_SIMPLE, ITEM_STATUS_ACTIVE);
#ifdef ALLVIEW_THREADS
	result = DBselect_thread(database, sql);
#else
	result = DBselect(sql);
#endif
		
	for(i=0;i<DBnum_rows(result);i++)
	{
		host.ip=DBget_field(result,i,0);
/*		host.host=DBget_field(result,i,2);*/

		fprintf(f,"%s\n",host.ip);

		allview_log( LOG_LEVEL_DEBUG, "IP [%s]", host.ip);
	}
	DBfree_result(result);

	/* Select hosts monitored by hostname */
	snprintf(sql,sizeof(sql)-1,"select distinct h.host from hosts h,items i where i.hostid=h.hostid and (h.status=%d or (h.status=%d and h.available=%d and h.disable_until<=%d)) and (i.key_='%s' or i.key_='%s') and i.type=%d and i.status=%d and h.useip=0", HOST_STATUS_MONITORED, HOST_STATUS_MONITORED, HOST_AVAILABLE_FALSE, now, SERVER_ICMPPING_KEY, SERVER_ICMPPINGSEC_KEY, ITEM_TYPE_SIMPLE, ITEM_STATUS_ACTIVE);
#ifdef ALLVIEW_THREADS
	result = DBselect_thread(database, sql);
#else
	result = DBselect(sql);
#endif
		
	for(i=0;i<DBnum_rows(result);i++)
	{
		host.host=DBget_field(result,i,0);

		fprintf(f,"%s\n",host.host);

		allview_log( LOG_LEVEL_DEBUG, "HOSTNAME [%s]", host.host);
	}
	DBfree_result(result);

	fclose(f);

	return SUCCEED;
}


#ifdef ALLVIEW_THREADS
int	do_ping(MYSQL *database)
#else
int	do_ping(void)
#endif
{
	FILE	*f;
	char	ip[MAX_STRING_LEN];
	char	str[MAX_STRING_LEN];
	char	tmp[MAX_STRING_LEN];
	double	mseconds;
	char	*c;
	int	alive;

	allview_log( LOG_LEVEL_DEBUG, "In do_ping()");

	snprintf(str,sizeof(str)-1,"cat /tmp/allview_suckerd.pinger | %s -e 2>/dev/null",CONFIG_FPING_LOCATION);
	
	f=popen(str,"r");
	if(f==0)
	{
		allview_log( LOG_LEVEL_ERR, "Cannot execute [%s] [%s]", CONFIG_FPING_LOCATION, strerror(errno));
		allview_syslog("Cannot execute [%s] [%s]", CONFIG_FPING_LOCATION, strerror(errno));
		return FAIL;
	}

	while(NULL!=fgets(ip,MAX_STRING_LEN,f))
	{
/*		allview_log( LOG_LEVEL_WARNING, "PING: [%s]", ip);*/

		ip[strlen(ip)-1]=0;
		allview_log( LOG_LEVEL_DEBUG, "Update IP [%s]", ip);

		if(strstr(ip,"alive") != NULL)
		{
			alive=1;
			sscanf(ip,"%s is alive (%lf ms)", tmp, &mseconds);
			allview_log( LOG_LEVEL_DEBUG, "Mseconds [%lf]", mseconds);
		}
		else
		{
			alive=0;
		}
		c=strstr(ip," ");
		if(c != NULL)
		{
			*c=0;
			allview_log( LOG_LEVEL_DEBUG, "IP [%s] alive [%d]", ip, alive);
			if(0 == alive)
			{
#ifdef ALLVIEW_THREADS
				process_value(database,SERVER_ICMPPING_KEY,ip,"0");
				process_value(database,SERVER_ICMPPINGSEC_KEY,ip,"0");
#else
				process_value(SERVER_ICMPPING_KEY,ip,"0");
				process_value(SERVER_ICMPPINGSEC_KEY,ip,"0");
#endif
			}
			else
			{
				snprintf(tmp,sizeof(tmp)-1,"%f",mseconds/1000);
#ifdef ALLVIEW_THREADS
				process_value(database,SERVER_ICMPPING_KEY,ip,"1");
				process_value(database,SERVER_ICMPPINGSEC_KEY,ip,tmp);
#else
				process_value(SERVER_ICMPPING_KEY,ip,"1");
				process_value(SERVER_ICMPPINGSEC_KEY,ip,tmp);
#endif
			}
		}
	}

	pclose(f);


	return	SUCCEED;
}

#ifdef ALLVIEW_THREADS
void *main_pinger_loop()
#else
int main_pinger_loop()
#endif
{
	int ret = SUCCEED;

#ifdef ALLVIEW_THREADS
	DB_HANDLE	database;
#endif


	if(1 == CONFIG_DISABLE_PINGER)
	{
		for(;;)
		{
			pause();
		}
	}
	else
	{
		for(;;)
		{
#ifdef HAVE_FUNCTION_SETPROCTITLE
			setproctitle("connecting to the database");
#endif

#ifdef ALLVIEW_THREADS
			DBconnect_thread(&database);
#else
			DBconnect();
#endif
	
/*	allview_set_log_level(LOG_LEVEL_DEBUG);*/

#ifdef ALLVIEW_THREADS
			ret = create_host_file(&database);
#else
			ret = create_host_file();
#endif
	
			if( SUCCEED == ret)
			{
#ifdef HAVE_FUNCTION_SETPROCTITLE
				setproctitle("pinging hosts");
#endif

#ifdef ALLVIEW_THREADS
				ret = do_ping(&database);
#else
				ret = do_ping();
#endif
			}
			unlink("/tmp/allview_suckerd.pinger");
	
/*	allview_set_log_level(LOG_LEVEL_WARNING); */

#ifdef ALLVIEW_THREADS
			DBclose_thread(&database);
#else
			DBclose();
#endif

#ifdef HAVE_FUNCTION_SETPROCTITLE
			setproctitle("pinger [sleeping for %d seconds]", CONFIG_PINGER_FREQUENCY);
#endif
			sleep(CONFIG_PINGER_FREQUENCY);
		}
	}
	
	/* Never reached */
}
