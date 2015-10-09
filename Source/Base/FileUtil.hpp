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


#ifndef _BIGOBJECT_FILE_UTIL_
#define _BIGOBJECT_FILE_UTIL_

#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>

namespace bigobject
{

/**
Read content from a file to shared_array.

@param file File.
@param content A shared array class to stores file content.
@return File size.
*/
int ReadFile(const std::string& file, boost::shared_array<char>& content)
{
 std::ifstream f(file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
 if(f.is_open())
 {
  int size = (int)f.tellg();
  content.reset(new char[size + 1]);
  f.seekg(0, std::ios::beg);
  f.read(content.get(), size);
  content[size] = 0;
  f.close();

  return size;
 }
 
 return -1;
}

/**
A utility function to read result content from a file.

@return File content.
*/
std::string ReadFile(std::string file)
{
 std::ifstream f(file.c_str(), std::ios::in | std::ios::binary | 
                               std::ios::ate);
 if(f.is_open())
 {
  int size = (int)f.tellg();
  boost::scoped_array<char> memblock(new char[size + 1]);
  f.seekg(0, std::ios::beg);
  f.read(memblock.get(), size);
  memblock[size] = 0;
  f.close();

  return std::string(memblock.get(), size);
 }

 return "";
}

/**
A utility function to write result content to a file.

@param file File name.
@param data File content.
@return True if succeeded.
*/
bool WriteFile(std::string file, std::string& data)
{
 std::ofstream f(file.c_str(),  std::ios::out | std::ios::binary);
 if(!f.is_open())
  return false;

 f.write(data.data(), data.size());
 f.flush();
 f.close();

 return true;
}

}

#endif
