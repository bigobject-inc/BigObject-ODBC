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


#include <boost/algorithm/string.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/function.hpp>

#include "Driver.hpp"

#ifdef WIN32

#include "../Build/Win32/Resource.h"

#define BUFFERSIZE 256

#define INITFILE    TEXT("ODBC.INI")
#define INSTINIFILE TEXT("ODBCINST.INI")

// #FIXME
#define DRIVER_DEFAULT_LOCATION TEXT("C:\\Program Files\\MarcodataLab\\BigObjectODBC\\BigObjectODBC.dll")

/**
Structure to contain parameters for dialog.
*/
struct DSNParam
{
 TCHAR currentDSN[BUFFERSIZE];
 LPCTSTR lpszAttributes;
};

static int GetValueFromODBCINI(const TCHAR* section, 
                               const TCHAR* key, const TCHAR* defaultValue,
                               TCHAR* buffer, int bufferSize,
                               const TCHAR* initFileName)
{
 int read = SQLGetPrivateProfileString(section, key, defaultValue, buffer, 
                                       bufferSize, initFileName);

 if(read == 0 && buffer[0] == TEXT('\0') && defaultValue)
  _tcscpy(buffer, defaultValue);

 return read;
}

static BOOL SetValueInODBCINI(const TCHAR* section, 
                              const TCHAR* key, const TCHAR* newValue,
                              const TCHAR* initFileName)
{
 return SQLWritePrivateProfileString(section, key, newValue, initFileName);
}

static BOOL AddDSN(const TCHAR* dsnName)
{
 return SQLWritePrivateProfileString(TEXT("ODBC Data Sources"), 
                                     dsnName, TEXT(ODBC_DRIVER_NAME), 
                                     INITFILE);
}

static BOOL RemoveDSN(const TCHAR* dsnName)
{
 BOOL temp = true;
 temp = SQLWritePrivateProfileString(dsnName, NULL, NULL, INITFILE);
 return (SQLWritePrivateProfileString(TEXT("ODBC Data Sources"), 
                                      dsnName, NULL, INITFILE) &&
         temp);
}

static void SetCurrentDSN(DSNParam& dsnParam, TCHAR* logFunc) 
{
 LOG_DEBUG_F_FUNC(TEXT("%s: lparam = %s"), logFunc, dsnParam.lpszAttributes);

 dsnParam.currentDSN[0] = TEXT('\0');
 tstring connStr(dsnParam.lpszAttributes);

 ConnectionStringParser::ConnectionStringMapPtr pt =
  ConnectionStringParser::Parse(connStr);

 if(!pt)
 {
  LOG_ERROR_F_FUNC(TEXT("%s: failed to parse the attribute string %s."),
   logFunc, connStr.c_str());

  return;
 }

 ConnectionStringParser::ConnectionStringMap::iterator i = pt->begin();
 ConnectionStringParser::ConnectionStringMap::iterator iEnd = pt->end();
 for(; i != iEnd; ++i)
 {
  const tstring& name = i->first;
  const tstring& value = i->second;

  if(boost::iequals(name, "DSN"))
  {
   _tcscpy(dsnParam.currentDSN, value.c_str());

   LOG_DEBUG_F_FUNC(TEXT("%s: success to set the currentDSN: %s"), 
                    logFunc, value);
  }

 }

 if(dsnParam.currentDSN[0] == TEXT('\0'))
 {
  LOG_DEBUG_F_FUNC(TEXT("%s: failed to find the DSN attribute in %s"),
                   logFunc, connStr);
 }

}

