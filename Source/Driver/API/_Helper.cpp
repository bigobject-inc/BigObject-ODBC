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

#ifdef WIN32
#else
// http://blog.csdn.net/delphiwcdj/article/details/8476547
#include <sys/types.h>
#include <sys/syscall.h>
#include <bits/syscall.h>

#include <unistd.h>

#define gettid() syscall(SYS_gettid)

#ifdef __x86_64
#else
#define LOWORD(a) (a & 0xffffffffU)
#define HIWORD(a) (a >> 32)
#endif

#endif

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/algorithm/string/replace.hpp>

#include "Driver.hpp"
#include "Helper.hpp"

ConnectionStringParser::ConnectionStringMapPtr ConnectionStringParser::Parse(
 tstring& connStr)
{
 itstringstream connStrStream(
  boost::replace_all_copy(connStr, TEXT(";"), TEXT("\n")));

#ifdef UNICODE
 typedef boost::property_tree::wptree tptree;
#else
 typedef boost::property_tree::ptree tptree;
#endif

 tptree pt;

 try
 {
  boost::property_tree::read_ini(connStrStream, pt);
 }
 catch(boost::property_tree::ini_parser_error& ex)
 {
  LOG_ERROR_F_FUNC(
   TEXT("ConnectionStringParser: Failed parse connection string: %s (%s)"),
   connStr.c_str(), ex.what());

  return ConnectionStringMapPtr();
 }

 ConnectionStringMapPtr map(new ConnectionStringMap);

 tptree::iterator i = pt.begin();
 tptree::iterator iEnd = pt.end();
 for(; i != iEnd; ++i)
 {
  tptree::value_type& nameValue = *i;

  const tstring& name = nameValue.first;
  const tstring& value = nameValue.second.get_value<tstring>();

  map->insert(std::make_pair(name, value));
 }

 return map;
}

ODBCAPIHelper::ODBCAPIHelper(SQLHANDLE _handle, int _type)
 : handle(_handle), type(_type), owner(0)
{
}

ODBCAPIHelper::~ODBCAPIHelper()
{
 Unlock();
}

bool ODBCAPIHelper::IsValid()
{
 if(type == SQL_HANDLE_ENV)
 {
  HDRVENV hEnv = (HDRVENV)handle;
  if(hEnv == SQL_NULL_HENV)
   return false;

  if(!hEnv || !hEnv->hEnvExtras || hEnv->hEnvExtras->magic != ODBC_ENV_MAGIC)
   return false;
 }
 else if(type == SQL_HANDLE_DBC)
 {
  HDRVDBC hDbc = (HDRVDBC)handle;
  if(hDbc == SQL_NULL_HDBC)
   return false;

  if(!hDbc || !hDbc->hDbcExtras || hDbc->hDbcExtras->magic != ODBC_DBC_MAGIC)
   return false;
 }
 else if(type == SQL_HANDLE_STMT)
 {
  HDRVSTMT hStmt = (HDRVSTMT)handle;

  if(hStmt == SQL_NULL_HSTMT)
   return false;

  if(hStmt->hStmtExtras == NULL)
   return false;
 }
 else
 {
  LOG_DEBUG_F_FUNC(TEXT("%s: Invalid type."), LOG_FUNCTION_NAME);
  return false;
 }

 return true;
}

void ODBCAPIHelper::Lock()
{
 if(owner != 0)
 {
#ifdef WIN32
  if(type == SQL_HANDLE_ENV)
   EnterCriticalSection(&((HDRVENV)handle)->hEnvExtras->cs);
  else if(type == SQL_HANDLE_DBC)
   EnterCriticalSection(&((HDRVDBC)handle)->hDbcExtras->cs);

  owner = GetCurrentThreadId();
#else
  owner = gettid();
#endif
 }
}

void ODBCAPIHelper::Unlock()
{
 if(owner != 0)
 {
#ifdef WIN32
  owner = 0;

  if(type == SQL_HANDLE_ENV)
   LeaveCriticalSection(&((HDRVENV)handle)->hEnvExtras->cs);
  else if(type == SQL_HANDLE_DBC)
   LeaveCriticalSection(&((HDRVDBC)handle)->hDbcExtras->cs);
#else
  owner = 0;
#endif
 }
}

SQLRETURN ODBCAPIHelper::Prepare(SQLTCHAR *szSqlStr, SQLINTEGER nSqlStrLength)
{
 HDRVSTMT hStmt = (HDRVSTMT)handle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvStmt = 0x%08lX, szSqlStr = %s, nSqlStrLength = %d"),
  LOG_FUNCTION_NAME, (long)hStmt, szSqlStr, nSqlStrLength);

 if(szSqlStr == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No SQL to process."));
  return SQL_ERROR;
 }

 if(hStmt->pszQuery != NULL)
 {
  LOG_WARN_FUNC(TEXT("Statement already in use."));
  //return SQL_ERROR;

  if(NULL != hStmt->pszQuery) // Reset prepared SQL.
  {
   free(hStmt->pszQuery);
   hStmt->pszQuery = NULL;
  }
 }

 /* allocate and copy statement to buffer
 (process escape sequences and parameter tokens as required) */
 hStmt->pszQuery = (SQLTCHAR*)_tcsdup(
  GetSafeTString(szSqlStr, nSqlStrLength).c_str());

 if(NULL == hStmt->pszQuery)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: Memory allocation error."));
  return SQL_ERROR;
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

