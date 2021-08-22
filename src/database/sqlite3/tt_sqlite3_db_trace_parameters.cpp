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
																			"NodeEntryID, "
																			"NodeExitID, "
																			"ParamName TEXT, "
																			"ParamValue INTEGER, "
																			"PRIMARY KEY(TraceParamIntID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);


					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterFloatData(TraceParamFloatID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryID, "
																			"NodeExitID, "
																			"ParamName TEXT, "
																			"ParamValue REAL, "
																			"PRIMARY KEY(TraceParamFloatID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);


					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterBoolData(TraceParamBoolID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryID, "
																			"NodeExitID, "
																			"ParamName TEXT, "
																			"ParamValue INTEGER, "
																			"PRIMARY KEY(TraceParamBoolID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					stmt = "CREATE TABLE IF NOT EXISTS TraceParameterStringData(TraceParamStringID INTEGER, "
																			"RunID INTEGER, "
																			"ProcessID INTEGER, "
																			"CallPathID INTEGER, "
																			"NodeEntryID, "
																			"NodeExitID, "
																			"ParamName TEXT, "
																			"ParamValue TEXT, "
																			"PRIMARY KEY(TraceParamStringID)"
																			");";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findTraceParameterIntDataID(TTSQLite3& dataAccess,
												 int processID, int callPathID, int nodeEntryID, 
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
											  "NodeEntryID = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in findTraceParameterIntDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamIntID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_text(pStmt,5, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,6, paramValue);

					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in findTraceParameterIntDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamIntID = -1;
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
												 int processID, int callPathID, int nodeEntryID, 
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
											  "NodeEntryID = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in findTraceParameterFloatDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamDoubleID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_text(pStmt,5, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,6, paramValue);

					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in findTraceParameterFloatDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamDoubleID = -1;
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
												 int processID, int callPathID, int nodeEntryID, 
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
											  "NodeEntryID = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in findTraceParameterBoolDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamBoolID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_text(pStmt,5, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,6, paramValue);

					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in findTraceParameterBoolDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamBoolID = -1;
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
												 int processID, int callPathID, int nodeEntryID, 
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
											  "NodeEntryID = ? AND "
											  "ParamName LIKE ? AND "
											  "ParamValue LIKE ?",
											  -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in findTraceParameterStringDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamStringID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_text(pStmt,5, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,6, paramValue.c_str(), -1, SQLITE_TRANSIENT);

					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in findTraceParameterStringDataID (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamStringID = -1;
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
												 int processID, int callPathID, int nodeEntryID, int nodeExitID, 
												 std::string paramName, int paramValue,
												 int * traceParamIntID)
				{
					sqlite3_stmt * pStmt;
					int err;

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterIntData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in writeTraceParameterIntData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamIntID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_int(pStmt,5, nodeExitID);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,7, paramValue);
					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in writeTraceParameterIntData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamIntID = -1;
					}
					else
					{
						*traceParamIntID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceParameterFloatData(TTSQLite3& dataAccess,
											 int processID, int callPathID, int nodeEntryID, int nodeExitID, 
											 std::string paramName, double paramValue,
											 int * traceParamFloatID)
				{
					sqlite3_stmt * pStmt;
					int err;

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterFloatData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in writeTraceParameterFloatData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamFloatID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_int(pStmt,5, nodeExitID);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,7, paramValue);
					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in writeTraceParameterFloatData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamFloatID = -1;
					}
					else
					{
						*traceParamFloatID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceParameterBoolData(TTSQLite3& dataAccess,
												 int processID, int callPathID, int nodeEntryID, int nodeExitID, 
												 std::string paramName, int paramValue,
												 int * traceParamBoolID)
				{
					sqlite3_stmt * pStmt;
					int err;

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterBoolData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in writeTraceParameterBoolData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamBoolID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_int(pStmt,5, nodeExitID);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,7, paramValue);
					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in writeTraceParameterBoolData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamBoolID = -1;
					}
					else
					{
						*traceParamBoolID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}


				void writeTraceParameterStringData(TTSQLite3& dataAccess,
												 int processID, int callPathID, int nodeEntryID, int nodeExitID, 
												 std::string paramName, std::string paramValue,
												 int * traceParamStringID)
				{
					sqlite3_stmt * pStmt;
					int err;

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO TraceParameterStringData VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);
					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_prepare() failed in writeTraceParameterStringData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;
						*traceParamStringID = -1;
						return;
					}

					sqlite3_bind_int(pStmt,1, dataAccess.runID);
					sqlite3_bind_int(pStmt,2, processID);
					sqlite3_bind_int(pStmt,3, callPathID);
					sqlite3_bind_int(pStmt,4, nodeEntryID);
					sqlite3_bind_int(pStmt,5, nodeExitID);
					sqlite3_bind_text(pStmt,6, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,7, paramValue.c_str(), -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if (err==SQLITE_ERROR || err==SQLITE_BUSY || err==SQLITE_MISUSE) {
						std::cout << "SQL sqlite3_step() failed in writeTraceParameterStringData (error=";
						if (err==SQLITE_BUSY) {
							std::cout << "SQLITE_BUSY";
						} else if (err==SQLITE_MISUSE) {
							std::cout << "SQLITE_MISUSE";
						} else {
							std::cout << "SQLITE_ERROR";
						}
						std::cout << ")" << std::endl;

						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						*traceParamStringID = -1;
					}
					else
					{
						*traceParamStringID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}
			}
		}
	}
}
