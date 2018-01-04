
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


#include <stdlib.h>
#include <stdio.h>

/* for setproctitle() */
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <strings.h>

#include "db.h"
#include "log.h"
#include "zlog.h"
#include "common.h"

#ifdef	HAVE_MYSQL
	MYSQL	mysql;
#endif

#ifdef	HAVE_PGSQL
	PGconn	*conn;
#endif

#ifdef ALLVIEW_THREADS
void	DBclose_thread(MYSQL *database)
{
#ifdef	HAVE_MYSQL
	mysql_close(database);
#endif
#ifdef	HAVE_PGSQL
	PQfinish(conn);
#endif
}
#endif

void	DBclose(void)
{
#ifdef	HAVE_MYSQL
	mysql_close(&mysql);
#endif
#ifdef	HAVE_PGSQL
	PQfinish(conn);
#endif
}

/*
 * Connect to the database.
 * If fails, program terminates.
 */ 
#ifdef ALLVIEW_THREADS
void    DBconnect_thread(MYSQL *database)
{
	for(;;)
	{
		/*	allview_log(LOG_LEVEL_ERR, "[%s] [%s] [%s]\n",dbname, dbuser, dbpassword ); */
#ifdef	HAVE_MYSQL
	/* For MySQL >3.22.00 */
	/*	if( ! mysql_connect( &mysql, NULL, dbuser, dbpassword ) )*/
		mysql_init(database);
		if( ! mysql_real_connect( database, CONFIG_DBHOST, CONFIG_DBUSER, CONFIG_DBPASSWORD, CONFIG_DBNAME, 3306, CONFIG_DBSOCKET,0 ) )
		{
			fprintf(stderr, "Failed to connect to database: Error: %s\n",mysql_error(database) );
			allview_log(LOG_LEVEL_ERR, "Failed to connect to database: Error: %s",mysql_error(database) );
			if( (ER_SERVER_SHUTDOWN != mysql_errno(database)) && (CR_SERVER_GONE_ERROR != mysql_errno(database)))
			{
				exit(FAIL);
			}
		}
		else
		{
			if( mysql_select_db( database, CONFIG_DBNAME ) != 0 )

			{
				fprintf(stderr, "Failed to select database: Error: %s\n",mysql_error(database) );
				allview_log(LOG_LEVEL_ERR, "Failed to select database: Error: %s",mysql_error(database) );
				exit( FAIL );
			}
			else
			{
				break;
			}
		}
#endif
#ifdef	HAVE_PGSQL
/*	conn = PQsetdb(pghost, pgport, pgoptions, pgtty, dbName); */
/*	conn = PQsetdb(NULL, NULL, NULL, NULL, dbname);*/
		conn = PQsetdbLogin(CONFIG_DBHOST, NULL, NULL, NULL, CONFIG_DBNAME, CONFIG_DBUSER, CONFIG_DBPASSWORD );

/* check to see that the backend connection was successfully made */
		if (PQstatus(conn) != CONNECTION_OK)
		{
			fprintf(stderr, "Connection to database '%s' failed.\n", CONFIG_DBNAME);
			allview_log(LOG_LEVEL_ERR, "Connection to database '%s' failed.\n", CONFIG_DBNAME);
			fprintf(stderr, "%s\n", PQerrorMessage(conn));
			allview_log(LOG_LEVEL_ERR, "%s", PQerrorMessage(conn));
			exit(FAIL);
		}
		else
		{
			break;
		}
#endif
		fprintf(stderr, "Will retry to connect to the database after 30 seconds\n");
		allview_log(LOG_LEVEL_ERR, "Will retry to connect to the database after 30 seconds");
		sleep(30);
	}
}
#endif

/*
 * Connect to the database.
 * If fails, program terminates.
 */ 
void    DBconnect(void)
{
	for(;;)
	{
		/*	allview_log(LOG_LEVEL_ERR, "[%s] [%s] [%s]\n",dbname, dbuser, dbpassword ); */
#ifdef	HAVE_MYSQL
	/* For MySQL >3.22.00 */
	/*	if( ! mysql_connect( &mysql, NULL, dbuser, dbpassword ) )*/
		mysql_init(&mysql);
		if( ! mysql_real_connect( &mysql, CONFIG_DBHOST, CONFIG_DBUSER, CONFIG_DBPASSWORD, CONFIG_DBNAME, 3306, CONFIG_DBSOCKET,0 ) )
		{
			fprintf(stderr, "Failed to connect to database: Error: %s\n",mysql_error(&mysql) );
			allview_log(LOG_LEVEL_ERR, "Failed to connect to database: Error: %s",mysql_error(&mysql) );
			if( (ER_SERVER_SHUTDOWN != mysql_errno(&mysql)) && (CR_SERVER_GONE_ERROR != mysql_errno(&mysql)))
			{
				exit(FAIL);
			}
		}
		else
		{
			if( mysql_select_db( &mysql, CONFIG_DBNAME ) != 0 )

			{
				fprintf(stderr, "Failed to select database: Error: %s\n",mysql_error(&mysql) );
				allview_log(LOG_LEVEL_ERR, "Failed to select database: Error: %s",mysql_error(&mysql) );
				exit( FAIL );
			}
			else
			{
				break;
			}
		}
#endif
#ifdef	HAVE_PGSQL
/*	conn = PQsetdb(pghost, pgport, pgoptions, pgtty, dbName); */
/*	conn = PQsetdb(NULL, NULL, NULL, NULL, dbname);*/
		conn = PQsetdbLogin(CONFIG_DBHOST, NULL, NULL, NULL, CONFIG_DBNAME, CONFIG_DBUSER, CONFIG_DBPASSWORD );

/* check to see that the backend connection was successfully made */
		if (PQstatus(conn) != CONNECTION_OK)
		{
			fprintf(stderr, "Connection to database '%s' failed.\n", CONFIG_DBNAME);
			allview_log(LOG_LEVEL_ERR, "Connection to database '%s' failed.\n", CONFIG_DBNAME);
			fprintf(stderr, "%s\n", PQerrorMessage(conn));
			allview_log(LOG_LEVEL_ERR, "%s", PQerrorMessage(conn));
			exit(FAIL);
		}
		else
		{
			break;
		}
#endif
		fprintf(stderr, "Will retry to connect to the database after 30 seconds\n");
		allview_log(LOG_LEVEL_ERR, "Will retry to connect to the database after 30 seconds");
		sleep(30);
	}
}

#ifdef ALLVIEW_THREADS
/*
 * Execute SQL statement. For non-select statements only.
 * If fails, program terminates.
 */ 
int	DBexecute_thread(MYSQL *database, char *query)
{
/* Do not include any code here. Will break HAVE_PGSQL section */
#ifdef	HAVE_MYSQL
	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	while( mysql_query(database,query) != 0)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s [%d]", mysql_error(database), mysql_errno(database) );
		if( (ER_SERVER_SHUTDOWN != mysql_errno(database)) && (CR_SERVER_GONE_ERROR != mysql_errno(database)))
		{
			return FAIL;
		}
		sleep(30);
	}
#endif
#ifdef	HAVE_PGSQL
	PGresult	*result;

	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	result = PQexec(conn,query);

	if( result==NULL)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", "Result is NULL" );
		PQclear(result);
		return FAIL;
	}
	if( PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", PQresStatus(PQresultStatus(result)) );
		PQclear(result);
		return FAIL;
	}
	PQclear(result);
#endif
/*	allview_set_log_level(LOG_LEVEL_WARNING);*/
	return	SUCCEED;
}
#endif

#ifdef ALLVIEW_THREADS
/*
 * Execute SQL statement. For select statements only.
 * If fails, program terminates.
 */ 
DB_RESULT *DBselect_thread(MYSQL *database, char *query)
{
/* Do not include any code here. Will break HAVE_PGSQL section */
#ifdef	HAVE_MYSQL
	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	while(mysql_query(database,query) != 0)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s [%d]", mysql_error(database), mysql_errno(database) );
		if( (ER_SERVER_SHUTDOWN != mysql_errno(database)) && (CR_SERVER_GONE_ERROR != mysql_errno(database)))
		{
			exit(FAIL);
		}
		sleep(30);
	}
