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


#include <exception>
#include <stdexcept>
#include <string>

#include "Driver.hpp"
#include "Version.hpp"

using namespace bigobject::Logging;

#if defined(WIN32)

extern HINSTANCE hModule = NULL;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
 static int initialized = 0;

 // DLL is being loaded.
 if(fdwReason == DLL_PROCESS_ATTACH)
 {
  // Complicated debugging mechanism not suitable for ODBC Driver.
  // DEBUG_SETUP_FACILITIES

  if(!initialized++)
  {
   // Any initialization global to DLL goes here.

   hModule = hInst;
   Logger logger = LoggerFactory::InitLogger();

#ifdef _DEBUG
   // Get the current dll path.
   TCHAR dllDir[MAX_PATH + 1];
   DWORD c = GetModuleFileName(hModule, dllDir, MAX_PATH);
   for(int i = (int)c; i >= 0; i--)
   {
    if(dllDir[i] == '\\')
     break;

    dllDir[i] = '\0';
   }

   tstring logFile(dllDir);

   logFile.append(TEXT(DEBUG_FILE));
#endif

   if(!LoggerFactory::ConfigLogger(TEXT(""),
#ifdef _DEBUG
        TEXT("DEBUG"), //LOG4CPLUS_STRING_TO_TSTRING(logLevel),
        logFile))
#else
        TEXT("ERROR"),
        TEXT("")))//LOG4CPLUS_STRING_TO_TSTRING(logFile)))
#endif
   {
    throw std::runtime_error("unable to configure logging subsystem");
   }

#ifdef _DEBUG
   LOG_INFO(logger, TEXT(ODBC_DRIVER_NAME) TEXT(" (DEBUG) ") <<
            LOG4CPLUS_STRING_TO_TSTRING(ODBCDriver::GetVersion()));
#else
   LOG_INFO(logger, TEXT(ODBC_DRIVER_NAME) TEXT(" ") <<
            LOG4CPLUS_STRING_TO_TSTRING(ODBCDriver::GetVersion()));
#endif
  }
 }
 else if(fdwReason == DLL_THREAD_DETACH)
 {
  LoggerFactory::ThreadClean();
 }
 // DLL is being unloaded.
 else if(fdwReason == DLL_PROCESS_DETACH)
 {   
  // Any finalization global to DLL goes here.
  LoggerFactory::Clean();
 }

 return TRUE;
}

#elif defined(__GNUG__)

#include <dlfcn.h>

void __attribute__ ((constructor)) SharedLibraryLoad(void);
void __attribute__ ((destructor)) SharedLibraryUnload(void);

// Called when the library is loaded and before dlopen() returns.
void SharedLibraryLoad(void)
{
 // Add initialization code…

 // PLEASE NOTE!!! This function is called before any C++ static variable
 // initialized, do something here will cause segmentation fault, be carefully.

}

// Called when the library is unloaded and before dlclose() returns.
void SharedLibraryUnload(void)
{
}

#ifdef __cplusplus
extern "C" {
#endif

// unixODBC specified initialize function. See DriverManager/SQLConnection.c
SQLRETURN SQLDriverLoad(void)
{
 Logger logger = LoggerFactory::InitLogger();

#ifdef _DEBUG
 // http://stackoverflow.com/questions/1681060/
 // library-path-when-dynamically-loaded
 Dl_info dl_info;
 dladdr((void *)SharedLibraryLoad, &dl_info);

 std::string path(dl_info.dli_fname);
 std::string soDir = path.substr(0, path.find_last_of('/') + 1);
 std::string logFile(soDir);

 logFile.append(DEBUG_FILE);
#endif

 if(!LoggerFactory::ConfigLogger(TEXT(""),
#ifdef _DEBUG
      TEXT("DEBUG"), //LOG4CPLUS_STRING_TO_TSTRING(logLevel),
      LOG4CPLUS_STRING_TO_TSTRING(logFile)))
#else
      TEXT("ERROR"), //LOG4CPLUS_STRING_TO_TSTRING(logLevel),
      TEXT("")))//LOG4CPLUS_STRING_TO_TSTRING(logFile)))
#endif
 {
  throw std::runtime_error("unable to configure logging subsystem");
 }

#ifdef _DEBUG
 LOG_INFO(logger, TEXT(ODBC_DRIVER_NAME) TEXT(" (DEBUG) ") <<
          LOG4CPLUS_STRING_TO_TSTRING(ODBCDriver::GetVersion()));
#else
 LOG_INFO(logger, TEXT(ODBC_DRIVER_NAME) TEXT(" ") <<
          LOG4CPLUS_STRING_TO_TSTRING(ODBCDriver::GetVersion()));
#endif
}

// unixODBC specified uninitialize function. See DriverManager/SQLConnection.c
SQLRETURN SQLDriverUnload(void)
{
 // Add clean-up code…
 LoggerFactory::Clean();
}

#ifdef __cplusplus
}
#endif

#else
#endif
