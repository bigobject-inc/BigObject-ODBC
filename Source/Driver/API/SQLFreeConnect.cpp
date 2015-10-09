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

SQLRETURN _FreeDbc(SQLHDBC hDrvDbc)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;
 SQLRETURN nReturn;

 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_ERROR;

 helper.Lock();

 /* TRY TO FREE STATEMENTS						*/
 /* THIS IS JUST IN CASE; SHOULD NOT BE REQUIRED */
 nReturn = helper.FreeStmtList();
 if(nReturn != SQL_SUCCESS)
  return nReturn;

 /* SPECIAL CHECK FOR FIRST IN LIST 				*/
 if(((HDRVENV)hDbc->hEnv)->hFirstDbc == hDbc)
  ((HDRVENV)hDbc->hEnv)->hFirstDbc = hDbc->pNext;

 /* SPECIAL CHECK FOR LAST IN LIST 				*/
 if(((HDRVENV)hDbc->hEnv)->hLastDbc == hDbc)
  ((HDRVENV)hDbc->hEnv)->hLastDbc = hDbc->pPrev;

 /* EXTRACT SELF FROM LIST						*/
 if(hDbc->pPrev != SQL_NULL_HDBC)
  hDbc->pPrev->pNext = hDbc->pNext;
 if(hDbc->pNext != SQL_NULL_HDBC)
  hDbc->pNext->pPrev = hDbc->pPrev;

 /**********************************************/
 /* 	!!! FREE DRIVER SPECIFIC MEMORY (hidden in hStmtExtras) HERE !!!	*/
 if(hDbc->hDbcExtras)
 {
  if(hDbc->hDbcExtras->conn != NULL)
  {
   ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
   ODBCDriver::IService& service = ODBCDriver::Service::instance();

   try
   {
    serverRet = service.CloseConnection(hDbc->hDbcExtras->conn);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_FUNC(ex.What());
    serverRet = ODBCDriver::SERVER_ERROR;
   }

   if(serverRet == ODBCDriver::SERVER_ERROR)
   {
    LOG_ERROR_FUNC(TEXT("SQL_ERROR: client unable to disconnect."));
    return SQL_ERROR;
   }

   hDbc->hDbcExtras->conn = NULL; /* Just to be safe */
  }

  if(hDbc->hDbcExtras->odbcSettings != NULL)
  {
   free(hDbc->hDbcExtras->odbcSettings);
   hDbc->hDbcExtras->odbcSettings = NULL;
  }

  free(hDbc->hDbcExtras);
 }
 /**********************************************/

 free(hDbc);

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

SQLRETURN _FreeConnect(SQLHDBC hDrvDbc)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;
 int nReturn;

 LOG_DEBUG_F_FUNC(TEXT("%s: hDbc = 0x%08lX"), LOG_FUNCTION_NAME, (long)hDbc);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 if(hDbc->bConnected)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Connection is active."));
  return SQL_ERROR;
 }

 if(hDbc->hFirstStmt != NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Connection has allocated statements."));
  return SQL_ERROR;
 }

 nReturn = _FreeDbc(hDbc);

 return nReturn;
}

/*
https://msdn.microsoft.com/en-us/library/ms709370%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLFreeConnect(SQLHDBC hDrvDbc)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: hDrvDbc = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)hDrvDbc);

 API_HOOK_ENTRY(SQLFreeConnect, hDrvDbc);

 API_HOOK_RETURN(_FreeConnect(hDrvDbc));
}
