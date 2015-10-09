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

SQLRETURN _GetData(
 SQLHSTMT hDrvStmt, SQLUSMALLINT nCol, 
 SQLSMALLINT nTargetType, SQLPOINTER pTarget,
 SQLLEN nTargetLength, SQLLEN *pnLengthOrIndicator)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvStmt = 0x%08lX, nCol = %d, ")
  TEXT("nTargetType = %d, pTarget = 0x%08lX, ")
  TEXT("nTargetLength = %d, pnLengthOrIndicator = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)hDrvStmt, nCol,
  nTargetType, (long)pTarget,
  nTargetLength, (long)pnLengthOrIndicator);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hDrvStmt, SQL_HANDLE_STMT);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 return helper.GetData(nCol, nTargetType, pTarget, 
                       nTargetLength, pnLengthOrIndicator);
}

/*
https://msdn.microsoft.com/en-us/library/ms715441%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLGetData(0x065bf898, 2, -8, 0x04cf0810, 4094, 0x0658ec2c)	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLGetData(
 SQLHSTMT StatementHandle, SQLUSMALLINT Col_or_Param_Num, 
 SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr,
 SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: StatementHandle = 0x%08lX, Col_or_Param_Num = %d, ")
  TEXT("TargetType = %d, TargetValuePtr = 0x%08lX, ")
  TEXT("BufferLength = %d, StrLen_or_IndPtr = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)StatementHandle, Col_or_Param_Num,
  TargetType, (long)TargetValuePtr,
  BufferLength, (long)StrLen_or_IndPtr);

 API_HOOK_ENTRY(SQLGetData, 
  StatementHandle, Col_or_Param_Num, TargetType, TargetValuePtr,
  BufferLength, StrLen_or_IndPtr);

 API_HOOK_RETURN(_GetData(
  StatementHandle, Col_or_Param_Num, TargetType, TargetValuePtr,
  BufferLength, StrLen_or_IndPtr));
}
