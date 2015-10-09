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

#ifndef _BIGOBJECT_ODBC_CLIENT_ROW_DETAIL_
#define _BIGOBJECT_ODBC_CLIENT_ROW_DETAIL_

#include <string>
#include <vector>
#include <locale>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/any.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/typeof/typeof.hpp>

// http://stackoverflow.com/questions/6425178/
// boost-tuple-increasing-maximum-number-of-elements
//#include <boost/tuple/tuple.hpp>
#include <boost/tr1/tuple.hpp>

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/slot/slot.hpp>

#include <boost/optional.hpp>

#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_back.hpp>

#include <boost/lambda/lambda.hpp>

#include <boost/type_traits.hpp>

#include "../../../Base/CRTP.hpp"
#include "../../../Base/TString.hpp"

#include "../Row.hpp"

namespace ODBCDriver
{

/// Row container type, all column value are TCHAR*.
typedef std::vector<TCHAR*> CharPtrRowStorage;

/// Row container type, all column value are string.
typedef std::vector<boost::optional<tstring> > StringRowStorage;

/// Row container type, all column value are boost::any.
typedef std::vector<boost::any> AnyRowStorage;

/**
Row storage policy for storage which uses NULL pointer based to represent NULL.
*/
template<typename R>
struct NullPolicy
{
 static inline const typename R::StorageType::value_type GetNull()
 {
  return NULL;
 }
};

/**
Row storage policy for storage which uses boost::optional based to represent 
NULL.
*/
template<typename R>
struct OptionalPolicy
{
 static inline const boost::none_t& GetNull()
 {
  return boost::none;
 }
};

/**
Row storage type policy for list based storage 
(std::vector, std::list, ...etc.).
*/
template<typename R>
struct ListStoragePolicy
{
 /**
 Create RowStoragePtr.

 @param iter Values iterator.
 @param iterEnd Values end iterator.
 @return RowStoragePtr.
 */
 template<typename I>
 static typename R::StoragePtrType Create(I iter, I iterEnd)
 {
  return typename R::StoragePtrType(
   new typename R::StorageType(iter, iterEnd));
 }

};

/**
Abstract row implementation for any type storage.
*/
template<typename T, typename Impl>
class AbstractRow : public Row, public bigobject::CRTP<Impl, AbstractRow<T, Impl> >
{
public:

 /// Row storage type.
 typedef T StorageType;

 /// Point to StorageType.
 typedef boost::shared_ptr<T> StoragePtrType;

public:

 /**
 Set row value.

 @param v Row value container form.
 @return Old value pointer.
 */
 StoragePtrType Set(StoragePtrType& v)
 {
  StoragePtrType old = values;
  values = v;

  return old;
 }

 virtual ServerReturn IsNull(std::size_t columnIdx, bool* isNull)
 {
  *isNull = IsNull(columnIdx);
  return SERVER_SUCCESS;
 }

 virtual ServerReturn GetFieldDataLen(std::size_t columnIdx, 
                                      std::size_t* colLen)
 {
  tstring value;

  ServerReturn ret = Get<tstring>(columnIdx, value, NULL);
  *colLen = value.size();

  return ret;
 }

 virtual ServerReturn GetFieldAsString(std::size_t columnIdx, tstring& value,
                                       bool* isNull)
 {
  return Get<tstring>(columnIdx, value, isNull);
 }

 virtual ServerReturn GetFieldAsDouble(std::size_t columnIdx, double* buffer,
                                       bool* isNull)
 {
  return Get<double>(columnIdx, *buffer, isNull);
 }

 virtual ServerReturn GetFieldAsInt(std::size_t columnIdx, int* buffer, 
                                    bool* isNull)
 {
  return Get<int>(columnIdx, *buffer, isNull);
 }

 virtual ServerReturn GetFieldAsLong(std::size_t columnIdx, long* buffer, 
                                     bool* isNull)
 {
  return Get<long>(columnIdx, *buffer, isNull);
 }

 virtual ServerReturn GetFieldAsULong(std::size_t columnIdx,
                                      unsigned long* buffer, bool* isNull)
 {
  return Get<unsigned long>(columnIdx, *buffer, isNull);
 }

