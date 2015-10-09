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

#include <algorithm>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <boost/type_traits.hpp>

#include <boost/typeof/typeof.hpp>

#include "Driver.hpp"

/**
Set default values of ARD header.

@param pDesc ARD*.
@param pStmt HDRVSTMT.
*/
void _SQLSetARDFieldsDefault(ARD* pDesc, const SQLHSTMT pStmt)
{
 // reset all
 memset(pDesc, 0, sizeof(ARD));
 // set explicit defaults
 pDesc->sign = SQL_DESC_ARD;
 pDesc->hStmt = (SQLHSTMT)pStmt;
 pDesc->allocType = SQL_DESC_ALLOC_AUTO;
 pDesc->rowArraySize = 1;
 pDesc->bindTypeOrSize = SQL_BIND_BY_COLUMN;
}

/**
Set default values of IRD header.

@param pDesc IRD*.
@param pStmt HDRVSTMT.
*/
void _SQLSetIRDFieldsDefault(IRD* pDesc, const SQLHSTMT pStmt)
{
 // reset all
 memset(pDesc, 0, sizeof(IRD));
 // set explicit defaults
 pDesc->sign = SQL_DESC_IRD;
 pDesc->hStmt = (SQLHSTMT)pStmt;
}

/**
Set default values of APD header.

@param pDesc APD*.
@param pStmt HDRVSTMT.
*/
void _SQLSetAPDFieldsDefault(APD* pDesc, const SQLHSTMT pStmt)
{
 // reset all
 memset(pDesc, 0, sizeof(APD));
 // set explicit defaults
 pDesc->sign = SQL_DESC_APD;
 pDesc->hStmt = (SQLHSTMT)pStmt;
 pDesc->allocType = SQL_DESC_ALLOC_AUTO;
 pDesc->rowArraySize = 1;
 pDesc->bindTypeOrSize = SQL_BIND_BY_COLUMN;
}

/**
Set default values of IPD header.

@param pDesc IPD*.
@param pStmt HDRVSTMT.
*/
void _SQLSetIPDFieldsDefault(IPD* pDesc, const SQLHSTMT pStmt)
{
 // reset all
 memset(pDesc, 0, sizeof(IPD));
 // set explicit defaults
 pDesc->sign = SQL_DESC_IPD;
 pDesc->hStmt = (SQLHSTMT)pStmt;
}

/**
Set default values of ARD item.

@param pDesc ARDItem*.
@param pRecNum Record number (Descriptor records are numbered from 1).
@return True if succeed otherwise false.
*/
bool _SQLSetARDItemFieldsDefault(ARDItem* pDescItem, SQLSMALLINT pRecNum) 
{
 // reset
 memset(pDescItem, 0, sizeof(ARDItem));
 // set explicit defaults
 pDescItem->colNum = pRecNum;
 pDescItem->dataConciseType = SQL_C_DEFAULT;
 pDescItem->dataVerboseType = SQL_C_DEFAULT;
 return true;
}

/**
Set default values of IRD item.

@param pDesc IRDItem*.
@param pRecNum Record number (Descriptor records are numbered from 1).
@return True if succeed otherwise false.
*/
bool _SQLSetIRDItemFieldsDefault(IRDItem* pDescItem, SQLSMALLINT pRecNum)
{
 return true;
}

/**
Set default values of APD item.

@param pDesc APDItem*.
@param pRecNum Record number (Descriptor records are numbered from 1).
@return True if succeed otherwise false.
*/
bool _SQLSetAPDItemFieldsDefault(APDItem* pDescItem, SQLSMALLINT pRecNum)
{
 // reset all
 memset(pDescItem, 0, sizeof(APDItem));
 // set explicit defaults
 pDescItem->dataConciseType = SQL_C_DEFAULT;
 pDescItem->dataVerboseType = SQL_C_DEFAULT;
 return true;
}

/**
Set default values of IPD item.

@param pDesc IPDItem*.
@param pRecNum Record number (Descriptor records are numbered from 1).
@return True if succeed otherwise false.
*/
bool _SQLSetIPDItemFieldsDefault(IPDItem* pDescItem, SQLSMALLINT pRecNum)
{
 // reset all
 memset(pDescItem, 0, sizeof(IPDItem));
 // set explicit defaults
 pDescItem->paramType = SQL_PARAM_INPUT;
 pDescItem->dataConciseType = SQL_DEFAULT;
 pDescItem->dataVerboseType = SQL_DEFAULT;
 return true;
}

// http://nealabq.com/blog/2008/09/25/member_object_pointer_traits/

// Declare member_object_pointer_traits but do not
// define a body. The struct will only ever be
// instantiated through specialization.
template<typename DATA_MEMBER_PTR>
struct MemberObjectPointerTraits;

