/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2014-2015 BigObject Inc.
 * All Rights Reserved.
 *
 * Use of, copying, modifications to, and distribution of this software
 * and its documentation without BigObject's written permission can
 * result in the violation of U.S., Taiwan and China Copyright and Patent laws.
 * Violators will be prosecuted to the highest extent of the applicable laws.
 *
 * BIGOBJECT MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 * ----------------------------------------------------------------------------
 */


#ifndef _BIGOBJECT_DEBUG_H_
#define _BIGOBJECT_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstring>

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE  // TCHAR.h
#endif
#endif

#ifndef DEBUG_FILE
#define DEBUG_FILE "Debug.log"
#endif

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <wchar.h>
#include <winerror.h>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

#else
#ifdef _UNICODE
#ifndef __SQLTYPES_H
#define TCHAR   wchar_t
#endif

#define _TEXT_HELPER(x) L ## x
#define TEXT(x)         _TEXT_HELPER(x)

#define _fputts    fputws
#define _sntprintf swprintf
#define _tcslen    wcslen
#define _tfopen    _wfopen
#define _vstprintf vswprintf
#define _tcsrchr   wcsrchr 
#else
// Dirty hack to avoid conflict, in sqltypes.h TCHAR defined as typedef, we
// don't have a good way to detect typedef this time.
#ifndef __SQLTYPES_H
#define TCHAR   char
#endif

#define TEXT(x) x

#define _fputts    fputs
#define _sntprintf snprintf
#define _tcslen    strlen
#define _tfopen    fopen
#define _vstprintf vsprintf
#define _tcsrchr   strrchr 
#endif
#define LPTSTR  TCHAR*
#define LPCTSTR const LPTSTR
#endif

#ifdef _DEBUG

/**
Debug dumper.
*/
class DebugDumper
{
public:

#ifdef UNICODE
 typedef std::wstring tstring;
#else
 typedef std::string tstring;
#endif

 DebugDumper(void)
 {
 }
 
 ~DebugDumper(void)
 {
 }

#ifdef WIN32 

 static BOOL WINAPI WriteMiniDump(HANDLE hFile, 
                                  LPEXCEPTION_POINTERS pException)
 {
  MINIDUMP_EXCEPTION_INFORMATION exInfo = { 0 };

  if(pException == NULL)
   return FALSE;

  exInfo.ThreadId = ::GetCurrentThreadId();
  exInfo.ExceptionPointers = pException;
  exInfo.ClientPointers = NULL;

  // Write the dump.
  if(!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, 
                        MiniDumpNormal, &exInfo, NULL, NULL))
  {
   return FALSE;
  }
  
  return TRUE;
 }

 static LONG WINAPI HandleException(LPEXCEPTION_POINTERS pException)
 {
#define CREATE_DUMP_FILE(p)                                            \
  {                                                                    \
   _sntprintf(szPath, MAX_PATH, TEXT("%s\\Crash-%d-%s.dmp"),           \
    p.c_str(), GetCurrentProcessId(), strTime.c_str());                \
                                                                       \
   hFile = ::CreateFile(szPath, GENERIC_WRITE,                         \
                        FILE_SHARE_WRITE, NULL,                        \
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);   \
  }
  
  TCHAR szPath[MAX_PATH] = { 0 };
#ifdef _DEBUG  
  LONG ret = EXCEPTION_CONTINUE_SEARCH;
#else
  LONG ret = EXCEPTION_EXECUTE_HANDLER; 
#endif    

#ifdef _USRDLL
  // Create dump in DLL not stable.
  return ret;
#endif

  HANDLE hFile;
  
  tstring strTime = DebugDumper::FormateDateTime(time(NULL));
  tstring strRootPath = DebugDumper::GetRootPath();

  CREATE_DUMP_FILE(strRootPath);
  if(hFile != INVALID_HANDLE_VALUE)
  {
   if(!DebugDumper::WriteMiniDump(hFile, pException))
   {
    ; // Pass...
   }         
   
   ::CloseHandle(hFile);
  }
  else
  {
   // Try current directory.
   TCHAR szCurDir[MAX_PATH] = { 0 };
   tstring strCurPath;

   GetCurrentDirectory(MAX_PATH - 1, szCurDir);
   
   strCurPath.assign(szCurDir);
   
   CREATE_DUMP_FILE(strCurPath);
   if(hFile != INVALID_HANDLE_VALUE)
   {
    if(!DebugDumper::WriteMiniDump(hFile, pException))
    {
     ; // Pass...
    }         
   
    ::CloseHandle(hFile);   
   }
  }

  return ret;
 }
