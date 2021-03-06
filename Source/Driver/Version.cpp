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

#include <string>

#include "Version.hpp"

namespace ODBCDriver
{

const std::string GetVersion()
{
 std::string v(ODBC_DRIVER_VERSION_MAJOR "."
               ODBC_DRIVER_VERSION_MINOR "."
               ODBC_DRIVER_VERSION_BUILD ".");
 v.append(ODBC_DRIVER_VERSION_REVISION);
 return v;
}

}
