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

#ifndef _ODBC_DRIVER_IMPL_DEFINE_
#define _ODBC_DRIVER_IMPL_DEFINE_

#define ODBC_DRIVER_VERSION_MAJOR_INT      0
#define ODBC_DRIVER_VERSION_MINOR_INT      11
#define ODBC_DRIVER_VERSION_BUILD_INT      2

#define ODBC_DRIVER_VERSION_SVN_REVISION   "$Revision: 1002 $"

#define ODBC_DRIVER_COMPANY     "BigObject Inc."
#define ODBC_DRIVER_COPYRIGHT   "Copyright 2015"

#define ODBC_DRIVER_NAME        "BigObject ODBC Driver"
#define ODBC_DRIVER_SHORT_NAME  "BigObjectODBC"

#define ODBC_DRIVER_DBMS_NAME   "BigObject"

#define ODBC_DRIVER_DSN_CFG_CAPTION "BigObject ODBC DSN Configuration Dialog"

/* Default connection parameters */
/*
SCAN <RESOURCE_ID> <START> <END> <PAGE>

START and END positions are inclusive. PAGE defines the chunk size.
The maximum allowed data chunk size is 1000.
*/
#define ODBC_DRIVER_MAX_BUFFERED_ROWS  1000

#define ODBC_DRIVER_CONNSTR_DATABASE_KEY "DATABASE"
#define ODBC_DRIVER_CONNSTR_HOST_KEY     "HOST"
#define ODBC_DRIVER_CONNSTR_PORT_KEY     "PORT"
#define ODBC_DRIVER_CONNSTR_LOGLEVEL_KEY "LOGLEVEL"
#define ODBC_DRIVER_CONNSTR_LOGFILE_KEY  "LOGFILE"

#define ODBC_DRIVER_DEFAULT_DSN        "BigObject"
#define ODBC_DRIVER_DEFAULT_UID        ""
#define ODBC_DRIVER_DEFAULT_PWD        ""
#define ODBC_DRIVER_DEFAULT_DATABASE   ""
#define ODBC_DRIVER_DEFAULT_HOST       "localhost"
#define ODBC_DRIVER_DEFAULT_PORT       "9090"

#ifdef _DEBUG
#define ODBC_DRIVER_DEFAULT_LOGLEVEL   "DEBUG"
#else
#define ODBC_DRIVER_DEFAULT_LOGLEVEL   "ERROR"
#endif
#define ODBC_DRIVER_DEFAULT_LOGFILE    ""

#endif
