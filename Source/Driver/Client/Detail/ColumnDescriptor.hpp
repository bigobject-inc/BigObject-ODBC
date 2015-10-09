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

#ifndef _BIGOBJECT_ODBC_CLIENT_COLUMN_DESCRIPTOR_DETAIL_
#define _BIGOBJECT_ODBC_CLIENT_COLUMN_DESCRIPTOR_DETAIL_

#include "../Client.hpp"
#include "../Exception.hpp"
#include "../ColumnDescriptor.hpp"

namespace ODBCDriver
{

/**
Column descriptor for SQLTables (Service.GetTables).
*/
class TableColumnDescriptor : public ColumnDescriptor
{
public:

 /**
 Constructor.

 @param _columnIdx Column index, it must be 0 ~ 4.
 */
 TableColumnDescriptor(const std::size_t _columnIdx) 
  : ColumnDescriptor(), columnIdx(_columnIdx), 
    columnType(TEXT("VARCHAR")), serverType(SERVER_STRING_TYPE)
 {
  if(columnIdx >= 5)
   throw ClientException("Illegal argument for TableColumnDescriptor.");
 }

 /**
 Destructor.
 */
 virtual ~TableColumnDescriptor()
 {
 }

 tstring GetColumnName();

 tstring GetColumnType();

 ServerType GetServerType();

 bool GetIsNullable();

 bool GetIsCaseSensitive();

protected:

 /// Column index.
 const std::size_t columnIdx;

 /// Fixed column type.
 const tstring columnType;

 /// Fixed server type.
 const ServerType serverType;

};

/**
Column descriptor for SQLGetTypeInfo (Service.GetTypeInfo).
*/
class TypeInfoColumnDescriptor : public ColumnDescriptor
{
public:

 /**
 Constructor.

 @param _columnIdx Column index, it must be 0 ~ 18.
 */
 TypeInfoColumnDescriptor(const std::size_t _columnIdx)
  : ColumnDescriptor(), columnIdx(_columnIdx)
 {
  if(columnIdx >= 19)
   throw ClientException("Illegal argument for TypeInfoColumnDescriptor.");
 }

 /**
 Destructor.
 */
 virtual ~TypeInfoColumnDescriptor()
 {
 }

 tstring GetColumnName();

 tstring GetColumnType();

 ServerType GetServerType();

 bool GetIsNullable();

 bool GetIsCaseSensitive();

protected:

 /// Column index.
 const std::size_t columnIdx;

};

/**
Column descriptor for SQLColumns (Service.GetColumns).
*/
class ColumnColumnDescriptor : public ColumnDescriptor
{
public:

 /**
 Constructor.

 @param _columnIdx Column index, it must be 0 ~ 17.
 */
 ColumnColumnDescriptor(const std::size_t _columnIdx)
  : ColumnDescriptor(), columnIdx(_columnIdx)
 {
  if(columnIdx >= 18)
   throw ClientException("Illegal argument for ColumnColumnDescriptor.");
 }

 /**
 Destructor.
 */
 virtual ~ColumnColumnDescriptor()
 {
 }

 tstring GetColumnName();

 tstring GetColumnType();

 ServerType GetServerType();

 bool GetIsNullable();

 bool GetIsCaseSensitive();

protected:

 /// Column index.
 const std::size_t columnIdx;

};

}

#endif
