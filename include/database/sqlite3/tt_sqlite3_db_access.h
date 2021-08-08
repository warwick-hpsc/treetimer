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

#ifndef TT_DATABASE_SQLITE3_ACCESS_INCLUDE_H
#define TT_DATABASE_SQLITE3_ACCESS_INCLUDE_H

#include "sqlite3.h"
#include <string>
#include <vector>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			// To implement gather-at-root, need a place to collate data before transferring.
			// That place needs to be a class object passed around. Ideally would be a new class, 
			// but will put it into TTSQLite3
			typedef struct {
				int runID, rank, callPathID, processID;
				double minWallTime, avgWallTime, maxWallTime, stdev;
				int count;
			} aggTimeData;

			class TTSQLite3
			{
				public:
					TTSQLite3(std::string filePath);
					~TTSQLite3();

					std::string filePath;
					sqlite3 * db;

					// State for gather-at-root:
					int rankGlobal = -1;
					int rankLocal = -1;
					int nRanksLocal = -1;
					bool gatherIntraNode = false;
					std::vector<aggTimeData> aggTimeRecords;
			};

			namespace drivers
			{
				void openConnection(TTSQLite3& dataAccess);
				void closeConnection(TTSQLite3& dataAccess);
			}
		}
	}
}

#endif
