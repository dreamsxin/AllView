
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

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <errno.h>

#include <time.h>

/* No warning for bzero */
#include <string.h>
#include <strings.h>

/* For config file operations */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* For setpriority */
#include <sys/time.h>
#include <sys/resource.h>

/* Required for getpwuid */
#include <pwd.h>

#include <dirent.h>

#include "common.h"
#include "sysinfo.h"
#include "security.h"
#include "allview_agent.h"

#include "log.h"
#include "cfg.h"
#include "cpustat.h"

CPUSTAT cpustat;

void	init_stats_cpustat()
{
	int	i;

	for(i=0;i<60*15;i++)
	{
		cpustat.clock[i]=0;
	}
}

void	report_stats_cpustat(FILE *file, int now)
{
	int	time=0,
		time1=0,
		time5=0,
		time15=0;
	float
		cpu_idle=0,
		cpu_idle1=0,
		cpu_idle5=0,
		cpu_idle15=0,
		cpu_user=0,
		cpu_user1=0,
		cpu_user5=0,
		cpu_user15=0,
		cpu_system=0,
		cpu_system1=0,
		cpu_system5=0,
		cpu_system15=0,
		cpu_nice=0,
		cpu_nice1=0,
		cpu_nice5=0,
		cpu_nice15=0,
		cpu_sum=0,
		cpu_sum1=0,
		cpu_sum5=0,
		cpu_sum15=0;

	int	i;

	time=now+1;
	time1=now+1;
	time5=now+1;
	time15=now+1;
	for(i=0;i<60*15;i++)
	{
		if(cpustat.clock[i]==0)
		{
			continue;
		}
		if(cpustat.clock[i]==now)
		{
			continue;
		}
		if((cpustat.clock[i] >= now-60) && (time1 > cpustat.clock[i]))
		{
			time1=cpustat.clock[i];
		}
		if((cpustat.clock[i] >= now-5*60) && (time5 > cpustat.clock[i]))
		{
			time5=cpustat.clock[i];
		}
		if((cpustat.clock[i] >= now-15*60) && (time15 > cpustat.clock[i]))
		{
			time15=cpustat.clock[i];
		}
	}
	for(i=0;i<60*15;i++)
	{
		if(cpustat.clock[i]==now)
		{
			cpu_idle=cpustat.cpu_idle[i];
			cpu_user=cpustat.cpu_user[i];
			cpu_nice=cpustat.cpu_nice[i];
			cpu_system=cpustat.cpu_system[i];
			cpu_sum=cpu_idle+cpu_user+cpu_nice+cpu_system;
		}
		if(cpustat.clock[i]==time1)
		{
			cpu_idle1=cpustat.cpu_idle[i];
			cpu_user1=cpustat.cpu_user[i];
			cpu_nice1=cpustat.cpu_nice[i];
			cpu_system1=cpustat.cpu_system[i];
			cpu_sum1=cpu_idle1+cpu_user1+cpu_nice1+cpu_system1;
		}
		if(cpustat.clock[i]==time5)
		{
			cpu_idle5=cpustat.cpu_idle[i];
			cpu_user5=cpustat.cpu_user[i];
			cpu_nice5=cpustat.cpu_nice[i];
			cpu_system5=cpustat.cpu_system[i];
			cpu_sum5=cpu_idle5+cpu_user5+cpu_nice5+cpu_system5;
		}
		if(cpustat.clock[i]==time15)
		{
			cpu_idle15=cpustat.cpu_idle[i];
			cpu_user15=cpustat.cpu_user[i];
			cpu_nice15=cpustat.cpu_nice[i];
			cpu_system15=cpustat.cpu_system[i];
			cpu_sum15=cpu_idle15+cpu_user15+cpu_nice15+cpu_system15;
		}
	}
	if((cpu_idle!=0)&&(cpu_idle1!=0))
	{
		fprintf(file,"cpu[idle1] %f\n", 100*(float)((cpu_idle-cpu_idle1)/(cpu_sum-cpu_sum1)));
	}
	else
	{
		fprintf(file,"cpu[idle1] 0\n");
	}
	if((cpu_idle!=0)&&(cpu_idle5!=0))
	{
		fprintf(file,"cpu[idle5] %f\n",100*(float)((cpu_idle-cpu_idle5)/(cpu_sum-cpu_sum5)));
	}
	else
	{
		fprintf(file,"cpu[idle5] 0\n");
	}
	if((cpu_idle!=0)&&(cpu_idle15!=0))
	{
		fprintf(file,"cpu[idle15] %f\n", 100*(float)((cpu_idle-cpu_idle15)/((cpu_sum-cpu_sum15))));
	}
	else
	{
		fprintf(file,"cpu[idle15] 0\n");
	}

	if((cpu_user!=0)&&(cpu_user1!=0))
	{
		fprintf(file,"cpu[user1] %f\n", 100*(float)((cpu_user-cpu_user1)/((cpu_sum-cpu_sum1))));
	}
	else
	{
		fprintf(file,"cpu[user1] 0\n");
	}
	if((cpu_user!=0)&&(cpu_user5!=0))
	{
		fprintf(file,"cpu[user5] %f\n", 100*(float)((cpu_user-cpu_user5)/((cpu_sum-cpu_sum5))));
	}
	else
	{
		fprintf(file,"cpu[user5] 0\n");
	}
	if((cpu_user!=0)&&(cpu_user15!=0))
	{
		fprintf(file,"cpu[user15] %f\n", 100*(float)((cpu_user-cpu_user15)/((cpu_sum-cpu_sum15))));
	}
	else
	{
		fprintf(file,"cpu[user15] 0\n");
	}

	if((cpu_nice!=0)&&(cpu_nice1!=0))
	{
		fprintf(file,"cpu[nice1] %f\n", 100*(float)((cpu_nice-cpu_nice1)/((cpu_sum-cpu_sum1))));
	}
	else
	{
		fprintf(file,"cpu[nice1] 0\n");
	}
	if((cpu_nice!=0)&&(cpu_nice5!=0))
	{
		fprintf(file,"cpu[nice5] %f\n", 100*(float)((cpu_nice-cpu_nice5)/((cpu_sum-cpu_sum5))));
	}
	else
	{
		fprintf(file,"cpu[nice5] 0\n");
	}
	if((cpu_nice!=0)&&(cpu_nice15!=0))
	{
		fprintf(file,"cpu[nice15] %f\n", 100*(float)((cpu_nice-cpu_nice15)/((cpu_sum-cpu_sum15))));
	}
	else
	{
		fprintf(file,"cpu[nice15] 0\n");
	}

	if((cpu_system!=0)&&(cpu_system1!=0))
	{
		fprintf(file,"cpu[system1] %f\n", 100*(float)((cpu_system-cpu_system1)/((cpu_sum-cpu_sum1))));
	}
	else
	{
		fprintf(file,"cpu[system1] 0\n");
	}
	if((cpu_system!=0)&&(cpu_system5!=0))
	{
		fprintf(file,"cpu[system5] %f\n", 100*(float)((cpu_system-cpu_system5)/((cpu_sum-cpu_sum5))));
	}
	else
	{
		fprintf(file,"cpu[system5] 0\n");
	}
	if((cpu_system!=0)&&(cpu_system15!=0))
	{
		fprintf(file,"cpu[system15] %f\n", 100*(float)((cpu_system-cpu_system15)/((cpu_sum-cpu_sum15))));
	}
	else
	{
		fprintf(file,"cpu[system15] 0\n");
	}
}


