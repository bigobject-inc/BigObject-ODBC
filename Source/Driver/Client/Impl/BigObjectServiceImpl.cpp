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


#include <string>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/move/core.hpp>

#include <json/json.h>

#include <curl/curl.h>

#include "BaseServiceImpl.hpp"
#include "BigObjectServiceImpl.hpp"

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
 Constructor.

 @param hDbc SQLHDBC.
 @param _db database name (workspace).
 @param _host Server host.
 @param _port Server port.
 @param _uid User.
 @param _password Password.
 */
 Connection(SQLHDBC hDbc, const tstring& _db, 
            const tstring& _host, const int _port,
            const tstring _uid, const tstring _password)
  : ODBCDriver::Connection(hDbc), 
    database(ODBC_TSTR_TO_STR(_db)), host(ODBC_TSTR_TO_STR(_host)), 
    port(_port), 
    uid(ODBC_TSTR_TO_STR(_uid)), password(ODBC_TSTR_TO_STR(_password)),
    url("http://"), buffer(new std::string),
    fetchIndex(-1), header(NULL)
 {
  logger = bigobject::Logging::LoggerFactory::GetLogger(TEXT("Connection"));

  url.append(host);
  url.push_back(':');
  url.append(boost::lexical_cast<std::string>(port));
  
  // There is a only single point now.
  url.append("/cmd"); 

  LOG_DEBUG_F(logger, TEXT("BigObject URL: %s"), 
              LOG4CPLUS_STRING_TO_TSTRING(url).c_str());

  // Test connection.
  SendRequest("select 1");

  LOG_DEBUG(logger, TEXT("Connected."));
 }

 /**
 Execute SQL statement.

 @return True if success otherwise false.
 */
 bool Execute(const tstring& statement)
 {
  std::string rawStmt(ODBC_TSTR_TO_STR(statement));
  // Please use BOProxy to transform SQL for BO server.
  //std::string stmt(ConvertToNativeSQL(rawStmt));
  std::string& stmt = rawStmt;

  Json::Value contentJson(SendRequest(stmt));

  // #TODO: Support more operations.
  if(contentJson.empty() || contentJson.isNull()) // e.g. BUILD ASSOCIATION
   return true;

  // Content supposes to a resource id.
  try
  {
   resource = contentJson.get("res", -1).asString();
  }
  // Json::Logic cannot be caught as the destructor and/or copy constructor 
  // are inaccessible. :(
  catch(...) 
  {
   throw ODBCDriver::ClientException("Unable to get resource id.");
  }

  columnList = ReadSchema();

  // Reset control variables.
  fetchIndex = 1; // Scan index not zero-based.

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
  // No schema.
  if(columnList.empty())
   return 0;

  // http://bigobject.readme.io/v1.?/docs/bigobject-remote-api
  /*
  SCAN

  The syntax for retrieving data is specified in the following format 
  (case insensitive). 
  This is provided in the Stmt field in the request payload.

  SCAN <RESOURCE_ID> <START> <END> <PAGE>

  START and END positions are inclusive. PAGE defines the chunk size. 
  The maximum allowed data chunk size is 1000.

  Example (case insensitive):

  SCAN 12cf0a5dee76
  Scan all data, each data chunk will be the default page size
  SCAN 12cf0a5dee76 145
  Scan data from index 145 to the end.
  SCAN 12cf0a5dee76 100 9922
  Scan data from index 100 to index 9922.
  SCAN 12cf0a5dee76 1 -1 500
  Scan data from begin to end, adjusting chunk size to 500
  */
  // Example:
  /*
  {
  "Content":{
      "content":[
          ["1","8186","3798","am/pm","2013-01-01 00:22:44",6,49.62],
          ["1","8186","4389","7-11","2013-01-01 02:01:34",3,27.96],
          ...
      ],
      "index":11
   },
   "Err":""
  }
  {
  "Content":{
      "index":-1,
      "content":[]
   },
   "Err":""
  }
  */   
  if(fetchIndex == -1)
   return 0;

  // #TODO: If maxRows > 1000?.

  std::string scan("scan ");
  scan.append(resource);
  scan.push_back(' ');
  // START
  scan.append(boost::lexical_cast<std::string>(fetchIndex));
  scan.push_back(' ');
  // END
  scan.append(boost::lexical_cast<std::string>(fetchIndex + maxRows - 1));
  scan.push_back(' ');
  // PAGE (chunksize)
  scan.append(boost::lexical_cast<std::string>(maxRows));

  Json::Value contentJson(SendRequest(scan));

  // Update fetch index.
  fetchIndex = contentJson.get("index", -1).asInt();
  
  // Read data.
  Json::Value dataJson = contentJson["content"];
  for(std::size_t i = 0; i < dataJson.size(); ++i)
  {
   const Json::Value rowJson = dataJson[(int)i];
   
   ODBCDriver::StringRow::StoragePtrType row(
    new ODBCDriver::StringRow::StorageType);

   for(std::size_t j = 0; j < rowJson.size(); ++j)
    row->push_back(ODBC_STR_TO_TSTR(rowJson.get((int)j, "").asString()));

   container.push_back(row);
  }

  return dataJson.size();
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
  // Example:
  /*
  {
  "Content": [
      \"Product\", \"Customer\", \"sales\"
  ],
  "Err":""
  }
  */
  std::string cmd("show tables");
  Json::Value contentJson(SendRequest(cmd));
  
  std::size_t matched = 0;
  for(std::size_t i = 0; i < contentJson.size(); ++i)
  {
   const Json::Value tableJson = contentJson[(int)i];
   tstring table(ODBC_STR_TO_TSTR(tableJson.asString()));

   // Very very simple pattern search implementation.
   // #TODO: Do real pattern search.
   if(pattern == TEXT("%") || pattern == TEXT("*") ||
      table.find(pattern) != tstring::npos)
   {
    container.push_back(ODBCDriver::TableRow::Create(
     tstring(TEXT("")), // TABLE_CAT
     tstring(TEXT("")), // TABLE_SCHEM
     table, // TABLE_NAME
     tstring(TEXT("TABLE")), // TABLE_TYPE
     tstring(TEXT("")) // REMARKS
    ));

    ++matched;
   }
  }

  return matched;
 }

 /**
 Get type info row.

 @param dType SQL data type.
 @return TypeInfoRow::RowStoragePtrType, NULL if not implemented.
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
  typedef Base::TableResultSetImpl<Connection>::RowValueSet RowValueSet;

  RowValueSet tableRows;

  if(FetchTables(tableRows, tblPattern) == 0)
   return 0;

  RowValueSet::iterator i = tableRows.begin();
  RowValueSet::iterator iEnd = tableRows.end();
  for(; i != iEnd; ++i)
  {
   ODBCDriver::TableRow::StoragePtrType& tableRow = *i;
   tstring& tableName = std::tr1::get<2>(*tableRow);

   // Read schema for table.
   Base::ColumnList tableColumns = ReadSchema(tableName);

   Base::ColumnList matched;

   Base::ColumnList::iterator j = tableColumns.begin();
   Base::ColumnList::iterator jEnd = tableColumns.end();
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
     tableName, // 2, TABLE_NAME
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

 /**
 Get column list (metadata).

 @return Column list.
 */
 Base::ColumnList& GetColumnList()
 {
  return columnList;
 }

