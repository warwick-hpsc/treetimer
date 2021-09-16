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

#ifndef TT_DATABASE_SQLITE3_PROFILE_NODE_TYPE_INCLUDE
#define TT_DATABASE_SQLITE3_PROFILE_NODE_TYPE_INCLUDE

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
				void writeSchemaProfileNodeTypeData(TTSQLite3& dataAccess);
				void findProfileNodeTypeID(TTSQLite3& dataAccess, std::string profileTypeName, int *profileNodeTypeID);
				void writeProfileNodeTypeData(TTSQLite3& dataAccess, std::string profileTypeName, int *profileNodeTypeID);
			}
		}
	}
}

#endif
