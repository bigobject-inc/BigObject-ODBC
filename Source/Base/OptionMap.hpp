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


#ifndef _BIGOBJECT_OPIONMAP_
#define _BIGOBJECT_OPIONMAP_

#include <string>
#include <vector>

#include <boost/any.hpp>

#include <boost/unordered_map.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/distance.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include <boost/preprocessor/repetition/enum_params.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <boost/algorithm/string/case_conv.hpp>

namespace bigobject
{

namespace Option
{

/**
Abstract option base type.

@param _Name Option name.
@param Default Default value.
*/
template<
 typename _Name, 
 typename Default, 
 typename Flag = typename Default::tag
>
struct _Option
{
 /// Option name, must be boost::mpl::string.
 typedef _Name Name;
 /// Value type.
 typedef typename Default::value_type Type;

 static inline std::string GetName()
 {
  return boost::mpl::c_str<Name>::value;
 }

 /// Default value.
 //BOOST_STATIC_CONSTANT(type, value = typename Default::value);
 static inline const Type& Value()
 {
  return Default::value;
 }
};

template<typename _Name, typename Default>
struct _Option<_Name, Default, boost::mpl::string<>::tag>
{
 typedef _Name Name;
 typedef std::string Type;

 static inline std::string GetName()
 {
  return boost::mpl::c_str<Name>::value;
 }

 static inline const Type Value()
 {
  return boost::mpl::c_str<Default>::value;
 }
};

// Reference: http://lists.boost.org/boost-users/2008/09/40236.php
template<class T, class Tuple>
struct TuplePushFront;

template<class T, BOOST_PP_ENUM_PARAMS(10, class T)>
struct TuplePushFront<T, boost::tuple<BOOST_PP_ENUM_PARAMS(10, T)> > 
{
 typedef boost::tuple<typename T::Type, BOOST_PP_ENUM_PARAMS(9, T)> type;
};

template<class Sequence>
struct _OptionsTuple : public boost::mpl::reverse_copy<
 Sequence, boost::mpl::inserter<boost::tuple<>,
 TuplePushFront<boost::mpl::_2, boost::mpl::_1> >
 >::type
{
 typedef Sequence Options;
};

template<typename S, typename Iter, bool IsEnd, int StartIndex = 0>
struct Apply
{
 static const bool end = boost::is_same<
  typename boost::mpl::end<S>::type,
  typename boost::mpl::next<Iter>::type
 >::value ? true : false;

 template<typename OptionsTuple, typename Function>
 void operator()(OptionsTuple& options, Function func)
 {
  func(boost::tuples::get<StartIndex>(options),
       typename boost::mpl::deref<Iter>::type());
  Apply<S, typename boost::mpl::next<Iter>::type, end, StartIndex + 1>()(
   options, func);
 }
};

template<typename S, typename Iter, int Index>
struct Apply<S, Iter, true, Index>
{
 template<typename OptionsTuple, typename Function>
 void operator()(OptionsTuple&, Function)
 {   
 }
};

/**
Apply function to every value in options tuple with its type infomation.

@param options OptionsTuple.
@param func Function.
*/
template<typename OptionsTuple, typename Function>
void ForEach(OptionsTuple& options, Function func)
{
 typedef typename OptionsTuple::Options OptionTypes;

 Apply<OptionTypes, typename boost::mpl::begin<OptionTypes>::type, false>()(
  options, func);
}

/**
Get option from options tuple => Get<Option>(options).

@param options Options tuple.
@return Value of option.
*/
template<typename OptionsTuple, typename Opt>
typename Opt::Type& Get(OptionsTuple& options)
{
 typedef typename OptionsTuple::Options OptionTypes;

 return boost::tuples::get<boost::mpl::distance<
  typename boost::mpl::begin<OptionTypes>::type,
  typename boost::mpl::find<OptionTypes, Opt>::type>::value>(options);
}

} // end namespace Option.

/// Option map.
typedef boost::unordered_map<std::string, boost::any> OptionMap;

/**
Getter to get parsed options from option map.
*/
template<typename T>
class OptionGetter
{
public:

 typedef T OptionsTuple;

public:

 /**
 Constructor.
 */
 OptionGetter() : isSet(false)
 {
  Init();
 }

 /**
 Constructor.
  
 @param options OptionMap.
 */
 OptionGetter(const OptionMap& options) : isSet(false)
 {    
  Init();
  Set(options);
 }
 
private:

 /**
 Functor to get option from OptionMap to options tuple.
 */
 struct GetOption
 {
  const OptionMap& options;
  bool result;

  GetOption(const OptionMap& _options, bool _result) 
   : options(_options), result(result)
  {
  }

  template<typename Value, typename Type>
  void operator()(Value& value, Type x)
  {   
   OptionMap::const_iterator iter;
  
   try
   {
    iter = options.find(boost::mpl::c_str<typename Type::Name>::value);
    if(iter != options.end())
     value = boost::any_cast<Value>(iter->second);  
    else
     result = false;
   }
   catch(...)
   {
    result = false;
   }
  }
 };

public:

 /**
 Get option.

 @param Opt Option type.
 @return Option value.
 */
 template<typename Opt>
 typename Opt::Type& Get() 
 {
  return Option::Get<OptionsTuple, Opt>(options);
 }

 /**
 Set options from option map.
  
 @param options OptionMap.
 */
 bool Set(const OptionMap& options)
 {
  bool result = true;

  Option::ForEach(this->options, GetOption(options, result));  
  isSet = true;

  return result;
 }

 /**
 Is this OptionGetter ok to get real options (not default value).

 @return True if OptionGetter ok.
 */
 inline bool OK() const
 {
  return isSet;
 }
 
private:

 /**
 Functor to initialize options tuple with option default values.
 */
 struct InitValue
 {
  template<typename Value, typename Type>
  void operator()(Value& value, Type x)
  {   
   value = Type::Value();
  }
 };

 /**
 Initailize default option values.
 */
 void Init()
 {
  isSet = false;
  Option::ForEach(options, InitValue());
 }

private:

 /// Options tuple as its option value storage.
 OptionsTuple options;

 /// Is option map set?
 bool isSet;

};

}

#endif
