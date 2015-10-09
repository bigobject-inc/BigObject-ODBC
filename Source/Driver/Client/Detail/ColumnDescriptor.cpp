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


#include "../../Driver.hpp"

#include "ColumnDescriptor.hpp"

namespace ODBCDriver
{

std::size_t ColumnDescriptor::GetMaxDisplaySize(ServerType serverType)
{
 /*
 These are more or less arbitrarily determined values and may be changed if
 it causes any problems.
 */
 switch(serverType)
 {
  case SERVER_VOID_TYPE:
   return MAX_DISPLAY_SIZE;

  case SERVER_BOOLEAN_TYPE:
   return 5; //1; // False is 5 chars long

  case SERVER_TINYINT_TYPE:
   //return 4;

  case SERVER_SMALLINT_TYPE:
   //return 6;

  case SERVER_INT_TYPE:
   //return 11;

  case SERVER_BIGINT_TYPE:
   //return 20;

  case SERVER_DECIMAL_TYPE:
   return 32;

  case SERVER_FLOAT_TYPE:
   return 16;

  case SERVER_DOUBLE_TYPE:
   return 24;

  case SERVER_STRING_TYPE:
   return MAX_DISPLAY_SIZE;

  case SERVER_DATE_TYPE:
   return 10; //MAX_DISPLAY_SIZE;

  case SERVER_TIME_TYPE:
   return 12;

  case SERVER_DATETIME_TYPE:
   return 19; //MAX_DISPLAY_SIZE;

  case SERVER_TIMESTAMP_TYPE:
   return MAX_DISPLAY_SIZE;

  case SERVER_LIST_TYPE:
   return MAX_DISPLAY_SIZE;

  case SERVER_MAP_TYPE:
   return MAX_DISPLAY_SIZE;

  case SERVER_STRUCT_TYPE:
   return MAX_DISPLAY_SIZE;

  case SERVER_UNKNOWN_TYPE:
   return MAX_DISPLAY_SIZE;

  default:
   return MAX_DISPLAY_SIZE;
 }
}

std::size_t ColumnDescriptor::GetFieldByteSize(ServerType serverType)
{
 /*
 These are more or less arbitrarily determined values and may be changed if
 it causes any problems.
 */
 switch(serverType)
 {
  case SERVER_VOID_TYPE:
   return MAX_BYTE_LENGTH;

  case SERVER_BOOLEAN_TYPE:
   return sizeof(char);

  case SERVER_TINYINT_TYPE:
   return sizeof(char);

  case SERVER_SMALLINT_TYPE:
   return sizeof(SQLSMALLINT);

  case SERVER_INT_TYPE:
   return sizeof(SQLINTEGER);;

  case SERVER_BIGINT_TYPE:
   return sizeof(SQLBIGINT);;

  case SERVER_DECIMAL_TYPE:
   return sizeof(SQLDECIMAL);

  case SERVER_FLOAT_TYPE:
   return sizeof(SQLFLOAT);

  case SERVER_DOUBLE_TYPE:
   return sizeof(SQLDOUBLE);

  case SERVER_STRING_TYPE:
   return MAX_BYTE_LENGTH;

  case SERVER_DATE_TYPE:
   return SQL_DATE_LEN; //MAX_BYTE_LENGTH;

  case SERVER_TIME_TYPE:
   return SQL_TIME_LEN;

  case SERVER_DATETIME_TYPE:
   return SQL_TIMESTAMP_LEN; // MAX_BYTE_LENGTH;

  case SERVER_TIMESTAMP_TYPE:
   return SQL_TIMESTAMP_LEN; //MAX_BYTE_LENGTH;

  case SERVER_LIST_TYPE:
   return MAX_BYTE_LENGTH;

  case SERVER_MAP_TYPE:
   return MAX_BYTE_LENGTH;

  case SERVER_STRUCT_TYPE:
   return MAX_BYTE_LENGTH;

  case SERVER_UNKNOWN_TYPE:
   return MAX_BYTE_LENGTH;

  default:
   return MAX_BYTE_LENGTH;
 }
}

int ColumnDescriptor::GetSQLType(ServerType serverType)
{
 switch(serverType)
 {
  case SERVER_VOID_TYPE:
   return SQL_UNKNOWN_TYPE;

  case SERVER_BOOLEAN_TYPE:
   return SQL_BIT;

  case SERVER_TINYINT_TYPE:
   return SQL_TINYINT;

  case SERVER_SMALLINT_TYPE:
   return SQL_SMALLINT;

  case SERVER_INT_TYPE:
   return SQL_INTEGER;

  case SERVER_BIGINT_TYPE:
   return SQL_BIGINT;

  case SERVER_DECIMAL_TYPE:
   return SQL_DECIMAL;

  case SERVER_FLOAT_TYPE:
   return SQL_FLOAT;

  case SERVER_DOUBLE_TYPE:
   return SQL_DOUBLE;

  case SERVER_STRING_TYPE:
   return SQL_VARCHAR;

  case SERVER_DATE_TYPE:
   return SQL_TYPE_DATE;

  case SERVER_TIME_TYPE:
   return SQL_TYPE_TIME;

  case SERVER_DATETIME_TYPE:
   return SQL_TYPE_TIMESTAMP;

  case SERVER_TIMESTAMP_TYPE:
   return SQL_TYPE_TIMESTAMP;

  case SERVER_LIST_TYPE:
   return SQL_UNKNOWN_TYPE;

  case SERVER_MAP_TYPE:
   return SQL_UNKNOWN_TYPE;

  case SERVER_STRUCT_TYPE:
   return SQL_UNKNOWN_TYPE;

  case SERVER_UNKNOWN_TYPE:
   return SQL_UNKNOWN_TYPE;

  default:
   return SQL_UNKNOWN_TYPE;
 }
}

tstring ColumnDescriptor::GetSQLTypeName(SQLSMALLINT dType)
{
 switch(dType)
 {
  case SQL_CHAR:
   return TEXT("CHAR");

  case SQL_VARCHAR:
   return TEXT("VARCHAR");

  case SQL_LONGVARCHAR:
   return TEXT("TEXT"); // TEXT("LONG VARCHAR"); // NOT SQL92

  case SQL_WCHAR:
   return TEXT("CHAR"); // TEXT("WCHAR");

  case SQL_WVARCHAR:
   return TEXT("VARCHAR"); // TEXT("VARWCHAR"); 

  case SQL_WLONGVARCHAR:
   return TEXT("TEXT"); // TEXT("LONGWVARCHAR");  // NOT SQL92

  case SQL_DECIMAL:
   return TEXT("DECIMAL");

  case SQL_NUMERIC:
   return TEXT("NUMERIC");

  case SQL_SMALLINT:
   return TEXT("SMALLINT");

  case SQL_INTEGER:
   return TEXT("INTEGER");

  case SQL_REAL:
   return TEXT("REAL");

  case SQL_FLOAT:
   return TEXT("FLOAT");

  case SQL_DOUBLE:
   return TEXT("DOUBLE PRECISION");

  case SQL_BIT:
   return TEXT("BIT");

  case SQL_TINYINT:
   return TEXT("TINYINT"); // NOT SQL92

  case SQL_BIGINT:
   return TEXT("BIGINT"); // NOT SQL92

  case SQL_BINARY:
   return TEXT("BINARY"); // NOT SQL92

  case SQL_VARBINARY:
   return TEXT("VARBINARY"); // NOT SQL92

  case SQL_LONGVARBINARY:
   return TEXT("BLOB"); // TEXT("LONG VARBINARY"); // NOT SQL92

  case SQL_TYPE_DATE:
   return TEXT("DATE");

  case SQL_TYPE_TIME:
   return TEXT("TIME");

  case SQL_TYPE_TIMESTAMP:
   return TEXT("TIMESTAMP");

   //case SQL_TYPE_UTCDATETIME:
   // return TEXT("UTCDATETIME");

   //case SQL_TYPE_UTCTIME:
   // return TEXT("UTCTIME");

  case SQL_INTERVAL_MONTH:
   return TEXT("INTERVAL"); // TEXT("INTERVAL MONTH");

  case SQL_INTERVAL_YEAR:
   return TEXT("INTERVAL"); // TEXT("INTERVAL YEAR");

  case SQL_INTERVAL_YEAR_TO_MONTH:
   return TEXT("INTERVAL"); // TEXT("INTERVAL YEAR() TO MONTH");

  case SQL_INTERVAL_DAY:
   return TEXT("INTERVAL"); // TEXT("INTERVAL DAY()");

  case SQL_INTERVAL_HOUR:
   return TEXT("INTERVAL"); // TEXT("INTERVAL HOUR()");

  case SQL_INTERVAL_MINUTE:
   return TEXT("INTERVAL"); // TEXT("INTERVAL MINUTE()");

  case SQL_INTERVAL_SECOND:
   return TEXT("INTERVAL"); // TEXT("INTERVAL SECOND()");

  case SQL_INTERVAL_DAY_TO_HOUR:
   return TEXT("INTERVAL"); // TEXT("INTERVAL DAY() TO HOUR");

  case SQL_INTERVAL_DAY_TO_MINUTE:
   return TEXT("INTERVAL"); // TEXT("INTERVAL DAY() TO MINUTE");

  case SQL_INTERVAL_DAY_TO_SECOND:
   return TEXT("INTERVAL"); // TEXT("INTERVAL DAY() TO SECOND(q)");

  case SQL_INTERVAL_HOUR_TO_MINUTE:
   return TEXT("INTERVAL"); // TEXT("INTERVAL HOUR() TO MINUTE");

  case SQL_INTERVAL_HOUR_TO_SECOND:
   return TEXT("INTERVAL"); // TEXT("INTERVAL HOUR() TO SECOND()");

  case SQL_INTERVAL_MINUTE_TO_SECOND:
   return TEXT("INTERVAL"); // TEXT("INTERVAL MINUTE() TO SECOND()");

  case SQL_GUID:
   return TEXT("GUID"); // NOT SQL92

  default:
   throw ClientException("Illegal argument for GetSQLTypeName.");
 }
}

ServerType ColumnDescriptor::GetServerType(SQLSMALLINT dType)
{
 // Reference: (Mappings between jdbc and sql92.)
 // https://java.net/projects/glassfish/sources/svn/content/trunk/main/
 // appserver/persistence/cmp/generator-database/src/main/java/com/sun/jdo/
 // spi/persistence/generator/database/SQL92.properties?rev=63889

 switch(dType)
 {
  case SQL_CHAR:
  case SQL_VARCHAR:
  case SQL_LONGVARCHAR:
  case SQL_WCHAR:
  case SQL_WVARCHAR:
  case SQL_WLONGVARCHAR:
  case SQL_BINARY:
  case SQL_VARBINARY:
  case SQL_LONGVARBINARY:
  case SQL_GUID:
   return SERVER_STRING_TYPE;

  case SQL_DECIMAL:
   return SERVER_DECIMAL_TYPE;

  case SQL_NUMERIC:
   return SERVER_BIGINT_TYPE;

  case SQL_SMALLINT:
   return SERVER_SMALLINT_TYPE;

  case SQL_INTEGER:
   return SERVER_INT_TYPE;

  case SQL_REAL:
  case SQL_FLOAT:
   return SERVER_FLOAT_TYPE;

  case SQL_DOUBLE:
   return SERVER_DOUBLE_TYPE;

  case SQL_BIT:
   return SERVER_BOOLEAN_TYPE;

  case SQL_TINYINT:
   return SERVER_TINYINT_TYPE; 

  case SQL_BIGINT:
   return SERVER_BIGINT_TYPE; 

  case SQL_TYPE_DATE:
   return SERVER_DATE_TYPE;

  case SQL_TYPE_TIME:
   return SERVER_TIME_TYPE;

  case SQL_TYPE_TIMESTAMP:
   return SERVER_TIMESTAMP_TYPE;

  case SQL_INTERVAL_MONTH:
  case SQL_INTERVAL_YEAR:
  case SQL_INTERVAL_YEAR_TO_MONTH:
  case SQL_INTERVAL_DAY:
  case SQL_INTERVAL_HOUR:
  case SQL_INTERVAL_MINUTE:
  case SQL_INTERVAL_SECOND:
  case SQL_INTERVAL_DAY_TO_HOUR:
  case SQL_INTERVAL_DAY_TO_MINUTE:
  case SQL_INTERVAL_DAY_TO_SECOND:
  case SQL_INTERVAL_HOUR_TO_MINUTE:
  case SQL_INTERVAL_HOUR_TO_SECOND:
  case SQL_INTERVAL_MINUTE_TO_SECOND:
   return SERVER_UNKNOWN_TYPE;   

  default:
   return SERVER_UNKNOWN_TYPE;
 }
}

tstring TableColumnDescriptor::GetColumnName()
{
 switch(columnIdx)
 {
  case 0:
   return TEXT("TABLE_CAT");
  case 1:
   return TEXT("TABLE_SCHEM");
  case 2:
   return TEXT("TABLE_NAME");
  case 3:
   return TEXT("TABLE_TYPE");
  case 4:
   return TEXT("REMARKS");

  default:
   throw ClientException("Illegal state for TableColumnDescriptor.");
 };
}

tstring TableColumnDescriptor::GetColumnType()
{
 return columnType;
}

ServerType TableColumnDescriptor::GetServerType()
{
 return serverType;
}

bool TableColumnDescriptor::GetIsNullable()
{
 return true;
}

bool TableColumnDescriptor::GetIsCaseSensitive()
{
 return true;
}

tstring TypeInfoColumnDescriptor::GetColumnName()
{
 switch(columnIdx)
 {
  case 0:
   return TEXT("TYPE_NAME");
  case 1:
   return TEXT("DATA_TYPE");
  case 2:
   return TEXT("COLUMN_SIZE");
  case 3:
   return TEXT("LITERAL_PREFIX");
  case 4:
   return TEXT("LITERAL_SUFFIX");
  case 5:
   return TEXT("CREATE_PARAMS");
  case 6:
   return TEXT("NULLABLE");
  case 7:
   return TEXT("CASE_SENSITIVE");
  case 8:
   return TEXT("SEARCHABLE");
  case 9:
   return TEXT("UNSIGNED_ATTRIBUTE");
  case 10:
   return TEXT("FIXED_PREC_SCALE");
  case 11:
   return TEXT("AUTO_UNIQUE_VALUE");
  case 12:
   return TEXT("LOCAL_TYPE_NAME");
  case 13:
   return TEXT("MINIMUM_SCALE");
  case 14:
   return TEXT("MAXIMUM_SCALE");
  case 15:
   return TEXT("SQL_DATA_TYPE");
  case 16:
   return TEXT("SQL_DATETIME_SUB");
  case 17:
   return TEXT("NUM_PREC_RADIX");
  case 18:
   return TEXT("INTERVAL_PRECISION");

  default:
   throw ClientException("Illegal state for TypeInfoColumnDescriptor.");
 };
}

tstring TypeInfoColumnDescriptor::GetColumnType()
{
 return GetSQLTypeName();
}

ServerType TypeInfoColumnDescriptor::GetServerType()
{
 switch(columnIdx)
 {
  case 0:
   return SERVER_STRING_TYPE; // TYPE_NAME
  case 1:
   return SERVER_SMALLINT_TYPE; // DATA_TYPE
  case 2:
   return SERVER_INT_TYPE; // COLUMN_SIZE
  case 3:
   return SERVER_STRING_TYPE; // LITERAL_PREFIX
  case 4:
   return SERVER_STRING_TYPE; // LITERAL_SUFFIX
  case 5:
   return SERVER_STRING_TYPE; // CREATE_PARAMS
  case 6:
   return SERVER_SMALLINT_TYPE; // NULLABLE
  case 7:
   return SERVER_SMALLINT_TYPE; // CASE_SENSITIVE
  case 8:
   return SERVER_SMALLINT_TYPE; // SEARCHABLE
  case 9:
   return SERVER_SMALLINT_TYPE; // UNSIGNED_ATTRIBUTE
  case 10:
   return SERVER_SMALLINT_TYPE; // FIXED_PREC_SCALE
  case 11:
   return SERVER_SMALLINT_TYPE; // AUTO_UNIQUE_VALUE
  case 12:
   return SERVER_STRING_TYPE; // LOCAL_TYPE_NAME
  case 13:
   return SERVER_SMALLINT_TYPE; // MINIMUM_SCALE
  case 14:
   return SERVER_SMALLINT_TYPE; // MAXIMUM_SCALE
  case 15:
   return SERVER_SMALLINT_TYPE; // SQL_DATA_TYPE
  case 16:
   return SERVER_SMALLINT_TYPE; // SQL_DATETIME_SUB
  case 17:
   return SERVER_INT_TYPE; // NUM_PREC_RADIX
  case 18:
   return SERVER_SMALLINT_TYPE; // INTERVAL_PRECISION

  default:
   throw ClientException("Illegal state for TypeInfoColumnDescriptor.");
 };
}

bool TypeInfoColumnDescriptor::GetIsNullable()
{
 return true;
}

bool TypeInfoColumnDescriptor::GetIsCaseSensitive()
{
 return true;
}

tstring ColumnColumnDescriptor::GetColumnName()
{
 switch(columnIdx)
 {
  case 0:
   return TEXT("TABLE_CAT");
  case 1:
   return TEXT("TABLE_SCHEM");
  case 2:
   return TEXT("TABLE_NAME");
  case 3:
   return TEXT("COLUMN_NAME");
  case 4:
   return TEXT("DATA_TYPE");
  case 5:
   return TEXT("TYPE_NAME");
  case 6:
   return TEXT("COLUMN_SIZE");
  case 7:
   return TEXT("BUFFER_LENGTH");
  case 8:
   return TEXT("DECIMAL_DIGITS");
  case 9:
   return TEXT("NUM_PREC_RADIX");
  case 10:
   return TEXT("NULLABLE");
  case 11:
   return TEXT("REMARKS");
  case 12:
   return TEXT("COLUMN_DEF");
  case 13:
   return TEXT("SQL_DATA_TYPE");
  case 14:
   return TEXT("SQL_DATETIME_SUB");
  case 15:
   return TEXT("CHAR_OCTET_LENGTH");
  case 16:
   return TEXT("ORDINAL_POSITION");
  case 17:
   return TEXT("IS_NULLABLE");

  default:
   throw ClientException("Illegal state for ColumnColumnDescriptor.");
 };
}

tstring ColumnColumnDescriptor::GetColumnType()
{
 return GetSQLTypeName();
}

ServerType ColumnColumnDescriptor::GetServerType()
{
 switch(columnIdx)
 {
  case 0:
   return SERVER_STRING_TYPE; // TABLE_CAT
  case 1:
   return SERVER_STRING_TYPE; // TABLE_SCHEM
  case 2:
   return SERVER_STRING_TYPE; // TABLE_NAME
  case 3:
   return SERVER_STRING_TYPE; // COLUMN_NAME
  case 4:
   return SERVER_SMALLINT_TYPE; // DATA_TYPE
  case 5:
   return SERVER_STRING_TYPE; // TYPE_NAME
  case 6:
   return SERVER_INT_TYPE; // COLUMN_SIZE
  case 7:
   return SERVER_INT_TYPE; // BUFFER_LENGTH
  case 8:
   return SERVER_SMALLINT_TYPE; // DECIMAL_DIGITS
  case 9:
   return SERVER_SMALLINT_TYPE; // NUM_PREC_RADIX
  case 10:
   return SERVER_SMALLINT_TYPE; // NULLABLE
  case 11:
   return SERVER_STRING_TYPE; // REMARKS
  case 12:
   return SERVER_STRING_TYPE; // COLUMN_DEF
  case 13:
   return SERVER_SMALLINT_TYPE; // SQL_DATA_TYPE
  case 14:
   return SERVER_SMALLINT_TYPE; // SQL_DATETIME_SUB
  case 15:
   return SERVER_SMALLINT_TYPE; // CHAR_OCTET_LENGTH
  case 16:
   return SERVER_SMALLINT_TYPE; // ORDINAL_POSITION
  case 17:
   return SERVER_STRING_TYPE; // IS_NULLABLE

  default:
   throw ClientException("Illegal state for ColumnColumnDescriptor.");
 };
}

bool ColumnColumnDescriptor::GetIsNullable()
{
 return true;
}

bool ColumnColumnDescriptor::GetIsCaseSensitive()
{
 return true;
}

}
