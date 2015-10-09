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
https://msdn.microsoft.com/en-us/library/ms709276%28v=vs.85%29.aspx
*/

SQLRETURN SQL_API SQLGetEnvAttr(
 SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value,
 SQLINTEGER BufferLength, SQLINTEGER *StringLength)
{
 HDRVENV hEnv = (HDRVENV)EnvironmentHandle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: EnvironmentHandle = 0x%08lX, Attribute = %d, ")
  TEXT("Value = 0x%08lX, BufferLength = %d, StringLength = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)EnvironmentHandle, Attribute, (long)Value, 
  BufferLength, (long)StringLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hEnv, SQL_HANDLE_ENV);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 helper.Lock();

 API_HOOK_ENTRY(SQLGetEnvAttr,
  EnvironmentHandle, Attribute, Value, BufferLength, StringLength);

 switch(Attribute)
 {
 case SQL_ATTR_CONNECTION_POOLING:
  LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_CONNECTION_POOLING"));

  helper.CopyToSQLData(Value, StringLength,
                       hEnv->hEnvExtras->attrConnPooling);
  break;

 case SQL_ATTR_CP_MATCH:
  LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_CP_MATCH"));

  helper.CopyToSQLData(Value, StringLength,
                       hEnv->hEnvExtras->attrCPMatch);
  break;

 case SQL_ATTR_ODBC_VERSION:
  LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_ODBC_VERSION"));

  helper.CopyToSQLData(Value, StringLength,
                       hEnv->hEnvExtras->attrODBCVersion);
  break;

 case SQL_ATTR_OUTPUT_NTS:
  LOG_DEBUG_FUNC(TEXT("Getting SQL_ATTR_OUTPUT_NTS"));

  helper.CopyToSQLData(Value, StringLength,
                       hEnv->hEnvExtras->attrOutputNTS);
  break;

 default:
  LOG_ERROR_F_FUNC(
   TEXT("Getting Invalid Environment Attribute=%d"), Attribute);
  API_HOOK_RETURN(SQL_ERROR);
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
