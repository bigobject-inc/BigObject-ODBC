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


#ifndef _BIGOBJECT_ODBC_CLIENT_
#define _BIGOBJECT_ODBC_CLIENT_

namespace ODBCDriver
{

/// Maximum number of characters needed to display any field.
#define MAX_DISPLAY_SIZE 640

/// Maximum number of bytes needed to store any field
#define MAX_BYTE_LENGTH 16384

/**
* Enumeration of known server data types.
*/
enum ServerType
{
 SERVER_VOID_TYPE,
 SERVER_BOOLEAN_TYPE,
 SERVER_TINYINT_TYPE,
 SERVER_SMALLINT_TYPE,
 SERVER_INT_TYPE,
 SERVER_BIGINT_TYPE,
 SERVER_DECIMAL_TYPE,
 SERVER_FLOAT_TYPE,
 SERVER_DOUBLE_TYPE,
 SERVER_STRING_TYPE,
 SERVER_DATE_TYPE,
 SERVER_TIME_TYPE,
 SERVER_DATETIME_TYPE,
 SERVER_TIMESTAMP_TYPE,
 SERVER_LIST_TYPE,
 SERVER_MAP_TYPE,
 SERVER_STRUCT_TYPE,
 SERVER_UNKNOWN_TYPE
};

/**
Enumeration of Server return values.
*/
enum ServerReturn
{
 SERVER_SUCCESS,
 SERVER_ERROR,
 SERVER_NO_MORE_DATA,
 SERVER_SUCCESS_WITH_MORE_DATA,
 SERVER_STILL_EXECUTING
};

}

#endif
