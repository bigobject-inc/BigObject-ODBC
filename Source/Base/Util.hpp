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

#ifndef _BIGOBJECT_UTIL_
#define _BIGOBJECT_UTIL_

#include <queue>

namespace bigobject
{

/**
Traits for smart pointer (boost smart pointer, auto_ptr...etc.).
@param T Smart pointer type.
*/
template<typename T>
struct SmartPointerTraits
{
 typedef T PointerType;
 typedef typename T::element_type ValueType;
};
        
/**
Template partial specialize for smart pointer traits while T is a pointer.
@param T Type.
*/
template<typename T>
struct SmartPointerTraits<T*>
{
 typedef T* PointerType;
 typedef T ValueType;
}; 

// http://stackoverflow.com/questions/1723515/
// insert-into-an-stl-queue-using-stdcopy
template<typename T>
class QueueAdapter
{
public:

	typedef typename std::queue<T>::container_type container_type;
	typedef typename std::queue<T>::value_type value_type;
	typedef typename std::queue<T>::size_type size_type;
	typedef typename std::queue<T>::reference reference;
	typedef typename std::queue<T>::const_reference const_reference;

public:

 QueueAdapter(std::queue<T>& q) : _q(q) 
 {
 }
 
 void push_back(const T& t) 
 { 
  _q.push(t); 
 }

private:

 std::queue<T>& _q;
 
};

}

#endif
