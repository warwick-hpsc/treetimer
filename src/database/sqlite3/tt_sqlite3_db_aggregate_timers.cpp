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
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS AggregateTime(AggTimeID INTEGER, "
																				"RunID INTEGER, "
																				"Rank INTEGER, "
																				"CallPathID INTEGER, "
																				"ProcessID INTEGER, "
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
				}

				void findAggregateTimeDataID(TTSQLite3& dataAccess, aggTimeData d, int * aggTimeID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
												"SELECT AggTimeID FROM AggregateTime WHERE "
												"RunID = ? AND "
												"Rank = ? AND "
												"CallPathID = ? AND "
												"ProcessID = ? AND "
												"MinWallTime = ? AND "
												"AvgWallTime = ? AND "
												"MaxWallTime = ? AND "
												"StdDev = ? AND "
												"Count = ?",
												-1, &pStmt, NULL);

					sqlite3_bind_int(   pStmt,1, d.runID);
					sqlite3_bind_int(   pStmt,2, d.rank);
					sqlite3_bind_int(   pStmt,3, d.callPathID);
					sqlite3_bind_int(   pStmt,4, d.processID);
					sqlite3_bind_double(pStmt,5, d.minWallTime);
					sqlite3_bind_double(pStmt,6, d.avgWallTime);
					sqlite3_bind_double(pStmt,7, d.maxWallTime);
					sqlite3_bind_double(pStmt,8, d.stdev);
					sqlite3_bind_int(   pStmt,9, d.count);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findAggregateTimeDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*aggTimeID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*aggTimeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeAggregateTimeData(TTSQLite3& dataAccess, aggTimeData d, int * aggTimeID)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggTimeRecords.push_back(d);
						*aggTimeID = -1;
						return;
					}

					sqlite3_stmt * pStmt;
					int err;

					if (d.count == 0) {
						d.minWallTime = 0.0;
						d.avgWallTime = 0.0;
						d.maxWallTime = 0.0;
						d.stdev = 0.0;
					}

					// Check for existing entry
					int tmpID = -1;
					findAggregateTimeDataID(dataAccess, d, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateTime VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(   pStmt,1, d.runID);
						sqlite3_bind_int(   pStmt,2, d.rank);
						sqlite3_bind_int(   pStmt,3, d.callPathID);
						sqlite3_bind_int(   pStmt,4, d.processID);
						sqlite3_bind_double(pStmt,5, d.minWallTime);
						sqlite3_bind_double(pStmt,6, d.avgWallTime);
						sqlite3_bind_double(pStmt,7, d.maxWallTime);
						sqlite3_bind_double(pStmt,8, d.stdev);
						sqlite3_bind_int(   pStmt,9, d.count);
						err = sqlite3_step(pStmt);


						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeAggregateTimeData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*aggTimeID = -1;
						}
						else
						{
							*aggTimeID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*aggTimeID = tmpID;
					}
				}
			}
		}
	}
}
