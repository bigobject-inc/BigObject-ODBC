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

#ifndef _BIGOBJECT_ODBC_H_HELPER
#define _BIGOBJECT_ODBC_H_HELPER

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include "../Driver.hpp"

#include "Descriptor.hpp"
#include "Diagnostic.hpp"

/**
Connection string parser.
*/
class ConnectionStringParser
{
public:

 typedef boost::unordered_map<tstring, tstring> ConnectionStringMap;

 typedef boost::shared_ptr<ConnectionStringMap> ConnectionStringMapPtr;

public:

 /**
 Parse connection string to key value pair.

 @param connStr Connection string.
 @return Pointer to ConnectionStringMap.
 */
 static ConnectionStringMapPtr Parse(tstring& connStr);

};

/**
ODBC API Helper contains generic utility functions used across files/modules.
Data conversion and copying functions w.r.t ODBC type params
like its type, size, its size pointer, src data and src size etc.
*/
class ODBCAPIHelper
{
public:

 /**
 Constructor.

 @param _handle SQL Handle.
 @param _type SQL handle type.
 */
 ODBCAPIHelper(SQLHANDLE _handle, int _type);

 /**
 Destructor.
 */
 ~ODBCAPIHelper();

 /**
 Check handle is valid or not.

 @return True if valid otherwise else.
 */
 bool IsValid();

 /**
 SQLHENV, SQLHDBC, and SQLHSTMT synchronization is done using a critical 
 section in ENVEXTRAS and DBCEXTRAS structures.

 On UN*X assume that we are single-threaded or the driver manager provides 
 serialization for us.
 
 In iODBC (3.52.x) serialization can be turned on using the DSN property 
 "ThreadManager=yes".
 
 In unixODBC that property is named "Threading=0-3" and takes one of these 
 values:
 
 *   0 - no protection
 *   1 - statement level protection
 *   2 - connection level protection
 *   3 - environment level protection
 
 unixODBC 2.2.11 uses environment level protection by default when it has been 
 built with pthread support.
 */
 void Lock();

 /**
 Unlock SQLHENV, SQLHDBC, and SQLHSTMT synchronization.
 */
 void Unlock();

 /**
 Prepare SQL statement.

 @param szSqlStr SQL statement.
 @param nSqlStrLength Lenght of SQL statement.
 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN Prepare(SQLTCHAR *szSqlStr, SQLINTEGER nSqlStrLength);

 /**
 Execute prepared SQL statement.

 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN Execute();

 SQLRETURN FetchScroll(SQLUSMALLINT nOrientation, SQLLEN nOffset,
                       SQLULEN *rowCountPtr = NULL, 
                       SQLUSMALLINT *rowStatusArray = NULL);

 /**
 Get data from results.

 @param nCol For retrieving column data, it is the number of the column for 
             which to return data. Result set columns are numbered in 
             increasing column order starting at 1. The bookmark column is 
             column number 0; this can be specified only if bookmarks are 
             enabled. For retrieving parameter data, it is the ordinal of the 
             parameter, which starts at 1.
 @parma nTargetType The type identifier of the C data type of the *pTarget 
                    buffer. 
 @param pTarget Pointer to the buffer in which to return the data. 
                It cannot be NULL.
 @param nTargetLength Length of the *pTarget buffer in bytes.
 @param pnLengthOrIndicator Pointer to the buffer in which to return the 
                            length or indicator value. If this is a null 
                            pointer, no length or indicator value is returned. 
                            This returns an error when the data being fetched 
                            is NULL.
 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN GetData(SQLUSMALLINT nCol, 
                   SQLSMALLINT nTargetType, SQLPOINTER pTarget,
                   SQLLEN nTargetLength, SQLLEN *pnLengthOrIndicator);

 /**
 Prepare results for fetch.

 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN PrepareResults();

 /**
 Free results.

 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN FreeResults();

 /**
 Close any existing resultset for any API function which need to build new 
 resultset.

 @param existed Pointer to store the value to indicate results existed or not,
                true if there is any existing resultset, otherwise false.
 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN CloseResults(bool* existed = NULL);

 /**
 Free all statements associated to connection.

 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 SQLRETURN FreeStmtList();

 /**
 Map SQL_C_DEFAULT to proper C type.

 @param type Input C type.
 @param sType Input SQL type
 @param noSign 0 = signed, 0 > unsigned, 0 < undefined.
 @return C type.
 */
 SQLSMALLINT MapDefaultType(SQLSMALLINT type, SQLSMALLINT sType,
                            SQLSMALLINT noSign = -1);

 /**
 Log SQLRETURN value for debugging purpose.

 @param func Caller function name (it should be LOG_FUNCTION_NAME).
 @param result SQLRETURN.
 */
 void LogSQLReturn(const TCHAR* func, SQLRETURN result);
 
