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
https://msdn.microsoft.com/en-us/library/ms711683%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLColumns(
 SQLHSTMT StatementHandle, SQLTCHAR *CatalogName, SQLSMALLINT NameLength1, 
 SQLTCHAR *SchemaName, SQLSMALLINT NameLength2, 
 SQLTCHAR *TableName, SQLSMALLINT NameLength3, 
 SQLTCHAR *ColumnName, SQLSMALLINT NameLength4)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, CatalogName = %s, NameLength1 = %d, ")
  TEXT("SchemaName = %s, NameLength2 = %d, ")
  TEXT("TableName = %s, NameLength3 = %d, ")
  TEXT("ColumnName = %s, NameLength4 = %d"),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, CatalogName, NameLength1,
  SchemaName, NameLength2,
  TableName, NameLength3,
  ColumnName, NameLength4);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLColumns, 
  StatementHandle, CatalogName, NameLength1, SchemaName, NameLength2,
  TableName, NameLength3, ColumnName, NameLength4);

 /**************************
 * close any existing result
 **************************/
 helper.CloseResults();

 /**************************
 * Check the input arguments for proper behavior
 **************************/
 /* 
 #TODO: convert table search pattern and column search pattern
        from SQL pattern style to regex pattern style (not currently supported)
 */

 // If ColumnName is NULL, then assume caller wants to retrieve all columns 
 // in the table
 if(ColumnName == NULL)
 {
  ColumnName = (SQLTCHAR*)TEXT("*");
  NameLength4 = SQL_NTS;
 }

 /**************************
 * retrieve the tables resultset
 **************************/
 ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 try
 {
  serverRet = service.GetColumns(
   ((HDRVDBC)hStmt->hDbc)->hDbcExtras->conn, 
   helper.GetSafeTString(TableName, NameLength3), 
   helper.GetSafeTString(ColumnName, NameLength4),
   &(hStmt->hStmtExtras->resultset));
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: SQLColumns failed. %s"), ex.What());

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
