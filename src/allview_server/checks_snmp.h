
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

#ifndef ALLVIEW_CHECKS_SNMP_H
#define ALLVIEW_CHECKS_SNMP_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "db.h"

/* NET-SNMP is used */
#ifdef HAVE_NETSNMP
	#include <net-snmp/net-snmp-config.h>
	#include <net-snmp/net-snmp-includes.h>
#endif

/* Required for SNMP support*/
#ifdef HAVE_UCDSNMP
	#include <ucd-snmp/ucd-snmp-config.h>
	#include <ucd-snmp/ucd-snmp-includes.h>
	#include <ucd-snmp/system.h>
/* For usmHMACMD5AuthProtocol */
	#include <ucd-snmp/transform_oids.h>
/* For generate_Ku() */
	#include <ucd-snmp/keytools.h>
#endif


/*int	get_value_SNMP(int version,double *result,char *result_str,DB_ITEM *item);*/
int	get_value_snmp(double *result,char *result_str,DB_ITEM *item,char *error, int max_error_len);

#endif