SQLRETURN ODBCAPIHelper::Execute()
{
 HDRVSTMT hStmt = (HDRVSTMT)handle;
 RETCODE rc;

 LOG_DEBUG_F_FUNC(TEXT("%s: hStmt = 0x%08lX"), LOG_FUNCTION_NAME, 
                  (long)hStmt);

 if(hStmt->pszQuery == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No prepared statement."));
  return SQL_ERROR;
 }

 /**************************
 * Free any current results
 **************************/
 if(hStmt->hStmtExtras->resultset != NULL ||
    hStmt->hStmtExtras->ird.rowDesc != NULL)
 {
  FreeResults();
 }

 // #TODO: Resolve bound parameters (from SQLBindParameters) 
 //        (need to implement SQLBindParameters first).
 // #TODO: Determine type of query to figure out proper return values 
 //        (i.e. UPDATE/INSERT/DELETE).

 /**************************
 * send prepared query to server
 **************************/
#ifdef ODBC_DRIVER_MAX_BUFFERED_ROWS
 const std::size_t maxBufRows = ODBC_DRIVER_MAX_BUFFERED_ROWS;
#else
 const std::size_t maxBufRows = 2048
#endif

  // #TODO: maxBufRows should be got from SQL_ATTR_MAX_ROWS.

  ODBCDriver::IService& service = ODBCDriver::Service::instance();

 try
 {
  ODBCDriver::ServerReturn serverRet = service.Execute(
   ((HDRVDBC)hStmt->hDbc)->hDbcExtras->conn,
   tstring((TCHAR*)hStmt->pszQuery),
   &(hStmt->hStmtExtras->resultset), maxBufRows);
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(TEXT("SQL_ERROR Query failed. %s"), ex.What());

  FreeResults();
  return SQL_ERROR;
 }

 /**************************
 * prepare the resultset for consumption
 **************************/
 rc = PrepareResults();

 LogSQLReturn(LOG_FUNCTION_NAME, rc);
 
 return rc;
}

SQLRETURN ODBCAPIHelper::FetchScroll(SQLUSMALLINT nOrientation, SQLLEN nOffset,
                                     SQLULEN *rowCountPtr,
                                     SQLUSMALLINT *rowStatusArray)
{
 HDRVSTMT hStmt = (HDRVSTMT)handle;

 LOG_DEBUG_F_FUNC(TEXT("%s: hDrvStmt = 0x%08lX, ")
  TEXT("nOrientation = %d, nOffset = %d"),
  LOG_FUNCTION_NAME,
  (long)hStmt, nOrientation, nOffset);

 if(hStmt->hStmtExtras->cursorType == SQL_CURSOR_FORWARD_ONLY &&
    nOrientation != SQL_FETCH_NEXT)
 {
  LOG_DEBUG_F_FUNC(TEXT("%s: HY106 Fetch type out of range"),
   LOG_FUNCTION_NAME);
  return SQL_ERROR;
 }

 SQLULEN rowCount = 0;
 SQLULEN *pRowCountPtr = (hStmt->hStmtExtras->ird.rowsProcessedPtr ?
                          hStmt->hStmtExtras->ird.rowsProcessedPtr :
                          &rowCount);
 SQLUSMALLINT *pRowStatusArray = (hStmt->hStmtExtras->ird.arrayStatusPtr ?
                                  hStmt->hStmtExtras->ird.arrayStatusPtr :
                                  NULL);

 // Use specified pointer.
 if(rowCountPtr)
  pRowCountPtr = rowCountPtr;

 if(rowStatusArray)
  pRowStatusArray = rowStatusArray;

 // note
 // fetchoffset is treated as the number of rows to fetch
 SQLULEN pFetchOffset = hStmt->hStmtExtras->ard.rowArraySize;

 bool flgNoData = false;
 SQLULEN  i, n1, n2;

 // CALLER SAFE

 // caller safe for row fetched
 if(pRowCountPtr)
  *pRowCountPtr = 0;

 // caller safe for each row status
 if(pRowStatusArray)
 {
  /*
  memset(pRowStatusArray, SQL_ROW_NOROW,
   sizeof(*pRowStatusArray) * pFetchOffset);
  */
  for(int i = 0; i < pFetchOffset; i++) 
   pRowStatusArray[i] = SQL_ROW_NOROW;
 }

 ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 // MAIN LOOP to fetch rowset number of rows
 // loop to fetch the rows
 for(i = 0, n1 = 0, n2 = 0; i < pFetchOffset && !flgNoData; ++i)
 {
  LOG_DEBUG_F_FUNC(TEXT("Get One Row: %d"), i);

  /************************
  * get next row
  ************************/
  try
  {
   serverRet = service.Fetch(hStmt->hStmtExtras->resultset,
                             nOrientation, nOffset);
  }
  catch(ODBCDriver::ClientException& ex)
  {
   LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: Fetch failed. %s"), ex.What());
   return SQL_ERROR; // not continuing
  }

  /* Signal for no more rows to fetch */
  if(serverRet == ODBCDriver::SERVER_NO_MORE_DATA)
  {
   flgNoData = true; // can continue
   break;
  }

  // case SQL_SUCCESS:

  // LOOP to fetch cols of one row
  SQLSMALLINT columnNum;

  // loop to put data in all bound cols
  for(columnNum = 1; columnNum <= hStmt->hStmtExtras->ird.descCount;
      columnNum++)
  {
   ARDItem* ardcol = _SQLGetARDItem(&hStmt->hStmtExtras->ard, columnNum);

   if(!ardcol)
    continue;

   /* If the column has a buffer bound to it... */
   if(ardcol->dataPtr != NULL)
   {
    // get data using _SQLFetchCol
    SQLRETURN s = _SQLFetchCol(ardcol, i);

    // update row status
    switch(s)
    {
     case SQL_SUCCESS:
      if(pRowStatusArray && pRowStatusArray[i] == SQL_ROW_NOROW)
       pRowStatusArray[i] = SQL_ROW_SUCCESS;

      break;

     case SQL_SUCCESS_WITH_INFO:
      ++n1; // rows with info

      if(pRowStatusArray)
       pRowStatusArray[i] = SQL_ROW_SUCCESS_WITH_INFO;

      break;

     default:
      ++n2; // no. of rows with error

      if(pRowStatusArray)
       pRowStatusArray[i] = SQL_ROW_ERROR;
    }
   }
  }

  // update the number of rows fetched
  if(pRowCountPtr)
   *pRowCountPtr = i + 1;
 }

 // check if no data
 if(flgNoData && i <= 0)
 {
  LOG_INFO_FUNC(TEXT("SQL_NO_DATA"));
  return SQL_NO_DATA;
 }
 // check if all error
 else if(n2 == i)
 {
  return SQL_ERROR;
 }
 // check if any success with info
 else if(n1 > 0)
 {
  return SQL_SUCCESS_WITH_INFO;
 }
 // all success
 else
 {
  return SQL_SUCCESS;
 }
}

