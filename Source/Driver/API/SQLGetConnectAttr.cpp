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
https://msdn.microsoft.com/en-us/library/ms710297%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLGetConnectAttr(
 SQLHDBC ConnectionHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr,
 SQLINTEGER BufferLength, SQLINTEGER *StringLength)
{
 HDRVDBC hDbc = (HDRVDBC)ConnectionHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: ConnectionHandle = 0x%08lX, Attribute = %d, ")
  TEXT("ValuePtr = 0x%08lX, BufferLength = %d, StringLength = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)ConnectionHandle, Attribute, (long)ValuePtr, 
  BufferLength, (long)StringLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 helper.Lock();

 API_HOOK_ENTRY(SQLGetConnectAttr, 
  ConnectionHandle, Attribute, ValuePtr, BufferLength, StringLength);

 switch(Attribute)
 {
  case SQL_ATTR_ACCESS_MODE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ACCESS_MODE"));

   helper.CopyToSQLData(ValuePtr, StringLength, 
                        hDbc->hDbcExtras->accessMode);
   break;

  case SQL_ATTR_ASYNC_ENABLE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ASYNC_ENABLE"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->asyncEnable);
   break;

  case SQL_ATTR_AUTO_IPD:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_AUTO_IPD"));

   helper.CopyToSQLData(ValuePtr, StringLength, 
                        hDbc->hDbcExtras->autoIPD);
   break;

  case SQL_ATTR_AUTOCOMMIT:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_AUTOCOMMIT"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->autoCommit);
   break;

  case SQL_ATTR_CONNECTION_DEAD:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_CONNECTION_DEAD"));

   helper.CopyToSQLData(ValuePtr, StringLength,
    (hDbc->hDbcExtras->conn == NULL || !hDbc->bConnected) ? 1 : 0);
   break;

  case SQL_ATTR_CONNECTION_TIMEOUT:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_CONNECTION_TIMEOUT"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->timeOut);
   break;

  case SQL_ATTR_CURRENT_CATALOG:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_CURRENT_CATALOG: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_LOGIN_TIMEOUT:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_LOGIN_TIMEOUT"));

   helper.CopyToSQLData(ValuePtr, StringLength, 
                        hDbc->hDbcExtras->loginTimeOut);
   break;

  case SQL_ATTR_METADATA_ID:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_METADATA_ID"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->metaDataID);
   break;

  case SQL_ATTR_ODBC_CURSORS:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ODBC_CURSORS"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->odbcCursors);
   break;

  case SQL_ATTR_PACKET_SIZE:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_PACKET_SIZE: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_QUIET_MODE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_QUIET_MODE"));

   helper.CopyToSQLData(ValuePtr, StringLength, 
                        hDbc->hDbcExtras->window);
   break;

  case SQL_ATTR_TRACE: // basically for driver manager
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_TRACE: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_TRACEFILE: // basically for driver manager
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_TRACEFILE: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_TRANSLATE_LIB:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_TRANSLATE_LIB: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_TRANSLATE_OPTION:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_TRANSLATE_OPTION: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_TXN_ISOLATION:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_TXN_ISOLATION"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->txnIsolation);
   break;

  case SQL_ATTR_MAX_ROWS:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_MAX_ROWS"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->maxRows);
   break;

  case SQL_ATTR_QUERY_TIMEOUT:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_QUERY_TIMEOUT"));

   helper.CopyToSQLData(ValuePtr, StringLength,
                        hDbc->hDbcExtras->queryTimeout);
   break;

  default:
   LOG_ERROR_F_FUNC(TEXT("Getting Invalid Connection Attribute=%d"), 
                    Attribute);
   API_HOOK_RETURN(SQL_ERROR);
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
