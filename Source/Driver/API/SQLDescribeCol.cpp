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
https://msdn.microsoft.com/en-us/library/ms716289%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLDescribeCol(
 SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, 
 SQLTCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLengthPtr,
 SQLSMALLINT *DataTypePtr, SQLULEN *ColumnSizePtr,
 SQLSMALLINT *DecimalDigitsPtr, SQLSMALLINT *NullablePtr)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;
 COLUMNHDR *columnHeader;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, ColumnNumber = %d, ")
  TEXT("ColumnName = %s, BufferLength = %d, NameLengthPtr = 0x%08lX, ")
  TEXT("DataTypePtr = 0x%08lX, ColumnSizePtr = 0x%08lX, ")
  TEXT("DecimalDigitsPtr = 0x%08lX, NullablePtr = 0x%08lX"),
  LOG_FUNCTION_NAME, 
  (long)StatementHandle, ColumnNumber,
  ColumnName, BufferLength, (long)NameLengthPtr,
  (long)DataTypePtr, (long)ColumnSizePtr,
  (long)DecimalDigitsPtr, (long)NullablePtr);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLDescribeCol,
  StatementHandle, ColumnNumber, ColumnName, BufferLength, NameLengthPtr, 
  DataTypePtr, ColumnSizePtr, DecimalDigitsPtr, NullablePtr);

 if(hStmt->hStmtExtras->ird.rowDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Uninitialized column headers."));
  API_HOOK_RETURN(SQL_ERROR);
 }
 if(ColumnNumber < 1 || ColumnNumber > hStmt->hStmtExtras->ird.descCount)
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR: Column %d is out of range. Range is 1 - %d"), 
   hStmt->hStmtExtras->ird.descCount);

  API_HOOK_RETURN(SQL_ERROR);
 }

 /* OK */
 columnHeader = _SQLGetIRDItem(&hStmt->hStmtExtras->ird, ColumnNumber);

 SQLRETURN ret = SQL_SUCCESS;

 ret = helper.CopyToSQLData(
  ColumnName, BufferLength, NameLengthPtr,
  tstring(columnHeader->pszSQL_DESC_NAME));

 if(NameLengthPtr) // CopyToSQLData uses bytes, NameLengthPtr in characters.
  *NameLengthPtr = *NameLengthPtr / sizeof(TCHAR);

 if(DataTypePtr)
  *DataTypePtr = columnHeader->nSQL_DESC_TYPE;

 if(ColumnSizePtr)
  *ColumnSizePtr = columnHeader->nSQL_DESC_LENGTH;

 if(DecimalDigitsPtr)
  *DecimalDigitsPtr = columnHeader->nSQL_DESC_SCALE;

 if(NullablePtr)
  *NullablePtr = columnHeader->nSQL_DESC_NULLABLE;

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(ret);
}
