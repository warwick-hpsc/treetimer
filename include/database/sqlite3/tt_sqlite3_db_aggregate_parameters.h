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

#ifndef TT_DATABASE_SQLITE3_AGGREGATE_PARAMETERS_INCLUDE_H
#define TT_DATABASE_SQLITE3_AGGREGATE_PARAMETERS_INCLUDE_H

#include "sqlite3.h"
#include <string>
#include "tt_sqlite3_db_access.h"

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaAggregateParameterData(TTSQLite3& dataAccess);

				void findAggregateParameterIntDataID  (TTSQLite3& dataAccess, TT_AggParamInt   p, int* aggParamIntID);
				void findAggregateParameterFloatDataID(TTSQLite3& dataAccess, TT_AggParamFloat p, int* aggParamFloatID);
				void findAggregateParameterBoolDataID (TTSQLite3& dataAccess, TT_AggParamBool  p, int* aggParamBoolID);

				void writeAggregateParameterIntData   (TTSQLite3& dataAccess, TT_AggParamInt   p, int* aggParamIntID);
				void writeAggregateParameterFloatData (TTSQLite3& dataAccess, TT_AggParamFloat p, int* aggParamFloatID);
				void writeAggregateParameterBoolData  (TTSQLite3& dataAccess, TT_AggParamBool  p, int* aggParamBoolID);

				MPI_Datatype createAggregateParamIntMpiType();
				MPI_Datatype createAggregateParamFloatMpiType();
				MPI_Datatype createAggregateParamBoolMpiType();
			}
		}
	}
}

#endif
