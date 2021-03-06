/* 
** AllviewW32 - Win32 agent for Allview
** Copyright (C) 2002 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** $module: config.cpp
**
**/

#include "allvieww32.h"


//
// Static data
//

static int numSubAgents;


//
// Parse UserParameter=... parameter
// Argument is a parameter name and command line separated by comma
//

static BOOL AddUserParameter(char *args,int sourceLine)
{
   char *cmdLine;
   char *buffer;

   cmdLine=strchr(args,',');
   if (cmdLine==NULL)
   {
      if (IsStandalone())  
         printf("Error in configuration file, line %d: missing command line in UserParameter\n",sourceLine);
      return FALSE;
   }

   *cmdLine=0;
   cmdLine++;

   buffer=(char *)malloc(strlen(cmdLine)+32);
   sprintf(buffer,"__exec{%s}",cmdLine);
   AddAlias(args,buffer);
   free(buffer);
   return TRUE;
}


//
// Parse SubAgent=... parameter
// Argument is a module name and command line separated by comma
//

static BOOL AddSubAgent(char *args)
{
   char *cmdLine;

   cmdLine=strchr(args,',');
   if (cmdLine!=NULL)
   {
      *cmdLine=0;
      cmdLine++;
   }

   subagentNameList=(SUBAGENT_NAME *)realloc(subagentNameList,sizeof(SUBAGENT_NAME)*(numSubAgents+2));
   subagentNameList[numSubAgents].name=strdup(args);
   subagentNameList[numSubAgents].cmdLine=cmdLine==NULL ? NULL : strdup(cmdLine);
   numSubAgents++;
   subagentNameList[numSubAgents].name=NULL;

   return TRUE;
}


//
// Parse PerfCounter=... parameter and add new performance counter
// Argument is a config file parameter value which should have the following syntax:
//    <key>,"<counter path>",<time interval>
// Returns TRUE on success and FALSE otherwise
//

static BOOL AddPerformanceCounter(char *args)
{
   char *ptr1,*ptr2,*eptr,buffer[MAX_ALIAS_NAME];
   USER_COUNTER *counter;
   int i;

   ptr1=strchr(args,',');
   if (ptr1==NULL)
      return FALSE;     // Invalid syntax

   *ptr1=0;
   ptr1++;
   StrStrip(args);
   StrStrip(ptr1);
   if (*ptr1!='"')
      return FALSE;     // Invalid syntax
   ptr1++;
   ptr2=strchr(ptr1,'"');
   if (ptr2==NULL)
      return FALSE;     // Invalid syntax
   *ptr2=0;
   ptr2++;
   StrStrip(ptr2);
   if (*ptr2!=',')
      return FALSE;     // Invalid syntax
   ptr2++;
   StrStrip(ptr2);

   i=strtol(ptr2,&eptr,10);
   if ((*eptr!=0)||     // Not a decimal number
       (i<1)||(i>1800)) // Interval value out of range
      return FALSE;     // Invalid syntax

   // Add internal alias
   sprintf(buffer,"__usercnt{%s}",args);
   if (!AddAlias(args,buffer))
      return FALSE;

   counter=(USER_COUNTER *)malloc(sizeof(USER_COUNTER));
   memset(counter,0,sizeof(USER_COUNTER));

   strncpy(counter->name,args,MAX_COUNTER_NAME-1);
   strncpy(counter->counterPath,ptr1,MAX_PATH-1);
   counter->interval=i;
   counter->rawValueArray=(PDH_RAW_COUNTER *)malloc(sizeof(PDH_RAW_COUNTER)*counter->interval);

   // Add to the list
   counter->next=userCounterList;
   userCounterList=counter;
   return TRUE;
}


//
// Parse Server=... parameter
//

static int ParseServerList(char *args,int sourceLine)
{
   char *sptr,*eptr;
   int errors=0;

   for(sptr=args;(sptr!=(char *)1)&&(confServerCount<MAX_SERVERS);sptr=eptr+1)
   {
      eptr=strchr(sptr,',');
      if (eptr!=NULL)
         *eptr=0;

      confServerAddr[confServerCount]=inet_addr(sptr);
      if (confServerAddr[confServerCount]==INADDR_NONE)
      {
         errors++;
         if (IsStandalone())  
            printf("Error in configuration file, line %d: invalid server's IP address (%s)\n",sourceLine,sptr);
      }
      else
      {
         confServerCount++;
      }
   }

   return errors;
}


//
// Read configuration
//

