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

SQLRETURN SQL_API SQLConnect(
 SQLHDBC hDrvDbc, SQLTCHAR *szDataSource,
 SQLSMALLINT nDataSourceLength, SQLCHAR *szUID,
 SQLSMALLINT nUIDLength, SQLTCHAR *szPWD, SQLSMALLINT nPWDLength)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;
 TCHAR szDATABASE[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR szHOST[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR szPORT[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR szFLAG[INI_MAX_PROPERTY_VALUE + 1];

 LOG_DEBUG_F_FUNC(TEXT("%s: hDbc=$%08lX 3zDataSource=(%s)"),
                  LOG_FUNCTION_NAME, (long)hDbc, szDataSource);

 /* SANITY CHECKS */
 if(SQL_NULL_HDBC == hDbc)
  return SQL_INVALID_HANDLE;

 if(hDbc->bConnected == 1)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Already connected."));
  return SQL_ERROR;
 }

 if(nDataSourceLength == SQL_NTS)
 {
  if(strlen((char*)szDataSource) > ODBC_FILENAME_MAX + INI_MAX_OBJECT_NAME)
  {
   LOG_ERROR_FUNC(
    TEXT("SQL_ERROR: Given Data Source is too long. I consider it suspect."));
   return SQL_ERROR;
  }
 }
 else
 {
  if(nDataSourceLength > ODBC_FILENAME_MAX + INI_MAX_OBJECT_NAME)
  {
   LOG_ERROR_FUNC(
    TEXT("SQL_ERROR: Given Data Source is too long. I consider it suspect."));
   return SQL_ERROR;
  }
 }

 /********************
  * gather and use any required DSN properties
  * - DATABASE
  * - HOST (localhost assumed if not supplied)
  ********************/
 szDATABASE[0] = '\0';
 szHOST[0] = '\0';
 szPORT[0] = '\0';
 szFLAG[0] = '\0';
 SQLGetPrivateProfileString((TCHAR*)szDataSource, TEXT("DATABASE"), TEXT(""), 
                            szDATABASE, sizeof(szDATABASE), TEXT("odbc.ini"));
 if(szDATABASE[0] == '\0')
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR: ")
   TEXT("Could not find Driver entry for %s in system information."),
   szDataSource);
  return SQL_ERROR;
 }
 SQLGetPrivateProfileString((TCHAR*)szDataSource,
  TEXT("HOST"), TEXT("localhost"), szHOST, sizeof(szHOST), TEXT("odbc.ini"));
 SQLGetPrivateProfileString((TCHAR*)szDataSource,
  TEXT("PORT"), TEXT("0"), szPORT, sizeof(szPORT), TEXT("odbc.ini"));
 SQLGetPrivateProfileString((TCHAR*)szDataSource,
  TEXT("FLAG"), TEXT("0"), szFLAG, sizeof(szFLAG), TEXT("odbc.ini"));

 /********************
  * 1. initialise structures
  * 2. try connection with database using your native calls
  * 3. store your server handle in the extras somewhere
  * 4. set connection state
  *      hDbc->bConnected = TRUE;
  ********************/

 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}
