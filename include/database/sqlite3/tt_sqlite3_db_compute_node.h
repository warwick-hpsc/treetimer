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

#ifndef TT_DATABASE_SQLITE3_COMPUTE_NODE_INCLUDE_H
#define TT_DATABASE_SQLITE3_COMPUTE_NODE_INCLUDE_H

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
				void writeSchemaComputeNodeData(TTSQLite3& dataAccess);

				void findComputeNodeDataID(TTSQLite3& dataAccess,
											// int machineID,
											TT_ComputeNode d,
											int * computeNodeID);

				void writeComputeNodeData(TTSQLite3& dataAccess,
											// int machineID,
											TT_ComputeNode d,
											int * computeNodeID);
			}
		}
	}
}

#endif