private:

 /**
 Base holder to keep result memory allocated in main thread (or the other
 thread which deals with fetching and rendering).
 */
 template <typename T, typename R, typename Impl = boost::shared_ptr<T> >
 struct MemoryHolder
 {
  typedef T ValueType;
  typedef R ReturnType;
  typedef Impl ImplType;

  MemoryHolder(T* v) : value(v)
  {
  }

  inline T* Get() const
  {
   return (T*)value.get();
  }

  inline void Reset()
  {
   value.reset();
  }

  inline void Reset(T* p)
  {
   value.reset(p);
  }

  inline operator bool() const
  {
   return (bool)value;
  }

  ImplType value;
 };

 template <typename T = const char>
 struct StringHolderBase : public MemoryHolder<T, const char*>
 {
  typedef MemoryHolder<T, const char*> BaseType;

  StringHolderBase(T* v) : BaseType(v)
  {
  }
 };

 // StringHolder for string.
 struct StringHolder : StringHolderBase<std::string>
 {
  typedef StringHolderBase<std::string> BaseType;

  StringHolder(std::string* v) : BaseType(v)
  {
  }

  const char* Cast() const
  {
   return BaseType::value->c_str();
  }
 };

 static int WriteResponse(char* data, size_t size, size_t nmemb,
                          Connection* handler)
 {
  handler->buffer.Get()->append(data, size * nmemb);
  return (int)(size * nmemb);
 }

