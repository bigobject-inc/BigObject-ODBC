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

SQLRETURN SQL_API SQLSetParam(
 SQLHSTMT hDrvStmt, SQLUSMALLINT nPar, SQLSMALLINT nType,
 SQLSMALLINT nSqlType, SQLULEN nColDef, SQLSMALLINT nScale,
 SQLPOINTER pValue,
 SQLLEN *pnValue)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 /* SANITY CHECKS */
 if(hStmt == SQL_NULL_HSTMT)
  return SQL_INVALID_HANDLE;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = $%08lX"), LOG_FUNCTION_NAME, (long)hStmt);

 if( NULL == hStmt->pszQuery)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No prepared statement to work with."));
  return SQL_ERROR;
 }

 /******************
  * 1. Your param storage is in hStmt->hStmtExtras
  *    so you will have to code for it. Do it here.
  ******************/

 /************************
  * REPLACE THIS COMMENT WITH SOMETHING USEFULL
  ************************/
 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_ERROR;
}
