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

#ifndef _BIGOBJECT_ODBC_CLIENT_SERVICE_
#define _BIGOBJECT_ODBC_CLIENT_SERVICE_

#include <cstddef>

#include "../../Base/CRTP.hpp"
#include "../../Base/Platform.hpp"
#include "../../Base/TString.hpp"

#include "../Driver.hpp"

#include "Client.hpp"
#include "Connection.hpp"
#include "ResultSet.hpp"
#include "ColumnDescriptor.hpp"
#include "Exception.hpp"

namespace ODBCDriver
{

template<typename Impl>
class ServiceInterface : public bigobject::CRTP<Impl, ServiceInterface<Impl> >
{
public:

#ifdef __GNUG__
 typedef bigobject::CRTP<Impl, ServiceInterface<Impl> > BaseType;
 typedef typename BaseType::ThisType ThisType;
#endif

public:

 /**
 Connect to a database.

 Connects to a database on the specified server. The caller takes ownership of
 the returned Connection and is responsible for deallocating the memory and
 connection by calling CloseConnection.

 If there are any errors while connection, ClientException throws with
 error messages.

 @param hDbc SQLHDBC.
 @param database Name of the database on the server to which to connect.
 @param host Host address of the server.
 @param port Host port of the server.
 @param uid Host user.
 @param password Host password. 

 @return A Connection object representing the established database connection,
         or NULL if an error occurred.
 */
 Connection* OpenConnection(SQLHDBC hDbc, const tstring& database,
                            const tstring& host, int port, 
                            const tstring uid = tstring(), 
                            const tstring password = tstring())
 {
  return NULL;
 }

 /**
 Disconnects from a database.

 Disconnects from a database and destroys the supplied Connection object.
 This function should eventually be called for every Connection created by
 OpenConnection.

 @param connection  A Connection object associated a database connection.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn CloseConnection(Connection* connection)
 {
  delete connection;
  return SERVER_SUCCESS;
 }

 /**
 Query for database tables.
 
 Gets a resultset containing the set of tables in the database matching a 
 pattern (SQL pattern or regular expression, depends on implementation).
 
 Caller takes ownership of returned ResultSet and is responsible for 
 deallocating the object by calling CloseResultSet.

 @param connection A Connection object associated a database connection.
 @param pattern A pattern used to match with table names.
 @param pResultSet A pointer to a ResultSet pointer which will be associated
                   with the result, or NULL if the result is not needed.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetTables(Connection* connection, const tstring& pattern,
                        ResultSet** pResultSet)
 {
  return SERVER_ERROR;
 }

 /**
 Query for type info.

 Gets a resultset containing the SQL92 type info for the database.
 https://msdn.microsoft.com/en-us/library/ms710150%28v=vs.85%29.aspx

 Caller takes ownership of returned ResultSet and is responsible for
 deallocating the object by calling CloseResultSet.

 @param connection A Connection object associated a database connection.
 @param dataType One of SQL92 data type, or SQL_ALL_TYPES.
 @param pResultSet A pointer to a ResultSet pointer which will be associated
                   with the result, or NULL if the result is not needed.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetTypeInfo(Connection* connection, SQLSMALLINT dataType, 
                          ResultSet** pResultSet)
 {
  return SERVER_ERROR;
 }

 /**
 Query for columns in table(s).

 Gets a resultset containing the set of columns in the database matching a 
 pattern from a set of tables matching another pattern. 
 (SQL pattern or regular expression, depends on implementation).
 
 Caller takes ownership of returned ResultSet and is responsible for
 deallocating the object by calling CloseResultSet.

 @param connection A Connection object associated a database connection.
 @param tblPattern A pattern used to match with table names.
 @param colPattern A pattern used to match with column names.
 @param pResultSet A pointer to a ResultSet pointer which will be associated
                   with the result, or NULL if the result is not needed.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetColumns(Connection* connection, const tstring& tblPattern,
                         const tstring& colPattern, ResultSet** pResultSet)
 {
  return SERVER_ERROR;
 }

 /**
 Execute a query.

 Executes a query on a Hive connection and associates a ResultSet with the 
 result. Caller takes ownership of returned ResultSet and is responsible for 
 deallocating the object by calling CloseResultSet.

 @param connection A Connection object associated a database connection.
 @param query The query string to be executed.
 @param pResultSet A pointer to a ResultSet pointer which will be associated 
                   with the result, or NULL if the result is not needed.
 @param maxBufRows Maximum number of rows to buffer in the new ResultSet for 
                   the query results.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn Execute(Connection* connection, const tstring& query, 
                      ResultSet** pResultSet, std::size_t maxBufRows = 2048)
 {
  return SERVER_ERROR;
 }

 /**
 Determines the number of columns in the ResultSet.

 @param resultset A ResultSet from which to retrieve the column count.
 @param colCount Pointer to a size_t which will be set to the number of columns
                 in the result.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetColumnCount(ResultSet* resultset, std::size_t* colCount)
 {
  return resultset->GetColumnCount(colCount);
 }

 /**
 Construct a ColumnDescriptor.

 Constructs a ColumnDescriptor with information about a specified column in 
 the resultset. Caller takes ownership of returned ColumnDescriptor and is 
 responsible for deallocating the object by calling CloseColumnDescriptor.

 @param resultset A ResultSet context from which to construct the 
                  ColumnDescriptor.
 @param columnIdx Zero offset index of a column.
 @param pColumnDesc A pointer to a ColumnDescriptor pointer which will receive 
                    the new ColumnDescriptor.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn CreateColumnDescriptor(ResultSet* resultset, 
                                     std::size_t columnIdx,
                                     ColumnDescriptor** pColumnDesc)
 {
  return resultset->CreateColumnDescriptor(columnIdx, pColumnDesc);
 }

 /**
 Destroys a ColumnDescriptor object.

 @param resultset A ResultSet context from which to construct the
                  ColumnDescriptor.
 @param columnDesc A ColumnDescriptor object to be removed from memory.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn CloseColumnDescriptor(ResultSet* resultset, 
                                    ColumnDescriptor* columnDesc)
 {
  return resultset->CloseColumnDescriptor(columnDesc);
 }

 /**
 Get a column name.

 Retrieves the column name of the associated column in the result table from
 a ColumnDescriptor.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return Target tstring which stores the result.
 */
 tstring GetColumnName(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetColumnName();
 }

