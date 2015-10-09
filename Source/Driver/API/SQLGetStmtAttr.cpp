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
https://msdn.microsoft.com/en-us/library/ms715438%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLGetStmtAttr(
 SQLHSTMT StatementHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr,
 SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, Attribute = %d, ValuePtr = 0x%08lX, ")
  TEXT("BufferLength = %d, StringLengthPtr = 0x%08lX"), 
  LOG_FUNCTION_NAME, 
  (long)StatementHandle, Attribute, (long)ValuePtr,
  BufferLength, (long)StringLengthPtr);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 /* THIS TRICK IS STOLEN FROM SQLITE3 ODBC DRIVER. (we add static)*/
 static SQLINTEGER dummy = 0;
 static TCHAR dummybuf[16] = { 0 };

 if(!StringLengthPtr)
  StringLengthPtr = &dummy;

 if(!ValuePtr)
  ValuePtr = (SQLPOINTER)dummybuf;

 helper.Lock();

 API_HOOK_ENTRY(SQLGetStmtAttr,
  StatementHandle, Attribute, ValuePtr, BufferLength, StringLengthPtr);

 switch(Attribute)
 {
  case SQL_ATTR_IMP_ROW_DESC:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_IMP_ROW_DESC"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr, 
                        (SQLHDESC)&hStmt->hStmtExtras->ird);
   break;

  case SQL_ATTR_APP_ROW_DESC:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_APP_ROW_DESC"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        (SQLHDESC)&hStmt->hStmtExtras->ard);
   break;

  case SQL_ATTR_IMP_PARAM_DESC:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_IMP_PARAM_DESC"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        (SQLHDESC)&hStmt->hStmtExtras->ipd);
   break;

  case SQL_ATTR_APP_PARAM_DESC:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_APP_PARAM_DESC"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        (SQLHDESC)&hStmt->hStmtExtras->apd);
   break;

  case SQL_ATTR_ASYNC_ENABLE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ASYNC_ENABLE"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->asyncEnable);
   break;

  case SQL_ATTR_CONCURRENCY:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_CONCURRENCY"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->concurrency);
   break;

  case SQL_ATTR_CURSOR_SCROLLABLE:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_CURSOR_SCROLLABLE"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->cursorScroll);
   break;

  case SQL_ATTR_CURSOR_SENSITIVITY:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_CURSOR_SENSITIVITY"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->cursorSensitivity);
   break;

  case SQL_ATTR_CURSOR_TYPE:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_CURSOR_TYPE"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->cursorType);
   break;

  case SQL_ATTR_ENABLE_AUTO_IPD:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_ENABLE_AUTO_IPD"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->autoIPD);
   break;

  case SQL_ATTR_FETCH_BOOKMARK_PTR:
   LOG_WARN_FUNC(
    TEXT("Getting SQL_ATTR_FETCH_BOOKMARK_PTR: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_KEYSET_SIZE:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_KEYSET_SIZE"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->keysetSize);
   break;

  case SQL_ATTR_MAX_LENGTH:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_MAX_LENGTH: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_MAX_ROWS:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_MAX_ROWS: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_METADATA_ID:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_METADATA_ID"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->metaDataID);
   break;

  case SQL_ATTR_NOSCAN:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_NOSCAN"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->noScan);
   break;

  case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
   LOG_WARN_FUNC(
    TEXT("Getting SQL_ATTR_PARAM_BIND_OFFSET_PTR: ")
    TEXT("(NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_PARAM_BIND_TYPE:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_PARAM_BIND_TYPE: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_PARAM_OPERATION_PTR:
   LOG_WARN_FUNC(
    TEXT("Getting SQL_ATTR_PARAM_OPERATION_PTR: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_PARAM_STATUS_PTR:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_PARAM_STATUS_PTR: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_PARAMS_PROCESSED_PTR:
   LOG_WARN_FUNC(
    TEXT("Getting SQL_ATTR_PARAMS_PROCESSED_PTR: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_PARAMSET_SIZE:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_PARAMSET_SIZE: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_QUERY_TIMEOUT:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_QUERY_TIMEOUT"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->queryTimeout);
   break;

  case SQL_ATTR_RETRIEVE_DATA:
   LOG_DEBUG_FUNC(
    TEXT("Getting SQL_ATTR_RETRIEVE_DATA"));

   helper.CopyToSQLData(ValuePtr, StringLengthPtr,
                        hStmt->hStmtExtras->retrieveData);
   break;

  case SQL_ROWSET_SIZE:
  case SQL_ATTR_ROW_ARRAY_SIZE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ROW_ARRAY_SIZE"));

   _SQLGetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_ARRAY_SIZE, 
                   ValuePtr, BufferLength, StringLengthPtr);
   break;

  case SQL_ATTR_ROW_BIND_OFFSET_PTR:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ROW_BIND_OFFSET_PTR"));

   _SQLGetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_BIND_OFFSET_PTR,
                   ValuePtr, BufferLength, StringLengthPtr);
   break;

  case SQL_ATTR_ROW_BIND_TYPE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ROW_BIND_TYPE"));

   _SQLGetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_BIND_TYPE,
                   ValuePtr, BufferLength, StringLengthPtr);
   break;

  case SQL_ATTR_ROW_NUMBER:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_ROW_NUMBER: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_ROW_OPERATION_PTR:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ROW_OPERATION_PTR"));

   _SQLGetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_ARRAY_STATUS_PTR,
                   ValuePtr, BufferLength, StringLengthPtr);
   break;

  case SQL_ATTR_ROW_STATUS_PTR:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ROW_STATUS_PTR"));

   _SQLGetIRDField(&hStmt->hStmtExtras->ird, SQL_DESC_ARRAY_STATUS_PTR,
                   ValuePtr, BufferLength, StringLengthPtr);
   break;

  case SQL_ATTR_ROWS_FETCHED_PTR:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ROWS_FETCHED_PTR"));

   _SQLGetIRDField(&hStmt->hStmtExtras->ird, SQL_DESC_ROWS_PROCESSED_PTR,
                   ValuePtr, BufferLength, StringLengthPtr);
   break;

  case SQL_ATTR_SIMULATE_CURSOR:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_SIMULATE_CURSOR: (NOT IMPLEMENTED)"));
   break;

  case SQL_ATTR_USE_BOOKMARKS:
   LOG_WARN_FUNC(TEXT("Getting SQL_ATTR_USE_BOOKMARKS: (NOT IMPLEMENTED)"));
   break;

  default:
   LOG_ERROR_F_FUNC(TEXT("Getting Invalid Statement Attribute=%d"), Attribute);
   API_HOOK_RETURN(SQL_ERROR);
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
