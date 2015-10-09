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


#include <fstream> 

#include <boost/tokenizer.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/tuple/tuple.hpp>

#include <boost/filesystem.hpp>

#include "BaseServiceImpl.hpp"
#include "CSVServiceImpl.hpp"

namespace ODBCDriver
{
namespace Detail
{

/**
Connection implementation.
*/
class Connection : public ODBCDriver::Connection
{
public:
 
 /**
 Internal client for reading CSV file as data source.
 */
 struct CSVClient
 {
  /// CSV tokenizer.
  typedef boost::tokenizer<
   boost::escaped_list_separator<TCHAR>, 
   tstring::const_iterator, 
   tstring
  > Tokenizer;

  /// Logger.
  bigobject::Logging::Logger logger;

  /// CSV file name.
  const tstring csv;

  /// CSV file input stream.
  tifstream in;

  /// Column list for column meta information.
  Base::ColumnList columList;

  /**
  Constructor.

  @param _csv CSV file name.
  */
  CSVClient(const tstring& _csv) : csv(_csv), in(_csv.c_str())
  {   
   logger = bigobject::Logging::LoggerFactory::GetLogger(TEXT("CSVClient"));

   if(!in.is_open()) 
    throw ODBCDriver::ClientException("Unable to open CSV file.");
  }

  /**
  Destructor.
  */
  ~CSVClient()
  {
   in.close();
  }

  /**
  Reset CSV file reading point.
  */
  void Reset()
  {
   in.clear();
   in.seekg(0, in.beg);
   
   // Skip first schema line.
   tstring line;
   std::getline(in, line);
  }

  /**
  Get ServerType for given type string from header.

  @param t Type string.
  @return Corresponding ServerType.
  */
  ODBCDriver::ServerType GetType(const tstring& t)
  {
   tstring t2(t);
   boost::algorithm::to_upper(t2);

   if(t2.find(TEXT("CHAR")) != tstring::npos)
    return SERVER_STRING_TYPE;
   else if(t2 == TEXT("INTEGER") || t2 == TEXT("INT"))
    return SERVER_INT_TYPE;
   else if(t2 == TEXT("SMALLINT"))
    return SERVER_SMALLINT_TYPE;
   else if(t2 == TEXT("BIGINT"))
    return SERVER_BIGINT_TYPE;
   else if(t2 == TEXT("FLOAT"))
    return SERVER_FLOAT_TYPE;
   else if(t2 == TEXT("REAL") || t2 == TEXT("DOUBLE") || t2 == TEXT("DECIMAL"))
    return SERVER_DOUBLE_TYPE;
   else if(t2 == TEXT("DATE"))
    return SERVER_DATE_TYPE;
   else if(t2 == TEXT("TIME"))
    return SERVER_TIME_TYPE;
   else if(t2 == TEXT("DATETIME"))
    return SERVER_DATETIME_TYPE;
   else if(t2 == TEXT("TIMESTAMP"))
    return SERVER_TIMESTAMP_TYPE;
   else
    return SERVER_UNKNOWN_TYPE;
  }

  /**
  Read schema meta information. CSV file's first line must be columns formatted
  as name:type. For example id:int,name:varchar(30),age:int,height:float
  */
  void ReadSchema()
  {
   // Need to reset cursor because schema line is on first line.
   in.clear();
   in.seekg(0, in.beg);
   columList.clear();

   tstring line;
   std::vector<tstring> vec;

   if(std::getline(in, line))
   {
    Tokenizer tok(line);
    vec.assign(tok.begin(), tok.end());

    std::vector<tstring>::iterator i = vec.begin();
    std::vector<tstring>::iterator iEnd = vec.end();
    for(; i != iEnd; ++i)
    {
     std::vector<tstring> kv;
     boost::split(kv, *i, boost::is_any_of(":"));
     columList.push_back(
      Base::Column(kv[0], 
                   GetType(kv[1]), 
                   boost::algorithm::to_upper_copy(kv[1])));
    }
   }
  }

  /**
  Fetch data for given maximum rows.

  @param container Result container reference.
  @param maxRows Maximum reading rows.
  @return Actual reading rows.
  */
  template<typename T>
  std::size_t Fetch(T& container, std::size_t maxRows)
  {
   tstring line;
   std::size_t count = 0;

   while(std::getline(in, line))
   {
    StringRow::StoragePtrType vec;

    try
    {
     Tokenizer tok(line);
     vec = StringRow::Create(tok.begin(), tok.end());
    }
    catch(boost::escaped_list_error& ex)
    {
     LOG_ERROR(logger, LOG4CPLUS_STRING_TO_TSTRING(ex.what()));
     // Skip this broken line.
     continue;
    }

    container.push_back(vec);

    if(count >= maxRows)
     break;

    ++count;
   }

   return count;
  }

