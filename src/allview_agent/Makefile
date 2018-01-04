# Build rules

LIBS = -lnsl -lkstat -lsocket  

all:

#gcc -o ../../bin/allview_agent  -g -O2 -I../../include allview_agent.c sysinfo.c ../../include/log.c ../../include/cfg.c ../../include/security.c ../../include/snprintf.c ../../include/md5.c -Wall $(LIBS)
	gcc -o ../../bin/allview_agentd -g -O2 -I../../include allview_agentd.c sysinfo.c stats.c interfaces.c diskdevices.c cpustat.c ../../include/log.c ../../include/cfg.c ../../include/security.c ../../include/pid.c ../../include/snprintf.c ../../include/md5.c -Wall $(LIBS)

clean:
	rm -f *.o 
	rm -f ../../bin/allview_agent
	rm -f ../../bin/allview_agentd

