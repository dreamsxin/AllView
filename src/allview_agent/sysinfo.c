
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

#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif
#ifdef HAVE_FCNTL_H
	#include <fcntl.h>
#endif
#ifdef HAVE_DIRENT_H
	#include <dirent.h>
#endif
/* Linux */
#ifdef HAVE_SYS_VFS_H
	#include <sys/vfs.h>
#endif
#ifdef HAVE_SYS_SYSINFO_H
	#include <sys/sysinfo.h>
#endif
/* Solaris */
#ifdef HAVE_SYS_STATVFS_H
	#include <sys/statvfs.h>
#endif
/* Solaris */
#ifdef HAVE_SYS_PROCFS_H
/* This is needed to access the correct procfs.h definitions */
	#define _STRUCTURED_PROC 1
	#include <sys/procfs.h>
#endif
#ifdef HAVE_SYS_LOADAVG_H
	#include <sys/loadavg.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
	#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
	#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
	#include <arpa/inet.h>
#endif
/* OpenBSD/Solaris */
#ifdef HAVE_SYS_PARAM_H
	#include <sys/param.h>
#endif

#ifdef HAVE_SYS_MOUNT_H
	#include <sys/mount.h>
#endif

/* HP-UX */
#ifdef HAVE_SYS_PSTAT_H
	#include <sys/pstat.h>
#endif

#ifdef HAVE_NETDB_H
	#include <netdb.h>
#endif

/* Solaris */
#ifdef HAVE_SYS_SWAP_H
	#include <sys/swap.h>
#endif

/* FreeBSD */
#ifdef HAVE_SYS_SYSCTL_H
	#include <sys/sysctl.h>
#endif

/* Solaris */
#ifdef HAVE_SYS_SYSCALL_H
	#include <sys/syscall.h>
#endif

/* FreeBSD */
#ifdef HAVE_VM_VM_PARAM_H
	#include <vm/vm_param.h>
#endif
/* FreeBSD */
#ifdef HAVE_SYS_VMMETER_H
	#include <sys/vmmeter.h>
#endif
/* FreeBSD */
#ifdef HAVE_SYS_TIME_H
	#include <sys/time.h>
#endif

#ifdef HAVE_KSTAT_H
	#include <kstat.h>
#endif

#ifdef HAVE_LDAP
	#include <ldap.h>
#endif

#include "common.h"
#include "sysinfo.h"

#include "md5.h"

void	forward_request(char *proxy,char *command,int port,char *value);

/*COMMAND	commands[AGENT_MAX_USER_COMMANDS]=*/
COMMAND	*commands=NULL;

COMMAND	agent_commands[]=
/* 	KEY		FUNCTION (if double) FUNCTION (if string) PARAM*/
	{
	{"kern[maxfiles]"	,KERNEL_MAXFILES,	0, 0},
	{"kern[maxproc]"	,KERNEL_MAXPROC, 	0, 0},

	{"proc_cnt[*]"		,PROCCNT, 		0, "inetd"},
	{"proc_cnts[*]"		,PROCCNTS, 		0, "inetd"},

	{"memory[total]"	,TOTALMEM, 		0, 0},
	{"memory[shared]"	,SHAREDMEM, 		0, 0},
	{"memory[buffers]"	,BUFFERSMEM, 		0, 0},
	{"memory[cached]"	,CACHEDMEM, 		0, 0},
	{"memory[free]"		,FREEMEM, 		0, 0},

	{"version[allview_agent]",0,	 		VERSION, 0},

	{"diskfree[*]"		,DISKFREE,		0, "/"},
	{"disktotal[*]"		,DISKTOTAL,		0, "/"},
	{"diskused[*]"		,DISKUSED,		0, "/"},

	{"diskfree_perc[*]"	,DISKFREE_PERC,		0, "/"},
	{"diskused_perc[*]"	,DISKUSED_PERC,		0, "/"},

	{"inodefree[*]"		,INODEFREE, 		0, "/"},

	{"inodefree_perc[*]"	,INODEFREE_PERC,	0, "/"},

	{"inodetotal[*]"	,INODETOTAL, 		0, "/"},

	{"cksum[*]"		,CKSUM, 		0, "/etc/services"},

	{"md5sum[*]"		,0, 			MD5SUM, "/etc/services"},

	{"filesize[*]"		,FILESIZE, 		0, "/etc/passwd"},

	{"cpu[idle1]"		,CPUIDLE1, 		0, 0},
	{"cpu[idle5]"		,CPUIDLE5, 		0, 0},
	{"cpu[idle15]"		,CPUIDLE15, 		0, 0},

	{"cpu[nice1]"		,CPUNICE1, 		0, 0},
	{"cpu[nice5]"		,CPUNICE5, 		0, 0},
	{"cpu[nice15]"		,CPUNICE15, 		0, 0},

	{"cpu[user1]"		,CPUUSER1, 		0, 0},
	{"cpu[user5]"		,CPUUSER5, 		0, 0},
	{"cpu[user15]"		,CPUUSER15, 		0, 0},

	{"cpu[system1]"		,CPUSYSTEM1, 		0, 0},
	{"cpu[system5]"		,CPUSYSTEM5, 		0, 0},
	{"cpu[system15]"	,CPUSYSTEM15, 		0, 0},

	{"netloadin1[*]"	,NETLOADIN1, 		0, "lo"},
	{"netloadin5[*]"	,NETLOADIN5, 		0, "lo"},
	{"netloadin15[*]"	,NETLOADIN15, 		0, "lo"},

	{"netloadout1[*]"	,NETLOADOUT1, 		0, "lo"},
	{"netloadout5[*]"	,NETLOADOUT5, 		0, "lo"},
	{"netloadout15[*]"	,NETLOADOUT15, 		0, "lo"},

	{"disk_read_ops1[*]"	,DISKREADOPS1, 		0, "hda"},
	{"disk_read_ops5[*]"	,DISKREADOPS5, 		0, "hda"},
	{"disk_read_ops15[*]"	,DISKREADOPS15,		0, "hda"},

	{"disk_read_blks1[*]"	,DISKREADBLKS1,		0, "hda"},
	{"disk_read_blks5[*]"	,DISKREADBLKS5,		0, "hda"},
	{"disk_read_blks15[*]"	,DISKREADBLKS15,	0, "hda"},

	{"disk_write_ops1[*]"	,DISKWRITEOPS1, 	0, "hda"},
	{"disk_write_ops5[*]"	,DISKWRITEOPS5, 	0, "hda"},
	{"disk_write_ops15[*]"	,DISKWRITEOPS15,	0, "hda"},

	{"disk_write_blks1[*]"	,DISKWRITEBLKS1,	0, "hda"},
	{"disk_write_blks5[*]"	,DISKWRITEBLKS5,	0, "hda"},
	{"disk_write_blks15[*]"	,DISKWRITEBLKS15,	0, "hda"},

	{"sensor[temp1]"	,SENSOR_TEMP1, 		0, 0},
	{"sensor[temp2]"	,SENSOR_TEMP2, 		0, 0},
	{"sensor[temp3]"	,SENSOR_TEMP3, 		0, 0},

	{"swap[free]"		,SWAPFREE, 		0, 0},
	{"swap[total]"		,SWAPTOTAL, 		0, 0},

/****************************************
  	All these perameters require more than 1 second to retrieve.

  	{"swap[in]"		,EXECUTE, 0, "vmstat -n 1 2|tail -1|cut -b37-40"},
	{"swap[out]"		,EXECUTE, 0, "vmstat -n 1 2|tail -1|cut -b41-44"},

	{"system[interrupts]"	,EXECUTE, 0, "vmstat -n 1 2|tail -1|cut -b57-61"},
	{"system[switches]"	,EXECUTE, 0, "vmstat -n 1 2|tail -1|cut -b62-67"},
***************************************/

	{"io[disk_io]"		,DISK_IO,  	0, 0},
	{"io[disk_rio]"		,DISK_RIO, 	0, 0},
	{"io[disk_wio]"		,DISK_WIO, 	0, 0},
	{"io[disk_rblk]"	,DISK_RBLK, 	0, 0},
	{"io[disk_wblk]"	,DISK_WBLK, 	0, 0},

	{"system[procload]"	,PROCLOAD, 	0, 0},
	{"system[procload5]"	,PROCLOAD5, 	0, 0},
	{"system[procload15]"	,PROCLOAD15, 	0, 0},
	{"system[proccount]"	,PROCCOUNT, 	0, 0},
#ifdef HAVE_PROC_LOADAVG
	{"system[procrunning]"	,EXECUTE, 	0, "cat /proc/loadavg|cut -f1 -d'/'|cut -f4 -d' '"},
#endif
	{"system[hostname]"	,0,		EXECUTE_STR, "hostname"},
	{"system[uname]"	,0,		EXECUTE_STR, "uname -a"},
	{"system[uptime]"	,UPTIME,	0, 0},
	{"system[users]"	,EXECUTE, 	0,"who|wc -l"},

	{"ping"			,PING, 		0, 0},
/*	{"tcp_count"		,EXECUTE, 	0, "netstat -tn|grep EST|wc -l"}, */

	{"net[listen_23]"	,TCP_LISTEN, 	0, "0017"},
	{"net[listen_80]"	,TCP_LISTEN, 	0, "0050"},

	{"check_port[*]"	,CHECK_PORT, 	0, "80"},

	{"check_service[*]"	,CHECK_SERVICE, 	0, "ssh,127.0.0.1,22"},
	{"check_service_perf[*]",CHECK_SERVICE_PERF, 	0, "ssh,127.0.0.1,22"},

	{0}
	};