#define _CURL_SETOPT(opt, v)                                                 \
 code = curl_easy_setopt(conn, opt, v);                                      \
 if(code != CURLE_OK)                                                        \
 {                                                                           \
  LOG_ERROR(logger, TEXT("curl_easy_setopt ") << opt << TEXT(" EXCEPTION")); \
  throw ODBCDriver::ClientException((                                        \
   ("Failed to set " #opt " [") + std::string(errorBuffer) + "].").c_str()); \
 }

 /**
 Build HTTP request for BigObject.

 @return CURL*
 */
 CURL* BuildRequest()
 {
  CURLcode code;  
  char errorBuffer[CURL_ERROR_SIZE];

  CURL* conn = curl_easy_init();

  if(conn == NULL)
  {
   LOG_ERROR(logger, TEXT("curl_easy_init EXCEPTION"));
   throw ODBCDriver::ClientException("Failed to create CURL connection.");
  }

  code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
  if(code != CURLE_OK)
  {
   LOG_ERROR(logger, TEXT("curl_easy_setopt CURLOPT_ERRORBUFFER EXCEPTION"));
   throw ODBCDriver::ClientException((
   "Failed to set error buffer [" + 
    boost::lexical_cast<std::string>(code) + 
    "]").c_str());
  }

  header = curl_slist_append(header, "User-Agent: Mozilla/4.0");
  header = curl_slist_append(header, "Accepts: application/json");
  header = curl_slist_append(header, "Accept-Charset: UTF-8,*");
  //header = curl_slist_append(header, "Accept-Encoding: gzip,deflate");

  header = curl_slist_append(header, "Content-Type: application/json");

  _CURL_SETOPT(CURLOPT_HTTPHEADER, header)
  _CURL_SETOPT(CURLOPT_AUTOREFERER, true)
  _CURL_SETOPT(CURLOPT_FORBID_REUSE, 1)
  _CURL_SETOPT(CURLOPT_FILETIME, 1)
  _CURL_SETOPT(CURLOPT_ENCODING, "gzip")

  _CURL_SETOPT(CURLOPT_COOKIEFILE, "")

  _CURL_SETOPT(CURLOPT_URL, url.c_str())
  _CURL_SETOPT(CURLOPT_FOLLOWLOCATION, 1L)
  _CURL_SETOPT(CURLOPT_FAILONERROR, 1)

  _CURL_SETOPT(CURLOPT_WRITEFUNCTION, Connection::WriteResponse)
  _CURL_SETOPT(CURLOPT_WRITEDATA, this)

  return conn;
 }

 /**
 Send statment request to BigObject server.

 @param stmt Statement.
 @return Content JSON.
 */
 Json::Value SendRequest(const std::string& stmt)
 {
  CURL *conn = BuildRequest();
  char errorBuffer[CURL_ERROR_SIZE];

  CURLcode code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);

  std::ostringstream out;
  Json::Value requestJson;
  Json::Value optsJson;

  optsJson["Handle"] = true;

  // {"Stmt": "stmt", "Workspace": "", "Opts": ""}
  requestJson["Stmt"] = stmt;
  requestJson["Workspace"] = database;
  requestJson["Opts"] = optsJson;

  Json::StreamWriterBuilder wbuilder;
  wbuilder["indentation"] = "";

  out << Json::writeString(wbuilder, requestJson);

  std::string postData(out.str());

  LOG_DEBUG_F(logger, TEXT("SEND: %s"), 
              LOG4CPLUS_STRING_TO_TSTRING(postData).c_str());

  _CURL_SETOPT(CURLOPT_POST, 1)
  _CURL_SETOPT(CURLOPT_POSTFIELDSIZE, postData.size())
  _CURL_SETOPT(CURLOPT_COPYPOSTFIELDS, postData.c_str())

  // Retrieve content for the URL.
  code = curl_easy_perform(conn);

  if(code != CURLE_OK)
  {
   LOG_DEBUG(logger, TEXT("curl_easy_perform != CURL_OK"));

   long status = 0;

   if(code == CURLE_REMOTE_ACCESS_DENIED || code == CURLE_LOGIN_DENIED)
    status = 401;
   else if(code == CURLE_HTTP_RETURNED_ERROR)
    code = curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &status);
   else
    code = curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &status);

   if(status != 0)
   {
    if(code != CURLE_OK)
    {
     LOG_ERROR(logger, TEXT("curl_easy_getinfo EXCEPTION"));

     throw ODBCDriver::ClientException((
      "Failed to get '" + url + "' [" + 
      std::string(errorBuffer) + "].").c_str());
    }
   }

   curl_easy_cleanup(conn);
   if(header)
   {
    curl_slist_free_all(header);
    header = NULL;
   }

   throw ODBCDriver::ClientException((
    "curl status code: " + boost::lexical_cast<std::string>(status)).c_str());
  }

  // Only CURLE_OK will be here.
  curl_easy_cleanup(conn);
  if(header)
  {
   curl_slist_free_all(header);
   header = NULL;
  }

  // Make a alias to buffer content.
  std::string in(*buffer.Get());
  buffer.Reset(new std::string());

  LOG_DEBUG_F(logger, TEXT("GOT: %s"), 
              LOG4CPLUS_STRING_TO_TSTRING(in).c_str());

  // {"Content":"","Err":""}
  Json::Reader reader;
  Json::Value responseJson;
  
  bool parsed = reader.parse(in, responseJson);
  if(!parsed)
   throw ODBCDriver::ClientException("Illegal JSON response.");

  std::string err(responseJson.get("Err", "").asString());

  if(!err.empty()) // Error occurred.
   throw ODBCDriver::ClientException(("Err: " + err).c_str());

  // RVO?
  return BOOST_MOVE_RET(Json::Value, responseJson["Content"]);
 }

 /**
 Read schema meta information. (hdesc resource or table)

 @param table Table name. Empty if read fetched resource.
 @return Read columns.
 */
 Base::ColumnList ReadSchema(tstring table = tstring(TEXT("")))
 {
  // Example:
  /* 
  {
  "Content":{
      "create_stmt": "CREATE TABLE tmp_1 ('id' STRING, 'value' STRING, )", 
      "name": "tmp_1", 
      "schema": {
          "attr": [
              {
                  "name": "id", 
                  "type": "STRING"
              }, 
              {
                  "name": "value", 
                  "type": "STRING"
              }
          ]}, 
          "size": 100
      }",
  "Err":""
  }
  */
  Json::Value contentJson;
  std::string cmd;

  if(table.empty())
  {
   cmd.append("hdesc ");
   cmd.append(resource);
  }
  else
  {
   cmd.append("desc ");
   cmd.append(ODBC_TSTR_TO_STR(table));
  }

  contentJson = Json::Value(SendRequest(cmd));

  // Construct column list.
  Base::ColumnList columns;
  const Json::Value attr = contentJson["schema"]["attr"];
  for(std::size_t i = 0; i < attr.size(); ++i)
  {
   const Json::Value c = attr[(int)i];

   ODBCDriver::ServerType type = GetType(c.get("type", "UNKNOWN").asString());
   
   columns.push_back(Base::Column(
    ODBC_STR_TO_TSTR(c.get("name", "").asString()), 
    type,
    ODBCDriver::ColumnDescriptor::GetSQLTypeName(
     ODBCDriver::ColumnDescriptor::GetSQLType(type))));
  }

  return columns;
 }

 /**
 Get ServerType for given type string.

 @see http://bigobject.readme.io/v1.0/docs/data-model

 @param t Type string.
 @return Corresponding ServerType.
 */
 ODBCDriver::ServerType GetType(const std::string& t)
 {
  std::string t2(t);
  boost::algorithm::to_upper(t2);

  if(t2 == "STRING" || t2 == "BYTE")
   return SERVER_STRING_TYPE;
  else if(t2 == "INTEGER" || t2 == "INT32")
   return SERVER_INT_TYPE;
  else if(t2 == "TINYINT" || t2 == "INT8")
   return SERVER_TINYINT_TYPE;
  else if(t2 == "SMALLINT" || t2 == "INT16" )
   return SERVER_SMALLINT_TYPE;
  else if(t2 == "BIGINT" || t2 == "INT64")
   return SERVER_BIGINT_TYPE;
  else if(t2 == "FLOAT")
   return SERVER_FLOAT_TYPE;
  else if(t2 == "REAL" || t2 == "DOUBLE" || t2 == "DECIMAL")
   return SERVER_DOUBLE_TYPE;
  else if(t2 == "DATE" || t2 == "DATE32")
   return SERVER_DATE_TYPE;
  // DATETIME64?!
  else if(t2 == "TIME" || t2 == "DATETIME32" || t2 == "DATETIME64")
   return SERVER_DATETIME_TYPE;
  else if(t2 == "TIMESTAMP")
   return SERVER_TIMESTAMP_TYPE;
  else
   return SERVER_UNKNOWN_TYPE;
 }