 virtual ServerReturn GetFieldAsI64(std::size_t columnIdx, int64_t* buffer,
                                    bool* isNull)
 {
  return Get<int64_t>(columnIdx, *buffer, isNull);
 }

 virtual ServerReturn GetFieldAsI64U(std::size_t columnIdx, uint64_t* buffer,
                                     bool* isNull)
 {
  return Get<uint64_t>(columnIdx, *buffer, isNull);
 }

 virtual ServerReturn GetFieldAsDate(std::size_t columnIdx,
                                     struct tagDATE_STRUCT* buffer,
                                     bool* isNull)
 {
  tstring v;
  
  ServerReturn ret = GetFieldAsString(columnIdx, v, isNull);
  if(ret != SERVER_SUCCESS || *isNull)
   return ret;

  boost::posix_time::ptime ptime = ParseDate(v);

  buffer->year = ptime.date().year();
  buffer->month = ptime.date().month();
  buffer->day = ptime.date().day();

  return SERVER_SUCCESS;
 }

 virtual ServerReturn GetFieldAsTime(std::size_t columnIdx,
                                     struct tagTIME_STRUCT* buffer,
                                     bool* isNull)
 {
  tstring v;

  ServerReturn ret = GetFieldAsString(columnIdx, v, isNull);
  if(ret != SERVER_SUCCESS || *isNull)
   return ret;

  boost::posix_time::ptime ptime = ParseTime(v);

  buffer->hour = ptime.time_of_day().hours();
  buffer->minute = ptime.time_of_day().minutes();
  buffer->second = ptime.time_of_day().seconds();

  return SERVER_SUCCESS;
 }

 virtual ServerReturn GetFieldAsTimestamp(std::size_t columnIdx,
                                          struct tagTIMESTAMP_STRUCT* buffer,
                                          bool* isNull)
 {
  tstring v;

  ServerReturn ret = GetFieldAsString(columnIdx, v, isNull);
  if(ret != SERVER_SUCCESS || *isNull)
   return ret;

  bool dateValid, timeValid;
  boost::posix_time::ptime ptime = ParseTimestamp(v, false, 
                                                  &dateValid, &timeValid);

  if(dateValid)
  {
   try
   {
    BOOST_AUTO(pDate, ptime.date());

    buffer->year = pDate.year();
    buffer->month = pDate.month();
    buffer->day = pDate.day();
   }
   catch(std::exception& ex)
   {
    LOG_DEBUG_F_FUNC(TEXT("Unable to parse timestamp: %s (%s)"),
     v.c_str(), LOG4CPLUS_STRING_TO_TSTRING(ex.what()));
    
    dateValid = false;
   }
  }

  if(!dateValid)
  {
   // https://msdn.microsoft.com/en-us/library/bb677267%28v=sql.120%29.aspx
   buffer->year = 1900;
   buffer->month = 1;
   buffer->day = 1;
  }

  if(timeValid)
  {
   try
   {
    BOOST_AUTO(pTimeOfDay, ptime.time_of_day());

    buffer->hour = pTimeOfDay.hours();
    buffer->minute = pTimeOfDay.minutes();
    buffer->second = pTimeOfDay.seconds();
    buffer->fraction = (SQLUINTEGER)pTimeOfDay.fractional_seconds();
   }
   catch(std::exception& ex)
   {
    LOG_DEBUG_F_FUNC(TEXT("Unable to parse timestamp: %s (%s)"),
     v.c_str(), LOG4CPLUS_STRING_TO_TSTRING(ex.what()));

    timeValid = false;
   }
  }

  if(!timeValid)
  {
   // https://msdn.microsoft.com/en-us/library/bb677267%28v=sql.120%29.aspx
   buffer->hour = 0;
   buffer->minute = 0;
   buffer->second = 0;
   buffer->fraction = 0;
  }

  return SERVER_SUCCESS;
 }

protected:

