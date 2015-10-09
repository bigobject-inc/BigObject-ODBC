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

#ifndef _BIGOBJECT_ODBC_API_IMPL
#define _BIGOBJECT_ODBC_API_IMPL

#include "../../Driver.hpp"

#include "../../../Logging/Logging.hpp"

namespace ODBCDriver
{

/*
API_HOOK_CLASS(SQLAllocHandle)
{
 API_HOOK_CLASS_DEFAULT_IMPL(SQLAllocHandle)

 APIHookReturn PreCall()
 {
  SQLSMALLINT& nHandleType = Get<SQLSMALLINT>(0);

  LOG_DEBUG_F(logger, LOG_DECORATED_FUNCTION_NAME TEXT(": nHandleType = %d"), 
              nHandleType);

  return APIHookReturn(SQL_SUCCESS, APIHookReturn::CONTINUE);
 }
};
*/

}

#endif
