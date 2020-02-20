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

#include "tt_sqlite3_db_aggregate_parameters.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaAggregateParameterData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					std::string stmt;
					int err;

					stmt = "CREATE TABLE IF NOT EXISTS AggregateParameterInt(AggParamIntID INTEGER, "
																				  "RunID INTEGER, "
																				  "CallPathID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ParamName TEXT, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "MinValue INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "AvgValue REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "MaxValue INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "StdDev REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "Count INTEGER, "
																				  "FOREIGN KEY(RunID) REFERENCES ProfileRunConfigData(RunID), "
																				  "FOREIGN KEY(CallPathID) REFERENCES CallPathData(CallPathID), "
																				  "PRIMARY KEY(AggParamIntID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in writeSchemaAggregateParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					stmt = "CREATE TABLE IF NOT EXISTS AggregateParameterFloat(AggParamFloatID INTEGER, "
																				  "RunID INTEGER, "
																				  "CallPathID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ParamName TEXT, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "MinValue REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "AvgValue REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "MaxValue REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "StdDev REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "Count INTEGER, "
																				  "FOREIGN KEY(RunID) REFERENCES ProfileRunConfigData(RunID), "
																				  "FOREIGN KEY(CallPathID) REFERENCES CallPathData(CallPathID), "
																				  "PRIMARY KEY(AggParamFloatID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in writeSchemaAggregateParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					stmt = "CREATE TABLE IF NOT EXISTS AggregateParameterBool(AggParamBoolID INTEGER, "
																				  "RunID INTEGER, "
																				  "CallPathID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ParamName TEXT, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "MinValue INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "AvgValue REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "MaxValue INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "StdDev REAL, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "Count INTEGER, "
																				  "FOREIGN KEY(RunID) REFERENCES ProfileRunConfigData(RunID), "
																				  "FOREIGN KEY(CallPathID) REFERENCES CallPathData(CallPathID), "
																				  "PRIMARY KEY(AggParamBoolID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in writeSchemaAggregateParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findAggregateParameterIntDataID(TTSQLite3& dataAccess,
										   int runID, int callPathID, int processID, std::string paramName,
										   int minValue, double avgValue, int maxValue, double stddev, int count,
										   int * aggParamIntID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggParamIntID FROM AggregateParameterInt WHERE "
										      "RunID = ? AND "
											  "CallPathID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, callPathID);
					sqlite3_bind_int(pStmt,3, processID);
					sqlite3_bind_text(pStmt,4, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,5, minValue);
					sqlite3_bind_double(pStmt,6, avgValue);
					sqlite3_bind_int(pStmt,7, maxValue);
					sqlite3_bind_double(pStmt,8, stddev);
					sqlite3_bind_int(pStmt,9, count);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findAggregateParameterIntDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*aggParamIntID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*aggParamIntID = -1;
					}

					sqlite3_finalize(pStmt);
				}



				void findAggregateParameterFloatDataID(TTSQLite3& dataAccess,
										   int runID, int callPathID, int processID, std::string paramName,
										   double minValue, double avgValue, double maxValue, double stddev, int count,
										   int * aggParamFloatID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggParamFloatID FROM AggregateParameterFloat WHERE "
										      "RunID = ? AND "
											  "CallPathID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, callPathID);
					sqlite3_bind_int(pStmt,3, processID);
					sqlite3_bind_text(pStmt,4, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,5, minValue);
					sqlite3_bind_double(pStmt,6, avgValue);
					sqlite3_bind_double(pStmt,7, maxValue);
					sqlite3_bind_double(pStmt,8, stddev);
					sqlite3_bind_int(pStmt,9, count);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findAggregateParameterFloatDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*aggParamFloatID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*aggParamFloatID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findAggregateParameterBoolDataID(TTSQLite3& dataAccess,
										   int runID, int callPathID, int processID, std::string paramName,
										   int minValue, double avgValue, int maxValue, double stddev, int count,
										   int * aggParamBoolID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggParamBoolID FROM AggregateParameterBool WHERE "
										      "RunID = ? AND "
											  "CallPathID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, callPathID);
					sqlite3_bind_int(pStmt,3, processID);
					sqlite3_bind_text(pStmt,4, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,5, minValue);
					sqlite3_bind_double(pStmt,6, avgValue);
					sqlite3_bind_double(pStmt,7, maxValue);
					sqlite3_bind_double(pStmt,8, stddev);
					sqlite3_bind_int(pStmt,9, count);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findAggregateParameterBoolDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*aggParamBoolID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*aggParamBoolID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeAggregateParameterIntData(TTSQLite3& dataAccess,
						   int runID, int callPathID, int processID, std::string paramName,
						   int minValue, double avgValue, int maxValue, double stddev, int count,
						   int * aggParamIntID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findAggregateParameterIntDataID(dataAccess, runID, callPathID, processID,paramName,
												    minValue, avgValue, maxValue, stddev, count,
												    &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterInt VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, callPathID);
						sqlite3_bind_int(pStmt,3, processID);
						sqlite3_bind_text(pStmt,4, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,5, minValue);
						sqlite3_bind_double(pStmt,6, avgValue);
						sqlite3_bind_int(pStmt,7, maxValue);
						sqlite3_bind_double(pStmt,8, stddev);
						sqlite3_bind_int(pStmt,9, count);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeAggregateParameterIntData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*aggParamIntID = -1;
						}
						else
						{
							*aggParamIntID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*aggParamIntID = tmpID;
					}
				}


				void writeAggregateParameterFloatData(TTSQLite3& dataAccess,
						   int runID, int callPathID, int processID, std::string paramName,
						   double minValue, double avgValue, double maxValue, double stddev, int count,
						   int * aggParamFloatID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findAggregateParameterFloatDataID(dataAccess, runID, callPathID, processID,paramName,
												    minValue, avgValue, maxValue, stddev, count,
												    &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterFloat VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, callPathID);
						sqlite3_bind_int(pStmt,3, processID);
						sqlite3_bind_text(pStmt,4, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_double(pStmt,5, minValue);
						sqlite3_bind_double(pStmt,6, avgValue);
						sqlite3_bind_double(pStmt,7, maxValue);
						sqlite3_bind_double(pStmt,8, stddev);
						sqlite3_bind_int(pStmt,9, count);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeAggregateParameterFloatData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*aggParamFloatID = -1;
						}
						else
						{
							*aggParamFloatID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*aggParamFloatID = tmpID;
					}
				}


				void writeAggregateParameterBoolData(TTSQLite3& dataAccess,
						   int runID, int callPathID, int processID, std::string paramName,
						   int minValue, double avgValue, int maxValue, double stddev, int count,
						   int * aggParamBoolID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findAggregateParameterBoolDataID(dataAccess, runID, callPathID, processID,paramName,
												    minValue, avgValue, maxValue, stddev, count,
												    &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterBool VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, callPathID);
						sqlite3_bind_int(pStmt,3, processID);
						sqlite3_bind_text(pStmt,4, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,5, minValue);
						sqlite3_bind_double(pStmt,6, avgValue);
						sqlite3_bind_int(pStmt,7, maxValue);
						sqlite3_bind_double(pStmt,8, stddev);
						sqlite3_bind_int(pStmt,9, count);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeAggregateParameterBoolData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*aggParamBoolID = -1;
						}
						else
						{
							*aggParamBoolID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*aggParamBoolID = tmpID;
					}
				}
			}
		}
	}
}
