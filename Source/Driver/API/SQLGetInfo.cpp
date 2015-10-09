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
https://msdn.microsoft.com/en-us/library/ms711681%28v=vs.85%29.aspx

Example calls:

CLR (Demo/CSharp/Shell.exe):
SQLGetInfoW ( 0x007c6310, 77, 0x02079d70, 100, 0x0658ebe4 )	SQL_SUCCESS
*/

SQLRETURN SQL_API SQLGetInfo(
 SQLHDBC ConnectionHandle, SQLUSMALLINT InfoType, SQLPOINTER InfoValuePtr,
 SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr)
{
 HDRVDBC hDbc = (HDRVDBC)ConnectionHandle;
 SQLRETURN result = SQL_ERROR;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: ConnectionHandle = 0x%08lX, InfoType = %d, ")
  TEXT("InfoValuePtr = 0x%08lX, BufferLength = %d, StringLengthPtr = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)ConnectionHandle, (long)InfoType, InfoValuePtr, 
  BufferLength, (long)StringLengthPtr);

 ODBCSettings* settings = hDbc->hDbcExtras->odbcSettings;
 if(settings == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR odbc.ini has not been setup correctly."));
  //return SQL_ERROR;
 }

 /* SANITY CHECKS */
 ODBCAPIHelper helper((SQLHDBC)hDbc, SQL_HANDLE_DBC);
 if(!helper.IsValid())
  return SQL_INVALID_HANDLE;

 API_HOOK_ENTRY(SQLGetInfo, 
  ConnectionHandle, InfoType, InfoValuePtr, BufferLength, StringLengthPtr);

 switch(InfoType)
 {
  /* 
  The following parameters are needed for MSTR v.9
     - SQL_DRIVER_VER,
     - SQL_DRIVER_ODBC_VER
     - SQL_DRIVER_NAME
     - SQL_DBMS_NAME
  */
  case SQL_DRIVER_VER:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DRIVER_VER"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 ODBC_STR_TO_TSTR(ODBCDriver::GetVersion()));
   break;

  case SQL_DRIVER_ODBC_VER:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DRIVER_ODBC_VER"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(DRIVER_ODBC_VER));
   break;

  case SQL_DRIVER_NAME:
  {
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DRIVER_NAME"));

#if defined(_WIN32) || defined(_WIN64)
   TCHAR pathBuf[301];
   TCHAR* _driverName;
#else
   tstring driverName(settings->driver);
#endif

#if defined(_WIN32) || defined(_WIN64)
   GetModuleFileName(hModule, pathBuf, sizeof(pathBuf) / sizeof(TCHAR));
   _driverName = _tcsrchr(pathBuf, '\\');

   if(_driverName == NULL)
    _driverName = _tcsrchr(pathBuf, '/');

   if(_driverName == NULL)
    _driverName = pathBuf;
   else
    _driverName++;

   tstring driverName(_driverName);
#endif

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 driverName);
   break;
  }

  case SQL_DBMS_NAME:
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DBMS_NAME"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(TEXT(ODBC_DRIVER_DBMS_NAME)));
   break;

  case SQL_SERVER_NAME: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_SERVER_NAME"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(settings->host));
   break;

  case SQL_DATA_SOURCE_NAME: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DATA_SOURCE_NAME"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(settings->dsn));
   break;

  case SQL_CURSOR_COMMIT_BEHAVIOR: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_CURSOR_COMMIT_BEHAVIOR"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 (SQLUSMALLINT)SQL_CB_PRESERVE);
   break;

  case SQL_CURSOR_ROLLBACK_BEHAVIOR: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_CURSOR_ROLLBACK_BEHAVIOR"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 //(SQLUSMALLINT)SQL_CB_CLOSE);
                                 (SQLUSMALLINT)SQL_CB_PRESERVE);
   break;

  case SQL_GETDATA_EXTENSIONS: /* ODBC 2.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_GETDATA_EXTENSIONS"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 //(SQLINTEGER)SQL_GD_ANY_COLUMN);
                                 (SQLINTEGER)(SQL_GD_ANY_COLUMN | 
                                              SQL_GD_ANY_ORDER | 
                                              SQL_GD_BLOCK | 
                                              SQL_GD_BOUND));
   break;

  case SQL_MAX_CONCURRENT_ACTIVITIES: /* ODBC 1.0 */
  // case SQL_ACTIVE_STATEMENTS: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_MAX_CONCURRENT_ACTIVITIES"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 (SQLUSMALLINT)0);
   break;

  case SQL_DATA_SOURCE_READ_ONLY: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DATA_SOURCE_READ_ONLY"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(TEXT("N")));
   break;

  case SQL_SEARCH_PATTERN_ESCAPE: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_SEARCH_PATTERN_ESCAPE"));

   // https://msdn.microsoft.com/en-us/library/ms710171%28v=vs.85%29.aspx
   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(TEXT("\\")));
   break;

  case SQL_CORRELATION_NAME: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_CORRELATION_NAME"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 (SQLUSMALLINT)SQL_CN_ANY);
   break;

  case SQL_NON_NULLABLE_COLUMNS: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_NON_NULLABLE_COLUMNS"));

   // Apache Kylin: SQL_NNC_NULL
   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 (SQLUSMALLINT)SQL_NNC_NON_NULL);
   break;

  case SQL_QUALIFIER_NAME_SEPARATOR: /* ODBC 1.0 */
  // case SQL_CATALOG_NAME_SEPARATOR: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_QUALIFIER_NAME_SEPARATOR"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 //tstring(TEXT(".")));
                                 tstring(TEXT("")));
   break;

  case SQL_FILE_USAGE: /* ODBC 2.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_NON_NULLABLE_COLUMNS"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 (SQLUSMALLINT)SQL_FILE_NOT_SUPPORTED);
   break;

  case SQL_QUALIFIER_TERM: /* ODBC 1.0 */
  // case SQL_CATALOG_TERM: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_QUALIFIER_TERM"));

   // Apache Kylin: database
   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 //tstring(TEXT("catalog")));
                                 tstring(TEXT("")));
   break;

  case SQL_MAX_OWNER_NAME_LEN: /* ODBC 1.0 */
  // case SQL_MAX_SCHEMA_NAME_LEN: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_MAX_OWNER_NAME_LEN"));

   result = helper.CopyToSQLData(InfoValuePtr, StringLengthPtr,
                                 //(SQLUSMALLINT)128);
                                 (SQLUSMALLINT)0);
   break;

  case SQL_IDENTIFIER_QUOTE_CHAR: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_IDENTIFIER_QUOTE_CHAR"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(TEXT("\"")));
   break;

  case SQL_DATABASE_NAME: /* Support for old ODBC 1.0 Apps */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DATABASE_NAME"));

   if(settings == NULL)
    break; // SQL_ERROR.

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 tstring(settings->database));
   break;

  case SQL_DBMS_VER: /* ODBC 1.0 */
   LOG_DEBUG_FUNC(TEXT("Getting SQL_DBMS_VER"));

   result = helper.CopyToSQLData(InfoValuePtr, BufferLength, StringLengthPtr,
                                 ODBC_STR_TO_TSTR(ODBCDriver::GetVersion()));
   break;

  // # TODO: Implement these parameters if needed

  case SQL_ACCESSIBLE_PROCEDURES: /* ODBC 1.0 */
  case SQL_ACCESSIBLE_TABLES: /* ODBC 1.0 */
  case SQL_ACTIVE_CONNECTIONS: /* ODBC 1.0 */
  case SQL_ALTER_TABLE: /* ODBC 2.0 */
  case SQL_BOOKMARK_PERSISTENCE: /* ODBC 2.0 */
  case SQL_COLUMN_ALIAS: /* ODBC 2.0 */
  case SQL_CONCAT_NULL_BEHAVIOR: /* ODBC 1.0 */
  case SQL_CONVERT_BIGINT:
  case SQL_CONVERT_BINARY:
  case SQL_CONVERT_BIT:
  case SQL_CONVERT_CHAR:
  case SQL_CONVERT_DATE:
  case SQL_CONVERT_DECIMAL:
  case SQL_CONVERT_DOUBLE:
  case SQL_CONVERT_FLOAT:
  case SQL_CONVERT_INTEGER:
  case SQL_CONVERT_LONGVARBINARY:
  case SQL_CONVERT_LONGVARCHAR:
  case SQL_CONVERT_NUMERIC:
  case SQL_CONVERT_REAL:
  case SQL_CONVERT_SMALLINT:
  case SQL_CONVERT_TIME:
  case SQL_CONVERT_TIMESTAMP:
  case SQL_CONVERT_TINYINT:
  case SQL_CONVERT_VARBINARY:
  case SQL_CONVERT_VARCHAR: /* ODBC 1.0 */
  case SQL_CONVERT_FUNCTIONS: /* ODBC 1.0 */
  case SQL_DEFAULT_TXN_ISOLATION: /* ODBC 1.0 */
  case SQL_EXPRESSIONS_IN_ORDERBY: /* ODBC 1.0 */
  case SQL_FETCH_DIRECTION: /* ODBC 1.0 */
  case SQL_GROUP_BY: /* ODBC 2.0 */
  case SQL_IDENTIFIER_CASE: /* ODBC 1.0 */
  case SQL_KEYWORDS: /* ODBC 2.0 */
  case SQL_LIKE_ESCAPE_CLAUSE: /* ODBC 2.0 */
  case SQL_LOCK_TYPES: /* ODBC 2.0 */
  case SQL_MAX_BINARY_LITERAL_LEN: /* ODBC 2.0 */
  case SQL_MAX_CHAR_LITERAL_LEN: /* ODBC 2.0 */
  case SQL_MAX_COLUMN_NAME_LEN: /* ODBC 1.0 */
  case SQL_MAX_COLUMNS_IN_GROUP_BY: /* ODBC 2.0 */
  case SQL_MAX_COLUMNS_IN_INDEX: /* ODBC 2.0 */
  case SQL_MAX_COLUMNS_IN_ORDER_BY: /* ODBC 2.0 */
  case SQL_MAX_COLUMNS_IN_SELECT: /* ODBC 2.0 */
  case SQL_MAX_COLUMNS_IN_TABLE: /* ODBC 2.0 */
  case SQL_MAX_CURSOR_NAME_LEN: /* ODBC 1.0 */
  case SQL_MAX_INDEX_SIZE: /* ODBC 2.0 */
  case SQL_MAX_PROCEDURE_NAME_LEN: /* ODBC 1.0 */
  case SQL_MAX_QUALIFIER_NAME_LEN: /* ODBC 1.0 */
  case SQL_MAX_ROW_SIZE: /* ODBC 2.0 */
  case SQL_MAX_ROW_SIZE_INCLUDES_LONG: /* ODBC 2.0 */
  case SQL_MAX_STATEMENT_LEN: /* ODBC 2.0 */
  case SQL_MAX_TABLE_NAME_LEN: /* ODBC 1.0 */
  case SQL_MAX_TABLES_IN_SELECT: /* ODBC 2.0 */
  case SQL_MAX_USER_NAME_LEN:
  case SQL_MULT_RESULT_SETS: /* ODBC 1.0 */
  case SQL_MULTIPLE_ACTIVE_TXN: /* ODBC 1.0 */
  case SQL_NEED_LONG_DATA_LEN: /* ODBC 2.0 */
  case SQL_NULL_COLLATION: /* ODBC 2.0 */
  case SQL_NUMERIC_FUNCTIONS: /* ODBC 1.0 */
  case SQL_ODBC_API_CONFORMANCE: /* ODBC 1.0 */
  case SQL_ODBC_SAG_CLI_CONFORMANCE: /* ODBC 1.0 */
  case SQL_ODBC_SQL_CONFORMANCE: /* ODBC 1.0 */
  case SQL_ODBC_SQL_OPT_IEF: /* ODBC 1.0 */
  case SQL_OJ_CAPABILITIES: /* ODBC 2.01 */
  case SQL_ORDER_BY_COLUMNS_IN_SELECT: /* ODBC 2.0 */
  case SQL_OUTER_JOINS: /* ODBC 1.0 */
  case SQL_OWNER_TERM: /* ODBC 1.0 */
  case SQL_OWNER_USAGE: /* ODBC 2.0 */
  case SQL_POS_OPERATIONS: /* ODBC 2.0 */
  case SQL_POSITIONED_STATEMENTS: /* ODBC 2.0 */
  case SQL_PROCEDURE_TERM: /* ODBC 1.0 */
  case SQL_PROCEDURES: /* ODBC 1.0 */
  case SQL_QUALIFIER_LOCATION: /* ODBC 2.0 */
  case SQL_QUALIFIER_USAGE: /* ODBC 2.0 */
  case SQL_QUOTED_IDENTIFIER_CASE: /* ODBC 2.0 */
  case SQL_ROW_UPDATES: /* ODBC 1.0 */
  case SQL_SCROLL_CONCURRENCY: /* ODBC 1.0 */
  case SQL_SCROLL_OPTIONS: /* ODBC 1.0 */
  case SQL_SPECIAL_CHARACTERS: /* ODBC 2.0 */
  case SQL_STATIC_SENSITIVITY: /* ODBC 2.0 */
  case SQL_STRING_FUNCTIONS: /* ODBC 1.0 */
  case SQL_SUBQUERIES: /* ODBC 2.0 */
  case SQL_SYSTEM_FUNCTIONS: /* ODBC 1.0 */
  case SQL_TABLE_TERM: /* ODBC 1.0 */
  case SQL_TIMEDATE_ADD_INTERVALS: /* ODBC 2.0 */
  case SQL_TIMEDATE_DIFF_INTERVALS: /* ODBC 2.0 */
  case SQL_TIMEDATE_FUNCTIONS: /* ODBC 1.0 */
  case SQL_TXN_CAPABLE: /* ODBC 1.0 */
  case SQL_TXN_ISOLATION_OPTION: /* ODBC 1.0 */
  case SQL_UNION: /* ODBC 2.0 */
  case SQL_USER_NAME: /* ODBC 1.0 */
  case /*SQL_CREATE_VIEW*/134:
  case /*SQL_STATIC_CURSOR_ATTRIBUTES1*/167:
   LOG_ERROR_F_FUNC(
    TEXT("SQL_ERROR %s with input %d not currently supported."), 
    LOG_FUNCTION_NAME, InfoType);
   break;

  default:
   LOG_ERROR_F_FUNC(
    TEXT("SQL_ERROR %s with unrecognized input parameter %d."),
    LOG_FUNCTION_NAME, InfoType);
   break;
 }

 LOG_WARN_F_FUNC(TEXT("%s: This function partially supported."), 
                 LOG_FUNCTION_NAME);

 helper.LogSQLReturn(LOG_FUNCTION_NAME, result);

 API_HOOK_RETURN(result);
}
