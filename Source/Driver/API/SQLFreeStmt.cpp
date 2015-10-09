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

/**
Free all the params bound to a particular stmt.

@param pHandle HSTMTEXTRAS
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLFreeStmtParams(HSTMTEXTRAS pHandle) 
{
 // note always called as a sub-function so no diag reset
 // to be implemented along with
 // other details of parms
 RETCODE ret = _SQLFreeAPDContent(&(pHandle->apd));
 if(ret != SQL_SUCCESS)
  return ret;

 return _SQLFreeIPDContent(&(pHandle->ipd));
}

/**
Free all the cols bound to a specified stmt.

@param pHandle HSTMTEXTRAS
@return SQL_SUCCESS if OK otherwise SQL_ERROR. 
*/
RETCODE _SQLFreeStmtCols(HSTMTEXTRAS pHandle)
{
 // note
 // assumes handle already checked
 return _SQLFreeARDContent(&(pHandle->ard));
}

SQLRETURN _FreeStmt(SQLHSTMT hDrvStmt)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDrvStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_ERROR;

 /* SPECIAL CHECK FOR FIRST IN LIST 				*/
 if(((HDRVDBC)hStmt->hDbc)->hFirstStmt == hStmt)
  ((HDRVDBC)hStmt->hDbc)->hFirstStmt = hStmt->pNext;

 /* SPECIAL CHECK FOR LAST IN LIST 				*/
 if(((HDRVDBC)hStmt->hDbc)->hLastStmt == hStmt)
  ((HDRVDBC)hStmt->hDbc)->hLastStmt = hStmt->pPrev;

 /* EXTRACT SELF FROM LIST						*/
 if(hStmt->pPrev != SQL_NULL_HSTMT)
  hStmt->pPrev->pNext = hStmt->pNext;
 if(hStmt->pNext != SQL_NULL_HSTMT)
  hStmt->pNext->pPrev = hStmt->pPrev;

 /* FREE STANDARD MEMORY */
 if(NULL != hStmt->pszQuery)
 {
  free(hStmt->pszQuery);
  hStmt->pszQuery = NULL;
 }

 /*********************************************************************/
 /* 	!!! FREE DRIVER SPECIFIC MEMORY (hidden in hStmtExtras) HERE !!! */

 // clear/free col bindings
 _SQLFreeStmtCols(hStmt->hStmtExtras);

 // clear/free parms bindings
 _SQLFreeStmtParams(hStmt->hStmtExtras);

 helper.CloseResults();

 free(hStmt->hStmtExtras);
 /*********************************************************************/

 free(hStmt);

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

/*
https://msdn.microsoft.com/en-us/library/ms709284%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLFreeStmt(0x065bf898, SQL_CLOSE)	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT StatementHandle, SQLUSMALLINT Option)
{
 HDRVSTMT hStmt = (HDRVSTMT)StatementHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, Option = %d"), 
  LOG_FUNCTION_NAME, 
  (long)StatementHandle, Option);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLFreeStmt, StatementHandle, Option);

 /*********
  * RESET PARAMS
  *********/
 switch(Option)
 {
  case SQL_CLOSE:
   API_HOOK_RETURN(helper.CloseResults());

  case SQL_DROP:
   API_HOOK_RETURN(_FreeStmt(hStmt));

  case SQL_UNBIND:
   API_HOOK_RETURN(_SQLFreeStmtCols(hStmt->hStmtExtras));

  case SQL_RESET_PARAMS:
   API_HOOK_RETURN(_SQLFreeStmtParams(hStmt->hStmtExtras));

  default:
   LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: Invalid Option=%d"), Option);
   API_HOOK_RETURN(SQL_ERROR);
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
