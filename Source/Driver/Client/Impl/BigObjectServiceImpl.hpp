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

#ifndef _BIGOBJECT_ODBC_CLIENT_SERVICE_IMPL_RESTFUL_
#define _BIGOBJECT_ODBC_CLIENT_SERVICE_IMPL_RESTFUL_

#include "../../../Logging/Logging.hpp"

#include "../Service.hpp"

namespace ODBCDriver
{

/**
Service implementation for BigObject RESTful service.
*/
class BigObjectServiceImpl : public ServiceInterface<BigObjectServiceImpl>
{
public:
 
 /**
 Constructor.
 */
 BigObjectServiceImpl();

 // ####################################################################
 // Inherited
 // ####################################################################
 Connection* OpenConnection(SQLHDBC hDbc, const tstring& database, 
                            const tstring& host, int port,
                            const tstring uid = tstring(),
                            const tstring password = tstring());

 ServerReturn CloseConnection(Connection* connection);

 ServerReturn Execute(Connection* connection, const tstring& query,
                      ResultSet** pResultSet, std::size_t maxBufRows);

 ServerReturn GetTables(Connection* connection, const tstring& pattern,
                        ResultSet** pResultSet);

 ServerReturn GetTypeInfo(Connection* connection, SQLSMALLINT dataType, 
                          ResultSet** pResultSet);

 ServerReturn GetColumns(Connection* connection, const tstring& tblPattern,
                         const tstring& colPattern, ResultSet** pResultSet);

 ServerReturn CloseResultSet(ResultSet* resultset);
 // ####################################################################

private:

 /// Logger.
 bigobject::Logging::Logger logger;

};

}

#endif
