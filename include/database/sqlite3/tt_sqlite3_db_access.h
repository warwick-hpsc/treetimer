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

#include <string>
#include <vector>

#include "mpi.h"
#include "sqlite3.h"

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			// To implement gather-at-root, need a place to collate data before transferring.
			// That place needs to be a class object passed around. Ideally would be a new class, 
			// but will put it into TTSQLite3 to get something working asap.

			// 'rank' can be inferred, but included so that struct matches SQL exactly:
			typedef struct {
				int runID, rank, callPathID, processID;
				double minWallTime, avgWallTime, maxWallTime, stdev;
				int count;
			} TTAggTiming;

			typedef struct {
				int runID, rank, callPathID, processID;
				int nodeEntryID, nodeExitID;
				double walltime;
			} TTTraceTiming;

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
					MPI_Comm nodeComm;
					std::vector<TTAggTiming> aggTimeRecords;
					std::vector<TTTraceTiming> traceTimeRecords;
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