/*	allview_set_log_level(LOG_LEVEL_WARNING);*/
	return	mysql_store_result(database);
#endif
#ifdef	HAVE_PGSQL
	PGresult	*result;

	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	result = PQexec(conn,query);

	if( result==NULL)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", "Result is NULL" );
		exit( FAIL );
	}
	if( PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", PQresStatus(PQresultStatus(result)) );
		exit( FAIL );
	}
#endif
}
#endif

/*
 * Execute SQL statement. For non-select statements only.
 * If fails, program terminates.
 */ 
int	DBexecute(char *query)
{
/* Do not include any code here. Will break HAVE_PGSQL section */
#ifdef	HAVE_MYSQL
	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	while( mysql_query(&mysql,query) != 0)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s [%d]", mysql_error(&mysql), mysql_errno(&mysql) );
		if( (ER_SERVER_SHUTDOWN != mysql_errno(&mysql)) && (CR_SERVER_GONE_ERROR != mysql_errno(&mysql)))
		{
			return FAIL;
		}
		sleep(30);
	}
#endif
#ifdef	HAVE_PGSQL
	PGresult	*result;

	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	result = PQexec(conn,query);

	if( result==NULL)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", "Result is NULL" );
		PQclear(result);
		return FAIL;
	}
	if( PQresultStatus(result) != PGRES_COMMAND_OK)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", PQresStatus(PQresultStatus(result)) );
		PQclear(result);
		return FAIL;
	}
	PQclear(result);
#endif
/*	allview_set_log_level(LOG_LEVEL_WARNING);*/
	return	SUCCEED;
}

/*
 * Execute SQL statement. For select statements only.
 * If fails, program terminates.
 */ 
DB_RESULT *DBselect(char *query)
{
/* Do not include any code here. Will break HAVE_PGSQL section */
#ifdef	HAVE_MYSQL
	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	while(mysql_query(&mysql,query) != 0)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s [%d]", mysql_error(&mysql), mysql_errno(&mysql) );
		if( (ER_SERVER_SHUTDOWN != mysql_errno(&mysql)) && (CR_SERVER_GONE_ERROR != mysql_errno(&mysql)))
		{
			exit(FAIL);
		}
		sleep(30);
	}
/*	allview_set_log_level(LOG_LEVEL_WARNING);*/
	return	mysql_store_result(&mysql);
#endif
#ifdef	HAVE_PGSQL
	PGresult	*result;

	allview_log( LOG_LEVEL_DEBUG, "Executing query:%s",query);
	result = PQexec(conn,query);

	if( result==NULL)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", "Result is NULL" );
		exit( FAIL );
	}
	if( PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		allview_log( LOG_LEVEL_ERR, "Query::%s",query);
		allview_log(LOG_LEVEL_ERR, "Query failed:%s", PQresStatus(PQresultStatus(result)) );
		exit( FAIL );
	}
	return result;
#endif
}

/*
 * Get value for given row and field. Must be called after DBselect.
 */ 
char	*DBget_field(DB_RESULT *result, int rownum, int fieldnum)
{
#ifdef	HAVE_MYSQL
	MYSQL_ROW	row;

	mysql_data_seek(result, rownum);
	row=mysql_fetch_row(result);
	if(row == NULL)
	{
		allview_log(LOG_LEVEL_ERR, "Error while mysql_fetch_row():Error [%s] Rownum [%d] Fieldnum [%d]", mysql_error(&mysql), rownum, fieldnum );
		allview_syslog("MYSQL: Error while mysql_fetch_row():Error [%s] Rownum [%d] Fieldnum [%d]", mysql_error(&mysql), rownum, fieldnum );
		exit(FAIL);
	}
	return row[fieldnum];
#endif
#ifdef	HAVE_PGSQL
	return PQgetvalue(result, rownum, fieldnum);
#endif
}

/*
 * Return SUCCEED if result conains no records
 */ 
/*int	DBis_empty(DB_RESULT *result)
{
	allview_log(LOG_LEVEL_DEBUG, "In DBis_empty");
	if(result == NULL)
	{
		return	SUCCEED;
	}
	if(DBnum_rows(result) == 0)
	{
		return	SUCCEED;
	}
	if(DBget_field(result,0,0) == 0)
	{
		return	SUCCEED;
	}

	return FAIL;
}*/

/*
 * Get number of selected records.
 */ 
int	DBnum_rows(DB_RESULT *result)
{
#ifdef	HAVE_MYSQL
	int rows;

	allview_log(LOG_LEVEL_DEBUG, "In DBnum_rows");
	if(result == NULL)
	{
		return	0;
	}
/* Order is important ! */
	rows = mysql_num_rows(result);
	if(rows == 0)
	{
		return	0;
	}
	
/* This is necessary to exclude situations like
 * atoi(DBget_field(result,0,0). This leads to coredump.
 */
/* This is required for empty results for count(*), etc */
	if(DBget_field(result,0,0) == 0)
	{
		return	0;
	}
	allview_log(LOG_LEVEL_DEBUG, "Result of DBnum_rows [%d]", rows);
	return rows;
#endif
#ifdef	HAVE_PGSQL
	allview_log(LOG_LEVEL_DEBUG, "In DBnum_rows");
	return PQntuples(result);
#endif
}

#ifdef ALLVIEW_THREADS
/*
 * Get function value.
 */ 
int     DBget_function_result_thread(MYSQL *database, double *result,char *functionid)
{
	DB_RESULT *dbresult;
	int		res = SUCCEED;

        char	sql[MAX_STRING_LEN];

/* 0 is added to distinguish between lastvalue==NULL and empty result */
	snprintf( sql, sizeof(sql)-1, "select 0,lastvalue from functions where functionid=%s", functionid );
	dbresult = DBselect_thread(database, sql);

	if(DBnum_rows(dbresult) == 0)
	{
		allview_log(LOG_LEVEL_WARNING, "No function for functionid:[%s]", functionid );
		allview_syslog("No function for functionid:[%s]", functionid );
		res = FAIL;
	}
	else if(DBget_field(dbresult,0,1) == NULL)
	{
		allview_log(LOG_LEVEL_DEBUG, "function.lastvalue==NULL [%s]", functionid );
		res = FAIL;
	}
	else
	{
        	*result=atof(DBget_field(dbresult,0,1));
	}
        DBfree_result(dbresult);

        return res;
}
#endif

/*
 * Get function value.
 */ 
int     DBget_function_result(double *result,char *functionid)
{
	DB_RESULT *dbresult;
	int		res = SUCCEED;

        char	sql[MAX_STRING_LEN];

/* 0 is added to distinguish between lastvalue==NULL and empty result */
	snprintf( sql, sizeof(sql)-1, "select 0,lastvalue from functions where functionid=%s", functionid );
	dbresult = DBselect(sql);

	if(DBnum_rows(dbresult) == 0)
	{
		allview_log(LOG_LEVEL_WARNING, "No function for functionid:[%s]", functionid );
		allview_syslog("No function for functionid:[%s]", functionid );
		res = FAIL;
	}
	else if(DBget_field(dbresult,0,1) == NULL)
	{
		allview_log(LOG_LEVEL_DEBUG, "function.lastvalue==NULL [%s]", functionid );
		res = FAIL;
	}
	else
	{
        	*result=atof(DBget_field(dbresult,0,1));
	}
        DBfree_result(dbresult);

        return res;
}

