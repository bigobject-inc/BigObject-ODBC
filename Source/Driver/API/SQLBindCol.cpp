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
https://msdn.microsoft.com/en-us/library/ms711010%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLBindCol(
 SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, 
 SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr,
 SQLLEN BufferLength, SQLLEN *StrLen_or_Ind)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, ColumnNumber = %d, TargetType = %d, ")
  TEXT("TargetValuePtr = 0x%08lX, BufferLength = %d, StrLen_or_Ind = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, ColumnNumber, TargetType,
  (long)TargetValuePtr, BufferLength, (long)StrLen_or_Ind);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLBindCol,
  StatementHandle, ColumnNumber, TargetType, TargetValuePtr, BufferLength, 
  StrLen_or_Ind);

 if(hStmt->hStmtExtras->ird.rowDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Uninitialized column headers."));

  API_HOOK_RETURN(SQL_ERROR);
 }

 if(ColumnNumber < 1 || ColumnNumber > hStmt->hStmtExtras->ird.descCount)
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR: Column %d is out of range. Range is 1 - %d."),
   ColumnNumber, hStmt->hStmtExtras->ird.descCount);

  API_HOOK_RETURN(SQL_SUCCESS);
 }

 if(StrLen_or_Ind != NULL)
  *StrLen_or_Ind = 0;

 /* SET DEFAULTS */

 // extract the appl. row descriptor from stmt
 ARD* ard = &(hStmt->hStmtExtras->ard);
 // get the specified column if already bound
 ARDItem* ardcol = _SQLGetARDItem(ard, ColumnNumber);

 // EXISTS

 if(ardcol != NULL) 
 {
  // check if total unbind is required
  if(TargetValuePtr == NULL && StrLen_or_Ind == NULL)
  {
   // detach it from ARD link list
   _SQLDetachARDItem(ard, ardcol);
  }
  else  
  {
   // unbind/rebind col details
   _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_DATA_PTR, 
                       TargetValuePtr, -1);
   _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_CONCISE_TYPE, 
                       (SQLPOINTER)TargetType, -1);
   _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_LENGTH, 
                       (SQLPOINTER)BufferLength, -1);
   _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_OCTET_LENGTH_PTR, 
                       StrLen_or_Ind, -1);
  }

  API_HOOK_RETURN(SQL_SUCCESS);
 }

 // DOES NOT EXIST

 // check for bad params
 if(TargetValuePtr == NULL && StrLen_or_Ind == NULL) 
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Bad params."));

  API_HOOK_RETURN(SQL_ERROR);
 }

 /* store app col pointer */
 // CREATE
 // allocate a new col-item
 ardcol = new ARDItem;
 // reset
 _SQLSetARDItemFieldsDefault(ardcol, ColumnNumber);
 // set all values - bind
 _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_DATA_PTR, 
                     TargetValuePtr, -1);
 _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_CONCISE_TYPE, 
                     (SQLPOINTER)TargetType, -1);
 _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_LENGTH, 
                     (SQLPOINTER)BufferLength, -1);
 _SQLSetARDItemField(ard, ardcol, ColumnNumber, SQL_DESC_OCTET_LENGTH_PTR, 
                     StrLen_or_Ind, -1);
 // attach it to link list
 _SQLAttachARDItem(ard, ardcol);

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
