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
https://msdn.microsoft.com/en-us/library/ms716209%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLGetCursorName(
 SQLHSTMT StatementHandle, SQLTCHAR *CursorName,
 SQLSMALLINT BufferLength, SQLSMALLINT *NameLengthPtr)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 size_t ci; /* counter variable         */

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = 0x%08lX, CursorName = %s, ")
  TEXT("BufferLength = %d, NameLengthPtr = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, CursorName,
  BufferLength, NameLengthPtr);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 if(NULL == CursorName)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No cursor name."));
  return SQL_ERROR;
 }

 /*
  ** copy cursor name
  */
 _tcsncpy((TCHAR*)CursorName, (TCHAR*)hStmt->szCursorName, BufferLength);

 /*
  ** set length of transfered data
  */
 ci = _tcslen((TCHAR*)hStmt->szCursorName);
 /*
  if ( NULL != NameLengthPtr )
  *NameLengthPtr = MIN( ci, BufferLength );
  */
 if(BufferLength < ci)
 {
  LOG_INFO_FUNC(TEXT("SQL_SUCCESS_WITH_INFO: Cursor was truncated"));
  return SQL_SUCCESS_WITH_INFO;
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}
