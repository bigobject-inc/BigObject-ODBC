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


#ifndef _BIGOBJECT_ODBC_DRIVEREXTRAS_H
#define _BIGOBJECT_ODBC_DRIVEREXTRAS_H

/**********************************************
 * KEEP IT SIMPLE; PUT ALL DRIVER INCLUDES HERE 
 * THEN EACH DRIVER MODULE JUST INCLUDES THIS ONE FILE
 **********************************************/
#ifdef WIN32
#else
#include <sys/types.h>
#endif

#include <cstdlib>
#include <cstring>
#include <ctime>

#include "../../Logging/Logging.hpp"

#include "../Impl.hpp"

#include "../Client/Service.hpp"
#include "../Client/ServiceImpl.hpp"

#include "Descriptor.hpp"
#include "Diagnostic.hpp"
#include "APIHook.hpp"

/********************************************************
* these limits are originally defined in ini.h         *
* but are needed to implement ODBCINSTGetProperties    *
* for the Driver Setup                                 *
********************************************************/

#define     INI_MAX_LINE            1000
#define     INI_MAX_OBJECT_NAME     INI_MAX_LINE
#define     INI_MAX_PROPERTY_NAME   INI_MAX_LINE
#define     INI_MAX_PROPERTY_VALUE  INI_MAX_LINE

#ifdef WIN32
#define PATH_MAX MAX_PATH
#else
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#endif

#ifdef PATH_MAX
#define ODBC_FILENAME_MAX         PATH_MAX
#elif MAXPATHLEN
#define ODBC_FILENAME_MAX         MAXPATHLEN
#else
#define ODBC_FILENAME_MAX         FILENAME_MAX
#endif

/**********************************************
 * ENVIRONMENT: DRIVER SPECIFIC STUFF THAT NEEDS TO BE STORED 
 * IN THE DRIVERS ENVIRONMENT
 **********************************************/

#define ODBC_ENV_MAGIC  0x05280802

typedef struct tENVEXTRAS
{
 int magic;	/// Magic cookie.

#if defined(_WIN32) || defined(_WIN64)
 CRITICAL_SECTION cs;	/// For serializing most APIs.
#endif

 ////////////////////////////////////////////// 
 // SQLSetEnvAttr
 //////////////////////////////////////////////
 SQLINTEGER  attrODBCVersion; // SQL_ATTR_ODBC_VERSION                200
 SQLINTEGER  attrConnPooling; // SQL_ATTR_CONNECTION_POOLING          201
 SQLINTEGER  attrCPMatch;     // SQL_ATTR_CP_MATCH                    202
 SQLINTEGER  attrOutputNTS;   // SQL_ATTR_OUTPUT_NTS
 //////////////////////////////////////////////

} ENVEXTRAS, *HENVEXTRAS;

/**********************************************
 * CONNECTION: DRIVER SPECIFIC STUFF THAT NEEDS TO BE STORED IN THE DRIVERS
 * CONNECTIONS
 **********************************************/

/**
Structure to hold all the connection attributes for a specific
connection (used for both registry and file, DSN and DRIVER).
*/
typedef struct tODBCSETTINGS
{
 TCHAR	dsn[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	driver[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	uid[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	password[INI_MAX_PROPERTY_VALUE + 1];

 TCHAR	host[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	database[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	port[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	logLevel[INI_MAX_PROPERTY_VALUE + 1];
 TCHAR	logFile[INI_MAX_PROPERTY_VALUE + 1];
} ODBCSettings;

#define ODBC_DBC_MAGIC  0x03300416

typedef struct tDBCEXTRAS
{
 int magic;	/// Magic cookie.

#if defined(_WIN32) || defined(_WIN64)
 CRITICAL_SECTION cs;	/// For serializing most APIs.
#endif

 ////////////////////////////////////////////// 
 // SQLSetConnectAttr
 //////////////////////////////////////////////
 SQLUINTEGER accessMode;
 SQLUINTEGER autoIPD;
 SQLUINTEGER asyncEnable;
 SQLUINTEGER autoCommit;
 SQLUINTEGER timeOut;
 SQLUINTEGER loginTimeOut;
 SQLUINTEGER metaDataID;
 SQLUINTEGER odbcCursors;
 SQLUINTEGER window;
 SQLUINTEGER txnIsolation;
 SQLUINTEGER maxRows;
 SQLUINTEGER queryTimeout;
 //////////////////////////////////////////////

 /// Settings read from odbc.ini.
 ODBCSettings* odbcSettings; 

 /// Handle to the Server connection.
 ODBCDriver::Connection* conn; 

} DBCEXTRAS, *HDBCEXTRAS;

/**********************************************
 * STATEMENT: DRIVER SPECIFIC STUFF THAT NEEDS TO BE STORED 
 * IN THE DRIVERS STATEMENTS
 **********************************************/
typedef struct tSTMTEXTRAS
{
 ////////////////////////////////////////////// 
 // SQLSetStmtAttr
 //////////////////////////////////////////////
 SQLUINTEGER asyncEnable;
 SQLUINTEGER concurrency;
 SQLUINTEGER cursorScroll;
 SQLUINTEGER cursorSensitivity;
 SQLUINTEGER cursorType;
 SQLUINTEGER autoIPD;
 SQLUINTEGER keysetSize;
 SQLUINTEGER metaDataID;
 SQLUINTEGER noScan;
 SQLUINTEGER queryTimeout;
 SQLUINTEGER retrieveData;
 //////////////////////////////////////////////

 ////////////////////////////////////////////// 
 // Descriptors
 ////////////////////////////////////////////// 
 ARD ard; ///< Application row descriptor.
 IRD ird; ///< Implementation row descriptor.

 APD apd; ///< Application parm descriptor.
 IPD ipd; ///< Implementation parm descriptor.
 //////////////////////////////////////////////

 /// Handle to the resultset generated by a query execution.
 ODBCDriver::ResultSet* resultset; 

} STMTEXTRAS, *HSTMTEXTRAS;

/**********************************************
 * Shadow functions
 *
 * There are times when a function needs to call another function to use 
 * common functionality. When the called function is part of the ODBC API 
 * (an entry point to the driver) bad things can happen. The linker will get 
 * confused and call the same-named function elsewhere in the namespace 
 * (ie the Driver Manager). To get around this you create a shadow function - 
 * a function with a slightly different name that will not be confused with 
 * others - and then put most if not all of the functionality in there for
 * common use.
 *
 **********************************************/

SQLRETURN _AllocEnv(SQLHENV *phDrvEnv);

SQLRETURN _AllocConnect(SQLHENV hDrvEnv, SQLHDBC *phDrvDbc);

SQLRETURN _AllocStmt(SQLHDBC hDrvDbc, SQLHSTMT *phDrvStmt);

SQLRETURN _FreeStmt(SQLHSTMT hDrvStmt);

SQLRETURN _FreeConnect(SQLHDBC hDrvDbc);

SQLRETURN _FreeEnv(SQLHENV hDrvEnv);

/**********************************************
* Utilities
**********************************************/

#include "Helper.hpp"

#ifdef ODBC_DRIVER_API_IMPL
#include INCLUDE_HPP(ODBC_DRIVER_API_IMPL)
#endif

#endif
