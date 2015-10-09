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


#ifndef _BIGOBJECT_UTF8_
#define _BIGOBJECT_UTF8_

#include <string>

#include <utf8.h>

#include "Platform.hpp"

/* 
The "white spaces" in UTF-8 can be various definition, simplest one:
http://www.bogofilter.org/pipermail/bogofilter/2003-March/001889.html

There is a detailed comparison between Java's implementation and neutral 
UNICODE standard from Google Guava source code:
http://code.google.com/p/guava-libraries/source/browse/guava/
src/com/google/common/base/CharMatcher.java
http://spreadsheets.google.com/pub?key=pd8dAQyHbdewRsnE5x5GzKQ

The Google Guava's implementation is UNICODE 5.0 Language Neutral definition,
which is almost the same as Python's UNICODE implementation but 0x1C ~ 0x1F.
*/

// 0x0009 ~ 0x000D
#define UNICODE_HORIZONTAL_TABULATION     0x0009
#define UNICODE_LINE_FEED                 0x000A
#define UNICODE_VERTICAL_TABULATION       0x000B
#define UNICODE_FORM_FEED                 0x000C
#define UNICODE_CARRIAGE_RETURN           0x000D

// 0x001C ~ 0x0020
#define UNICODE_FILE_SEPARATOR            0x001C
#define UNICODE_GROUP_SEPARATOR           0x001D
#define UNICODE_RECORD_SEPARATOR          0x001E
#define UNICODE_UNIT_SEPARATOR            0x001F
#define UNICODE_SPACE                     0x0020 

// http://www.fileformat.info/info/unicode/char/0085/index.htm
#define UNICODE_NEXT_LINE                 0x0085

// http://www.fileformat.info/info/unicode/char/00A0/index.htm
#define UNICODE_NO_BREAK_SPACE            0x00A0

// http://www.fileformat.info/info/unicode/char/1680/index.htm
#define UNICODE_OGHAM_SPACE_MARK          0x1680

// http://www.fileformat.info/info/unicode/char/180E/index.htm
#define UNICODE_MONGOLIAN_VOWEL_SEPARATOR 0x180E

// 0x2000 ~ 0x200F
// http://www.fileformat.info/info/unicode/char/2000/index.htm
#define UNICODE_EN_QUAD                   0x2000
// http://www.fileformat.info/info/unicode/char/2001/index.htm
#define UNICODE_EM_QUAD                   0x2001
// http://www.fileformat.info/info/unicode/char/2002/index.htm
#define UNICODE_EN_SPACE                  0x2002
// http://www.fileformat.info/info/unicode/char/2003/index.htm
#define UNICODE_EM_SPACE                  0x2003
// http://www.fileformat.info/info/unicode/char/2004/index.htm
#define UNICODE_THREE_PER_EM_SPACE        0x2004
// http://www.fileformat.info/info/unicode/char/2005/index.htm
#define UNICODE_FOUR_PER_EM_SPACE         0x2005
// http://www.fileformat.info/info/unicode/char/2006/index.htm
#define UNICODE_SIX_PER_EM_SPACE          0x2006
// http://www.fileformat.info/info/unicode/char/2007/index.htm
#define UNICODE_FIGURE_SPACE              0x2007
// http://www.fileformat.info/info/unicode/char/2008/index.htm
#define UNICODE_PUNCTUATION_SPACE         0x2008
// http://www.fileformat.info/info/unicode/char/2009/index.htm
#define UNICODE_THIN_SPACE                0x2009
// http://www.fileformat.info/info/unicode/char/200A/index.htm
#define UNICODE_HAIR_SPACE                0x200A
// http://www.fileformat.info/info/unicode/char/200B/index.htm
#define UNICODE_ZERO_WIDTH_SPACE          0x200B
// http://www.fileformat.info/info/unicode/char/200C/index.htm
#define UNICODE_ZERO_WIDTH_NON_JOINER     0x200C
// http://www.fileformat.info/info/unicode/char/200D/index.htm
#define UNICODE_ZERO_WIDTH_JOINER         0x200D
// http://www.fileformat.info/info/unicode/char/200E/index.htm
#define UNICODE_LEFT_TO_RIGHT_MARK        0x200E
// http://www.fileformat.info/info/unicode/char/200F/index.htm
#define UNICODE_RIGHT_TO_LEFT_MARK        0x200F

// UNICODE_SPACE_SEPARATOR:
// http://www.fileformat.info/info/unicode/category/Zs/list.htm

// 0x2028 ~ 0x2029
#define UNICODE_LINE_SEPARATOR            0x2028
#define UNICODE_PARAGRAPH_SEPARATOR       0x2029