#ifdef ALLVIEW_THREADS
/* Returns previous trigger value. If not value found, return TRIGGER_VALUE_FALSE */
int	DBget_prev_trigger_value_thread(MYSQL *database, int triggerid)
{
	char	sql[MAX_STRING_LEN];
	int	clock;
	int	value;

	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_prev_trigger_value[%d]", triggerid);

	snprintf(sql,sizeof(sql)-1,"select max(clock) from alarms where triggerid=%d",triggerid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
		DBfree_result(result);
		return TRIGGER_VALUE_UNKNOWN;
	}
	clock=atoi(DBget_field(result,0,0));
	DBfree_result(result);

	snprintf(sql,sizeof(sql),"select max(clock) from alarms where triggerid=%d and clock<%d",triggerid,clock);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database,sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
		DBfree_result(result);
/* Assume that initially Trigger value is False. Otherwise alarms will not be generated when
status changes to TRUE for te first time */
		return TRIGGER_VALUE_FALSE;
/*		return TRIGGER_VALUE_UNKNOWN;*/
	}
	clock=atoi(DBget_field(result,0,0));
	DBfree_result(result);

	snprintf(sql,sizeof(sql)-1,"select value from alarms where triggerid=%d and clock=%d",triggerid,clock);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database,sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "Result of [%s] is empty", sql );
		DBfree_result(result);
		return TRIGGER_VALUE_UNKNOWN;
	}
	value=atoi(DBget_field(result,0,0));
	DBfree_result(result);

	return value;
}
#endif

/* Returns previous trigger value. If not value found, return TRIGGER_VALUE_FALSE */
int	DBget_prev_trigger_value(int triggerid)
{
	char	sql[MAX_STRING_LEN];
	int	clock;
	int	value;

	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_prev_trigger_value[%d]", triggerid);

	snprintf(sql,sizeof(sql)-1,"select max(clock) from alarms where triggerid=%d",triggerid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
		DBfree_result(result);
		return TRIGGER_VALUE_UNKNOWN;
	}
	clock=atoi(DBget_field(result,0,0));
	DBfree_result(result);

	snprintf(sql,sizeof(sql),"select max(clock) from alarms where triggerid=%d and clock<%d",triggerid,clock);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
		DBfree_result(result);
/* Assume that initially Trigger value is False. Otherwise alarms will not be generated when
status changes to TRUE for te first time */
		return TRIGGER_VALUE_FALSE;
/*		return TRIGGER_VALUE_UNKNOWN;*/
	}
	clock=atoi(DBget_field(result,0,0));
	DBfree_result(result);

	snprintf(sql,sizeof(sql)-1,"select value from alarms where triggerid=%d and clock=%d",triggerid,clock);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_DEBUG, "Result of [%s] is empty", sql );
		DBfree_result(result);
		return TRIGGER_VALUE_UNKNOWN;
	}
	value=atoi(DBget_field(result,0,0));
	DBfree_result(result);

	return value;
}

