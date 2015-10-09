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

SQLRETURN _AllocStmt(SQLHDBC hDrvDbc, SQLHSTMT *phDrvStmt)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;
 HDRVSTMT *phStmt = (HDRVSTMT*)phDrvStmt;

 LOG_DEBUG_F_FUNC(TEXT("%s: hDbc = 0x%08lX, phDrvStmt = 0x%08lX"), 
                  LOG_FUNCTION_NAME, (long)hDbc, (long)phDrvStmt);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 if(NULL == phStmt)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: phStmt=NULL"));
  return SQL_ERROR;
 }

 helper.Lock();

 /* OK */

 /* allocate memory */
 *phStmt = (HDRVSTMT)malloc(sizeof(DRVSTMT));
 if(SQL_NULL_HSTMT == *phStmt)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: memory allocation failure."));
  return SQL_ERROR;
 }

 /* initialize memory */
 memset(*phStmt, 0, sizeof(DRVSTMT)); /* SAFETY */
 (*phStmt)->hDbc = (SQLPOINTER)hDbc;
 (*phStmt)->hStmtExtras = NULL;
 (*phStmt)->pNext = NULL;
 (*phStmt)->pPrev = NULL;
 (*phStmt)->pszQuery = NULL;
 _sntprintf((TCHAR*)(*phStmt)->szCursorName, SQL_MAX_CURSOR_NAME, 
            TEXT("CUR_%08lX"), (long)*phStmt);

 /* ADD TO DBCs STATEMENT LIST */

 /* ADD TO END OF LIST */
 if(hDbc->hFirstStmt == NULL)
 {
  /* 1st is null so the list is empty right now */
  hDbc->hFirstStmt = (*phStmt);
  hDbc->hLastStmt = (*phStmt);
 }
 else
 {
  /* at least one node in list */
  hDbc->hLastStmt->pNext = (HDRVSTMT)(*phStmt);
  (*phStmt)->pPrev = (HDRVSTMT)hDbc->hLastStmt;
  hDbc->hLastStmt = (*phStmt);
 }

 /****************************************************************************/
 /* ALLOCATE AND INIT DRIVER EXTRAS HERE 									*/
 (*phStmt)->hStmtExtras = (HSTMTEXTRAS)malloc(sizeof(STMTEXTRAS));
 memset((*phStmt)->hStmtExtras, 0, sizeof(STMTEXTRAS)); /* SAFETY */

 // default values
 (*phStmt)->hStmtExtras->asyncEnable = SQL_ASYNC_ENABLE_OFF;
 (*phStmt)->hStmtExtras->concurrency = SQL_CONCUR_READ_ONLY;
 (*phStmt)->hStmtExtras->cursorScroll = SQL_NONSCROLLABLE;
 (*phStmt)->hStmtExtras->cursorSensitivity = SQL_UNSPECIFIED;
 (*phStmt)->hStmtExtras->cursorType = SQL_CURSOR_FORWARD_ONLY;
 (*phStmt)->hStmtExtras->autoIPD = SQL_FALSE;
 (*phStmt)->hStmtExtras->keysetSize = 0;
 (*phStmt)->hStmtExtras->metaDataID = SQL_FALSE;
 (*phStmt)->hStmtExtras->noScan = SQL_NOSCAN_OFF;
 (*phStmt)->hStmtExtras->queryTimeout = 0;
 (*phStmt)->hStmtExtras->retrieveData = SQL_RD_DEFAULT;

 // set descriptor defaults
 _SQLSetARDFieldsDefault(&((*phStmt)->hStmtExtras->ard), *phDrvStmt);
 _SQLSetAPDFieldsDefault(&((*phStmt)->hStmtExtras->apd), *phDrvStmt);
 _SQLSetIRDFieldsDefault(&((*phStmt)->hStmtExtras->ird), *phDrvStmt);
 _SQLSetIPDFieldsDefault(&((*phStmt)->hStmtExtras->ipd), *phDrvStmt);

 (*phStmt)->hStmtExtras->resultset = NULL;
 /****************************************************************************/

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

/*
https://msdn.microsoft.com/en-us/library/ms712649%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC hDrvDbc, SQLHSTMT *phDrvStmt)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: hDrvDbc = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)hDrvDbc);

 API_HOOK_ENTRY(SQLAllocStmt, hDrvDbc, phDrvStmt);

 API_HOOK_RETURN(_AllocStmt(hDrvDbc, phDrvStmt));
}