 /**
 Parse date.

 @param v TString.
 @param throws Throw exception if parse error, otherwise return a default 
               datetime object (ptime).
 @return Ptime.
 */
 boost::posix_time::ptime ParseDate(const tstring& v, bool throws = true)
 {
  struct Validator
  {
   static bool Check(boost::posix_time::ptime& ptime, const tstring& v,
                     const tstring& separator)
   {
    if(ptime == boost::posix_time::ptime())
     return false;

    if(v.find(separator) == tstring::npos)
     return false;

    try
    {
     BOOST_AUTO(pDate, ptime.date());
    }
    catch(std::exception& ex)
    {
     return false;
    }

    return true;
   }

   static tstring Preprocess(const tstring& v, const tstring& separator)
   {
    if(v.find(separator) == tstring::npos)
     return v;

    std::string s(ODBC_TSTR_TO_STR(separator));

    std::vector<tstring> values;
    boost::split(values, v, boost::is_any_of(s));

    std::vector<tstring>::iterator i = values.begin();
    std::vector<tstring>::iterator iEnd = values.end();
    for(; i != iEnd; ++i)
    {
     tstring& t = *i;

     if(t.length() < 2)
      t.insert(0, TEXT("0"));
    }

    return boost::algorithm::join(values, s);
   }
  };

  const char* formats1[] = {
   "%Y-%m-%d",
   "%m-%d-%Y",
   "%d-%m-%Y",
   "%y-%m-%d",
   "%m-%d-%y",
   "%d-%m-%y",
  };
 
  /*
  #FIXME: It's wierd, if I use boost::gregorian::date, 2013/3/30 will not be 
          parsed, only 2013/03/30 will work. %m, %d must be 2 digits. 
          But both types (date_input_facet/time_input_facet) will make wrong
          result if it pass 09:07 to it, this should not be parsed, but it 
          does.
  */
  bool valid = false;

  tstring sep(TEXT("-"));

  boost::posix_time::ptime ptime = ParseDateTime<
   boost::gregorian::date,
   boost::gregorian::date_input_facet>(
   //boost::posix_time::ptime,
   //boost::posix_time::time_input_facet>(
   Validator::Preprocess(v, sep), 
   formats1, sizeof(formats1) / sizeof(formats1[0]), false);

  if(Validator::Check(ptime, v, sep))
   return ptime;

  const char* formats2[] = {
   "%Y/%m/%d",
   "%m/%d/%Y",
   "%d/%m/%Y",
   "%y/%m/%d",
   "%m/%d/%y",
   "%d/%m/%y",
  };

  sep = TEXT("/");

  boost::posix_time::ptime ptime2 = ParseDateTime<
   boost::gregorian::date,
   boost::gregorian::date_input_facet>(
   //boost::posix_time::ptime,
   //boost::posix_time::time_input_facet>(
   Validator::Preprocess(v, sep), 
   formats2, sizeof(formats2) / sizeof(formats2[0]), throws);

  if(Validator::Check(ptime2, v, sep))
   return ptime2;
  
  return boost::posix_time::ptime();
 }

 /**
 Parse time.

 @param v TString.
 @param throws Throw exception if parse error, otherwise return a default 
               datetime object (ptime).
 @return Ptime.
 */
 boost::posix_time::ptime ParseTime(const tstring& v, bool throws = true)
 {
  const char* formats[] = {
   "%H:%M:%S",
   "%H:%M:%S.%F",
   "%H:%M",

   "%H.%M.%S",
   "%H.%M.%S.%F",
   "%H.%M",
  };

  return ParseDateTime<
   boost::posix_time::ptime, 
   boost::posix_time::time_input_facet>(
   v, formats, sizeof(formats) / sizeof(formats[0]), throws);
 }