#ifdef ALLVIEW_THREADS
/* SUCCEED if latest alarm with triggerid has this status */
/* Rewrite required to simplify logic ?*/
int	latest_alarm_thread(MYSQL *database,int triggerid, int status)
{
	char	sql[MAX_STRING_LEN];
	int	clock;
	DB_RESULT	*result;
	int ret = FAIL;


	allview_log(LOG_LEVEL_DEBUG,"In latest_alarm()");

	snprintf(sql,sizeof(sql)-1,"select max(clock) from alarms where triggerid=%d",triggerid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
        {
                allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
                ret = FAIL;
        }
	else
	{
		clock=atoi(DBget_field(result,0,0));
		DBfree_result(result);

		snprintf(sql,sizeof(sql)-1,"select value from alarms where triggerid=%d and clock=%d",triggerid,clock);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		result = DBselect_thread(database, sql);
		if(DBnum_rows(result)==1)
		{
			if(atoi(DBget_field(result,0,0)) == status)
			{
				ret = SUCCEED;
			}
		}
	}

	DBfree_result(result);

	return ret;
}
#endif

/* SUCCEED if latest alarm with triggerid has this status */
/* Rewrite required to simplify logic ?*/
int	latest_alarm(int triggerid, int status)
{
	char	sql[MAX_STRING_LEN];
	int	clock;
	DB_RESULT	*result;
	int ret = FAIL;


	allview_log(LOG_LEVEL_DEBUG,"In latest_alarm()");

	snprintf(sql,sizeof(sql)-1,"select max(clock) from alarms where triggerid=%d",triggerid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 0)
        {
                allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
                ret = FAIL;
        }
	else
	{
		clock=atoi(DBget_field(result,0,0));
		DBfree_result(result);

		snprintf(sql,sizeof(sql)-1,"select value from alarms where triggerid=%d and clock=%d",triggerid,clock);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		result = DBselect(sql);
		if(DBnum_rows(result)==1)
		{
			if(atoi(DBget_field(result,0,0)) == status)
			{
				ret = SUCCEED;
			}
		}
	}

	DBfree_result(result);

	return ret;
}

#ifdef ALLVIEW_THREADS
/* SUCCEED if latest service alarm has this status */
/* Rewrite required to simplify logic ?*/
int	latest_service_alarm_thread(MYSQL *database, int serviceid, int status)
{
	char	sql[MAX_STRING_LEN];
	int	clock;
	DB_RESULT	*result;
	int ret = FAIL;


	allview_log(LOG_LEVEL_DEBUG,"In latest_service_alarm()");

	snprintf(sql,sizeof(sql)-1,"select max(clock) from service_alarms where serviceid=%d",serviceid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
        {
                allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
                ret = FAIL;
        }
	else
	{
		clock=atoi(DBget_field(result,0,0));
		DBfree_result(result);

		snprintf(sql,sizeof(sql)-1,"select value from service_alarms where serviceid=%d and clock=%d",serviceid,clock);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		result = DBselect_thread(database, sql);
		if(DBnum_rows(result)==1)
		{
			if(atoi(DBget_field(result,0,0)) == status)
			{
				ret = SUCCEED;
			}
		}
	}

	DBfree_result(result);

	return ret;
}
#endif

/* SUCCEED if latest service alarm has this status */
/* Rewrite required to simplify logic ?*/
int	latest_service_alarm(int serviceid, int status)
{
	char	sql[MAX_STRING_LEN];
	int	clock;
	DB_RESULT	*result;
	int ret = FAIL;


	allview_log(LOG_LEVEL_DEBUG,"In latest_service_alarm()");

	snprintf(sql,sizeof(sql)-1,"select max(clock) from service_alarms where serviceid=%d",serviceid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 0)
        {
                allview_log(LOG_LEVEL_DEBUG, "Result for MAX is empty" );
                ret = FAIL;
        }
	else
	{
		clock=atoi(DBget_field(result,0,0));
		DBfree_result(result);

		snprintf(sql,sizeof(sql)-1,"select value from service_alarms where serviceid=%d and clock=%d",serviceid,clock);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		result = DBselect(sql);
		if(DBnum_rows(result)==1)
		{
			if(atoi(DBget_field(result,0,0)) == status)
			{
				ret = SUCCEED;
			}
		}
	}

	DBfree_result(result);

	return ret;
}

#ifdef ALLVIEW_THREADS
int	add_alarm_thread(MYSQL *database, int triggerid,int status,int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_alarm()");

	if(latest_alarm_thread(database, triggerid,status) == SUCCEED)
	{
		return SUCCEED;
	}

	snprintf(sql,sizeof(sql)-1,"insert into alarms(triggerid,clock,value) values(%d,%d,%d)", triggerid, clock, status);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	DBexecute_thread(database, sql);

	allview_log(LOG_LEVEL_DEBUG,"End of add_alarm()");
	
	return SUCCEED;
}
#endif

int	add_alarm(int triggerid,int status,int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_alarm()");

	if(latest_alarm(triggerid,status) == SUCCEED)
	{
		return SUCCEED;
	}

	snprintf(sql,sizeof(sql)-1,"insert into alarms(triggerid,clock,value) values(%d,%d,%d)", triggerid, clock, status);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	DBexecute(sql);

	allview_log(LOG_LEVEL_DEBUG,"End of add_alarm()");
	
	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
int	DBadd_service_alarm_thread(MYSQL *database, int serviceid,int status,int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_service_alarm()");

	if(latest_service_alarm_thread(database, serviceid,status) == SUCCEED)
	{
		return SUCCEED;
	}

	snprintf(sql,sizeof(sql)-1,"insert into service_alarms(serviceid,clock,value) values(%d,%d,%d)", serviceid, clock, status);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	DBexecute_thread(database, sql);

	allview_log(LOG_LEVEL_DEBUG,"End of add_service_alarm()");
	
	return SUCCEED;
}
#endif

int	DBadd_service_alarm(int serviceid,int status,int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_service_alarm()");

	if(latest_service_alarm(serviceid,status) == SUCCEED)
	{
		return SUCCEED;
	}

	snprintf(sql,sizeof(sql)-1,"insert into service_alarms(serviceid,clock,value) values(%d,%d,%d)", serviceid, clock, status);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	DBexecute(sql);

	allview_log(LOG_LEVEL_DEBUG,"End of add_service_alarm()");
	
	return SUCCEED;
}

#ifdef	IT_HELPDESK
void	update_problems(int triggerid, int value, int clock)
{
}
#endif

#ifdef ALLVIEW_THREADS
int	DBupdate_trigger_value_thread(MYSQL *database,int triggerid,int value,int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In update_trigger_value[%d,%d,%d]", triggerid, value, clock);
	add_alarm_thread(database, triggerid,value,clock);

	snprintf(sql,sizeof(sql)-1,"update triggers set value=%d,lastchange=%d where triggerid=%d",value,clock,triggerid);
	DBexecute_thread(database,sql);

	if(TRIGGER_VALUE_UNKNOWN == value)
	{
		snprintf(sql,sizeof(sql)-1,"update functions set lastvalue=NULL where triggerid=%d",triggerid);
		DBexecute_thread(database,sql);
	}

	allview_log(LOG_LEVEL_DEBUG,"End of update_trigger_value()");
	return SUCCEED;
}
#endif

int	DBupdate_trigger_value(int triggerid,int value,int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In update_trigger_value[%d,%d,%d]", triggerid, value, clock);
	add_alarm(triggerid,value,clock);

	snprintf(sql,sizeof(sql)-1,"update triggers set value=%d,lastchange=%d where triggerid=%d",value,clock,triggerid);
	DBexecute(sql);

	if(TRIGGER_VALUE_UNKNOWN == value)
	{
		snprintf(sql,sizeof(sql)-1,"update functions set lastvalue=NULL where triggerid=%d",triggerid);
		DBexecute(sql);
	}

#ifdef	IT_HELPDESK
	update_problems(triggerid,value,clock);
#endif

	allview_log(LOG_LEVEL_DEBUG,"End of update_trigger_value()");
	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
void update_triggers_status_to_unknown_thread(MYSQL *database, int hostid,int clock)
{
	int	i;
	char	sql[MAX_STRING_LEN];
	int	triggerid;

	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In update_triggers_status_to_unknown()");

	snprintf(sql,sizeof(sql)-1,"select distinct t.triggerid from hosts h,items i,triggers t,functions f where f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and h.hostid=%d and i.key_ not in ('%s','%s','%s')",hostid,SERVER_STATUS_KEY, SERVER_ICMPPING_KEY, SERVER_ICMPPINGSEC_KEY);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database, sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		triggerid=atoi(DBget_field(result,i,0));
		DBupdate_trigger_value_thread(database, triggerid,TRIGGER_VALUE_UNKNOWN,clock);
	}

	DBfree_result(result);
	allview_log(LOG_LEVEL_DEBUG,"End of update_triggers_status_to_unknown()");

	return; 
}
#endif

void update_triggers_status_to_unknown(int hostid,int clock)
{
	int	i;
	char	sql[MAX_STRING_LEN];
	int	triggerid;

	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In update_triggers_status_to_unknown()");

/*	snprintf(sql,sizeof(sql)-1,"select distinct t.triggerid from hosts h,items i,triggers t,functions f where f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and h.hostid=%d and i.key_<>'%s'",hostid,SERVER_STATUS_KEY);*/
	snprintf(sql,sizeof(sql)-1,"select distinct t.triggerid from hosts h,items i,triggers t,functions f where f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and h.hostid=%d and i.key_ not in ('%s','%s','%s')",hostid,SERVER_STATUS_KEY, SERVER_ICMPPING_KEY, SERVER_ICMPPINGSEC_KEY);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		triggerid=atoi(DBget_field(result,i,0));
		DBupdate_trigger_value(triggerid,TRIGGER_VALUE_UNKNOWN,clock);
	}

	DBfree_result(result);
	allview_log(LOG_LEVEL_DEBUG,"End of update_triggers_status_to_unknown()");

	return; 
}

#ifdef ALLVIEW_THREADS
void  DBdelete_service_thread(MYSQL *database, int serviceid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from services_links where servicedownid=%d or serviceupid=%d", serviceid, serviceid);
	DBexecute_thread(database, sql);
	snprintf(sql,sizeof(sql)-1,"delete from services where serviceid=%d", serviceid);
	DBexecute_thread(database, sql);
}
#endif

void  DBdelete_service(int serviceid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from services_links where servicedownid=%d or serviceupid=%d", serviceid, serviceid);
	DBexecute(sql);
	snprintf(sql,sizeof(sql)-1,"delete from services where serviceid=%d", serviceid);
	DBexecute(sql);
}

#ifdef ALLVIEW_THREADS
void  DBdelete_services_by_triggerid_thread(MYSQL *database, int triggerid)
{
	int	i, serviceid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_services_by_triggerid(%d)", triggerid);
	snprintf(sql,sizeof(sql)-1,"select serviceid from services where triggerid=%d", triggerid);
	result = DBselect_thread(database,sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		serviceid=atoi(DBget_field(result,i,0));
		DBdelete_service_thread(database, serviceid);
	}
	DBfree_result(result);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_services_by_triggerid(%d)", triggerid);
}
#endif

void  DBdelete_services_by_triggerid(int triggerid)
{
	int	i, serviceid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_services_by_triggerid(%d)", triggerid);
	snprintf(sql,sizeof(sql)-1,"select serviceid from services where triggerid=%d", triggerid);
	result = DBselect(sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		serviceid=atoi(DBget_field(result,i,0));
		DBdelete_service(serviceid);
	}
	DBfree_result(result);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_services_by_triggerid(%d)", triggerid);
}

#ifdef ALLVIEW_THREADS
void  DBdelete_trigger_thread(MYSQL *database, int triggerid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from trigger_depends where triggerid_down=%d or triggerid_up=%d", triggerid, triggerid);
	DBexecute_thread(database, sql);
	snprintf(sql,sizeof(sql)-1,"delete from functions where triggerid=%d", triggerid);
	DBexecute_thread(database, sql);
	snprintf(sql,sizeof(sql)-1,"delete from alarms where triggerid=%d", triggerid);
	DBexecute_thread(database, sql);
	snprintf(sql,sizeof(sql)-1,"delete from actions where triggerid=%d and scope=%d", triggerid, ACTION_SCOPE_TRIGGER);
	DBexecute_thread(database, sql);

	DBdelete_services_by_triggerid_thread(database, triggerid);

	snprintf(sql,sizeof(sql)-1,"update sysmaps_links set triggerid=NULL where triggerid=%d", triggerid);
	DBexecute_thread(database, sql);
	snprintf(sql,sizeof(sql)-1,"delete from triggers where triggerid=%d", triggerid);
	DBexecute_thread(database, sql);
}
#endif

void  DBdelete_trigger(int triggerid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from trigger_depends where triggerid_down=%d or triggerid_up=%d", triggerid, triggerid);
	DBexecute(sql);
	snprintf(sql,sizeof(sql)-1,"delete from functions where triggerid=%d", triggerid);
	DBexecute(sql);
	snprintf(sql,sizeof(sql)-1,"delete from alarms where triggerid=%d", triggerid);
	DBexecute(sql);
	snprintf(sql,sizeof(sql)-1,"delete from actions where triggerid=%d and scope=%d", triggerid, ACTION_SCOPE_TRIGGER);
	DBexecute(sql);

	DBdelete_services_by_triggerid(triggerid);

	snprintf(sql,sizeof(sql)-1,"update sysmaps_links set triggerid=NULL where triggerid=%d", triggerid);
	DBexecute(sql);
	snprintf(sql,sizeof(sql)-1,"delete from triggers where triggerid=%d", triggerid);
	DBexecute(sql);
}

#ifdef ALLVIEW_THREADS
void  DBdelete_triggers_by_itemid_thread(MYSQL *database, int itemid)
{
	int	i, triggerid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_triggers_by_itemid(%d)", itemid);
	snprintf(sql,sizeof(sql)-1,"select triggerid from functions where itemid=%d", itemid);
	result = DBselect_thread(database, sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		triggerid=atoi(DBget_field(result,i,0));
		DBdelete_trigger_thread(database, triggerid);
	}
	DBfree_result(result);

	snprintf(sql,sizeof(sql)-1,"delete from functions where itemid=%d", itemid);
	DBexecute_thread(database, sql);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_triggers_by_itemid(%d)", itemid);
}
#endif

void  DBdelete_triggers_by_itemid(int itemid)
{
	int	i, triggerid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_triggers_by_itemid(%d)", itemid);
	snprintf(sql,sizeof(sql)-1,"select triggerid from functions where itemid=%d", itemid);
	result = DBselect(sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		triggerid=atoi(DBget_field(result,i,0));
		DBdelete_trigger(triggerid);
	}
	DBfree_result(result);

	snprintf(sql,sizeof(sql)-1,"delete from functions where itemid=%d", itemid);
	DBexecute(sql);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_triggers_by_itemid(%d)", itemid);
}

#ifdef ALLVIEW_THREADS
void DBdelete_trends_by_itemid_thread(MYSQL *database, int itemid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from trends where itemid=%d", itemid);
	DBexecute_thread(database, sql);
}
#endif

void DBdelete_trends_by_itemid(int itemid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from trends where itemid=%d", itemid);
	DBexecute(sql);
}

#ifdef ALLVIEW_THREADS
void DBdelete_history_by_itemid_thread(MYSQL *database, int itemid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from history where itemid=%d", itemid);
	DBexecute_thread(database, sql);
	snprintf(sql,sizeof(sql)-1,"delete from history_str where itemid=%d", itemid);
	DBexecute_thread(database, sql);
}
#endif

void DBdelete_history_by_itemid(int itemid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from history where itemid=%d", itemid);
	DBexecute(sql);
	snprintf(sql,sizeof(sql)-1,"delete from history_str where itemid=%d", itemid);
	DBexecute(sql);
}

#ifdef ALLVIEW_THREADS
void DBdelete_item_thread(MYSQL *database, int itemid)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_item(%d)", itemid);

	DBdelete_triggers_by_itemid_thread(database, itemid);
	DBdelete_history_by_itemid_thread(database, itemid);
	DBdelete_trends_by_itemid_thread(database, itemid);

	snprintf(sql,sizeof(sql)-1,"delete from items where itemid=%d", itemid);
	DBexecute_thread(database, sql);
	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_item(%d)", itemid);
}
#endif

void DBdelete_item(int itemid)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_item(%d)", itemid);

	DBdelete_triggers_by_itemid(itemid);
	DBdelete_history_by_itemid(itemid);
	DBdelete_trends_by_itemid(itemid);

	snprintf(sql,sizeof(sql)-1,"delete from items where itemid=%d", itemid);
	DBexecute(sql);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_item(%d)", itemid);
}