 /**
 Get the column type name.

 Retrieves the name of the column type of the associated column in the result 
 table from a ColumnDescriptor.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return Target tstring which stores the result.
 */
 tstring GetColumnType(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetColumnType();
 }

 /**
 Finds whether a column is nullable.

 Determines from a ColumnDescriptor whether a particular column in the result 
 table is able to contain NULLs.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return True if the column can contain NULLs, or false if the column cannot 
         contain NULLs.
 */
 bool GetIsNullable(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetIsNullable();
 }

 /**
 Finds whether a column is case sensitive.

 Determines from a ColumnDescriptor whether a particular column in the result 
 table contains case sensitive data.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return True if the column data is case sensitive, or false otherwise.
 */
 bool GetIsCaseSensitive(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetIsCaseSensitive();
 }

 /**
 Get the column type as a ServerType.

 Retrieves the column type as a ServerType for the associated column in the 
 result table from a ColumnDescriptor.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return ServerType of the column.
 */
 ServerType GetServerType(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetServerType();
 }

 /**
 Get the column type as a SQL type.
 https://msdn.microsoft.com/en-us/library/ms710150%28v=vs.85%29.aspx

 Retrieves the column type as a SQL type for the associated column in the
 result table from a ColumnDescriptor.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return SQL Type of the column.
 */
 int GetSQLType(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetSQLType();
 }

 /**
 Finds the max display size of a column's fields.

 From a ColumnDescriptor, determines the maximum number of characters needed to 
 represent a field within this column.

 @param columnDesc A ColumnDescriptor associated with the column in question.
 @return The maximum number of characters needed to represent a field within 
         this column.
 */
 std::size_t GetMaxDisplaySize(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetMaxDisplaySize();
 }

 /**
 Finds the max (native) byte size of a column's fields.

 From a ColumnDescriptor, determines the number of bytes needed to store a 
 field within this column in its native type.

 @param column_desc A HiveColumnDesc associated with the column in question.
 @return The number of bytes needed to store a field within this column in its 
         native type.
 */
 std::size_t GetFieldByteSize(ColumnDescriptor* columnDesc)
 {
  return columnDesc->GetFieldByteSize();
 }

 /**
 Fetches the next unfetched row in a ResultSet.

 Fetches the next unfetched row in a ResultSet. The fetched row will be stored
 internally within the resultset and may be accessed through other DB 
 functions.

 @param resultset A ResultSet from which to fetch rows.
 @param orientation Fetch orientation.
 @param offset Number of the row to fetch. The interpretation of this argument 
               depends on the value of the orientation argument.
 @return SERVER_SUCCESS if successful, SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if there are no more rows to fetch.
 */
 ServerReturn Fetch(ResultSet* resultset, 
                    unsigned short orientation = SQL_FETCH_NEXT,
                    std::size_t offset = 0)
 {
  return resultset->Fetch(orientation, offset);
 }

