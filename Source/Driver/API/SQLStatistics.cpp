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

enum nSQLStatistics
{
 TABLE_CAT = 1,
 TABLE_SCHEM,
 TABLE_NAME,
 NON_UNIQUE,
 INDEX_QUALIFIER,
 INDEX_NAME,
 TYPE,
 ORDINAL_POSITION,
 COLUMN_NAME,
 ASC_OR_DESC,
 CARDINALITY,
 PAGES,
 FILTER_CONDITION,
 COL_MAX
};

/****************************
 * replace this with init of some struct (see same func for MiniSQL driver) */
char *aSQLStatistics[] = {"one", "two"};
/****************************/

SQLRETURN SQL_API SQLStatistics(
 SQLHSTMT hDrvStmt, SQLTCHAR *szCatalogName,
 SQLSMALLINT nCatalogNameLength, SQLTCHAR *szSchemaName,
 SQLSMALLINT nSchemaNameLength, SQLTCHAR *szTableName, /* MUST BE SUPPLIED */
 SQLSMALLINT nTableNameLength, SQLUSMALLINT nTypeOfIndex,
 SQLUSMALLINT nReserved)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;
 int nColumn;
 int nCols;
 int nRow;
 COLUMNHDR *pColumnHeader;
 char szSQL[200];

 /* SANITY CHECKS */
 if(hStmt == SQL_NULL_HSTMT)
  return SQL_INVALID_HANDLE;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = $%08lX"), LOG_FUNCTION_NAME, (long)hStmt);

 if(szTableName == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No table name."));
  return SQL_ERROR;
 }

 if(szTableName[0] == '\0')
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No table name."));
  return SQL_ERROR;
 }

 /**************************
  * close any existing result
  **************************/
 //if(hStmt->hStmtExtras->aResults)
 // _FreeResults(hStmt->hStmtExtras);

 if(hStmt->pszQuery != NULL)
  free(hStmt->pszQuery);
 hStmt->pszQuery = NULL;

 /**************************
  * EXEC QUERY TO GET KEYS
  **************************/

 /**************************
  * allocate memory for columns headers and result data 
  * (row 0 is column header while col 0 is reserved for bookmarks)
  **************************/

 /**************************
  * gather column header information (save col 0 for bookmarks)
  **************************/

 /************************
  * gather data (save col 0 for bookmarks and factor out index columns)
  ************************/

 /**************************
  * free the snapshot
  **************************/

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}
