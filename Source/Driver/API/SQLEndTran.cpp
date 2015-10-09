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

SQLRETURN _EndTran(SQLSMALLINT nHandleType, SQLHANDLE nHandle,
                   SQLSMALLINT nCompletionType)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: nHandleType = %d, nHandle = 0x%08lX, nCompletionType = %d"),
  LOG_FUNCTION_NAME,
  nHandleType, (long)nHandle, nCompletionType);

 switch(nHandleType)
 {
  case SQL_HANDLE_DBC:
  {
   /* SANITY CHECKS */
   ODBCAPIHelper helper(nHandle, SQL_HANDLE_DBC);
   if(!helper.IsValid())
    return SQL_INVALID_HANDLE;

   helper.Lock();
   
   ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
   ODBCDriver::IService& service = ODBCDriver::Service::instance();

   switch(nCompletionType)
   {
    case SQL_COMMIT:
     try
     {
      //serverRet = service.Commit(nHandle);
      serverRet = ODBCDriver::SERVER_SUCCESS;
     }
     catch(ODBCDriver::ClientException& ex)
     {
      LOG_ERROR_FUNC(ex.What());
      serverRet = ODBCDriver::SERVER_ERROR;
     }
     break;

    case SQL_ROLLBACK:
     try
     {
      //serverRet = service.Rollback(nHandle);
      serverRet = ODBCDriver::SERVER_SUCCESS;
     }
     catch(ODBCDriver::ClientException& ex)
     {
      LOG_ERROR_FUNC(ex.What());
      serverRet = ODBCDriver::SERVER_ERROR;
     }
     break;
   }

   if(serverRet == ODBCDriver::SERVER_ERROR)
   {
    LOG_ERROR_FUNC(TEXT("SQL_ERROR: unable to commit or rollback."));
    return SQL_ERROR;
   }

   return SQL_SUCCESS;
  }

  case SQL_HANDLE_ENV:
  {
   /* SANITY CHECKS */
   ODBCAPIHelper helper(nHandle, SQL_HANDLE_ENV);
   if(!helper.IsValid())
    return SQL_INVALID_HANDLE;

   helper.Lock();

   SQLRETURN ret = SQL_SUCCESS;

   for(HDRVDBC hDC = ((HDRVENV)nHandle)->hFirstDbc; 
       hDC != ((HDRVENV)nHandle)->hLastDbc;
       hDC = hDC->pNext)
   {
    SQLRETURN retcode = _EndTran(SQL_HANDLE_DBC, (SQLHANDLE)hDC, 
                                 nCompletionType);
    if(retcode != SQL_SUCCESS)
     ret = retcode;
   }

   return ret;
  }
 }

 return SQL_INVALID_HANDLE;
}

/*
https://msdn.microsoft.com/en-us/library/ms716544%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLEndTran(SQLSMALLINT HandleType, SQLHANDLE Handle,
                             SQLSMALLINT CompletionType)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: HandleType = %d, Handle = 0x%08lX, CompletionType = %d"),
  LOG_FUNCTION_NAME,
  HandleType, (long)Handle, CompletionType);

 API_HOOK_ENTRY(SQLEndTran, HandleType, Handle, CompletionType);

 API_HOOK_RETURN(_EndTran(HandleType, Handle, CompletionType));
}
