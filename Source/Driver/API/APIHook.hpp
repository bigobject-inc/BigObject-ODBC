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

#ifndef _ODBC_DRIVER_API_HOOK_
#define _ODBC_DRIVER_API_HOOK_

#include <vector>

#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/make_vector.hpp>

#include <boost/any.hpp>

#include "../../Base/Platform.hpp"
#include "../../Base/CRTP.hpp"

#include "../../Logging/Logging.hpp"

namespace ODBCDriver
{

/**
SQLRETURN wrapper to provide return value and determine API execution path 
after hook called.
*/
class APIHookReturn
{
public:

 /**
 How to control API call after hook called. BREAK if you want immediately 
 return.
 */
 enum ExecutionType
 {
  BREAK,
  CONTINUE
 };

public:

 /**
 Constructor.

 @param _ret Produced SQLRETURN.
 @param _execType ExecutionType.
 */
 APIHookReturn(SQLRETURN _ret, ExecutionType _execType)
  : ret(_ret), execType(_execType)
 {
 }

 /**
 Get SQLRETURN value.

 @return SQLRETURN.
 */
 SQLRETURN Value() const
 {
  return ret;
 }

 /**
 Is BREAK?

 @return True if ExecutionType is BREAK.
 */
 bool IsBreak() const
 {
  return execType == BREAK;
 }

protected:

 /// SQLRETURN value.
 SQLRETURN ret;

