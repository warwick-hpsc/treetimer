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

#ifndef TT_DATABASE_SQLITE3_AGGREGATE_TIMERS_INCLUDE_H
#define TT_DATABASE_SQLITE3_AGGREGATE_TIMERS_INCLUDE_H

#include "sqlite3.h"
#include "tt_sqlite3_db_access.h"
#include <string>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaAggregateTimeData(TTSQLite3& dataAccess);

				void findAggregateTimeDataID(TTSQLite3& dataAccess, TT_AggTiming d, int * aggTimeID);

				void writeAggregateTimeData(TTSQLite3& dataAccess, TT_AggTiming d, int * aggTimeID);

				MPI_Datatype createAggregateTimeMpiType();

				MPI_Datatype createHwInfoMpiType();
			}
		}
	}
}

#endif
