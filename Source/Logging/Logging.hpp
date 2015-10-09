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


#ifndef _BIGOBJECT_LOGGING_
#define _BIGOBJECT_LOGGING_

#include <string>
#include <memory>
#include <iomanip>

#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#ifdef WIN32
#include <log4cplus/win32debugappender.h>
#endif
#include <log4cplus/loglevel.h>
#include <log4cplus/loggingmacros.h>

#include "../Base/Debug.hpp"
#include "../Base/TString.hpp"

#ifndef LOG_PATTERN
#define LOG_PATTERN TEXT("%D{%Y-%m-%d %H:%M:%S,%q} - %p - Thread: %t - %m%n")
#endif

#ifndef LOG_DEFAULT_IMMEDIATE_FLUSH
#ifdef _DEBUG
#define LOG_DEFAULT_IMMEDIATE_FLUSH true
#else
#define LOG_DEFAULT_IMMEDIATE_FLUSH false
#endif
#endif

#define LOG_DEBUG LOG4CPLUS_DEBUG
#define LOG_INFO LOG4CPLUS_INFO 
#define LOG_WARN LOG4CPLUS_WARN 
#define LOG_ERROR LOG4CPLUS_ERROR 
#define LOG_FATAL LOG4CPLUS_FATAL

#define LOG_DEBUG_F LOG4CPLUS_DEBUG_FMT
#define LOG_INFO_F LOG4CPLUS_INFO_FMT 
#define LOG_WARN_F LOG4CPLUS_WARN_FMT 
#define LOG_ERROR_F LOG4CPLUS_ERROR_FMT 
#define LOG_FATAL_F LOG4CPLUS_FATAL_FMT

// http://stackoverflow.com/questions/733056/
// is-there-a-way-to-get-function-name-inside-a-c-function
// http://stackoverflow.com/questions/1666802/is-there-a-class-macro-in-c
#if defined(WIN32)
#define LOG_FUNCTION_NAME TEXT(__FUNCTION__)
#define LOG_AUTO_LOGGER TEXT(__FUNCDNAME__)
#elif defined(__GNUG__)
// https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html
// These identifiers are variables, not preprocessor macros.
#ifdef UNICODE
#define LOG_FUNCTION_NAME LOG4CPLUS_STRING_TO_TSTRING(__FUNCTION__).c_str()
#define LOG_AUTO_LOGGER LOG4CPLUS_STRING_TO_TSTRING(__PRETTY_FUNCTION__).c_str()
#else
#define LOG_FUNCTION_NAME __FUNCTION__
#define LOG_AUTO_LOGGER __PRETTY_FUNCTION__
#endif
#else
#define LOG_FUNCTION_NAME TEXT(__FUNCTION__)
#define LOG_AUTO_LOGGER TEXT(__FUNCTION__)
#endif

#define LOG_DECORATED_FUNCTION_NAME LOG_AUTO_LOGGER

#define LOG_DEBUG_FUNC(logEvent) LOG4CPLUS_DEBUG(LOG_AUTO_LOGGER, logEvent)
#define LOG_INFO_FUNC(logEvent) LOG4CPLUS_INFO(LOG_AUTO_LOGGER, logEvent)
#define LOG_WARN_FUNC(logEvent) LOG4CPLUS_WARN(LOG_AUTO_LOGGER, logEvent)
#define LOG_ERROR_FUNC(logEvent) LOG4CPLUS_ERROR(LOG_AUTO_LOGGER, logEvent)
#define LOG_FATAL_FUNC(logEvent) LOG4CPLUS_FATAL(LOG_AUTO_LOGGER, logEvent)

// FMT
#define LOG_DEBUG_F_FUNC(logEvent, ...) \
 LOG4CPLUS_DEBUG_FMT(LOG_AUTO_LOGGER, logEvent, __VA_ARGS__)

#define LOG_INFO_F_FUNC(logEvent, ...) \
 LOG4CPLUS_INFO_FMT(LOG_AUTO_LOGGER, logEvent, __VA_ARGS__)

#define LOG_WARN_F_FUNC(logEvent, ...) \
 LOG4CPLUS_WARN_FMT(LOG_AUTO_LOGGER, logEvent, __VA_ARGS__)

#define LOG_DEBUG_F_FUNC(logEvent, ...) \
 LOG4CPLUS_DEBUG_FMT(LOG_AUTO_LOGGER, logEvent, __VA_ARGS__)

#define LOG_ERROR_F_FUNC(logEvent, ...) \
 LOG4CPLUS_ERROR_FMT(LOG_AUTO_LOGGER, logEvent, __VA_ARGS__)

namespace bigobject
{
namespace Logging
{

typedef log4cplus::LogLevel LogLevel;

const LogLevel LEVEL_OFF = log4cplus::OFF_LOG_LEVEL;
const LogLevel LEVEL_FATAL = log4cplus::FATAL_LOG_LEVEL;
const LogLevel LEVEL_ERROR = log4cplus::ERROR_LOG_LEVEL;
const LogLevel LEVEL_WARN = log4cplus::WARN_LOG_LEVEL;
const LogLevel LEVEL_INFO = log4cplus::INFO_LOG_LEVEL;
const LogLevel LEVEL_DEBUG = log4cplus::DEBUG_LOG_LEVEL;

typedef log4cplus::Logger Logger;

/**
Factory for Logger.
*/
struct LoggerFactory
{

