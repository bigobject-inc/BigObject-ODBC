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

#ifndef _BIGOBJECT_ODBC_CLIENT_RESULTSET_
#define _BIGOBJECT_ODBC_CLIENT_RESULTSET_

#include <cstddef>

#include "Client.hpp"
#include "ColumnDescriptor.hpp"
#include "Row.hpp"

namespace ODBCDriver
{

/**
ResultSet interface definition.

Abstract base class for Hive resultsets. Does not provide any additional 
logic for subclasses, but defines the interface expected for all ResultSets.
All subclasses extending ResultSet need to at least implement the below 
methods.
*/
class ResultSet
{
public:
  
  /**
  Destructor.
  */
  virtual ~ResultSet() 
  {
  } 

  /**
  Fetch results.
  
  @param orientation Fetch orientation.
  @param offset Number of the row to fetch. The interpretation of this argument 
                depends on the value of the orientation argument.
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn Fetch(unsigned short orientation = SQL_FETCH_NEXT,
                             std::size_t offset = 0) = 0;

  /**
  Determines whether the it has ever had any result rows.

  @param hasResults Pointer to an int which will be set to 1 if results, 0 if 
                    no results
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn HasResults(bool* results) = 0;

  /**
  Determines the number of columns.

  @param colCount Pointer to a size_t which will be set to the number of columns
                  in the result.
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn GetColumnCount(std::size_t* colCount) = 0;

  /**
  Constructs a ColumnDescriptor with information about a specified column.

  @param columnIdx Zero offset index of a column.
  @param columnDescPtr A pointer to a ColumnDescriptor pointer which will 
                       receive the new ColumnDescriptor.
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn CreateColumnDescriptor(
   std::size_t columnIdx, ColumnDescriptor** columnDescPtr) = 0;

  /**
  Destroys a ColumnDescriptor object.

  @param columnDesc A ColumnDescriptor object to be removed from memory.
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn CloseColumnDescriptor(
   ColumnDescriptor* columnDescPtr) = 0;
  
  /* 
  Get current Row object.

  The row will ONLY be valid after Fetch has been called at least once.

  @param rowPtr A pointer to a Row pointer which will receive the current Row.
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn GetCurrentRow(Row** rowPtr) = 0;

  /**
  Determines the position of current row.

  @param rowPos Pointer to a size_t which will be set to the position of 
                current row.
  @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
  */
  virtual ServerReturn GetCurrentRowPos(std::size_t* rowPos) = 0;

};

///////////////////////////////////////////////////////////////
// Special resultsets forward Declaration.
///////////////////////////////////////////////////////////////

/**
ResultSet implementation for SQLTables.
*/
class TableResultSet;

/**
ResultSet implementation for SQLGetTypeInfo.
*/
class TypeInfoResultSet;

/**
ResultSet implementation for SQLColumns.
*/
class ColumnResultSet;

}

#endif