  /**
  Fetch tables information.

  @param container Result container reference.
  @param pattern Table search pattern.
  @return Tables count.
  */
  template<typename T>
  std::size_t FetchTables(T& container, const tstring& pattern)
  {
   boost::filesystem::path p(csv);

   tstring filename(p.stem().c_str());

   // Very very simple pattern search implementation.
   // #TODO: Do real pattern search.
   if(pattern == TEXT("%") || pattern == TEXT("*") ||
      filename.find(pattern) != tstring::npos)
   {
    container.push_back(ODBCDriver::TableRow::Create(
     tstring(TEXT("")), // TABLE_CAT
     tstring(TEXT("")), // TABLE_SCHEM
     filename, // TABLE_NAME
     tstring(TEXT("TABLE")), // TABLE_TYPE
     tstring(TEXT("")) // REMARKS
    ));

    return 1;
   }
   else
   {
    return 0;
   }
  }

  /**
  Fetch columns information.

  @param container Result container reference.
  @param tblPattern Table search pattern.
  @param colPattern Column search pattern.
  @return Row count.
  */
  template<typename T>
  std::size_t FetchColumns(T& container, const tstring& tblPattern,
                           const tstring& colPattern)
  {
   typedef Base::TableResultSetImpl<Connection>::RowValueSet RowValueSet;

   RowValueSet tableRows;

   if(FetchTables(tableRows, tblPattern) == 0)
    return 0;

   RowValueSet::iterator i = tableRows.begin();
   RowValueSet::iterator iEnd = tableRows.end();
   for(; i != iEnd; ++i)
   {
    ODBCDriver::TableRow::StoragePtrType& tableRow = *i;

    // There should always be only one table here. So update schema.
    ReadSchema();

    Base::ColumnList matched;

    Base::ColumnList::iterator j = columList.begin();
    Base::ColumnList::iterator jEnd = columList.end();
    for(; j != jEnd; ++j)
    {
     tstring& columnName = boost::get<0>(*j);

     // Very very simple pattern search implementation.
     // #TODO: Do real pattern search.
     if(colPattern == TEXT("%") || colPattern == TEXT("*") ||
        columnName.find(colPattern) != tstring::npos)
     {
      matched.push_back(*j);
     }
    }

    // Now we have column information. Create row.
    j = matched.begin();
    jEnd = matched.end();
    for(int p = 0; j != jEnd; ++j, ++p)
    {
     Base::Column& column = *j;

     ServerType serverType = boost::get<1>(column);
     SQLSMALLINT dType = ColumnDescriptor::GetSQLType(serverType);

     // NUM_PREC_RADIX
     boost::optional<SQLSMALLINT> numPrecRadix;
     if(dType == SQL_DECIMAL || dType == SQL_DOUBLE)
      numPrecRadix = 10;
     else
      numPrecRadix = boost::none;

     // SQL_DATA_TYPE
     SQLSMALLINT sqlDataType = dType;
     if(dType == SQL_TYPE_DATE || dType == SQL_TYPE_TIME ||
        dType == SQL_TIMESTAMP_LEN)
     {
      sqlDataType = SQL_DATETIME;
     }

     // CHAR_OCTECT_LENGTH
     boost::optional<SQLINTEGER> charOctectLength;
     if(serverType == SERVER_STRING_TYPE)
      charOctectLength = ColumnDescriptor::GetFieldByteSize(serverType);
     else
      charOctectLength = boost::none;
     
     container.push_back(ColumnRow::Create(
      tstring(TEXT("")), // 0, TABLE_CAT
      tstring(TEXT("")), // 1, TABLE_SCHEM
      std::tr1::get<2>(*tableRow), // 2, TABLE_NAME
      boost::get<0>(column), // 3, COLUMN_NAME
      dType, // 4, DATA_TYPE
      ColumnDescriptor::GetSQLTypeName(dType), // 5, TYPE_NAME
      ColumnDescriptor::GetMaxDisplaySize(serverType), // 6, COLUMN_SIZE
      ColumnDescriptor::GetFieldByteSize(serverType), // 7, BUFFER_LENGTH
      boost::none, // 8, DECIMAL_DIGITS
      numPrecRadix, // 9, NUM_PREC_RADIX
      SQL_NULLABLE, // 10, NULLABLE
      boost::none, // 11, REMARKS
      boost::none, // 12, COLUMN_DEF
      sqlDataType, // 13, SQL_DATA_TYPE
      boost::none, // 14, SQL_DATETIME_SUB
      charOctectLength, // 15, CHAR_OCTET_LENGTH
      p, // 16, ORDINAL_POSITION
      tstring(TEXT("YES")) // 17, IS_NULLABLE
     ));
    }

    return matched.size();
   }
   
   return 0;
  }

 };

public:
 
