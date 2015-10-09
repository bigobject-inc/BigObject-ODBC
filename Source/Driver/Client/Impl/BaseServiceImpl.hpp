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


#include <utility>
#include <string>

#include <boost/lexical_cast.hpp>

#include <boost/tuple/tuple.hpp>

#include "../../../Base/CRTP.hpp"
#include "../../../Logging/Logging.hpp"

#include "../Detail/Service.hpp"

namespace ODBCDriver
{
namespace Detail
{
namespace Base
{

/// Column Name, ServerType, Type string.
typedef boost::tuple<tstring, ODBCDriver::ServerType, tstring> Column;

typedef std::vector<Column> ColumnList;

/**
ColumnDescriptor implementation.
*/
class ColumnDescriptor : public ODBCDriver::ColumnDescriptor
{
public:
 
 /**
 Constructor.

 @param _column Column list.
 */
 ColumnDescriptor(Column& _column) : column(_column)
 {
 }

 tstring GetColumnName()
 {
  return boost::get<0>(column);
 }

 tstring GetColumnType()
 {
  return boost::get<2>(column);
 }

 ServerType GetServerType()
 {
  return boost::get<1>(column);
 }

 bool GetIsNullable()
 {
  return true;
 }

 bool GetIsCaseSensitive()
 {
  return false;
 }

private:

 /// Column metadata.
 Column& column;

};

/**
Abstract ResultSet implementation, it provides basic implementation for all 
ResultSet subclasses.
*/
template<typename _ConnectionType, typename _RowType = StringRow>
class AbstractResultSet : public virtual ODBCDriver::ResultSet
{
public:

 typedef _ConnectionType ConnectionType;

 typedef _RowType RowType;

 /// List type of rows.
 typedef std::vector<typename RowType::StoragePtrType> RowValueSet;

public:

 ServerReturn Fetch(unsigned short orientation = SQL_FETCH_NEXT,
                    std::size_t offset = 0)
 {
  // #TODO: Implement different orientation.

  // Initialize.
  if(rows.empty())
   FetchNewResults();

  if(currentIdx >= rows.size())
  {
   if(eof)
    return ODBCDriver::SERVER_NO_MORE_DATA;

   currentIdx = 0;
   rows.clear();

   FetchNewResults();

   // Corner case.
   if(currentIdx >= rows.size())
    return ODBCDriver::SERVER_NO_MORE_DATA;
  }

  row.Set(rows[currentIdx]);
  currentIdx++;

  return ODBCDriver::SERVER_SUCCESS;
 }

 virtual ServerReturn HasResults(bool* results)
 {
  *results = !rows.empty();
  return ODBCDriver::SERVER_SUCCESS;
 }

 virtual ServerReturn GetColumnCount(std::size_t* colCount)
 {
  *colCount = columns.size();
  return ODBCDriver::SERVER_SUCCESS;
 }

 virtual ServerReturn GetCurrentRow(ODBCDriver::Row** rowPtr)
 {
  *rowPtr = &row;
  return ODBCDriver::SERVER_SUCCESS;
 }

 virtual ServerReturn GetCurrentRowPos(std::size_t* rowPos)
 {
  *rowPos = currentIdx - 1;
  return ODBCDriver::SERVER_SUCCESS;
 }

protected:

 /**
 Costructor.

 @param _conn Connection.
 */
 AbstractResultSet(ConnectionType* _conn) 
  : conn(_conn), currentIdx(0), eof(false)
 {
 }

 /**
 Destructor.
 */
 virtual ~AbstractResultSet()
 {
 }

 /**
 Fetch new results to rows.
 */
 virtual void FetchNewResults() = 0;

protected:

 /// Connection.
 ConnectionType* conn;

 /// Column list.
 ColumnList columns;

 /// Current row.
 RowType row;

 /// Read rows.
 RowValueSet rows;

 /// Current index for rows.
 std::size_t currentIdx;

 /// EOF?
 bool eof;

};

/**
ResultSet implementation.
*/
template<typename ConnectionType>
class QueryResultSet : public AbstractResultSet<ConnectionType>
{
public:

 /**
 Constructor.

 @param _conn Connection.
 @param _query Query string.
 @param _maxBufRows Maximum buffer rows.
 */
 QueryResultSet(ConnectionType* _conn, tstring _query, std::size_t _maxBufRows)
  : AbstractResultSet<ConnectionType>(_conn),
    query(_query), maxBufRows(_maxBufRows)
 {
  this->conn->Execute(query);
  this->columns = this->conn->GetColumnList();
 }

 ServerReturn CreateColumnDescriptor(
  size_t columnIdx, ODBCDriver::ColumnDescriptor** columnDescPtr)
 {
  if(columnIdx >= this->columns.size())
   return ODBCDriver::SERVER_ERROR;

  *columnDescPtr = new ColumnDescriptor(this->columns[columnIdx]);
  return ODBCDriver::SERVER_SUCCESS;
 }

