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
SQLMoreResults ( 0x065bf898 )	SQL_NO_DATA
*/

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT hDrvStmt)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = $%08lX"), LOG_FUNCTION_NAME, (long)hStmt);

 /* SANITY CHECKS */
 if(NULL == hStmt)
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLMoreResults, hDrvStmt);

 /************************
  * REPLACE THIS COMMENT WITH SOMETHING USEFULL
  ************************/
 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_NO_DATA"), LOG_FUNCTION_NAME);

 // Always return SQL_NO_DATA, reference: SQLite3 ODBC Driver.
 API_HOOK_RETURN(SQL_NO_DATA);
}
