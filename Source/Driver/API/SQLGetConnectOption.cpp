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
https://msdn.microsoft.com/en-us/library/ms711815%28v=vs.85%29.aspx

https://msdn.microsoft.com/en-us/library/ms715440%28v=vs.85%29.aspx

WE DON'T EXPORT THIS API.
*/

SQLRETURN SQL_API SQLGetConnectOption(SQLHDBC hDrvDbc, UWORD fOption, 
                                      PTR pvParam)
{
 HDRVDBC hDbc = (HDRVDBC)hDrvDbc;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvDbc = 0x%08lX, fOption = %d, pvParam = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)hDrvDbc, fOption, (long)pvParam);

 API_HOOK_ENTRY(SQLGetConnectOption, hDrvDbc, fOption, pvParam);

 /************************
  * REPLACE THIS COMMENT WITH SOMETHING USEFULL
  ************************/
 LOG_WARN_F_FUNC(TEXT("%s: This function not supported."), LOG_FUNCTION_NAME);
 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), LOG_FUNCTION_NAME);

 API_HOOK_RETURN(SQL_ERROR);
}
