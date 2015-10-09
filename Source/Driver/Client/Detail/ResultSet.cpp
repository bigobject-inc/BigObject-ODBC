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


#include "ResultSet.hpp"

namespace ODBCDriver
{

ServerReturn TableResultSet::GetColumnCount(std::size_t* colCount)
{
 *colCount = 5;
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn TableResultSet::CreateColumnDescriptor(
  size_t columnIdx, ColumnDescriptor** columnDescPtr)
{
 *columnDescPtr = new TableColumnDescriptor(columnIdx);
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn TableResultSet::CloseColumnDescriptor(
 ColumnDescriptor* columnDescPtr)
{
 delete columnDescPtr;
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn TypeInfoResultSet::GetColumnCount(std::size_t* colCount)
{
 *colCount = 19;
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn TypeInfoResultSet::CreateColumnDescriptor(
 size_t columnIdx, ColumnDescriptor** columnDescPtr)
{
 *columnDescPtr = new TypeInfoColumnDescriptor(columnIdx);
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn TypeInfoResultSet::CloseColumnDescriptor(
 ColumnDescriptor* columnDescPtr)
{
 delete columnDescPtr;
 return ODBCDriver::SERVER_SUCCESS;
}

TypeInfoRow::StoragePtrType TypeInfoResultSet::GetRow(
 SQLSMALLINT dType, boost::optional<SQLINTEGER> percRadix,
 boost::optional<SQLSMALLINT> unsignedAtt,
 boost::optional<tstring> literalPrefix,
 boost::optional<tstring> literalSuffix)
{
 TypeInfoRow::StoragePtrType resultRow;

 // SQL_DATA_TYPE
 SQLSMALLINT sqlDataType;
 // SQL_DATETIME_SUB
 boost::optional<SQLSMALLINT> sqlDatetimeSub;

 // SQL_DATA_TYPE and SQL_DATETIME_SUB
 if(dType == SQL_TYPE_DATE || dType == SQL_TYPE_TIME ||
  dType == SQL_TYPE_TIMESTAMP)
 {
  sqlDataType = SQL_DATETIME;
  sqlDatetimeSub = dType;
 }
 else
 {
  sqlDataType = dType;
  sqlDatetimeSub = boost::none;
 }

 resultRow = TypeInfoRow::Create(
  ColumnDescriptor::GetSQLTypeName(dType), // TYPE_NAME
  dType, // DATA_TYPE
  // COLUMN_SIZE
  (SQLINTEGER)ColumnDescriptor::GetFieldByteSize(
  ColumnDescriptor::GetServerType(dType)),
  literalPrefix, // LITERAL_PREFIX
  literalSuffix, // LITERAL_SUFFIX
  // CREATE_PARAMS
  // This might need to be "length" for VARCHAR and CHAR, but since we
  // don't support CREATE, lets leave it NULL for now
  boost::none,
  SQL_NULLABLE, // NULLABLE
  SQL_FALSE, // CASE_SENSITIVE
  SQL_SEARCHABLE, // SEARCHABLE
  unsignedAtt, // UNSIGNED_ATTRIBUTE
  SQL_FALSE, // FIXED_PERC_SCALE
  boost::none, // AUTO_UNIQUE_VALUE
  ColumnDescriptor::GetSQLTypeName(dType), // LOCAL_TYPE_NAME
  boost::none, // MINIMUM_SCALE
  boost::none, // MAXIMUM_SCALE
  sqlDataType, // SQL_DATA_TYPE
  sqlDatetimeSub, // SQL_DATETIME_SUB
  percRadix, // NUM_PERC_RADIX
  boost::none // INTERVAL_PRECISION
 );

 return resultRow;
}

ServerReturn ColumnResultSet::GetColumnCount(std::size_t* colCount)
{
 *colCount = 18;
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn ColumnResultSet::CreateColumnDescriptor(
 size_t columnIdx, ColumnDescriptor** columnDescPtr)
{
 *columnDescPtr = new ColumnColumnDescriptor(columnIdx);
 return ODBCDriver::SERVER_SUCCESS;
}

ServerReturn ColumnResultSet::CloseColumnDescriptor(
 ColumnDescriptor* columnDescPtr)
{
 delete columnDescPtr;
 return ODBCDriver::SERVER_SUCCESS;
}

}
