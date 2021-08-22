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

#ifndef TT_DATABASE_SQLITE3_PROFILE_RUN_CONFIG_INCLUDE_H
#define TT_DATABASE_SQLITE3_PROFILE_RUN_CONFIG_INCLUDE_H

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
				// Most of table is useless, trim it down!
				void writeSchemaProfileRunConfigData(TTSQLite3& dataAccess);

				// void writeProfileRunConfigData(TTSQLite3& dataAccess, int appConfigID, int libConfigID, int processCount, int * runID);
				void writeProfileRunConfigData(TTSQLite3& dataAccess, int libConfigID, int processCount, int * runID);
			}
		}
	}
}

#endif
