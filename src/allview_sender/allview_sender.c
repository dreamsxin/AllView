
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
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>

/* OpenBSD*/
#ifdef HAVE_SYS_SOCKET_H
	#include <sys/socket.h>
#endif

#include <signal.h>
#include <time.h>

#include "common.h"

void    signal_handler( int sig )
{
	if( SIGALRM == sig )
	{
		signal( SIGALRM, signal_handler );
		fprintf(stderr,"Timeout while executing operation.\n");
	}
 
	if( SIGQUIT == sig || SIGINT == sig || SIGTERM == sig )
	{
/*		fprintf(stderr,"\nGot QUIT or INT or TERM signal. Exiting..." ); */
	}
	exit( FAIL );
}

int	send_value(char *server,int port,char *shortname,char *value)
{
	int	i,s;
	char	tosend[1024];
	char	result[1024];
	struct hostent *hp;

	struct sockaddr_in myaddr_in;
	struct sockaddr_in servaddr_in;

/*	struct linger ling;*/

	servaddr_in.sin_family=AF_INET;
	hp=gethostbyname(server);

	if(hp==NULL)
	{
		return	FAIL;
	}

	servaddr_in.sin_addr.s_addr=((struct in_addr *)(hp->h_addr))->s_addr;

	servaddr_in.sin_port=htons(port);

	s=socket(AF_INET,SOCK_STREAM,0);
	if(s == -1)
	{
		return	FAIL;
	}

/*	ling.l_onoff=1;*/
/*	ling.l_linger=0;*/
/*	if(setsockopt(s,SOL_SOCKET,SO_LINGER,&ling,sizeof(ling))==-1)*/
/*	{*/
/* Ignore */
/*	}*/
 
	myaddr_in.sin_family = AF_INET;
	myaddr_in.sin_port=0;
	myaddr_in.sin_addr.s_addr=INADDR_ANY;

	if( connect(s,(struct sockaddr *)&servaddr_in,sizeof(struct sockaddr_in)) == -1 )
	{
		close(s);
		return	FAIL;
	}

	snprintf(tosend,sizeof(tosend)-1,"%s:%s\n",shortname,value);

	if( sendto(s,tosend,strlen(tosend),0,(struct sockaddr *)&servaddr_in,sizeof(struct sockaddr_in)) == -1 )
	{
		perror("sendto");
		close(s);
		return	FAIL;
	} 
	i=sizeof(struct sockaddr_in);
/*	i=recvfrom(s,result,1023,0,(struct sockaddr *)&servaddr_in,(size_t *)&i);*/
	i=recvfrom(s,result,1023,0,(struct sockaddr *)&servaddr_in,(socklen_t *)&i);
	if(s==-1)
	{
		perror("recfrom");
		close(s);
		return	FAIL;
	}

	result[i-1]=0;

	if(strcmp(result,"OK") == 0)
	{
		printf("OK\n");
	}
 
	if( close(s)!=0 )
	{
		perror("close");
		
	}

	return SUCCEED;
}

int main(int argc, char **argv)
{
	int	port;
	int	ret=SUCCEED;
	char	line[MAX_STRING_LEN];
	char	port_str[MAX_STRING_LEN];
	char	allview_server[MAX_STRING_LEN];
	char	server_key[MAX_STRING_LEN];
	char	value[MAX_STRING_LEN];
	char	*s;

	signal( SIGINT,  signal_handler );
	signal( SIGQUIT, signal_handler );
	signal( SIGTERM, signal_handler );
	signal( SIGALRM, signal_handler );

	if(argc == 5)
	{
		port=atoi(argv[2]);

		alarm(SENDER_TIMEOUT);

		ret = send_value(argv[1],port,argv[3],argv[4]);

		alarm(0);
	}
/* No parameters are given */	
	else if(argc == 1)
	{
		while(fgets(line,MAX_STRING_LEN,stdin) != NULL)
		{
/*			printf("[%s]\n",line);*/
			alarm(SENDER_TIMEOUT);

			s=(char *)strtok(line," ");
			strscpy(allview_server,s);
			s=(char *)strtok(NULL," ");
			strscpy(port_str,s);
			s=(char *)strtok(NULL," ");
			strscpy(server_key,s);
			s=(char *)strtok(NULL," ");
			strscpy(value,s);
			ret = send_value(allview_server,atoi(port_str),server_key,value);

			alarm(0);
		}
	}
	else
	{
		printf("Usage: allview_sender <Allview server> <port> <server:key> <value>\n");
		printf("If no arguments are given, allview_sender expects list of parameters\n");
		printf("from standard input.\n");
		
		ret = FAIL;
	}

	return ret;
}
