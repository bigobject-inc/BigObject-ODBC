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

#ifndef _BIGOBJECT_ODBC_CLIENT_CONNECTION_
#define _BIGOBJECT_ODBC_CLIENT_CONNECTION_

namespace ODBCDriver
{

/**
Abstract container class for database connections.

This class stores the database connection information. 
It was only meant to be created by OpenConnection and destroyed by 
CloseConnection.
*/
class Connection
{
public:
 
 /**
 Constructor.

 @param _hDbc SQLHDBC, implementation should cast it to HDRVDBC if it needs to 
              do something with it.
 */
 Connection(SQLHDBC _hDbc = NULL) : hDbc(_hDbc)
 {
 }

 /**
 Destructor.
 */
 virtual ~Connection()
 {
 }

protected:

 SQLHDBC hDbc;

};

}

#endif
