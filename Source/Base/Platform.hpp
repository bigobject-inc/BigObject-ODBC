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


#ifndef _BIGOBJECT_PLATFORM_
#define _BIGOBJECT_PLATFORM_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#ifdef WIN32
#define PATH_SEPARATOR   "\\"
#else
#define PATH_SEPARATOR   "/"
#endif

#ifndef EXPORT
#if defined(WIN32) 
#define EXPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE
#define EXP_STL
#else
#define EXPORT __attribute__ ((visibility("default")))
#define EXPIMP_TEMPLATE
#endif
#endif

#if defined(_DEBUG) || defined(_TEST)
#define TEST_EXPORT EXPORT
#else
#define TEST_EXPORT
#endif

/// Type define for pointer to string.
typedef boost::shared_ptr<std::string> StringPtr;

typedef std::vector<StringPtr> StringList;

typedef boost::shared_ptr<StringList> StringListPtr;

#endif