// Specialized member_object_pointer_traits that
// only matches member-object pointers.
template<typename T, typename C>
struct MemberObjectPointerTraits<T(C::*)>
{
 typedef C ClassType;
 typedef T DataType;
};

/**
Attach a new item to descriptor item list.

@param pDesc Pointer to Descriptor.
@param pDescItem Pointer to descriptor item.
@param ptrToItemList Member pointer to descriptor item list.
@return True if succeed otherwise false.
*/
template<typename T, typename I, typename P>
static bool _AttachItem(T* pDesc, I* pDescItem, P ptrToItemList)
{
 // note
 // this function also helps in maintaining highest desc number

 // precaution
 if(pDesc == NULL || pDescItem == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return false;
 }

 if((pDesc->*ptrToItemList) == NULL)
 {
  typedef typename boost::remove_pointer<
   typename MemberObjectPointerTraits<P>::DataType
  >::type PT;

  (pDesc->*ptrToItemList) = new PT;
 }

 (pDesc->*ptrToItemList)->push_back(pDescItem);

 return true;
}

template<typename T, typename I, typename P>
static void _DetachItemDescCount(T* pDesc, I* pDescItem, P ptrToItemList,
                                 int ptrToCount)
{
}

template<typename T, typename I, typename P, typename P2>
static void _DetachItemDescCount(T* pDesc, I* pDescItem, P ptrToItemList,
                                 P2 ptrToCount)
{
 // maintain highest desc number
 if(pDesc->descCount == pDescItem->*ptrToCount)
 {
  // loop to find the highest number and set that
  I& i = *std::max_element(
   (pDesc->*ptrToItemList)->begin(), (pDesc->*ptrToItemList)->end(),
   boost::lambda::bind(ptrToCount, boost::lambda::_2) >
   boost::lambda::bind(ptrToCount, boost::lambda::_1));

  // set the highest count to this column
  pDesc->descCount = i.*ptrToCount;
 }
}

/**
Detach an existing item from descriptor item list.

@param pDesc Pointer to Descriptor.
@param pDescItem Pointer to descriptor item.
@param ptrToItemList Member pointer to descriptor item list.
@param ptrToCount Member pointer to item's member for descriptor's descCount.
@return True if succeed otherwise false.
*/
template<typename T, typename I, typename P, typename P2>
static bool _DetachItem(T* pDesc, I* pDescItem, P ptrToItemList, 
                        P2 ptrToCount)
{
 // note
 // this function also helps in maintaining highest desc number

 // precaution
 if(pDesc == NULL || pDescItem == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return false;
 }

 (pDesc->*ptrToItemList)->erase_if(pDescItem == &boost::lambda::_1);

 _DetachItemDescCount(pDesc, pDescItem, ptrToItemList, ptrToCount);

 return true;
}

template<typename T, typename I, typename P>
static bool _DetachItem(T* pDesc, I* pDescItem, P ptrToItemList,
                        int* ptrToCount = NULL)
{
 return _DetachItem(pDesc, pDescItem, ptrToItemList,
                    (int)((std::size_t)ptrToCount));
}

/**
Attach a new item to ARD item list.

@param pDesc ARD*
@param pDescItem ARDItem*
@return True if succeed otherwise false.
*/
bool _SQLAttachARDItem(ARD* pDesc, ARDItem* pDescItem) 
{
 // maintain highest desc number
 if(pDesc->descCount < pDescItem->colNum)
  pDesc->descCount = pDescItem->colNum;

 return _AttachItem(pDesc, pDescItem, &ARD::bindCols);
}

/**
Attach a new item to IRD item list.

@param pDesc IRD*
@param pDescItem IRDItem*
@return True if succeed otherwise false.
*/
bool _SQLAttachIRDItem(IRD* pDesc, IRDItem* pDescItem)
{
 bool ok = _AttachItem(pDesc, pDescItem, &IRD::rowDesc);

 // maintain highest desc number
 pDesc->descCount = (SQLSMALLINT)pDesc->rowDesc->size();

 return ok;
}

/**
Attach a new item to APD item list.

@param pDesc APD*
@param pDescItem APDItem*
@return True if succeed otherwise false.
*/
bool _SQLAttachAPDItem(APD* pDesc, APDItem* pDescItem)
{
 // maintain highest desc number
 if(pDesc->descCount < pDescItem->paramNum)
  pDesc->descCount = pDescItem->paramNum;

 return _AttachItem(pDesc, pDescItem, &APD::bindParams);
}

/**
Attach a new item to IPD item list.

@param pDesc IPD*
@param pDescItem IPDItem*
@return True if succeed otherwise false.
*/
bool _SQLAttachIPDItem(IPD* pDesc, IPDItem* pDescItem)
{
 // maintain highest desc number
 if(pDesc->descCount < pDescItem->paramNum)
  pDesc->descCount = pDescItem->paramNum;

 return _AttachItem(pDesc, pDescItem, &IPD::bindParams);
}

