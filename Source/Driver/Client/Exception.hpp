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

#ifndef _BIGOBJECT_ODBC_CLIENT_EXCEPTION_
#define _BIGOBJECT_ODBC_CLIENT_EXCEPTION_

#include <stdexcept>
#include <exception>
#include <string>

#include <boost/shared_ptr.hpp>

#include "../../Base/TString.hpp"

namespace ODBCDriver
{

/**
Client exception contains error messages raised from server side.
*/
class ClientException : public std::exception
{
public:
 
 /**
 Constructor.

 @param _what Exception message.
 */
 ClientException(const char* _what);

#ifdef __GNUG__

 /**
 Destructor.
 */
 virtual ~ClientException() throw()
 {
 }

 virtual const char* what() const throw()
 {
  return whatStr.c_str();
 }

#endif

 /**
 What TCHAR* version.

 @return The same as what() but TCHAR*.
 */
 TCHAR* What();

private:

 /// Memory holder for converted TCHAR* what.
 boost::shared_ptr<TCHAR> tWhat;

#ifdef __GNUG__
 /// Standard std::exception without what member, we define one.
 std::string whatStr;
#endif

};

}

#endif