 ServerReturn CloseColumnDescriptor(
  ODBCDriver::ColumnDescriptor* columnDescPtr)
 {
  delete columnDescPtr;
  return ODBCDriver::SERVER_SUCCESS;
 }

protected:

 void FetchNewResults()
 {
  if(this->eof)
   return;

  std::size_t readRows = this->conn->Fetch(this->rows, maxBufRows);

  if(readRows < maxBufRows)
   this->eof = true;
 }

protected:
 
 /// Query string.
 tstring query; 
 
 /// Maximum buffer rows.
 std::size_t maxBufRows;

};

/**
ResultSet implementation for SQLTables.
*/
template<typename ConnectionType>
class TableResultSetImpl 
 : public AbstractResultSet<ConnectionType, ODBCDriver::TableRow>,
   public ODBCDriver::TableResultSet
{
public:

 /**
 Constructor.
 */
 TableResultSetImpl(ConnectionType* _conn, const tstring& pattern)
  : AbstractResultSet<ConnectionType, ODBCDriver::TableRow>(_conn), 
    ODBCDriver::TableResultSet()
 {
  this->conn->FetchTables(this->rows, pattern);
  this->eof = true;
 }

 ServerReturn GetColumnCount(std::size_t* colCount)
 {  
  return ODBCDriver::TableResultSet::GetColumnCount(colCount);
 }

protected:

 void FetchNewResults()
 {
 }

};

/**
ResultSet implementation for SQLGetTypeInfo.
*/
template<typename ConnectionType>
class TypeInfoResultSetImpl
 : public AbstractResultSet<ConnectionType, ODBCDriver::TypeInfoRow>,
   public ODBCDriver::TypeInfoResultSet
{
public:

 /**
 Constructor.
 */
 TypeInfoResultSetImpl(ConnectionType* _conn, SQLSMALLINT dataType)
  : AbstractResultSet<ConnectionType, ODBCDriver::TypeInfoRow>(_conn), 
    ODBCDriver::TypeInfoResultSet()
 {
  ODBCDriver::TypeInfoRow::StoragePtrType resultRow;

  bool allTypes = dataType == SQL_ALL_TYPES;

  // if dataType == SQL_ALL_TYPES report on all.  Otherwise, just
  // report on the one we care about.
  if(allTypes || dataType == SQL_BIT)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_BIT, 2);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_TINYINT)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_TINYINT, 2, SQL_FALSE);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_BIGINT)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_BIGINT, 2, SQL_FALSE);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_CHAR)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_CHAR, 10, boost::none, 
                       tstring(TEXT("'")), tstring(TEXT("'")));

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_DECIMAL)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_DECIMAL, 2, SQL_FALSE);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_INTEGER)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_INTEGER, 2, SQL_FALSE);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_SMALLINT)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_SMALLINT, 2, SQL_FALSE);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_DOUBLE)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_DOUBLE, 2, SQL_FALSE);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_VARCHAR)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_VARCHAR, 10, boost::none, 
                       tstring(TEXT("'")), tstring(TEXT("'")));

   this->rows.push_back(resultRow);
  }
  // These need to be special cased.
  if(allTypes || dataType == SQL_TYPE_DATE)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_TYPE_DATE, 2);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_TYPE_TIME)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_TYPE_TIME, 2);

   this->rows.push_back(resultRow);
  }
  if(allTypes || dataType == SQL_TYPE_TIMESTAMP)
  {
   resultRow = this->conn->GetTypeInfo(dataType);
   if(!resultRow)
    resultRow = GetRow(SQL_TYPE_TIMESTAMP, 2);

   this->rows.push_back(resultRow);
  }

  if(this->rows.empty())
  {
   throw ODBCDriver::ClientException(
    "It does not support this SQL data type.");
  }

  this->eof = true;
 }

 ServerReturn GetColumnCount(std::size_t* colCount)
 {
  return ODBCDriver::TypeInfoResultSet::GetColumnCount(colCount);
 }

protected:

 void FetchNewResults()
 {
 }

};

/**
ResultSet implementation for SQLColumns.
*/
template<typename ConnectionType>
class ColumnResultSetImpl
 : public AbstractResultSet<ConnectionType, ODBCDriver::ColumnRow>,
   public ODBCDriver::ColumnResultSet
{
public:

 /**
 Constructor.
 */
 ColumnResultSetImpl(ConnectionType* _conn, const tstring& tblPattern,
                     const tstring& colPattern)
  : AbstractResultSet<ConnectionType, ODBCDriver::ColumnRow>(_conn),
    ODBCDriver::ColumnResultSet()
 {
  this->conn->FetchColumns(this->rows, tblPattern, colPattern);
  this->eof = true;

  // #TODO: SQLColumns returns the results as a standard result set, 
  //        ordered by TABLE_CAT, TABLE_SCHEM, TABLE_NAME, and 
  //        ORDINAL_POSITION. 
 }

 ServerReturn GetColumnCount(std::size_t* colCount)
 {
  return ODBCDriver::ColumnResultSet::GetColumnCount(colCount);
 }

protected:

 void FetchNewResults()
 {
 }

};

}
}

}
