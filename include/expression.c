
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "functions.h"
#include "common.h"
#include "db.h"
#include "log.h"
#include "zlog.h"

/*
 * Delete all right spaces from given string
 */ 
void	rtrim_spaces(char *c)
{
	int i,len;

	len=strlen(c);
	for(i=len-1;i>=0;i--)
	{
		if( c[i] == ' ')
		{
			c[i]=0;
		}
		else	break;
	}
}

/*
 * Delete all left spaces from given string
 */ 
void	ltrim_spaces(char *c)
{
	int i,spaces;

/* Number of left spaces */
	spaces=0;
	for(i=0;c[i]!=0;i++)
	{
		if( c[i] == ' ')
		{
			spaces++;
		}
		else	break;
	}
	for(i=0;c[i+spaces]!=0;i++)
	{
		c[i]=c[i+spaces];
	}

	c[strlen(c)-spaces]=0;
}

/*
 * Delete all left and right spaces from given string
 */ 
void	lrtrim_spaces(char *c)
{
	ltrim_spaces(c);
	rtrim_spaces(c);
}

/* Convert string to double. This function supports prefixes 'K', 'M', 'G' */
double	str2double(char *str)
{
	if(str[strlen(str)-1] == 'K')
	{
		str[strlen(str)-1] = 0;
		return (double)1024*atof(str);
	}
	else if(str[strlen(str)-1] == 'M')
	{
		str[strlen(str)-1] = 0;
		return (double)1024*1024*atof(str);
	}
	else if(str[strlen(str)-1] == 'G')
	{
		str[strlen(str)-1] = 0;
		return (double)1024*1024*1024*atof(str);
	}
	return atof(str);
}

/*
 * Return 0 if arguments are equal (differs less than 0.000001), 1 - otherwise
 */
int	cmp_double(double a,double b)
{
	if(fabs(a-b)<0.000001)
	{
		return	0;
	}
	return	1;
}

/*
 * Return SUCCEED if parameter has format X.X<prefix> or X, where X is [0..9]{1,n}
 * In other words, parameter is float number :)
 * Prefix: K,M,G (kilo, mega, giga)
 */
int	is_double(char *c)
{
	int i;
	int dot=-1;

	allview_log(LOG_LEVEL_DEBUG, "Starting is_double:[%s]", c );
	for(i=0;c[i]!=0;i++)
	{
		if((c[i]>='0')&&(c[i]<='9'))
		{
			continue;
		}

		if((c[i]=='.')&&(dot==-1))
		{
			dot=i;

			if((dot!=0)&&(dot!=(int)strlen(c)-1))
			{
				continue;
			}
		}
		/* Last digit is prefix 'K', 'M', 'G' */
		if( ((c[i]=='K')||(c[i]=='M')||(c[i]=='G')) && (i == (int)strlen(c)-1))
		{
			continue;
		}

		allview_log(LOG_LEVEL_DEBUG, "It is NOT double [%s]",c );
		return FAIL;
	}
	allview_log(LOG_LEVEL_DEBUG, "It is double" );
	return SUCCEED;
}

/*
 * Delete all spaces from given string
 */ 
void	delete_spaces(char *c)
{
	int i,j;

	allview_log( LOG_LEVEL_DEBUG, "Before deleting spaces:%s", c );

	j=0;
/*	for(i=0;i<(int)strlen(c);i++)*/
	for(i=0;c[i]!=0;i++)
	{
		if( c[i] != ' ')
		{
			c[j]=c[i];
			j++;
		}
	}
	c[j]=0;

	allview_log(LOG_LEVEL_DEBUG, "After deleting spaces:%s", c );
}

/*
 * Locate character in given string. FAIL - not found, otherwise character position is returned
 */
int	find_char(char *str,char c)
{
	int i;

	allview_log( LOG_LEVEL_DEBUG, "Before find_char:%s[%c]", str, c );

/*	for(i=0;i<(int)strlen(str);i++)*/
	for(i=0;str[i]!=0;i++)
	{
		if(str[i]==c) return i;
	}
	return	FAIL;
}