/**
Detach an existing item from ARD item list.

@param pDesc ARD*
@param pDescItem ARDItem*
@return True if succeed otherwise false.
*/
bool _SQLDetachARDItem(ARD* pDesc, ARDItem* pDescItem) 
{
 return _DetachItem(pDesc, pDescItem, &ARD::bindCols, &ARDItem::colNum);
}

/**
Detach an existing item from IRD item list.

@param pDesc IRD*
@param pDescItem IRDItem*
@return True if succeed otherwise false.
*/
bool _SQLDetachIRDItem(IRD* pDesc, IRDItem* pDescItem)
{
 return _DetachItem(pDesc, pDescItem, &IRD::rowDesc);
}

/**
Detach an existing item from APD item list.

@param pDesc APD*
@param pDescItem APDItem*
@return True if succeed otherwise false.
*/
bool _SQLDetachAPDItem(APD* pDesc, APDItem* pDescItem)
{
 return _DetachItem(pDesc, pDescItem, &APD::bindParams);
}

/**
Detach an existing item from IPD item list.

@param pDesc IPD*
@param pDescItem IPDItem*
@return True if succeed otherwise false.
*/
bool _SQLDetachIPDItem(IPD* pDesc, IPDItem* pDescItem)
{
 return _DetachItem(pDesc, pDescItem, &IPD::bindParams);
}


template<typename T, typename P>
static void* _GetItemRecNum(T* pDesc, SQLSMALLINT pRecNum, P ptrToItemList,
                            int ptrToRecNum)
{
 if((std::size_t)pRecNum > (pDesc->*ptrToItemList)->size())
  return NULL;

 return &(pDesc->*ptrToItemList)->at(pRecNum - 1);
}

template<typename T, typename P, typename P2>
static void* _GetItemRecNum(T* pDesc, SQLSMALLINT pRecNum, P ptrToItemList,
                            P2 ptrToRecNum)
{
 typedef BOOST_TYPEOF((pDesc->*ptrToItemList)->begin()) Iterator;

 Iterator i = std::find_if(
  (pDesc->*ptrToItemList)->begin(), (pDesc->*ptrToItemList)->end(),
  boost::lambda::bind(ptrToRecNum, boost::lambda::_1) == pRecNum);

 if(i == (pDesc->*ptrToItemList)->end())
  return NULL;
 else
  return &(*i);
}

/**
Get a particular descriptor item.

@param pDesc Pointer to descriptor.
@param pRecNum Record number (Descriptor records are numbered from 1).
@param ptrToItemList Member pointer to descriptor item list.
@param ptrToRecNum Member pointer to item's record number.
@return Pointer to descriptor item.
*/
template<typename T, typename P, typename P2>
static void* _GetItem(T* pDesc, SQLSMALLINT pRecNum, P ptrToItemList, 
                      P2 ptrToRecNum)
{
 // check if descriptor is valid
 if(pDesc == NULL || pDesc->hStmt == NULL)
  return NULL;

 if((pDesc->*ptrToItemList) == NULL)
  return NULL;

 return _GetItemRecNum(pDesc, pRecNum, ptrToItemList, ptrToRecNum);
}

template<typename T, typename P>
static void* _GetItem(T* pDesc, SQLSMALLINT pRecNum, P ptrToItemList,
                      int* ptrToRecNum = NULL)
{
 return _GetItem(pDesc, pRecNum, ptrToItemList,
                 (int)((std::size_t)ptrToRecNum));
}

/**
Get a particular ARD item.

@param pDesc ARD*
@param pRecNum Record number (Descriptor records are numbered from 1).
@return ARDItem*
*/
ARDItem* _SQLGetARDItem(const ARD* pDesc, SQLSMALLINT pRecNum) 
{
 // return the ARD item
 return (ARDItem*)_GetItem(pDesc, pRecNum, &ARD::bindCols, &ARDItem::colNum);
}

/**
Get a particular IRD item.

@param pDesc IRD*
@param pRecNum Record number (Descriptor records are numbered from 1).
@return IRDItem*
*/
IRDItem* _SQLGetIRDItem(const IRD* pDesc, SQLSMALLINT pRecNum)
{
 return (IRDItem*)_GetItem(pDesc, pRecNum, &IRD::rowDesc);
}

/**
Get a particular APD item.

@param pDesc APD*
@param pRecNum Record number (Descriptor records are numbered from 1).
@return APDItem*
*/
APDItem* _SQLGetAPDItem(const APD* pDesc, SQLSMALLINT pRecNum)
{
 return (APDItem*)_GetItem(pDesc, pRecNum, &APD::bindParams, 
                           &APDItem::paramNum);
}

