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

#ifndef TT_DATABASE_SQLITE3_CPU_INCLUDE_H
#define TT_DATABASE_SQLITE3_CPU_INCLUDE_H

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
				void writeSchemaCPUData(TTSQLite3& dataAccess);

				void findCPUDataID(TTSQLite3& dataAccess, TT_Cpu d, int *cpuID);

				void writeCPUData(TTSQLite3& dataAccess, TT_Cpu d, int *cpuID);
			}
		}
	}
}

#endif
