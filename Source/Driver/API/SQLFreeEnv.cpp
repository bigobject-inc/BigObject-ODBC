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

SQLRETURN _FreeEnv(SQLHENV hDrvEnv)
{
 HDRVENV hEnv = (HDRVENV)hDrvEnv;

 LOG_DEBUG_F_FUNC(TEXT("%s: hEnv = 0x%08lX"), LOG_FUNCTION_NAME, (long)hEnv);

 /* SANITY CHECKS */
 if(hEnv == SQL_NULL_HENV)
  return SQL_INVALID_HANDLE;

 if(hEnv->hEnvExtras->magic != ODBC_ENV_MAGIC)
  return SQL_SUCCESS;

 if(hEnv->hFirstDbc != NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: There are allocated Connections."));
  return SQL_ERROR;
 }

 /************
  * 	!!! FREE DRIVER SPECIFIC MEMORY (hidden in hEnvExtras) HERE !!!
  ************/
#if defined(_WIN32) || defined(_WIN64)
 LeaveCriticalSection(&hEnv->hEnvExtras->cs);
 DeleteCriticalSection(&hEnv->hEnvExtras->cs);
#endif

 hEnv->hEnvExtras->magic = 0;

 free(hEnv->hEnvExtras);
 free(hEnv);

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

/*
https://msdn.microsoft.com/en-us/library/ms710920%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLFreeEnv(SQLHENV hDrvEnv)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: hDrvEnv = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)hDrvEnv);

 API_HOOK_ENTRY(SQLFreeEnv, hDrvEnv);

 API_HOOK_RETURN(_FreeEnv(hDrvEnv));
}
