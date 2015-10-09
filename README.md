# BigObject-ODBC

BigObject-ODBC is a ODBC connetor to [BigOBject](https://hub.docker.com/r/bigobject/bigobject/) Analytics Database via rest API. 

## Required Library

* Boost 1.58
* Log4cplus 1.1.2
* unixODBC 2.3.2
* JsonCpp 0.10.2
* libcurl 7.42.1
* OpenSSL 1.0.2a
* UTF8-CPP 2.3.4

## ODBC 3.51 API status

API             | status
----------------|----------
SQLAllocConnect | supported  
SQLAllocEnv     | supported  
SQLAllocHandle  | supported  
SQLAllocStmt    | supported
SQLBindCol      | supported  
SQLBindParameter| NOT supported  
SQLCancel       | NOT supported  
SQLCloseCursor  | NOT supported  
SQLColAttribute  | PARTIAL supported  
SQLColAttributes |  supported  
SQLColumnPrivileges | NOT supported  
SQLColumns  | PARTIAL supported  
SQLConnect  | PARTIAL supported  
SQLDescribeCol  | supported  
SQLDescribeParam | NOT supported  
SQLDisconnect  | supported  
SQLDriverConnect  | PARTIAL supported  
SQLEndTran  | supported  
SQLError  | NOT supported  
SQLExecDirect  | supported  
SQLExecute  | supported  
SQLExtendedFetch  | PARTIAL supported  
SQLFetch  | supported  
SQLFetchScroll  | PARTIAL supported  
SQLForeignKeys  | NOT supported  
SQLFreeConnect  | supported  
SQLFreeEnv  | supported  
SQLFreeHandle  | supported  
SQLFreeStmt  | supported  
SQLGetConnectAttr  | PARTIAL supported  
SQLGetConnectOption | PARTIAL supported  
SQLGetCursorName  | NOT supported  
SQLGetData  | supported  
SQLGetDescField |  NOT supported  
SQLGetDescRec  | NOT supported  
SQLGetDiagField  | NOT supported  
SQLGetDiagRec  | NOT supported  
SQLGetEnvAttr  | PARTIAL supported  
SQLGetInfo  | PARTIAL supported  
SQLGetStmtAttr  | PARTIAL supported  
SQLGetStmtOption  | PARTIAL supported  
SQLGetTypeInfo  | PARTIAL supported  
SQLNativeSql  | NOT supported  
SQLMoreResults  | supported  
SQLNumParams  | NOT supported  
SQLNumResultCols  | supported  
SQLParamData  | NOT supported  
SQLParamOptions |  NOT supported  
SQLPrepare  | supported  
SQLPrimaryKeys  | NOT supported  
SQLProcedureColumns | NOT supported  
SQLPutData  | NOT supported  
SQLRowCount |  NOT supported  
SQLSetConnectAttr | PARTIAL supported  
SQLSetConnectOption | PARTIAL supported  
SQLSetEnvAttr | PARTIAL supported  
SQLSetStmtAttr |  PARTIAL supported  
SQLSetStmtOption  | PARTIAL supported  
SQLSetCursorName  | NOT supported  
SQLSetDescField  | NOT supported  
SQLSetDescRec  | NOT supported  
SQLSetParam  | NOT supported  
SQLSetPos  | NOT supported  
SQLSetScrollOptions | NOT supported  
SQLSpecialColumns  | NOT supported  
SQLStatistics  | NOT supported  
SQLTablePrivileges |  NOT supported  
SQLTables  | PARTIAL supported  
SQLTransact  | NOT supported  
