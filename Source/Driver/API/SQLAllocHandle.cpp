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
https://msdn.microsoft.com/en-us/library/ms712455%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLAllocHandle ( SQL_HANDLE_ENV, NULL, 0x020567cc )	SQL_SUCCESS
...
SQLAllocHandle ( SQL_HANDLE_DBC, 0x007c6298, 0x02056858 )	SQL_SUCCESS
...
SQLAllocHandle ( SQL_HANDLE_STMT, 0x007c6310, 0x02079ca4 )	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT HandleType, 
                                 SQLHANDLE InputHandle,
                                 SQLHANDLE *OutputHandlePtr)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: HandleType = %d, ")
  TEXT("InputHandle = 0x%08lX, OutputHandlePtr = 0x%08lX"),
  LOG_FUNCTION_NAME, 
  HandleType, (long)InputHandle, (long)OutputHandlePtr);

 API_HOOK_ENTRY(SQLAllocHandle, HandleType, InputHandle, OutputHandlePtr);

 switch(HandleType)
 {
  case SQL_HANDLE_ENV:
   API_HOOK_RETURN(_AllocEnv((SQLHENV*)OutputHandlePtr));

  case SQL_HANDLE_DBC:
   API_HOOK_RETURN(_AllocConnect((SQLHENV)InputHandle, 
                                 (SQLHDBC*)OutputHandlePtr));

  case SQL_HANDLE_STMT:
   API_HOOK_RETURN(_AllocStmt((SQLHDBC)InputHandle, 
                              (SQLHSTMT*)OutputHandlePtr));

  case SQL_HANDLE_DESC:
   break;

  default:
   API_HOOK_RETURN(SQL_ERROR);
   break;
 }

 API_HOOK_RETURN(SQL_ERROR);
}
