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

#ifndef TT_DATABASE_SQLITE3_CPU_SOCKET_INCLUDE_H
#define TT_DATABASE_SQLITE3_CPU_SOCKET_INCLUDE_H

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
				void writeSchemaCPUSocketData(TTSQLite3& dataAccess);

				void findCPUSocketDataID(TTSQLite3& dataAccess, int computeNodeID, int cpuID, TT_Socket d_skt, int * cpuSocketID);

				void writeCPUSocketData(TTSQLite3& dataAccess, int computeNodeID, int cpuID, TT_Socket d_skt, int * cpuSocketID);
			}
		}
	}
}

#endif
