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

#ifndef _BIGOBJECT_ODBC_H_DESCRIPTOR
#define _BIGOBJECT_ODBC_H_DESCRIPTOR

#include <boost/ptr_container/ptr_vector.hpp>

#include "../Driver.hpp"

/**********************************************
* Descriptors
*
* https://msdn.microsoft.com/en-us/library/ms716339%28v=vs.85%29.aspx
*
* See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/
*     htm/odbcsqlsetstmtattr.asp
*
* SQL_ATTR_PARAM_BIND_OFFSET_PTR   SQL_DESC_BIND_OFFSET_PTR        APD
* SQL_ATTR_PARAM_BIND_TYPE         SQL_DESC_BIND_TYPE              APD
* SQL_ATTR_PARAM_OPERATION_PTR     SQL_DESC_ARRAY_STATUS_PTR       APD
* SQL_ATTR_PARAMSET_SIZE           SQL_DESC_ARRAY_SIZE             APD
*
* SQL_ATTR_PARAM_STATUS_PTR        SQL_DESC_ARRAY_STATUS_PTR       IPD
* SQL_ATTR_PARAMS_PROCESSED_PTR    SQL_DESC_ROWS_PROCESSED_PTR     IPD
*
* SQL_ATTR_ROW_ARRAY_SIZE          SQL_DESC_ARRAY_SIZE             ARD
* SQL_ATTR_ROW_BIND_OFFSET_PTR     SQL_DESC_BIND_OFFSET_PTR        ARD
* SQL_ATTR_ROW_BIND_TYPE           SQL_DESC_BIND_TYPE              ARD
* SQL_ATTR_ROW_OPERATION_PTR       SQL_DESC_ARRAY_STATUS_PTR       ARD
*
* SQL_ATTR_ROW_STATUS_PTR          SQL_DESC_ARRAY_STATUS_PTR       IRD
* SQL_ATTR_ROWS_FETCHED_PTR        SQL_DESC_ROWS_PROCESSED_PTR     IRD
**********************************************/

// ----------------------- descriptor types/signatures ----------------------
#define SQL_DESC_ARD            1
#define SQL_DESC_IRD            2
#define SQL_DESC_APD            3
#define SQL_DESC_IPD            4

/**
APPL ROW DESC ITEM - to hold appl specified buffers & other details for one
column.
*/
typedef struct tARDItem
{
 SQLSMALLINT colNum; ///< Column number.

 SQLSMALLINT dataConciseType; ///< Concise data type.
 SQLSMALLINT dataVerboseType; ///< Basic data type.
 SQLSMALLINT dateTimeIntervalCode; ///< Date-time interval code
 SQLSMALLINT dateTimeIntervalPrec; ///< Date-time precision.
 SQLINTEGER numPrecRadix; ///< 2 if approx num type or 10 exact num type
 SQLSMALLINT scale; ///< Scale, right of decimal.

 SQLPOINTER dataPtr; ///< Col data.
 SQLINTEGER dataSize; ///< Size of data buffer.
 SQLINTEGER* sizePtr; ///< Actual data size.
 SQLINTEGER* sizeIndPtr; ///< Size indicator ptr
} ARDItem;

/// Type of list of ARDItems.
typedef boost::ptr_vector<ARDItem> ARDItemList;

/**
APPL ROW DESC - to hold appl specified buffers & other details for columns.
*/
typedef struct tODBCARD
{
 SQLSMALLINT sign;

 ////////////////////////////////////////////// 
 // Header fields.
 //////////////////////////////////////////////
 SQLSMALLINT allocType; ///< SQL_DESC_ALLOC_TYPE
 SQLUINTEGER rowArraySize; ///< SQL_DESC_ARRAY_SIZE
 SQLUSMALLINT* arrayStatusPtr; ///< SQL_DESC_ARRAY_STATUS_PTR
 SQLINTEGER* bindOffsetPtr; ///< SQL_DESC_BIND_OFFSET_PTR
 SQLINTEGER  bindTypeOrSize; ///< SQL_DESC_BIND_TYPE
 SQLSMALLINT descCount; ///< SQL_DESC_COUNT
 //////////////////////////////////////////////

 // Rows for binding each col.
 ARDItemList* bindCols; // Col bindings.

 SQLHSTMT hStmt; ///< Container statememt.
} ARD;