BOOL ReadConfig(void)
{
   FILE *cfg;
   char *ptr,buffer[4096];
   int sourceLine=0,errors=0;

   if (IsStandalone())
      printf("Using configuration file \"%s\"\n",confFile);

   cfg=fopen(confFile,"r");
   if (cfg==NULL)
   {
      if (IsStandalone())
         printf("Unable to open configuration file: %s\n",strerror(errno));
      return FALSE;
   }

   numSubAgents=0;

   while(!feof(cfg))
   {
      buffer[0]=0;
      fgets(buffer,4095,cfg);
      sourceLine++;
      ptr=strchr(buffer,'\n');
      if (ptr!=NULL)
         *ptr=0;
      ptr=strchr(buffer,'#');
      if (ptr!=NULL)
         *ptr=0;

      StrStrip(buffer);
      if (buffer[0]==0)
         continue;

      ptr=strchr(buffer,'=');
      if (ptr==NULL)
      {
         errors++;
         if (IsStandalone())
            printf("Syntax error in configuration file, line %d\n",sourceLine);
         continue;
      }
      *ptr=0;
      ptr++;
      StrStrip(buffer);
      StrStrip(ptr);

      if (!stricmp(buffer,"LogFile"))
      {
         if (!stricmp(ptr,"{EventLog}"))
         {
            dwFlags|=AF_USE_EVENT_LOG;
         }
         else
         {
            dwFlags&=~AF_USE_EVENT_LOG;
            memset(logFile,0,MAX_PATH);
            strncpy(logFile,ptr,MAX_PATH-1);
         }
      }
      else if (!stricmp(buffer,"Server"))
      {
         int rc;

         if ((rc=ParseServerList(ptr,sourceLine))>0)
            errors+=rc;
      }
      else if (!stricmp(buffer,"ListenPort"))
      {
         int n;

         n=atoi(ptr);
         if ((n<1)||(n>65535))
         {
            confListenPort=10000;
            errors++;
            if (IsStandalone())
               printf("Error in configuration file, line %d: invalid port number (%s)\n",sourceLine,ptr);
         }
         else
         {
            confListenPort=(WORD)n;
         }
      }
      else if (!stricmp(buffer,"Alias"))
      {
         char *sep;

         sep=strchr(ptr,':');
         if (sep==NULL)
         {
            errors++;
            if (IsStandalone())
               printf("Error in configuration file, line %d: invalid alias syntax\n",sourceLine);
         }
         else
         {
            *sep=0;
            sep++;
            StrStrip(ptr);
            StrStrip(sep);
            AddAlias(ptr,sep);
         }
      }
      else if (!stricmp(buffer,"Timeout"))
      {
         int tm;

         tm=atoi(ptr);
         if ((tm>0)&&(tm<=30))
         {
            confTimeout=tm*1000;    // Convert to milliseconds
         }
         else
         {
            errors++;
            if (IsStandalone())
               printf("Error in configuration file, line %d: invalid timeout value (%d seconds)\n",
                      sourceLine,tm);
         }
      }
      else if (!stricmp(buffer,"LogLevel"))
      {
         g_dwLogLevel = strtoul(buffer, NULL, 0);
      }
      else if (!stricmp(buffer,"PerfCounter"))
      {
         if (!AddPerformanceCounter(ptr))
         {
            errors++;
            if (IsStandalone())
               printf("Error in configuration file, line %d: invalid performance counter specification\n",
                      sourceLine);
         }
      }
      else if (!stricmp(buffer,"MaxCollectorProcessingTime"))
      {
         int tm;

         tm=atoi(ptr);
         if ((tm>0)&&(tm<=500))
         {
            confMaxProcTime=tm;
         }
         else
         {
            errors++;
            if (IsStandalone())
               printf("Error in configuration file, line %d: invalid collector sample processing time value (%d milliseconds)\n",
                      sourceLine,tm);
         }
      }
      else if (!stricmp(buffer,"LogUnresolvedSymbols"))
      {
         if ((!stricmp(ptr,"1"))||(!stricmp(ptr,"yes"))||(!stricmp(ptr,"true")))
            dwFlags|=AF_LOG_UNRESOLVED_SYMBOLS;
         else
            dwFlags&=~AF_LOG_UNRESOLVED_SYMBOLS;
      }
      else if (!stricmp(buffer,"SubAgent"))
      {
         if (!AddSubAgent(ptr))
            errors++;
      }
      else if (!stricmp(buffer,"UserParameter"))
      {
         if (!AddUserParameter(ptr,sourceLine))
            errors++;
      }
      else if ((!stricmp(buffer,"PidFile"))||(!stricmp(buffer,"NoTimeWait"))||
               (!stricmp(buffer,"StartAgents"))||(!stricmp(buffer,"DebugLevel")))
      {
         // Ignore these parameters, they are for compatibility with UNIX agent only
      }
      else
      {
         errors++;
         if (dwFlags & AF_STANDALONE)
            printf("Error in configuration file, line %d: unknown option \"%s\"\n",sourceLine,buffer);
      }
   }

   if ((IsStandalone())&&(!errors))
      printf("Configuration file OK\n");

   fclose(cfg);
   return TRUE;
}