 /**
 Parse timestamp.

 @param v TString.
 @param throws Throw exception if parse error, otherwise return a default 
               datetime object (ptime).
 @param dateValid Result boolean pointer to indicate date format valid or not.
 @param timeValid Result boolean pointer to indicate time format valid or not.
 @return Ptime.
 */
 boost::posix_time::ptime ParseTimestamp(const tstring& v, bool throws = true,
                                         bool* dateValid = NULL, 
                                         bool* timeValid = NULL)
 {
  if(dateValid)
   *dateValid = false;

  if(timeValid)
   *timeValid = false;

  // https://www.ibphoenix.com/resources/documents/design/doc_169
  std::vector<tstring> kv;
  boost::split(kv, v, boost::is_any_of(" "));

  if(kv.size() != 2)
  {
   if(throws)
   {
    std::string msg("Unable to parse timestamp: ");
    msg.append(ODBC_TSTR_TO_STR(v));

    throw ClientException(msg.c_str());
   }
   else
   {
    LOG_DEBUG_F_FUNC(TEXT("Unable to parse timestamp: %s"), v.c_str());
   }
  }

  std::size_t i = 0;

  boost::posix_time::ptime datePart = ParseDate(kv[0], throws);  

  if(datePart != boost::posix_time::ptime())
  {
   if(kv.size() > 1)
   {
    ++i;
   }
   else
   {
    if(dateValid)
     *dateValid = true;

    return datePart; // Only date.
   }
  }

  boost::posix_time::ptime timePart = ParseTime(kv[i], throws);

  if(datePart == boost::posix_time::ptime())
  {
   if(timePart != boost::posix_time::ptime())
   {
    if(timeValid)
     *timeValid = true;
   }

   return timePart; // Only time.
  }

  // All parsed.
  if(dateValid)
   *dateValid = true;

  if(timeValid)
   *timeValid = true;

  return boost::posix_time::ptime(datePart.date(), timePart.time_of_day());
 }

 /**
 Parse datetime by given formats.

 @param v TString.
 @param formats Datetime formats array.
 @param formatsN Size of formats.
 @param throws Throw exception if parse error, otherwise return a default 
               datetime object (ptime).
 @return Ptime.
 */
 template<typename P, typename F>
 boost::posix_time::ptime ParseDateTime(const tstring& v, 
                                        const char* formats[],
                                        const std::size_t formatsN, 
                                        bool throws = true)
 {
  // http://stackoverflow.com/questions/3786201/
  // how-to-parse-date-time-from-string
  // http://stackoverflow.com/questions/13513944/
  // find-out-the-date-time-format-of-string-date-in-c-sharp
  // http://stackoverflow.com/questions/5330459/
  // ownership-deleteing-the-facet-in-a-locale-stdlocale  
  P ptime;
  bool parsed = false;

  std::string s(ODBC_TSTR_TO_STR(v));

  for(std::size_t i = 0; i < formatsN; ++i)
  {
   std::istringstream is(s);
   is.imbue(std::locale(is.getloc(), new F(formats[i])));

   try
   {
    is >> ptime;
   }
   catch(std::exception&)
   {
    continue;
   }

   if(ptime != P())
   {
    parsed = true;
    break;
   }
  }

  if(!parsed)
  {
   if(throws)
   {
    std::string msg("Unable to parse date time: ");
    msg.append(ODBC_TSTR_TO_STR(v));

    throw ClientException(msg.c_str());
   }
   else
   {
    LOG_DEBUG_F_FUNC(TEXT("Unable to parse date time: %s"), v.c_str());
    return boost::posix_time::ptime();
   }
  }

  return boost::posix_time::ptime(ptime);
 }

 /**
 Get storage size template method.

 @return Storage size.
 */
 std::size_t GetStorageSizeImpl()
 {
  return values->size();
 }

 /**
 Is column NULL value?

 @param columnIdx Column index.
 @return True if NULL otherwise false.
 */
 bool IsNull(std::size_t columnIdx)
 {
  if(columnIdx >= this->This().GetStorageSizeImpl())
  {
   std::string what("Illegal access to column ");

   what.append(boost::lexical_cast<std::string>(columnIdx));
   what.append(", total column number is ");
   what.append(boost::lexical_cast<std::string>(
    this->This().GetStorageSizeImpl()));
   what.push_back('.');

   throw ClientException(what.c_str());
  }

  return this->This().IsNullImpl(columnIdx);
 }

 /**
 IsNull implementation template method.

 @param columnIdx Column index.
 @return True if NULL otherwise false.
 */
 bool IsNullImpl(std::size_t columnIdx)
 {
  return false;
 }

 /**
 Get column value for certain type V.

 @param columnIdx Column index.
 @param value Reference to store result value.
 @param isNull Pointer to store indication value of null.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 template<typename V>
 ServerReturn Get(std::size_t columnIdx, V& value, bool* isNull)
 {
  if(isNull)
   *isNull = IsNull(columnIdx);

  if(*isNull)
   return SERVER_SUCCESS;

  try
  {
   return this->This().GetImpl<V>(columnIdx, value);
  }
  catch(std::exception& ex)
  {
   std::string msg("Unable to get column ");
   msg.append(boost::lexical_cast<std::string>(columnIdx));
   msg.append(" as desired type:");
   msg.append(ex.what());

   throw ClientException(msg.c_str());
  }
 }

 /**
 Get<V> implementation template method.

 @param columnIdx Column index.
 @param value Reference to store result value.
 @return SERVER_SUCCESS if successful, or SERVER_ERROR if an error occurred.
 */
 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  return SERVER_SUCCESS;
 }

protected:

