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
https://msdn.microsoft.com/en-us/library/ms713611%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLExecDirectW(0x065bf898, 0x020799d4, -3)	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT StatementHandle, 
                                SQLTCHAR *StatementText, 
                                SQLINTEGER TextLength)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;
 RETCODE rc;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, StatementText = %s, TextLength = %d"), 
  LOG_FUNCTION_NAME, 
  (long)StatementHandle, StatementText, TextLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLExecDirect, StatementHandle, StatementText, TextLength);

 /* prepare command, avoid finding the driver manager SQLPrepare */
 rc = helper.Prepare(StatementText, TextLength);
 if(SQL_SUCCESS != rc)
 {
  LOG_ERROR_FUNC(TEXT("Could not prepare statement."));
  API_HOOK_RETURN(rc);
 }

 /* execute command */
 rc = helper.Execute();
 if(SQL_SUCCESS != rc)
 {
  LOG_ERROR_FUNC(TEXT("Problem calling SQLExecute."));
  API_HOOK_RETURN(rc);
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);
 
 API_HOOK_RETURN(SQL_SUCCESS);
}
