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
					char *zErrMsg = 0;
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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaTraceParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}


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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaTraceParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}


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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaTraceParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaTraceParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findTraceParameterIntDataID(TTSQLite3& dataAccess, TT_TraceParamInt p, int *traceParamIntID)
				{
					sqlite3_stmt *pStmt = nullptr;

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
						if(traceParamIntID!=nullptr) *traceParamIntID = -1;
						return;
					}

					sqlite3_bind_int (pStmt,1, dataAccess.runID);
					sqlite3_bind_int (pStmt,2, p.processID);
					sqlite3_bind_int (pStmt,3, p.callPathID);
					sqlite3_bind_int (pStmt,4, p.nodeEntryID);
					sqlite3_bind_text(pStmt,5, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_int (pStmt,6, p.paramValue);

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

						if(traceParamIntID!=nullptr) *traceParamIntID = -1;
					}
					else if(err == SQLITE_ROW) {
						if(traceParamIntID!=nullptr) *traceParamIntID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(traceParamIntID!=nullptr) *traceParamIntID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findTraceParameterFloatDataID(TTSQLite3& dataAccess, TT_TraceParamFloat p, int *traceParamFloatID)
				{
					sqlite3_stmt *pStmt = nullptr;

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
						if(traceParamFloatID!=nullptr) *traceParamFloatID = -1;
						return;
					}

					sqlite3_bind_int   (pStmt,1, dataAccess.runID);
					sqlite3_bind_int   (pStmt,2, p.processID);
					sqlite3_bind_int   (pStmt,3, p.callPathID);
					sqlite3_bind_int   (pStmt,4, p.nodeEntryID);
					sqlite3_bind_text  (pStmt,5, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,6, p.paramValue);

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findTraceParameterFloatDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findTraceParameterFloatDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findTraceParameterFloatDataID - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamFloatID!=nullptr) *traceParamFloatID = -1;
					}
					else if(err == SQLITE_ROW) {
						if(traceParamFloatID!=nullptr) *traceParamFloatID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(traceParamFloatID!=nullptr) *traceParamFloatID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findTraceParameterBoolDataID(TTSQLite3& dataAccess, TT_TraceParamBool p, int *traceParamBoolID)
				{
					sqlite3_stmt *pStmt = nullptr;

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
						if(traceParamBoolID!=nullptr) *traceParamBoolID = -1;
						return;
					}

					sqlite3_bind_int (pStmt,1, dataAccess.runID);
					sqlite3_bind_int (pStmt,2, p.processID);
					sqlite3_bind_int (pStmt,3, p.callPathID);
					sqlite3_bind_int (pStmt,4, p.nodeEntryID);
					sqlite3_bind_text(pStmt,5, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_int (pStmt,6, p.paramValue);

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findTraceParameterBoolDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findTraceParameterBoolDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findTraceParameterBoolDataID - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamBoolID!=nullptr) *traceParamBoolID = -1;
					} 
					else if(err == SQLITE_ROW) {
						if(traceParamBoolID!=nullptr) *traceParamBoolID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(traceParamBoolID!=nullptr) *traceParamBoolID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findTraceParameterStringDataID(TTSQLite3& dataAccess, TT_TraceParamString p, int *traceParamStringID)
				{
					sqlite3_stmt *pStmt = nullptr;

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
						if(traceParamStringID!=nullptr) *traceParamStringID = -1;
						return;
					}

					sqlite3_bind_int (pStmt,1, dataAccess.runID);
					sqlite3_bind_int (pStmt,2, p.processID);
					sqlite3_bind_int (pStmt,3, p.callPathID);
					sqlite3_bind_int (pStmt,4, p.nodeEntryID);
					sqlite3_bind_text(pStmt,5, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,6, p.paramValue, -1, SQLITE_TRANSIENT);

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findTraceParameterStringDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findTraceParameterStringDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findTraceParameterStringDataID - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamStringID!=nullptr) *traceParamStringID = -1;
					}
					else if(err == SQLITE_ROW) {
						if(traceParamStringID!=nullptr) *traceParamStringID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(traceParamStringID!=nullptr) *traceParamStringID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceParameterIntData(TTSQLite3& dataAccess, TT_TraceParamInt p, int *traceParamIntID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.traceParamIntRecords.push_back(p);
						if(traceParamIntID!=nullptr) *traceParamIntID = -1;
						return;
					}

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findTraceParameterIntDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(traceParamIntID!=nullptr) *traceParamIntID = tmpID;
							return;
						}
					}

					sqlite3_stmt *pStmt = nullptr;
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
						if(traceParamIntID!=nullptr) *traceParamIntID = -1;
						return;
					}

					sqlite3_bind_int ( pStmt,1, dataAccess.runID);
					sqlite3_bind_int ( pStmt,2, p.processID);
					sqlite3_bind_int ( pStmt,3, p.callPathID);
					sqlite3_bind_int ( pStmt,4, p.nodeEntryID);
					sqlite3_bind_int ( pStmt,5, p.nodeExitID);
					sqlite3_bind_text( pStmt,6, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_int ( pStmt,7, p.paramValue);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeTraceParameterIntData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeTraceParameterIntData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeTraceParameterIntData - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamIntID!=nullptr) *traceParamIntID = -1;
					}
					else {
						if(traceParamIntID!=nullptr) *traceParamIntID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceParameterFloatData(TTSQLite3& dataAccess, TT_TraceParamFloat p, int *traceParamFloatID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.traceParamFloatRecords.push_back(p);
						if(traceParamFloatID!=nullptr) *traceParamFloatID = -1;
						return;
					}

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findTraceParameterFloatDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(traceParamFloatID!=nullptr) *traceParamFloatID = tmpID;
							return;
						}
					}

					sqlite3_stmt *pStmt = nullptr;
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
						if(traceParamFloatID!=nullptr) *traceParamFloatID = -1;
						return;
					}

					sqlite3_bind_int   (pStmt,1, dataAccess.runID);
					sqlite3_bind_int   (pStmt,2, p.processID);
					sqlite3_bind_int   (pStmt,3, p.callPathID);
					sqlite3_bind_int   (pStmt,4, p.nodeEntryID);
					sqlite3_bind_int   (pStmt,5, p.nodeExitID);
					sqlite3_bind_text  (pStmt,6, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,7, p.paramValue);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeTraceParameterFloatData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeTraceParameterFloatData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeTraceParameterFloatData - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamFloatID!=nullptr) *traceParamFloatID = -1;
					}
					else {
						if(traceParamFloatID!=nullptr) *traceParamFloatID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				void writeTraceParameterBoolData(TTSQLite3& dataAccess, TT_TraceParamBool p, int *traceParamBoolID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.traceParamBoolRecords.push_back(p);
						if(traceParamBoolID!=nullptr) *traceParamBoolID = -1;
						return;
					}

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findTraceParameterBoolDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(traceParamBoolID!=nullptr) *traceParamBoolID = tmpID;
							return;
						}
					}

					sqlite3_stmt *pStmt = nullptr;
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
						if(traceParamBoolID!=nullptr) *traceParamBoolID = -1;
						return;
					}

					sqlite3_bind_int (pStmt,1, dataAccess.runID);
					sqlite3_bind_int (pStmt,2, p.processID);
					sqlite3_bind_int (pStmt,3, p.callPathID);
					sqlite3_bind_int (pStmt,4, p.nodeEntryID);
					sqlite3_bind_int (pStmt,5, p.nodeExitID);
					sqlite3_bind_text(pStmt,6, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_int (pStmt,7, p.paramValue);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeTraceParameterBoolData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeTraceParameterBoolData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeTraceParameterBoolData - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamBoolID!=nullptr) *traceParamBoolID = -1;
					}
					else {
						if(traceParamBoolID!=nullptr) *traceParamBoolID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}


				void writeTraceParameterStringData(TTSQLite3& dataAccess, TT_TraceParamString p, int *traceParamStringID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.traceParamStringRecords.push_back(p);
						if(traceParamStringID!=nullptr) *traceParamStringID = -1;
						return;
					}

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findTraceParameterStringDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(traceParamStringID!=nullptr) *traceParamStringID = tmpID;
							return;
						}
					}

					sqlite3_stmt *pStmt = nullptr;
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
						if(traceParamStringID!=nullptr) *traceParamStringID = -1;
						return;
					}

					sqlite3_bind_int (pStmt,1, dataAccess.runID);
					sqlite3_bind_int (pStmt,2, p.processID);
					sqlite3_bind_int (pStmt,3, p.callPathID);
					sqlite3_bind_int (pStmt,4, p.nodeEntryID);
					sqlite3_bind_int (pStmt,5, p.nodeExitID);
					sqlite3_bind_text(pStmt,6, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,7, p.paramValue, -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeTraceParameterStringData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeTraceParameterStringData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeTraceParameterStringData - unknown error code " << err << std::endl;
						}

						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
						sqlite3_free(expandedQuery);

						if(traceParamStringID!=nullptr) *traceParamStringID = -1;
					}
					else {
						if(traceParamStringID!=nullptr) *traceParamStringID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				MPI_Datatype createTraceParamIntMpiType()
				{
					// Create MPI type for a TraceParamInt record:
					int err;
					MPI_Datatype traceParamIntRecord_MPI, tmpType;

					int lengths[7] = {1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1};
					MPI_Aint displacements[7];
					displacements[0] = offsetof(TT_TraceParamInt, rank);
					displacements[1] = offsetof(TT_TraceParamInt, callPathID);
					displacements[2] = offsetof(TT_TraceParamInt, processID);
					displacements[3] = offsetof(TT_TraceParamInt, paramName);
					displacements[4] = offsetof(TT_TraceParamInt, paramValue);
					displacements[5] = offsetof(TT_TraceParamInt, nodeEntryID);
					displacements[6] = offsetof(TT_TraceParamInt, nodeExitID);
					MPI_Datatype types[7] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_INT, 
												MPI_INT, MPI_INT };
					err = MPI_Type_create_struct(7, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for traceParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for traceParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &traceParamIntRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for traceParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&traceParamIntRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for traceParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return traceParamIntRecord_MPI;
				}

				MPI_Datatype createTraceParamFloatMpiType()
				{
					// Create MPI type for a TraceParamFloat record:
					int err;
					MPI_Datatype traceParamFloatRecord_MPI, tmpType;

					int lengths[7] = {1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1};
					MPI_Aint displacements[7];
					displacements[0] = offsetof(TT_TraceParamFloat, rank);
					displacements[1] = offsetof(TT_TraceParamFloat, callPathID);
					displacements[2] = offsetof(TT_TraceParamFloat, processID);
					displacements[3] = offsetof(TT_TraceParamFloat, paramName);
					displacements[4] = offsetof(TT_TraceParamFloat, paramValue);
					displacements[5] = offsetof(TT_TraceParamFloat, nodeEntryID);
					displacements[6] = offsetof(TT_TraceParamFloat, nodeExitID);
					MPI_Datatype types[7] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_DOUBLE, 
												MPI_INT, MPI_INT };
					err = MPI_Type_create_struct(7, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for traceParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for traceParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &traceParamFloatRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for traceParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&traceParamFloatRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for traceParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return traceParamFloatRecord_MPI;
				}

				MPI_Datatype createTraceParamBoolMpiType()
				{
					// Create MPI type for a TraceParamBool record:
					int err;
					MPI_Datatype traceParamBoolRecord_MPI, tmpType;

					int lengths[7] = {1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1};
					MPI_Aint displacements[7];
					displacements[0] = offsetof(TT_TraceParamBool, rank);
					displacements[1] = offsetof(TT_TraceParamBool, callPathID);
					displacements[2] = offsetof(TT_TraceParamBool, processID);
					displacements[3] = offsetof(TT_TraceParamBool, paramName);
					displacements[4] = offsetof(TT_TraceParamBool, paramValue);
					displacements[5] = offsetof(TT_TraceParamBool, nodeEntryID);
					displacements[6] = offsetof(TT_TraceParamBool, nodeExitID);
					MPI_Datatype types[7] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_INT, 
												MPI_INT, MPI_INT };
					err = MPI_Type_create_struct(7, lengths, displacements, types, &tmpType);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for traceParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for traceParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &traceParamBoolRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for traceParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&traceParamBoolRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for traceParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return traceParamBoolRecord_MPI;
				}

				MPI_Datatype createTraceParamStringMpiType()
				{
					// Create MPI type for a TraceParamString record:
					int err;
					MPI_Datatype traceParamStringRecord_MPI, tmpType;

					int lengths[7] = {1, 1, 1, MAX_STRING_LENGTH, MAX_STRING_LENGTH, 1, 1};
					MPI_Aint displacements[7];
					displacements[0] = offsetof(TT_TraceParamString, rank);
					displacements[1] = offsetof(TT_TraceParamString, callPathID);
					displacements[2] = offsetof(TT_TraceParamString, processID);
					displacements[3] = offsetof(TT_TraceParamString, paramName);
					displacements[4] = offsetof(TT_TraceParamString, paramValue);
					displacements[5] = offsetof(TT_TraceParamString, nodeEntryID);
					displacements[6] = offsetof(TT_TraceParamString, nodeExitID);
					MPI_Datatype types[7] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_CHAR, 
												MPI_INT, MPI_INT };
					err = MPI_Type_create_struct(7, lengths, displacements, types, &tmpType);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for traceParamStringRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for traceParamStringRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &traceParamStringRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for traceParamStringRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&traceParamStringRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for traceParamStringRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return traceParamStringRecord_MPI;
				}
			}
		}
	}
}
