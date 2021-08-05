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

				void findAggregateTimeDataID(TTSQLite3& dataAccess,
											 int runID, int callPathID, int processID,
											 double minWallTime, double avgWallTime, double maxWallTime, double stdev, int count,
											 int * aggTimeID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggTimeID FROM AggregateTime WHERE "
										      "RunID = ? AND "
											  "CallPathID = ? AND "
											  "ProcessID = ? AND "
							  	  	  	  	  "MinWallTime = ? AND "
											  "AvgWallTime = ? AND "
							  	  	  	  	  "MaxWallTime = ? AND "
							  	  	  	  	  "StdDev = ? AND "
							  	  	  	  	  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, callPathID);
					sqlite3_bind_int(pStmt,3, processID);
					sqlite3_bind_double(pStmt,4, minWallTime);
					sqlite3_bind_double(pStmt,5, avgWallTime);
					sqlite3_bind_double(pStmt,6, maxWallTime);
					sqlite3_bind_double(pStmt,7, stdev);
					sqlite3_bind_int(pStmt,8, count);
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

				void writeAggregateTimeData(TTSQLite3& dataAccess,
											 int runID, int callPathID, int processID,
											 double minWallTime, double avgWallTime, double maxWallTime, double stdev, int count,
											 int * aggTimeID)
				{
					sqlite3_stmt * pStmt;
					int err;

					if (count == 0) {
						minWallTime = 0.0;
						avgWallTime = 0.0;
						maxWallTime = 0.0;
						stdev = 0.0;
					}

					// Check for existing entry
					int tmpID;
					findAggregateTimeDataID(dataAccess,
							 runID, callPathID, processID,
							 minWallTime, avgWallTime, maxWallTime, stdev, count,
							 &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateTime VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, callPathID);
						sqlite3_bind_int(pStmt,3, processID);
						sqlite3_bind_double(pStmt,4, minWallTime);
						sqlite3_bind_double(pStmt,5, avgWallTime);
						sqlite3_bind_double(pStmt,6, maxWallTime);
						sqlite3_bind_double(pStmt,7, stdev);
						sqlite3_bind_int(pStmt,8, count);
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