static bool LoadODBCINIDataToDlgDSNCfg(HWND hDlg, TCHAR* currentDSN)
{
 LRESULT x;
 TCHAR buffer[BUFFERSIZE] = { 0 };

 // precaution
 if(!hDlg) 
 {
  _SQLPopDebugMsg(
   TEXT("LoadODBCINIDataToDlgDSNCfg - Bad params: hDlg is NULL"));

  return false;
 }

 // DSN name
 x = SetDlgItemText(hDlg, IDC_DSNNAME, currentDSN);

 if(!x)  
  return false; 

 // user name
 GetValueFromODBCINI(currentDSN, TEXT("UID"), TEXT(ODBC_DRIVER_DEFAULT_UID), 
                     buffer, BUFFERSIZE, INITFILE);
 x = SetDlgItemText(hDlg, IDC_UID, buffer);

 if(!x)
  return false;

 // password
 GetValueFromODBCINI(currentDSN, TEXT("PWD"), TEXT(ODBC_DRIVER_DEFAULT_PWD), 
                     buffer, BUFFERSIZE, INITFILE);
 x = SetDlgItemText(hDlg, IDC_PWD, buffer);

 if(!x)
  return false;

 // server name/IP
 GetValueFromODBCINI(currentDSN, TEXT(ODBC_DRIVER_CONNSTR_HOST_KEY),
  TEXT(ODBC_DRIVER_DEFAULT_HOST),
  buffer, BUFFERSIZE, INITFILE);
 x = SetDlgItemText(hDlg, IDC_SERVER, buffer);

 if(!x)
  return false;

 // server port
 GetValueFromODBCINI(currentDSN, TEXT(ODBC_DRIVER_CONNSTR_PORT_KEY),
  TEXT(ODBC_DRIVER_DEFAULT_PORT),
  buffer, BUFFERSIZE, INITFILE);

 int portTemp = 0;

 try
 {
  portTemp = boost::lexical_cast<int>(buffer);
 }
 catch(const boost::bad_lexical_cast &)
 {
  // unable to convert.
  LOG_DEBUG_F_FUNC(TEXT("%s: Unable to get port from ini."),
                   LOG_FUNCTION_NAME);
 }

 if(portTemp == 0)
  portTemp = boost::lexical_cast<int>(ODBC_DRIVER_DEFAULT_PORT);;

 x = SetDlgItemInt(hDlg, IDC_PORT, portTemp, FALSE);

 if(!x)
  return false;

 return true;
}

