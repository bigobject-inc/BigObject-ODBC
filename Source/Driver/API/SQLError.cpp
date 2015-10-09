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
Get oldest error for the given handle.

This is deprecated - use SQLGetDiagRec instead. This is mapped
to SQLGetDiagRec. The main difference between this and 
SQLGetDiagRec is that this call will delete the error message to
allow multiple calls here to work their way through all of the
errors even with the lack of an ability to pass a specific message
number to be returned.
*/
//SQLRETURN SQL_API SQLError(
// SQLHENV hDrvEnv, SQLHDBC hDrvDbc, SQLHSTMT hDrvStmt,
// SQLTCHAR *szSqlState, SQLINTEGER *pfNativeError, SQLTCHAR *szErrorMsg,
// SQLSMALLINT nErrorMsgMax, SQLSMALLINT *pcbErrorMsg)
//{
// SQLSMALLINT nHandleType;
// SQLHANDLE hHandle;
// SQLRETURN nReturn;
//
// LOG_DEBUG_F_FUNC(TEXT("%s: hDrvEnv = $%08lX"),
//                  LOG_FUNCTION_NAME, (long)hDrvEnv);
//
// /* map call to SQLGetDiagRec */
// if(hDrvEnv)
// {
//  nHandleType = SQL_HANDLE_ENV;
//  hHandle = hDrvEnv;
// }
// else if(hDrvDbc)
// {
//  nHandleType = SQL_HANDLE_DBC;
//  hHandle = hDrvDbc;
// }
// else if(hDrvStmt)
// {
//  nHandleType = SQL_HANDLE_STMT;
//  hHandle = hDrvStmt;
// }
// else
//  return SQL_INVALID_HANDLE;
//
// nReturn = _SQLGetDiagRec(nHandleType, hHandle, 1, szSqlState, pfNativeError,
//                          szErrorMsg, nErrorMsgMax, pcbErrorMsg);
//
// return nReturn;
//}
