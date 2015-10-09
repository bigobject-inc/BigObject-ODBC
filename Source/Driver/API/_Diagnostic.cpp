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


#include "Driver.hpp"

SQLRETURN _SQLGetDiagRec(
 SQLSMALLINT nHandleType, SQLHANDLE hHandle,
 SQLSMALLINT nRecordNumber, SQLTCHAR * pszState,
 SQLINTEGER * pnNativeError, SQLTCHAR * pszMessageText,
 SQLSMALLINT nBufferLength, SQLSMALLINT * pnStringLength)
{
 /* sanity checks */
 if(!hHandle)
  return SQL_INVALID_HANDLE;

 /* clear return values */
 if(pszState)
  strcpy((char*)pszState, "-----");

 if(pnNativeError)
  *pnNativeError = 0;

 if(pszMessageText)
  memset(pszMessageText, 0, nBufferLength);

 if(pnStringLength)
  *pnStringLength = 0;

 switch(nHandleType)
 {
  case SQL_HANDLE_ENV:
   break;
  case SQL_HANDLE_DBC:
   break;
  case SQL_HANDLE_STMT:
   break;
  case SQL_HANDLE_DESC:
  default:
   return SQL_ERROR;
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

/**
Show a message immmediately to a user.

@param level Log level.
@param pMsgArgs Message formats.
@param ... Format arguments.
*/
void _SQLPopMsg(bigobject::Logging::LogLevel level, const TCHAR* pMsgArgs, ...)
{
 TCHAR s[4096]; // arbitary and maximum length of message.
 va_list args;

 // check if there is some message
 if(pMsgArgs) 
 {
  // convert to full msg including parsed params
  va_start(args, pMsgArgs);
  _vstprintf(s, 4096, pMsgArgs, args);
  va_end(args);
 }

 else
 {
  s[0] = 0;
 }

 if(level == bigobject::Logging::LEVEL_FATAL)
  LOG_FATAL_FUNC(s);
 else if(level == bigobject::Logging::LEVEL_ERROR)
  LOG_ERROR_FUNC(s);
 else if(level == bigobject::Logging::LEVEL_WARN)
  LOG_WARN_FUNC(s);
 else if(level == bigobject::Logging::LEVEL_INFO)
  LOG_INFO_FUNC(s);
 else // LEVEL_DEBUG
  LOG_DEBUG_FUNC(s);

#ifdef ODBC_DRIVER_QUIET_MODE
 // no thing
 LOG_FATAL_FUNC(TEXT("In quiet mode, Skip popping up window..."));
#elif WIN32
 {
  UINT flag = MB_OK;

  if(level == bigobject::Logging::LEVEL_FATAL ||
     level == bigobject::Logging::LEVEL_ERROR)
   flag |= MB_ICONERROR;
  else if(level == bigobject::Logging::LEVEL_WARN)
   flag |= MB_ICONWARNING;
  else if(level == bigobject::Logging::LEVEL_INFO)
   flag |= MB_ICONINFORMATION;
  else // LEVEL_DEBUG
   ;

  // show as window
  MessageBox(GetDesktopWindow(), s, TEXT(ODBC_DRIVER_NAME), flag);
 }
#else
 LOG_FATAL_FUNC(TEXT("_SQLPopMsg not implemented in this platform."));
#endif
}
