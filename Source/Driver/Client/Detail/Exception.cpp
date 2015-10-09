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

#include <stdexcept>
#include <exception>

#include <utf8.h>

#include "../../../Logging/Logging.hpp"

#include "../Exception.hpp"

namespace ODBCDriver
{

ClientException::ClientException(const char* _what)
#ifdef __GNUG__
 : std::exception(), whatStr(_what)
#else
 : std::exception(_what)
#endif
{
}

TCHAR* ClientException::What()
{
 tstring error;

#ifdef UNICODE
 std::string _what(what());
 // Convert it to utf-16.
 utf8::utf8to16(_what.begin(), _what.end(), std::back_inserter(error));
#else
 error.assign(what());
#endif

 const std::size_t s = error.size() * (sizeof(TCHAR) + 1);

 if(s == 0)
 {
  tWhat.reset(new TCHAR);
  *tWhat = '\0';
 }
 else
 {
  tWhat.reset(new TCHAR[s]);
  _tcscpy(tWhat.get(), error.c_str());
 }

 return tWhat.get();
}

}
