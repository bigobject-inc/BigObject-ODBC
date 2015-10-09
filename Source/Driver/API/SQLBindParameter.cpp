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

SQLRETURN SQL_API SQLBindParameter(
 SQLHSTMT hDrvStmt, SQLUSMALLINT nParameterNumber,
 SQLSMALLINT nIOType, SQLSMALLINT nBufferType,
 SQLSMALLINT nParamType,
 SQLULEN nParamLength,
 SQLSMALLINT nScale, SQLPOINTER pData,
 SQLLEN nBufferLength,
 SQLLEN *pnLengthOrIndicator)
{
 HDRVSTMT hStmt = (HDRVSTMT)hDrvStmt;

 /* SANITY CHECKS */
 if(NULL == hStmt)
  return SQL_INVALID_HANDLE;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hStmt=$%08lX nParameterNumber=%d nIOType=%d ")
  TEXT("nBufferType=%d nParamType=%d nParamLength=%ld nScale=%d ")
  TEXT("pData=$%08lX nBufferLength=%ld *pnLengthOrIndicator=$%08lX"), 
  LOG_FUNCTION_NAME, (long)hStmt, nParameterNumber, nIOType, 
  nBufferType, nParamType, (long)nParamLength, nScale, 
  (long)pData, (long)nBufferLength, *pnLengthOrIndicator);

 /************************
  * REPLACE THIS COMMENT WITH SOMETHING USEFULL
  ************************/
 LOG_WARN_F_FUNC(TEXT("%s: This function not currently supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 return SQL_ERROR;
}