#endif 
 
 static tstring FormateDateTime(time_t t)
 {
  TCHAR szData[128] = { 0 };
  
  tm* ptm = localtime(&t);
  
  _sntprintf(szData, 128, TEXT("%d-%.2d-%.2d %.2d-%.2d-%.2d"),
   ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, 
   ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
   
  return szData; 
 }
 
 static const LPTSTR GetRootPath(void)
 {
#ifdef WIN32
  static TCHAR szPath[MAX_PATH];
  static bool bFirstTime = true;

  if(bFirstTime)
  {
   bFirstTime = false;
   GetModuleFileName(NULL, szPath, sizeof(szPath));
   TCHAR *p = _tcsrchr(szPath, '\\');
   *p = '\0';
  }

  return szPath;
#else
  return NULL;  
#endif 
 }

 static void SetupDebugFacilities()
 {
#ifdef WIN32

  SetUnhandledExceptionFilter(DebugDumper::HandleException); 

  // Please see ACE C++ Library Object_Manager.cpp ACE_Object_Manager::init.
 #if (_MSC_VER >= 1400) // VC++ 8.0 and above.
  // And this will stop the abort system call from being treated as a crash
  //_set_abort_behavior( 0,  _CALL_REPORTFAULT);

  /*
  Note the following fix was derived from that proposed by Jochen Kalmbach
  http://blog.kalmbachnet.de/?postid=75
  See also:
  http://connect.microsoft.com/VisualStudio/feedback/
  ViewFeedback.aspx?FeedbackID=101337
 
  Starting with VC8, Microsoft changed the behaviour of the CRT in some
  security related and special situations. The are many situations in which our
  ACE_UnhandledExceptionFilter will never be called. This is a major change to
  the previous versions of the CRT and is not very well documented.
  The CRT simply forces the call to the default-debugger without informing the
  registered unhandled exception filter. Jochen's solution is to stop the CRT
  from calling SetUnhandledExceptionFilter() after we have done so above.
  NOTE this only works for intel based windows builds.
  */
 #ifdef _M_IX86
  HMODULE hKernel32 = LoadLibrary(TEXT("kernel32.dll"));
  if(hKernel32)
  {
   void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
   if(pOrgEntry)
   {
    unsigned char newJump[ 100 ];
    DWORD dwOrgEntryAddr = reinterpret_cast<DWORD>(pOrgEntry);
    dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
    void *pNewFunc = &DebugDumper::HandleException;
    DWORD dwNewEntryAddr = reinterpret_cast<DWORD>(pNewFunc);
    DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

    newJump[0] = 0xE9;  // JMP absolute
    memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
    SIZE_T bytesWritten;
    WriteProcessMemory(GetCurrentProcess(), pOrgEntry, newJump, 
                       sizeof(pNewFunc) + 1, &bytesWritten);
   }
  }
#endif // _M_IX86
#endif // (_MSC_VER >= 1400) // VC++ 8.0 and above.
#endif // WIN32
 }
};

#define DEBUG_SETUP_FACILITIES DebugDumper::SetupDebugFacilities();

#else

#define DEBUG_SETUP_FACILITIES

#endif // end _DEBUG

#endif
