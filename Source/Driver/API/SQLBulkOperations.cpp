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

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT hDrvStmt, SQLSMALLINT nOperation)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 /* SANITY CHECKS */
 if(NULL == hStmt)
  return SQL_INVALID_HANDLE;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = $%08lX"), LOG_FUNCTION_NAME, (long)hStmt);

 /* OK */
 switch(nOperation)
 {
  case SQL_ADD:
   break;
  case SQL_UPDATE_BY_BOOKMARK:
   break;
  case SQL_DELETE_BY_BOOKMARK:
   break;
  case SQL_FETCH_BY_BOOKMARK:
   break;
  default:
   LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: Unknown nOperation=%d"), nOperation);
   return SQL_ERROR;
 }

 /************************
  * REPLACE THIS COMMENT WITH SOMETHING USEFULL
  ************************/
 LOG_WARN_F_FUNC(TEXT("%s: This function not currently supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 return SQL_ERROR;
}