SQLRETURN ODBCAPIHelper::GetData(
 SQLUSMALLINT nCol, SQLSMALLINT nTargetType, SQLPOINTER pTarget,
 SQLLEN nTargetLength, SQLLEN *pnLengthOrIndicator)
{
 HDRVSTMT hStmt = (HDRVSTMT)handle;

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hDrvStmt = 0x%08lX, nCol = %d, ")
  TEXT("nTargetType = %d, pTarget = 0x%08lX, ")
  TEXT("nTargetLength = %d, pnLengthOrIndicator = 0x%08lX"),
  LOG_FUNCTION_NAME,
  (long)hStmt, nCol,
  nTargetType, (long)pTarget,
  nTargetLength, (long)pnLengthOrIndicator);

 if(pTarget == NULL)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR Target buffer pointer is NULL."));
  return SQL_ERROR;
 }

 ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 bool hasResults = false;

 try
 {
  serverRet = service.HasResults(hStmt->hStmtExtras->resultset, &hasResults);
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(TEXT("SQL_ERROR: check for results failed. %s"), ex.What());
  return SQL_ERROR;
 }

 if(!hasResults)
 {
  LOG_ERROR_FUNC(TEXT("SQL_ERROR: No query results."));
  return SQL_ERROR;
 }

 /**********************************************************************
 * CONVERT TO A DEFAULY SQL TYPE IF SQL_C_DEFAULT IS SPECIFIED
 **********************************************************************/
 if(nTargetType == SQL_C_DEFAULT)
 {
  assert(hStmt->hStmtExtras->ird.rowDesc != NULL);

  COLUMNHDR *columnHeader;
  columnHeader = _SQLGetIRDItem(&hStmt->hStmtExtras->ird, nCol);

  nTargetType = MapDefaultType(nTargetType,
                               columnHeader->nSQL_DESC_CONCISE_TYPE);

  LOG_DEBUG_F_FUNC(TEXT("SQL_C_DEFAULT for column %i => %d"),
                   nCol, nTargetType);
 }

 /**********************************************************************
 * EXTRACT THE FIELD DATA BASED ON C TYPE
 **********************************************************************/
 ODBCDriver::Row* row;

 try
 {
  serverRet = service.GetCurrentRow(hStmt->hStmtExtras->resultset, &row);
  if(serverRet != ODBCDriver::SERVER_SUCCESS)
  {
   LOG_ERROR_FUNC(TEXT("SQL_ERROR: Unable to get current row."));
   return SQL_ERROR;
  }
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR: Unable to get current row. %s"),
   nCol, ex.What());
  return SQL_ERROR;
 }

 SQLRETURN helperReturn = SQL_SUCCESS;

 SQLLEN dataByteSize;
 bool isNullValue;
 int intBuffer;
 long longBuffer;
 unsigned long ulongBuffer;
 int64_t i64Buffer;
 uint64_t i64uBuffer;
 double doubleBuffer;
 struct tagDATE_STRUCT dateBuffer = { 0 };
 struct tagTIME_STRUCT timeBuffer = { 0 };
 struct tagTIMESTAMP_STRUCT timestampBuffer = { 0 };

 switch(nTargetType)
 {
  case SQL_C_CHAR:
  case SQL_C_WCHAR:
  {
   tstring value;

   try
   {
    serverRet = service.GetFieldAsString(row, nCol - 1, value, &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get C string data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   if(nTargetType == SQL_C_CHAR)
   {
    helperReturn = CopyToSQLData(
     pTarget, nTargetLength, &dataByteSize, ODBC_TSTR_TO_STR(value));
   }
   else // SQL_C_TCHAR
   {
    helperReturn = CopyToSQLData(
     pTarget, nTargetLength, &dataByteSize, value);
   }

   break;
  }

  case SQL_C_TINYINT: /* For ODBC 2.x compatibility */
  case SQL_C_STINYINT:
   try
   {
    serverRet = service.GetFieldAsInt(row, nCol - 1, &intBuffer, &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get signed tiny int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SCHAR*)pTarget) = (SCHAR)intBuffer;
   dataByteSize = sizeof(SCHAR);
   break;

  case SQL_C_UTINYINT:
   try
   {
    serverRet = service.GetFieldAsInt(row, nCol - 1, &intBuffer, &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get unsigned tiny int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((UCHAR*)pTarget) = (UCHAR)intBuffer;
   dataByteSize = sizeof(UCHAR);
   break;

  case SQL_C_SHORT: /* For ODBC 2.x compatibility */
  case SQL_C_SSHORT:
   try
   {
    serverRet = service.GetFieldAsInt(row, nCol - 1, &intBuffer, &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get signed short int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SQLSMALLINT*)pTarget) = (SQLSMALLINT)intBuffer;
   dataByteSize = sizeof(SQLSMALLINT);
   break;

  case SQL_C_USHORT:
   try
   {
    serverRet = service.GetFieldAsInt(row, nCol - 1, &intBuffer, &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get unsigned short int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SQLUSMALLINT*)pTarget) = (SQLUSMALLINT)intBuffer;
   dataByteSize = sizeof(SQLUSMALLINT);
   break;

  case SQL_C_LONG: /* For ODBC 2.x compatibility */
  case SQL_C_SLONG:
   try
   {
    serverRet = service.GetFieldAsLong(row, nCol - 1, &longBuffer, 
                                       &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get signed long int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SQLINTEGER*)pTarget) = (SQLINTEGER)longBuffer;
   dataByteSize = sizeof(SQLINTEGER);
   break;

  case SQL_C_ULONG:
   try
   {
    serverRet = service.GetFieldAsULong(row, nCol - 1, &ulongBuffer, 
                                        &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get unsigned long int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SQLUINTEGER*)pTarget) = (SQLUINTEGER)ulongBuffer;
   dataByteSize = sizeof(SQLUINTEGER);
   break;

   /*
   #TODO: BIG INTS DONT EXIST FOR ODBC VERSIONS BELOW 3.x or
   those without ODBCINT64
   */

  case SQL_C_SBIGINT:
   try
   {
    serverRet = service.GetFieldAsI64(row, nCol - 1, &i64Buffer, &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get signed big int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

#if defined(__x86_64) || defined(WIN32)
   *((SQLBIGINT*)pTarget) = (SQLBIGINT)i64Buffer;
#else //  defined(__GNUG__) x86
   SQLBIGINT _bigint;
   _bigint.hiword = HIWORD(i64Buffer);
   _bigint.loword = LOWORD(i64Buffer);
   *((SQLBIGINT*)pTarget) = _bigint;
#endif
   dataByteSize = sizeof(SQLBIGINT);
   break;

  case SQL_C_UBIGINT:
   try
   {
    serverRet = service.GetFieldAsI64U(row, nCol - 1, &i64uBuffer, 
                                       &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get unsigned big int data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

#if defined(__x86_64) || defined(WIN32)
   *((SQLUBIGINT*)pTarget) = (SQLUBIGINT)i64uBuffer;
#else //  defined(__GNUG__) x86
   SQLUBIGINT _ubigint;
   _ubigint.hiword = HIWORD(i64Buffer);
   _ubigint.loword = LOWORD(i64Buffer);
   *((SQLUBIGINT*)pTarget) = _ubigint;
#endif
   dataByteSize = sizeof(SQLUBIGINT);
   break;

  case SQL_C_FLOAT:
   try
   {
    serverRet = service.GetFieldAsDouble(row, nCol - 1, &doubleBuffer, 
                                         &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR get float data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SQLREAL*)pTarget) = (SQLREAL)doubleBuffer;
   dataByteSize = sizeof(SQLREAL);
   break;

  case SQL_C_DOUBLE:
   try
   {
    serverRet = service.GetFieldAsDouble(row, nCol - 1, &doubleBuffer, 
                                         &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get double data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((SQLDOUBLE*)pTarget) = (SQLDOUBLE)doubleBuffer;
   dataByteSize = sizeof(SQLDOUBLE);
   break;

  case SQL_C_DATE:
  case SQL_C_TYPE_DATE:
   try
   {
    serverRet = service.GetFieldAsDate(row, nCol - 1, &dateBuffer, 
                                       &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get double data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((struct tagDATE_STRUCT *)pTarget) = (struct tagDATE_STRUCT)dateBuffer;
   dataByteSize = sizeof(struct tagDATE_STRUCT);
   break;

  case SQL_C_TIME:
  case SQL_C_TYPE_TIME:
   try
   {
    serverRet = service.GetFieldAsTime(row, nCol - 1, &timeBuffer, 
                                       &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get double data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((struct tagTIME_STRUCT *)pTarget) = (struct tagTIME_STRUCT)timeBuffer;
   dataByteSize = sizeof(struct tagTIME_STRUCT);
   break;

  case SQL_C_TIMESTAMP:
  case SQL_C_TYPE_TIMESTAMP:
   try
   {
    serverRet = service.GetFieldAsTimestamp(row, nCol - 1, &timestampBuffer, 
                                            &isNullValue);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    LOG_ERROR_F_FUNC(
     TEXT("SQL_ERROR: get double data failed for column %i. %s"),
     nCol, ex.What());
    return SQL_ERROR;
   }

   *((struct tagTIMESTAMP_STRUCT *)pTarget) =
    (struct tagTIMESTAMP_STRUCT)timestampBuffer;
   dataByteSize = sizeof(struct tagTIMESTAMP_STRUCT);
   break;

  default:
   LOG_ERROR_F_FUNC(
    TEXT("SQL_ERROR: Unknown target type %d"), nTargetType);
   return SQL_ERROR;
 }

 /**********************************************************************
 * CHECK WHETHER THERE WAS ANY DATA TO FETCH
 *********************************************************************/
 if(serverRet == ODBCDriver::SERVER_NO_MORE_DATA)
 {
  LOG_WARN_F_FUNC(
   TEXT("SQL_NO_DATA column %i has already been completely fetched."), nCol);
  return SQL_NO_DATA;
 }

 /**********************************************************************
 * FILL IN THE pnLengthOrIndicator FIELD
 **********************************************************************/
 if(pnLengthOrIndicator != NULL)
 {
  *pnLengthOrIndicator = isNullValue ? SQL_NULL_DATA : dataByteSize;
 }
 else
 {
  if(isNullValue)
  {
   LOG_ERROR_F_FUNC(
    TEXT("SQL_ERROR column %i contained NULL but pnLengthOrIndicator ")
    TEXT("was not provided."),
    nCol);

   /*
   #TODO: should be setting SQLSTATE 22002
   (Indicator variable required but not supplied)
   */
   return SQL_ERROR;
  }
 }

 /**********************************************************************
 * CHECK WHETHER THE DATA WAS TRUNCATED
 **********************************************************************/
 if(helperReturn == SQL_SUCCESS_WITH_INFO)
 {
  /*
  #TODO: this should be returning SQL_SUCCESS_WITH_INFO and setting
  SQLSTATE 01004 (Data truncated)
  */
  LOG_INFO_F_FUNC(
   TEXT("SQL_SUCCESS_WITH_INFO column %i has more data to fetch."),
   nCol);
  return SQL_SUCCESS_WITH_INFO;
 }

 LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), LOG_FUNCTION_NAME);

 return SQL_SUCCESS;
}

SQLRETURN ODBCAPIHelper::PrepareResults()
{
 HDRVSTMT hStmt = (HDRVSTMT)handle;

 SQLULEN columnIdx;
 SQLULEN numColumns;
 COLUMNHDR* columnHeader;
 ODBCDriver::ColumnDescriptor* columnDesc;

 // This must be true before calling _PrepareResults
 assert(hStmt->hStmtExtras->resultset != NULL);

 /**************************
  * allocate memory for columns headers
  **************************/
 ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 try
 {
  serverRet = service.GetColumnCount(
   hStmt->hStmtExtras->resultset, (std::size_t*)&numColumns);
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(TEXT("SQL_ERROR Column count failed. %s"), ex.What());

  FreeResults();
  return SQL_ERROR;
 }

 if(numColumns == 0)
 {
  LOG_INFO_FUNC(TEXT("SQL_INFO Zero columns found in result set."));
  
  // Nothing to do
  return SQL_SUCCESS;
 }

 // columnHeaders should already have been cleared before calling this function
 assert(hStmt->hStmtExtras->ird.rowDesc == NULL);

 hStmt->hStmtExtras->ird.rowDesc = new IRDItemList;

 if(hStmt->hStmtExtras->ird.rowDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Not enough memory. (malloc failed)"));

  FreeResults();
  return SQL_ERROR;
 }

 /**************************
  * gather column header information
  **************************/
 for(columnIdx = 0; columnIdx < numColumns; columnIdx++)
 {
  try
  {
   serverRet = service.CreateColumnDescriptor(
    hStmt->hStmtExtras->resultset, columnIdx, &columnDesc);
  }
  catch(ODBCDriver::ClientException& ex)
  {
   LOG_ERROR_F_FUNC(
    TEXT("SQL_ERROR Get column descriptor for column %d failed. %s"), 
    columnIdx, ex.What());

   service.CloseColumnDescriptor(hStmt->hStmtExtras->resultset, columnDesc);

   FreeResults();
   return SQL_ERROR;
  }

  columnHeader = new IRDItem;
  _SQLAttachIRDItem(&hStmt->hStmtExtras->ird, columnHeader);

  _NativeToSQLColumnHeader(columnHeader, columnDesc);

  service.CloseColumnDescriptor(hStmt->hStmtExtras->resultset, columnDesc);
 }

 columnDesc = NULL; /* Just to be safe */

 return SQL_SUCCESS;
}

SQLRETURN ODBCAPIHelper::FreeResults()
{
 HDRVSTMT hStmt = (HDRVSTMT)handle;

 if(hStmt == NULL)
  return SQL_ERROR;

 /* RESULT SET HANDLE */
 if(hStmt->hStmtExtras->resultset)
 {
  ODBCDriver::IService& service = ODBCDriver::Service::instance();

  service.CloseResultSet(hStmt->hStmtExtras->resultset);
  hStmt->hStmtExtras->resultset = NULL;
 }

 _SQLFreeIRDContent(&(hStmt->hStmtExtras->ird));

 return SQL_SUCCESS;
}

SQLRETURN ODBCAPIHelper::CloseResults(bool* existed)
{
 if(type != SQL_HANDLE_STMT)
 {
  LOG_DEBUG_F_FUNC(TEXT("%s: type != SQL_HANDLE_STMT"), LOG_FUNCTION_NAME);
  return SQL_ERROR;
 }

 HDRVSTMT hStmt = (HDRVSTMT)handle;

 SQLRETURN rc = SQL_SUCCESS;

 if(existed)
  *existed = false;

 if(hStmt->hStmtExtras->resultset != NULL ||
    hStmt->hStmtExtras->ird.rowDesc != NULL)
 {
  rc = FreeResults();

  if(existed)
   *existed = true;
 }

 if(hStmt->pszQuery != NULL)
 {
  free(hStmt->pszQuery);
  hStmt->pszQuery = NULL;
 }

 return rc;
}

SQLRETURN ODBCAPIHelper::FreeStmtList()
{
 HDRVDBC hDbc = (HDRVDBC)handle;

 if(hDbc == SQL_NULL_HDBC)
  return SQL_SUCCESS;

 if(hDbc->hFirstStmt == NULL)
  return SQL_SUCCESS;

 while(_FreeStmt(hDbc->hFirstStmt) == SQL_SUCCESS)
 {
 }

 return SQL_SUCCESS;
}

SQLSMALLINT ODBCAPIHelper::MapDefaultType(SQLSMALLINT type, SQLSMALLINT sType,
                                          SQLSMALLINT noSign)
{
 if(type == SQL_C_DEFAULT) 
 {
  switch(sType) 
  {
   case SQL_INTEGER:
    type = (noSign > 0) ? SQL_C_ULONG : SQL_C_LONG;
    break;

   case SQL_TINYINT:
    type = (noSign > 0) ? SQL_C_UTINYINT : SQL_C_TINYINT;
    break;

   case SQL_SMALLINT:
    type = (noSign > 0) ? SQL_C_USHORT : SQL_C_SHORT;
    break;

   case SQL_FLOAT:
    type = SQL_C_FLOAT;
    break;

   case SQL_DOUBLE:
    type = SQL_C_DOUBLE;
    break;

   case SQL_TIMESTAMP:
    type = SQL_C_TIMESTAMP;
    break;

   case SQL_TIME:
    type = SQL_C_TIME;
    break;

   case SQL_DATE:
    type = SQL_C_DATE;
    break;

#ifdef SQL_C_TYPE_TIMESTAMP
   case SQL_TYPE_TIMESTAMP:
    type = SQL_C_TYPE_TIMESTAMP;
    break;
#endif

#ifdef SQL_C_TYPE_TIME
   case SQL_TYPE_TIME:
    type = SQL_C_TYPE_TIME;
    break;
#endif

#ifdef SQL_C_TYPE_DATE
   case SQL_TYPE_DATE:
    type = SQL_C_TYPE_DATE;
    break;
#endif

#ifdef WIN32
   case SQL_WVARCHAR:
   case SQL_WCHAR:
#ifdef SQL_WLONGVARCHAR
   case SQL_WLONGVARCHAR:
#endif
#ifdef UNICODE
    type = SQL_C_WCHAR;
#else
    type = SQL_C_CHAR;
#endif
    break;
#endif

   case SQL_BINARY:
   case SQL_VARBINARY:
   case SQL_LONGVARBINARY:
    type = SQL_C_BINARY;
    break;

#ifdef SQL_BIT
   case SQL_BIT:
    type = SQL_C_BIT;
    break;
#endif

#ifdef SQL_BIGINT
   case SQL_BIGINT:
    type = SQL_C_CHAR;
    break;
#endif

   default:
#ifdef UNICODE
    type = SQL_C_WCHAR;
#else
    type = SQL_C_CHAR;
#endif

    break;
  }
 }
 return type;
}

void ODBCAPIHelper::LogSQLReturn(const TCHAR* func, SQLRETURN result)
{
 if(result == SQL_SUCCESS_WITH_INFO)
  LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS_WITH_INFO"), func);
 else if(result == SQL_ERROR)
  LOG_DEBUG_F_FUNC(TEXT("%s: SQL_ERROR"), func);
 else if(result == SQL_SUCCESS)
  LOG_DEBUG_F_FUNC(TEXT("%s: SQL_SUCCESS"), func);
}

SQLRETURN ODBCAPIHelper::_NativeToSQLColumnHeader(
 COLUMNHDR* columnHeader, ODBCDriver::ColumnDescriptor* columnDesc)
{
 // #TODO: this needs to be updated when Server is able to return more 
 //        information about the result column description.

 if(!columnHeader)
  return SQL_ERROR;

 if(!columnDesc)
  return SQL_ERROR;

 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 tstring columnName = service.GetColumnName(columnDesc);

 tstring columnType = service.GetColumnType(columnDesc);

 /* IS AUTO INCREMENT COL?                                     */
 columnHeader->bSQL_DESC_AUTO_UNIQUE_VALUE = SQL_FALSE;

 /* empty string if N/A                                          */
 columnHeader->pszSQL_DESC_BASE_COLUMN_NAME = 
  (TCHAR*)_tcsdup(columnName.c_str());

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_BASE_TABLE_NAME = (TCHAR*)_tcsdup(TEXT(""));

 /* IS CASE SENSITIVE COLUMN?                          */
 columnHeader->bSQL_DESC_CASE_SENSITIVE =
  service.GetIsCaseSensitive(columnDesc) ? SQL_TRUE : SQL_FALSE;

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_CATALOG_NAME = (TCHAR*)_tcsdup(TEXT(""));

 /* ie SQL_CHAR, SQL_TYPE_TIME...                      */
 columnHeader->nSQL_DESC_CONCISE_TYPE = service.GetSQLType(columnDesc);

 /* max digits required to display                     */
 columnHeader->nSQL_DESC_DISPLAY_SIZE = service.GetMaxDisplaySize(columnDesc);

 /* has data source specific precision?                */
 // #TODO: change this to be true for floats and doubles
 columnHeader->bSQL_DESC_FIXED_PREC_SCALE = SQL_FALSE; 

 /* display label, col name or empty string    */
 columnHeader->pszSQL_DESC_LABEL = (TCHAR*)_tcsdup(columnName.c_str());

 /* strlen or bin size                                         */
 columnHeader->nSQL_DESC_LENGTH = 
  service.GetFieldByteSize(columnDesc);

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_LITERAL_PREFIX = (TCHAR*)_tcsdup(TEXT(""));

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_LITERAL_SUFFIX = (TCHAR*)_tcsdup(TEXT(""));

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_LOCAL_TYPE_NAME = (TCHAR*)_tcsdup(TEXT(""));

 /* col alias, col name or empty string                */
 columnHeader->pszSQL_DESC_NAME = (TCHAR*)_tcsdup(columnName.c_str());

 /* SQL_NULLABLE, _NO_NULLS or _UNKNOWN                */
 columnHeader->nSQL_DESC_NULLABLE =
  service.GetIsNullable(columnDesc) ? SQL_NULLABLE : SQL_NO_NULLS;

 /* 2, 10, or if N/A... 0                                      */
 // #TODO: this will probably need to change for floats and doubles
 columnHeader->nSQL_DESC_NUM_PREC_RADIX = 0; 

 /* max size                                                   */
 columnHeader->nSQL_DESC_OCTET_LENGTH = 
  service.GetFieldByteSize(columnDesc);

 /*                                                            */
 // #TODO: this will probably need to be updated for numericals
 columnHeader->nSQL_DESC_PRECISION = 4; 

 /*                                                            */
 // #TODO: this will probably need to be updated for exact numericals
 columnHeader->nSQL_DESC_SCALE = 4; 

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_SCHEMA_NAME = (TCHAR*)_tcsdup(TEXT(""));

 /* can be in a filter ie SQL_PRED_NONE...     */
 columnHeader->nSQL_DESC_SEARCHABLE = SQL_PRED_SEARCHABLE;

 /* empty string if N/A                                                */
 columnHeader->pszSQL_DESC_TABLE_NAME = (TCHAR*)_tcsdup(TEXT(""));

 /* SQL data type ie SQL_CHAR, SQL_INTEGER..   */
 columnHeader->nSQL_DESC_TYPE = service.GetSQLType(columnDesc);

 /* DBMS data type ie VARCHAR, MONEY...                */
 columnHeader->pszSQL_DESC_TYPE_NAME = (TCHAR*)_tcsdup(columnType.c_str());

 /* qualifier for SQL_DESC_NAME ( SQL_NAMED if SQL_DESC_NAME has a 
    column name, or SQL_UNNAMED if no column name or alias exists  */
 columnHeader->nSQL_DESC_UNNAMED = columnName.empty() ? SQL_UNNAMED : 
                                                        SQL_NAMED;

 /* if signed FALSE else TRUE                          */
 columnHeader->bSQL_DESC_UNSIGNED = SQL_FALSE;

 /* ie SQL_ATTR_READONLY, SQL_ATTR_WRITE...    */
 columnHeader->nSQL_DESC_UPDATABLE = SQL_ATTR_READONLY;

 return SQL_SUCCESS;
}

SQLRETURN ODBCAPIHelper::_SQLFetchCol(ARDItem* pARDCol, 
                                      SQLULEN processedRowCount)
{
 HDRVSTMT pStmt = (HDRVSTMT)handle;

 // note
 // this function checks the binding type and positions the pointer
 // for copying the data accordingly. It takes into account the
 // current row position in rowset, the initial min increment specified
 // by client and the size of the row or col buffer
 SQLINTEGER i;
 SQLINTEGER j;
 SQLINTEGER* tgtsizeptr; // target size ptr
 SQLPOINTER tgtdataptr; // target data ptr
 IRDItem* coldesc;

 ODBCDriver::IService& service = ODBCDriver::Service::instance();

 // COMPUTE DATA AND SIZE PTR
 // get the row pos in current rowset
 i = (SQLINTEGER)processedRowCount;
 // compute min increment
 j = (pStmt->hStmtExtras->ard.bindOffsetPtr) ? 
     *(pStmt->hStmtExtras->ard.bindOffsetPtr) : 0;

 // check the binding type
 if(pStmt->hStmtExtras->ard.bindTypeOrSize != SQL_BIND_BY_COLUMN) 
 {  
  // note
  SQLINTEGER k;

  // compute row-size increment
  k = (pStmt->hStmtExtras->ard.bindTypeOrSize);

  // compute target col and size ptr
  tgtdataptr = (SQLPOINTER)(((char*)(pARDCol->dataPtr)) + j + (i * k));
  tgtsizeptr = (SQLINTEGER*)(((char*)(pARDCol->sizePtr)) + j + (i * k));
 }
 // column-wise binding
 else 
 { 
  // move both data and size ptr in the array
  //TODO find out where the pARDCol->DataSize if set
  tgtdataptr = (SQLPOINTER)(((char*)(pARDCol->dataPtr)) + j + 
                            (i * pARDCol->dataSize)); // use based on data type
  tgtsizeptr = (SQLINTEGER*)(((char*)(pARDCol->sizePtr)) + j + 
                             (i * sizeof(SQLINTEGER)));
 }

 // PRECAUTION

 if(tgtdataptr)   
  *((char*)tgtdataptr) = 0; 

 if(tgtsizeptr)
  *((SQLINTEGER*)tgtsizeptr) = 0;
 
 // COLLECT AND CHECK
 // get col desc for specified col ( response )
 coldesc = _SQLGetIRDItem(&pStmt->hStmtExtras->ird, pARDCol->colNum); 

 if(coldesc == NULL) 
 {
  LOG_ERROR_F_FUNC(
   TEXT("column not found in resultset for specified index: %d"), 
   pARDCol->colNum);

  return SQL_SUCCESS_WITH_INFO; // no col for specified index
 }

 // NULL DATA                                            

 // check if data is NULL
 try
 {
  ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;
  ODBCDriver::Row* row;

  serverRet = service.GetCurrentRow(pStmt->hStmtExtras->resultset, &row);
  if(serverRet != ODBCDriver::SERVER_SUCCESS)
  {
   LOG_ERROR_FUNC(TEXT("SQL_ERROR: Unable to get current row."));

   return SQL_ERROR;
  }

  bool isNull;

  serverRet = service.IsNull(row, pARDCol->colNum - 1, &isNull);
  if(serverRet != ODBCDriver::SERVER_SUCCESS)
  {
   LOG_ERROR_FUNC(
    TEXT("SQL_ERROR: Unable to determine is NULL or not for current row."));

   return SQL_ERROR;
  }

  if(isNull)
  {
   // check if a size indicator is available
   if(tgtsizeptr == NULL) 
   {
    LOG_ERROR_F_FUNC(
     TEXT("Indicator variable required but not supplied: %d"),
     pARDCol->colNum);

    return SQL_SUCCESS_WITH_INFO;
   }
   // set to SQL_NULL_DATA
   else 
   {
    // indicate null data
    *((SQLINTEGER*)tgtsizeptr) = SQL_NULL_DATA;
    // added precaution for bad appl design
    /*  if ( tgtdataptr )
    memset ( tgtdataptr, 0, pARDCol->maxSize );*/
    return SQL_SUCCESS;
   }
  }
 }
 catch(ODBCDriver::ClientException& ex)
 {
  LOG_ERROR_F_FUNC(
   TEXT("SQL_ERROR: Failed to check data is null for column: %d. %s"),
   pARDCol->colNum, ex.What());

  return SQL_ERROR;
 }

 // CONVERT AND TRANSFER
 // Apache Kylin: _SQLColConvert
 return GetData(pARDCol->colNum, 
                pARDCol->dataConciseType, 
                tgtdataptr, 
                pARDCol->dataSize, 
                (SQLLEN*)tgtsizeptr);
}
