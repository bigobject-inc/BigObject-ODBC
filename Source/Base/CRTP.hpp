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


#ifndef _BIGOBJECT_CRTP_
#define _BIGOBJECT_CRTP_

#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/if.hpp>

#include <boost/type_traits/is_same.hpp>

namespace bigobject
{

/**
The base class for classes which implements Curiously Recurring Template 
Pattern. 
http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern

@param Derived Itself's derived class.
@param Itself The subclass that inherits CRTP.
*/
template<typename Derived, typename Itself>
class CRTP : public boost::mpl::empty_base
{
protected:

 /// Define correct implementation type.
 //  We can't use boost::is_empty here, 
 //  because it can not be used with incomplete types (Derived).
 typedef typename boost::mpl::if_<
  boost::is_same<Derived, boost::mpl::empty_base>, Itself, Derived>::type 
  ThisType;

 /**
 Helper function to cast this to proper type.

 @param T Type.
 @return Type&.
 */
 template<typename T>
 inline T& Type() const 
 {
  return const_cast<T&>(static_cast<const T&>(*this));
 }

 /**
 Get derived implementation type if it is not empty_base.
 So, if we use this class directly, it will return *this.

 @return Implementation type reference or this.
 */
 inline ThisType& This() const 
 {
  return Type<ThisType>();
 }

 /**
 Get derived implementation reference.

 @return Derived reference.
 */
 inline Derived& Impl() const 
 {
  return Type<Derived>();
 }
};

}

#endif
