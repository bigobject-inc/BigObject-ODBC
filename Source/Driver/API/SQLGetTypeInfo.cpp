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
https://msdn.microsoft.com/en-us/library/ms714632%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT StatementHandle, 
                                 SQLSMALLINT DataType)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(TEXT("%s: StatementHandle = 0x%08lX, DataType = %d"),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, DataType);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLGetTypeInfo, StatementHandle, DataType);

 /**************************
 * close any existing result
 **************************/
 helper.CloseResults();

 /**************************
 * retrieve the typeinfos resultset
 **************************/
 ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 try
 {
  serverRet = service.GetTypeInfo(
   ((HDRVDBC)hStmt->hDbc)->hDbcExtras->conn, DataType,
   &(hStmt->hStmtExtras->resultset));
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: SQLGetTypeInfo failed. %s"), ex.What());

  helper.FreeResults();
  API_HOOK_RETURN(SQL_ERROR);
 }

 assert(hStmt->hStmtExtras->resultset != NULL);

 /**************************
 * prepare the resultset for consumption
 **************************/
 SQLRETURN rc = helper.PrepareResults();

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);

 helper.LogSQLReturn(LOG_FUNCTION_NAME, rc);

 API_HOOK_RETURN(rc);
}