 /// Internal value pointer.
 StoragePtrType values;

};

/**
Row for TCHAR* storage implementation.
*/
class CharPtrRow 
 : public AbstractRow<CharPtrRowStorage, CharPtrRow>,
   public ListStoragePolicy<AbstractRow<CharPtrRowStorage, CharPtrRow> >,
   public NullPolicy<AbstractRow<CharPtrRowStorage, CharPtrRow> >
{
public:

 bool IsNullImpl(std::size_t columnIdx)
 {
  const TCHAR* v = values->at(columnIdx);
  return v == GetNull();
 }

 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  const TCHAR* v = values->at(columnIdx);
  value = boost::lexical_cast<V>(v);

  return SERVER_SUCCESS;
 }

};

/**
Row for tstring storage implementation.
*/
class StringRow 
 : public AbstractRow<StringRowStorage, StringRow>,
   public ListStoragePolicy<AbstractRow<StringRowStorage, StringRow> >,
   public OptionalPolicy<AbstractRow<StringRowStorage, StringRow> >
{
public:
 
 bool IsNullImpl(std::size_t columnIdx)
 {
  const boost::optional<tstring>& v = values->at(columnIdx);
  return v == GetNull();
 }

 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  const tstring& v = *values->at(columnIdx);
  value = boost::lexical_cast<V>(v);

  return SERVER_SUCCESS;
 }

};

/**
Row for boost::any storage implementation.
*/
class AnyRow 
 : public AbstractRow<AnyRowStorage, AnyRow>,
   public ListStoragePolicy<AbstractRow<AnyRowStorage, AnyRow> >
{
public:

 static int* GetNull()
 {
  return NULL;
 }

public:

 bool IsNullImpl(std::size_t columnIdx)
 {
  boost::any v = values->at(columnIdx);
  return v.empty() || boost::any_cast<int*>(v) == GetNull();
 }

 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  value = boost::any_cast<V>(values->at(columnIdx));
  return SERVER_SUCCESS;
 }

};

/**
Row storage type policy for boost::tuple storage.
*/
template<typename R>
struct TupleStoragePolicy
{

// Helper macro.
#define _TUPLE_STORAGE_CREATE_REMOVE_CONST(z, i, var)                         \
  const_cast<typename boost::remove_const<T##i>::type&>(t##i),

 /**
 Create RowStoragePtr.

 @param ... Tuple values.
 @return RowStoragePtr.
 */
#define TUPLE_STORAGE_CREATE(z, n, unused)                                    \
 template<BOOST_PP_ENUM_PARAMS(n, typename T)>                                \
 static typename R::StoragePtrType Create(                                    \
  BOOST_PP_ENUM_BINARY_PARAMS(n, T, t))                                       \
 {                                                                            \
  return typename R::StoragePtrType(                                          \
   new typename R::StorageType(                                               \
    BOOST_PP_REPEAT(BOOST_PP_SUB(n, 1), _TUPLE_STORAGE_CREATE_REMOVE_CONST, T)\
    const_cast<typename boost::remove_const<                                  \
     BOOST_PP_CAT(T, BOOST_PP_SUB(n, 1))>::type&>(                            \
      BOOST_PP_CAT(t, BOOST_PP_SUB(n, 1)))));                                 \
 }

 // Does the actual code replication of TUPLE_STORAGE_CREATE
#define BOOST_PP_LOCAL_MACRO(n) TUPLE_STORAGE_CREATE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS   (1, _ODBC_DRIVER_MACRO_MAX_PARAMS)
#include BOOST_PP_LOCAL_ITERATE()

 /**
 Helper template class to simplify IsNull implementation.
 */
 template<typename V>
 struct IsNullHelper
 {
  std::size_t columnIdx;

  V& v;

  typename R::StoragePtrType& values;

  bool& matched;