void	add_metric(char *key, void *function,void *function_str,char *parameter )
{

	int i;

	for(i=0;;i++)
	{
		if(commands[i].key == NULL)
		{

			commands[i].key=strdup(key);
			if(parameter == NULL)
			{
				commands[i].parameter=NULL;
			}
			else
			{
				commands[i].parameter=strdup(parameter);
			}
			commands[i].function=function;
			commands[i].function_str=function_str;

			commands=realloc(commands,(i+2)*sizeof(COMMAND));
			commands[i+1].key=NULL;
			break;
		}
	}
}

void	add_user_parameter(char *key,char *command)
{
	int i;

	for(i=0;;i++)
	{
		if( commands[i].key == 0)
		{
			commands[i].key=strdup(key);
			commands[i].function=0;
			commands[i].function_str=&EXECUTE_STR;
			commands[i].parameter=strdup(command);

			commands=realloc(commands,(i+2)*sizeof(COMMAND));
			commands[i+1].key=NULL;

			break;
		}
		/* Replace existing parameters */
		if(strcmp(commands[i].key, key) == 0)
		{
			free(commands[i].key);
			if(commands[i].parameter!=NULL)	free(commands[i].parameter);

			commands[i].key=strdup(key);
			commands[i].function=0;
			commands[i].function_str=&EXECUTE_STR;
			commands[i].parameter=strdup(command);

			break;
		}
	}
}

void	init_metrics()
{
	int 	i;

	commands=malloc(sizeof(COMMAND));
	commands[0].key=NULL;

	for(i=0;agent_commands[i].key!=0;i++)
	{
		add_metric(agent_commands[i].key, agent_commands[i].function, agent_commands[i].function_str, agent_commands[i].parameter);
	}
}

void    escape_string(char *from, char *to, int maxlen)
{
	int     i,ptr;
	char    *f;

	ptr=0;
	f=(char *)strdup(from);
	for(i=0;f[i]!=0;i++)
	{
		if( (f[i]=='\'') || (f[i]=='\\'))
		{
			if(ptr>maxlen-1)        break;
			to[ptr]='\\';
			if(ptr+1>maxlen-1)      break;
			to[ptr+1]=f[i];
			ptr+=2;
		}
		else
		{
			if(ptr>maxlen-1)        break;
			to[ptr]=f[i];
			ptr++;
		}
	}
	free(f);

	to[ptr]=0;
	to[maxlen-1]=0;
}


void	test_parameters(void)
{
	int	i;

	char	c[MAX_STRING_LEN];

	i=0;
	while(0 != commands[i].key)
	{
		process(commands[i].key,c);
		printf("Key: [%s]\tResult: [%s]\n",commands[i].key,c);
		fflush(stdout);
		i++;
	}
}

/* This messy function must be rewritten!
 */
void	process(char *command,char *value)
{
	char	*p;
	double	result=0;
	int	i;
	char	*n,*l,*r;
	int	(*function)() = NULL;
	int	(*function_str)() = NULL;
	char	*parameter = NULL;
	char	*cmd_line_param = NULL;
	char	key[MAX_STRING_LEN];
	char	param[1024];
	char	cmd[1024];
	char	*res2 = NULL;
	int	ret_str=0;
	double	value_double;

	for( p=command+strlen(command)-1; p>command && ( *p=='\r' || *p =='\n' || *p == ' ' ); --p );

	if( (p[1]=='\r') || (p[1]=='\n') ||(p[1]==' '))
	{
		p[1]=0;
	}

	for(i=0;;i++)
	{
		/* End of array? */
		if( commands[i].key == 0)
		{
			function=0;
			break;
		}

		cmd_line_param = NULL;
		strcpy(key, commands[i].key);

		if( (n = strstr(key,"[*]")) != NULL)
		{
			n[0]=0;

			l=strstr(command,"[");	
			r=strstr(command,"]");

			if( (l==NULL)||(r==NULL) )
			{
				continue;
			}

			strncpy( param,l+1, r-l-1);
			param[r-l-1]=0;

			strncpy( cmd, command, l-command);
			cmd[l-command]=0;

			if( strcmp(key, cmd) == 0)
			{
				function=commands[i].function;
				if(function==0)
				{
					function_str=commands[i].function_str;
					/* Command returns string, not double */
					ret_str=1;
				}
#ifdef TEST_PARAMETERS
				parameter=commands[i].parameter;
#else
				cmd_line_param = param;
				parameter=param;
#endif
				break;
			}
		}
		else
		{
			if( strcmp(key,command) == 0)
			{
				function=commands[i].function;
				if(function==0)
				{
					function_str=commands[i].function_str;
					ret_str=1;
				}
				parameter=commands[i].parameter;
				break;
			}	
		}
	}
	
	if(ret_str == 0)
	{
		if(function != 0)
		{
			if(SYSINFO_RET_FAIL == function(command,parameter,&value_double))
			{
				result = NOTSUPPORTED;
			}
		}
		else
		{
			result = NOTSUPPORTED;
		}
	}
	else
	{
		/* Special processing for EXECUTE_STR, it has more parameters */
		if(function_str == EXECUTE_STR)
		{
			i=function_str(command,commands[i].parameter,cmd_line_param,&res2);
		}
		else
		{
			i=function_str(command,parameter,&res2);
		}

		if(i==SYSINFO_RET_FAIL)
		{
			result = NOTSUPPORTED;
		}
/* (int) to avoid compiler's warning */
		else if(i==SYSINFO_RET_TIMEOUT)
		{
			result = TIMEOUT_ERROR;
		}
	}

	if(result==NOTSUPPORTED)
	{
/* New protocol */
/*			sprintf(value,"%f",result);*/
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_NOTSUPPORTED\n");
	}
	else if(result==TIMEOUT_ERROR)
	{
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_ERROR\n");
	}
	else
	{
		if(ret_str==0)
		{
			snprintf(value,MAX_STRING_LEN-1,"%f",value_double);
		}
		else
		{
			snprintf(value,MAX_STRING_LEN-1,"%s",res2);
			free(res2);
		}
	}

}

/* MD5 sum calculation */

int	MD5SUM(const char *cmd, const char *filename, char **value)
{
	int	fd;
	int	i,nr;
	struct stat	buf_stat;

        md5_state_t state;
	u_char	buf[16 * 1024];

	unsigned char	hashText[MD5_DIGEST_SIZE*2+1];
	unsigned char	hash[MD5_DIGEST_SIZE];

	if(stat(filename,&buf_stat) != 0)
	{
		/* Cannot stat() file */
		return	SYSINFO_RET_FAIL;
	}

	if(buf_stat.st_size > 64*1024*1024)
	{
		/* Will not calculate MD5 for files larger than 64M */
		return	SYSINFO_RET_FAIL;
	}

	fd=open(filename,O_RDONLY);
	if(fd == -1)
	{
		return	SYSINFO_RET_FAIL;
	}

        md5_init(&state);
	while ((nr = read(fd, buf, sizeof(buf))) > 0)
	{
        	md5_append(&state,(const md5_byte_t *)buf,nr);
	}
        md5_finish(&state,(md5_byte_t *)hash);

	close(fd);

/* Convert MD5 hash to text form */
	for(i=0;i<MD5_DIGEST_SIZE;i++)
		sprintf(&hashText[i<<1],"%02x",hash[i]);

	*value=strdup(hashText);

	return SYSINFO_RET_OK;
}

/* Code for cksum is based on code from cksum.c */

