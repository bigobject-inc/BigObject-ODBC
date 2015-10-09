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
https://msdn.microsoft.com/en-us/library/ms713946%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLDisconnect(SQLHDBC ConnectionHandle)
{
 HDRVDBC hDbc = (HDRVDBC)ConnectionHandle;

 LOG_DEBUG_F_FUNC(TEXT("%s: ConnectionHandle = 0x%08lX"),
                  LOG_FUNCTION_NAME, (long)ConnectionHandle);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 helper.Lock();

 API_HOOK_ENTRY(SQLDisconnect, ConnectionHandle);

 if(hDbc->bConnected == 0)
 {
  LOG_INFO_FUNC(TEXT("SQL_SUCCESS_WITH_INFO: Connection not open."));
  API_HOOK_RETURN(SQL_SUCCESS_WITH_INFO);
 }

 if(hDbc->hFirstStmt != SQL_NULL_HSTMT)
 {
  LOG_WARN_FUNC(TEXT("Active Statements exist. Force to disconnect."));

  SQLRETURN nReturn = helper.FreeStmtList();
  if(nReturn != SQL_SUCCESS)
  {
   LOG_ERROR_FUNC(TEXT("SQL_ERROR: FreeStmtList error."));
   API_HOOK_RETURN(nReturn);
  }
 }

 /****************************
  * 1. do driver specific close here
  ****************************/
 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 ODBCDriver::ServerReturn serverRet = 
  service.CloseConnection(hDbc->hDbcExtras->conn);

 if(serverRet == ODBCDriver::SERVER_ERROR)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: client unable to disconnect."));
  API_HOOK_RETURN(SQL_ERROR);
 }

 hDbc->hDbcExtras->conn = NULL; /* Just to be safe */
 if(hDbc->hDbcExtras->odbcSettings != NULL)
 {
  free(hDbc->hDbcExtras->odbcSettings);
  hDbc->hDbcExtras->odbcSettings = NULL;
 }
 // #TODO: should we free hDbcExtras ??

 hDbc->bConnected = 0;

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
