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

#ifndef TT_DATABASE_SQLITE3_LIB_CONFIG_INCLUDE_H
#define TT_DATABASE_SQLITE3_LIB_CONFIG_INCLUDE_H

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
				void writeSchemaLibraryConfigData(TTSQLite3& dataAccess);

				void findLibraryConfigID(TTSQLite3& dataAccess,
										 int verMajor, int verMinor,
										 bool aggTimers, bool traceTimers,
										 bool aggParam, bool traceParam,
										 bool aggPAPI, bool tracePAPI,
										 bool mpiHooks,
										 int * libConfigID);

				void writeLibraryConfigID(TTSQLite3& dataAccess,
										 int verMajor, int verMinor,
										 bool aggTimers, bool traceTimers,
										 bool aggParam, bool traceParam,
										 bool aggPAPI, bool tracePAPI,
										 bool mpiHooks,
										 int * libConfigID);
			}
		}
	}
}

#endif