/**
Get a particular IPD item.

@param pDesc IPD*
@param pRecNum Record number (Descriptor records are numbered from 1).
@return IPDItem*
*/
IPDItem* _SQLGetIPDItem(const IPD* pDesc, SQLSMALLINT pRecNum) 
{
 return (IPDItem*)_GetItem(pDesc, pRecNum, &IPD::bindParams, 
                           &IPDItem::paramNum);
}

/**
Get a field value in ARD header.

@param pDesc ARD*.
@param pFldID Field identity.
@param pDataPtr Data pointer.
@param pDataSize Data size.
@param pDataSizePtr A pointer to a buffer in which to return the total number
                    of bytes.
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLGetARDField(const ARD* pDesc, SQLSMALLINT pFldID,
                        SQLPOINTER pDataPtr,
                        SQLINTEGER pDataSize, SQLINTEGER* pDataSizePtr)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: pDesc: %d, Fld: %d, DataPtr: %d, DataSize: %d, DataSizePtr: %d"),
  LOG_FUNCTION_NAME,
  pDesc, pFldID, pDataPtr, pDataSize, pDataSizePtr);

 // precaution
 if(pDesc == NULL || pDataPtr == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return SQL_ERROR;
 }

 // as per required field
 switch(pFldID)
 {
  case SQL_DESC_ALLOC_TYPE:
   *((SQLSMALLINT*)pDataPtr) = pDesc->allocType;
   break;

  case SQL_DESC_ARRAY_SIZE:
   *((SQLUINTEGER*)pDataPtr) = pDesc->rowArraySize;
   break;

  case SQL_DESC_ARRAY_STATUS_PTR:
   *((SQLUSMALLINT**)pDataPtr) = pDesc->arrayStatusPtr;
   break;

  case SQL_DESC_BIND_OFFSET_PTR:
   *((SQLINTEGER**)pDataPtr) = pDesc->bindOffsetPtr;
   break;

  case SQL_DESC_BIND_TYPE:
   *((SQLINTEGER*)pDataPtr) = pDesc->bindTypeOrSize;
   break;

  case SQL_DESC_COUNT:
   *((SQLSMALLINT*)pDataPtr) = pDesc->descCount;
   break;

  default:
   LOG_ERROR_F_FUNC(TEXT("unknown field (%d)."), pFldID);
   return SQL_ERROR;
 }

 return SQL_SUCCESS;
}

/**
Get a field value in IRD header.

@param pDesc IRD*.
@param pFldID Field identity.
@param pDataPtr Data pointer.
@param pDataSize Data size.
@param pDataSizePtr A pointer to a buffer in which to return the total number
                    of bytes.
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLGetIRDField(const IRD* pDesc, SQLSMALLINT pFldID,
                        SQLPOINTER pDataPtr,
                        SQLINTEGER pDataSize, SQLINTEGER* pDataSizePtr)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: pDesc: %d, Fld: %d, DataPtr: %d, DataSize: %d, DataSizePtr: %d"),
  LOG_FUNCTION_NAME,
  pDesc, pFldID, pDataPtr, pDataSize, pDataSizePtr);

 // precaution
 if(pDesc == NULL || pDataPtr == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return SQL_ERROR;
 }

 // as per required field
 switch(pFldID)
 {
  case SQL_DESC_ALLOC_TYPE:
   *((SQLSMALLINT*)pDataPtr) = SQL_DESC_ALLOC_AUTO;
   break;

  case SQL_DESC_ARRAY_STATUS_PTR:
   *((SQLUSMALLINT**)pDataPtr) = pDesc->arrayStatusPtr;
   break;

  case SQL_DESC_COUNT:
   *((SQLSMALLINT*)pDataPtr) = pDesc->descCount;
   break;

  case SQL_DESC_ROWS_PROCESSED_PTR:
   *((SQLULEN**)pDataPtr) = pDesc->rowsProcessedPtr;
   break;

  default:
   LOG_ERROR_F_FUNC(TEXT("unknown field (%d)."), pFldID);
   return SQL_ERROR;
 }

 return SQL_SUCCESS;
}

/**
Set a field value in ARD header.

@param pDesc ARD*.
@param pFldID Field identity.
@param pDataPtr Data pointer.
@param pDataSize Data size.
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLSetARDField(ARD* pDesc, SQLSMALLINT pFldID,
                        const SQLPOINTER pDataPtr, SQLINTEGER pDataSize)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: pDesc: %d, Fld: %d, DataPtr: %d, DataSize: %d"),
  LOG_FUNCTION_NAME,
  pDesc, pFldID, pDataPtr, pDataSize);

 // precaution
 if(pDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return SQL_ERROR;
 }

 // as per required field
 switch(pFldID)
 {
  case SQL_DESC_ARRAY_SIZE:
   LOG_DEBUG_F_FUNC(TEXT("ARD RowArraySize is set to %d"),
    (SQLUINTEGER)((std::size_t)pDataPtr));
   pDesc->rowArraySize = (SQLUINTEGER)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_ARRAY_STATUS_PTR:
   pDesc->arrayStatusPtr = (SQLUSMALLINT*)pDataPtr;
   break;

  case SQL_DESC_BIND_OFFSET_PTR:
   pDesc->bindOffsetPtr = (SQLINTEGER*)pDataPtr;
   break;

  case SQL_DESC_BIND_TYPE:
   pDesc->bindTypeOrSize = (SQLINTEGER)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_COUNT:
   // ???? requires that all descriptors which r above the specified
   // value are freed
   LOG_ERROR_FUNC(TEXT("SQL_DESC_COUNT not implemented."));
   return SQL_ERROR;

  default:
   LOG_ERROR_F_FUNC(TEXT("unknown field (%d)."), pFldID);
   return SQL_ERROR;
 }

 return SQL_SUCCESS;
}

/**
Set a field value in IRD header.

@param pDesc IRD*.
@param pFldID Field identity.
@param pDataPtr Data pointer.
@param pDataSize Data size.
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLSetIRDField(IRD* pDesc, SQLSMALLINT pFldID,
                        const SQLPOINTER pDataPtr, SQLINTEGER pDataSize)
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: pDesc: %d, Fld: %d, DataPtr: %d, DataSize: %d"),
  LOG_FUNCTION_NAME,
  pDesc, pFldID, pDataPtr, pDataSize);

 // precaution
 if(pDesc == NULL)
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return SQL_ERROR;
 }

 // as per required field
 switch(pFldID)
 {
  case SQL_DESC_ARRAY_STATUS_PTR:
   pDesc->arrayStatusPtr = (SQLUSMALLINT*)pDataPtr;
   break;

  case SQL_DESC_ROWS_PROCESSED_PTR:
   pDesc->rowsProcessedPtr = (SQLULEN*)pDataPtr;
   break;

  default:
   LOG_ERROR_F_FUNC(TEXT("unknown field (%d)."), pFldID);
   return SQL_ERROR;
 }

 return SQL_SUCCESS;
}

/**
Check if the specified type is a valid SQL data type.

@param pDataType Data type.
@return True if succeed otherwise false.
*/
static bool _SQLCheckDataType(SQLSMALLINT pDataType) 
{
 switch(pDataType) 
 {
  case SQL_CHAR:
  case SQL_VARCHAR:
  case SQL_WCHAR:
  case SQL_WVARCHAR:
  case SQL_C_SSHORT:
  case SQL_C_USHORT:
  case SQL_SMALLINT:
  case SQL_C_SLONG:
  case SQL_C_ULONG:
  case SQL_INTEGER:
  case SQL_NUMERIC:
  case SQL_DECIMAL:
  case SQL_FLOAT:
  case SQL_REAL:
  case SQL_DOUBLE:
  case SQL_TYPE_DATE:
  case SQL_TYPE_TIME:
  case SQL_TYPE_TIMESTAMP:
  case SQL_BIT:
  case SQL_DEFAULT:
  case SQL_C_SBIGINT:
  case SQL_C_UBIGINT:
  case SQL_C_TINYINT:
  case SQL_C_STINYINT:
  case SQL_C_UTINYINT:
   return true;

  default:
   LOG_ERROR_F_FUNC(TEXT("Unknown data type: %d"), pDataType);
   return false;
 }
}

