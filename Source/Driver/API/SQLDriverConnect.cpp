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


#include <boost/algorithm/string.hpp>

#include <boost/lexical_cast.hpp>

#include "Driver.hpp"

/*
https://msdn.microsoft.com/en-us/library/ms715433%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLDriverConnectW(0x007c6310, NULL, 0x020441e8, -3, NULL, 0, 0x0658eb4c, 
                  SQL_DRIVER_NOPROMPT)	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLDriverConnect(
 SQLHDBC ConnectionHandle, SQLHWND WindowHandle, SQLTCHAR *InConnectionString,
 SQLSMALLINT StringLength1, SQLTCHAR *OutConnectionString,
 SQLSMALLINT BufferLength, SQLSMALLINT *StringLength2Ptr,
 SQLUSMALLINT DriverCompletion)
{
 HDRVDBC hDbc = (HDRVDBC)ConnectionHandle;
 TCHAR szDSN[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szDRIVER[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szUID[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szPWD[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szDATABASE[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szHOST[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szPORT[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szLOGLEVEL[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");
 TCHAR szLOGFILE[INI_MAX_PROPERTY_VALUE + 1] = TEXT("");

 LOG_DEBUG_F_FUNC(
  TEXT("%s: ConnectionHandle = 0x%08lX, WindowHandle = 0x%08lX, ")
  TEXT("InConnectionString = %s, StringLength1 = %d, ")
  TEXT("OutConnectionString = 0x%08lX, BufferLength = %d, ")
  TEXT("StringLength2Ptr = 0x%08lX, DriverCompletion = %d"),
  LOG_FUNCTION_NAME,
  (long)ConnectionHandle, (long)WindowHandle,
  (TCHAR*)InConnectionString, StringLength1,
  (long)OutConnectionString, BufferLength,
  (long)StringLength2Ptr, DriverCompletion);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 if(hDbc->bConnected == 1)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Already connected."));
  return SQL_ERROR;
 }

 if(!InConnectionString)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Bad argument."));
  return SQL_ERROR;
 }

 API_HOOK_ENTRY(SQLDriverConnect,
  ConnectionHandle, WindowHandle, InConnectionString, StringLength1, 
  OutConnectionString, BufferLength, StringLength2Ptr, DriverCompletion);

 switch(DriverCompletion)
 {
  case SQL_DRIVER_PROMPT:
  case SQL_DRIVER_COMPLETE:
  case SQL_DRIVER_COMPLETE_REQUIRED:
  case SQL_DRIVER_NOPROMPT:
   LOG_DEBUG_F_FUNC(TEXT("DriverCompletion=%d (NOT IMPLEMENTED)"), 
                    DriverCompletion);
   break;

  default:
   LOG_DEBUG_F_FUNC(TEXT("Invalid DriverCompletion=%d"), DriverCompletion);
   break;
 }

 /*************************
 * 1. parse StringLength1 for connection options. Format is;
 *      Property=Value;...
 * 2. we may not have all options so handle as per DM request
 * 3. fill as required OutConnectionString
 *************************/
 tstring connStr = helper.GetSafeTString(InConnectionString, StringLength1);

 ConnectionStringParser::ConnectionStringMapPtr pt = 
  ConnectionStringParser::Parse(connStr);

 if(!pt)
 {
  LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: Failed parse connection string: %s."), 
                   connStr.c_str());

  API_HOOK_RETURN(SQL_ERROR);
 }

 ConnectionStringParser::ConnectionStringMap::iterator i = pt->begin();
 ConnectionStringParser::ConnectionStringMap::iterator iEnd = pt->end();
 for(; i != iEnd; ++i)
 {
  const tstring& name = i->first;
  const tstring& value = i->second;

  if(boost::iequals(name, "DSN"))
   _tcsncpy(szDSN, value.c_str(), sizeof(szDSN) / sizeof(TCHAR));
  else if(boost::iequals(name, "DRIVER"))
   _tcsncpy(szDRIVER, value.c_str(), sizeof(szDRIVER) / sizeof(TCHAR));
  else if(boost::iequals(name, "UID"))
   _tcsncpy(szUID, value.c_str(), sizeof(szUID) / sizeof(TCHAR));
  else if(boost::iequals(name, "PWD"))
   _tcsncpy(szPWD, value.c_str(), sizeof(szPWD) / sizeof(TCHAR));
  else if(boost::iequals(name, ODBC_DRIVER_CONNSTR_HOST_KEY))
   _tcsncpy(szHOST, value.c_str(), sizeof(szHOST) / sizeof(TCHAR));
  else if(boost::iequals(name, ODBC_DRIVER_CONNSTR_DATABASE_KEY))
   _tcsncpy(szDATABASE, value.c_str(), sizeof(szDATABASE) / sizeof(TCHAR));
  else if(boost::iequals(name, ODBC_DRIVER_CONNSTR_PORT_KEY))
   _tcsncpy(szPORT, value.c_str(), sizeof(szPORT) / sizeof(TCHAR));
  else if(boost::iequals(name, ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY))
   _tcsncpy(szLOGLEVEL, value.c_str(), sizeof(szLOGLEVEL) / sizeof(TCHAR));
  else if(boost::iequals(name, ODBC_DRIVER_CONNSTR_LOGFILE_KEY))
   _tcsncpy(szLOGFILE, value.c_str(), sizeof(szLOGFILE) / sizeof(TCHAR));
 }

 /****************************
 * Fill in values that were omitted from the connection string 
 * (augment only, no overwrites)
 ***************************/
 // Can retrieve system data source info only if DRIVER is not defined.
 // (ODBC API Spec)
 if(szDRIVER[0] == '\0') 
 {
  // If DSN is DEFAULT or DSN is not provided, 
  // then replace it with the default DSN name.
  if(szDSN[0] == '\0' || boost::iequals(szDSN, "DEFAULT"))
  {
   _tcsncpy(szDSN, TEXT(ODBC_DRIVER_DEFAULT_DSN), 
            sizeof(szDSN) / sizeof(TCHAR));
  }

  if(szUID[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT("UID"),
    TEXT(ODBC_DRIVER_DEFAULT_UID),
    szUID, sizeof(szUID) / sizeof(TCHAR),
    TEXT("odbc.ini"));
  }
  if(szPWD[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT("PWD"),
    TEXT(ODBC_DRIVER_DEFAULT_PWD),
    szPWD, sizeof(szPWD) / sizeof(TCHAR),
    TEXT("odbc.ini"));
  }


  if(szHOST[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT(ODBC_DRIVER_CONNSTR_HOST_KEY),
                              TEXT(ODBC_DRIVER_DEFAULT_HOST), 
                              szHOST, sizeof(szHOST) / sizeof(TCHAR), 
                              TEXT("odbc.ini"));
  }
  if(szDATABASE[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT(ODBC_DRIVER_CONNSTR_DATABASE_KEY),
                              TEXT(ODBC_DRIVER_DEFAULT_DATABASE),
                              szDATABASE, sizeof(szDATABASE) / sizeof(TCHAR),
                              TEXT("odbc.ini"));
  }
  if(szPORT[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT(ODBC_DRIVER_CONNSTR_PORT_KEY),
                              TEXT(ODBC_DRIVER_DEFAULT_PORT), 
                              szPORT, sizeof(szPORT) / sizeof(TCHAR), 
                              TEXT("odbc.ini"));
  }
  if(szLOGLEVEL[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT(ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY),
    TEXT(ODBC_DRIVER_DEFAULT_LOGLEVEL),
    szLOGLEVEL, sizeof(szLOGLEVEL) / sizeof(TCHAR),
    TEXT("odbc.ini"));
  }
  if(szLOGFILE[0] == '\0')
  {
   SQLGetPrivateProfileString(szDSN, TEXT(ODBC_DRIVER_CONNSTR_LOGFILE_KEY),
    TEXT(ODBC_DRIVER_DEFAULT_LOGFILE),
    szLOGFILE, sizeof(szLOGFILE) / sizeof(TCHAR),
    TEXT("odbc.ini"));
  }
 }

 // If missing any remaining connection values, then just assume 
 // default values.
 if(szUID[0] == '\0')
 {
  _tcsncpy(szUID, TEXT(ODBC_DRIVER_DEFAULT_UID),
   sizeof(szUID) / sizeof(TCHAR));
 }

 if(szPWD[0] == '\0')
 {
  _tcsncpy(szPWD, TEXT(ODBC_DRIVER_DEFAULT_PWD),
   sizeof(szPWD) / sizeof(TCHAR));
 }


 if(szHOST[0] == '\0')
 {
  _tcsncpy(szHOST, TEXT(ODBC_DRIVER_DEFAULT_HOST), 
           sizeof(szHOST) / sizeof(TCHAR));
 }

 if(szDATABASE[0] == '\0')
 {
  _tcsncpy(szDATABASE, TEXT(ODBC_DRIVER_DEFAULT_DATABASE), 
           sizeof(szDATABASE) / sizeof(TCHAR));
 }

 if(szPORT[0] == '\0')
 {
  _tcsncpy(szPORT, TEXT(ODBC_DRIVER_DEFAULT_PORT), 
           sizeof(szPORT) / sizeof(TCHAR));
 }

 if(szLOGLEVEL[0] == '\0')
 {
  _tcsncpy(szLOGLEVEL, TEXT(ODBC_DRIVER_DEFAULT_LOGLEVEL),
   sizeof(szLOGLEVEL) / sizeof(TCHAR));
 }

 if(szLOGFILE[0] == '\0')
 {
  _tcsncpy(szLOGFILE, TEXT(ODBC_DRIVER_DEFAULT_LOGFILE),
   sizeof(szLOGFILE) / sizeof(TCHAR));
 }

 // Reconfigure logging settings.
 bool reconfigure = false;

 tstring currentLogLevel = bigobject::Logging::LoggerFactory::GetLogLevel();
 tstring logLevel(szLOGLEVEL);
 if(currentLogLevel != logLevel)
 {
  LOG_INFO_F_FUNC(TEXT("RESET LOG LEVEL: %s"), szLOGLEVEL);
  reconfigure = true;
 }

 tstring logFile(szLOGFILE);
