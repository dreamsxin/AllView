# Build rules

INCLUDE = -I/usr/local/mysql/include/mysql  -I/usr/local/include
LIBS    = -lnsl -lkstat -lsocket  -L/usr/local/mysql/lib/mysql -lmysqlclient -lm -lz  -L/usr/local/lib -lnetsnmp  

all:

	gcc -o ../../bin/allview_server -g -O2 -I../../include $(INCLUDE) server.c ../../include/db.c ../../include/functions.c ../../include/expression.c ../../include/cfg.c ../../include/log.c ../../include/security.c ../../include/pid.c ../../include/email.c ../../include/snprintf.c ../allview_agent/sysinfo.c ../../include/md5.c ../../include/zlog.c alerter.c pinger.c housekeeper.c checks_simple.c checks_snmp.c checks_agent.c checks_internal.c -Wall $(LIBS) 

clean:
	rm -f *.o 
	rm -f ../../bin/allview_server