#ifdef ALLVIEW_THREADS
void DBdelete_sysmaps_links_by_shostid_thread(MYSQL *database, int shostid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from sysmaps_links where shostid1=%d or shostid2=%d", shostid, shostid);
	DBexecute_thread(database, sql);
}
#endif

void DBdelete_sysmaps_links_by_shostid(int shostid)
{
	char	sql[MAX_STRING_LEN];

	snprintf(sql,sizeof(sql)-1,"delete from sysmaps_links where shostid1=%d or shostid2=%d", shostid, shostid);
	DBexecute(sql);
}

#ifdef ALLVIEW_THREADS
void DBdelete_sysmaps_hosts_by_hostid_thread(MYSQL *database, int hostid)
{
	int	i, shostid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_sysmaps_hosts(%d)", hostid);
	snprintf(sql,sizeof(sql)-1,"select shostid from sysmaps_hosts where hostid=%d", hostid);
	result = DBselect_thread(database, sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		shostid=atoi(DBget_field(result,i,0));
		DBdelete_sysmaps_links_by_shostid_thread(database, shostid);
	}
	DBfree_result(result);

	snprintf(sql,sizeof(sql)-1,"delete from sysmaps_hosts where hostid=%d", hostid);
	DBexecute_thread(database, sql);
}
#endif

void DBdelete_sysmaps_hosts_by_hostid(int hostid)
{
	int	i, shostid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_sysmaps_hosts(%d)", hostid);
	snprintf(sql,sizeof(sql)-1,"select shostid from sysmaps_hosts where hostid=%d", hostid);
	result = DBselect(sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		shostid=atoi(DBget_field(result,i,0));
		DBdelete_sysmaps_links_by_shostid(shostid);
	}
	DBfree_result(result);

	snprintf(sql,sizeof(sql)-1,"delete from sysmaps_hosts where hostid=%d", hostid);
	DBexecute(sql);
}

#ifdef ALLVIEW_THREADS
void DBdelete_host_thread(MYSQL *database, int hostid)
{
	int	i, itemid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_host(%d)", hostid);
	snprintf(sql,sizeof(sql)-1,"select itemid from items where hostid=%d", hostid);
	result = DBselect_thread(database, sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		itemid=atoi(DBget_field(result,i,0));
		DBdelete_item_thread(database, itemid);
	}
	DBfree_result(result);

	DBdelete_sysmaps_hosts_by_hostid_thread(database, hostid);

	snprintf(sql,sizeof(sql)-1,"delete from actions where triggerid=%d and scope=%d", hostid, ACTION_SCOPE_HOST);
	DBexecute_thread(database,sql);

	snprintf(sql,sizeof(sql)-1,"delete from hosts_groups where hostid=%d", hostid);
	DBexecute_thread(database,sql);

	snprintf(sql,sizeof(sql)-1,"delete from hosts where hostid=%d", hostid);
	DBexecute_thread(database,sql);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_host(%d)", hostid);
}
#endif

void DBdelete_host(int hostid)
{
	int	i, itemid;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBdelete_host(%d)", hostid);
	snprintf(sql,sizeof(sql)-1,"select itemid from items where hostid=%d", hostid);
	result = DBselect(sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		itemid=atoi(DBget_field(result,i,0));
		DBdelete_item(itemid);
	}
	DBfree_result(result);

	DBdelete_sysmaps_hosts_by_hostid(hostid);

	snprintf(sql,sizeof(sql)-1,"delete from actions where triggerid=%d and scope=%d", hostid, ACTION_SCOPE_HOST);
	DBexecute(sql);

	snprintf(sql,sizeof(sql)-1,"delete from hosts_groups where hostid=%d", hostid);
	DBexecute(sql);

	snprintf(sql,sizeof(sql)-1,"delete from hosts where hostid=%d", hostid);
	DBexecute(sql);

	allview_log(LOG_LEVEL_DEBUG,"End of DBdelete_host(%d)", hostid);
}

