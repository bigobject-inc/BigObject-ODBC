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
SQLRETURN SQL_API SQLGetDiagRec(
 SQLSMALLINT nHandleType, SQLHANDLE hHandle,
 SQLSMALLINT nRecordNumber, SQLTCHAR * pszState,
 SQLINTEGER * pnNativeError, SQLTCHAR * pszMessageText,
 SQLSMALLINT nBufferLength, SQLSMALLINT * pnStringLength)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: nHandleType = %d, hHandle = $%08lX"),
  LOG_FUNCTION_NAME, nHandleType, (long)hHandle);

 return _SQLGetDiagRec(nHandleType, hHandle, nRecordNumber, pszState,
                       pnNativeError, pszMessageText, nBufferLength,
                       pnStringLength);
}
*/