/**
Check if the specified type is a valid interval code.

@param pIntervalCode Interval code.
@return True if succeed otherwise false.
*/
static bool _SQLCheckIntervalCode(SQLSMALLINT pIntervalCode) 
{
 // can check the interval code
 return true;
}


/**
Set the value of data type and interval codes in descriptors.

@param pFldID Field identity.
@param pFldValue Field value.
@param pVerboseDataType Verbose data type.
@param pConciseDataType Concise data type.
@param pDateTimeIntervalCode Date time interval code.
@return True if succeed otherwise false.
*/
static bool _SQLSetDataType(SQLSMALLINT pFldID, SQLSMALLINT pFldValue, 
                            SQLSMALLINT* pVerboseDataType, 
                            SQLSMALLINT* pConciseDataType, 
                            SQLSMALLINT* pDateTimeIntervalCode) 
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: pFldID:%d, pFldValue:%d"),
  LOG_FUNCTION_NAME,
  pFldID, pFldValue);

 // note
 // the data type, concise data type, datetime interval code r interdependent
 // setting one of these changes the other.
 SQLSMALLINT conciseType;
 SQLSMALLINT verboseType;
 SQLSMALLINT datetimeInterval;

 // initial values
 conciseType = pConciseDataType ? *pConciseDataType : 0;
 verboseType = pVerboseDataType ? *pVerboseDataType : 0;
 datetimeInterval = pDateTimeIntervalCode ? *pDateTimeIntervalCode : 0;

 // check if concise type has been specified
 if(pFldID == SQL_DESC_CONCISE_TYPE) 
 {
  // set the concise type itself first
  conciseType = pFldValue;

  // based on concise type set the verbose type and datetime interval
  switch(conciseType) 
  {
   case SQL_TYPE_DATE:
    //case SQL_C_TYPE_DATE:
    verboseType = SQL_DATETIME;
    datetimeInterval = SQL_CODE_DATE;
    break;

   case SQL_TYPE_TIME:
    //case SQL_C_TYPE_TIME:
    verboseType = SQL_DATETIME;
    datetimeInterval = SQL_CODE_TIME;
    break;

   case SQL_TYPE_TIMESTAMP:
    //case SQL_C_TYPE_TIMESTAMP:
    verboseType = SQL_DATETIME;
    datetimeInterval = SQL_CODE_TIMESTAMP;
    break;

   case SQL_INTERVAL_MONTH:
    //case SQL_C_INTERVAL_MONTH:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_MONTH;
    break;

   case SQL_INTERVAL_YEAR:
    //case SQL_C_INTERVAL_YEAR:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_YEAR;
    break;

   case SQL_INTERVAL_YEAR_TO_MONTH:
    //case SQL_C_INTERVAL_YEAR_TO_MONTH:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_YEAR_TO_MONTH;
    break;

   case SQL_INTERVAL_DAY:
    //case SQL_C_INTERVAL_DAY:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_DAY;
    break;

   case SQL_INTERVAL_HOUR:
    //case SQL_C_INTERVAL_HOUR:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_HOUR;
    break;

   case SQL_INTERVAL_MINUTE:
    //case SQL_C_INTERVAL_MINUTE:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_MINUTE;
    break;

   case SQL_INTERVAL_SECOND:
    //case SQL_C_INTERVAL_SECOND:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_SECOND;
    break;

   case SQL_INTERVAL_DAY_TO_HOUR:
    //case SQL_C_INTERVAL_DAY_TOHOUR:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_DAY_TO_HOUR;
    break;

   case SQL_INTERVAL_DAY_TO_MINUTE:
    //case SQL_C_INTERVAL_DAY_TO_MINUTE:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_DAY_TO_MINUTE;
    break;

   case SQL_INTERVAL_DAY_TO_SECOND:
    //case SQL_C_INTERVAL_DAY_TO_SECOND:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_DAY_TO_SECOND;
    break;

   case SQL_INTERVAL_HOUR_TO_MINUTE:
    //case SQL_C_INTERVAL_HOUR_TO_MINUTE:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_HOUR_TO_MINUTE;
    break;

   case SQL_INTERVAL_HOUR_TO_SECOND:
    //case SQL_C_INTERVAL_HOUR_TO_SECOND:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_HOUR_TO_SECOND;
    break;

   case SQL_INTERVAL_MINUTE_TO_SECOND:
    //case SQL_C_INTERVAL_MINUTE_TO_SECOND:
    verboseType = SQL_INTERVAL;
    datetimeInterval = SQL_CODE_MINUTE_TO_SECOND;
    break;

   default:

    // check if data type if valid
    if(_SQLCheckDataType(conciseType) != true)
     return false;

    // concise type does not relate to datetime or interval
    // hence both concise and verbose type r equal
    verboseType = conciseType;
    datetimeInterval = 0;
  }
 }
 // check if verbose data type is being set
 else if(pFldID == SQL_DESC_TYPE) 
 {
  // set the verbose type itself first
  verboseType = pFldValue;

  // based on verbose type & datetime interval code determine concise type
  switch(verboseType) 
  {
   case SQL_INTERVAL:
    switch(datetimeInterval) 
    {
     case SQL_CODE_DATE:
      conciseType = SQL_TYPE_DATE;
      break;

     case SQL_CODE_TIME:
      conciseType = SQL_TYPE_TIME;
      break;

     case SQL_CODE_TIMESTAMP:
      conciseType = SQL_TYPE_TIMESTAMP;
      break;

     default:
      // interval should have been set
      LOG_ERROR_FUNC(TEXT("Interval code not yet set for SQL_INTERVAL"));
      return false;
    }

    break;

   case SQL_DATETIME:
    switch(datetimeInterval) 
    {
     case SQL_CODE_MONTH:
      conciseType = SQL_INTERVAL_MONTH;
      break;

     case SQL_CODE_YEAR:
      conciseType = SQL_INTERVAL_YEAR;
      break;

     case SQL_CODE_YEAR_TO_MONTH:
      conciseType = SQL_INTERVAL_YEAR_TO_MONTH;
      break;

     case SQL_CODE_DAY:
      conciseType = SQL_INTERVAL_DAY;
      break;

     case SQL_CODE_HOUR:
      conciseType = SQL_INTERVAL_HOUR;
      break;

     case SQL_CODE_MINUTE:
      conciseType = SQL_INTERVAL_MINUTE;
      break;

     case SQL_CODE_SECOND:
      conciseType = SQL_INTERVAL_SECOND;
      break;

     case SQL_CODE_DAY_TO_HOUR:
      conciseType = SQL_INTERVAL_DAY_TO_HOUR;
      break;

     case SQL_CODE_DAY_TO_MINUTE:
      conciseType = SQL_INTERVAL_DAY_TO_MINUTE;
      break;

     case SQL_CODE_DAY_TO_SECOND:
      conciseType = SQL_INTERVAL_DAY_TO_SECOND;
      break;

     case SQL_CODE_HOUR_TO_MINUTE:
      conciseType = SQL_INTERVAL_HOUR_TO_MINUTE;
      break;

     case SQL_CODE_HOUR_TO_SECOND:
      conciseType = SQL_INTERVAL_HOUR_TO_SECOND;
      break;

     case SQL_CODE_MINUTE_TO_SECOND:
      conciseType = SQL_INTERVAL_MINUTE_TO_SECOND;
      break;

     default:
      // interval should have been set
      LOG_ERROR_FUNC(TEXT("Interval code not yet set for SQL_DATETIME"));
      return false;
    }

    break;

   default:

    // check if data type if valid
    if(_SQLCheckDataType(verboseType) != true)
     return false;

    // verbose type does not relate to datetime or interval
    // hence both concise and verbose type r equal
    conciseType = verboseType;
    datetimeInterval = 0;
    break;
  }
 }
 else if(pFldID == SQL_DESC_DATETIME_INTERVAL_CODE) 
 {
  // check if date interval code is valid
  if(_SQLCheckIntervalCode(pFldValue) != true)
   return false;

  // set the datetime interval code, autonomously
  datetimeInterval = pFldValue;
 }

 // unknown field to set
 else 
 {
  LOG_ERROR_FUNC(TEXT("Unknown field type"));
  return false;
 }

 // pass back values to caller
 if(pVerboseDataType)
  *pVerboseDataType = verboseType;

 if(pConciseDataType)
  *pConciseDataType = conciseType;

 if(pDateTimeIntervalCode)
  *pDateTimeIntervalCode = datetimeInterval;

 return true;
}