  IsNullHelper(std::size_t _columnIdx, V& _v, 
               typename R::StoragePtrType& _values, bool& _matched)
   : columnIdx(_columnIdx), v(_v), values(_values), matched(_matched)
  {
  }

  template <class T>
  void operator()(boost::optional<T>)
  {
   if(columnIdx == T::value)
   {
    v = std::tr1::get<T::value>(*values);
    matched = true;
   }
  }
 };

 // Helper macro.
#define _TUPLE_STORAGE_PUSH_BACK(z, i, var)                                   \
 typedef typename boost::mpl::push_back<_N##i, boost::mpl::int_<var##i>       \
    >::type BOOST_PP_CAT(_N, BOOST_PP_ADD(i, 1));

#define TUPLE_STORAGE_ISNULL(z, n, unused)                                    \
 template<typename V, BOOST_PP_ENUM_PARAMS(n, int N)>                         \
 bool IsNullTupleImpl(std::size_t columnIdx, V& v,                            \
                      typename R::StoragePtrType& values)                     \
 {                                                                            \
  typedef boost::mpl::vector_c<int> _N0;                                      \
  BOOST_PP_REPEAT(n, _TUPLE_STORAGE_PUSH_BACK, N)                             \
  bool matched = false;                                                       \
  IsNullHelper<V> helper(columnIdx, v, values, matched);                      \
  boost::mpl::for_each<_N##n, boost::optional<boost::mpl::_1> >(helper);      \
  return matched;                                                             \
 }

 // Does the actual code replication of TUPLE_STORAGE_ISNULL
#define BOOST_PP_LOCAL_MACRO(n) TUPLE_STORAGE_ISNULL(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS   (1, _ODBC_DRIVER_MACRO_MAX_PARAMS)
#include BOOST_PP_LOCAL_ITERATE()

 /**
 Helper template class to simplify Get implementation.
 */
 template<typename O, typename V>
 struct GetHelper
 {
  std::size_t columnIdx;

  V& v;

  typename R::StoragePtrType& values;

  GetHelper(std::size_t _columnIdx, V& _v, typename R::StoragePtrType& _values)
   : columnIdx(_columnIdx), v(_v), values(_values)
  {
  }

  template <class T>
  void operator()(boost::optional<T>)
  {
   if(columnIdx == T::value)
   {
    Assign<T::value>(
     boost::is_base_of<boost::optional_detail::optional_tag, O>());
   }
  }

  template<int N>
  void Assign(boost::false_type)
  {
   v = boost::lexical_cast<V>(std::tr1::get<N>(*values));
  }

  template<int N>
  void Assign(boost::true_type)
  {
   v = boost::lexical_cast<V>(*std::tr1::get<N>(*values));
  }
 };

#define TUPLE_STORAGE_GET(z, n, unused)                                       \
 template<typename O, typename V, BOOST_PP_ENUM_PARAMS(n, int N)>             \
 void GetTupleImpl(std::size_t columnIdx, V& v,                               \
                   typename R::StoragePtrType& values)                        \
 {                                                                            \
  typedef boost::mpl::vector_c<int> _N0;                                      \
  BOOST_PP_REPEAT(n, _TUPLE_STORAGE_PUSH_BACK, N)                             \
  GetHelper<O, V> helper(columnIdx, v, values);                               \
  boost::mpl::for_each<_N##n, boost::optional<boost::mpl::_1> >(helper);      \
 }

 // Does the actual code replication of TUPLE_STORAGE_GET
#define BOOST_PP_LOCAL_MACRO(n) TUPLE_STORAGE_GET(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS   (1, _ODBC_DRIVER_MACRO_MAX_PARAMS)
#include BOOST_PP_LOCAL_ITERATE()
};

/// Row container type for SQLTables.
typedef std::tr1::tuple<
 boost::optional<tstring>, // TABLE_CAT
 boost::optional<tstring>, // TABLE_SCHEM
 tstring, // TABLE_NAME
 tstring, // TABLE_TYPE
 tstring // REMARKS
> TableRowStorage;