void DBupdate_triggers_status_after_restart(void)
{
	int	i;
	char	sql[MAX_STRING_LEN];
	int	triggerid, lastchange;
	int	now;

	DB_RESULT	*result;
	DB_RESULT	*result2;

	allview_log(LOG_LEVEL_DEBUG,"In DBupdate_triggers_after_restart()");

	now=time(NULL);

	snprintf(sql,sizeof(sql)-1,"select distinct t.triggerid from hosts h,items i,triggers t,functions f where f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and i.nextcheck+i.delay<%d and i.key_<>'%s' and h.status not in (%d,%d)",now,SERVER_STATUS_KEY, HOST_STATUS_DELETED, HOST_STATUS_TEMPLATE);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	for(i=0;i<DBnum_rows(result);i++)
	{
		triggerid=atoi(DBget_field(result,i,0));

		snprintf(sql,sizeof(sql)-1,"select min(i.nextcheck+i.delay) from hosts h,items i,triggers t,functions f where f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and i.nextcheck<>0 and t.triggerid=%d and i.type<>%d",triggerid,ITEM_TYPE_TRAPPER);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		result2 = DBselect(sql);
		if( DBnum_rows(result2) == 0 )
		{
			allview_log(LOG_LEVEL_DEBUG, "No triggers to update (2)");
			DBfree_result(result2);
			continue;
		}

		lastchange=atoi(DBget_field(result2,0,0));
		DBfree_result(result2);

		DBupdate_trigger_value(triggerid,TRIGGER_VALUE_UNKNOWN,lastchange);
	}

	DBfree_result(result);
	allview_log(LOG_LEVEL_DEBUG,"End of DBupdate_triggers_after_restart()");

	return; 
}

#ifdef ALLVIEW_THREADS
void DBupdate_host_availability_thread(MYSQL *database, int hostid,int available,int clock,char *error)
{
	DB_RESULT	*result;
	char	sql[MAX_STRING_LEN];
	char	error_esc[MAX_STRING_LEN];
	int	disable_until;

	allview_log(LOG_LEVEL_DEBUG,"In update_host_availability()");

	DBescape_string(error,error_esc,MAX_STRING_LEN);

	snprintf(sql,sizeof(sql)-1,"select available,disable_until from hosts where hostid=%d",hostid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot select host with hostid [%d]",hostid);
		allview_syslog("Cannot select host with hostid [%d]",hostid);
		DBfree_result(result);
		return;
	}

	disable_until = atoi(DBget_field(result,0,1));

	if(available == atoi(DBget_field(result,0,0)))
	{
		if((availability==HOST_AVAILABLE_FALSE) 
		&&(clock+DELAY_ON_NETWORK_FAILURE>disable_until) )
		{
		}
		else
		{
			allview_log(LOG_LEVEL_DEBUG, "Host already has availability [%d]", available);
			DBfree_result(result);
			return;
		}
	}

	DBfree_result(result);

	if(available==HOST_AVAILABLE_TRUE)
	{
		snprintf(sql,sizeof(sql)-1,"update hosts set available=%d,error='' where hostid=%d",HOST_AVAILABLE_TRUE,hostid);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		DBexecute_thread(database,sql);
	}
	else if(available==HOST_AVAILABLE_FALSE)
	{
		if(disable_until+DELAY_ON_NETWORK_FAILURE>clock)
		{
			snprintf(sql,sizeof(sql)-1,"update hosts set available=%d,disable_until=disable_until+%d,error='%s' where hostid=%d",HOST_AVAILABLE_FALSE,DELAY_ON_NETWORK_FAILURE,error_esc,hostid);
		}
		else
		{
			snprintf(sql,sizeof(sql)-1,"update hosts set available=%d,disable_until=%d,error='%s' where hostid=%d",HOST_AVAILABLE_FALSE,clock+DELAY_ON_NETWORK_FAILURE,error_esc,hostid);
		}
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		DBexecute_thread(database,sql);
	}
	else
	{
		allview_log( LOG_LEVEL_ERR, "Unknown host availability [%d] for hostid [%d]", availability, hostid);
		allview_syslog("Unknown host availability [%d] for hostid [%d]", availability, hostid);
		return;
	}

	update_triggers_status_to_unknown_thread(database,hostid,clock);
	allview_log(LOG_LEVEL_DEBUG,"End of update_host_availability()");

	return;
}
#endif

void DBupdate_host_availability(int hostid,int available,int clock, char *error)
{
	DB_RESULT	*result;
	char	sql[MAX_STRING_LEN];
	char	error_esc[MAX_STRING_LEN];
	int	disable_until;

	allview_log(LOG_LEVEL_DEBUG,"In update_host_availability()");

	DBescape_string(error,error_esc,MAX_STRING_LEN);

	snprintf(sql,sizeof(sql)-1,"select available,disable_until from hosts where hostid=%d",hostid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot select host with hostid [%d]",hostid);
		allview_syslog("Cannot select host with hostid [%d]",hostid);
		DBfree_result(result);
		return;
	}

	disable_until = atoi(DBget_field(result,0,1));

	if(available == atoi(DBget_field(result,0,0)))
	{
		if((available==HOST_AVAILABLE_FALSE) 
		&&(clock+DELAY_ON_NETWORK_FAILURE>disable_until) )
		{
		}
		else
		{
			allview_log(LOG_LEVEL_DEBUG, "Host already has availability [%d]",available);
			DBfree_result(result);
			return;
		}
	}

	DBfree_result(result);

	if(available==HOST_AVAILABLE_TRUE)
	{
		snprintf(sql,sizeof(sql)-1,"update hosts set available=%d where hostid=%d",HOST_AVAILABLE_TRUE,hostid);
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		DBexecute(sql);
	}
	else if(available==HOST_AVAILABLE_FALSE)
	{
		if(disable_until+DELAY_ON_NETWORK_FAILURE>clock)
		{
			snprintf(sql,sizeof(sql)-1,"update hosts set available=%d,disable_until=disable_until+%d,error='%s' where hostid=%d",HOST_AVAILABLE_FALSE,DELAY_ON_NETWORK_FAILURE,error_esc,hostid);
		}
		else
		{
			snprintf(sql,sizeof(sql)-1,"update hosts set available=%d,disable_until=%d,error='%s' where hostid=%d",HOST_AVAILABLE_FALSE,clock+DELAY_ON_NETWORK_FAILURE,error_esc,hostid);
		}
		allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
		DBexecute(sql);
	}
	else
	{
		allview_log( LOG_LEVEL_ERR, "Unknown host availability [%d] for hostid [%d]", available, hostid);
		allview_syslog("Unknown host availability [%d] for hostid [%d]", available, hostid);
		return;
	}

	update_triggers_status_to_unknown(hostid,clock);
	allview_log(LOG_LEVEL_DEBUG,"End of update_host_availability()");

	return;
}

