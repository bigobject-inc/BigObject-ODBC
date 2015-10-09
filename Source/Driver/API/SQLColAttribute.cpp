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
https://msdn.microsoft.com/en-us/library/ms713558%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLColAttributeW ( 0x065bf898, 1, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 2, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 3, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 4, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 5, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 6, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 7, 1011, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
...SQLFetch ( 0x065bf898 )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 1, 2, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 1, 8, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
...SQLGetData ( 0x065bf898, 1, -16, 0x04cf0810, 4092, 0x0658ec34 )	SQL_SUCCESS
SQLColAttributeW ( 0x065bf898, 2, 2, <ptr>, 4096, <ptr>, <ptr> )	SQL_SUCCESS
...
*/

SQLRETURN SQL_API SQLColAttribute(
 SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
 SQLUSMALLINT FieldIdentifier, SQLPOINTER CharacterAttributePtr,
 SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr,
#if defined(_WIN64) || !defined(WIN32)
 SQLLEN *NumericAttributePtr)
#else
 SQLPOINTER NumericAttributePtr)
#endif
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;
 COLUMNHDR *columnHeader;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, ColumnNumber = %d, ")
  TEXT("FieldIdentifier = %d, CharacterAttributePtr = 0x%08lX, ")
  TEXT("BufferLength = %d, StringLengthPtr = 0x%08lX, ")
  TEXT("NumericAttributePtr = 0x%08lX"),
  LOG_FUNCTION_NAME, 
  (long)StatementHandle, ColumnNumber,
  FieldIdentifier, (long)CharacterAttributePtr,
  BufferLength, (long)StringLengthPtr,
  (long)NumericAttributePtr);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 if(hStmt->hStmtExtras->ird.rowDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Uninitialized column headers."));
  return SQL_ERROR;
 }

 if(ColumnNumber < 1 || ColumnNumber > hStmt->hStmtExtras->ird.descCount)
 {
  LOG_ERROR_F_FUNC(TEXT("Column out of bounds : %u"), ColumnNumber);
  return SQL_ERROR;
 }

 API_HOOK_ENTRY(SQLColAttribute,
  StatementHandle, ColumnNumber, FieldIdentifier, CharacterAttributePtr,
  BufferLength, StringLengthPtr, NumericAttributePtr);

 /* OK */
 columnHeader = _SQLGetIRDItem(&hStmt->hStmtExtras->ird, ColumnNumber);

 SQLRETURN ret = SQL_SUCCESS;

 switch(FieldIdentifier)
 {
  case SQL_DESC_AUTO_UNIQUE_VALUE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_AUTO_UNIQUE_VALUE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->bSQL_DESC_AUTO_UNIQUE_VALUE);
   break;

  case SQL_DESC_BASE_COLUMN_NAME:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_BASE_COLUMN_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_BASE_COLUMN_NAME));
   break;

  case SQL_DESC_BASE_TABLE_NAME:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_BASE_TABLE_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_BASE_TABLE_NAME));
   break;

  case SQL_DESC_CASE_SENSITIVE:
  // case SQL_COLUMN_CASE_SENSITIVE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_CASE_SENSITIVE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->bSQL_DESC_CASE_SENSITIVE);
   break;

  case SQL_DESC_CATALOG_NAME:
  // case SQL_COLUMN_QUALIFIER_NAME: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_CATALOG_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_CATALOG_NAME));
   break;

  case SQL_DESC_CONCISE_TYPE:
  // case SQL_COLUMN_TYPE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_CONCISE_TYPE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_CONCISE_TYPE);
   break;

  case SQL_DESC_COUNT:
  case SQL_COLUMN_COUNT: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_COUNT"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              hStmt->hStmtExtras->ird.descCount);
   break;

  case SQL_DESC_DISPLAY_SIZE:
  // case SQL_COLUMN_DISPLAY_SIZE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_DISPLAY_SIZE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_DISPLAY_SIZE);
   break;

  case SQL_DESC_FIXED_PREC_SCALE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_FIXED_PREC_SCALE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->bSQL_DESC_FIXED_PREC_SCALE);
   break;

  case SQL_DESC_LABEL:
  // case SQL_COLUMN_LABEL: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_LABEL"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_LABEL));
   break;

  case SQL_DESC_LENGTH:
  case SQL_COLUMN_LENGTH: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_LENGTH"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_LENGTH);
   break;

  case SQL_DESC_LITERAL_PREFIX:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_LITERAL_PREFIX"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_LITERAL_PREFIX));
   break;

  case SQL_DESC_LITERAL_SUFFIX:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_LITERAL_SUFFIX"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_LITERAL_SUFFIX));
   break;

  case SQL_DESC_LOCAL_TYPE_NAME:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_LOCAL_TYPE_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_LOCAL_TYPE_NAME));
   break;

  case SQL_DESC_NAME:
  case SQL_COLUMN_NAME: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_NAME));
   break;

  case SQL_DESC_NULLABLE:
  case SQL_COLUMN_NULLABLE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_NULLABLE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_NULLABLE);   
   break;

  case SQL_DESC_NUM_PREC_RADIX:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_NUM_PREC_RADIX"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_NUM_PREC_RADIX);
   break;

  case SQL_DESC_OCTET_LENGTH:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_OCTET_LENGTH"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_OCTET_LENGTH);
   break;

  case SQL_DESC_PRECISION:
  case SQL_COLUMN_PRECISION: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_PRECISION"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_PRECISION);
   break;

  case SQL_DESC_SCALE:
  case SQL_COLUMN_SCALE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_SCALE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_SCALE);
   break;

  case SQL_DESC_SCHEMA_NAME:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_SCHEMA_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_SCHEMA_NAME));
   break;

  case SQL_DESC_SEARCHABLE:
  // case SQL_COLUMN_SEARCHABLE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_SEARCHABLE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_SEARCHABLE);
   break;

  case SQL_DESC_TABLE_NAME:
  // case SQL_COLUMN_TABLE_NAME: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_TABLE_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_TABLE_NAME));
   break;

  case SQL_DESC_TYPE:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_TYPE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_TYPE);
   break;

  case SQL_DESC_TYPE_NAME:
  // case SQL_COLUMN_TYPE_NAME: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_TYPE_NAME"));

   ret = helper.CopyToSQLData(
    CharacterAttributePtr, BufferLength, StringLengthPtr,
    tstring(columnHeader->pszSQL_DESC_TYPE_NAME));
   break;

  case SQL_DESC_UNNAMED:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_UNNAMED"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_UNNAMED);
   break;

  case SQL_DESC_UNSIGNED:
  // case SQL_COLUMN_UNSIGNED: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_UNSIGNED"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->bSQL_DESC_UNSIGNED);
   break;

  case SQL_DESC_UPDATABLE:
  // case SQL_COLUMN_UPDATABLE: // SQLColAttributes
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DESC_UPDATABLE"));

   ret = helper.CopyToSQLData(NumericAttributePtr, StringLengthPtr,
                              columnHeader->nSQL_DESC_UPDATABLE);
   break;

  default:
   LOG_ERROR_F_FUNC(
    TEXT("Invalid FieldIdentifier value of %d"), FieldIdentifier);
   API_HOOK_RETURN(SQL_ERROR);
 }

 helper.LogSQLReturn(LOG_FUNCTION_NAME, ret);

 API_HOOK_RETURN(ret);
}