/**
Row for SQLTables implementation.
*/
class TableRow 
 : public AbstractRow<TableRowStorage, TableRow>,
   public TupleStoragePolicy<AbstractRow<TableRowStorage, TableRow> >,
   public OptionalPolicy<AbstractRow<TableRowStorage, TableRow> >
{
public:

 std::size_t GetStorageSizeImpl()
 {
  return std::tr1::tuple_size<StorageType>::value;
 }

 bool IsNullImpl(std::size_t columnIdx)
 {

  if(columnIdx == 2 || columnIdx == 3 || columnIdx == 4)
   return false;
  
  boost::optional<tstring> v;

  // if(columnIdx == N) { v = std::tr1::get<N>(*values); } wherer N = {0, 1}
  IsNullTupleImpl<boost::optional<tstring>, 0, 1>(columnIdx, v, values);

  return v == GetNull();
 }

 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  GetTupleImpl<boost::optional<tstring>, V, 0, 1>(columnIdx, value, values);

  GetTupleImpl<tstring, V, 2, 3, 4>(columnIdx, value, values);

  return SERVER_SUCCESS;
 }

};

/// Row container type for SQLGetTypeInfo.
typedef std::tr1::tuple<
 tstring, // 0, TYPE_NAME
 SQLSMALLINT, // 1, DATA_TYPE
 boost::optional<SQLINTEGER>, // 2, COLUMN_SIZE
 boost::optional<tstring>, // 3, LITERAL_PREFIX
 boost::optional<tstring>, // 4, LITERAL_SUFFIX
 boost::optional<tstring>, // 5, CREATE_PARAMS
 SQLSMALLINT, // 6, NULLABLE
 SQLSMALLINT, // 7, CASE_SENSITIVE
 SQLSMALLINT, // 8, SEARCHABLE
 boost::optional<SQLSMALLINT>, // 9, UNSIGNED_ATTRIBUTE
 SQLSMALLINT, // 10, FIXED_PREC_SCALE
 boost::optional<SQLSMALLINT>, // 11, AUTO_UNIQUE_VALUE
 boost::optional<tstring>, // 12, LOCAL_TYPE_NAME
 boost::optional<SQLSMALLINT>, // 13, MINIMUM_SCALE
 boost::optional<SQLSMALLINT>, // 14, MAXIMUM_SCALE
 SQLSMALLINT, // 15, SQL_DATA_TYPE
 boost::optional<SQLSMALLINT>, // 16, SQL_DATETIME_SUB
 boost::optional<SQLINTEGER>, // 17, NUM_PREC_RADIX
 boost::optional<SQLSMALLINT> // 18, INTERVAL_PRECISION
> TypeInfoRowStorage;

/**
Row for SQLGetTypeInfo implementation.
*/
class TypeInfoRow
 : public AbstractRow<TypeInfoRowStorage, TypeInfoRow>,
   public TupleStoragePolicy<AbstractRow<TypeInfoRowStorage, TypeInfoRow> >,
   public OptionalPolicy<AbstractRow<TypeInfoRowStorage, TypeInfoRow> >
{
public:

 std::size_t GetStorageSizeImpl()
 {
  return std::tr1::tuple_size<StorageType>::value;
 }

 bool IsNullImpl(std::size_t columnIdx)
 {
  switch(columnIdx)
  {
   case 0:
   case 1:
   case 6:
   case 7:
   case 8:
   case 10:
   case 15:
    return false;

   default:
    break;
  }

  {
   boost::optional<tstring> v;
   if(IsNullTupleImpl<boost::optional<tstring>, 3, 4, 5, 12>(
       columnIdx, v, values))
   {
    return v == GetNull();
   }
  }

  {
   boost::optional<SQLSMALLINT> v;   
   if(IsNullTupleImpl<boost::optional<SQLSMALLINT>, 9, 11, 13, 14, 16, 18>(
       columnIdx, v, values))
   {
    return v == GetNull();
   }
  }

  {
   boost::optional<SQLINTEGER> v;
   if(IsNullTupleImpl<boost::optional<SQLINTEGER>, 2, 17>(
       columnIdx, v, values))
   {
    return v == GetNull();
   }
  }

  // Shall not happen.
  throw ClientException("Illegal argument columnIdx.");
 }

 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  // Not optional.  
  GetTupleImpl<tstring, V, 0>(columnIdx, value, values);

  GetTupleImpl<SQLSMALLINT, V, 1, 6, 7, 8, 10, 15>(columnIdx, value, values);

  // Optional.
  GetTupleImpl<boost::optional<tstring>, V, 3, 4, 5, 12>(
   columnIdx, value, values);

  GetTupleImpl<boost::optional<SQLSMALLINT>, V, 9, 11, 13, 14, 16, 18>(
   columnIdx, value, values);

  GetTupleImpl<boost::optional<SQLINTEGER>, V, 2, 17>(
   columnIdx, value, values);

  return SERVER_SUCCESS;
 }

};