 /**
 Check for results.

 Determines whether the ResultSet has ever had any result rows.

 @param resultset A ResultSet from which to check for results.
 @param hasResults Pointer to an int which will be set to 1 if results, 0 if 
                   no results.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn HasResults(ResultSet* resultset, bool* hasResults)
 {
  return resultset->HasResults(hasResults);
 }

 /*
 Get current Row object.

 The row will ONLY be valid after Fetch has been called at least once.

 @param resultset An initialized resultset.
 @param pRow A pointer to a Row pointer which will receive the current Row.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetCurrentRow(ResultSet* resultset, Row** pRow)
 {
  return resultset->GetCurrentRow(pRow);
 }

 /**
 Determines the position of current row.

 @param resultset An initialized resultset.
 @param rowPos Pointer to a size_t which will be set to the position of 
               current row.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetCurrentRowPos(ResultSet* resultset, std::size_t* rowPos)
 {
  return resultset->GetCurrentRowPos(rowPos);
 }

 /**
 Is column NULL value?

 @param row A fetched current row.
 @param columnIdx Column index.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn IsNull(Row* row, std::size_t columnIdx, bool* isNull)
 {
  return row->IsNull(columnIdx, isNull);
 }

 /**
 Find the size of a field as a string.
 
 Determines the number of characters needed to display a field stored in the 
 fetched row of a ResultSet.
 
 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param colLen Pointer to an size_t which will be set to the byte length of 
               the specified field.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 ServerReturn GetFieldDataLen(Row* row, std::size_t columnIdx,
                              std::size_t* colLen)
 {
  return row->GetFieldDataLen(columnIdx, colLen);
 }

 /**
 Get a field as a string.

 Reads out a field from the currently fetched rowset in a resultset as a 
 String.

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param value Pointer to a buffer that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field 
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful and there is no more data to fetch
         SERVER_ERROR if an error occurred 
         SERVER_SUCCESS_WITH_MORE_DATA if data has been successfully fetched, 
         but there is still more data to get SERVER_NO_MORE_DATA if this field 
         has already been completely fetched.
 */
 ServerReturn GetFieldAsString(Row* row, std::size_t columnIdx,
                               tstring& value, bool* isNull)
 {
  return row->GetFieldAsString(columnIdx, value, isNull);
 }

 /**
 Get a field as a double.

 Reads out a field from the currently fetched row in a resultset as a double 
 (platform specific).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a double that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field 
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful.
         SERVER_ERROR if an error occurred, SERVER_NO_MORE_DATA if this field 
         has already been fetched.
 */
 ServerReturn GetFieldAsDouble(Row* row, std::size_t columnIdx, 
                               double* buffer, bool* isNull)
 {
  return row->GetFieldAsDouble(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an int.

 Reads out a field from the currently fetched row in a resultset as an int
 (platform specific).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to an int that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsInt(Row* row, std::size_t columnIdx, 
                            int* buffer, bool* isNull)
 {
  return row->GetFieldAsInt(columnIdx, buffer, isNull);
 }

 /**
 Get a field as a long int.

 Reads out a field from the currently fetched row in a resultset as a long 
 int (platform specific).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a long int that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsLong(Row* row, std::size_t columnIdx, 
                             long* buffer, bool* isNull)
 {
  return row->GetFieldAsLong(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an unsigned long int.

 Reads out a field from the currently fetched row in a resultset as an 
 unsigned long int (platform specific).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to an unsigned long int that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsULong(Row* row, std::size_t columnIdx, 
                              unsigned long* buffer, bool* isNull)
 {
  return row->GetFieldAsULong(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an int64_t.

 Reads out a field from the currently fetched row in a resultset as a 
 int64_t (platform independent).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a int64_t that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsI64(Row* row, std::size_t columnIdx, 
                            int64_t* buffer, bool* isNull)
 {
  return row->GetFieldAsI64(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an uint64_t.

 Reads out a field from the currently fetched row in a resultset as a 
 uint64_t (platform independent).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a uint64_t that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsI64U(Row* row, std::size_t columnIdx, 
                             uint64_t* buffer, bool* isNull)
 {
  return row->GetFieldAsI64U(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an tagDATE_STRUCT.

 Reads out a field from the currently fetched row in a resultset as a
 struct tagDATE_STRUCT (platform independent).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a tagDATE_STRUCT that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsDate(Row* row, std::size_t columnIdx,
                             struct tagDATE_STRUCT* buffer, bool* isNull)
 {
  return row->GetFieldAsDate(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an tagTIME_STRUCT.

 Reads out a field from the currently fetched row in a resultset as a
 struct tagTIME_STRUCT (platform independent).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a tagTIME_STRUCT that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsTime(Row* row, std::size_t columnIdx,
                             struct tagTIME_STRUCT* buffer, bool* isNull)
 {
  return row->GetFieldAsTime(columnIdx, buffer, isNull);
 }

 /**
 Get a field as an tagTIMESTAMP_STRUCT.

 Reads out a field from the currently fetched rowset in a resultset as a
 struct tagTIMESTAMP_STRUCT (platform independent).

 @param row A fetched current row.
 @param columnIdx Zero offset index of a column.
 @param buffer Pointer to a tagTIMESTAMP_STRUCT that will receive the data.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred,
         SERVER_NO_MORE_DATA if this field has already been fetched.
 */
 ServerReturn GetFieldAsTimestamp(Row* row, std::size_t columnIdx,
                                  struct tagTIMESTAMP_STRUCT* buffer, 
                                  bool* isNull)
 {
  return row->GetFieldAsTimestamp(columnIdx, buffer, isNull);
 }

 /**
 Destroys any specified ResultSet object.
 
 Destroys any specified ResultSet object. The ResultSet may have been
 created by a number of other functions.
 
 @param resultset A ResultSet object to be removed from memory.
 @return SERVER_SUCCESS if successful. SERVER_ERROR if an error occurred.
 */
 ServerReturn CloseResultSet(ResultSet* resultset)
 {
  delete resultset;
  return SERVER_SUCCESS;
 }

};


}

#endif
