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

SQLRETURN SQL_API SQLBrowseConnect(
 SQLHDBC hDrvDbc, SQLTCHAR *szConnStrIn, SQLSMALLINT cbConnStrIn,
 SQLTCHAR *szConnStrOut, SQLSMALLINT cbConnStrOutMax,
 SQLSMALLINT *pcbConnStrOut)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;

 if(hDbc == NULL)
  return SQL_INVALID_HANDLE;

 LOG_DEBUG_F_FUNC(TEXT("%s: hDbc = $%08lX"), LOG_FUNCTION_NAME, (long)hDbc);

 LOG_WARN_F_FUNC(TEXT("%s: This function not currently supported."), 
                 LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 return SQL_ERROR;
}
