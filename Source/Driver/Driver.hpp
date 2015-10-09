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

#ifndef _BIGOBJECT_ODBC_DRIVER
#define _BIGOBJECT_ODBC_DRIVER

// ODBC VERSION (THAT THIS DRIVER COMPLIES WITH)
#define ODBCVER 0x0351

#ifdef WIN32
#include <Windows.h>
#else
#endif

#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "../Base/Platform.hpp"
#include "../Base/TString.hpp"
#include "../Base/Debug.hpp"

#include "../Logging/Logging.hpp"

#include "Define.h"
#include "Version.hpp"

#define _ODBC_DRIVER_MACRO_MAX_PARAMS 20

// Shortcut implementation for conversion between string and tstring.
#define ODBC_STR_TO_TSTR LOG4CPLUS_STRING_TO_TSTRING
#define ODBC_TSTR_TO_STR LOG4CPLUS_TSTRING_TO_STRING

// http://stackoverflow.com/questions/4743822/
// generate-include-file-name-in-a-macro
#define INCLUDE_HPP(M) BOOST_PP_STRINGIZE(Impl/M.hpp)

#define INCLUDE_CPP(M) BOOST_PP_STRINGIZE(Impl/M.cpp)

#ifdef WIN32

extern HINSTANCE hModule;	/* Saved module handle for resources. */

#endif

#ifndef ODBC_DRIVER_CONNSTR_DATABASE_KEY
#define ODBC_DRIVER_CONNSTR_DATABASE_KEY   "DATABASE"
#endif 

#ifndef ODBC_DRIVER_CONNSTR_HOST_KEY
#define ODBC_DRIVER_CONNSTR_HOST_KEY       "HOST"
#endif 

#ifndef ODBC_DRIVER_CONNSTR_PORT_KEY
#define ODBC_DRIVER_CONNSTR_PORT_KEY       "PORT"
#endif 

#ifndef ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY
#define ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY   "LOGLEVEL"
#endif 

#ifndef ODBC_DRIVER_CONNSTR_LOGFILE_KEY
#define ODBC_DRIVER_CONNSTR_LOGFILE_KEY    "LOGFILE"
#endif 

#ifndef ODBC_DRIVER_DEFAULT_DSN
#error ODBC_DRIVER_DEFAULT_DSN define is required.
#endif

#ifndef ODBC_DRIVER_DEFAULT_DSN
#define ODBC_DRIVER_DEFAULT_DSN        ""
#endif 

#ifndef ODBC_DRIVER_DEFAULT_UID
#define ODBC_DRIVER_DEFAULT_UID        ""
#endif 

#ifndef ODBC_DRIVER_DEFAULT_PWD
#define ODBC_DRIVER_DEFAULT_PWD        ""
#endif 

#ifndef ODBC_DRIVER_DEFAULT_DATABASE
#define ODBC_DRIVER_DEFAULT_DATABASE   ""
#endif 

#ifndef ODBC_DRIVER_DEFAULT_HOST
#define ODBC_DRIVER_DEFAULT_HOST       "localhost"
#endif

#ifndef ODBC_DRIVER_DEFAULT_PORT
#define ODBC_DRIVER_DEFAULT_PORT       "80"
#endif

#ifndef ODBC_DRIVER_DEFAULT_LOGLEVEL
#ifdef _DEBUG
#define ODBC_DRIVER_DEFAULT_LOGLEVEL   "DEBUG"
#else
#define ODBC_DRIVER_DEFAULT_LOGLEVEL   "INFO"
#endif
#endif

#ifndef ODBC_DRIVER_DEFAULT_LOGFILE
#define ODBC_DRIVER_DEFAULT_LOGFILE    ""
#endif

#endif