// http://www.fileformat.info/info/unicode/char/202F/index.htm
#define UNICODE_NARROW_NO_BREAK_SPACE     0x202F

// http://www.fileformat.info/info/unicode/char/205F/index.htm
#define UNICODE_MEDIUM_MATHEMATICAL_SPACE 0x205F

// http://www.fileformat.info/info/unicode/char/3000/index.htm
#define UNICODE_IDEOGRAPHIC_SPACE         0x3000

namespace bigobject
{

/**
UTF-8 utility class.
*/
struct UTF8
{
 
 enum
 {
  TRIM_LEFT,
  TRIM_RIGHT,
  TRIM_BOTH
 };

 /**
 Trim and copy result string in UTF-8 way.
 The implementation is refered to 
 http://hg.python.org/cpython/file/5395f96588d4/Objects/unicodeobject.c#l6984
 the Python unicode.strip source code.

 @param s Raw string.
 @param type Trim type.
 @return Result string.
 */
 static std::string TrimCopy(const std::string& s, int type = TRIM_BOTH)
 {
  std::string::const_iterator sIter = s.begin();
  std::string::const_iterator sIterEnd = s.end();

  utf8::iterator<std::string::const_iterator> src(sIter, sIter, sIterEnd);
  utf8::iterator<std::string::const_iterator> end(sIterEnd, sIter, sIterEnd);

  std::size_t len = utf8::distance(sIter, sIterEnd);
  std::size_t i = 0;

  if(type != TRIM_RIGHT) 
  {
   while(i < len && IsPythonWhiteSpace(*src)) 
   {
    i++;
    ++src;
   }
  }

  std::size_t j = len;
  if(type != TRIM_LEFT) 
  {
   do 
   {
    j--;
    --end;
   } while (j >= i && IsPythonWhiteSpace(*end));

   j++;
   ++end;
  }

  if(i == 0 && j == len) 
  {
   return std::string(s);
  }
  else
  {
   std::size_t si = (std::size_t)(src.base() - sIter);
   std::size_t sj = (std::size_t)(end.base() - sIter);

   return s.substr(si, sj - si);
  }
 }

 /**
 The Java Character.isWhiteSpace simulation.
 Please note that we do not follow Java SDK source code to implement it,
 we only follow above link which mentioned UTF-8 white spaces, and more
 importantly, Java Character.isWhiteSpace SDK comment:
 http://docs.oracle.com/javase/6/docs/api/java/lang/Character.html
 #isWhitespace%28int%29

 @param ch Character code, it might be a UNICODE not only char byte.
 @return True if this character is white space.
 */
 static bool IsJavaWhiteSpace(int ch)
 {
  switch(ch) 
  {
   // 0x0009 ~ 0x000D
   case UNICODE_HORIZONTAL_TABULATION:
   case UNICODE_LINE_FEED:
   case UNICODE_VERTICAL_TABULATION:
   case UNICODE_FORM_FEED:
   case UNICODE_CARRIAGE_RETURN:
   // 0x001C ~ 0x0020
   case UNICODE_FILE_SEPARATOR:
   case UNICODE_GROUP_SEPARATOR:
   case UNICODE_RECORD_SEPARATOR:
   case UNICODE_UNIT_SEPARATOR:
   case UNICODE_SPACE:
   // SPACE_SEPARATOR
   case UNICODE_OGHAM_SPACE_MARK:
   case UNICODE_MONGOLIAN_VOWEL_SEPARATOR:
   // 0x2000 ~ 0x200B (but not 0x2007)
   case UNICODE_EN_QUAD:
   case UNICODE_EM_QUAD:
   case UNICODE_EN_SPACE:
   case UNICODE_EM_SPACE:
   case UNICODE_THREE_PER_EM_SPACE:
   case UNICODE_FOUR_PER_EM_SPACE:
   case UNICODE_SIX_PER_EM_SPACE:
   case UNICODE_PUNCTUATION_SPACE:
   case UNICODE_THIN_SPACE:
   case UNICODE_HAIR_SPACE:
   case UNICODE_ZERO_WIDTH_SPACE:
   // LINE_SEPARATOR, PARAGRAPH_SEPARATOR
   case UNICODE_LINE_SEPARATOR:
   case UNICODE_PARAGRAPH_SEPARATOR:
   // SPACE_SEPARATOR
   case UNICODE_MEDIUM_MATHEMATICAL_SPACE:
   case UNICODE_IDEOGRAPHIC_SPACE:
    return true;
  }
    
  return false;
 }