static u_long crctab[] = {
	0x0,
	0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6,
	0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
	0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f,
	0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a,
	0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58,
	0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033,
	0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
	0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4,
	0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5,
	0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
	0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c,
	0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1,
	0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b,
	0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698,
	0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
	0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f,
	0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80,
	0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
	0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629,
	0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c,
	0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e,
	0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65,
	0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
	0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2,
	0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74,
	0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
	0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a,
	0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087,
	0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d,
	0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce,
	0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
	0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09,
	0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf,
	0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

/*
 * Compute a POSIX 1003.2 checksum.  These routines have been broken out so
 * that other programs can use them.  The first routine, crc(), takes a file
 * descriptor to read from and locations to store the crc and the number of
 * bytes read.  The second routine, crc_buf(), takes a buffer and a length,
 * and a location to store the crc.  Both routines return 0 on success and 1
 * on failure.  Errno is set on failure.
 */

int	CKSUM(const char *cmd, const char *filename,double  *value)
{
	register u_char *p;
	register int nr;
/*	AV Crashed under 64 platforms. Must be 32 bit! */
/*	register u_long crc, len;*/
	register uint32_t crc, len;
	u_char buf[16 * 1024];
	u_long cval, clen;
	int	fd;

	fd=open(filename,O_RDONLY);
	if(fd == -1)
	{
		return	SYSINFO_RET_FAIL;
	}

#define	COMPUTE(var, ch)	(var) = (var) << 8 ^ crctab[(var) >> 24 ^ (ch)]

	crc = len = 0;
	while ((nr = read(fd, buf, sizeof(buf))) > 0)
	{
		for( len += nr, p = buf; nr--; ++p)
		{
			COMPUTE(crc, *p);
		}
	}
	close(fd);
	
	if (nr < 0)
	{
		return	SYSINFO_RET_FAIL;
	}

	clen = len;

	/* Include the length of the file. */
	for (; len != 0; len >>= 8) {
		COMPUTE(crc, len & 0xff);
	}

	cval = ~crc;

	*value=(double)cval;

	return	SYSINFO_RET_OK;
}

int
crc_buf2(p, clen, cval)
	register u_char *p;
	u_long clen;
	u_long *cval;
{
	register u_long crc, len;

	crc = 0;
	for (len = clen; len--; ++p)
		COMPUTE(crc, *p);

	/* Include the length of the file. */
	for (len = clen; len != 0; len >>= 8)
		COMPUTE(crc, len & 0xff);

	*cval = ~crc;
	return (0);
}

/* Solaris. */
#ifndef HAVE_SYSINFO_FREESWAP
#ifdef HAVE_SYS_SWAP_SWAPTABLE
void get_swapinfo(double *total, double *fr)
{
	register int cnt, i, page_size;
/* Support for >2Gb */
/*	register int t, f;*/
	double	t, f;
	struct swaptable *swt;
	struct swapent *ste;
	static char path[256];

	/* get total number of swap entries */
	cnt = swapctl(SC_GETNSWP, 0);

	/* allocate enough space to hold count + n swapents */
	swt = (struct swaptable *)malloc(sizeof(int) +
		cnt * sizeof(struct swapent));

	if (swt == NULL)
	{
		*total = 0;
		*fr = 0;
		return;
	}
	swt->swt_n = cnt;

/* fill in ste_path pointers: we don't care about the paths, so we
point them all to the same buffer */
	ste = &(swt->swt_ent[0]);
	i = cnt;
	while (--i >= 0)
	{
		ste++->ste_path = path;
	}

	/* grab all swap info */
	swapctl(SC_LIST, swt);

	/* walk thru the structs and sum up the fields */
	t = f = 0;
	ste = &(swt->swt_ent[0]);
	i = cnt;
	while (--i >= 0)
	{
		/* dont count slots being deleted */
		if (!(ste->ste_flags & ST_INDEL) &&
		!(ste->ste_flags & ST_DOINGDEL))
		{
			t += ste->ste_pages;
			f += ste->ste_free;
		}
		ste++;
	}

	page_size=getpagesize();

	/* fill in the results */
	*total = page_size*t;
	*fr = page_size*f;
	free(swt);
}
#endif
#endif

int	FILESIZE(const char *cmd, const char *filename,double  *value)
{
	struct stat	buf;

	if(stat(filename,&buf) == 0)
	{
		*value=(double)buf.st_size;
		return SYSINFO_RET_OK;
	}
	return	SYSINFO_RET_FAIL;
}

int	SENSOR_TEMP1(const char *cmd, const char *param,double  *value)
{
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];
	char	line[MAX_STRING_LEN];
	double	d1,d2,d3;

	FILE	*f;

	dir=opendir("/proc/sys/dev/sensors");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/sys/dev/sensors/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/temp1",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			f=fopen(filename,"r");
			if(f==NULL)
			{
				continue;
			}
			fgets(line,MAX_STRING_LEN,f);
			fclose(f);

			if(sscanf(line,"%lf\t%lf\t%lf\n",&d1, &d2, &d3) == 3)
			{
				closedir(dir);
				*value=d3;
				return  SYSINFO_RET_OK;
			}
			else
			{
				closedir(dir);
				return  SYSINFO_RET_FAIL;
			}
		}
	}
	closedir(dir);
	return	SYSINFO_RET_FAIL;
}

int	SENSOR_TEMP2(const char *cmd, const char *param,double  *value)
{
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];
	char	line[MAX_STRING_LEN];
	double	d1,d2,d3;

	FILE	*f;

	dir=opendir("/proc/sys/dev/sensors");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/sys/dev/sensors/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/temp2",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			f=fopen(filename,"r");
			if(f==NULL)
			{
				continue;
			}
			fgets(line,MAX_STRING_LEN,f);
			fclose(f);

			if(sscanf(line,"%lf\t%lf\t%lf\n",&d1, &d2, &d3) == 3)
			{
				closedir(dir);
				*value=d3;
				return  SYSINFO_RET_OK;
			}
			else
			{
				closedir(dir);
				return  SYSINFO_RET_FAIL;
			}
		}
	}
	closedir(dir);
	return	SYSINFO_RET_FAIL;
}

int	SENSOR_TEMP3(const char *cmd, const char *param,double  *value)
{
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];
	char	line[MAX_STRING_LEN];
	double	d1,d2,d3;

	FILE	*f;

	dir=opendir("/proc/sys/dev/sensors");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/sys/dev/sensors/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/temp3",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			f=fopen(filename,"r");
			if(f==NULL)
			{
				continue;
			}
			fgets(line,MAX_STRING_LEN,f);
			fclose(f);

			if(sscanf(line,"%lf\t%lf\t%lf\n",&d1, &d2, &d3) == 3)
			{
				closedir(dir);
				*value=d3;
				return  SYSINFO_RET_OK;
			}
			else
			{
				closedir(dir);
				return  SYSINFO_RET_FAIL;
			}
		}
	}
	closedir(dir);
	return	SYSINFO_RET_FAIL;
}

int	PROCCNT(const char *cmd, const char *procname,double  *value)
{
#ifdef	HAVE_PROC_0_PSINFO
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];

	int	fd;
/* In the correct procfs.h, the structure name is psinfo_t */
	psinfo_t psinfo;

	int	proccount=0;

	dir=opendir("/proc");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/psinfo",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			fd = open (filename, O_RDONLY);
			if (fd != -1)
			{
				if (read (fd, &psinfo, sizeof(psinfo)) == -1)
				{
					closedir(dir);
					return SYSINFO_RET_FAIL;
				}
				else
				{
					if(strcmp(procname,psinfo.pr_fname)==0)
					{
						proccount++;
					}
				}
				close (fd);
			}
			else
			{
				continue;
			}
		}
	}
	closedir(dir);
	*value=(double)proccount;
	return	SYSINFO_RET_OK;
#else
#ifdef	HAVE_PROC_1_STATUS
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];
	char	line[MAX_STRING_LEN];
	char	name1[MAX_STRING_LEN];
	char	name2[MAX_STRING_LEN];

	FILE	*f;

	int	proccount=0;

	dir=opendir("/proc");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/status",MAX_STRING_LEN);

/* Self is a symbolic link. It leads to incorrect results for proc_cnt[allview_agentd] */
/* Better approach: check if /proc/x/ is symbolic link */
		if(strncmp(entries->d_name,"self",MAX_STRING_LEN) == 0)
		{
			continue;
		}

		if(stat(filename,&buf)==0)
		{
			f=fopen(filename,"r");
			if(f==NULL)
			{
				continue;
			}
			fgets(line,MAX_STRING_LEN,f);
			fclose(f);

			if(sscanf(line,"%s\t%s\n",name1,name2)==2)
                        {
				/* For Linux only */
                                if(strcmp(name1,"Name:") == 0)
                                {
                                        if(strcmp(procname,name2)==0)
                                        {
                                                proccount++;
                                        }
                                }
				/* Assuming that this is FreeBSD. First field is process name */
				else if(strcmp(procname,name1)==0)
				{
					proccount++;
				}
/*                                else
                                {
                                        closedir(dir);
                                        return  SYSINFO_RET_FAIL;
                                }*/
                        }
                        else
                        {
                                closedir(dir);
                                return  SYSINFO_RET_FAIL;
                        }
		}
	}
	closedir(dir);
	*value=(double)proccount;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
}

int	PROCCNTS(const char *cmd, const char *procname,double  *value)
{
#ifdef	HAVE_PROC_0_PSINFO
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];

	int	fd;
/* In the correct procfs.h, the structure name is psinfo_t */
	psinfo_t psinfo;

	int	proccount=0;

	dir=opendir("/proc");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/psinfo",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			fd = open (filename, O_RDONLY);
			if (fd != -1)
			{
				if (read (fd, &psinfo, sizeof(psinfo)) == -1)
				{
					closedir(dir);
					return SYSINFO_RET_FAIL;
				}
				else
				{
					if(strcmp(procname,psinfo.pr_fname)==0)
					{
						proccount++;
					}
				}
				close (fd);
			}
			else
			{
				continue;
			}
		}
	}
	closedir(dir);
	*value=(double)proccount;
	return	SYSINFO_RET_OK;
#else
#ifdef	HAVE_PROC_1_STATUS
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];
	char	line[MAX_STRING_LEN];
	char	name1[MAX_STRING_LEN];
	char	name2[MAX_STRING_LEN];

	FILE	*f;

	int	proccount=0;

	dir=opendir("/proc");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/status",MAX_STRING_LEN);

/* Self is a symbolic link. It leads to incorrect results for proc_cnt[allview_agentd] */
/* Better approach: check if /proc/x/ is symbolic link */
		if(strncmp(entries->d_name,"self",MAX_STRING_LEN) == 0)
		{
			continue;
		}

		if(stat(filename,&buf)==0)
		{
			f=fopen(filename,"r");
			if(f==NULL)
			{
				continue;
			}
			fgets(line,MAX_STRING_LEN,f);
			fclose(f);

			if(sscanf(line,"%s\t%s\n",name1,name2)==2)
                        {
				/* For Linux only */
                                if(strcmp(name1,"Name:") == 0)
                                {
                                        if(strcmp(procname,name2)==0)
                                        {
                                                proccount++;
                                        }
                                }
				/* Assuming that this is FreeBSD. First field is process name */
				else if(strcmp(procname,name1)==0)
				{
					proccount++;
				}
/*                                else
                                {
                                        closedir(dir);
                                        return  SYSINFO_RET_FAIL;
                                }*/
                        }
                        else
                        {
                                closedir(dir);
                                return  SYSINFO_RET_FAIL;
                        }
		}
	}
	closedir(dir);
	*value=(double)proccount;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
}