private:

 /// Logger.
 bigobject::Logging::Logger logger;

 const std::string database;

 const std::string host;

 const int port;

 const std::string uid;

 const std::string password;

 std::string url;

 /// Request header.
 curl_slist *header;

 /// Response content.
 StringHolder buffer;
 
 std::string resource;

 int fetchIndex;

 Base::ColumnList columnList;

};

};


BigObjectServiceImpl::BigObjectServiceImpl()
{
 logger = bigobject::Logging::LoggerFactory::GetLogger(
  TEXT("BigObjectServiceImpl"));
}

Connection* BigObjectServiceImpl::OpenConnection(
 SQLHDBC hDbc, const tstring& database, const tstring& host, int port,
 const tstring uid, const tstring password)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);
 return new Detail::Connection(hDbc, database, host, port, uid, password);
}

ServerReturn BigObjectServiceImpl::CloseConnection(Connection* connection)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 delete connection;
 return SERVER_SUCCESS;
}

ServerReturn BigObjectServiceImpl::Execute(
 Connection* connection, const tstring& query,
 ResultSet** pResultSet, std::size_t maxBufRows)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 /* resultset_ptr may be NULL if the caller does not care about the result */
 if(pResultSet != NULL)
 {
  *pResultSet = new Detail::Base::QueryResultSet<Detail::Connection>(
   (Detail::Connection*)connection, query, maxBufRows);
 }

 return SERVER_SUCCESS;
}

