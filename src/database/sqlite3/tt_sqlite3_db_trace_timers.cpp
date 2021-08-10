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

				void findTraceTimeDataID(TTSQLite3& dataAccess, TTTraceTiming d, int * traceTimeID)
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

					sqlite3_bind_int(   pStmt,1, d.runID);
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

				void writeTraceTimeData(TTSQLite3& dataAccess, TTTraceTiming d, int * traceTimeID)
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

						sqlite3_bind_int(   pStmt,1, d.runID);
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
			}
		}
	}
}
