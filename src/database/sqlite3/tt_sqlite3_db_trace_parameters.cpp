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

#include "tt_sqlite3_db_trace_parameters.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaTraceParameterData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					std::string stmt;
					int err;

					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterIntData(TraceParamIntID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryCount, "
																			"NodeExitCount, "
																			"ParamName TEXT, "
																			"ParamValue INTEGER, "
																			"PRIMARY KEY(TraceParamIntID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);


					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterFloatData(TraceParamFloatID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryCount, "
																			"NodeExitCount, "
																			"ParamName TEXT, "
																			"ParamValue REAL, "
																			"PRIMARY KEY(TraceParamFloatID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);


					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterBoolData(TraceParamBoolID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryCount, "
																			"NodeExitCount, "
																			"ParamName TEXT, "
																			"ParamValue INTEGER, "
																			"PRIMARY KEY(TraceParamBoolID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterStringData(TraceParamStringID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryCount, "
																			"NodeExitCount, "
																			"ParamName TEXT, "
																			"ParamValue TEXT, "
																			"PRIMARY KEY(TraceParamStringID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findTraceParameterIntDataID(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, int paramValue,
												 int * traceParamIntID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT TraceParamIntID FROM TraceParameterIntData WHERE "
										      "RunID = ? AND "
											  "ProcessID = ? AND "
											  "CallPathID = ? AND "
											  "NodeEntryCount = ? AND "
											  "NodeExitCount = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryCount);
					sqlite3_bind_int(pStmt,5, nodeExitCount);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,7, paramValue);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findTraceParameterIntDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*traceParamIntID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*traceParamIntID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findTraceParameterFloatDataID(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, double paramValue,
												 int * traceParamDoubleID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT TraceParamFloatID FROM TraceParameterFloatData WHERE "
										      "RunID = ? AND "
											  "ProcessID = ? AND "
											  "CallPathID = ? AND "
											  "NodeEntryCount = ? AND "
											  "NodeExitCount = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryCount);
					sqlite3_bind_int(pStmt,5, nodeExitCount);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,7, paramValue);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findTraceParameterFloatDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*traceParamDoubleID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*traceParamDoubleID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findTraceParameterBoolDataID(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, int paramValue,
												 int * traceParamBoolID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT TraceParamBoolID FROM TraceParameterBoolData WHERE "
										      "RunID = ? AND "
											  "ProcessID = ? AND "
											  "CallPathID = ? AND "
											  "NodeEntryCount = ? AND "
											  "NodeExitCount = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryCount);
					sqlite3_bind_int(pStmt,5, nodeExitCount);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,7, paramValue);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findTraceParameterBoolDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*traceParamBoolID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*traceParamBoolID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findTraceParameterStringDataID(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, std::string paramValue,
												 int * traceParamStringID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT TraceParamStringID FROM TraceParameterStringData WHERE "
										      "RunID = ? AND "
											  "ProcessID = ? AND "
											  "CallPathID = ? AND "
											  "NodeEntryCount = ? AND "
											  "NodeExitCount = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue LIKE ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryCount);
					sqlite3_bind_int(pStmt,5, nodeExitCount);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,7, paramValue.c_str(), -1, SQLITE_TRANSIENT);

					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findTraceParameterStringDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*traceParamStringID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*traceParamStringID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceParameterIntData(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, int paramValue,
												 int * traceParamIntID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findTraceParameterIntDataID(dataAccess, runID, processID, callPathID, nodeEntryCount, nodeExitCount,
							 	 	 	 	 	paramName, paramValue, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterIntData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, processID);
						sqlite3_bind_int(pStmt,3, callPathID);
						sqlite3_bind_int(pStmt,4, nodeEntryCount);
						sqlite3_bind_int(pStmt,5, nodeExitCount);
						sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,7, paramValue);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeTraceParameterIntData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*traceParamIntID = -1;
						}
						else
						{
							*traceParamIntID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*traceParamIntID = tmpID;
					}
				}

				void writeTraceParameterFloatData(TTSQLite3& dataAccess,
											 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
											 std::string paramName, double paramValue,
											 int * traceParamFloatID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findTraceParameterFloatDataID(dataAccess, runID, processID, callPathID, nodeEntryCount, nodeExitCount,
												paramName, paramValue, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterFloatData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, processID);
						sqlite3_bind_int(pStmt,3, callPathID);
						sqlite3_bind_int(pStmt,4, nodeEntryCount);
						sqlite3_bind_int(pStmt,5, nodeExitCount);
						sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_double(pStmt,7, paramValue);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeTraceParameterFloatData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*traceParamFloatID = -1;
						}
						else
						{
							*traceParamFloatID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*traceParamFloatID = tmpID;
					}
				}

				void writeTraceParameterBoolData(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, int paramValue,
												 int * traceParamBoolID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findTraceParameterBoolDataID(dataAccess, runID, processID, callPathID, nodeEntryCount, nodeExitCount,
							 	 	 	 	 	paramName, paramValue, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterBoolData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, processID);
						sqlite3_bind_int(pStmt,3, callPathID);
						sqlite3_bind_int(pStmt,4, nodeEntryCount);
						sqlite3_bind_int(pStmt,5, nodeExitCount);
						sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,7, paramValue);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeTraceParameterBoolData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*traceParamBoolID = -1;
						}
						else
						{
							*traceParamBoolID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*traceParamBoolID = tmpID;
					}
				}


				void writeTraceParameterStringData(TTSQLite3& dataAccess,
												 int runID, int processID, int callPathID, int nodeEntryCount, int nodeExitCount,
												 std::string paramName, std::string paramValue,
												 int * traceParamStringID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findTraceParameterStringDataID(dataAccess, runID, processID, callPathID, nodeEntryCount, nodeExitCount,
							 	 	 	 	 	paramName, paramValue, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterStringData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, runID);
						sqlite3_bind_int(pStmt,2, processID);
						sqlite3_bind_int(pStmt,3, callPathID);
						sqlite3_bind_int(pStmt,4, nodeEntryCount);
						sqlite3_bind_int(pStmt,5, nodeExitCount);
						sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_text(pStmt,7, paramValue.c_str(), -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeTraceParameterStringData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*traceParamStringID = -1;
						}
						else
						{
							*traceParamStringID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*traceParamStringID = tmpID;
					}
				}
			}
		}
	}
}
