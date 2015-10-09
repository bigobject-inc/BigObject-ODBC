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

#ifndef _BIGOBJECT_ODBC_H_DIAGNOSTIC
#define _BIGOBJECT_ODBC_H_DIAGNOSTIC

#include "../Driver.hpp"

////////////////////////////////////////////// 
// Functions
//////////////////////////////////////////////

SQLRETURN _SQLGetDiagRec(
 SQLSMALLINT nHandleType, SQLHANDLE hHandle,
 SQLSMALLINT nRecordNumber, SQLTCHAR * pszState,
 SQLINTEGER * pnNativeError, SQLTCHAR * pszMessageText,
 SQLSMALLINT nBufferLength, SQLSMALLINT * pnStringLength);

void _SQLPopMsg(bigobject::Logging::LogLevel level, const TCHAR* pMsgArgs, ...);

#define _SQLPopErrorMsg(pMsgArgs, ...) \
 _SQLPopMsg(bigobject::Logging::LEVEL_ERROR, pMsgArgs, __VA_ARGS__)

#define _SQLPopInfoMsg(pMsgArgs, ...) \
 _SQLPopMsg(bigobject::Logging::LEVEL_INFO, pMsgArgs, __VA_ARGS__)

#define _SQLPopDebugMsg(pMsgArgs, ...) \
 _SQLPopMsg(bigobject::Logging::LEVEL_DEBUG, pMsgArgs, __VA_ARGS__)

//////////////////////////////////////////////

#endif