int	get_stat(const char *key, double *value)
{
	FILE	*f;
	char	line[MAX_STRING_LEN];
	char	name1[MAX_STRING_LEN];
	char	name2[MAX_STRING_LEN];

	f=fopen("/tmp/allview_agentd.tmp","r");
	if(f==NULL)
	{
		return SYSINFO_RET_FAIL;
	}
	while(fgets(line,MAX_STRING_LEN,f))
	{
		if(sscanf(line,"%s %s\n",name1,name2)==2)
		{
			if(strcmp(name1,key) == 0)
			{
				fclose(f);
				*value=atof(name2);
				return SYSINFO_RET_OK;
			}
		}

	}
	fclose(f);
	return SYSINFO_RET_FAIL;
}

int	DISKREADOPS1(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_read_ops1[%s]",device);

	return	get_stat(key,value);
}

int	DISKREADOPS5(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_read_ops5[%s]",device);

	return	get_stat(key,value);
}

int	DISKREADOPS15(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_read_ops15[%s]",device);

	return	get_stat(key,value);
}

int	DISKREADBLKS1(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_read_blks1[%s]",device);

	return	get_stat(key,value);
}

int	DISKREADBLKS5(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_read_blks5[%s]",device);

	return	get_stat(key,value);
}

int	DISKREADBLKS15(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_read_blks15[%s]",device);

	return	get_stat(key,value);
}

int	DISKWRITEOPS1(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_write_ops1[%s]",device);

	return	get_stat(key,value);
}

int	DISKWRITEOPS5(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_write_ops5[%s]",device);

	return	get_stat(key,value);
}

int	DISKWRITEOPS15(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_write_ops15[%s]",device);

	return	get_stat(key,value);
}

int	DISKWRITEBLKS1(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_write_blks1[%s]",device);

	return	get_stat(key,value);
}

int	DISKWRITEBLKS5(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_write_blks5[%s]",device);

	return	get_stat(key,value);
}

int	DISKWRITEBLKS15(const char *cmd, const char *device,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"disk_write_blks15[%s]",device);

	return	get_stat(key,value);
}

int	CPUIDLE1(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[idle1]",value);
}

int	CPUIDLE5(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[idle5]",value);
}

int	CPUIDLE15(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[idle15]",value);
}

int	CPUNICE1(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[nice1]",value);
}

int	CPUNICE5(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[nice5]",value);
}
int	CPUNICE15(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[nice15]",value);
}

int	CPUUSER1(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[user1]",value);
}

int	CPUUSER5(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[user5]",value);
}

int	CPUUSER15(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[user15]",value);
}

int	CPUSYSTEM1(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[system1]",value);
}

int	CPUSYSTEM5(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[system5]",value);
}

int	CPUSYSTEM15(const char *cmd, const char *param,double  *value)
{
	return	get_stat("cpu[system15]",value);
}

int	NETLOADIN1(const char *cmd, const char *parameter,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"netloadin1[%s]",parameter);

	return	get_stat(key,value);
}

int	NETLOADIN5(const char *cmd, const char *parameter,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"netloadin5[%s]",parameter);

	return	get_stat(key,value);
}

int	NETLOADIN15(const char *cmd, const char *parameter,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"netloadin15[%s]",parameter);

	return	get_stat(key,value);
}

int	NETLOADOUT1(const char *cmd, const char *parameter,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"netloadout1[%s]",parameter);

	return	get_stat(key,value);
}

int	NETLOADOUT5(const char *cmd, const char *parameter,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"netloadout5[%s]",parameter);

	return	get_stat(key,value);
}

int	NETLOADOUT15(const char *cmd, const char *parameter,double  *value)
{
	char	key[MAX_STRING_LEN];

	snprintf(key,sizeof(key)-1,"netloadout15[%s]",parameter);

	return	get_stat(key,value);
}


int	INODEFREE(const char *cmd, const char *mountPoint,double  *value)
{
#ifdef HAVE_SYS_STATVFS_H
	struct statvfs   s;

	if ( statvfs( (char *)mountPoint, &s) != 0 )
	{
		return  SYSINFO_RET_FAIL;
	}

	*value=s.f_favail;
	return SYSINFO_RET_OK;
#else
	struct statfs   s;
	long            blocks_used;
	long            blocks_percent_used;

	if ( statfs( (char *)mountPoint, &s) != 0 ) 
	{
		return	SYSINFO_RET_FAIL;
	}
        
	if ( s.f_blocks > 0 ) {
		blocks_used = s.f_blocks - s.f_bfree;
		blocks_percent_used = (long)
		(blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);

/*		printf(
		"%7.0f %7.0f  %7.0f  %5ld%%   %s\n"
		,s.f_blocks * (s.f_bsize / 1024.0)
		,(s.f_blocks - s.f_bfree)  * (s.f_bsize / 1024.0)
		,s.f_bavail * (s.f_bsize / 1024.0)
		,blocks_percent_used
		,mountPoint);
*/
		*value=s.f_ffree;
		return SYSINFO_RET_OK;

	}
	return	SYSINFO_RET_FAIL;
#endif
}

int	INODETOTAL(const char *cmd, const char *mountPoint,double  *value)
{
#ifdef HAVE_SYS_STATVFS_H
	struct statvfs   s;

	if ( statvfs( (char *)mountPoint, &s) != 0 )
	{
		return  SYSINFO_RET_FAIL;
	}

	*value=s.f_files;
	return SYSINFO_RET_OK;
#else
	struct statfs   s;
	long            blocks_used;
	long            blocks_percent_used;

	if ( statfs( (char *)mountPoint, &s) != 0 ) 
	{
		return	SYSINFO_RET_FAIL;
	}
        
	if ( s.f_blocks > 0 ) {
		blocks_used = s.f_blocks - s.f_bfree;
		blocks_percent_used = (long)
		(blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);

/*		printf(
		"%7.0f %7.0f  %7.0f  %5ld%%   %s\n"
		,s.f_blocks * (s.f_bsize / 1024.0)
		,(s.f_blocks - s.f_bfree)  * (s.f_bsize / 1024.0)
		,s.f_bavail * (s.f_bsize / 1024.0)
		,blocks_percent_used
		,mountPoint);
*/
		*value=s.f_files;
		return SYSINFO_RET_OK;

	}
	return	SYSINFO_RET_FAIL;
#endif
}

int	INODEFREE_PERC(const char *cmd, const char *mountPoint,double  *value)
{
	double	total;
	double	free;

	if(SYSINFO_RET_OK != INODETOTAL(cmd, mountPoint, &total))
	{
		return SYSINFO_RET_FAIL;
	}

	if(SYSINFO_RET_OK != INODEFREE(cmd, mountPoint, &free))
	{
		return SYSINFO_RET_FAIL;
	}

	if(total == 0)
	{
		return SYSINFO_RET_FAIL;
	}

	*value = 100*free/total;
	return SYSINFO_RET_OK;
}

int	DISKUSED_PERC(const char *cmd, const char *mountPoint,double  *value)
{
	double	total;
	double	used;

	if(SYSINFO_RET_OK != DISKTOTAL(cmd, mountPoint, &total))
	{
		return SYSINFO_RET_FAIL;
	}

	if(SYSINFO_RET_OK != DISKUSED(cmd, mountPoint, &used))
	{
		return SYSINFO_RET_FAIL;
	}

	if(total == 0)
	{
		return SYSINFO_RET_FAIL;
	}

	*value = 100*used/total;
	return SYSINFO_RET_OK;
}

int	DISKFREE_PERC(const char *cmd, const char *mountPoint,double  *value)
{
	double	total;
	double	free;

	if(SYSINFO_RET_OK != DISKTOTAL(cmd, mountPoint, &total))
	{
		return SYSINFO_RET_FAIL;
	}

	if(SYSINFO_RET_OK != DISKFREE(cmd, mountPoint, &free))
	{
		return SYSINFO_RET_FAIL;
	}

	if(total == 0)
	{
		return SYSINFO_RET_FAIL;
	}

	*value = 100*free/total;
	return SYSINFO_RET_OK;
}

int	DISKFREE(const char *cmd, const char *mountPoint,double  *value)
{
#ifdef HAVE_SYS_STATVFS_H
	struct statvfs   s;

	if ( statvfs( (char *)mountPoint, &s) != 0 )
	{
		return  SYSINFO_RET_FAIL;
	}

/*	return  s.f_bavail * (s.f_bsize / 1024.0);*/
	*value=s.f_bavail * (s.f_frsize / 1024.0);
	return SYSINFO_RET_OK;
#else
	struct statfs   s;
	long            blocks_used;
	long            blocks_percent_used;

	if ( statfs( (char *)mountPoint, &s) != 0 )
	{
		return	SYSINFO_RET_FAIL;
	}
        
	if ( s.f_blocks > 0 ) {
		blocks_used = s.f_blocks - s.f_bfree;
		blocks_percent_used = (long)
		(blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);

/*		printf(
		"%7.0f %7.0f  %7.0f  %5ld%%   %s\n"
		,s.f_blocks * (s.f_bsize / 1024.0)
		,(s.f_blocks - s.f_bfree)  * (s.f_bsize / 1024.0)
		,s.f_bavail * (s.f_bsize / 1024.0)
		,blocks_percent_used
		,mountPoint);
*/
		*value=s.f_bavail * (s.f_bsize / 1024.0);
		return SYSINFO_RET_OK;

	}

	return	SYSINFO_RET_FAIL;
#endif
}

