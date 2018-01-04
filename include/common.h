
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


/*#define TESTTEST*/

/*
#define	ALLVIEW_THREADS
*/
#define	IT_HELPDESK

#ifndef ALLVIEW_COMMON_H
#define ALLVIEW_COMMON_H
 
#define	SUCCEED		0
#define	FAIL		(-1)
#define	NOTSUPPORTED	(-2)
#define	NETWORK_ERROR	(-3)
#define	TIMEOUT_ERROR	(-4)
#define	AGENT_ERROR	(-5)

#define	MAXFD	64

/*
#define ALV_POLLER
*/

#ifdef ALV_POLLER
	#define MAX_STRING_LEN	800
#else
	#define MAX_STRING_LEN	4096
#endif

/* Item types */
#define ITEM_TYPE_ALLVIEW	0
#define ITEM_TYPE_SNMPv1	1
#define ITEM_TYPE_TRAPPER	2
#define ITEM_TYPE_SIMPLE	3
#define ITEM_TYPE_SNMPv2c	4
#define ITEM_TYPE_INTERNAL	5
#define ITEM_TYPE_SNMPv3	6

/* Item value types */
#define ITEM_VALUE_TYPE_FLOAT	0
#define ITEM_VALUE_TYPE_STR	1

/* Item snmpv3 security levels */
#define ITEM_SNMPV3_SECURITYLEVEL_NOAUTHNOPRIV	0
#define ITEM_SNMPV3_SECURITYLEVEL_AUTHNOPRIV	1
#define ITEM_SNMPV3_SECURITYLEVEL_AUTHPRIV	2

/* Item multiplier types */
#define ITEM_MULTIPLIER_DO_NOT_USE		0
#define ITEM_MULTIPLIER_USE			1

/* Item delta types */
#define ITEM_STORE_AS_IS		0
#define ITEM_STORE_SPEED_PER_SECOND	1
#define ITEM_STORE_SIMPLE_CHANGE	2

/* Recipient types for actions */
#define RECIPIENT_TYPE_USER	0
#define RECIPIENT_TYPE_GROUP	1

/* Special item key used for storing server status */
#define SERVER_STATUS_KEY	"status"
/* Special item key used for ICMP pings */
#define SERVER_ICMPPING_KEY	"icmpping"
/* Special item key used for ICMP ping latency */
#define SERVER_ICMPPINGSEC_KEY	"icmppingsec"
/* Special item key used for internal ALLVIEW log */
#define SERVER_ALLVIEWLOG_KEY	"allview[log]"

/* Alert types */
#define ALERT_TYPE_EMAIL	0
#define ALERT_TYPE_EXEC		1

/* Item statuses */
#define ITEM_STATUS_ACTIVE	0
#define ITEM_STATUS_DISABLED	1
/*#define ITEM_STATUS_TRAPPED	2*/
#define ITEM_STATUS_NOTSUPPORTED	3
#define ITEM_STATUS_DELETED	4

/* Host statuses */
#define HOST_STATUS_MONITORED	0
#define HOST_STATUS_NOT_MONITORED	1
/*#define HOST_STATUS_UNREACHABLE	2*/
#define HOST_STATUS_TEMPLATE	3
#define HOST_STATUS_DELETED	4

/* Host availability */
#define HOST_AVAILABLE_UNKNOWN	0
#define HOST_AVAILABLE_TRUE	1
#define HOST_AVAILABLE_FALSE	2

/* Use host IP or host name */
#define HOST_USE_HOSTNAME	0
#define HOST_USE_IP		1

/* Trigger statuses */
/*#define TRIGGER_STATUS_FALSE	0
#define TRIGGER_STATUS_TRUE	1
#define TRIGGER_STATUS_DISABLED	2
#define TRIGGER_STATUS_UNKNOWN	3
#define TRIGGER_STATUS_NOTSUPPORTED	4*/

/* Trigger statuses */
#define TRIGGER_STATUS_ENABLED	0
#define TRIGGER_STATUS_DISABLED	1

/* Trigger values */
#define TRIGGER_VALUE_FALSE	0
#define TRIGGER_VALUE_TRUE	1
#define TRIGGER_VALUE_UNKNOWN	2

/* Media statuses */
#define MEDIA_STATUS_ACTIVE	0
#define MEDIA_STATUS_DISABLED	1

/* Algorithms for service status calculation */
#define SERVICE_ALGORITHM_NONE	0
#define SERVICE_ALGORITHM_MAX	1
#define SERVICE_ALGORITHM_MIN	2

/* Scope of action */
#define	ACTION_SCOPE_TRIGGER	0
#define	ACTION_SCOPE_HOST	1
#define	ACTION_SCOPE_HOSTS	2

#define	AGENTD_FORKS	5

#define	TRAPPERD_FORKS	5

#define	SUCKER_FORKS	11
#define	SUCKER_DELAY	60

#define	SUCKER_TIMEOUT	5
/* Delay on network failure*/
#define DELAY_ON_NETWORK_FAILURE 60

#define	AGENT_TIMEOUT	3

#define	SENDER_TIMEOUT		5
#define	TRAPPER_TIMEOUT		5
#define	SNMPTRAPPER_TIMEOUT	5

/* Secure string copy */
#define strscpy(x,y) { strncpy(x,y,sizeof(x)); x[sizeof(x)-1]=0; }

#endif