static bool RetriveDlgData(HWND hDlg, TCHAR* newDSN, TCHAR* serverStr, 
                           TCHAR* uidStr, TCHAR* pwdStr, int* port,
                           TCHAR* databaseStr) 
{
 LOG_DEBUG_F_FUNC(TEXT("%s: Start retrieving the configs..."), 
                  LOG_FUNCTION_NAME);

 LRESULT x;

 if(!hDlg) 
 {
  _SQLPopDebugMsg(TEXT("RetriveDlgData - Bad params: hDlg is NULL"));
  return false;
 }

 // get text from dialog
 x = SendDlgItemMessage(hDlg, IDC_DSNNAME, EM_LINELENGTH, 0, 0); 

 if(x > 0) 
  GetDlgItemText(hDlg, IDC_DSNNAME, newDSN, BUFFERSIZE);        
 else
  newDSN[0] = TEXT('\0');

 ////// User name
 // get length
 x = SendDlgItemMessage(hDlg, IDC_UID, EM_LINELENGTH, 0, 0);

 if(x > 0) // allocate space
  GetDlgItemText(hDlg, IDC_UID, uidStr, BUFFERSIZE);
 else
  uidStr[0] = TEXT('\0');

 ////// Password
 // get length
 x = SendDlgItemMessage(hDlg, IDC_PWD, EM_LINELENGTH, 0, 0);

 if(x > 0)
  GetDlgItemText(hDlg, IDC_PWD, pwdStr, BUFFERSIZE);
 else
  pwdStr[0] = TEXT('\0');

 ////// server name/IP
 // get length of input text
 x = SendDlgItemMessage(hDlg, IDC_SERVER, EM_LINELENGTH, 0, 0);

 if(x > 0) 
  GetDlgItemText(hDlg, IDC_SERVER, serverStr, BUFFERSIZE);
 else
  serverStr[0] = TEXT('\0');

 /////  Port
 // get value
 *port = GetDlgItemInt(hDlg, IDC_PORT, NULL, FALSE);

 // #TODO: Add database configuration setting.
 databaseStr[0] = TEXT('\0');

 tstring _newDSN(newDSN);
 tstring _serverStr(serverStr);
 tstring _uidStr(uidStr);
 tstring _pwdStr(pwdStr);
 tstring _databaseStr(databaseStr);

 boost::trim(_newDSN);
 boost::trim(_serverStr);
 boost::trim(_uidStr);
 boost::trim(_pwdStr);
 boost::trim(_databaseStr);

 if(_newDSN.empty()) 
 {
  _SQLPopErrorMsg(TEXT("DSN name cannot be empty."));
  return false;
 }
 else
 {
  _tcscpy(newDSN, _newDSN.c_str());
 }

 if(_uidStr.empty() && strlen(ODBC_DRIVER_DEFAULT_UID) != 0)
 {
  _SQLPopErrorMsg(TEXT("Username cannot be empty."));
  return false;
 }
 else if(!_uidStr.empty())
 {
  _tcscpy(uidStr, _uidStr.c_str());
 }

 if(_pwdStr.empty() && strlen(ODBC_DRIVER_DEFAULT_PWD) != 0)
 {
  _SQLPopErrorMsg(TEXT("Password cannot be empty."));
  return false;
 }
 else if(!_pwdStr.empty())
 {
  _tcscpy(pwdStr, _pwdStr.c_str());
 }

 if(_serverStr.empty()) 
 {
  _SQLPopErrorMsg(TEXT("Server cannot be empty."));
  return false;
 }
 else
 {
  _tcscpy(serverStr, _serverStr.c_str());
 }

 if(port == 0) 
 {
  _SQLPopErrorMsg(TEXT("Port cannot be 0."));
  return false;
 }

 if(_databaseStr.empty() && strlen(ODBC_DRIVER_DEFAULT_DATABASE) != 0)
 {
  _SQLPopErrorMsg(TEXT("Database cannot be empty."));
  return false;
 }
 else if(!_databaseStr.empty())
 {
  _tcscpy(databaseStr, _databaseStr.c_str());
 }

 return true;
}

typedef boost::function<bool(ODBCDriver::Connection*)> TestConnectionCallback;

static bool TestConnection(TCHAR* serverStr, TCHAR* uidStr, TCHAR* pwdStr, 
                           int port, TCHAR* databaseStr, 
                           TestConnectionCallback callback = 0)
{
 ODBCDriver::IService& service = ODBCDriver::Service::instance();
 ODBCDriver::Connection* conn = NULL;

 try
 {
  conn = service.OpenConnection(NULL, tstring(databaseStr),
   tstring(serverStr), port,
   tstring(uidStr), tstring(pwdStr));
 }
 catch(ODBCDriver::ClientException& ex)
 {
  //validation of data & other prompts goes here
  _SQLPopErrorMsg(
   TEXT("Username/Password not authorized, or server out of service."));

  return false;
 }

 bool ok = true;

 if(callback)
  ok = callback(conn);

 // now free the structure itself
 service.CloseConnection(conn);

 return ok;
}

static bool TestGetMetadata(TCHAR* serverStr, TCHAR* uidStr, TCHAR* pwdStr, 
                            int port, TCHAR* databaseStr) 
{
 struct _Local
 {
  static bool GetMetadata(ODBCDriver::Connection* conn)
  {
   ODBCDriver::IService& service = ODBCDriver::Service::instance();
   ODBCDriver::ServerReturn serverRet = ODBCDriver::SERVER_ERROR;

   try
   {
    ODBCDriver::ResultSet* resultset;

    serverRet = service.GetTables(conn, tstring(TEXT("*")), &resultset);

    service.CloseResultSet(resultset);
   }
   catch(ODBCDriver::ClientException& ex)
   {
    //validation of data & other prompts goes here
    _SQLPopErrorMsg(TEXT("Unable to get metadata from server."));

    return false;
   }

   return true;
  }
 };

 return TestConnection(serverStr, uidStr, pwdStr, port, databaseStr, 
                       _Local::GetMetadata);
}

