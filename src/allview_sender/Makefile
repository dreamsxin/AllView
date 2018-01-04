# Build rules

all:

	gcc -o ../../bin/allview_sender -g -O2 -I../../include ../../include/snprintf.c allview_sender.c -lnsl -lkstat -lsocket 

clean:
	rm -f *.o 
	rm -f ../../bin/allview_sender

