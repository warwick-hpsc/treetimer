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
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS TraceTimeData(TraceTimeID INTEGER, "
																			"RunID INTEGER, "
																			"CallPathID INTEGER, "
																			"ProcessID INTEGER, "
																			"NodeEntryID INTEGER, "
																			"NodeExitID INTEGER, "
																			"WallTime REAL, "
																			"FOREIGN KEY(RunID) REFERENCES ProfileRunConfigData(RunID),"
																			"FOREIGN KEY(CallPathID) REFERENCES CallPathData(CallPathID), "
																			"FOREIGN KEY(ProcessID) REFERENCES ProcessData(ProcessID), "
																			"UNIQUE(RunID, CallPathID, ProcessID, NodeEntryID, NodeExitID), "
																		 	"PRIMARY KEY(TraceTimeID)"
																		 	");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaTraceTimeData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findTraceTimeDataID(TTSQLite3& dataAccess, TT_TraceTiming d, int *traceTimeID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT TraceTimeID FROM TraceTimeData WHERE "
										      "RunID = ? AND "
											  "CallPathID = ? AND "
											  "ProcessID = ? AND "
											  "NodeEntryID = ? AND "
											  "NodeExitID = ? AND "
											  "WallTime = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(   pStmt,1, dataAccess.runID);
					sqlite3_bind_int(   pStmt,2, d.callPathID);
					sqlite3_bind_int(   pStmt,3, d.processID);
					sqlite3_bind_int(   pStmt,4, d.nodeEntryID);
					sqlite3_bind_int(   pStmt,5, d.nodeExitID);
					sqlite3_bind_double(pStmt,6, d.walltime);

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findTraceParameterIntDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findTraceParameterIntDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findTraceParameterIntDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						sqlite3_free(expandedQuery);

						if(traceTimeID!=nullptr) *traceTimeID = -1;
					}
					else if(err == SQLITE_ROW) {
						if(traceTimeID!=nullptr) *traceTimeID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(traceTimeID!=nullptr) *traceTimeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceTimeData(TTSQLite3& dataAccess, TT_TraceTiming d, int *traceTimeID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.traceTimeRecords.push_back(d);
						if(traceTimeID!=nullptr) *traceTimeID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findTraceTimeDataID(dataAccess, d, &tmpID);
						if (tmpID != -1) {
							if(traceTimeID!=nullptr) *traceTimeID = tmpID;
							return;
						}
					}

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceTimeData VALUES(NULL, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

					sqlite3_bind_int(   pStmt,1, dataAccess.runID);
					sqlite3_bind_int(   pStmt,2, d.callPathID);
					sqlite3_bind_int(   pStmt,3, d.processID);
					sqlite3_bind_int(   pStmt,4, d.nodeEntryID);
					sqlite3_bind_int(   pStmt,5, d.nodeExitID);
					sqlite3_bind_double(pStmt,6, d.walltime);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeTraceTimeData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeTraceTimeData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeTraceTimeData - unknown error code " << err << std::endl;
						}
						
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);

						if(traceTimeID!=nullptr) *traceTimeID = -1;
					}
					else {
						if(traceTimeID!=nullptr) *traceTimeID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
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