static bool SaveConfigToODBCINI(TCHAR* currentDSN, TCHAR* newDSN, 
                                TCHAR* serverStr, 
                                TCHAR* uidStr, TCHAR* pwdStr, int port,
                                TCHAR* databaseStr) 
{
 SetValueInODBCINI(newDSN, TEXT(ODBC_DRIVER_CONNSTR_HOST_KEY), 
                   serverStr, INITFILE);
 SetValueInODBCINI(newDSN, TEXT(ODBC_DRIVER_CONNSTR_PORT_KEY),
                   boost::lexical_cast<tstring>(port).c_str(), INITFILE);
 SetValueInODBCINI(newDSN, TEXT("UID"), uidStr, INITFILE);
 SetValueInODBCINI(newDSN, TEXT("PWD"), pwdStr, INITFILE);
 SetValueInODBCINI(newDSN, TEXT(ODBC_DRIVER_CONNSTR_DATABASE_KEY), 
                   databaseStr, INITFILE);

 //If a new dsn name comes, add a new entry in regedit
 if(!boost::iequals(newDSN, currentDSN)) 
 {
  AddDSN(newDSN);

  //it is a dsn renaming
  if(_tcslen(currentDSN) != 0)
   RemoveDSN(currentDSN);
 }

 _tcscpy(currentDSN, newDSN);

 TCHAR temp[BUFFERSIZE] = { 0 };

 GetValueFromODBCINI(TEXT(ODBC_DRIVER_NAME), TEXT("Driver"), 
                     DRIVER_DEFAULT_LOCATION, temp, BUFFERSIZE, INSTINIFILE);
 SetValueInODBCINI(currentDSN, TEXT("Driver"), temp, INITFILE);

 LOG_DEBUG_FUNC(TEXT("Finish saving the configurations to ODBC INI"));

 return true;
}

static bool RetriveDlgDataToODBCINI(HWND hDlg, bool onlyTest, 
                                    TCHAR* currentDSN)
{
 LOG_DEBUG_FUNC(TEXT("Start retrieving the configurations to ODBC INI"));

 int port;
 TCHAR newDSN[BUFFERSIZE] = { 0 };
 TCHAR serverStr[BUFFERSIZE] = { 0 };
 TCHAR uidStr[BUFFERSIZE] = { 0 };
 TCHAR pwdStr[BUFFERSIZE] = { 0 };
 TCHAR portStrBuffer[BUFFERSIZE] = { 0 };
 TCHAR databaseStr[BUFFERSIZE] = { 0 };

 if(!RetriveDlgData(hDlg, newDSN, serverStr, uidStr, pwdStr, &port, 
                    databaseStr))
  return false;

 if(onlyTest) 
  return TestConnection(serverStr, uidStr, pwdStr, port, databaseStr);

 return SaveConfigToODBCINI(currentDSN, newDSN, serverStr, uidStr, pwdStr, 
                            port, databaseStr);
}


