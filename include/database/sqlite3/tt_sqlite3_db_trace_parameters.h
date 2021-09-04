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

#ifndef TT_DATABASE_SQLITE3_TRACE_PARAMETERS_INCLUDE_H
#define TT_DATABASE_SQLITE3_TRACE_PARAMETERS_INCLUDE_H

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
				void writeSchemaTraceParameterData(TTSQLite3& dataAccess);

				void findTraceParameterIntDataID   (TTSQLite3& dataAccess, TT_TraceParamInt    p, int *traceParamIntID);
				void findTraceParameterFloatDataID (TTSQLite3& dataAccess, TT_TraceParamFloat  p, int *traceParamDoubleID);
				void findTraceParameterBoolDataID  (TTSQLite3& dataAccess, TT_TraceParamBool   p, int *traceParamBoolID);
				void findTraceParameterStringDataID(TTSQLite3& dataAccess, TT_TraceParamString p, int *traceParamStringID);

				void writeTraceParameterIntData   (TTSQLite3& dataAccess, TT_TraceParamInt    p, int *traceParamIntID);
				void writeTraceParameterFloatData (TTSQLite3& dataAccess, TT_TraceParamFloat  p, int *traceParamFloatID);
				void writeTraceParameterBoolData  (TTSQLite3& dataAccess, TT_TraceParamBool   p, int *traceParamBoolID);
				void writeTraceParameterStringData(TTSQLite3& dataAccess, TT_TraceParamString p, int *traceParamStringID);

				MPI_Datatype createTraceParamIntMpiType();
				MPI_Datatype createTraceParamFloatMpiType();
				MPI_Datatype createTraceParamBoolMpiType();
				MPI_Datatype createTraceParamStringMpiType();
			}
		}
	}
}

#endif
