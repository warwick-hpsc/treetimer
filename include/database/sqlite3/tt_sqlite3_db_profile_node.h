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

#ifndef TT_DATABASE_SQLITE3_PROFILE_NODE_INCLUDE_H
#define TT_DATABASE_SQLITE3_PROFILE_NODE_INCLUDE_H

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
				// ========================================================================================================================

				void writeSchemaProfileNodeData(TTSQLite3& dataAccess);

				// ========================================================================================================================

				void findProfileNodeID(TTSQLite3& dataAccess, std::string profileNodeName, int profileNodeTypeID, int * profileNodeID);

				// ========================================================================================================================

				void writeTreeNodeProfileNodeData(TTSQLite3& dataAccess,
												  std::string profileNodeName,
												  int profileNodeTypeID,
												  int * profileNodeID);

				// ========================================================================================================================
			}
		}
	}
}

#endif