#ifdef _DEBUG
 if(logFile != TEXT(DEBUG_FILE) && !logFile.empty())
#else
 if(logFile != TEXT(DEBUG_FILE))
#endif
 {
  LOG_INFO_F_FUNC(TEXT("RESET LOG FILE: %s"), szLOGFILE);
  reconfigure = true;
 }

 if(reconfigure)
 {
  if(!bigobject::Logging::LoggerFactory::ConfigLogger(TEXT(""), logLevel, logFile))
  {
   LOG_ERROR_FUNC(TEXT("Unable to configure logging subsystem."));
   // Nothing, pass it.
  }
 }

 /****************************
 * return the connect string we are using
 ***************************/
 if(OutConnectionString)
 {
  int bytesToWrite;

  if(szDRIVER[0] != '\0')
  {
   bytesToWrite = _sntprintf(
    (TCHAR*)OutConnectionString, BufferLength,
    TEXT("DRIVER=%s;UID=%s;PWD=%s;%s=%s;%s=%s;%s=%s;%s=%s;%s=%s"),
    szDRIVER, szUID, szPWD,
    TEXT(ODBC_DRIVER_CONNSTR_HOST_KEY), szHOST, 
    TEXT(ODBC_DRIVER_CONNSTR_DATABASE_KEY), szDATABASE,
    TEXT(ODBC_DRIVER_CONNSTR_PORT_KEY), szPORT,
    TEXT(ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY), szLOGLEVEL,
    TEXT(ODBC_DRIVER_CONNSTR_LOGFILE_KEY), szLOGFILE);
  }
  else
  {
   bytesToWrite = _sntprintf(
    (TCHAR*)OutConnectionString, BufferLength,
    TEXT("DSN=%s;UID=%s;PWD=%s;%s=%s;%s=%s;%s=%s;%s=%s;%s=%s"),
    szDSN, szUID, szPWD,
    TEXT(ODBC_DRIVER_CONNSTR_HOST_KEY), szHOST,
    TEXT(ODBC_DRIVER_CONNSTR_DATABASE_KEY), szDATABASE,
    TEXT(ODBC_DRIVER_CONNSTR_PORT_KEY), szPORT,
    TEXT(ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY), szLOGLEVEL,
    TEXT(ODBC_DRIVER_CONNSTR_LOGFILE_KEY), szLOGFILE);
  }

  if(StringLength2Ptr)
   *StringLength2Ptr = bytesToWrite;
 }

 /*************************
 * 4. try to connect
 * 5. set gathered options (ie USE Database or whatever)
 * 6. set connection state
 *      hDbc->bConnected = TRUE;
 *************************/
 helper.Lock();

 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 try
 {
  hDbc->hDbcExtras->conn = service.OpenConnection(
   hDbc, tstring(szDATABASE), 
   tstring(szHOST), boost::lexical_cast<int>(szPORT),
   tstring(szUID), tstring(szPWD));
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR Failed to connect to (%s:%s): %s"), 
   szHOST, szPORT, ex.What());

  API_HOOK_RETURN(SQL_ERROR);
 }

 if(hDbc->hDbcExtras->conn == NULL)
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR Failed to connect to (%s:%s)."), szHOST, szPORT);

  API_HOOK_RETURN(SQL_ERROR);
 }
 else
 {
  hDbc->bConnected = 1;

  /* Remember the successful connection settings. */
  if(hDbc->hDbcExtras->odbcSettings == NULL)
  {
   ODBCSettings* conn = (ODBCSettings*)malloc(sizeof(ODBCSettings));

   memset(conn, 0, sizeof(ODBCSettings));

   _tcsncpy(conn->driver, szDRIVER, sizeof(szDRIVER) / sizeof(TCHAR));
   _tcsncpy(conn->dsn, szDSN, sizeof(szDSN) / sizeof(TCHAR));
   _tcsncpy(conn->uid, szUID, sizeof(szUID) / sizeof(TCHAR));
   _tcsncpy(conn->password, szPWD, sizeof(szPWD) / sizeof(TCHAR));

   _tcsncpy(conn->database, szDATABASE, sizeof(szDATABASE) / sizeof(TCHAR));
   _tcsncpy(conn->host, szHOST, sizeof(szHOST) / sizeof(TCHAR));
   _tcsncpy(conn->port, szPORT, sizeof(szPORT) / sizeof(TCHAR));
   _tcsncpy(conn->logLevel, szLOGLEVEL, sizeof(szLOGLEVEL) / sizeof(TCHAR));
   _tcsncpy(conn->logFile, szLOGFILE, sizeof(szLOGFILE) / sizeof(TCHAR));

   hDbc->hDbcExtras->odbcSettings = conn;
  }
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
