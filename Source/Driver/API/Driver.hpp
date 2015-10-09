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

#ifndef _BIGOBJECT_ODBC_H_DRIVER
#define _BIGOBJECT_ODBC_H_DRIVER

#include "../Driver.hpp"

#include "DriverExtras.hpp"

/****************************
* include DBMS specific includes here and make any required defines here
* as well.
***************************/

#ifdef WIN32
static const TCHAR* DRIVER_ODBC_VER = TEXT("03.51.0000");
#else
#define DRIVER_ODBC_VER TEXT("02.00")
#endif

#define SQL_MAX_CURSOR_NAME 100

/*****************************************************************************
 * STATEMENT
 *****************************************************************************/
typedef struct tDRVSTMT
{
 struct tDRVSTMT *pPrev; /* prev struct or null		*/
 struct tDRVSTMT *pNext; /* next struct or null		*/
 SQLPOINTER hDbc; /* pointer to DB context    */

 SQLTCHAR szCursorName[SQL_MAX_CURSOR_NAME]; /* name of cursor           */
 SQLTCHAR *pszQuery; /* query string             */

 HSTMTEXTRAS hStmtExtras; /* DRIVER SPECIFIC STORAGE  */

} DRVSTMT, *HDRVSTMT;

/*****************************************************************************
 * CONNECTION
 *****************************************************************************/
typedef struct tDRVDBC
{
 struct tDRVDBC *pPrev; /* prev struct or null		*/
 struct tDRVDBC *pNext; /* next struct or null		*/
 SQLPOINTER hEnv; /* pointer to ENV structure */
 HDRVSTMT hFirstStmt; /* first in list or null	*/
 HDRVSTMT hLastStmt; /* last in list or null		*/

 int bConnected; /* TRUE on open connection  */
 HDBCEXTRAS hDbcExtras; /* DRIVER SPECIFIC DATA     */

} DRVDBC, *HDRVDBC;

/*****************************************************************************
 * ENVIRONMENT
 *****************************************************************************/
typedef struct tDRVENV
{
 HDRVDBC hFirstDbc; /* first in list or null	*/
 HDRVDBC hLastDbc; /* last in list or null		*/

 HENVEXTRAS hEnvExtras;

} DRVENV, *HDRVENV;

#endif
