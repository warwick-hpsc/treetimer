/*
 * @file
 * @author James Davis
 * @version 0.1
 *
 * @section LICENSE
 * Based on PMTM/Ichniea - ToDo: Check License for these
 *
 * @section DESCRIPTION
 *
 */

#ifndef TT_DATABASE_SQLITE3_APP_CONFIG_INCLUDE_H
#define TT_DATABASE_SQLITE3_APP_CONFIG_INCLUDE_H

#include "tt_sqlite3_db_access.h"
#include <vector>
#include <string>
#include "sqlite3.h"

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaApplicationConfigData(TTSQLite3& dataAccess);


				// ToDo: Store as pairs in the vector?
				void findApplicationConfigDataID(TTSQLite3& dataAccess,
						 	 	 	 	 	 	 int applicationID,
												 std::vector<std::string> intParamNames,  std::vector<int> intParamValues,
												 std::vector<std::string> floatingParamNames,  std::vector<double> floatingParamValues,
												 std::vector<std::string> boolParamNames,  std::vector<bool> boolParamValues,
												 std::vector<std::string> stringParamNames,  std::vector<std::string> stringParamValues,
												 int * applicationConfigID);

				void writeApplicationConfigData(TTSQLite3& dataAccess,
						 int applicationID,
						 std::vector<std::string> intParamNames,  std::vector<int> intParamValues,
						 std::vector<std::string> floatingParamNames,  std::vector<double> floatingParamValues,
						 std::vector<std::string> boolParamNames,  std::vector<bool> boolParamValues,
						 std::vector<std::string> stringParamNames,  std::vector<std::string> stringParamValues,
						 int * applicationConfigID);
			}
		}
	}
}

#endif