 /**
 Copy string to SQLPOINTER with maximum size (bytes), copied size (bytes) to 
 meet API requirement.

 @param pTarget Pointer to a buffer in which to return the information, a 
                null-terminated character string. If pTarget is NULL, 
                pnLength will still return the total number of bytes 
                (excluding the null-termination character for character data) 
                available to return in the buffer pointed to by pTarget.
 @param nTargetLength Length of the *pTarget buffer. If pTarget is a null 
                      pointer, the nTargetLength argument is ignored. If 
                      *pTarget is a Unicode string, the nTargetMax argument 
                      must be an even number; If not, SQLSTATE HY090 
                      (Invalid string or buffer length) is returned.
 @param pnLength Pointer to a buffer in which to return the total number of 
                 bytes (excluding the null-termination character) available 
                 to return in *pTarget. If the number of bytes available to 
                 return is greater than or equal to nTargetLength, 
                 the information in *pTarget is truncated to nTargetLength 
                 bytes minus the length of a null-termination character and 
                 is null-terminated by the driver.
 @param src Source string.
 @return SQL_SUCCESS if OK, SQL_SUCCESS_WITH_INFO if pTarget truncated,
         otherwise SQL_ERROR.
 */
 template<typename L1, typename L2, typename T>
 SQLRETURN CopyToSQLData(SQLPOINTER pTarget, L1 nTargetLength,
                         L2* pnLength, const T& src)
 {
  // Check if there is a holder for size.
  if(!pnLength)
  {
   // Check if src data but no size holder.
   if(!src.empty())
   {
    // #TODO: Check if diag to be set.
    // This should be invalid, return SQL_ERROR. But, pyodbc call
    // SQLDescribeCol in this way with NameLengthPtr = NULL.
    // We still copy the value to pTarget within nTargetLength.
   }
  }

  if(src.empty())
  {
   if(pTarget)
    memset(pTarget, 0, (size_t)nTargetLength);

   if(pnLength)
    *pnLength = 0;

   return SQL_SUCCESS;
  }

  std::size_t srcLen = src.size();
  bool trunc = false;
  size_t cpyBytes = 0;

  if(srcLen * sizeof(typename T::value_type) >= (std::size_t)nTargetLength)
  {
   // Truncated.
   cpyBytes = nTargetLength - sizeof(typename T::value_type);

   if(pnLength)
    *pnLength = (L2)cpyBytes;

   trunc = true;
  }
  else
  {
   cpyBytes = srcLen * sizeof(typename T::value_type);

   if(pnLength)
    *pnLength = (L2)cpyBytes;
  }

  if(pTarget)
  {
   memset(pTarget, 0, (size_t)nTargetLength);
   memcpy(pTarget, (const void*)src.c_str(), cpyBytes);
  }

  if(trunc)
  {
   // #TODO: Set diag.
   return SQL_SUCCESS_WITH_INFO;
  }
  else
  {
   return SQL_SUCCESS;
  }
 }

 /**
 Copy numeric value to SQLPOINTER.

 @param pTarget A pointer to memory in which to return the value. Integer-type.
                Therefore, applications should use a buffer of SQLULEN and 
                initialize the value to 0 before calling this function.
                If it is NULL, pnLength will still return the total number of 
                bytes available to return in the buffer pointed to by pTarget.
 @param pnLength A pointer to a buffer in which to return the total number of 
                 bytes.
 @param srcValue Source value.
 @return SQL_SUCCESS if OK, otherwise SQL_ERROR.
 */
 template<typename T, typename L>
 SQLRETURN CopyToSQLData(SQLPOINTER pTarget, L* pnLength, T srcValue)
 {
  if(pTarget)
   *((T*)pTarget) = srcValue;

  if(pnLength)
   *pnLength = sizeof(T);

  return SQL_SUCCESS;
 }

 /**
 Get tstring for ODBC SQLTCHAR* string with length. ODBC API can pass actual
 strubg length or SQL_NTS (-3) to driver.

 @param str String.
 @param len Length.
 @return TString.
 */
 template<typename T>
 tstring GetSafeTString(const SQLTCHAR* str, T len)
 {
  if(len > 0)
  {
   return tstring((TCHAR*)str, (std::size_t)len);
  }
  else // SQL_NTS
  {
   return tstring((TCHAR*)str);
  }
 }


protected:

 SQLRETURN _NativeToSQLColumnHeader(COLUMNHDR* columnHeader,
                                    ODBCDriver::ColumnDescriptor* columnDesc);

 SQLRETURN _SQLFetchCol(ARDItem* pARDCol, SQLULEN processedRowCount);

protected:

 /// SQLHENV, SQLHDBC or SQLHSTMT.
 SQLHANDLE handle;

 /// SQL_HANDLE_ENV, SQL_HANDLE_DBC or SQL_HANDLE_STMT.
 SQLUSMALLINT type;

#if defined(_WIN32) || defined(_WIN64)
 DWORD owner;	/// Current owner of CS or 0.
#else
 pid_t owner; /// Current owner of CS or 0.
#endif

};

#endif