int	DISKUSED(const char *cmd, const char *mountPoint,double  *value)
{
#ifdef HAVE_SYS_STATVFS_H
	struct statvfs   s;

	if ( statvfs( (char *)mountPoint, &s) != 0 )
	{
		return  SYSINFO_RET_FAIL;
	}

/*	return  (s.f_blocks-s.f_bavail) * (s.f_bsize / 1024.0);*/
	*value=(s.f_blocks-s.f_bavail) * (s.f_frsize / 1024.0);
	return SYSINFO_RET_OK;
#else
	struct statfs   s;
	long            blocks_used;
	long            blocks_percent_used;

	if ( statfs( (char *)mountPoint, &s) != 0 )
	{
		return	SYSINFO_RET_FAIL;
	}
        
	if ( s.f_blocks > 0 ) {
		blocks_used = s.f_blocks - s.f_bfree;
		blocks_percent_used = (long)
		(blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);

/*		printf(
		"%7.0f %7.0f  %7.0f  %5ld%%   %s\n"
		,s.f_blocks * (s.f_bsize / 1024.0)
		,(s.f_blocks - s.f_bfree)  * (s.f_bsize / 1024.0)
		,s.f_bavail * (s.f_bsize / 1024.0)
		,blocks_percent_used
		,mountPoint);
*/
		*value=blocks_used * (s.f_bsize / 1024.0);
		return SYSINFO_RET_OK;

	}

	return	SYSINFO_RET_FAIL;
#endif
}

int	DISKTOTAL(const char *cmd, const char *mountPoint,double  *value)
{
#ifdef HAVE_SYS_STATVFS_H
	struct statvfs   s;

	if ( statvfs( (char *)mountPoint, &s) != 0 )
	{
		return  SYSINFO_RET_FAIL;
	}

/*	return  s.f_blocks * (s.f_bsize / 1024.0);*/
	*value= s.f_blocks * (s.f_frsize / 1024.0);
	return SYSINFO_RET_OK;
#else
	struct statfs   s;
	long            blocks_used;
	long            blocks_percent_used;

	if ( statfs( (char *)mountPoint, &s) != 0 )
	{
		return	SYSINFO_RET_FAIL;
	}
        
	if ( s.f_blocks > 0 ) {
		blocks_used = s.f_blocks - s.f_bfree;
		blocks_percent_used = (long)
		(blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);

/*		printf(
		"%7.0f %7.0f  %7.0f  %5ld%%   %s\n"
		,s.f_blocks * (s.f_bsize / 1024.0)
		,(s.f_blocks - s.f_bfree)  * (s.f_bsize / 1024.0)
		,s.f_bavail * (s.f_bsize / 1024.0)
		,blocks_percent_used
		,mountPoint);
*/
		*value=s.f_blocks * (s.f_bsize / 1024.0);
		return SYSINFO_RET_OK;

	}

	return	SYSINFO_RET_FAIL;
#endif
}

int	TCP_LISTEN(const char *cmd, const char *porthex,double  *value)
{
#ifdef HAVE_PROC
	FILE	*f;
	char	c[MAX_STRING_LEN];

	char	pattern[MAX_STRING_LEN]="0050 00000000:0000 0A";

	strscpy(pattern,porthex);
	strncat(pattern," 00000000:0000 0A", MAX_STRING_LEN);

	f=fopen("/proc/net/tcp","r");
	if(NULL == f)
	{
		return	SYSINFO_RET_FAIL;
	}

	while (NULL!=fgets(c,MAX_STRING_LEN,f))
	{
		if(NULL != strstr(c,pattern))
		{
			fclose(f);
			*value=1;
			return SYSINFO_RET_OK;
		}
	}
	fclose(f);

	*value=0;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
}

#ifdef	HAVE_PROC
int	getPROC(char *file,int lineno,int fieldno, double *value)
{
	FILE	*f;
	char	*t;
	char	c[MAX_STRING_LEN];
	double	result;
	int	i;

	f=fopen(file,"r");
	if(NULL == f)
	{
		return	SYSINFO_RET_FAIL;
	}
	for(i=1;i<=lineno;i++)
	{	
		fgets(c,MAX_STRING_LEN,f);
	}
	t=(char *)strtok(c," ");
	for(i=2;i<=fieldno;i++)
	{
		t=(char *)strtok(NULL," ");
	}
	fclose(f);

	sscanf(t, "%lf", &result );

	*value=result;
	return SYSINFO_RET_OK;
}
#endif

int	CACHEDMEM(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_PROC
/* Get CACHED memory in bytes */
/*	return getPROC("/proc/meminfo",8,2);*/
/* It does not work for both 2.4 and 2.6 */
/*	return getPROC("/proc/meminfo",2,7);*/
	FILE	*f;
	char	*t;
	char	c[MAX_STRING_LEN];
	double	result = SYSINFO_RET_FAIL;

	f=fopen("/proc/meminfo","r");
	if(NULL == f)
	{
		return	SYSINFO_RET_FAIL;
	}
	while(NULL!=fgets(c,MAX_STRING_LEN,f))
	{
		if(strncmp(c,"Cached:",7) == 0)
		{
			t=(char *)strtok(c," ");
			t=(char *)strtok(NULL," ");
			sscanf(t, "%lf", &result );
			break;
		}
	}
	fclose(f);

	*value=result;
	return SYSINFO_RET_OK;
#else
	return SYSINFO_RET_FAIL;
#endif
}