/// Row container type for SQLColumns.
typedef std::tr1::tuple<
 boost::optional<tstring>, // 0, TABLE_CAT
 boost::optional<tstring>, // 1, TABLE_SCHEM
 tstring, // 2, TABLE_NAME
 tstring, // 3, COLUMN_NAME
 SQLSMALLINT, // 4, DATA_TYPE
 tstring, // 5, TYPE_NAME
 boost::optional<SQLINTEGER>, // 6, COLUMN_SIZE
 boost::optional<SQLINTEGER>, // 7, BUFFER_LENGTH
 boost::optional<SQLSMALLINT>, // 8, DECIMAL_DIGITS
 boost::optional<SQLSMALLINT>, // 9, NUM_PREC_RADIX
 SQLSMALLINT, // 10, NULLABLE
 boost::optional<tstring>, // 11, REMARKS
 boost::optional<tstring>, // 12, COLUMN_DEF
 SQLSMALLINT, // 13, SQL_DATA_TYPE
 boost::optional<SQLSMALLINT>, // 14, SQL_DATETIME_SUB
 boost::optional<SQLINTEGER>, // 15, CHAR_OCTET_LENGTH
 SQLINTEGER, // 16, ORDINAL_POSITION
 boost::optional<tstring> // 17, IS_NULLABLE
> ColumnRowStorage;

/**
Row for SQLColumns implementation.
*/
class ColumnRow
 : public AbstractRow<ColumnRowStorage, ColumnRow>,
   public TupleStoragePolicy<AbstractRow<ColumnRowStorage, ColumnRow> >,
   public OptionalPolicy<AbstractRow<ColumnRowStorage, ColumnRow> >
{
public:

 std::size_t GetStorageSizeImpl()
 {
  return std::tr1::tuple_size<StorageType>::value;
 }

 bool IsNullImpl(std::size_t columnIdx)
 {
  switch(columnIdx)
  {
   case 2:
   case 3:
   case 4:
   case 5:
   case 10:
   case 13:
   case 16:
    return false;

   default:
    break;
  }

  {
   boost::optional<tstring> v;
   if(IsNullTupleImpl<boost::optional<tstring>, 0, 1, 11, 12, 17>(
    columnIdx, v, values))
   {
    return v == GetNull();
   }
  }

  {
   boost::optional<SQLSMALLINT> v;
   if(IsNullTupleImpl<boost::optional<SQLSMALLINT>, 8, 9, 14>(
    columnIdx, v, values))
   {
    return v == GetNull();
   }
  }

  {
   boost::optional<SQLINTEGER> v;
   if(IsNullTupleImpl<boost::optional<SQLINTEGER>, 6, 7, 15>(
    columnIdx, v, values))
   {
    return v == GetNull();
   }
  }

  // Shall not happen.
  throw ClientException("Illegal argument columnIdx.");
 }

 template<typename V>
 ServerReturn GetImpl(std::size_t columnIdx, V& value)
 {
  // Not optional.  
  GetTupleImpl<tstring, V, 2, 3, 5>(columnIdx, value, values);

  GetTupleImpl<SQLSMALLINT, V, 4, 10, 13>(columnIdx, value, values);

  GetTupleImpl<SQLINTEGER, V, 16>(columnIdx, value, values);

  // Optional.
  GetTupleImpl<boost::optional<tstring>, V, 0, 1, 11, 12, 17>(
   columnIdx, value, values);

  GetTupleImpl<boost::optional<SQLSMALLINT>, V, 8, 9, 14>(
   columnIdx, value, values);

  GetTupleImpl<boost::optional<SQLINTEGER>, V, 6, 7, 15>(
   columnIdx, value, values);

  return SERVER_SUCCESS;
 }

};

}

#endif
