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
https://msdn.microsoft.com/en-us/library/ms709285%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLSetEnvAttr(0x007c6298, 200, 0x00000003, -6)	SQL_SUCCESS
SQLSetEnvAttr(0x007c6298, 201, 0x00000002, -6)	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLSetEnvAttr(
 SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value,
 SQLINTEGER StringLength)
{
 HDRVENV hEnv = (HDRVENV)EnvironmentHandle;
 SQLINTEGER val;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: EnvironmentHandle = 0x%08lX, Attribute = %d, ") 
  TEXT("Value = 0x%08lX, StringLength = %d"),
  LOG_FUNCTION_NAME, 
  (long)EnvironmentHandle, Attribute, (long)Value, StringLength);

 /* SANITY CHECKS */
 ODBCAPIHelper helper(hEnv, SQL_HANDLE_ENV);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 helper.Lock();

 API_HOOK_ENTRY(SQLSetEnvAttr, 
  EnvironmentHandle, Attribute, Value, StringLength);

 /* These cases permit null env handles */
 /* We may do something with these later */
 if(!hEnv && (Attribute == SQL_ATTR_CONNECTION_POOLING ||
              Attribute == SQL_ATTR_CP_MATCH))
 {
  goto __SQLSetEnvAttr_End;
 }

 switch(Attribute)
 {
  case SQL_ATTR_CONNECTION_POOLING:
   LOG_DEBUG_F_FUNC(TEXT("Setting SQL_ATTR_CONNECTION_POOLING: Value=%u"), 
                    (SQLUINTEGER)((std::size_t)Value));
   
   hEnv->hEnvExtras->attrConnPooling = (SQLINTEGER)((std::size_t)Value);
   break;

  case SQL_ATTR_CP_MATCH:
   LOG_DEBUG_F_FUNC(TEXT("Setting SQL_ATTR_CP_MATCH: Value=%u"), 
                    (SQLUINTEGER)((std::size_t)Value));

   hEnv->hEnvExtras->attrCPMatch = (SQLINTEGER)((std::size_t)Value);
   break;

  case SQL_ATTR_ODBC_VERSION:
   LOG_DEBUG_F_FUNC(TEXT("Setting SQL_ATTR_ODBC_VERSION: Value=%d"), 
                    (SQLUINTEGER)((std::size_t)Value));

   val = (SQLINTEGER)((std::size_t)Value);
   if(val != SQL_OV_ODBC2 && val != SQL_OV_ODBC3)
   {
#if (ODBCVER >= 0x0380)
    if(val != SQL_OV_ODBC3_80)
#endif
    LOG_WARN_F_FUNC(TEXT("Unrecognized ODBC version=%d"), val);
   }

   hEnv->hEnvExtras->attrODBCVersion = (SQLINTEGER)((std::size_t)Value);
   break;

  case SQL_ATTR_OUTPUT_NTS:
   LOG_DEBUG_F_FUNC(TEXT("Setting SQL_ATTR_OUTPUT_NTS: Value=%d"),
                    (SQLUINTEGER)((std::size_t)Value));

   hEnv->hEnvExtras->attrOutputNTS = (SQLINTEGER)((std::size_t)Value);
   break;

  default:
   LOG_ERROR_F_FUNC(
    TEXT("Setting Invalid Environment Attribute=%d"), Attribute);
   API_HOOK_RETURN(SQL_ERROR);
 }

__SQLSetEnvAttr_End:

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_SUCCESS);
}