int	BUFFERSMEM(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_SYSINFO_BUFFERRAM
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
#ifdef HAVE_SYSINFO_MEM_UNIT
		*value=(double)info.bufferram * (double)info.mem_unit;
#else
		*value=(double)info.bufferram;
#endif
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	SHAREDMEM(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_SYSINFO_SHAREDRAM
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
#ifdef HAVE_SYSINFO_MEM_UNIT
		*value=(double)info.sharedram * (double)info.mem_unit;
#else
		*value=(double)info.sharedram;
#endif
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
#else
#ifdef HAVE_SYS_VMMETER_VMTOTAL
	int mib[2],len;
	struct vmtotal v;

	len=sizeof(struct vmtotal);
	mib[0]=CTL_VM;
	mib[1]=VM_METER;

	sysctl(mib,2,&v,&len,NULL,0);

	*value=(double)(v.t_armshr<<2);
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
}

int	TOTALMEM(const char *cmd, const char *parameter,double  *value)
{
/* Solaris */
#ifdef HAVE_UNISTD_SYSCONF
	*value=(double)sysconf(_SC_PHYS_PAGES)*sysconf(_SC_PAGESIZE);
	return SYSINFO_RET_OK;
#else
#ifdef HAVE_SYS_PSTAT_H
	struct	pst_static pst;
	long	page;

	if(pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
	{
		return SYSINFO_RET_FAIL;
	}
	else
	{
		/* Get page size */	
		page = pst.page_size;
		/* Total physical memory in bytes */	
		*value=page*pst.physical_memory;
		return SYSINFO_RET_OK;
	}
#else
#ifdef HAVE_SYSINFO_TOTALRAM
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
#ifdef HAVE_SYSINFO_MEM_UNIT
		*value=(double)info.totalram * (double)info.mem_unit;
#else
		*value=(double)info.totalram;
#endif
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
#else
#ifdef HAVE_SYS_VMMETER_VMTOTAL
	int mib[2],len;
	struct vmtotal v;

	len=sizeof(struct vmtotal);
	mib[0]=CTL_VM;
	mib[1]=VM_METER;

	sysctl(mib,2,&v,&len,NULL,0);

	*value=(double)(v.t_rm<<2);
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
#endif
}

int	FREEMEM(const char *cmd, const char *parameter,double  *value)
{
/* Solaris */
#ifdef HAVE_UNISTD_SYSCONF
	*value=(double)sysconf(_SC_AVPHYS_PAGES)*sysconf(_SC_PAGESIZE);
	return SYSINFO_RET_OK;
#else
#ifdef HAVE_SYS_PSTAT_H
	struct	pst_static pst;
	struct	pst_dynamic dyn;
	long	page;

	if(pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1)
	{
		return SYSINFO_RET_FAIL;
	}
	else
	{
		/* Get page size */	
		page = pst.page_size;
/*		return pst.physical_memory;*/

		if (pstat_getdynamic(&dyn, sizeof(dyn), 1, 0) == -1)
		{
			return SYSINFO_RET_FAIL;
		}
		else
		{
/*		cout<<"total virtual memory allocated is " << dyn.psd_vm << "
		pages, " << dyn.psd_vm * page << " bytes" << endl;
		cout<<"active virtual memory is " << dyn.psd_avm <<" pages, " <<
		dyn.psd_avm * page << " bytes" << endl;
		cout<<"total real memory is " << dyn.psd_rm << " pages, " <<
		dyn.psd_rm * page << " bytes" << endl;
		cout<<"active real memory is " << dyn.psd_arm << " pages, " <<
		dyn.psd_arm * page << " bytes" << endl;
		cout<<"free memory is " << dyn.psd_free << " pages, " <<
*/
		/* Free memory in bytes */

			*value=(double)dyn.psd_free * page;
			return SYSINFO_RET_OK;
		}
	}
#else
#ifdef HAVE_SYSINFO_FREERAM
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
#ifdef HAVE_SYSINFO_MEM_UNIT
		*value=(double)info.freeram * (double)info.mem_unit;
#else
		*value=(double)info.freeram;
#endif
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
#else
#ifdef HAVE_SYS_VMMETER_VMTOTAL
	int mib[2],len;
	struct vmtotal v;

	len=sizeof(struct vmtotal);
	mib[0]=CTL_VM;
	mib[1]=VM_METER;

	sysctl(mib,2,&v,&len,NULL,0);

	*value=(double)(v.t_free<<2);
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
#endif
}

int	KERNEL_MAXFILES(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_FUNCTION_SYSCTL_KERN_MAXFILES
	int	mib[2],len;
	int	maxfiles;

	mib[0]=CTL_KERN;
	mib[1]=KERN_MAXFILES;

	len=sizeof(maxfiles);

	if(sysctl(mib,2,&maxfiles,(size_t *)&len,NULL,0) != 0)
	{
		return	SYSINFO_RET_FAIL;
	}

	*value=(double)(maxfiles);
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	KERNEL_MAXPROC(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_FUNCTION_SYSCTL_KERN_MAXPROC
	int	mib[2],len;
	int	maxproc;

	mib[0]=CTL_KERN;
	mib[1]=KERN_MAXPROC;

	len=sizeof(maxproc);

	if(sysctl(mib,2,&maxproc,(size_t *)&len,NULL,0) != 0)
	{
		return	SYSINFO_RET_FAIL;
/*		printf("Errno [%m]");*/
	}

	*value=(double)(maxproc);
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	UPTIME(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_SYSINFO_UPTIME
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
		*value=(double)info.uptime;
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
#else
#ifdef HAVE_FUNCTION_SYSCTL_KERN_BOOTTIME
	int	mib[2],len;
	struct timeval	uptime;
	int	now;

	mib[0]=CTL_KERN;
	mib[1]=KERN_BOOTTIME;

	len=sizeof(uptime);

	if(sysctl(mib,2,&uptime,(size_t *)&len,NULL,0) != 0)
	{
		return	SYSINFO_RET_FAIL;
/*		printf("Errno [%m]\n");*/
	}

	now=time(NULL);

	*value=(double)(now-uptime.tv_sec);
	return SYSINFO_RET_OK;
#else
/* Solaris */
#ifdef HAVE_KSTAT_H
	kstat_ctl_t   *kc;
	kstat_t       *kp;
	kstat_named_t *kn;

	long          hz;
	long          secs;

	hz = sysconf(_SC_CLK_TCK);

	/* open kstat */
	kc = kstat_open();
	if (0 == kc)
	{
		return SYSINFO_RET_FAIL;
	}

	/* read uptime counter */
	kp = kstat_lookup(kc, "unix", 0, "system_misc");
	if (0 == kp)
	{
		kstat_close(kc);
		return SYSINFO_RET_FAIL;
	}

	if(-1 == kstat_read(kc, kp, 0))
	{
		kstat_close(kc);
		return SYSINFO_RET_FAIL;
	}
	kn = (kstat_named_t*)kstat_data_lookup(kp, "clk_intr");
	secs = kn->value.ul / hz;

	/* close kstat */
	kstat_close(kc);
	*value=(double)secs;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
}

int	PING(const char *cmd, const char *parameter,double  *value)
{
	*value=1;
	return SYSINFO_RET_OK;
}

int	PROCLOAD(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_GETLOADAVG
	double	load[3];

	if(getloadavg(load, 3))
	{
		*value=load[0];
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;	
	}
#else
#ifdef	HAVE_SYS_PSTAT_H
	struct	pst_dynamic dyn;

	if (pstat_getdynamic(&dyn, sizeof(dyn), 1, 0) == -1)
	{
		return SYSINFO_RET_FAIL;
	}
	else
	{
		*value=(double)dyn.psd_avg_1_min;
		return SYSINFO_RET_OK;
	}
#else
#ifdef HAVE_PROC_LOADAVG
	return	getPROC("/proc/loadavg",1,1,value);
#else
#ifdef HAVE_KSTAT_H
	static kstat_ctl_t *kc = NULL;
	kstat_t *ks;
	kstat_named_t *kn;

	if (!kc && !(kc = kstat_open()))
	{
		return SYSINFO_RET_FAIL;
	}
	if (!(ks = kstat_lookup(kc, "unix", 0, "system_misc")) ||
		kstat_read(kc, ks, 0) == -1 ||
		!(kn = kstat_data_lookup(ks,"avenrun_1min")))
	{
		return SYSINFO_RET_FAIL;
	}
        *value=(double)kn->value.ul/256.0;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
#endif
}

int	PROCLOAD5(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_GETLOADAVG
	double	load[3];

	if(getloadavg(load, 3))
	{
		*value=load[1];
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;	
	}
#else
#ifdef	HAVE_SYS_PSTAT_H
	struct	pst_dynamic dyn;

	if (pstat_getdynamic(&dyn, sizeof(dyn), 1, 0) == -1)
	{
		return SYSINFO_RET_FAIL;
	}
	else
	{
		*value=(double)dyn.psd_avg_5_min;
		return SYSINFO_RET_OK;
	}
#else
#ifdef	HAVE_PROC_LOADAVG
	return	getPROC("/proc/loadavg",1,2,value);
#else
#ifdef HAVE_KSTAT_H
	static kstat_ctl_t *kc = NULL;
	kstat_t *ks;
	kstat_named_t *kn;

	if (!kc && !(kc = kstat_open()))
	{
		return SYSINFO_RET_FAIL;
	}
	if (!(ks = kstat_lookup(kc, "unix", 0, "system_misc")) ||
		kstat_read(kc, ks, 0) == -1 ||
		!(kn = kstat_data_lookup(ks,"avenrun_5min")))
	{
		return SYSINFO_RET_FAIL;
	}
        *value=(double)kn->value.ul/256.0;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
#endif
}

int	PROCLOAD15(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_GETLOADAVG
	double	load[3];

	if(getloadavg(load, 3))
	{
		*value=load[2];	
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;	
	}
#else
#ifdef	HAVE_SYS_PSTAT_H
	struct	pst_dynamic dyn;

	if (pstat_getdynamic(&dyn, sizeof(dyn), 1, 0) == -1)
	{
		return SYSINFO_RET_FAIL;
	}
	else
	{
		*value=(double)dyn.psd_avg_15_min;
		return SYSINFO_RET_OK;
	}
#else
#ifdef	HAVE_PROC_LOADAVG
	return	getPROC("/proc/loadavg",1,3,value);
#else
#ifdef HAVE_KSTAT_H
	static kstat_ctl_t *kc = NULL;
	kstat_t *ks;
	kstat_named_t *kn;

	if (!kc && !(kc = kstat_open()))
	{
		return SYSINFO_RET_FAIL;
	}
	if (!(ks = kstat_lookup(kc, "unix", 0, "system_misc")) ||
		kstat_read(kc, ks, 0) == -1 ||
		!(kn = kstat_data_lookup(ks,"avenrun_15min")))
	{
		return SYSINFO_RET_FAIL;
	}
        *value=(double)kn->value.ul/256.0;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
#endif
}

int	SWAPFREE(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_SYSINFO_FREESWAP
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
#ifdef HAVE_SYSINFO_MEM_UNIT
		*value=(double)info.freeswap * (double)info.mem_unit;
#else
		*value=(double)info.freeswap;
#endif
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
/* Solaris */
#else
#ifdef HAVE_SYS_SWAP_SWAPTABLE
	double swaptotal,swapfree;

	get_swapinfo(&swaptotal,&swapfree);

	*value=swapfree;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
}

int	PROCCOUNT(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_SYSINFO_PROCS
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
		*value=(double)info.procs;
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
#else
#ifdef	HAVE_PROC_0_PSINFO
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];

	int	fd;
/* In the correct procfs.h, the structure name is psinfo_t */
	psinfo_t psinfo;

	int	proccount=0;

	dir=opendir("/proc");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/psinfo",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			fd = open (filename, O_RDONLY);
			if (fd != -1)
			{
				if (read (fd, &psinfo, sizeof(psinfo)) == -1)
				{
					closedir(dir);
					return SYSINFO_RET_FAIL;
				}
				else
				{
					proccount++;
				}
				close (fd);
			}
			else
			{
				continue;
			}
		}
	}
	closedir(dir);
	*value=(double)proccount;
	return SYSINFO_RET_OK;
#else
#ifdef	HAVE_PROC_1_STATUS
	DIR	*dir;
	struct	dirent *entries;
	struct	stat buf;
	char	filename[MAX_STRING_LEN];
	char	line[MAX_STRING_LEN];

	FILE	*f;

	int	proccount=0;

	dir=opendir("/proc");
	if(NULL == dir)
	{
		return SYSINFO_RET_FAIL;
	}

	while((entries=readdir(dir))!=NULL)
	{
		strscpy(filename,"/proc/");	
		strncat(filename,entries->d_name,MAX_STRING_LEN);
		strncat(filename,"/status",MAX_STRING_LEN);

		if(stat(filename,&buf)==0)
		{
			f=fopen(filename,"r");
			if(f==NULL)
			{
				continue;
			}
			/* This check can be skipped. No need to read anything from this file. */
			if(NULL != fgets(line,MAX_STRING_LEN,f))
			{
				proccount++;
			}
			fclose(f);
		}
	}
	closedir(dir);
	*value=(double)proccount;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
#endif
}

int	SWAPTOTAL(const char *cmd, const char *parameter,double  *value)
{
#ifdef HAVE_SYSINFO_TOTALSWAP
	struct sysinfo info;

	if( 0 == sysinfo(&info))
	{
#ifdef HAVE_SYSINFO_MEM_UNIT
		*value=(double)info.totalswap * (double)info.mem_unit;
#else
		*value=(double)info.totalswap;
#endif
		return SYSINFO_RET_OK;
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}
/* Solaris */
#else
#ifdef HAVE_SYS_SWAP_SWAPTABLE
	double swaptotal,swapfree;

	get_swapinfo(&swaptotal,&swapfree);

	*value=(double)swaptotal;
	return SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif
#endif
}

int	DISK_IO(const char *cmd, const char *parameter,double  *value)
{
#ifdef	HAVE_PROC
	return	getPROC("/proc/stat",2,2,value);
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	DISK_RIO(const char *cmd, const char *parameter,double  *value)
{
#ifdef	HAVE_PROC
	return	getPROC("/proc/stat",3,2,value);
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	DISK_WIO(const char *cmd, const char *parameter,double  *value)
{
#ifdef	HAVE_PROC
	return	getPROC("/proc/stat",4,2,value);
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	DISK_RBLK(const char *cmd, const char *parameter,double  *value)
{
#ifdef	HAVE_PROC
	return	getPROC("/proc/stat",5,2,value);
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	DISK_WBLK(const char *cmd, const char *parameter,double  *value)
{
#ifdef	HAVE_PROC
	return	getPROC("/proc/stat",6,2,value);
#else
	return	SYSINFO_RET_FAIL;
#endif
}

int	VERSION(const char *cmd, const char *parameter,char  **value)
{
	static	char	version[]="1.0 Release\n";

	*value=strdup(version);
	return	SYSINFO_RET_OK;
}

int	EXECUTE_STR(const char *cmd, const char *command, const char *parameter, char  **value)
{
	FILE	*f;
	static	char	c[MAX_STRING_LEN];
	static	char	full_cmd[MAX_STRING_LEN];

	strscpy(full_cmd,command);
	if(parameter != NULL)
	{
		strncat(full_cmd," \"",MAX_STRING_LEN);
		strncat(full_cmd,parameter,MAX_STRING_LEN);
		strncat(full_cmd,"\"",MAX_STRING_LEN);
	}

	f=popen(full_cmd,"r");
	if(f==0)
	{
		switch (errno)
		{
			case	EINTR:
/* (char *) to avoid compiler warning */
				return SYSINFO_RET_TIMEOUT;
			default:
				return SYSINFO_RET_FAIL;
		}
	}

	if(NULL == fgets(c,MAX_STRING_LEN,f))
	{
		switch (errno)
		{
			case	EINTR:
				pclose(f);
/* (char *) to avoid compiler warning */
				return SYSINFO_RET_TIMEOUT;
			default:
				pclose(f);
				return SYSINFO_RET_FAIL;
		}
	}

	if(pclose(f) != 0)
	{
		switch (errno)
		{
			case	EINTR:
/* (char *) to avoid compiler warning */
				return SYSINFO_RET_TIMEOUT;
			default:
				return SYSINFO_RET_FAIL;
		}
	}

	/* We got EOL only */
	if(c[0] == '\n')
	{
		return SYSINFO_RET_FAIL;
	}

	*value=strdup(c);
	return	SYSINFO_RET_OK;
}

int	EXECUTE(const char *cmd, const char *command,double *value)
{
	FILE	*f;
	double	result;
	char	c[MAX_STRING_LEN];

	f=popen( command,"r");
	if(f==0)
	{
		switch (errno)
		{
			case	EINTR:
				return SYSINFO_RET_TIMEOUT;
			default:
				return SYSINFO_RET_FAIL;
		}
	}

	if(NULL == fgets(c,MAX_STRING_LEN,f))
	{
		pclose(f);
		switch (errno)
		{
			case	EINTR:
				return SYSINFO_RET_TIMEOUT;
			default:
				return SYSINFO_RET_FAIL;
		}
	}

	if(pclose(f) != 0)
	{
		switch (errno)
		{
			case	EINTR:
				return SYSINFO_RET_TIMEOUT;
			default:
				return SYSINFO_RET_FAIL;
		}
	}

	/* We got EOL only */
	if(c[0] == '\n')
	{
		return SYSINFO_RET_FAIL;
	}

	sscanf(c, "%lf", &result );

	*value=result;
	return	SYSINFO_RET_OK;
}

void	forward_request(char *proxy,char *command,int port,char *value)
{
	char	*haddr;
	char	c[1024];
	
	int	s;
	struct	sockaddr_in addr;
	int	addrlen;


	struct hostent *host;

	host = gethostbyname(proxy);
	if(host == NULL)
	{
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_NETWORK_ERROR\n");
		return;
	}

	haddr=host->h_addr;

	addrlen = sizeof(addr);
	memset(&addr, 0, addrlen);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	bcopy(haddr, (void *) &addr.sin_addr.s_addr, 4);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_NOTSUPPORTED\n");
		close(s);
		return;
	}

	if (connect(s, (struct sockaddr *) &addr, addrlen) == -1)
	{
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_NETWORK_ERROR\n");
		close(s);
		return;
	}

	if(write(s,command,strlen(command)) == -1)
	{
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_NETWORK_ERROR\n");
		close(s);
		return;
	}

	memset(&c, 0, 1024);
	if(read(s, c, 1024) == -1)
	{
		snprintf(value,MAX_STRING_LEN-1,"%s","ALV_ERROR\n");
		close(s);
		return;
	}
	close(s);
	strcpy(value,c);
}


/* 
 * 0 - NOT OK
 * 1 - OK
 * */
int	tcp_expect(char	*hostname, short port, char *expect,char *sendtoclose, int *value_int)
{
	char	*haddr;
	char	c[1024];
	
	int	s;
	struct	sockaddr_in addr;
	int	addrlen;


	struct hostent *host;

	host = gethostbyname(hostname);
	if(host == NULL)
	{
		*value_int = 0;
		return SYSINFO_RET_OK;
	}

	haddr=host->h_addr;


	addrlen = sizeof(addr);
	memset(&addr, 0, addrlen);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	bcopy(haddr, (void *) &addr.sin_addr.s_addr, 4);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		close(s);
		*value_int = 0;
		return SYSINFO_RET_OK;
	}

	if (connect(s, (struct sockaddr *) &addr, addrlen) == -1)
	{
		close(s);
		*value_int = 0;
		return SYSINFO_RET_OK;
	}

	if( expect == NULL)
	{
		close(s);
		*value_int = 1;
		return SYSINFO_RET_OK;
	}

	memset(&c, 0, 1024);
	recv(s, c, 1024, 0);
	if ( strncmp(c, expect, strlen(expect)) == 0 )
	{
		send(s,sendtoclose,strlen(sendtoclose),0);
		close(s);
		*value_int = 1;
		return SYSINFO_RET_OK;
	}
	else
	{
		send(s,sendtoclose,strlen(sendtoclose),0);
		close(s);
		*value_int = 0;
		return SYSINFO_RET_OK;
	}
}

#ifdef HAVE_LDAP
int    check_ldap(char *hostname, short port,int *value)
{
	int rc;
	LDAP *ldap;
	LDAPMessage *res;
	LDAPMessage *msg;

	char *base = "";
	int scope = LDAP_SCOPE_BASE;
	char *filter="(objectClass=*)";
	int attrsonly=0;
	char *attrs[2];

	attrs[0] = "namingContexts";
	attrs[1] = NULL;

	BerElement *ber;
	char *attr=NULL;
	char **valRes=NULL;

	ldap = ldap_init(hostname, port);
	if ( !ldap )
	{
		*value=0;
		return	SYSINFO_RET_OK;
	}

	rc = ldap_search_s(ldap, base, scope, filter, attrs, attrsonly, &res);
	if( rc != 0 )
	{
		*value=0;
		return	SYSINFO_RET_OK;
	}

	msg = ldap_first_entry(ldap, res);
	if( !msg )
	{
		*value=0;
		return	SYSINFO_RET_OK;
	}
       
	attr = ldap_first_attribute (ldap, msg, &ber);
	valRes = ldap_get_values( ldap, msg, attr );

	ldap_value_free(valRes);
	ldap_memfree(attr);
	if (ber != NULL) {
		ber_free(ber, 0);
	}
	ldap_msgfree(res);
	ldap_unbind(ldap);
       
	*value=1;
	return	SYSINFO_RET_OK;
}
#endif


/* 
 *  0- NOT OK
 *  1 - OK
 * */
int	check_ssh(char	*hostname, short port, int *value)
{
	char	*haddr;
	char	c[MAX_STRING_LEN];
	char	out[MAX_STRING_LEN];
	char	*ssh_proto=NULL;
	char	*ssh_server=NULL;
	
	int	s;
	struct	sockaddr_in addr;
	int	addrlen;


	struct hostent *host;

	host = gethostbyname(hostname);
	if(host == NULL)
	{
		*value=0;
		return	SYSINFO_RET_OK;
	}

	haddr=host->h_addr;

	addrlen = sizeof(addr);
	memset(&addr, 0, addrlen);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	bcopy(haddr, (void *) &addr.sin_addr.s_addr, 4);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		close(s);
		*value=0;
		return	SYSINFO_RET_OK;
	}

	if (connect(s, (struct sockaddr *) &addr, addrlen) == -1)
	{
		close(s);
		*value=0;
		return	SYSINFO_RET_OK;
	}

	memset(&c, 0, 1024);
	recv(s, c, 1024, 0);
	if ( strncmp(c, "SSH", 3) == 0 )
	{
		ssh_proto = c + 4;
		ssh_server = ssh_proto + strspn (ssh_proto, "0123456789-. ");
		ssh_proto[strspn (ssh_proto, "0123456789-. ")] = 0;

/*		printf("[%s] [%s]\n",ssh_proto, ssh_server);*/

		snprintf(out,sizeof(out)-1,"SSH-%s-%s\r\n", ssh_proto, "allview_agent");
		send(s,out,strlen(out),0);

/*		printf("[%s]\n",out);*/

		close(s);
		*value=1;
		return	SYSINFO_RET_OK;
	}
	else
	{
		send(s,"0\n",2,0);
		close(s);
		*value=0;
		return	SYSINFO_RET_OK;
	}
}

/* Example check_service[ssh], check_service[smtp,29],check_service[ssh,127.0.0.1,22]*/
/* check_service[ssh,127.0.0.1,ssh] */
int	CHECK_SERVICE_PERF(const char *cmd, const char *service_and_ip_and_port,double  *value)
{
	char	*c,*c1;
	int	port=0;
	char	service[MAX_STRING_LEN];
	char	ip[MAX_STRING_LEN];
	char	port_str[MAX_STRING_LEN];

	struct timeval t1,t2;
	struct timezone tz1,tz2;

	int	result;
	int	value_int;

	long	exec_time;

	gettimeofday(&t1,&tz1);

	c=strchr(service_and_ip_and_port,',');
	strscpy(service,service_and_ip_and_port);

	if(c != NULL)
	{
		strscpy(ip,c+1);
		service[c-service_and_ip_and_port]=0;

		c1=strchr(ip,',');
		
		if(c1!=NULL)
		{
			strscpy(port_str,c1+1);
			ip[c1-ip]=0;
			port=atoi(port_str);
		}
		else
		{
			if(strchr(ip,'.')==NULL)
			{
				strscpy(port_str,ip);
				port=atoi(port_str);
				strcpy(ip,"127.0.0.1");
			}
		}
	}
	else
	{
		strcpy(ip,"127.0.0.1");
	}

/*	printf("IP:[%s]",ip);
	printf("Service:[%s]",service);
	printf("Port:[%d]",port);*/

	if(strcmp(service,"ssh") == 0)
	{
		if(port == 0)	port=22;
		result=check_ssh(ip,port,&value_int);
	}
#ifdef HAVE_LDAP
	else if(strcmp(service,"ldap") == 0)
	{
		if(port == 0)   port=389;
		result=check_ldap(ip,port,&value_int);
	}
#endif
	else if(strcmp(service,"smtp") == 0)
	{
		if(port == 0)	port=25;
		result=tcp_expect(ip,port,"220","QUIT\n",&value_int);
	}
	else if(strcmp(service,"ftp") == 0)
	{
		if(port == 0)	port=21;
		result=tcp_expect(ip,port,"220","",&value_int);
	}
	else if(strcmp(service,"http") == 0)
	{
		if(port == 0)	port=80;
		result=tcp_expect(ip,port,NULL,"",&value_int);
	}
	else if(strcmp(service,"pop") == 0)
	{
		if(port == 0)	port=110;
		result=tcp_expect(ip,port,"+OK","",&value_int);
	}
	else if(strcmp(service,"nntp") == 0)
	{
		if(port == 0)	port=119;
/* 220 is incorrect */
/*		result=tcp_expect(ip,port,"220","");*/
		result=tcp_expect(ip,port,"200","",&value_int);
	}
	else if(strcmp(service,"imap") == 0)
	{
		if(port == 0)	port=143;
		result=tcp_expect(ip,port,"* OK","a1 LOGOUT\n",&value_int);
	}
	else if(strcmp(service,"tcp") == 0)
	{
		if(port == 0)	port=80;
		result=tcp_expect(ip,port,NULL,"",&value_int);
	}
	else
	{
		return SYSINFO_RET_FAIL;
	}

	if(1 == value_int)
	{
		gettimeofday(&t2,&tz2);
   		exec_time=(t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
		*value=(double)exec_time/1000000;
		return SYSINFO_RET_OK;
	}
	else
	{
		*value=0;
		return SYSINFO_RET_OK;
	}
}

/* Example check_service[ssh], check_service[smtp,29],check_service[ssh,127.0.0.1,22]*/
/* check_service[ssh,127.0.0.1,ssh] */
int	CHECK_SERVICE(const char *cmd, const char *service_and_ip_and_port,double  *value)
{
	int	port=0;
	char	service[MAX_STRING_LEN];
	char	ip[MAX_STRING_LEN];
	char	port_str[MAX_STRING_LEN];
	char	tmp[MAX_STRING_LEN];
	char	*s;

	int	result;
	int	value_int;

	/* Default IP address */
	strscpy(ip,"127.0.0.1");

	strscpy(tmp,service_and_ip_and_port);

	s=strtok(tmp,",");
	if(s)
	{
		strscpy(service,s);

		s = strtok(NULL,",");
	}
	if(s)
	{
		if(strchr(s,'.')!=NULL)
		{
			strscpy(ip,s);
		}
		else
		{
			strscpy(port_str,s);
			port=atoi(port_str);
		}

		s = strtok(NULL,",");
	}
	if(s)
	{
		if(strchr(s,'.')!=NULL)
		{
			strscpy(ip,s);
		}
		else
		{
			strscpy(port_str,s);
			port=atoi(port_str);
		}
		s = strtok(NULL,",");
	}

/*	printf("IP:[%s]\n",ip);
	printf("Service:[%s]\n",service);
	printf("Port:[%d]\n\n",port);*/

/*	c=strchr(service_and_ip_and_port,',');
	strscpy(service,service_and_ip_and_port);

	if(c != NULL)
	{
		strscpy(ip,c+1);
		service[c-service_and_ip_and_port]=0;

		c1=strchr(ip,',');
		
		if(c1!=NULL)
		{
			strscpy(port_str,c1+1);
			ip[c1-ip]=0;
			port=atoi(port_str);
		}
		else
		{
			if(strchr(ip,'.')==NULL)
			{
				strscpy(port_str,ip);
				port=atoi(port_str);
				strcpy(ip,"127.0.0.1");
			}
		}
	}
	else
	{
		strcpy(ip,"127.0.0.1");
	}*/

/*	printf("IP:[%s]",ip);
	printf("Service:[%s]",service);
	printf("Port:[%d]",port);*/

	if(strcmp(service,"ssh") == 0)
	{
		if(port == 0)	port=22;
		result=check_ssh(ip,port,&value_int);
	}
#ifdef HAVE_LDAP
	else if(strcmp(service,"ldap") == 0)
	{
		if(port == 0)   port=389;
		result=check_ldap(ip,port,&value_int);
	}
#endif
	else if(strcmp(service,"smtp") == 0)
	{
		if(port == 0)	port=25;
		result=tcp_expect(ip,port,"220","QUIT\n",&value_int);
	}
	else if(strcmp(service,"ftp") == 0)
	{
		if(port == 0)	port=21;
		result=tcp_expect(ip,port,"220","",&value_int);
	}
	else if(strcmp(service,"http") == 0)
	{
		if(port == 0)	port=80;
		result=tcp_expect(ip,port,NULL,"",&value_int);
	}
	else if(strcmp(service,"pop") == 0)
	{
		if(port == 0)	port=110;
		result=tcp_expect(ip,port,"+OK","",&value_int);
	}
	else if(strcmp(service,"nntp") == 0)
	{
		if(port == 0)	port=119;
/* 220 is incorrect */
/*		result=tcp_expect(ip,port,"220","");*/
		result=tcp_expect(ip,port,"200","",&value_int);
	}
	else if(strcmp(service,"imap") == 0)
	{
		if(port == 0)	port=143;
		result=tcp_expect(ip,port,"* OK","a1 LOGOUT\n",&value_int);
	}
	else if(strcmp(service,"tcp") == 0)
	{
		if(port == 0)	port=80;
		result=tcp_expect(ip,port,NULL,"",&value_int);
	}
	else
	{
		result=SYSINFO_RET_FAIL;
	}

	*value=(double)value_int;

	return result;
}

int	CHECK_PORT(const char *cmd, const char *ip_and_port,double  *value)
{
	char	*c;
	int	port=0;
	int	value_int;
	int	result;
	char	ip[MAX_STRING_LEN];

	c=strchr(ip_and_port,',');
	strscpy(ip,ip_and_port);

	if(c != NULL)
	{
		port=atoi(c+1);
		ip[c-ip_and_port]=0;
	}
	else
	{
		port=atoi(ip_and_port);
		strcpy(ip,"127.0.0.1");
	}

	result = tcp_expect(ip,port,NULL,"",&value_int);
	*value = (double)value_int;
	return result;
}