void	add_values_cpustat(int now,float cpu_user,float cpu_system,float cpu_nice,float cpu_idle)
{
	int i;

/*	printf("Add_values [%s] [%f] [%f]\n",interface,value_sent,value_received);*/

	for(i=0;i<15*60;i++)
	{
		if(cpustat.clock[i]<now-15*60)
		{
			cpustat.clock[i]=now;
			cpustat.cpu_user[i]=cpu_user;;
			cpustat.cpu_system[i]=cpu_system;
			cpustat.cpu_nice[i]=cpu_nice;
			cpustat.cpu_idle[i]=cpu_idle;
			break;
		}
	}
}

void	collect_stats_cpustat(FILE *outfile)
{
	FILE	*file;

	char	*s;
	char	line[MAX_STRING_LEN];
	int	i;
	int	now;
	float	cpu_user, cpu_nice, cpu_system, cpu_idle;

	/* Must be static */
	static	int initialised=0;

	if( 0 == initialised)
	{
		init_stats_cpustat();
		initialised=1;
	}

	now=time(NULL);

	file=fopen("/proc/stat","r");
	if(NULL == file)
	{
		fprintf(stderr, "Cannot open [%s] [%m]\n","/proc/stat");
		return;
	}
	i=0;
	while(fgets(line,1024,file) != NULL)
	{
		if( (s=strstr(line,"cpu ")) == NULL)
			continue;

		s=line;

		sscanf(s,"cpu %f %f %f %f",&cpu_user, &cpu_nice, &cpu_system, &cpu_idle);
		add_values_cpustat(now,cpu_user, cpu_system, cpu_nice, cpu_idle);
		break;
	}

	fclose(file);

	report_stats_cpustat(outfile, now);
}
