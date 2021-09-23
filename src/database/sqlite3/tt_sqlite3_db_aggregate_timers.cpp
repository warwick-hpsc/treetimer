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

#include "tt_sqlite3_db_aggregate_timers.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaAggregateTimeData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS AggregateTime(AggTimeID INTEGER, "
																				"RunID INTEGER, "
																				"ProcessID INTEGER, "
																				"Window INTEGER, "
																				"CallPathID INTEGER, "
																				"MinWallTime REAL, "
																				"AvgWallTime REAL, "
																				"MaxWallTime REAL, "
																				"StdDev REAL, "
																				"Count INTEGER, "
																				"FOREIGN KEY(RunID) REFERENCES ProfileRunConfigData(RunID),"
																				"FOREIGN KEY(CallPathID) REFERENCES CallPathData(CallPathID), "
																				"FOREIGN KEY(ProcessID) REFERENCES ProcessData(ProcessID), "
																				"PRIMARY KEY(AggTimeID)"
																				");";
					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaAggregateTimeData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findAggregateTimeDataID(TTSQLite3& dataAccess, TT_AggTiming d, int *aggTimeID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
												"SELECT AggTimeID FROM AggregateTime WHERE "
												"RunID = ? AND "
												"ProcessID = ? AND "
												"Window = ? AND "
												"CallPathID = ? AND "
												"MinWallTime = ? AND "
												"AvgWallTime = ? AND "
												"MaxWallTime = ? AND "
												"StdDev = ? AND "
												"Count = ?",
												-1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int(   pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int(   pStmt,i, d.processID); i++;
					sqlite3_bind_int(   pStmt,i, d.window); i++;
					sqlite3_bind_int(   pStmt,i, d.callPathID); i++;
					sqlite3_bind_double(pStmt,i, d.minWallTime); i++;
					sqlite3_bind_double(pStmt,i, d.avgWallTime); i++;
					sqlite3_bind_double(pStmt,i, d.maxWallTime); i++;
					sqlite3_bind_double(pStmt,i, d.stdev); i++;
					sqlite3_bind_int(   pStmt,i, d.count); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findAggregateTimeDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findAggregateTimeDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findAggregateTimeDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(aggTimeID!=nullptr) *aggTimeID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(aggTimeID!=nullptr) *aggTimeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeAggregateTimeData(TTSQLite3& dataAccess, TT_AggTiming d, int *aggTimeID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggTimeRecords.push_back(d);
						if(aggTimeID!=nullptr) *aggTimeID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					if (d.count == 0) {
						d.minWallTime = 0.0;
						d.avgWallTime = 0.0;
						d.maxWallTime = 0.0;
						d.stdev = 0.0;
					}

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findAggregateTimeDataID(dataAccess, d, &tmpID);
						if (tmpID != -1) {
							if(aggTimeID!=nullptr) *aggTimeID = tmpID;
							return;
						}
					}

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateTime VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);
					int i=1;
					sqlite3_bind_int(   pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int(   pStmt,i, d.processID); i++;
					sqlite3_bind_int(   pStmt,i, d.window); i++;
					sqlite3_bind_int(   pStmt,i, d.callPathID); i++;
					sqlite3_bind_double(pStmt,i, d.minWallTime); i++;
					sqlite3_bind_double(pStmt,i, d.avgWallTime); i++;
					sqlite3_bind_double(pStmt,i, d.maxWallTime); i++;
					sqlite3_bind_double(pStmt,i, d.stdev); i++;
					sqlite3_bind_int(   pStmt,i, d.count); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeAggregateTimeData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeAggregateTimeData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeAggregateTimeData - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
						if(aggTimeID!=nullptr) *aggTimeID = -1;
					}
					else {
						if(aggTimeID!=nullptr) *aggTimeID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				MPI_Datatype createAggregateTimeMpiType()
				{
					// Create MPI type for a AggregateTime record:
					int err;
					MPI_Datatype aggTimeRecord_MPI, tmpType;

					int lengths[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					int i=0;
					displacements[i] = offsetof(TT_AggTiming, rank); i++;
					displacements[i] = offsetof(TT_AggTiming, processID); i++;
					displacements[i] = offsetof(TT_AggTiming, window); i++;
					displacements[i] = offsetof(TT_AggTiming, callPathID); i++;
					displacements[i] = offsetof(TT_AggTiming, minWallTime); i++;
					displacements[i] = offsetof(TT_AggTiming, avgWallTime); i++;
					displacements[i] = offsetof(TT_AggTiming, maxWallTime); i++;
					displacements[i] = offsetof(TT_AggTiming, stdev); i++;
					displacements[i] = offsetof(TT_AggTiming, count); i++;
					MPI_Datatype types[9] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, 
												MPI_INT };
					err = MPI_Type_create_struct(9, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for aggTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for aggTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &aggTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for aggTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&aggTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for aggTimeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return aggTimeRecord_MPI;
				}

			}
		}
	}
}