 /**
 Constructor.

 @param hDbc SQLHDBC.
 @param _db CSV file name.
 */
 Connection(SQLHDBC hDbc, const tstring& _db) 
  : ODBCDriver::Connection(hDbc), client(_db)
 {
 }

 /**
 Execute SQL statement.
  
 @return True if success otherwise false.
 */
 bool Execute(const tstring& statement)
 {
  // Nothing can execute.
  client.ReadSchema();
  return true;
 } 
 
 /**
 Fetch data for given maximum rows.

 @param container Result container reference.
 @param maxRows Maximum reading rows.
 @return Actual reading rows.
 */
 template<typename T>
 std::size_t Fetch(T& container, std::size_t maxRows)
 {
  return client.Fetch(container, maxRows);
 }
 
 /**
 Fetch tables information.

 @param container Result container reference.
 @param pattern Table search pattern.
 @return Tables count.
 */
 template<typename T>
 std::size_t FetchTables(T& container, const tstring& pattern)
 {
  return client.FetchTables(container, pattern);
 } 

 /**
 Get type info row.

 @param dType SQL data type.
 @return TypeInfoRow::StoragePtrType, NULL if not implemented.
 */
 TypeInfoRow::StoragePtrType GetTypeInfo(SQLSMALLINT dType)
 {
  return TypeInfoRow::StoragePtrType();
 }

 /**
 Fetch columns information.

 @param container Result container reference.
 @param tblPattern Table search pattern.
 @param colPattern Column search pattern.
 @return Row count.
 */
 template<typename T>
 std::size_t FetchColumns(T& container, const tstring& tblPattern, 
                          const tstring& colPattern)
 {
  return client.FetchColumns(container, tblPattern, colPattern);
 }

 /**
 Reset CSV file reading point.
 */
 void ResetCursor()
 {
  client.Reset();
 }

 /**
 Get column list (metadata).

 @return Column list.
 */
 Base::ColumnList& GetColumnList()
 {
  return client.columList;
 }

private:
 
 /// Internal client.
 CSVClient client;

};

}

CSVServiceImpl::CSVServiceImpl()
{
 logger = bigobject::Logging::LoggerFactory::GetLogger(TEXT("CSVServiceImpl"));
}

Connection* CSVServiceImpl::OpenConnection(
 SQLHDBC hDbc, const tstring& database, const tstring& host, int port
 const tstring uid, const tstring password)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);
 return new Detail::Connection(hDbc, database);
}

ServerReturn CSVServiceImpl::CloseConnection(Connection* connection)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 delete connection;
 return SERVER_SUCCESS;
}

ServerReturn CSVServiceImpl::Execute(
 Connection* connection, const tstring& query,
 ResultSet** pResultSet, std::size_t maxBufRows)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 /* pResultSet may be NULL if the caller does not care about the result */
 if(pResultSet != NULL)
 {
  *pResultSet = new Detail::Base::QueryResultSet<Detail::Connection>(
   (Detail::Connection*)connection, query, maxBufRows);
 }

 return SERVER_SUCCESS;
}

ServerReturn CSVServiceImpl::GetTables(
 Connection* connection, const tstring& pattern, ResultSet** pResultSet)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);
 
 *pResultSet = new Detail::Base::TableResultSetImpl<Detail::Connection>(
  (Detail::Connection*)connection, pattern);
 
 return SERVER_SUCCESS;
}         

ServerReturn CSVServiceImpl::GetTypeInfo(
 Connection* connection, SQLSMALLINT dataType, ResultSet** pResultSet)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);
 
 *pResultSet = new Detail::Base::TypeInfoResultSetImpl<Detail::Connection>(
  (Detail::Connection*)connection, dataType);
 
 return SERVER_SUCCESS;
}                 

ServerReturn CSVServiceImpl::GetColumns(
 Connection* connection, const tstring& tblPattern, const tstring& colPattern, 
 ResultSet** pResultSet)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 *pResultSet = new Detail::Base::ColumnResultSetImpl<Detail::Connection>(
  (Detail::Connection*)connection, tblPattern, colPattern);

 return SERVER_SUCCESS;
}

ServerReturn CSVServiceImpl::CloseResultSet(ResultSet* resultset)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 delete resultset;
 return SERVER_SUCCESS;
}

}
