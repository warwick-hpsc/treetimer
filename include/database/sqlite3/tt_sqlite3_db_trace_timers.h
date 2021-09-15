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

#ifndef TT_DATABASE_SQLITE3_TRACE_TIMERS_INCLUDE_H
#define TT_DATABASE_SQLITE3_TRACE_TIMERS_INCLUDE_H

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
				void writeSchemaTraceTimeData(TTSQLite3& dataAccess);

				void findTraceTimeDataID(TTSQLite3& dataAccess, TT_TraceTiming d, int *traceTimeID);

				// Note: 'verifyUnique' prevents data duplication, but makes this O(1) insertion a O(N) operation.
				void writeTraceTimeData(TTSQLite3& dataAccess, TT_TraceTiming d, int *traceTimeID, bool verifyUnique);

				MPI_Datatype createTraceTimeMpiType();
			}
		}
	}
}

#endif