/**
ColumnHeader defined in DriverExtras in unixODBC implementation:
each element of the 1st row of results is one of these (except col 0) 
*/
typedef struct tCOLUMNHDR
{
 // EVERYTHING YOU WOULD EVER WANT TO KNOW ABOUT THE COLUMN. INIT THIS BY 
 // CALLING _NativeToSQLColumnHeader AS SOON AS YOU HAVE COLUMN INFO. 
 // THIS MAKES THE COL HDR LARGER	BUT GENERALIZES MORE CODE. 
 // see SQLColAttribute()

 int bSQL_DESC_AUTO_UNIQUE_VALUE; /* IS AUTO INCREMENT COL?					*/
 TCHAR *pszSQL_DESC_BASE_COLUMN_NAME; /* empty string if N/A						*/
 TCHAR *pszSQL_DESC_BASE_TABLE_NAME; /* empty string if N/A						*/
 int bSQL_DESC_CASE_SENSITIVE; /* IS CASE SENSITIVE COLUMN?				*/
 TCHAR *pszSQL_DESC_CATALOG_NAME; /* empty string if N/A						*/
 int nSQL_DESC_CONCISE_TYPE; /* ie SQL_CHAR, SQL_TYPE_TIME...			*/
 SQLULEN nSQL_DESC_DISPLAY_SIZE; /* max digits required to display			*/
 int bSQL_DESC_FIXED_PREC_SCALE; /* has data source specific precision?		*/
 TCHAR *pszSQL_DESC_LABEL; /* display label, col name or empty string	*/
 SQLULEN nSQL_DESC_LENGTH; /* strlen or bin size						*/
 TCHAR *pszSQL_DESC_LITERAL_PREFIX; /* empty string if N/A						*/
 TCHAR *pszSQL_DESC_LITERAL_SUFFIX; /* empty string if N/A						*/
 TCHAR *pszSQL_DESC_LOCAL_TYPE_NAME; /* empty string if N/A						*/
 TCHAR *pszSQL_DESC_NAME; /* col alias, col name or empty string		*/
 int nSQL_DESC_NULLABLE; /* SQL_NULLABLE, _NO_NULLS or _UNKNOWN		*/
 int nSQL_DESC_NUM_PREC_RADIX; /* 2, 10, or if N/A... 0 					*/
 SQLULEN nSQL_DESC_OCTET_LENGTH; /* max size									*/
 int nSQL_DESC_PRECISION; /*											*/
 int nSQL_DESC_SCALE; /*											*/
 TCHAR *pszSQL_DESC_SCHEMA_NAME; /* empty string if N/A						*/
 int nSQL_DESC_SEARCHABLE; /* can be in a filter ie SQL_PRED_NONE...	*/
 TCHAR *pszSQL_DESC_TABLE_NAME; /* empty string if N/A						*/
 int nSQL_DESC_TYPE; /* SQL data type ie SQL_CHAR, SQL_INTEGER..	*/
 TCHAR *pszSQL_DESC_TYPE_NAME; /* DBMS data type ie VARCHAR, MONEY...		*/
 int nSQL_DESC_UNNAMED; /* qualifier for SQL_DESC_NAME ie SQL_NAMED	*/
 int bSQL_DESC_UNSIGNED; /* if signed FALSE else TRUE				*/
 int nSQL_DESC_UPDATABLE; /* ie SQL_ATTR_READONLY, SQL_ATTR_WRITE...	*/

 /* Original code on unixODBC BINDING INFO is moved to ARDItem. */
 /* BINDING INFO		*/
 //SQLSMALLINT nTargetType; /* BIND: C DATA TYPE ie SQL_C_CHAR			*/
 //SQLPOINTER pTargetValue; /* BIND: POINTER FROM APPLICATION TO COPY TO*/
 //SQLLEN nTargetValueMax; /* BIND: MAX SPACE IN pTargetValue			*/
 //SQLLEN *pnLengthOrIndicator; /* BIND: TO RETURN LENGTH OR NULL INDICATOR	*/

} COLUMNHDR;

typedef COLUMNHDR IRDItem;

/// Type of list of IRDItems.
typedef boost::ptr_vector<IRDItem> IRDItemList;

/**
IMPL ROW DESC - to hold response col description from server.
*/
typedef struct tODBCIRD
{
 SQLSMALLINT sign;

 ////////////////////////////////////////////// 
 // Header fields.
 //////////////////////////////////////////////
 SQLUSMALLINT* arrayStatusPtr;  ///< SQL_DESC_ARRAY_STATUS_PTR
 SQLSMALLINT descCount; ///< SQL_DESC_COUNT
 SQLULEN* rowsProcessedPtr; ///< SQL_DESC_ROWS_PROCESSED_PTR
 //////////////////////////////////////////////

 // Rows for binding each col
 // (We follow unixODBC's implementation, that is defined in COLUMNHDR*).
 IRDItemList* rowDesc; ///< Row descriptor.

 SQLHSTMT hStmt; ///< Container statememt.
} IRD;