 /// ExecutionType.
 ExecutionType execType;
};

/**
Empty tag for all APIHook.
*/
struct APIHookBase
{
};

/**
Abstract API hook, it provides common intercept operation methods for ODBC
driver API.

@param API API tag.
@param _Derived APIHook.
*/
template<typename API, typename _Derived = boost::mpl::empty_base>
class AbstractAPIHook :
 public bigobject::CRTP<_Derived, AbstractAPIHook<API> >,
 public APIHookBase
{
public:

 /// List type of API arguments.
 typedef std::vector<boost::any> ArgsList;

private:

 /**
 Internal struct for initialization.
 */
 struct _Mapper
 {
  ArgsList& args;
  
  _Mapper(ArgsList& _args) : args(_args)
  {
  }

  template<typename T>
  void operator()(T const& x) const
  {
   args.push_back(x);
  }

 };

public:

 /**
 Constructor.

 @param v Generated boost::fusion vector for API arguments.
 */
 template<typename T>
 explicit AbstractAPIHook(T v)
 {
  logger = bigobject::Logging::LoggerFactory::GetLogger(TEXT("APIHook"));

  _Mapper mapper(args);
  boost::fusion::for_each(v, mapper);
 }

 /**
 Pre-call API. This method will be called before main API implementation 
 called (after sanity check).

 @return APIHookReturn.
 */
 APIHookReturn PreCall()
 {
  return APIHookReturn(SQL_SUCCESS, APIHookReturn::CONTINUE);
 }

 /**
 Post-call API. This method will be called after main API implementation
 called.

 @param r Original SQLRETURN result.
 @return APIHookReturn.
 */
 APIHookReturn PostCall(SQLRETURN r)
 {
  return APIHookReturn(r, APIHookReturn::CONTINUE);
 }

protected:

 /**
 Default constructor. Only called by dummy APIHook.
 Do not initialize anything for avoiding performance wasting.
 */
 explicit AbstractAPIHook()
 {
 }

 /**
 Get API argument by given type T and sequence n.

 @param n n-th arguments.
 @return Argument reference.
 */
 template<typename T>
 T& Get(std::size_t n)
 {
  return boost::any_cast<T&>(args[n]);
 }

protected:

 /// Logger.
 bigobject::Logging::Logger logger;

 /// List of API arguments.
 ArgsList args;

};

/**
Dummy hook, all hook should use template specialization for hooked API.

@param API API tag.
*/
template<typename API>
struct APIHook : public AbstractAPIHook<API, APIHook<API> >
{
 template<typename T>
 explicit APIHook(const T& v) : AbstractAPIHook<API, APIHook<API> >()
 {
 }
};

/**********************************************
* API tags for Hook.
***********************************************/

/**
Empty tag for all API.
*/
struct API
{
};

#define API_HOOK_API_TAG(API_FUNC) \
struct API_FUNC : public API       \
{                                  \
}

API_HOOK_API_TAG(SQLAllocConnect);
API_HOOK_API_TAG(SQLAllocEnv);
API_HOOK_API_TAG(SQLAllocStmt);
API_HOOK_API_TAG(SQLAllocHandle);
API_HOOK_API_TAG(SQLFreeConnect);
API_HOOK_API_TAG(SQLFreeEnv);
API_HOOK_API_TAG(SQLFreeStmt);
API_HOOK_API_TAG(SQLBindCol);
API_HOOK_API_TAG(SQLCancel);
API_HOOK_API_TAG(SQLConnect);
API_HOOK_API_TAG(SQLDescribeCol);
API_HOOK_API_TAG(SQLDisconnect);
API_HOOK_API_TAG(SQLExecDirect);
API_HOOK_API_TAG(SQLExecute);
API_HOOK_API_TAG(SQLExtendedFetch);
API_HOOK_API_TAG(SQLFetch);
API_HOOK_API_TAG(SQLGetCursorName);
API_HOOK_API_TAG(SQLNumResultCols);
API_HOOK_API_TAG(SQLPrepare);
API_HOOK_API_TAG(SQLRowCount);
API_HOOK_API_TAG(SQLSetCursorName);
API_HOOK_API_TAG(SQLColumns);
API_HOOK_API_TAG(SQLDriverConnect);
API_HOOK_API_TAG(SQLGetData);
API_HOOK_API_TAG(SQLGetInfo);
API_HOOK_API_TAG(SQLGetTypeInfo);
API_HOOK_API_TAG(SQLParamData);
API_HOOK_API_TAG(SQLPutData);
API_HOOK_API_TAG(SQLStatistics);
API_HOOK_API_TAG(SQLTables);
API_HOOK_API_TAG(SQLBrowseConnect);
API_HOOK_API_TAG(SQLColumnPrivileges);
API_HOOK_API_TAG(SQLDescribeParam);
API_HOOK_API_TAG(SQLForeignKeys);
API_HOOK_API_TAG(SQLMoreResults);
API_HOOK_API_TAG(SQLNativeSql);
API_HOOK_API_TAG(SQLNumParams);
API_HOOK_API_TAG(SQLPrimaryKeys);
API_HOOK_API_TAG(SQLProcedureColumns);
API_HOOK_API_TAG(SQLProcedures);
API_HOOK_API_TAG(SQLSetPos);
API_HOOK_API_TAG(SQLTablePrivileges);
API_HOOK_API_TAG(SQLBindParameter);
API_HOOK_API_TAG(SQLCloseCursor);
API_HOOK_API_TAG(SQLColAttribute);
API_HOOK_API_TAG(SQLCopyDesc);
API_HOOK_API_TAG(SQLEndTran);
API_HOOK_API_TAG(SQLFetchScroll);
API_HOOK_API_TAG(SQLFreeHandle);
API_HOOK_API_TAG(SQLGetConnectAttr);
API_HOOK_API_TAG(SQLGetDescField);
API_HOOK_API_TAG(SQLGetDescRec);
API_HOOK_API_TAG(SQLGetDiagField);
API_HOOK_API_TAG(SQLGetDiagRec);
API_HOOK_API_TAG(SQLGetEnvAttr);
API_HOOK_API_TAG(SQLGetStmtAttr);
API_HOOK_API_TAG(SQLSetConnectAttr);
API_HOOK_API_TAG(SQLSetDescField);
API_HOOK_API_TAG(SQLSetDescRec);
API_HOOK_API_TAG(SQLSetEnvAttr);
API_HOOK_API_TAG(SQLSetStmtAttr);
API_HOOK_API_TAG(SQLBulkOperations);
API_HOOK_API_TAG(SQLSpecialColumns);
API_HOOK_API_TAG(SQLSetStmtOption);
API_HOOK_API_TAG(SQLGetStmtOption);
API_HOOK_API_TAG(SQLSetConnectOption);
API_HOOK_API_TAG(SQLGetConnectOption);
API_HOOK_API_TAG(SQLColAttributes);

/***********************************************/

/// Helper macro for driver implementation to make a APIHook class.
#define API_HOOK_CLASS(API)                                                  \
template<>                                                                   \
struct APIHook<API> : public AbstractAPIHook<API, APIHook<API> > 

/// Helper macro for driver implementation to insert default APIHook behavior.
#define API_HOOK_CLASS_DEFAULT_IMPL(API)                                     \
 template<typename T>                                                        \
 explicit APIHook(T& v) : AbstractAPIHook<API, APIHook<API> >(v)             \
 {                                                                           \
 }

/// Helper macro for API to attach hook.
#define API_HOOK_ENTRY(API, ...)                                             \
 ODBCDriver::APIHook<ODBCDriver::API> __apihook(                             \
  boost::fusion::make_vector(__VA_ARGS__));                                  \
 ODBCDriver::APIHookReturn __hookRet = __apihook.PreCall();                  \
 if(__hookRet.IsBreak())                                                     \
  return __hookRet.Value();

/// Helper macro for API to attach hook before return.
#define API_HOOK_RETURN(ret)                                                 \
 {                                                                           \
  SQLRETURN __ret = ret;                                                     \
  __hookRet = __apihook.PostCall(__ret);                                     \
  if(__hookRet.IsBreak())                                                    \
   return __hookRet.Value();                                                 \
  return __ret;                                                              \
 }
}

#endif