 /**
 Intialize root logger.
 
 @param pattern Log pattern.
 @param immediateFlush When it is set true, output stream will be flushed 
                       after each appended event.
 @return Logger instance.
 */
 static Logger InitLogger(tstring pattern = tstring(LOG_PATTERN), 
                          bool immediateFlush = LOG_DEFAULT_IMMEDIATE_FLUSH)
 {
  log4cplus::initialize();

  log4cplus::SharedAppenderPtr appender(
   new log4cplus::ConsoleAppender(false, immediateFlush));
  appender->setName(TEXT("default"));
  appender->setLayout(std::auto_ptr<log4cplus::Layout>(
   new log4cplus::PatternLayout(pattern)));

  Logger logger = log4cplus::Logger::getRoot();
  logger.addAppender(appender);

#if defined(WIN32) // && defined(_DEBUG) 

  log4cplus::SharedAppenderPtr _appender(new log4cplus::Win32DebugAppender);
  _appender->setName(TEXT("win32"));
  _appender->setLayout(std::auto_ptr<log4cplus::PatternLayout>(
   new log4cplus::PatternLayout(pattern)));
  logger.addAppender(_appender);
    
#endif
  
#ifdef _DEBUG
  logger.setLogLevel(LEVEL_DEBUG);
#else
  logger.setLogLevel(LEVEL_INFO);
#endif

  return logger;
 }

 /**
 Clearn up logging for thread.
 */
 static void ThreadClean()
 {
#ifdef _DEBUG
  Logger logger = log4cplus::Logger::getRoot();

  LOG_INFO(logger, TEXT("LoggerFactory::ThreadClean"));
#endif

  // Do thread-specific cleanup.
  log4cplus::threadCleanup();
 }

 /**
 Clearn up logging subsystem.
 */
 static void Clean()
 {
#ifdef _DEBUG
  Logger logger = log4cplus::Logger::getRoot();

  LOG_INFO(logger, TEXT("LoggerFactory::Clean"));
#endif

  log4cplus::Logger::shutdown();

  // Do thread-specific cleanup.
  log4cplus::threadCleanup();
 }

 /**
 Get logger instance.
 
 @param name Logger name.
 @return Logger instance.
 */
 static Logger GetLogger(const tstring& name = TEXT(""))
 {
  if(name.empty())
   return log4cplus::Logger::getRoot();
  else
   return log4cplus::Logger::getInstance(name);
 }
 
 /**
 Configurate specified logger.
 
 @param name Logger name.
 @param logLevel Log level.
 @param logFile Log file name, empty if console.
 @param immediateFlush When it is set true, output stream will be flushed
                       after each appended event.
 @return Has the logger been configurated before?
 */
 static bool ConfigLogger(const tstring& name = TEXT(""), 
                          const tstring& logLevel = TEXT("INFO"), 
                          const tstring& logFile = TEXT(""),
                          bool immediateFlush = LOG_DEFAULT_IMMEDIATE_FLUSH)
 {
  Logger logger = GetLogger(name);
  bool existed = false;
    
  if(name.empty()) // Root.
  {
   if(logger.getAppender(TEXT("default")))
    existed = true;   
  }
  
  if(logger.getAppender(TEXT("config")))
   existed = true;

  if(logger.getAppender(TEXT("default")))
   logger.removeAppender(TEXT("default"));
   
  if(logger.getAppender(TEXT("config")))
   logger.removeAppender(TEXT("config"));

  if(!logFile.empty())
  {    
   log4cplus::SharedAppenderPtr appender(new log4cplus::FileAppender(
    logFile, std::ios::app, immediateFlush));
   appender->setName(TEXT("config"));
   appender->setLayout(std::auto_ptr<log4cplus::Layout>(
    new log4cplus::PatternLayout(LOG_PATTERN)));   
   logger.addAppender(appender);
  }
  else
  {
   log4cplus::SharedAppenderPtr appender(
    new log4cplus::ConsoleAppender(false, immediateFlush));
   appender->setName(TEXT("config"));
   appender->setLayout(std::auto_ptr<log4cplus::Layout>(
    new log4cplus::PatternLayout(LOG_PATTERN)));
   logger.addAppender(appender);
  }

  log4cplus::LogLevelManager& llm = log4cplus::getLogLevelManager();
  logger.setLogLevel(llm.fromString(logLevel));
  
  return existed;
 }

 /**
 Get log level string for logger.

 @param name Logger name.
 @return Log level string.
 */
 static tstring GetLogLevel(const tstring& name = TEXT(""))
 {
  Logger logger = GetLogger(name);

  log4cplus::LogLevelManager& llm = log4cplus::getLogLevelManager();
  return llm.toString(logger.getLogLevel());
 }

};

}
}

#endif
