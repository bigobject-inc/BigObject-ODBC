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

#ifndef _BIGOBJECT_ODBC_CLIENT_SERVICE_IMPL_
#define _BIGOBJECT_ODBC_CLIENT_SERVICE_IMPL_

#include <boost/system/system_error.hpp>
#include <boost/thread/detail/singleton.hpp> 

#include "../Driver.hpp"
#include "../Impl.hpp"

#ifndef ODBC_DRIVER_SERVICE_IMPL
#define ODBC_DRIVER_SERVICE_IMPL CSVServiceImpl
#endif

#include INCLUDE_HPP(ODBC_DRIVER_SERVICE_IMPL)

namespace ODBCDriver
{

//typedef ServiceInterface<RestfulServiceImpl> IService;
typedef ODBC_DRIVER_SERVICE_IMPL IService;

/// Service public typedef, singleton.
typedef boost::detail::thread::singleton<IService> Service;

}

#endif
