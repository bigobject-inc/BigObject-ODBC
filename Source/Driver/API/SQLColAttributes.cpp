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
https://msdn.microsoft.com/en-us/library/ms711003%28v=vs.85%29.aspx

https://msdn.microsoft.com/en-us/library/ms710910%28v=vs.85%29.aspx

WE DON'T EXPORT THIS API.
*/

SQLRETURN SQL_API SQLColAttributes(
 SQLHSTMT hDrvStmt, SQLUSMALLINT nCol, SQLUSMALLINT nDescType,
 SQLPOINTER pszDesc, SQLSMALLINT nDescMax, SQLSMALLINT *pcbDesc,
 SQLLEN *pfDesc)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvStmt = 0x%08lX, nCol = %d, nDescType = %d, pszDesc = 0x%08lX,")
  TEXT(" nDescMax = %d, pcbDesc = 0x%08lX, pfDesc = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)hDrvStmt, nCol, nDescType, (long)pszDesc,
  nDescMax, (long)pcbDesc, (long)pfDesc);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 /**************************
  * 1. verify we have result set
  * 2. verify col is within range
  **************************/
 if(hStmt->hStmtExtras->ird.rowDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Uninitialized column headers."));
  return SQL_ERROR;
 }

 if(nCol < 1 || nCol > hStmt->hStmtExtras->ird.descCount)
 {
  LOG_ERROR_F_FUNC(TEXT("Column out of bounds : %u"), nCol);
  return SQL_ERROR;
 }

 API_HOOK_ENTRY(SQLColAttributes,
  hDrvStmt, nCol, nDescType, pszDesc, nDescMax, pcbDesc, pfDesc);

 /**************************
  * 3. process request
  **************************/
 switch(nDescType)
 {
  // enum these
  /*
   case SQL_COLUMN_AUTO_INCREMENT:
   case SQL_COLUMN_CASE_SENSITIVE: // *
   case SQL_COLUMN_COUNT: // *
   case SQL_COLUMN_DISPLAY_SIZE: // *
   case SQL_COLUMN_LENGTH: // *
   case SQL_COLUMN_MONEY:
   case SQL_COLUMN_NULLABLE: // *
   case SQL_COLUMN_PRECISION: // *
   case SQL_COLUMN_SCALE: // *
   case SQL_COLUMN_SEARCHABLE: // *
   case SQL_COLUMN_TYPE: // *
   case SQL_COLUMN_UNSIGNED: // *
   case SQL_COLUMN_UPDATABLE: // *
   case SQL_COLUMN_CATALOG_NAME: // ?
   case SQL_COLUMN_QUALIFIER_NAME: // *
   case SQL_COLUMN_DISTINCT_TYPE: // ?
   case SQL_COLUMN_LABEL: // *
   case SQL_COLUMN_NAME: // *
   case SQL_COLUMN_SCHEMA_NAME: // ?
   case SQL_COLUMN_OWNER_NAME:
   case SQL_COLUMN_TABLE_NAME: // *
   case SQL_COLUMN_TYPE_NAME: // *
  */
  // *: Migrated to SQLColAttribute.
  // ?: Undefined?

  default:
   LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: nDescType = %d"), 
    LOG_FUNCTION_NAME, nDescType);
   return SQL_ERROR;
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 return SQL_ERROR;
}
