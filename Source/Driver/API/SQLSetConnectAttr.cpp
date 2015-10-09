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

/*
https://msdn.microsoft.com/en-us/library/ms713605%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLSetConnectAttrW(0x007c6310, 103, 0x0000000f, -5)	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC ConnectionHandle,
                                    SQLINTEGER Attribute,
                                    SQLPOINTER ValuePtr,
                                    SQLINTEGER StringLength)
{
 HDRVDBC hDbc = (HDRVDBC)ConnectionHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: ConnectionHandle = 0x%08lX, Attribute = %d, ")
  TEXT("ValuePtr = 0x%08lX, StringLength = %d"),
  LOG_FUNCTION_NAME, 
  (long)ConnectionHandle, Attribute, (long)ValuePtr, StringLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 helper.Lock();

 API_HOOK_ENTRY(SQLSetConnectAttr, 
  ConnectionHandle, Attribute, ValuePtr, StringLength);

 switch(Attribute)
 {
  case SQL_ATTR_ACCESS_MODE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ACCESS_MODE: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->accessMode = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_ASYNC_ENABLE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ASYNC_ENABLE: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->asyncEnable = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_AUTO_IPD: // read-only
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_AUTO_IPD: Value=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_AUTOCOMMIT:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_AUTOCOMMIT: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->autoCommit = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_CONNECTION_DEAD: // read-only
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_CONNECTION_DEAD: Value=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_CONNECTION_TIMEOUT:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_CONNECTION_TIMEOUT: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->timeOut = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_CURRENT_CATALOG:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_CURRENT_CATALOG: Value=%s (NOT IMPLEMENTED)"),
    (SQLTCHAR*)ValuePtr);
   break;

  case SQL_ATTR_LOGIN_TIMEOUT:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_LOGIN_TIMEOUT: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->loginTimeOut = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_METADATA_ID:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_METADATA_ID: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->metaDataID = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_ODBC_CURSORS:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ODBC_CURSORS: Value=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->odbcCursors = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_PACKET_SIZE:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PACKET_SIZE: Value=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_QUIET_MODE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_QUIET_MODE: Value=0x%08lX"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->window = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_TRACE: // only for driver manager
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_TRACE: Value=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_TRACEFILE: // only for driver manager
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_TRACEFILE: Value=%s (NOT IMPLEMENTED)"),
    (SQLTCHAR*)ValuePtr);
   break;

  case SQL_ATTR_TRANSLATE_LIB:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_TRANSLATE_LIB: Value=%s (NOT IMPLEMENTED)"),
    (SQLTCHAR*)ValuePtr);
   break;

  case SQL_ATTR_TRANSLATE_OPTION:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_TRANSLATE_OPTION: Value=%d (NOT IMPLEMENTED)"),
    (SQLINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_TXN_ISOLATION:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_TXN_ISOLATION: Value=%d"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->txnIsolation = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_MAX_ROWS:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_MAX_ROWS: Value=%d"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->maxRows = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_QUERY_TIMEOUT:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_QUERY_TIMEOUT: Value=%d"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hDbc->hDbcExtras->queryTimeout = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  default:
   LOG_WARN_F_FUNC(
    TEXT("Setting Invalid Connection Attribute=%d"), Attribute);
   API_HOOK_RETURN(SQL_SUCCESS);
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
