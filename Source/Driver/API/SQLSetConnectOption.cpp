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
https://msdn.microsoft.com/en-us/library/ms713564%28v=vs.85%29.aspx

https://msdn.microsoft.com/en-us/library/ms714008%28v=vs.85%29.aspx

WE DON'T EXPORT THIS API.
*/
SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC hDrvDbc, UWORD nOption,
                                      SQLULEN vParam)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvDbc = 0x%08lX, nOption = %d, vParam = %d"),
  LOG_FUNCTION_NAME,
  (long)hDrvDbc, nOption, vParam);

 API_HOOK_ENTRY(SQLSetConnectOption, hDrvDbc, nOption, vParam);

 /* SANITY CHECKS */
 if(hDbc == SQL_NULL_HDBC)
  return SQL_INVALID_HANDLE;

 switch(nOption)
 {
  case SQL_TRANSLATE_DLL:
  case SQL_TRANSLATE_OPTION:
   /*	case SQL_CONNECT_OPT_DRVR_START: */
  case SQL_ODBC_CURSORS:
  case SQL_OPT_TRACE:
   switch(vParam)
   {
    case SQL_OPT_TRACE_ON:
    case SQL_OPT_TRACE_OFF:
    default:
     ;
   }
   break;

  case SQL_OPT_TRACEFILE:
  case SQL_ACCESS_MODE:
  case SQL_AUTOCOMMIT:
  default:
   ;
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 return SQL_ERROR;
}
