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

SQLRETURN _AllocEnv(SQLHENV *phDrvEnv)
{
 HDRVENV *phEnv = (HDRVENV*)phDrvEnv;

 /* SANITY CHECKS */
 if(NULL == phEnv)
  return SQL_INVALID_HANDLE;

 /* OK */

 /* allocate environment */
 *phEnv = (HDRVENV)malloc(sizeof(DRVENV));
 if(SQL_NULL_HENV == *phEnv)
 {
  *phEnv = SQL_NULL_HENV;
  return SQL_ERROR;
 }

 /* initialise environment */
 memset(*phEnv, 0, sizeof(DRVENV));
 (*phEnv)->hFirstDbc = NULL;
 (*phEnv)->hLastDbc = NULL;

 /* ALLOCATE AND INIT DRIVER SPECIFIC STORAGE */
 (*phEnv)->hEnvExtras = (HENVEXTRAS)malloc(sizeof(ENVEXTRAS));
 (*phEnv)->hEnvExtras->magic = ODBC_ENV_MAGIC;

#if defined(_WIN32) || defined(_WIN64)
 InitializeCriticalSection(&(*phEnv)->hEnvExtras->cs);
#else
#endif

 // default values.
 (*phEnv)->hEnvExtras->attrODBCVersion = SQL_OV_ODBC3;
 (*phEnv)->hEnvExtras->attrConnPooling = SQL_CP_OFF;
 (*phEnv)->hEnvExtras->attrCPMatch = SQL_CP_STRICT_MATCH;
 (*phEnv)->hEnvExtras->attrOutputNTS = SQL_TRUE;

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

/*
https://msdn.microsoft.com/en-us/library/ms709270%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLAllocEnv(SQLHENV *phDrvEnv)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: phDrvEnv = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)phDrvEnv);

 API_HOOK_ENTRY(SQLAllocEnv, phDrvEnv);

 API_HOOK_RETURN(_AllocEnv(phDrvEnv));
}
