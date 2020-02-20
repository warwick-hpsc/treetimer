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

#include "tt_sqlite3_db_access.h"
#include "sqlite3.h"
#include <string>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaApplicationData(TTSQLite3& dataAccess);
				void findApplicationDataID(TTSQLite3& dataAccess, std::string appName, std::string appVer, int * applicationID);
				void writeApplicationData(TTSQLite3& dataAccess, std::string appName, std::string appVer, int * applicationID);
			}
		}
	}
}
