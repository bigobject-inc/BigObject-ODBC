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

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT hDrvStmt, SQLTCHAR *szCursor,
                                   SQLSMALLINT nCursorLength)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 /* SANITY CHECKS */
 if(hStmt == SQL_NULL_HSTMT)
  return SQL_INVALID_HANDLE;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = $%08lX"), LOG_FUNCTION_NAME, (long)hStmt);

 if( NULL == szCursor || 0 == isalpha(*szCursor))
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Invalid cursor name."));
  return SQL_ERROR;
 }

 /* COPY CURSOR */
 if(SQL_NTS == nCursorLength)
 {
  strncpy((char*)hStmt->szCursorName, (char*)szCursor, SQL_MAX_CURSOR_NAME);
 }
 else
 {
  /*
   strncpy( hStmt->szCursorName, szCursor, MIN(SQL_MAX_CURSOR_NAME - 1, nCursorLength) );
   hStmt->szCursorName[ MIN( SQL_MAX_CURSOR_NAME - 1, nCursorLength) ] = '\0';
   */
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}