/**
Set a field value in ARD item.

@param pDesc ARD*
@param pDescItem ARDItem*
@param pFldID Field identity.
@param pDataPtr Data pointer.
@param pDataSize Data size.
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLSetARDItemField(ARD* pDesc, ARDItem* pDescItem, 
                            SQLSMALLINT pRecNum, SQLSMALLINT pFldID,
                            const SQLPOINTER pDataPtr, SQLINTEGER pDataSize) 
{
 LOG_DEBUG_F_FUNC(
  TEXT("%s: pDesc: %d, pDescItem: %d, Recnum: %d, Fld: %d, ")
  TEXT("DataPtr: %d, DataSize: %d"), 
  LOG_FUNCTION_NAME,
  pDesc, pDescItem, pRecNum, pFldID, pDataPtr, pDataSize);

 ARDItem* item;

 // precaution
 if(pDesc == NULL) 
 {
  LOG_ERROR_FUNC(TEXT("Invalid params"));
  return SQL_ERROR;
 }

 // check if item has not been specified directly
 if(pDescItem == NULL) 
 {
  // get item from ARD
  item = _SQLGetARDItem(pDesc, pRecNum);

  // check if item located
  if(item == NULL) 
  {
   LOG_ERROR_FUNC(TEXT("Invalid item"));
   return SQL_ERROR;
  }
 }

 else
 {
  item = pDescItem;
 }

 // as per required field
 switch(pFldID) 
 {
  case SQL_DESC_CONCISE_TYPE:
   _SQLSetDataType(SQL_DESC_CONCISE_TYPE,
     (SQLSMALLINT)((std::size_t)pDataPtr),
     &(item->dataVerboseType), &(item->dataConciseType), 
     &(item->dateTimeIntervalCode));
   break;

  case SQL_DESC_DATA_PTR:
   item->dataPtr = pDataPtr;
   break;

  case SQL_DESC_DATETIME_INTERVAL_CODE:
   _SQLSetDataType(SQL_DESC_DATETIME_INTERVAL_CODE,
     (SQLSMALLINT)((std::size_t)pDataPtr),
     &(item->dataVerboseType), &(item->dataConciseType), 
     &(item->dateTimeIntervalCode));
   break;

  case SQL_DESC_DATETIME_INTERVAL_PRECISION:
   item->dateTimeIntervalPrec = (SQLSMALLINT)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_INDICATOR_PTR:
   item->sizeIndPtr = (SQLINTEGER*)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_LENGTH:
  case SQL_DESC_OCTET_LENGTH:
   item->dataSize = (SQLINTEGER)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_NUM_PREC_RADIX:
   item->numPrecRadix = (SQLINTEGER)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_OCTET_LENGTH_PTR: // 1004
   item->sizePtr = (SQLINTEGER*)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_PRECISION:
   // bytes required for numeric type
   item->dataSize = (SQLSMALLINT)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_SCALE:
   item->scale = (SQLSMALLINT)((std::size_t)pDataPtr);
   break;

  case SQL_DESC_TYPE:
   _SQLSetDataType(SQL_DESC_TYPE,
    (SQLSMALLINT)((std::size_t)pDataPtr),
    &(item->dataVerboseType), &(item->dataConciseType), 
    &(item->dateTimeIntervalCode));
   break;

  default:
   LOG_ERROR_F_FUNC(TEXT("unknown field (%d)."), pFldID);
   return SQL_ERROR;
 }

 return SQL_SUCCESS;
}

/**
Free/release the content of descriptor.

@param pDesc Pointer to descriptor.
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
template<typename T, typename I>
static RETCODE _FreeContent(T* pDesc, I*& itemList)
{
 if(itemList)
 {
  delete itemList;
  itemList = NULL;
 }

 // reset the highest descriptor count
 pDesc->descCount = 0;

 return SQL_SUCCESS;
}

/**
Free/release the content of ARD.

@param pDesc ARD*
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLFreeARDContent(ARD* pDesc) 
{
 return _FreeContent(pDesc, pDesc->bindCols);
}

/**
Free/release the content of IRD.

@param pDesc IRD*
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLFreeIRDContent(IRD* pDesc) 
{
 /* COLUMN HDRS */
 if(pDesc->rowDesc)
 {
  IRDItemList::iterator i = pDesc->rowDesc->begin();
  IRDItemList::iterator iEnd = pDesc->rowDesc->end();
  for(; i != iEnd; ++i)
  {
   IRDItem* columnHeader = &(*i);

   if(columnHeader->pszSQL_DESC_BASE_COLUMN_NAME)
    free(columnHeader->pszSQL_DESC_BASE_COLUMN_NAME);
   if(columnHeader->pszSQL_DESC_BASE_TABLE_NAME)
    free(columnHeader->pszSQL_DESC_BASE_TABLE_NAME);
   if(columnHeader->pszSQL_DESC_CATALOG_NAME)
    free(columnHeader->pszSQL_DESC_CATALOG_NAME);
   if(columnHeader->pszSQL_DESC_LABEL)
    free(columnHeader->pszSQL_DESC_LABEL);
   if(columnHeader->pszSQL_DESC_LITERAL_PREFIX)
    free(columnHeader->pszSQL_DESC_LITERAL_PREFIX);
   if(columnHeader->pszSQL_DESC_LITERAL_SUFFIX)
    free(columnHeader->pszSQL_DESC_LITERAL_SUFFIX);
   if(columnHeader->pszSQL_DESC_LOCAL_TYPE_NAME)
    free(columnHeader->pszSQL_DESC_LOCAL_TYPE_NAME);
   if(columnHeader->pszSQL_DESC_NAME)
    free(columnHeader->pszSQL_DESC_NAME);
   if(columnHeader->pszSQL_DESC_SCHEMA_NAME)
    free(columnHeader->pszSQL_DESC_SCHEMA_NAME);
   if(columnHeader->pszSQL_DESC_TABLE_NAME)
    free(columnHeader->pszSQL_DESC_TABLE_NAME);
   if(columnHeader->pszSQL_DESC_TYPE_NAME)
    free(columnHeader->pszSQL_DESC_TYPE_NAME);
  }
 }

 return _FreeContent(pDesc, pDesc->rowDesc);
}

/**
Free/release the content of APD.

@param pDesc APD*
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLFreeAPDContent(APD* pDesc)
{
 // since params have not been implemented
 // it assumes that no params have been allocated
 return _FreeContent(pDesc, pDesc->bindParams);
}

/**
Free/release the content of IPD.

@param pDesc IPD*
@return SQL_SUCCESS if OK otherwise SQL_ERROR.
*/
RETCODE _SQLFreeIPDContent(IPD* pDesc)
{
 // since params have not been implemented
 // it assumes that no params have been allocated
 return _FreeContent(pDesc, pDesc->bindParams);
}
