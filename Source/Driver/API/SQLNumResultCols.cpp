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
https://msdn.microsoft.com/en-us/library/ms715393%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLNumResultCols ( 0x065bf898, 0x0658ec1c )	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT StatementHandle, 
                                   SQLSMALLINT *ColumnCountPtr)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, ColumnCountPtr = 0x%08lX"),
  LOG_FUNCTION_NAME, (long)StatementHandle, (long)ColumnCountPtr);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLNumResultCols, StatementHandle, ColumnCountPtr);

 if(NULL == ColumnCountPtr)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: ColumnCountPtr can not be NULL."));
  API_HOOK_RETURN(SQL_ERROR);
 }

 ///********************
 // * get number of columns in result set
 // ********************/
 *ColumnCountPtr = (SQLSMALLINT)hStmt->hStmtExtras->ird.descCount;

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
