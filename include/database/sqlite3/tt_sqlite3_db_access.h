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

#include <map>
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

			typedef struct {
				int rank, processID, window, callPathID;
				double minWallTime, avgWallTime, maxWallTime, stdev;
				int count;
			} TT_AggTiming;

			typedef struct {
				int rank, processID, callPathID;
				int nodeEntryID, nodeExitID;
				double walltime;
			} TT_TraceTiming;

			typedef struct {
				int rank, processID, window, callPathID;
				char paramName[MAX_STRING_LENGTH];
				int minValue, maxValue, count;
				double avgValue, stdev;
			} TT_AggParamInt;

			typedef struct {
				int rank, processID, window, callPathID;
				char paramName[MAX_STRING_LENGTH];
				double minValue, maxValue;
				int count;
				double avgValue, stdev;
			} TT_AggParamFloat;

			typedef struct {
				int rank, processID, window, callPathID;
				char paramName[MAX_STRING_LENGTH];
				int minValue, maxValue, count;
				double avgValue, stdev;
			} TT_AggParamBool;

			typedef struct {
				int rank, processID, callPathID;
				char paramName[MAX_STRING_LENGTH];
				int paramValue;
				int nodeEntryID, nodeExitID;
			} TT_TraceParamInt;

			typedef struct {
				int rank, processID, callPathID;
				char paramName[MAX_STRING_LENGTH];
				double paramValue;
				int nodeEntryID, nodeExitID;
			} TT_TraceParamFloat;

			typedef struct {
				int rank, processID, callPathID;
				char paramName[MAX_STRING_LENGTH];
				bool paramValue;
				int nodeEntryID, nodeExitID;
			} TT_TraceParamBool;

			typedef struct {
				int rank, processID, callPathID;
				char paramName[MAX_STRING_LENGTH];
				char paramValue[MAX_STRING_LENGTH];
				int nodeEntryID, nodeExitID;
			} TT_TraceParamString;

			typedef struct {
				int rank, processID;
				char nodeName[MAX_STRING_LENGTH];
				int blockType;
				int callPathID, parentID, profileNodeID;
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

					std::string filePath;
					sqlite3 *db;

					// State for gather-at-root:
					int rankGlobal = -1;
					int rankLocal = -1;
					int nRanksLocal = -1;
					bool gatherIntraNode = false;
					MPI_Comm nodeComm = -1;

					std::vector<TT_CallPathNode> callPathNodeRecords;
					std::vector<TT_AggTiming> aggTimeRecords;
					std::vector<TT_AggParamInt> aggParamIntRecords;
					std::vector<TT_AggParamFloat> aggParamFloatRecords;
					std::vector<TT_AggParamBool> aggParamBoolRecords;
					std::vector<TT_TraceTiming> traceTimeRecords;
					std::vector<TT_TraceParamInt> traceParamIntRecords;
					std::vector<TT_TraceParamFloat> traceParamFloatRecords;
					std::vector<TT_TraceParamBool> traceParamBoolRecords;
					std::vector<TT_TraceParamString> traceParamStringRecords;

					std::vector<int> rankLocalToProcessID;
					std::vector<int> rankLocalToRankGlobal;

					// For each local rank, store a mapping of old Callpath node ID to new ID.
					// Only root rank of local communicator creates this.
					std::vector<std::map<int,int>> callpathNodeIdRemap;
					bool callpathNodeIdRemap_created = false;

					// Global variables:
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