/**
APPL PARM DESC ITEM - to hold appl specified buffers & other details for
one param.
*/
typedef struct tAPDItem
{
 SQLSMALLINT paramNum; ///< Parameter number.

 SQLSMALLINT dataConciseType; ///< Data type.
 SQLSMALLINT dataVerboseType; ///< Basic data type.
 SQLSMALLINT dateTimeIntervalCode; ///< Date-time interval code ????
 SQLINTEGER dateTimeIntervalPrec; ///< Date-time precision ????
 SQLINTEGER numPrecRadix; ///< 2 if approx num type or 10 exact num type.
 SQLSMALLINT scale; ///< Scale, right of decimal.

 SQLPOINTER dataPtr; ///< Param data.
 SQLINTEGER dataSize; ///< Size of data.
 SQLINTEGER* sizePtr; ///< Size of data buffer.
 SQLINTEGER* sizeIndPtr; ///< Size indicator ptr.
} APDItem;

/// Type of list of APDItems.
typedef boost::ptr_vector<APDItem> APDItemList;

/**
APPL PARAM DESC - to hold appl specified buffers & other details for params.
*/
typedef struct tODBCAPD
{
 SQLSMALLINT sign;

 ////////////////////////////////////////////// 
 // Header fields.
 //////////////////////////////////////////////
 SQLSMALLINT allocType; ///< SQL_DESC_ALLOC_TYPE
 SQLUINTEGER  rowArraySize; ///< SQL_DESC_ARRAY_SIZE
 SQLUSMALLINT* arrayStatusPtr; ///< SQL_DESC_ARRAY_STATUS_PTR
 SQLINTEGER* bindOffsetPtr; ///< SQL_DESC_BIND_OFFSET_PTR
 SQLINTEGER bindTypeOrSize; ///< SQL_DESC_BIND_TYPE
 SQLSMALLINT descCount; ///< SQL_DESC_COUNT
 //////////////////////////////////////////////

 SQLUINTEGER paramSetSize;
 SQLINTEGER* paramProcessedPtr;

 // Rows for binding each col.
 APDItemList* bindParams; ///< Param bindings.

 SQLHSTMT hStmt; ///< Container statememt.
} APD;

/**
IMPL PARM DESC ITEM - to hold impl buffers & other details for one param.
*/
typedef struct tIPDItem
{
 SQLSMALLINT paramNum; ///< Parameter number.
 SQLSMALLINT paramType; ///< Parameter type input/output.
 SQLTCHAR paramName[33]; ///< Param name - arbitray/dummy.

 SQLSMALLINT dataConciseType; ///< Data type.
 SQLSMALLINT dataVerboseType; ///< Basic data type.
 SQLSMALLINT dateTimeIntervalCode; ///< Date-time interval code ????
 SQLINTEGER dateTimeIntervalPrec; ///< Date-time precision ????
 SQLINTEGER numPrecRadix; ///< 2 if approx num type or 10 exact num type.
 SQLSMALLINT scale; ///< Scale, right of decimal.

 SQLPOINTER dataPtr; ///< Param data.
 SQLINTEGER dataSize; ///< Size of data buffer.
 SQLSMALLINT fixedPrecScale; ///< Fixed precision scale.
 SQLSMALLINT nullable; ///< Is it nullable.
} IPDItem;

/// Type of list of IPDItems.
typedef boost::ptr_vector<IPDItem> IPDItemList;

/**
IMPL PARAM DESC - to hold impl buffers & other details for params.
*/
typedef struct tODBCIPD
{
 SQLSMALLINT sign;

 ////////////////////////////////////////////// 
 // Header fields.
 //////////////////////////////////////////////
 //  SQLSMALLINT allocType;     (read-only) ///< SQL_DESC_ALLOC_TYPE
 SQLUSMALLINT* arrayStatusPtr; ///< SQL_DESC_ARRAY_STATUS_PTR
 SQLSMALLINT descCount; ///< SQL_DESC_COUNT
 SQLUINTEGER* rowsProcessedPtr; ///< SQL_DESC_ROWS_PROCESSED_PTR
 ////////////////////////////////////////////// 

 // Rows for binding each col.
 IPDItemList* bindParams; ///< Param bindings.

 SQLHSTMT hStmt; ///< Container statememt.
} IPD;

