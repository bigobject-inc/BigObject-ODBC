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

SQLRETURN _Prepare(SQLHSTMT hDrvStmt, SQLTCHAR *szSqlStr, 
                   SQLINTEGER nSqlStrLength)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvStmt = 0x%08lX, szSqlStr = %s, nSqlStrLength = %d"),
  LOG_FUNCTION_NAME, (long)hDrvStmt, szSqlStr, nSqlStrLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 return helper.Prepare(szSqlStr, nSqlStrLength);
}

/*
https://msdn.microsoft.com/en-us/library/ms710926%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLPrepare(SQLHSTMT StatementHandle, SQLTCHAR *StatementText, 
                             SQLINTEGER TextLength)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, StatementText = %s, ")
  TEXT("TextLength = %d"),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, StatementText,
  TextLength);

 API_HOOK_ENTRY(SQLPrepare, StatementHandle, StatementText, TextLength);
 
 API_HOOK_RETURN(_Prepare(StatementHandle, StatementText, TextLength));
}