/*
 * Evaluate simple expression
 * Simple expression is either <float> or <float> <operator> <float>
 */ 
int	evaluate_simple (double *result,char *exp)
{
	double	value1,value2;
	char	first[MAX_STRING_LEN],second[MAX_STRING_LEN];
	int	i,j,l;

	allview_log( LOG_LEVEL_DEBUG, "Evaluating simple expression [%s]", exp );

/* Remove left and right spaces */
	lrtrim_spaces(exp);

	if( is_double(exp) == SUCCEED )
	{
/*		*result=atof(exp);*/
/* str2double support prefixes */
		*result=str2double(exp);
		return SUCCEED;
	}

	if( find_char(exp,'|') != FAIL )
	{
		allview_log( LOG_LEVEL_DEBUG, "| is found" );
		l=find_char(exp,'|');
		strscpy( first, exp );
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( value1 == 1)
		{
			*result=value1;
			return SUCCEED;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if( value2 == 1)
		{
			*result=value2;
			return SUCCEED;
		}
		*result=0;
		return SUCCEED;
	}
	else if( find_char(exp,'&') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "& is found" );
		l=find_char(exp,'&');
		strscpy( first, exp );
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		allview_log(LOG_LEVEL_DEBUG, "[%s] [%s]",first,second );
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if( (value1 == 1) && (value2 == 1) )
		{
			*result=1;
		}
		else
		{
			*result=0;
		}
		return SUCCEED;
	}
	else if( find_char(exp,'>') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "> is found" );
		l=find_char(exp,'>');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if( value1 > value2 )
		{
			*result=1;
		}
		else
		{
			*result=0;
		}
		return SUCCEED;
	}
	else if( find_char(exp,'<') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "< is found" );
		l=find_char(exp,'<');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		allview_log(LOG_LEVEL_DEBUG, "[%s] [%s]",first,second );
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if( value1 < value2 )
		{
			*result=1;
		}
		else
		{
			*result=0;
		}
		allview_log(LOG_LEVEL_DEBUG, "Result [%f]",*result );
		return SUCCEED;
	}
	else if( find_char(exp,'*') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "* is found" );
		l=find_char(exp,'*');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		*result=value1*value2;
		return SUCCEED;
	}
	else if( find_char(exp,'/') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "/ is found" );
		l=find_char(exp,'/');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if(cmp_double(value2,0) == 0)
		{
			allview_log(LOG_LEVEL_WARNING, "Division by zero. Cannot evaluate expression [%s/%s]", first,second );
			allview_syslog("Division by zero. Cannot evaluate expression [%s/%s]", first,second );
			return FAIL;
		}
		else
		{
			*result=value1/value2;
		}
		return SUCCEED;
	}
	else if( find_char(exp,'+') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "+ is found" );
		l=find_char(exp,'+');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		*result=value1+value2;
		return SUCCEED;
	}
	else if( find_char(exp,'-') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "- is found" );
		l=find_char(exp,'-');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		*result=value1-value2;
		return SUCCEED;
	}
	else if( find_char(exp,'=') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "= is found" );
		l=find_char(exp,'=');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if( cmp_double(value1,value2) ==0 )
		{
			*result=1;
		}
		else
		{
			*result=0;
		}
		return SUCCEED;
	}
	else if( find_char(exp,'#') != FAIL )
	{
		allview_log(LOG_LEVEL_DEBUG, "# is found" );
		l=find_char(exp,'#');
		strscpy(first, exp);
		first[l]=0;
		j=0;
/*		for(i=l+1;i<(int)strlen(exp);i++)*/
		for(i=l+1;exp[i]!=0;i++)
		{
			second[j]=exp[i];
			j++;
		}
		second[j]=0;
		if( evaluate_simple(&value1,first) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", first );
			allview_syslog("Cannot evaluate expression [%s]", first );
			return FAIL;
		}
		if( evaluate_simple(&value2,second) == FAIL )
		{
			allview_log(LOG_LEVEL_DEBUG, "Cannot evaluate expression [%s]", second );
			allview_syslog("Cannot evaluate expression [%s]", second );
			return FAIL;
		}
		if( cmp_double(value1,value2) != 0 )
		{
			*result=1;
		}
		else
		{
			*result=0;
		}
		return SUCCEED;
	}
	else
	{
			allview_log( LOG_LEVEL_WARNING, "Format error or unsupported operator.  Exp: [%s]", exp );
			return FAIL;
	}
	return SUCCEED;
}

