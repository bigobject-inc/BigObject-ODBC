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

#ifndef _BIGOBJECT_ODBC_CLIENT_RESULTSET_DETAIL_
#define _BIGOBJECT_ODBC_CLIENT_RESULTSET_DETAIL_

#include <cstddef>

#include <boost/optional.hpp>

#include "../../Driver.hpp"

#include "../Client.hpp"
#include "../ResultSet.hpp"

#include "ColumnDescriptor.hpp"
#include "Row.hpp"

namespace ODBCDriver
{

/**
ResultSet implementation for SQLTables.
*/
class TableResultSet : public virtual ResultSet
{
public:

 /**
 Constructor.
 */
 TableResultSet() : ResultSet()
 {
 }

 /**
 Destructor.
 */
 virtual ~TableResultSet()
 {
 }

 ServerReturn GetColumnCount(std::size_t* colCount);

 ServerReturn CreateColumnDescriptor(size_t columnIdx, 
                                     ColumnDescriptor** columnDescPtr);

 ServerReturn CloseColumnDescriptor(ColumnDescriptor* columnDescPtr);

};

/**
ResultSet implementation for SQLGetTypeInfo.
*/
class TypeInfoResultSet : public virtual ResultSet
{
public:

 /**
 Constructor.
 */
 TypeInfoResultSet() : ResultSet()
 {
 }

 /**
 Destructor.
 */
 virtual ~TypeInfoResultSet()
 {
 }

 ServerReturn GetColumnCount(std::size_t* colCount);

 ServerReturn CreateColumnDescriptor(size_t columnIdx,
                                     ColumnDescriptor** columnDescPtr);

 ServerReturn CloseColumnDescriptor(ColumnDescriptor* columnDescPtr);

 virtual TypeInfoRow::StoragePtrType GetRow(
  SQLSMALLINT dType, boost::optional<SQLINTEGER> percRadix = boost::none,
  boost::optional<SQLSMALLINT> unsignedAtt = boost::none,
  boost::optional<tstring> literalPrefix = boost::none,
  boost::optional<tstring> literalSuffix = boost::none);

};

/**
ResultSet implementation for SQLColumns.
*/
class ColumnResultSet : public virtual ResultSet
{
public:

 /**
 Constructor.
 */
 ColumnResultSet() : ResultSet()
 {
 }

 /**
 Destructor.
 */
 virtual ~ColumnResultSet()
 {
 }

 ServerReturn GetColumnCount(std::size_t* colCount);

 ServerReturn CreateColumnDescriptor(size_t columnIdx,
                                     ColumnDescriptor** columnDescPtr);

 ServerReturn CloseColumnDescriptor(ColumnDescriptor* columnDescPtr);

};

}

#endif
