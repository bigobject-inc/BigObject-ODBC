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

#ifndef _BIGOBJECT_ODBC_CLIENT_COLUMN_DESCRIPTOR_
#define _BIGOBJECT_ODBC_CLIENT_COLUMN_DESCRIPTOR_

#include "Client.hpp"
#include "Exception.hpp"

namespace ODBCDriver
{

/**
Abstract descriptor for a column in a ResultSet.

This class stores the information describing a column in a ResultSet.
It was only meant to be created by CreateColumnDescriptor and destroyed by 
CloseColumnDescriptor.
*/
class ColumnDescriptor
{
public:

 /**
 Get maximum display size.

 @param serverType ServerType.
 @return Display size.
 */
 static std::size_t GetMaxDisplaySize(ServerType serverType);

 /**
 Get corresponding field size in bytes.

 @param serverType ServerType.
 @param Field byte size.
 */
 static std::size_t GetFieldByteSize(ServerType serverType);

 /**
 Get SQL type. (_NativeToSQLType in Apache Hive implementation).

 @param serverType ServerType.
 @return SQL type.
 */
 static int GetSQLType(ServerType serverType);

 /**
 Get SQL type name. 

 https://msdn.microsoft.com/en-us/library/ms710150%28v=vs.85%29.aspx

 @param dType SQL data type.
 @return SQL type name.
 */
 static tstring GetSQLTypeName(SQLSMALLINT dType);

 /**
 Get ServerType by SQL data type.

 @param dType SQL data type.
 @return ServerType.
 */
 static ServerType GetServerType(SQLSMALLINT dType);

public:

 /**
 Destructor.
 */
 virtual ~ColumnDescriptor()
 {
 }

 /**
 Get column name.

 @return Column name.
 */
 virtual tstring GetColumnName() = 0;

 /**
 Get column type.

 @return Column type.
 */
 virtual tstring GetColumnType() = 0;

 /**
 Get column's server type.

 @return ServerType.
 */
 virtual ServerType GetServerType() = 0;

 /**
 Is this column nullable?

 @return True if allow nullable otherwise false.
 */
 virtual bool GetIsNullable() = 0;

 /**
 Is this column case sensitive?

 @return True if it is case sensitive otherwise false.
 */
 virtual bool GetIsCaseSensitive() = 0;

 /**
 Get maximum display size.

 @return Display size.
 */
 virtual std::size_t GetMaxDisplaySize()
 {
  return GetMaxDisplaySize(GetServerType());
 }

 /**
 Get corresponding field size in bytes.

 @param Field byte size.
 */
 virtual std::size_t GetFieldByteSize()
 {
  return GetFieldByteSize(GetServerType());
 }

 /**
 Get column's SQL type. (_NativeToSQLType in Apache Hive implementation).

 @return SQL type.
 */
 virtual int GetSQLType()
 {
  return GetSQLType(GetServerType());
 }

 /**
 Get column's SQL type name.

 @return SQL type name.
 */
 virtual tstring GetSQLTypeName()
 {
  return GetSQLTypeName(GetSQLType(GetServerType()));
 }

};

///////////////////////////////////////////////////////////////
// Special descriptors forward Declaration.
///////////////////////////////////////////////////////////////
/**
Column descriptor for SQLTables (Service.GetTables).
*/
class TableColumnDescriptor;

/**
Column descriptor for SQLGetTypeInfo (Service.GetTypeInfo).
*/
class TypeInfoColumnDescriptor;

/**
Column descriptor for SQLColumns (Service.GetColumns).
*/
class ColumnColumnDescriptor;

}

#endif