////////////////////////////////////////////// 
// Functions
//////////////////////////////////////////////

void _SQLSetARDFieldsDefault(ARD* pDesc, const SQLHSTMT pStmt);

void _SQLSetIRDFieldsDefault(IRD* pDesc, const SQLHSTMT pStmt);

void _SQLSetAPDFieldsDefault(APD* pDesc, const SQLHSTMT pStmt);

void _SQLSetIPDFieldsDefault(IPD* pDesc, const SQLHSTMT pStmt);

bool _SQLSetARDItemFieldsDefault(ARDItem* pDescItem, SQLSMALLINT pRecNum = 0);

bool _SQLSetIRDItemFieldsDefault(IRDItem* pDescItem, SQLSMALLINT pRecNum = 0);

bool _SQLSetAPDItemFieldsDefault(APDItem* pDescItem, SQLSMALLINT pRecNum = 0);

bool _SQLSetIPDItemFieldsDefault(IPDItem* pDescItem, SQLSMALLINT pRecNum = 0);

RETCODE _SQLGetARDField(const ARD* pDesc, SQLSMALLINT pFldID,
                        SQLPOINTER pDataPtr,
                        SQLINTEGER pDataSize, SQLINTEGER* pDataSizePtr);

RETCODE _SQLGetIRDField(const IRD* pDesc, SQLSMALLINT pFldID,
                        SQLPOINTER pDataPtr,
                        SQLINTEGER pDataSize, SQLINTEGER* pDataSizePtr);

RETCODE _SQLGetAPDField(const APD* pDesc, SQLSMALLINT pFldID,
                        SQLPOINTER pDataPtr,
                        SQLINTEGER pDataSize, SQLINTEGER* pDataSizePtr);

RETCODE _SQLGetIPDField(const IPD* pDesc, SQLSMALLINT pFldID,
                        SQLPOINTER pDataPtr,
                        SQLINTEGER pDataSize, SQLINTEGER* pDataSizePtr);

RETCODE _SQLSetARDField(ARD* pDesc, SQLSMALLINT pFldID,
                        const SQLPOINTER pDataPtr, SQLINTEGER pDataSize);

RETCODE _SQLSetIRDField(IRD* pDesc, SQLSMALLINT pFldID,
                        const SQLPOINTER pDataPtr, SQLINTEGER pDataSize);

RETCODE _SQLSetAPDField(APD* pDesc, SQLSMALLINT pFldID,
                        const SQLPOINTER pDataPtr, SQLINTEGER pDataSize);

RETCODE _SQLSetIPDField(IPD* pDesc, SQLSMALLINT pFldID,
                        const SQLPOINTER pDataPtr, SQLINTEGER pDataSize);

RETCODE _SQLSetARDItemField(ARD* pDesc, ARDItem* pDescItem, 
                            SQLSMALLINT pRecNum, SQLSMALLINT pFldID,
                            const SQLPOINTER pDataPtr, SQLINTEGER pDataSize);

bool _SQLAttachARDItem(ARD* pDesc, ARDItem* pDescItem);

bool _SQLAttachIRDItem(IRD* pDesc, IRDItem* pDescItem);

bool _SQLAttachAPDItem(APD* pDesc, APDItem* pDescItem);

bool _SQLAttachIPDItem(IPD* pDesc, IPDItem* pDescItem);

bool _SQLDetachARDItem(ARD* pDesc, ARDItem* pDescItem);

bool _SQLDetachIRDItem(IRD* pDesc, IRDItem* pDescItem);

bool _SQLDetachAPDItem(APD* pDesc, APDItem* pDescItem);

bool _SQLDetachIPDItem(IPD* pDesc, IPDItem* pDescItem);

ARDItem* _SQLGetARDItem(const ARD* pDesc, SQLSMALLINT pRecNum);

IRDItem* _SQLGetIRDItem(const IRD* pDesc, SQLSMALLINT pRecNum);

APDItem* _SQLGetAPDItem(const APD* pDesc, SQLSMALLINT pRecNum);

IPDItem* _SQLGetIPDItem(const IPD* pDesc, SQLSMALLINT pRecNum);

RETCODE _SQLFreeARDContent(ARD* pDesc);

RETCODE _SQLFreeIRDContent(IRD* pDesc);

RETCODE _SQLFreeAPDContent(APD* pDesc);

RETCODE _SQLFreeIPDContent(IPD* pDesc);

//////////////////////////////////////////////

#endif
