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

SQLRETURN _AllocConnect(SQLHENV hDrvEnv, SQLHDBC *phDrvDbc)
{
 HDRVENV hEnv = (HDRVENV)hDrvEnv;
 HDRVDBC *phDbc = (HDRVDBC*)phDrvDbc;

 LOG_DEBUG_F_FUNC(TEXT("%s: hEnv = 0x%08lX phDbc = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)hEnv, (long)phDbc);

 /************************
  * SANITY CHECKS
  ************************/
 ODBCAPIHelper helper(hEnv, SQL_HANDLE_ENV);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 if(SQL_NULL_HDBC == phDbc)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: *phDbc is NULL."));
  return SQL_ERROR;
 }

 helper.Lock();

 /************************
  * OK LETS DO IT
  ************************/

 /* allocate database access structure */
 *phDbc = (HDRVDBC)malloc(sizeof(DRVDBC));
 if(SQL_NULL_HDBC == *phDbc)
 {
  *phDbc = SQL_NULL_HDBC;
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: malloc error."));
  return SQL_ERROR;
 }

 /* initialize structure */
 memset(*phDbc, 0, sizeof(DRVDBC));

 (*phDbc)->bConnected = 0;
 (*phDbc)->hDbcExtras = NULL;
 (*phDbc)->hFirstStmt = NULL;
 (*phDbc)->hLastStmt = NULL;
 (*phDbc)->pNext = NULL;
 (*phDbc)->pPrev = NULL;
 (*phDbc)->hEnv = (SQLPOINTER)hEnv;

 /* ADD TO END OF LIST */
 if(hEnv->hFirstDbc == NULL)
 {
  /* 1st is null so the list is empty right now */
  hEnv->hFirstDbc = (*phDbc);
  hEnv->hLastDbc = (*phDbc);
 }
 else
 {
  /* at least one node in list */
  hEnv->hLastDbc->pNext = (HDRVDBC)(*phDbc);
  (*phDbc)->pPrev = (HDRVDBC)hEnv->hLastDbc;
  hEnv->hLastDbc = (*phDbc);
 }

 /********************************************************/
 /* ALLOCATE AND INIT EXTRAS HERE 						*/
 (*phDbc)->hDbcExtras = (HDBCEXTRAS)malloc(sizeof(DBCEXTRAS));
 memset((*phDbc)->hDbcExtras, 0, sizeof(DBCEXTRAS));
 (*phDbc)->hDbcExtras->magic = ODBC_DBC_MAGIC;

#if defined(_WIN32) || defined(_WIN64)
 InitializeCriticalSection(&(*phDbc)->hDbcExtras->cs);
#else
#endif

 // default values
 (*phDbc)->hDbcExtras->accessMode = SQL_MODE_READ_ONLY;
 (*phDbc)->hDbcExtras->autoIPD = SQL_FALSE;
 (*phDbc)->hDbcExtras->asyncEnable = SQL_ASYNC_ENABLE_OFF;
 (*phDbc)->hDbcExtras->autoCommit = SQL_AUTOCOMMIT_ON;
 (*phDbc)->hDbcExtras->timeOut = 0;
 (*phDbc)->hDbcExtras->loginTimeOut = 0;
 (*phDbc)->hDbcExtras->metaDataID = SQL_FALSE;
 (*phDbc)->hDbcExtras->odbcCursors = SQL_CUR_USE_DRIVER;
 (*phDbc)->hDbcExtras->window = (SQLUINTEGER)NULL;
 (*phDbc)->hDbcExtras->txnIsolation = 0;
 (*phDbc)->hDbcExtras->maxRows = 0;
 (*phDbc)->hDbcExtras->queryTimeout = 0;

 (*phDbc)->hDbcExtras->conn = NULL;
 (*phDbc)->hDbcExtras->odbcSettings = NULL;
 /********************************************************/

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

/*
https://msdn.microsoft.com/en-us/library/ms714852%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLAllocConnect(SQLHENV hDrvEnv, SQLHDBC *phDrvDbc)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: hDrvEnv = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)hDrvEnv);

 API_HOOK_ENTRY(SQLAllocConnect, hDrvEnv, phDrvDbc);

 API_HOOK_RETURN(_AllocConnect(hDrvEnv, phDrvDbc));
}