ServerReturn BigObjectServiceImpl::GetTables(
 Connection* connection, const tstring& pattern, ResultSet** pResultSet)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 if(pResultSet != NULL)
 {
  *pResultSet = new Detail::Base::TableResultSetImpl<Detail::Connection>(
   (Detail::Connection*)connection, pattern);
 }

 return SERVER_SUCCESS;
}

ServerReturn BigObjectServiceImpl::GetTypeInfo(
 Connection* connection, SQLSMALLINT dataType, ResultSet** pResultSet)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 if(pResultSet != NULL)
 {
  *pResultSet = new Detail::Base::TypeInfoResultSetImpl<Detail::Connection>(
   (Detail::Connection*)connection, dataType);
 }

 return SERVER_SUCCESS;
}

ServerReturn BigObjectServiceImpl::GetColumns(
 Connection* connection, const tstring& tblPattern, const tstring& colPattern,
 ResultSet** pResultSet)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 *pResultSet = new Detail::Base::ColumnResultSetImpl<Detail::Connection>(
  (Detail::Connection*)connection, tblPattern, colPattern);

 return SERVER_SUCCESS;
}

ServerReturn BigObjectServiceImpl::CloseResultSet(ResultSet* resultset)
{
 LOG_DEBUG(logger, LOG_DECORATED_FUNCTION_NAME);

 delete resultset;
 return SERVER_SUCCESS;
}

}