INT_PTR CALLBACK DlgDSNCfgProc(HWND hDlg, UINT uMsg, 
                               WPARAM wParam, LPARAM lParam) 
{
 DSNParam* pDsnParam = (DSNParam*)lParam;

 int  port;
 TCHAR newDSN[BUFFERSIZE] = { 0 };
 TCHAR serverStr[BUFFERSIZE] = { 0 };
 TCHAR uidStr[BUFFERSIZE] = { 0 };
 TCHAR pwdStr[BUFFERSIZE] = { 0 };
 TCHAR databaseStr[BUFFERSIZE] = { 0 };

 switch(uMsg) 
 {
  case WM_INITDIALOG:
   SetCurrentDSN(*pDsnParam, LOG_FUNCTION_NAME);

   // store the structure for future use
   SetWindowLongPtr(hDlg, DWLP_USER, lParam);

   // initialize the dialog with data from REGEDIT
   if(!LoadODBCINIDataToDlgDSNCfg(hDlg, pDsnParam->currentDSN))
    return false;

   SetFocus(GetDlgItem(hDlg, IDC_DSNNAME));

   // set focus automatically
   return TRUE;

  case WM_COMMAND:
   switch(LOWORD(wParam)) 
   {
    case IDC_BTEST: 
    {
     if(RetriveDlgData(hDlg, newDSN, serverStr, uidStr, pwdStr, &port, 
                       databaseStr))
     {
      if(TestConnection(serverStr, uidStr, pwdStr, port, databaseStr))
      {
       HWND hwndOK = GetDlgItem(hDlg, IDOK);
       
       //passed verification
       EnableWindow(hwndOK, TRUE);

       return TRUE;
      }
      else 
      {
       _SQLPopErrorMsg(TEXT("Test connection failed."));
      }
     }
     else 
     {
      _SQLPopErrorMsg(TEXT("Retrive data failed."));
     }

     return FALSE;
    }

    case IDOK: 
    {
     pDsnParam = (DSNParam*)GetWindowLongPtr(hDlg, DWLP_USER);

     if(RetriveDlgData(hDlg, newDSN, serverStr, uidStr, pwdStr, &port, 
                       databaseStr))
     {
      if(TestGetMetadata(serverStr, uidStr, pwdStr, port, databaseStr))
      {
       SaveConfigToODBCINI(pDsnParam->currentDSN, newDSN, serverStr, 
                           uidStr, pwdStr, port, databaseStr);
       EndDialog(hDlg, wParam);
       return TRUE;
      }
     }

     return FALSE;
    }

    case IDCANCEL:
     // indicate end with control id as return value
     EndDialog(hDlg, wParam);
     return TRUE;
   }
 }

 return FALSE;
}

#ifdef  UNICODE
// not defined in odbcinst.h.
#define  ConfigDSN         ConfigDSNW
#endif

/*
https://msdn.microsoft.com/en-us/library/ms709275%28v=vs.85%29.aspx
*/

BOOL INSTAPI ConfigDSN(HWND hwndParent, WORD fRequest, LPCTSTR lpszDriver, 
                       LPCTSTR lpszAttributes) 
{
 DSNParam dsnParam = { 0 };

 LOG_DEBUG_F_FUNC(
  TEXT("%s: hwndParent = 0x%08lX, fRequest = %d, ")
  TEXT("lpszDriver = %s, lpszAttributes = %s"),
  LOG_FUNCTION_NAME,
  (long)hwndParent, fRequest, 
  LOG4CPLUS_STRING_TO_TSTRING(lpszDriver), 
  LOG4CPLUS_STRING_TO_TSTRING(lpszAttributes));

 dsnParam.lpszAttributes = lpszAttributes;

 if(fRequest == ODBC_REMOVE_DSN)
 {
  SetCurrentDSN(dsnParam, LOG_FUNCTION_NAME);

  if(_tcslen(dsnParam.currentDSN) <= 0)
  {
   _SQLPopErrorMsg(
    TEXT("The DSN name is not defined in the connection string!"));

   return FALSE;
  }

  BOOL ret = TRUE;
  ret = RemoveDSN(dsnParam.currentDSN);

  if(!ret)
  {
   _SQLPopErrorMsg(TEXT("The DSN is not found, removal failed!"));
   return false;
  }

  return true;
 }

 // ODBC_CONFIG_DSN or ODBC_ADD_DSN
 INT_PTR i = DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_DSN_CFG), NULL, 
                            DlgDSNCfgProc, (LPARAM)&dsnParam);

 // check status
 switch(i) 
 {
  case IDOK: // complete
   LOG_INFO_FUNC(TEXT("User click OK button on DSN config."));
   return TRUE;           

  default: // user-cancelled
   LOG_INFO_FUNC(TEXT("User click Cancel button on DSN config."));
   return FALSE;          
 }

 return TRUE;
}

#endif
