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
Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLRowCount ( 0x065bf898, 0x0658ebf4 )	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLRowCount(SQLHSTMT hDrvStmt, SQLLEN *pnRowCount)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = $%08lX"), LOG_FUNCTION_NAME, (long)hStmt);

 /* SANITY CHECKS */
 if(hStmt == SQL_NULL_HSTMT)
  return SQL_INVALID_HANDLE;

 if(hStmt->hStmtExtras == NULL)
  return SQL_INVALID_HANDLE;

 if(NULL == pnRowCount)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: pnRowCount can not be NULL."));
  return SQL_ERROR;
 }

 *pnRowCount = -1; // The value if row count cannot be determined.

 LOG_WARN_F_FUNC(TEXT("%s not currently supported: returning -1."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}
