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

				void findAggregateParameterIntDataID(TTSQLite3& dataAccess,
										   int runID, int callPathID, int processID, std::string paramName,
										   int minValue, double avgValue, int maxValue, double stddev, int count,
										   int * aggParamIntID);


				void findAggregateParameterFloatDataID(TTSQLite3& dataAccess,
										   int runID, int callPathID, int processID, std::string paramName,
										   double minValue, double avgValue, double maxValue, double stddev, int count,
										   int * aggParamFloatID);

				void findAggregateParameterBoolDataID(TTSQLite3& dataAccess,
										   int runID, int callPathID, int processID, std::string paramName,
										   int minValue, double avgValue, int maxValue, double stddev, int count,
										   int * aggParamBoolID);

				void writeAggregateParameterIntData(TTSQLite3& dataAccess,
						   int runID, int callPathID, int processID, std::string paramName,
						   int minValue, double avgValue, int maxValue, double stddev, int count,
						   int * aggParamIntID);

				void writeAggregateParameterFloatData(TTSQLite3& dataAccess,
						   int runID, int callPathID, int processID, std::string paramName,
						   double minValue, double avgValue, double maxValue, double stddev, int count,
						   int * aggParamFloatID);

				void writeAggregateParameterBoolData(TTSQLite3& dataAccess,
						   int runID, int callPathID, int processID, std::string paramName,
						   int minValue, double avgValue, int maxValue, double stddev, int count,
						   int * aggParamBoolID);
			}
		}
	}
}

#endif
