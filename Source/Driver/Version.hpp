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

#ifndef _ODBC_DRIVER_VERSION_
#define _ODBC_DRIVER_VERSION_

#include <string>

#include "Define.h"

#ifndef ODBC_DRIVER_VERSION_MAJOR_INT
#define ODBC_DRIVER_VERSION_MAJOR_INT      1
#endif

#ifndef ODBC_DRIVER_VERSION_MINOR_INT
#define ODBC_DRIVER_VERSION_MINOR_INT      0
#endif

#ifndef ODBC_DRIVER_VERSION_BUILD_INT
#define ODBC_DRIVER_VERSION_BUILD_INT      0
#endif

#ifndef ODBC_DRIVER_VERSION_SVN_REVISION
#define ODBC_DRIVER_VERSION_SVN_REVISION   "$Revision: 912 $"
#endif

#define _STR_HELPER(x) #x
#define _STR(x) _STR_HELPER(x)

#define ODBC_DRIVER_VERSION_MAJOR    _STR(ODBC_DRIVER_VERSION_MAJOR_INT)
#define ODBC_DRIVER_VERSION_MINOR    _STR(ODBC_DRIVER_VERSION_MINOR_INT)
#define ODBC_DRIVER_VERSION_BUILD    _STR(ODBC_DRIVER_VERSION_BUILD_INT)

#define ODBC_DRIVER_VERSION_REVISION \
 std::string(ODBC_DRIVER_VERSION_SVN_REVISION).substr(11, \
  sizeof(ODBC_DRIVER_VERSION_SVN_REVISION) / sizeof(char) - 2 - 11)

namespace ODBCDriver
{

/**
Get current release version.

@return Version string.
*/
const std::string GetVersion();

}

#endif