 /**
 Is all the content of string is white space (in Java)?
 We use UTF-8 as unit to check that it is white space or not.

 @param sb String.
 @return True if all the content of string is white space.
 */
 static bool IsJavaWhiteSpace(const std::string& sb)
 {
  utf8::iterator<std::string::const_iterator> 
   src(sb.begin(), sb.begin(), sb.end());

  utf8::iterator<std::string::const_iterator> 
   end(sb.end(), sb.begin(), sb.end());

  for(; src != end; ++src)
  {
   if(!IsJavaWhiteSpace(*src))
    return false;
  }

  return true;
 }

 /**
 The Python unicode.isspace simulation.
 We follow the Python SDK implementation:
 http://hg.python.org/cpython/file/5395f96588d4/Objects/unicodeobject.c#l6719
 http://hg.python.org/cpython/file/5395f96588d4/Objects/unicodetype_db.h#l3274

 @param ch Character code, it might be a UNICODE not only char byte.
 @return True if this character is white space.
 */
 static bool IsPythonWhiteSpace(int ch)
 {
  // http://hg.python.org/cpython/file/5395f96588d4/Objects/unicodeobject.c
  /* Fast detection of the most frequent whitespace characters */
  static const unsigned char _Py_ascii_whitespace[] = {
   0, 0, 0, 0, 0, 0, 0, 0,
   /*     case 0x0009: * CHARACTER TABULATION */
   /*     case 0x000A: * LINE FEED */
   /*     case 0x000B: * LINE TABULATION */
   /*     case 0x000C: * FORM FEED */
   /*     case 0x000D: * CARRIAGE RETURN */
   0, 1, 1, 1, 1, 1, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   /*     case 0x001C: * FILE SEPARATOR */
   /*     case 0x001D: * GROUP SEPARATOR */
   /*     case 0x001E: * RECORD SEPARATOR */
   /*     case 0x001F: * UNIT SEPARATOR */
   0, 0, 0, 0, 1, 1, 1, 1,
   /*     case 0x0020: * SPACE */
   1, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,

   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0
  };

  if(ch < 128U)
   return (bool)_Py_ascii_whitespace[ch];

  switch(ch) 
  {
   // 0x0009 ~ 0x000D
   /*
   case UNICODE_HORIZONTAL_TABULATION:
   case UNICODE_LINE_FEED:
   case UNICODE_VERTICAL_TABULATION:
   case UNICODE_FORM_FEED:
   case UNICODE_CARRIAGE_RETURN:
   // 0x001C ~ 0x0020
   case UNICODE_FILE_SEPARATOR:
   case UNICODE_GROUP_SEPARATOR:
   case UNICODE_RECORD_SEPARATOR:
   case UNICODE_UNIT_SEPARATOR:
   case UNICODE_SPACE:
   */
   // 0x0085
   case UNICODE_NEXT_LINE:
   // SPACE_SEPARATOR
   case UNICODE_NO_BREAK_SPACE:
   case UNICODE_OGHAM_SPACE_MARK:
   case UNICODE_MONGOLIAN_VOWEL_SEPARATOR:
   // 0x2000 ~ 0x200A
   case UNICODE_EN_QUAD:
   case UNICODE_EM_QUAD:
   case UNICODE_EN_SPACE:
   case UNICODE_EM_SPACE:
   case UNICODE_THREE_PER_EM_SPACE:
   case UNICODE_FOUR_PER_EM_SPACE:
   case UNICODE_SIX_PER_EM_SPACE:
   case UNICODE_FIGURE_SPACE:
   case UNICODE_PUNCTUATION_SPACE:
   case UNICODE_THIN_SPACE:
   case UNICODE_HAIR_SPACE:
   // LINE_SEPARATOR, PARAGRAPH_SEPARATOR
   case UNICODE_LINE_SEPARATOR:
   case UNICODE_PARAGRAPH_SEPARATOR:
   // SPACE_SEPARATOR
   case UNICODE_NARROW_NO_BREAK_SPACE:
   case UNICODE_MEDIUM_MATHEMATICAL_SPACE:
   case UNICODE_IDEOGRAPHIC_SPACE:
    return true;
  }
    
  return false;
 }

 /**
 Is all the content of string is white space (in Python)?
 We use UTF-8 as unit to check that it is white space or not.

 @param sb String.
 @return True if all the content of string is white space.
 */
 static bool IsPythonWhiteSpace(const std::string& sb)
 {
  utf8::iterator<std::string::const_iterator> 
   src(sb.begin(), sb.begin(), sb.end());

  utf8::iterator<std::string::const_iterator> 
   end(sb.end(), sb.begin(), sb.end());

  for(; src != end; ++src)
  {
   if(!IsPythonWhiteSpace(*src))
    return false;
  }

  return true;
 }

};

}

#endif
