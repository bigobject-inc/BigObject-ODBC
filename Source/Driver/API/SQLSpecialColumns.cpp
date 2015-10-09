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
https://msdn.microsoft.com/en-us/library/ms714602%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLSpecialColumns(
 SQLHSTMT StatementHandle, SQLSMALLINT IdentifierType, 
 SQLTCHAR *CatalogName, SQLSMALLINT NameLength1, 
 SQLTCHAR *SchemaName, SQLSMALLINT NameLength2, 
 SQLTCHAR *TableName, SQLSMALLINT NameLength3, 
 SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, IdentifierType = %d, ")
  TEXT("CatalogName = %s, NameLength1 = %d, ")
  TEXT("SchemaName = %s, NameLength2 = %d, ")
  TEXT("TableName = %s, NameLength3 = %d, ")
  TEXT("Scope = %d, Nullable = %d, "),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, IdentifierType,
  CatalogName, NameLength1,
  SchemaName, NameLength2,
  TableName, NameLength3,
  Scope, Nullable);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLSpecialColumns,
  StatementHandle, CatalogName, NameLength1, SchemaName, NameLength2,
  TableName, NameLength3, Scope, Nullable);

 /************************
  * REPLACE THIS COMMENT WITH SOMETHING USEFULL
  ************************/
 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_ERROR);
}
