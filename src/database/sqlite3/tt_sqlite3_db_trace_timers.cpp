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

#include "tt_sqlite3_db_trace_timers.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaTraceTimeData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS TraceTimeData(TraceTimeID INTEGER, "
																			"RunID INTEGER, "
																			"Rank INTEGER, "
																			"CallPathID INTEGER, "
																			"ProcessID INTEGER, "
																			"NodeEntryID INTEGER, "
																			"NodeExitID INTEGER, "
																			"WallTime REAL, "
																			"FOREIGN KEY(RunID) REFERENCES ProfileRunConfigData(RunID),"
																			"FOREIGN KEY(CallPathID) REFERENCES CallPathData(CallPathID), "
																			"FOREIGN KEY(ProcessID) REFERENCES ProcessData(ProcessID), "
																			"UNIQUE(RunID, Rank, CallPathID, ProcessID, NodeEntryID, NodeExitID), "
																		 	"PRIMARY KEY(TraceTimeID)"
																		 	");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findTraceTimeDataID(TTSQLite3& dataAccess, TT_TraceTiming d, int * traceTimeID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT TraceTimeID FROM TraceTimeData WHERE "
										      "RunID = ? AND "
										      "Rank = ? AND "
											  "CallPathID = ? AND "
											  "ProcessID = ? AND "
											  "NodeEntryID = ? AND "
											  "NodeExitID = ? AND "
											  "WallTime = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(   pStmt,1, dataAccess.runID);
					sqlite3_bind_int(   pStmt,2, d.rank);
					sqlite3_bind_int(   pStmt,3, d.callPathID);
					sqlite3_bind_int(   pStmt,4, d.processID);
					sqlite3_bind_int(   pStmt,5, d.nodeEntryID);
					sqlite3_bind_int(   pStmt,6, d.nodeExitID);
					sqlite3_bind_double(pStmt,7, d.walltime);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findTraceTimeDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*traceTimeID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*traceTimeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceTimeData(TTSQLite3& dataAccess, TT_TraceTiming d, int * traceTimeID)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.traceTimeRecords.push_back(d);
						*traceTimeID = -1;
						return;
					}

					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findTraceTimeDataID(dataAccess, d, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceTimeData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(   pStmt,1, dataAccess.runID);
						sqlite3_bind_int(   pStmt,2, d.rank);
						sqlite3_bind_int(   pStmt,3, d.callPathID);
						sqlite3_bind_int(   pStmt,4, d.processID);
						sqlite3_bind_int(   pStmt,5, d.nodeEntryID);
						sqlite3_bind_int(   pStmt,6, d.nodeExitID);
						sqlite3_bind_double(pStmt,7, d.walltime);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeTraceTimeData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
							sqlite3_free(expandedQuery);
							*traceTimeID = -1;
						}
						else
						{
							*traceTimeID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*traceTimeID = tmpID;
					}
				}

				MPI_Datatype createTraceTimeMpiType()
				{
					// Create MPI type for a TraceTime record:
					int err;
					MPI_Datatype traceTimeRecord_MPI, tmpType;

					int lengths[6] = {1, 1, 1, 1, 1, 1};
					MPI_Aint displacements[6];
					displacements[0] = offsetof(TT_TraceTiming, rank);
					displacements[1] = offsetof(TT_TraceTiming, callPathID);
					displacements[2] = offsetof(TT_TraceTiming, processID);
					displacements[3] = offsetof(TT_TraceTiming, nodeEntryID);
					displacements[4] = offsetof(TT_TraceTiming, nodeExitID);
					displacements[5] = offsetof(TT_TraceTiming, walltime);
					MPI_Datatype types[6] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_INT, MPI_INT, MPI_DOUBLE };
					err = MPI_Type_create_struct(6, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for traceTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for traceTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &traceTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for traceTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&traceTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for traceTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return traceTimeRecord_MPI;
				}
			}
		}
	}
}
