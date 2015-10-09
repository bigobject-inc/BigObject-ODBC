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
https://msdn.microsoft.com/en-us/library/ms710123%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLFreeHandle(SQL_HANDLE_STMT, 0x065bf898)	SQL_SUCCESS
...
SQLFreeHandle ( SQL_HANDLE_DBC, 0x007c6310 )	SQL_SUCCESS
SQLFreeHandle ( SQL_HANDLE_ENV, 0x007c6298 )	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle)
{
 LOG_DEBUG_F_FUNC(TEXT("%s: HandleType = %d, Handle = 0x%08lX"),
                  LOG_FUNCTION_NAME, HandleType, (long)Handle);

 API_HOOK_ENTRY(SQLFreeHandle, HandleType, Handle);

 switch(HandleType)
 {
  case SQL_HANDLE_ENV:
   API_HOOK_RETURN(_FreeEnv((SQLHENV)Handle));

  case SQL_HANDLE_DBC:
   API_HOOK_RETURN(_FreeConnect((SQLHDBC)Handle));

  case SQL_HANDLE_STMT:
   API_HOOK_RETURN(_FreeStmt((SQLHSTMT)Handle));

  case SQL_HANDLE_DESC:
   break;

  default:
   API_HOOK_RETURN(SQL_ERROR);
 }

 API_HOOK_RETURN(SQL_ERROR);
}
