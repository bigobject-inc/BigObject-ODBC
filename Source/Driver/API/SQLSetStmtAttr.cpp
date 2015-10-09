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
https://msdn.microsoft.com/en-us/library/ms712631%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLSetStmtAttrW(0x065bf898, 0, 0x0000001e, -5)	SQL_SUCCESS
SQLSetStmtAttrW ( 0x065bf898, 1228, NULL, -5 )	SQL_ERROR
SQLSetStmtAttrW ( 0x065bf898, 1227, NULL, -5 )	SQL_ERROR
*/

SQLRETURN SQL_API SQLSetStmtAttr(
 SQLHSTMT StatementHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr,
 SQLINTEGER StringLength)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, Attribute = %d, ")
  TEXT("ValuePtr = 0x%08lX, StringLength = %d"),
  LOG_FUNCTION_NAME, 
  (long)StatementHandle, Attribute, (long)ValuePtr, StringLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 helper.Lock();

 API_HOOK_ENTRY(SQLSetStmtAttr,
  StatementHandle, Attribute, ValuePtr, StringLength);

 switch(Attribute)
 {
  // HY017
  case SQL_ATTR_APP_PARAM_DESC:
  case SQL_ATTR_APP_ROW_DESC:
  case SQL_ATTR_IMP_PARAM_DESC:
  case SQL_ATTR_IMP_ROW_DESC:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting APD, ARD, IPD, IRD: ValuePtr=$%08lX ")
    TEXT("(Cannot set descriptor.)"),
    ValuePtr);

   API_HOOK_RETURN(SQL_ERROR);

  case SQL_ATTR_ASYNC_ENABLE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ASYNC_ENABLE: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->asyncEnable = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_CONCURRENCY:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_CONCURRENCY: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->concurrency = (SQLUINTEGER)((std::size_t)ValuePtr);

   if(hStmt->hStmtExtras->concurrency == SQL_CONCUR_READ_ONLY)
    hStmt->hStmtExtras->cursorSensitivity = SQL_INSENSITIVE;
   else
    hStmt->hStmtExtras->cursorSensitivity = SQL_UNSPECIFIED;

   break;

  case SQL_ATTR_CURSOR_SCROLLABLE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_CURSOR_SCROLLABLE: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->cursorScroll = (SQLUINTEGER)((std::size_t)ValuePtr);

   if(hStmt->hStmtExtras->cursorScroll == SQL_NONSCROLLABLE)
    hStmt->hStmtExtras->cursorType = SQL_CURSOR_FORWARD_ONLY;
   else
    hStmt->hStmtExtras->cursorType = SQL_CURSOR_STATIC;

   break;

  case SQL_ATTR_CURSOR_SENSITIVITY:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_CURSOR_SENSITIVITY: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->cursorSensitivity = (SQLUINTEGER)((std::size_t)ValuePtr);

   if(hStmt->hStmtExtras->cursorSensitivity == SQL_INSENSITIVE)
   {
    hStmt->hStmtExtras->concurrency = SQL_CONCUR_READ_ONLY;
    hStmt->hStmtExtras->cursorType = SQL_CURSOR_STATIC;
   }
   else if(hStmt->hStmtExtras->cursorSensitivity == SQL_SENSITIVE)
   {
    hStmt->hStmtExtras->concurrency = SQL_CONCUR_ROWVER;
    hStmt->hStmtExtras->cursorType = SQL_CURSOR_STATIC;
   }
   else // if ( cursorSensitivity == SQL_UNSPECIFIED )
   {
    hStmt->hStmtExtras->concurrency = SQL_CONCUR_READ_ONLY;
    hStmt->hStmtExtras->cursorType = SQL_CURSOR_FORWARD_ONLY;
   }

   break;

  case SQL_ATTR_CURSOR_TYPE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_CURSOR_TYPE: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->cursorType = (SQLUINTEGER)((std::size_t)ValuePtr);

   if(hStmt->hStmtExtras->cursorType == SQL_CURSOR_DYNAMIC)
   {
    hStmt->hStmtExtras->cursorScroll = SQL_SCROLLABLE;
    if(hStmt->hStmtExtras->concurrency != SQL_CONCUR_READ_ONLY)
     hStmt->hStmtExtras->cursorSensitivity = SQL_SENSITIVE;
   }
   else if(hStmt->hStmtExtras->cursorType == SQL_CURSOR_FORWARD_ONLY)
   {
    hStmt->hStmtExtras->cursorScroll = SQL_NONSCROLLABLE;
   }
   else if(hStmt->hStmtExtras->cursorType == SQL_CURSOR_KEYSET_DRIVEN)
   {
    hStmt->hStmtExtras->cursorScroll = SQL_SCROLLABLE;
    if(hStmt->hStmtExtras->concurrency != SQL_CONCUR_READ_ONLY)
     hStmt->hStmtExtras->cursorSensitivity = SQL_UNSPECIFIED;
   }
   else if(hStmt->hStmtExtras->cursorType == SQL_CURSOR_STATIC)
   {
    hStmt->hStmtExtras->cursorScroll = SQL_SCROLLABLE;
    if(hStmt->hStmtExtras->concurrency != SQL_CONCUR_READ_ONLY)
     hStmt->hStmtExtras->cursorSensitivity = SQL_UNSPECIFIED;
    else
     hStmt->hStmtExtras->cursorSensitivity = SQL_INSENSITIVE;
   }

   break;

  case SQL_ATTR_ENABLE_AUTO_IPD:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ENABLE_AUTO_IPD: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->autoIPD = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_FETCH_BOOKMARK_PTR:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_FETCH_BOOKMARK_PTR: ValuePtr=$%08lX ")
    TEXT("(NOT IMPLEMENTED)"), 
    (SQLLEN*)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_KEYSET_SIZE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_KEYSET_SIZE: ValuePtr=%u"), 
    (SQLULEN)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->keysetSize = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_MAX_LENGTH:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_MAX_LENGTH: ValuePtr=%u (NOT IMPLEMENTED)"), 
    (SQLULEN)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_MAX_ROWS:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_MAX_ROWS: ValuePtr=%u (NOT IMPLEMENTED)"), 
    (SQLULEN)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_METADATA_ID:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_METADATA_ID: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->metaDataID = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_NOSCAN:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_NOSCAN: ValuePtr=%u"), 
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->noScan = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PARAM_BIND_OFFSET_PTR: ValuePtr=$%08lX ")
    TEXT("(NOT IMPLEMENTED)"), 
    (SQLULEN*)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_PARAM_BIND_TYPE:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PARAM_BIND_TYPE: ValuePtr=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_PARAM_OPERATION_PTR:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PARAM_OPERATION_PTR: ValuePtr=$%08lX ")
    TEXT("(NOT IMPLEMENTED)"),
    (SQLUSMALLINT*)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_PARAM_STATUS_PTR:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PARAM_STATUS_PTR: ValuePtr=$%08lX ")
    TEXT("(NOT IMPLEMENTED)"),
    (SQLUSMALLINT*)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_PARAMS_PROCESSED_PTR:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PARAMS_PROCESSED_PTR: ValuePtr=$%08lX ")
    TEXT("(NOT IMPLEMENTED)"),
    (SQLULEN*)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_PARAMSET_SIZE:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_PARAMSET_SIZE: ValuePtr=%u (NOT IMPLEMENTED)"),
    (SQLULEN)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_QUERY_TIMEOUT:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_QUERY_TIMEOUT: ValuePtr=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->queryTimeout = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ATTR_RETRIEVE_DATA:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_RETRIEVE_DATA: ValuePtr=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   hStmt->hStmtExtras->retrieveData = (SQLUINTEGER)((std::size_t)ValuePtr);
   break;

  case SQL_ROWSET_SIZE:
  case SQL_ATTR_ROW_ARRAY_SIZE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ROW_ARRAY_SIZE: ValuePtr=%u"),
    (SQLULEN)((std::size_t)ValuePtr));

   _SQLSetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_ARRAY_SIZE, 
                   ValuePtr, StringLength);
   break;

  case SQL_ATTR_ROW_BIND_OFFSET_PTR:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ROW_BIND_OFFSET_PTR: ValuePtr=$%08lX"),
    (SQLULEN*)((std::size_t)ValuePtr));

   _SQLSetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_BIND_OFFSET_PTR,
                   ValuePtr, StringLength);
   break;

  case SQL_ATTR_ROW_BIND_TYPE:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ROW_BIND_TYPE: ValuePtr=%u"),
    (SQLUINTEGER)((std::size_t)ValuePtr));

   _SQLSetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_BIND_TYPE,
                   ValuePtr, StringLength);
   break;

  case SQL_ATTR_ROW_NUMBER:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_ROW_NUMBER: ValuePtr=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_ROW_OPERATION_PTR:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ROW_OPERATION_PTR: ValuePtr=$%08lX"),
    (SQLUSMALLINT*)((std::size_t)ValuePtr));

   _SQLSetARDField(&hStmt->hStmtExtras->ard, SQL_DESC_ARRAY_STATUS_PTR,
                   ValuePtr, StringLength);
   break;

  case SQL_ATTR_ROW_STATUS_PTR:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ROW_STATUS_PTR: ValuePtr=$%08lX"),
    (SQLUSMALLINT*)((std::size_t)ValuePtr));

   _SQLSetIRDField(&hStmt->hStmtExtras->ird, SQL_DESC_ARRAY_STATUS_PTR,
                   ValuePtr, StringLength);
   break;

  case SQL_ATTR_ROWS_FETCHED_PTR:
   LOG_DEBUG_F_FUNC(
    TEXT("Setting SQL_ATTR_ROWS_FETCHED_PTR: ValuePtr=$%08lX"),
    (SQLULEN*)((std::size_t)ValuePtr));

   _SQLSetIRDField(&hStmt->hStmtExtras->ird, SQL_DESC_ROWS_PROCESSED_PTR,
                   ValuePtr, StringLength);
   break;

  case SQL_ATTR_SIMULATE_CURSOR:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_SIMULATE_CURSOR: ValuePtr=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  case SQL_ATTR_USE_BOOKMARKS:
   LOG_WARN_F_FUNC(
    TEXT("Setting SQL_ATTR_USE_BOOKMARKS: ValuePtr=%u (NOT IMPLEMENTED)"),
    (SQLUINTEGER)((std::size_t)ValuePtr));
   break;

  default:
   LOG_WARN_F_FUNC(TEXT("Setting Invalid Statement Attribute=%d"), Attribute);
   API_HOOK_RETURN(SQL_ERROR);
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
