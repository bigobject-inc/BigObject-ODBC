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

#ifndef _BIGOBJECT_ODBC_CLIENT_ROW_
#define _BIGOBJECT_ODBC_CLIENT_ROW_

#include <stdint.h>

#include <string>

#include "../../Base/TString.hpp"

#include "Client.hpp"

namespace ODBCDriver
{

/**
Row interface definition.

Abstract base class for rows. Provides the logic to extract fields as 
various data types, allowing subclasses to focus on the storage and field 
parsing of the data.

@see Detail/Row.hpp for predefined rows.
*/
class Row
{
public:

 /**
 Destructor.
 */
 virtual ~Row()
 {
 }


 /**
 Is column NULL value?

 @param columnIdx Column index.
 @param isNull Pointer to an int which will be set to true if the field
               contains a NULL value, or false otherwise.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 virtual ServerReturn IsNull(std::size_t columnIdx, bool* isNull) = 0;

 /**
 Find the size of a field as a string.
 
 @param columnIdx Zero offset index of a column.
 @param colLen Pointer to an size_t which will be set to the byte length of 
               the specified field.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 virtual ServerReturn GetFieldDataLen(std::size_t columnIdx, 
                                      std::size_t* colLen) = 0;

 virtual ServerReturn GetFieldAsString(std::size_t columnIdx, tstring& value, 
                                       bool* isNull) = 0;

 virtual ServerReturn GetFieldAsDouble(std::size_t columnIdx, double* buffer, 
                                       bool* isNull) = 0;

 virtual ServerReturn GetFieldAsInt(std::size_t columnIdx, int* buffer, 
                                    bool* isNull) = 0;

 virtual ServerReturn GetFieldAsLong(std::size_t columnIdx, long* buffer, 
                                     bool* isNull) = 0;

 virtual ServerReturn GetFieldAsULong(std::size_t columnIdx, 
                                      unsigned long* buffer, bool* isNull) = 0;

 virtual ServerReturn GetFieldAsI64(std::size_t columnIdx, int64_t* buffer, 
                                    bool* isNull) = 0;

 virtual ServerReturn GetFieldAsI64U(std::size_t columnIdx, uint64_t* buffer, 
                                     bool* isNull) = 0;

 virtual ServerReturn GetFieldAsDate(std::size_t columnIdx,
                                     struct tagDATE_STRUCT* buffer,
                                     bool* isNull) = 0;

 virtual ServerReturn GetFieldAsTime(std::size_t columnIdx,
                                     struct tagTIME_STRUCT* buffer,
                                     bool* isNull) = 0;

 virtual ServerReturn GetFieldAsTimestamp(std::size_t columnIdx, 
                                          struct tagTIMESTAMP_STRUCT* buffer,
                                          bool* isNull) = 0;

};

///////////////////////////////////////////////////////////////
// Special row forward Declaration.
///////////////////////////////////////////////////////////////

/**
Row for SQLTables implementation.
*/
class TableRow;

/**
Row for SQLGetTypeInfo implementation.
*/
class TypeInfoRow;

/**
Row for SQLColumns implementation.
*/
class ColumnRow;

}

#endif