/*
 * Evaluate expression. Example of input expression: ({15}>10)|({123}=1) 
 */ 
int	evaluate(int *result,char *exp)
{
	double	value;
	char	res[MAX_STRING_LEN];
	char	simple[MAX_STRING_LEN];
	int	i,l,r;

	allview_log(LOG_LEVEL_DEBUG, "In evaluate([%s])",exp);

	strscpy( res,exp );

	while( find_char( exp, ')' ) != FAIL )
	{
		l=-1;
		r=find_char(exp,')');
		for(i=r;i>=0;i--)
		{
			if( exp[i] == '(' )
			{
				l=i;
				break;
			}
		}
		if( r == -1 )
		{
			allview_log(LOG_LEVEL_WARNING, "Cannot find left bracket [(]. Expression:[%s]", exp );
			allview_syslog("Cannot find left bracket [(]. Expression:[%s]", exp );
			return	FAIL;
		}
		for(i=l+1;i<r;i++)
		{
			simple[i-l-1]=exp[i];
		} 
		simple[r-l-1]=0;

		if( evaluate_simple( &value, simple ) != SUCCEED )
		{
			/* Changed to LOG_LEVEL_DEBUG */
			allview_log( LOG_LEVEL_DEBUG, "Unable to evaluate simple expression1 [%s]", simple );
			allview_syslog("Unable to evaluate simple expression1 [%s]", simple );
			return	FAIL;
		}

		allview_log(LOG_LEVEL_DEBUG, "Expression1:[%s]", exp );

		exp[l]='%';
		exp[l+1]='l';
		exp[l+2]='f';
/*		exp[l]='%';
		exp[l+1]='f';
		exp[l+2]=' ';*/

		for(i=l+3;i<=r;i++) exp[i]=' ';

		snprintf(res,sizeof(res)-1,exp,value);
		strcpy(exp,res);
		delete_spaces(res);
		allview_log(LOG_LEVEL_DEBUG, "Expression4:[%s]", res );
	}
	if( evaluate_simple( &value, res ) != SUCCEED )
	{
		allview_log(LOG_LEVEL_WARNING, "Unable to evaluate simple expression2 [%s]", simple );
		allview_syslog("Unable to evaluate simple expression2 [%s]", simple );
		return	FAIL;
	}
	allview_log( LOG_LEVEL_DEBUG, "Evaluate end:[%lf]", value );
	*result=value;
	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
/* Translate {DATE}, {TIME} */
/* Doesn't work yet */
void	substitute_simple_macros_thread(MYSQL *database, DB_TRIGGER *trigger, DB_ACTION *action, char *exp)
{
	int	found = SUCCEED;
	char	*s;
	char	sql[MAX_STRING_LEN];
	char	str[MAX_STRING_LEN];
	char	tmp[MAX_STRING_LEN];

	time_t  now;
	struct  tm      *tm;

	DB_RESULT *result;

	allview_log(LOG_LEVEL_DEBUG, "In substitute_simple_macros [%s]",exp);

	while (found == SUCCEED)
	{
		strscpy(str, exp);


		if( (s = strstr(str,"{HOSTNAME}")) != NULL )
		{
/*			snprintf(sql,sizeof(sql)-1,"select distinct t.description,h.host from triggers t, functions f,items i, hosts h where t.triggerid=%d and f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid", trigger->triggerid);*/
			snprintf(sql,sizeof(sql)-1,"select distinct h.host from triggers t, functions f,items i, hosts h where t.triggerid=%d and f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid", trigger->triggerid);
			result = DBselect_thread(database, sql);

			if(DBnum_rows(result) == 0)
			{
				allview_log( LOG_LEVEL_ERR, "No hostname in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				allview_syslog("No hostname in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				strscpy(tmp, "*UNKNOWN*");
				DBfree_result(result);
			}
			else
			{
				strscpy(tmp,DBget_field(result,0,0));

				DBfree_result(result);
			}

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{HOSTNAME}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{IPADDRESS}")) != NULL )
		{
			snprintf(sql,sizeof(sql)-1,"select distinct h.ip from triggers t, functions f,items i, hosts h where t.triggerid=%d and f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and i.useip=1", trigger->triggerid);
			result = DBselect_thread(database, sql);

			if(DBnum_rows(result) == 0)
			{
				allview_log( LOG_LEVEL_ERR, "No IP address in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				allview_syslog("No IP address in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				strscpy(tmp, "*UNKNOWN IP*");
				DBfree_result(result);
			}
			else
			{
				strscpy(tmp,DBget_field(result,0,0));

				DBfree_result(result);
			}

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{IPADDRESS}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{DATE}")) != NULL )
		{
			now=time(NULL);
			tm=localtime(&now);
			snprintf(tmp,sizeof(tmp)-1,"%.4d.%.2d.%.2d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday);

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{DATE}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{TIME}")) != NULL )
		{
			now=time(NULL);
			tm=localtime(&now);
			snprintf(tmp,sizeof(tmp)-1,"%.2d:%.2d:%.2d",tm->tm_hour,tm->tm_min,tm->tm_sec);

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{TIME}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{STATUS}")) != NULL )
		{
			/* This is old value */
			if(trigger->value == TRIGGER_VALUE_TRUE)
			{
				snprintf(tmp,sizeof(tmp)-1,"OFF");
			}
			else
			{
				snprintf(tmp,sizeof(tmp)-1,"ON");
			}

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{STATUS}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else
		{
			found = FAIL;
		}
	}

	allview_log( LOG_LEVEL_DEBUG, "Result expression [%s]", exp );
}
#endif

/* Translate {DATE}, {TIME} */
/* Doesn't work yet */
void	substitute_simple_macros(DB_TRIGGER *trigger, DB_ACTION *action, char *exp)
{
	int	found = SUCCEED;
	char	*s;
	char	sql[MAX_STRING_LEN];
	char	str[MAX_STRING_LEN];
	char	tmp[MAX_STRING_LEN];

	time_t  now;
	struct  tm      *tm;

	DB_RESULT *result;

	allview_log(LOG_LEVEL_DEBUG, "In substitute_simple_macros [%s]",exp);

	while (found == SUCCEED)
	{
		strscpy(str, exp);


		if( (s = strstr(str,"{HOSTNAME}")) != NULL )
		{
/*			snprintf(sql,sizeof(sql)-1,"select distinct t.description,h.host from triggers t, functions f,items i, hosts h where t.triggerid=%d and f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid", trigger->triggerid);*/
			snprintf(sql,sizeof(sql)-1,"select distinct h.host from triggers t, functions f,items i, hosts h where t.triggerid=%d and f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid", trigger->triggerid);
			result = DBselect(sql);

			if(DBnum_rows(result) == 0)
			{
				allview_log( LOG_LEVEL_ERR, "No hostname in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				allview_syslog("No hostname in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				strscpy(tmp, "*UNKNOWN*");
				DBfree_result(result);
			}
			else
			{
				strscpy(tmp,DBget_field(result,0,0));

				DBfree_result(result);
			}

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{HOSTNAME}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{IPADDRESS}")) != NULL )
		{
			snprintf(sql,sizeof(sql)-1,"select distinct h.ip from triggers t, functions f,items i, hosts h where t.triggerid=%d and f.triggerid=t.triggerid and f.itemid=i.itemid and h.hostid=i.hostid and i.useip=1", trigger->triggerid);
			result = DBselect(sql);

			if(DBnum_rows(result) == 0)
			{
				allview_log( LOG_LEVEL_ERR, "No IP address in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				allview_syslog("No IP address in substitute_simple_macros. Triggerid [%d]", trigger->triggerid);
				strscpy(tmp, "*UNKNOWN IP*");
				DBfree_result(result);
			}
			else
			{
				strscpy(tmp,DBget_field(result,0,0));

				DBfree_result(result);
			}

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{IPADDRESS}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{DATE}")) != NULL )
		{
			now=time(NULL);
			tm=localtime(&now);
			snprintf(tmp,sizeof(tmp)-1,"%.4d.%.2d.%.2d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday);

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{DATE}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{TIME}")) != NULL )
		{
			now=time(NULL);
			tm=localtime(&now);
			snprintf(tmp,sizeof(tmp)-1,"%.2d:%.2d:%.2d",tm->tm_hour,tm->tm_min,tm->tm_sec);

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{TIME}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else if( (s = strstr(str,"{STATUS}")) != NULL )
		{
			/* This is old value */
			if(trigger->value == TRIGGER_VALUE_TRUE)
			{
				snprintf(tmp,sizeof(tmp)-1,"OFF");
			}
			else
			{
				snprintf(tmp,sizeof(tmp)-1,"ON");
			}

			s[0]=0;
			strcpy(exp, str);
			strncat(exp, tmp, MAX_STRING_LEN);
			strncat(exp, s+strlen("{STATUS}"), MAX_STRING_LEN);

			found = SUCCEED;
		}
		else
		{
			found = FAIL;
		}
	}

	allview_log( LOG_LEVEL_DEBUG, "Result expression [%s]", exp );
}

#ifdef ALLVIEW_THREADS
/*
 * Translate "{127.0.0.1:system[procload].last(0)}" to "1.34" 
 */
/*
 * Make this function more secure. Get rid of snprintf. Utilise substr()
*/
int	substitute_macros_thread(MYSQL *database, DB_TRIGGER *trigger, DB_ACTION *action, char *exp)
{
	char	res[MAX_STRING_LEN];
	char	macro[MAX_STRING_LEN];
	char	host[MAX_STRING_LEN];
	char	key[MAX_STRING_LEN];
	char	function[MAX_STRING_LEN];
	char	parameter[MAX_STRING_LEN];
	static	char	value[MAX_STRING_LEN];
	int	i;
	int	r,l;
	int	r1,l1;

	allview_log(LOG_LEVEL_DEBUG, "In substitute_macros([%s])",exp);

	substitute_simple_macros_thread(database, trigger, action, exp);

	while( find_char(exp,'{') != FAIL )
	{
		l=find_char(exp,'{');
		r=find_char(exp,'}');

		if( r == FAIL )
		{
			allview_log( LOG_LEVEL_WARNING, "Cannot find right bracket. Expression:[%s]", exp );
			allview_syslog("Cannot find right bracket. Expression:[%s]", exp );
			return	FAIL;
		}

		if( r < l )
		{
			allview_log( LOG_LEVEL_WARNING, "Right bracket is before left one. Expression:[%s]", exp );
			allview_syslog("Right bracket is before left one. Expression:[%s]", exp );
			return	FAIL;
		}

		for(i=l+1;i<r;i++)
		{
			macro[i-l-1]=exp[i];
		} 
		macro[r-l-1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Macro:%s", macro );

		/* macro=="host:key.function(parameter)" */

		r1=find_char(macro,':');

		for(i=0;i<r1;i++)
		{
			host[i]=macro[i];
		} 
		host[r1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Host:%s", host );

		r1=r1+1;
/* Doesn't work if the key contains '.' */
/*		l1=find_char(macro+r1,'.');*/

		l1=FAIL;
		for(i=0;(macro+r1)[i]!=0;i++)
		{
			if((macro+r1)[i]=='.') l1=i;
		}

		for(i=r1;i<l1+r1;i++)
		{
			key[i-r1]=macro[i];
		} 
		key[l1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Key:%s", key );

		l1=l1+r1+1;
		r1=find_char(macro+l1,'(');

		for(i=l1;i<l1+r1;i++)
		{
			function[i-l1]=macro[i];
		} 
		function[r1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Function:%s", function );

		l1=l1+r1+1;
		r1=find_char(macro+l1,')');

		for(i=l1;i<l1+r1;i++)
		{
			parameter[i-l1]=macro[i];
		} 
		parameter[r1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Parameter:%s", parameter );

		i=get_lastvalue_thread(database,value,host,key,function,parameter);
		allview_log( LOG_LEVEL_DEBUG, "Value3 [%s]", value );


		allview_log( LOG_LEVEL_DEBUG, "Value4 [%s]", exp );
		exp[l]='%';
		exp[l+1]='s';

		allview_log( LOG_LEVEL_DEBUG, "Value41 [%s]", exp+l+2 );
		allview_log( LOG_LEVEL_DEBUG, "Value42 [%s]", exp+r+1 );
		strcpy(exp+l+2,exp+r+1);

		allview_log( LOG_LEVEL_DEBUG, "Value5 [%s]", exp );

		snprintf(res,sizeof(res)-1,exp,value);
		strcpy(exp,res);
/*		delete_spaces(exp); */
		allview_log( LOG_LEVEL_DEBUG, "Expression4:[%s]", exp );
	}

	allview_log( LOG_LEVEL_DEBUG, "Result expression:%s", exp );

	return SUCCEED;
}
#endif

/*
 * Translate "{127.0.0.1:system[procload].last(0)}" to "1.34" 
 */
/*
 * Make this function more secure. Get rid of snprintf. Utilise substr()
*/
int	substitute_macros(DB_TRIGGER *trigger, DB_ACTION *action, char *exp)
{
	char	res[MAX_STRING_LEN];
	char	macro[MAX_STRING_LEN];
	char	host[MAX_STRING_LEN];
	char	key[MAX_STRING_LEN];
	char	function[MAX_STRING_LEN];
	char	parameter[MAX_STRING_LEN];
	static	char	value[MAX_STRING_LEN];
	int	i;
	int	r,l;
	int	r1,l1;

	allview_log(LOG_LEVEL_DEBUG, "In substitute_macros([%s])",exp);

	substitute_simple_macros(trigger, action, exp);

	while( find_char(exp,'{') != FAIL )
	{
		l=find_char(exp,'{');
		r=find_char(exp,'}');

		if( r == FAIL )
		{
			allview_log( LOG_LEVEL_WARNING, "Cannot find right bracket. Expression:[%s]", exp );
			allview_syslog("Cannot find right bracket. Expression:[%s]", exp );
			return	FAIL;
		}

		if( r < l )
		{
			allview_log( LOG_LEVEL_WARNING, "Right bracket is before left one. Expression:[%s]", exp );
			allview_syslog("Right bracket is before left one. Expression:[%s]", exp );
			return	FAIL;
		}

		for(i=l+1;i<r;i++)
		{
			macro[i-l-1]=exp[i];
		} 
		macro[r-l-1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Macro:%s", macro );

		/* macro=="host:key.function(parameter)" */

		r1=find_char(macro,':');

		for(i=0;i<r1;i++)
		{
			host[i]=macro[i];
		} 
		host[r1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Host:%s", host );

		r1=r1+1;
/* Doesn't work if the key contains '.' */
/*		l1=find_char(macro+r1,'.');*/

		l1=FAIL;
		for(i=0;(macro+r1)[i]!=0;i++)
		{
			if((macro+r1)[i]=='.') l1=i;
		}

		for(i=r1;i<l1+r1;i++)
		{
			key[i-r1]=macro[i];
		} 
		key[l1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Key:%s", key );

		l1=l1+r1+1;
		r1=find_char(macro+l1,'(');

		for(i=l1;i<l1+r1;i++)
		{
			function[i-l1]=macro[i];
		} 
		function[r1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Function:%s", function );

		l1=l1+r1+1;
		r1=find_char(macro+l1,')');

		for(i=l1;i<l1+r1;i++)
		{
			parameter[i-l1]=macro[i];
		} 
		parameter[r1]=0;

		allview_log( LOG_LEVEL_DEBUG, "Parameter:%s", parameter );

		i=get_lastvalue(value,host,key,function,parameter);
		allview_log( LOG_LEVEL_DEBUG, "Value3 [%s]", value );


		allview_log( LOG_LEVEL_DEBUG, "Value4 [%s]", exp );
		exp[l]='%';
		exp[l+1]='s';

		allview_log( LOG_LEVEL_DEBUG, "Value41 [%s]", exp+l+2 );
		allview_log( LOG_LEVEL_DEBUG, "Value42 [%s]", exp+r+1 );
		strcpy(exp+l+2,exp+r+1);

		allview_log( LOG_LEVEL_DEBUG, "Value5 [%s]", exp );

		snprintf(res,sizeof(res)-1,exp,value);
		strcpy(exp,res);
/*		delete_spaces(exp); */
		allview_log( LOG_LEVEL_DEBUG, "Expression4:[%s]", exp );
	}

	allview_log( LOG_LEVEL_DEBUG, "Result expression:%s", exp );

	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
/*
 * Translate "({15}>10)|({123}=0)" to "(6.456>10)|(0=0)" 
 */
int	substitute_functions_thread(MYSQL *database, char *exp)
{
	double	value;
	char	functionid[MAX_STRING_LEN];
	char	res[MAX_STRING_LEN];
	int	i,l,r;

	allview_log(LOG_LEVEL_DEBUG, "BEGIN substitute_functions (%s)", exp);

	while( find_char(exp,'{') != FAIL )
	{
		l=find_char(exp,'{');
		r=find_char(exp,'}');
		if( r == FAIL )
		{
			allview_log( LOG_LEVEL_WARNING, "Cannot find right bracket. Expression:[%s]", exp );
			allview_syslog("Cannot find right bracket. Expression:[%s]", exp );
			return	FAIL;
		}
		if( r < l )
		{
			allview_log( LOG_LEVEL_WARNING, "Right bracket is before left one. Expression:[%s]", exp );
			allview_syslog("Right bracket is before left one. Expression:[%s]", exp );
			return	FAIL;
		}

		for(i=l+1;i<r;i++)
		{
			functionid[i-l-1]=exp[i];
		} 
		functionid[r-l-1]=0;

		if( DBget_function_result_thread(database, &value, functionid ) != SUCCEED )
		{
/* It may happen because of functions.lastvalue is NULL, so this is not warning  */
			allview_log( LOG_LEVEL_DEBUG, "Unable to get value for functionid [%s]", functionid );
			allview_syslog("Unable to get value for functionid [%s]", functionid );
			return	FAIL;
		}


		allview_log( LOG_LEVEL_DEBUG, "Expression1:[%s]", exp );

		exp[l]='%';
		exp[l+1]='l';
		exp[l+2]='f';
/*		exp[l]='%';
		exp[l+1]='f';
		exp[l+2]=' ';*/

		allview_log( LOG_LEVEL_DEBUG, "Expression2:[%s]", exp );

		for(i=l+3;i<=r;i++) exp[i]=' ';

		allview_log( LOG_LEVEL_DEBUG, "Expression3:[%s]", exp );

		snprintf(res,sizeof(res)-1,exp,value);
		strcpy(exp,res);
		delete_spaces(exp);
		allview_log( LOG_LEVEL_DEBUG, "Expression4:[%s]", exp );
	}
	allview_log( LOG_LEVEL_DEBUG, "Expression:[%s]", exp );
	allview_log( LOG_LEVEL_DEBUG, "END substitute_functions" );
	return SUCCEED;
}
#endif

/*
 * Translate "({15}>10)|({123}=0)" to "(6.456>10)|(0=0)" 
 */
int	substitute_functions(char *exp)
{
	double	value;
	char	functionid[MAX_STRING_LEN];
	char	res[MAX_STRING_LEN];
	int	i,l,r;

	allview_log(LOG_LEVEL_DEBUG, "BEGIN substitute_functions (%s)", exp);

	while( find_char(exp,'{') != FAIL )
	{
		l=find_char(exp,'{');
		r=find_char(exp,'}');
		if( r == FAIL )
		{
			allview_log( LOG_LEVEL_WARNING, "Cannot find right bracket. Expression:[%s]", exp );
			allview_syslog("Cannot find right bracket. Expression:[%s]", exp );
			return	FAIL;
		}
		if( r < l )
		{
			allview_log( LOG_LEVEL_WARNING, "Right bracket is before left one. Expression:[%s]", exp );
			allview_syslog("Right bracket is before left one. Expression:[%s]", exp );
			return	FAIL;
		}

		for(i=l+1;i<r;i++)
		{
			functionid[i-l-1]=exp[i];
		} 
		functionid[r-l-1]=0;

		if( DBget_function_result( &value, functionid ) != SUCCEED )
		{
/* It may happen because of functions.lastvalue is NULL, so this is not warning  */
			allview_log( LOG_LEVEL_DEBUG, "Unable to get value for functionid [%s]", functionid );
			allview_syslog("Unable to get value for functionid [%s]", functionid );
			return	FAIL;
		}


		allview_log( LOG_LEVEL_DEBUG, "Expression1:[%s]", exp );

		exp[l]='%';
		exp[l+1]='l';
		exp[l+2]='f';
/*		exp[l]='%';
		exp[l+1]='f';
		exp[l+2]=' ';*/

		allview_log( LOG_LEVEL_DEBUG, "Expression2:[%s]", exp );

		for(i=l+3;i<=r;i++) exp[i]=' ';

		allview_log( LOG_LEVEL_DEBUG, "Expression3:[%s]", exp );

		snprintf(res,sizeof(res)-1,exp,value);
		strcpy(exp,res);
		delete_spaces(exp);
		allview_log( LOG_LEVEL_DEBUG, "Expression4:[%s]", exp );
	}
	allview_log( LOG_LEVEL_DEBUG, "Expression:[%s]", exp );
	allview_log( LOG_LEVEL_DEBUG, "END substitute_functions" );
	return SUCCEED;
}

#ifdef ALLVIEW_THREADS
/*
 * Evaluate complex expression. Example: ({127.0.0.1:system[procload].last(0)}>1)|({127.0.0.1:system[procload].max(300)}>3)
 */ 
int	evaluate_expression_thread(MYSQL *database, int *result,char *expression)
{
	allview_log(LOG_LEVEL_DEBUG, "In evaluate_expression(%s)", expression );

	delete_spaces(expression);
	if( substitute_functions_thread(database,expression) == SUCCEED)
	{
		/* No need for _thread function */
		if( evaluate(result, expression) == SUCCEED)
		{
			return SUCCEED;
		}
	}
	allview_log(LOG_LEVEL_WARNING, "Evaluation of expression [%s] failed", expression );
	allview_syslog("Evaluation of expression [%s] failed", expression );
	return FAIL;
}
#endif

/*
 * Evaluate complex expression. Example: ({127.0.0.1:system[procload].last(0)}>1)|({127.0.0.1:system[procload].max(300)}>3)
 */ 
int	evaluate_expression(int *result,char *expression)
{
	allview_log(LOG_LEVEL_DEBUG, "In evaluate_expression(%s)", expression );

	delete_spaces(expression);
	if( substitute_functions(expression) == SUCCEED)
	{
		if( evaluate(result, expression) == SUCCEED)
		{
			return SUCCEED;
		}
	}
	allview_log(LOG_LEVEL_WARNING, "Evaluation of expression [%s] failed", expression );
	allview_syslog("Evaluation of expression [%s] failed", expression );
	return FAIL;
}

