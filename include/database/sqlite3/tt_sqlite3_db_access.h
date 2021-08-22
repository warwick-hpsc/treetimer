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

#include "tt_global.h"

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
				int rank, callPathID, processID;
				double minWallTime, avgWallTime, maxWallTime, stdev;
				int count;
			} TT_AggTiming;

			typedef struct {
				int rank, callPathID, processID;
				int nodeEntryID, nodeExitID;
				double walltime;
			} TT_TraceTiming;

			typedef struct {
				int rank;
				char nodeName[MAX_STRING_LENGTH];
				int blockType;
				int callPathID, parentID;
			} TT_CallPathNode;


			typedef struct {
				int physicalCoreNum;
			} TT_CpuCore;

			typedef struct {
				char cpuModel[MAX_STRING_LENGTH];
			} TT_Cpu;

			typedef struct {
				int phySocketNum;
			} TT_Socket;

			typedef struct {
				char nodeName[MAX_STRING_LENGTH];
				int socketCount;
			} TT_ComputeNode;

			typedef struct {
				int thread, rankGlobal;
				char nodeName[MAX_STRING_LENGTH];
				char cpuModel[MAX_STRING_LENGTH];
				int socketCount, phySocketNum, physicalCoreNum;
			} TT_HwInfo;



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

					std::vector<TT_CallPathNode> callPathNodeRecords;
					std::vector<TT_AggTiming> aggTimeRecords;
					std::vector<TT_TraceTiming> traceTimeRecords;

					std::vector<int> rankLocalToProcessID;
					std::vector<int> rankLocalToRankGlobal;

					// Global variables:
					int processID = -1;
					int runID = -1;
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
