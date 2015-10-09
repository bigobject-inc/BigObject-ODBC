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


#ifndef _BIGOBJECT_TSTRING_HPP_
#define _BIGOBJECT_TSTRING_HPP_

#include <string>

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE  // TCHAR.h
#endif
#endif

#ifdef WIN32
#include <tchar.h>

#define strdup _strdup
#else
#ifdef UNICODE
#ifndef __SQLTYPES_H
#define TCHAR   wchar_t
#endif

#define _fputts    fputws
#ifdef _CRT_NON_CONFORMING_SWPRINTFS
#define _stprintf  swprintf
#endif
#define _sntprintf swprintf 
#define _tcslen    wcslen
#define _tfopen    _wfopen
#define _vstprintf vswprintf
#define _tcsrchr   wcsrchr 
#define _tcsdup    wcsdup
#define _tcscpy    wcscpy
#define _tcscmp    wcscmp
#define _tcsstr    wcsstr
#define _tcsncpy   wcsncpy
#else
// Dirty hack to avoid conflict, in sqltypes.h TCHAR defined as typedef, we
// don't have a good way to detect typedef this time.
#ifndef __SQLTYPES_H
#define TCHAR   char
#endif

#define _fputts    fputs
#define _stprintf  sprintf
#define _sntprintf snprintf
#define _tcslen    strlen
#define _tfopen    fopen
#define _vstprintf vsprintf
#define _tcsrchr   strrchr 
#define _tcsdup    strdup
#define _tcscpy    strcpy
#define _tcscmp    strcmp
#define _tcsstr    strstr
#define _tcsncpy   strncpy
#endif
#endif

#ifndef TEXT
#ifdef UNICODE
#define _TEXT_HELPER(x) L ## x
#define TEXT(x)         _TEXT_HELPER(x)
#else
#define TEXT(x)      x
#endif
#endif

#ifdef UNICODE
typedef std::wstring tstring;
typedef std::wstringstream tstringstream;
typedef std::wistringstream itstringstream;
typedef std::wostringstream otstringstream;
typedef std::wifstream tifstream;
#else
typedef std::string tstring;
typedef std::stringstream tstringstream;
typedef std::istringstream itstringstream;
typedef std::ostringstream otstringstream;
typedef std::ifstream tifstream;
#endif

#endif 