#ifdef ALLVIEW_THREADS
int	DBupdate_item_status_to_notsupported_thread(MYSQL *database, int itemid, char *error)
{
	char	sql[MAX_STRING_LEN];
	char	error_esc[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In DBupdate_item_status_to_notsupported()");

	DBescape_string(error,error_esc,MAX_STRING_LEN);

	snprintf(sql,sizeof(sql)-1,"update items set status=%d,error='%s' where itemid=%d",ITEM_STATUS_NOTSUPPORTED,error_esc,itemid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	DBexecute_thread(database, sql);

	return SUCCEED;
}
#endif

int	DBupdate_item_status_to_notsupported(int itemid, char *error)
{
	char	sql[MAX_STRING_LEN];
	char	error_esc[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In DBupdate_item_status_to_notsupported()");

	DBescape_string(error,error_esc,MAX_STRING_LEN);

	snprintf(sql,sizeof(sql)-1,"update items set status=%d,error='%s' where itemid=%d",ITEM_STATUS_NOTSUPPORTED,error_esc,itemid);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	DBexecute(sql);

	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
int	DBadd_trend_thread(MYSQL *database, int itemid, double value, int clock)
{
	DB_RESULT	*result;
	char	sql[MAX_STRING_LEN];
	int	hour;
	int	num;
	double	value_min, value_avg, value_max;	

	allview_log(LOG_LEVEL_DEBUG,"In add_trend()");

	hour=clock-clock%3600;

	snprintf(sql,sizeof(sql)-1,"select num,value_min,value_avg,value_max from trends where itemid=%d and clock=%d", itemid, hour);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect_thread(database, sql);

	if(DBnum_rows(result) == 1)
	{
		num=atoi(DBget_field(result,0,0));
		value_min=atof(DBget_field(result,0,1));
		value_avg=atof(DBget_field(result,0,2));
		value_max=atof(DBget_field(result,0,3));
		if(value<value_min)	value_min=value;
/* Unfortunate mistake... */
/*		if(value>value_avg)	value_max=value;*/
		if(value>value_max)	value_max=value;
		value_avg=(num*value_avg+value)/(num+1);
		num++;
		snprintf(sql,sizeof(sql)-1,"update trends set num=%d, value_min=%f, value_avg=%f, value_max=%f where itemid=%d and clock=%d", num, value_min, value_avg, value_max, itemid, hour);
	}
	else
	{
		snprintf(sql,sizeof(sql)-1,"insert into trends (clock,itemid,num,value_min,value_avg,value_max) values (%d,%d,%d,%f,%f,%f)", hour, itemid, 1, value, value, value);
	}
	DBexecute_thread(database, sql);

	DBfree_result(result);

	return SUCCEED;
}
#endif

int	DBadd_trend(int itemid, double value, int clock)
{
	DB_RESULT	*result;
	char	sql[MAX_STRING_LEN];
	int	hour;
	int	num;
	double	value_min, value_avg, value_max;	

	allview_log(LOG_LEVEL_DEBUG,"In add_trend()");

	hour=clock-clock%3600;

	snprintf(sql,sizeof(sql)-1,"select num,value_min,value_avg,value_max from trends where itemid=%d and clock=%d", itemid, hour);
	allview_log(LOG_LEVEL_DEBUG,"SQL [%s]",sql);
	result = DBselect(sql);

	if(DBnum_rows(result) == 1)
	{
		num=atoi(DBget_field(result,0,0));
		value_min=atof(DBget_field(result,0,1));
		value_avg=atof(DBget_field(result,0,2));
		value_max=atof(DBget_field(result,0,3));
		if(value<value_min)	value_min=value;
/* Unfortunate mistake... */
/*		if(value>value_avg)	value_max=value;*/
		if(value>value_max)	value_max=value;
		value_avg=(num*value_avg+value)/(num+1);
		num++;
		snprintf(sql,sizeof(sql)-1,"update trends set num=%d, value_min=%f, value_avg=%f, value_max=%f where itemid=%d and clock=%d", num, value_min, value_avg, value_max, itemid, hour);
	}
	else
	{
		snprintf(sql,sizeof(sql)-1,"insert into trends (clock,itemid,num,value_min,value_avg,value_max) values (%d,%d,%d,%f,%f,%f)", hour, itemid, 1, value, value, value);
	}
	DBexecute(sql);

	DBfree_result(result);

	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
int	DBadd_history_thread(MYSQL *database, int itemid, double value, int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_history()");

	snprintf(sql,sizeof(sql)-1,"insert into history (clock,itemid,value) values (%d,%d,%f)",clock,itemid,value);
	DBexecute_thread(database, sql);

	DBadd_trend_thread(database, itemid, value, clock);

	return SUCCEED;
}
#endif

int	DBadd_history(int itemid, double value, int clock)
{
	char	sql[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_history()");

	snprintf(sql,sizeof(sql)-1,"insert into history (clock,itemid,value) values (%d,%d,%f)",clock,itemid,value);
	DBexecute(sql);

	DBadd_trend(itemid, value, clock);

	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
int	DBadd_history_str_thread(MYSQL *database, int itemid, char *value, int clock)
{
	char	sql[MAX_STRING_LEN];
	char	value_esc[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_history_str()");

	DBescape_string(value,value_esc,MAX_STRING_LEN);
	snprintf(sql,sizeof(sql)-1,"insert into history_str (clock,itemid,value) values (%d,%d,'%s')",clock,itemid,value_esc);
	DBexecute_thread(database, sql);

	return SUCCEED;
}
#endif

int	DBadd_history_str(int itemid, char *value, int clock)
{
	char	sql[MAX_STRING_LEN];
	char	value_esc[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_history_str()");

	DBescape_string(value,value_esc,MAX_STRING_LEN);
	snprintf(sql,sizeof(sql)-1,"insert into history_str (clock,itemid,value) values (%d,%d,'%s')",clock,itemid,value_esc);
	DBexecute(sql);

	return SUCCEED;
}

int	DBget_items_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_items_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from items");

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBget_items_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_items_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from items");

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

#ifdef ALLVIEW_THREADS
int	DBget_triggers_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_triggers_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from triggers");

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

int	DBget_triggers_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_triggers_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from triggers");

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBget_items_unsupported_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_items_unsupported_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from items where status=%d", ITEM_STATUS_NOTSUPPORTED);

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

int	DBget_items_unsupported_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_items_unsupported_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from items where status=%d", ITEM_STATUS_NOTSUPPORTED);

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBget_history_str_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_history_str_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from history_str");

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

int	DBget_history_str_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_history_str_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from history_str");

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBget_history_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_history_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from history");

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

int	DBget_history_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_history_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from history");

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBget_trends_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_trends_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from trends");

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

int	DBget_trends_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_trends_count()");

	snprintf(sql,sizeof(sql)-1,"select count(*) from trends");

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBget_queue_count_thread(MYSQL *database)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;
	int	now;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_queue_count()");

	now=time(NULL);
/*	snprintf(sql,sizeof(sql)-1,"select count(*) from items i,hosts h where i.status=%d and i.type not in (%d) and h.status=%d and i.hostid=h.hostid and i.nextcheck<%d and i.key_<>'status'", ITEM_STATUS_ACTIVE, ITEM_TYPE_TRAPPER, HOST_STATUS_MONITORED, now);*/
	snprintf(sql,sizeof(sql)-1,"select count(*) from items i,hosts h where i.status=%d and i.type not in (%d) and ((h.status=%d and h.available!=%d) or (h.status=%d and h.available=%d and h.disable_until<=%d)) and i.hostid=h.hostid and i.nextcheck<%d and i.key_ not in ('%s','%s','%s','%s')", ITEM_STATUS_ACTIVE, ITEM_TYPE_TRAPPER, HOST_STATUS_MONITORED, HOST_AVAILABLE_FALSE, HOST_STATUS_MONITORED, HOST_AVAILABLE_FALSE, now, now, SERVER_STATUS_KEY, SERVER_ICMPPING_KEY, SERVER_ICMPPINGSEC_KEY, SERVER_ALLVIEWLOG_KEY);

	result=DBselect_thread(database, sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}
#endif

int	DBget_queue_count(void)
{
	int	res;
	char	sql[MAX_STRING_LEN];
	DB_RESULT	*result;
	int	now;

	allview_log(LOG_LEVEL_DEBUG,"In DBget_queue_count()");

	now=time(NULL);
/*	snprintf(sql,sizeof(sql)-1,"select count(*) from items i,hosts h where i.status=%d and i.type not in (%d) and h.status=%d and i.hostid=h.hostid and i.nextcheck<%d and i.key_<>'status'", ITEM_STATUS_ACTIVE, ITEM_TYPE_TRAPPER, HOST_STATUS_MONITORED, now);*/
	snprintf(sql,sizeof(sql)-1,"select count(*) from items i,hosts h where i.status=%d and i.type not in (%d) and ((h.status=%d and h.available!=%d) or (h.status=%d and h.available=%d and h.disable_until<=%d)) and i.hostid=h.hostid and i.nextcheck<%d and i.key_ not in ('%s','%s','%s','%s')", ITEM_STATUS_ACTIVE, ITEM_TYPE_TRAPPER, HOST_STATUS_MONITORED, HOST_AVAILABLE_FALSE, HOST_STATUS_MONITORED, HOST_AVAILABLE_FALSE, now, now, SERVER_STATUS_KEY, SERVER_ICMPPING_KEY, SERVER_ICMPPINGSEC_KEY, SERVER_ALLVIEWLOG_KEY);

	result=DBselect(sql);

	if(DBnum_rows(result) == 0)
	{
		allview_log(LOG_LEVEL_ERR, "Cannot execute query [%s]", sql);
		allview_syslog("Cannot execute query [%s]", sql);
		DBfree_result(result);
		return 0;
	}

	res  = atoi(DBget_field(result,0,0));

	DBfree_result(result);

	return res;
}

#ifdef ALLVIEW_THREADS
int	DBadd_alert_thread(MYSQL *database, int actionid, int mediatypeid, char *sendto, char *subject, char *message)
{
	int	now;
	char	sql[MAX_STRING_LEN];
	char	sendto_esc[MAX_STRING_LEN];
	char	subject_esc[MAX_STRING_LEN];
	char	message_esc[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_alert()");

	now = time(NULL);
/* Does not work on PostgreSQL */
/*	snprintf(sql,sizeof(sql)-1,"insert into alerts (alertid,actionid,clock,mediatypeid,sendto,subject,message,status,retries) values (NULL,%d,%d,%d,'%s','%s','%s',0,0)",actionid,now,mediatypeid,sendto,subject,message);*/
	DBescape_string(sendto,sendto_esc,MAX_STRING_LEN);
	DBescape_string(subject,subject_esc,MAX_STRING_LEN);
	DBescape_string(message,message_esc,MAX_STRING_LEN);
	snprintf(sql,sizeof(sql)-1,"insert into alerts (actionid,clock,mediatypeid,sendto,subject,message,status,retries) values (%d,%d,%d,'%s','%s','%s',0,0)",actionid,now,mediatypeid,sendto_esc,subject_esc,message_esc);
	DBexecute_thread(database, sql);

	return SUCCEED;
}
#endif

int	DBadd_alert(int actionid, int mediatypeid, char *sendto, char *subject, char *message)
{
	int	now;
	char	sql[MAX_STRING_LEN];
	char	sendto_esc[MAX_STRING_LEN];
	char	subject_esc[MAX_STRING_LEN];
	char	message_esc[MAX_STRING_LEN];

	allview_log(LOG_LEVEL_DEBUG,"In add_alert()");

	now = time(NULL);
/* Does not work on PostgreSQL */
/*	snprintf(sql,sizeof(sql)-1,"insert into alerts (alertid,actionid,clock,mediatypeid,sendto,subject,message,status,retries) values (NULL,%d,%d,%d,'%s','%s','%s',0,0)",actionid,now,mediatypeid,sendto,subject,message);*/
	DBescape_string(sendto,sendto_esc,MAX_STRING_LEN);
	DBescape_string(subject,subject_esc,MAX_STRING_LEN);
	DBescape_string(message,message_esc,MAX_STRING_LEN);
	snprintf(sql,sizeof(sql)-1,"insert into alerts (actionid,clock,mediatypeid,sendto,subject,message,status,retries) values (%d,%d,%d,'%s','%s','%s',0,0)",actionid,now,mediatypeid,sendto_esc,subject_esc,message_esc);
	DBexecute(sql);

	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
void	DBvacuum_thread(MYSQL *database)
{
#ifdef	HAVE_PGSQL
	char *table_for_housekeeping[]={"services", "services_links", "graphs_items", "graphs", "sysmaps_links",
			"sysmaps_hosts", "sysmaps", "config", "groups", "hosts_groups", "alerts",
			"actions", "alarms", "functions", "history", "history_str", "hosts", "trends",
			"items", "media", "media_type", "triggers", "trigger_depends", "users",
			"sessions", "rights", "service_alarms", "profiles", "screens", "screens_items",
			"stats",
			NULL};

	char	sql[MAX_STRING_LEN];
	char	*table;
	int	i;
#ifdef HAVE_FUNCTION_SETPROCTITLE
	setproctitle("housekeeper [vacuum DB]");
#endif
	i=0;
	while (NULL != (table = table_for_housekeeping[i++]))
	{
		snprintf(sql,sizeof(sql)-1,"vacuum analyze %s", table);
		DBexecute(sql);
	}
#endif

#ifdef	HAVE_MYSQL
	/* Nothing to do */
#endif
}
#endif

void	DBvacuum(void)
{
#ifdef	HAVE_PGSQL
	char *table_for_housekeeping[]={"services", "services_links", "graphs_items", "graphs", "sysmaps_links",
			"sysmaps_hosts", "sysmaps", "config", "groups", "hosts_groups", "alerts",
			"actions", "alarms", "functions", "history", "history_str", "hosts", "trends",
			"items", "media", "media_type", "triggers", "trigger_depends", "users",
			"sessions", "rights", "service_alarms", "profiles", "screens", "screens_items",
			"stats",
			NULL};

	char	sql[MAX_STRING_LEN];
	char	*table;
	int	i;
#ifdef HAVE_FUNCTION_SETPROCTITLE
	setproctitle("housekeeper [vacuum DB]");
#endif
	i=0;
	while (NULL != (table = table_for_housekeeping[i++]))
	{
		snprintf(sql,sizeof(sql)-1,"vacuum analyze %s", table);
		DBexecute(sql);
	}
#endif

#ifdef	HAVE_MYSQL
	/* Nothing to do */
#endif
}

void    DBescape_string(char *from, char *to, int maxlen)
{
	int	i,ptr;
	char	*f;

	ptr=0;
	f=(char *)strdup(from);
	for(i=0;f[i]!=0;i++)
	{
		if( (f[i]=='\'') || (f[i]=='\\'))
		{
			if(ptr>maxlen-1)	break;
			to[ptr]='\\';
			if(ptr+1>maxlen-1)	break;
			to[ptr+1]=f[i];
			ptr+=2;
		}
		else
		{
			if(ptr>maxlen-1)	break;
			to[ptr]=f[i];
			ptr++;
		}
	}
	free(f);

	to[ptr]=0;
	to[maxlen-1]=0;
}

void	DBget_item_from_db(DB_ITEM *item,DB_RESULT *result, int row)
{
	char	*s;
	int	i;

	i=row;

	item->itemid=atoi(DBget_field(result,i,0));
	item->key=DBget_field(result,i,1);
	item->host=DBget_field(result,i,2);
	item->port=atoi(DBget_field(result,i,3));
	item->delay=atoi(DBget_field(result,i,4));
	item->description=DBget_field(result,i,5);
	item->nextcheck=atoi(DBget_field(result,i,6));
	item->type=atoi(DBget_field(result,i,7));
	item->snmp_community=DBget_field(result,i,8);
	item->snmp_oid=DBget_field(result,i,9);
	item->useip=atoi(DBget_field(result,i,10));
	item->ip=DBget_field(result,i,11);
	item->history=atoi(DBget_field(result,i,12));
	s=DBget_field(result,i,13);
	if(s==NULL)
	{
		item->lastvalue_null=1;
	}
	else
	{
		item->lastvalue_null=0;
		item->lastvalue_str=s;
		item->lastvalue=atof(s);
	}
	s=DBget_field(result,i,14);
	if(s==NULL)
	{
		item->prevvalue_null=1;
	}
	else
	{
		item->prevvalue_null=0;
		item->prevvalue_str=s;
		item->prevvalue=atof(s);
	}
	item->hostid=atoi(DBget_field(result,i,15));
	item->host_status=atoi(DBget_field(result,i,16));
	item->value_type=atoi(DBget_field(result,i,17));

	item->host_network_errors=atoi(DBget_field(result,i,18));
	item->snmp_port=atoi(DBget_field(result,i,19));
	item->delta=atoi(DBget_field(result,i,20));

	s=DBget_field(result,i,21);
	if(s==NULL)
	{
		item->prevorgvalue_null=1;
	}
	else
	{
		item->prevorgvalue_null=0;
		item->prevorgvalue=atof(s);
	}
	s=DBget_field(result,i,22);
	if(s==NULL)
	{
		item->lastclock=0;
	}
	else
	{
		item->lastclock=atoi(s);
	}

	item->units=DBget_field(result,i,23);
	item->multiplier=atoi(DBget_field(result,i,24));

	item->snmpv3_securityname = DBget_field(result,i,25);
	item->snmpv3_securitylevel = atoi(DBget_field(result,i,26));
	item->snmpv3_authpassphrase = DBget_field(result,i,27);
	item->snmpv3_privpassphrase = DBget_field(result,i,28);
	item->formula = DBget_field(result,i,29);
	item->host_available=atoi(DBget_field(result,i,30));
}

