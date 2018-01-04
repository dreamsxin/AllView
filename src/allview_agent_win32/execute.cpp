/* 
** AllviewW32 - Win32 agent for Allview
** Copyright (C) 2002,2003 Victor Kirhenshtein
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
** $module: execute.cpp
**
**/

#include "allvieww32.h"


LONG H_Execute(char *cmd,char *arg,char **value)
{
   char *ptr1,*ptr2;
   STARTUPINFO si;
   PROCESS_INFORMATION pi;
   SECURITY_ATTRIBUTES sa;
   HANDLE hOutput;
   char szTempPath[MAX_PATH],szTempFile[MAX_PATH];
   DWORD dwBytes=0;

   // Extract command line
   ptr1=strchr(cmd,'{');
   ptr2=strchr(cmd,'}');
   ptr1++;
   *ptr2=0;

   // Create temporary file to hold process output
   GetTempPath(MAX_PATH-1,szTempPath);
   GetTempFileName(szTempPath,"zbx",0,szTempFile);
   sa.nLength=sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor=NULL;
   sa.bInheritHandle=TRUE;
   hOutput=CreateFile(szTempFile,GENERIC_READ | GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
   if (hOutput==INVALID_HANDLE_VALUE)
   {
      WriteLog(MSG_CREATE_TMP_FILE_FAILED,EVENTLOG_ERROR_TYPE,"e",GetLastError());
      return SYSINFO_RC_ERROR;
   }

   // Fill in process startup info structure
   memset(&si,0,sizeof(STARTUPINFO));
   si.cb=sizeof(STARTUPINFO);
   si.dwFlags=STARTF_USESTDHANDLES;
   si.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
   si.hStdOutput=hOutput;
   si.hStdError=GetStdHandle(STD_ERROR_HANDLE);

   // Create new process
   if (!CreateProcess(NULL,ptr1,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
   {
      WriteLog(MSG_CREATE_PROCESS_FAILED,EVENTLOG_ERROR_TYPE,"se",ptr1,GetLastError());
      return SYSINFO_RC_NOTSUPPORTED;
   }

   // Wait for process termination and close all handles
   WaitForSingleObject(pi.hProcess,INFINITE);
   CloseHandle(pi.hThread);
   CloseHandle(pi.hProcess);

   // Rewind temporary file for reading
   SetFilePointer(hOutput,0,NULL,FILE_BEGIN);

   // Read process output
   *value=(char *)malloc(MAX_STRING_LEN);
   ReadFile(hOutput,*value,MAX_STRING_LEN-1,&dwBytes,NULL);
   (*value)[dwBytes]=0;
   ptr1=strchr(*value,'\n');
   if (ptr1!=NULL)
      *ptr1=0;

   // Remove temporary file
   CloseHandle(hOutput);
   DeleteFile(szTempFile);

   return SYSINFO_RC_SUCCESS;
}
