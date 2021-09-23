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
					char *zErrMsg = 0;
					std::string stmt;
					int err;

					stmt = "CREATE TABLE IF NOT EXISTS AggregateParameterInt(AggParamIntID INTEGER, "
																				  "RunID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "Window INTEGER, "
																				  "CallPathID INTEGER, "
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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaAggregateParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					stmt = "CREATE TABLE IF NOT EXISTS AggregateParameterFloat(AggParamFloatID INTEGER, "
																				  "RunID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "Window INTEGER, "
																				  "CallPathID INTEGER, "
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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaAggregateParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					stmt = "CREATE TABLE IF NOT EXISTS AggregateParameterBool(AggParamBoolID INTEGER, "
																				  "RunID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "Window INTEGER, "
																				  "CallPathID INTEGER, "
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
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaAggregateParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findAggregateParameterIntDataID(TTSQLite3& dataAccess, TT_AggParamInt p, int *aggParamIntID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggParamIntID FROM AggregateParameterInt WHERE "
										      "RunID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
							  	  	  	  	  "Window = ? AND "
											  "CallPathID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int   (pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int   (pStmt,i, p.processID); i++;
					sqlite3_bind_int   (pStmt,i, p.window); i++;
					sqlite3_bind_int   (pStmt,i, p.callPathID); i++;
					sqlite3_bind_text  (pStmt,i, p.paramName, -1, SQLITE_TRANSIENT); i++;
					sqlite3_bind_int   (pStmt,i, p.minValue); i++;
					sqlite3_bind_double(pStmt,i, p.avgValue); i++;
					sqlite3_bind_int   (pStmt,i, p.maxValue); i++;
					sqlite3_bind_double(pStmt,i, p.stdev); i++;
					sqlite3_bind_int   (pStmt,i, p.count); i++;

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findAggregateParameterIntDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findAggregateParameterIntDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findAggregateParameterIntDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(aggParamIntID!=nullptr) *aggParamIntID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(aggParamIntID!=nullptr) *aggParamIntID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findAggregateParameterFloatDataID(TTSQLite3& dataAccess, TT_AggParamFloat p, int *aggParamFloatID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggParamFloatID FROM AggregateParameterFloat WHERE "
										      "RunID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
							  	  	  	  	  "Window = ? AND "
											  "CallPathID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int   (pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int   (pStmt,i, p.processID); i++;
					sqlite3_bind_int   (pStmt,i, p.window); i++;
					sqlite3_bind_int   (pStmt,i, p.callPathID); i++;
					sqlite3_bind_text  (pStmt,i, p.paramName, -1, SQLITE_TRANSIENT); i++;
					sqlite3_bind_double(pStmt,i, p.minValue); i++;
					sqlite3_bind_double(pStmt,i, p.avgValue); i++;
					sqlite3_bind_double(pStmt,i, p.maxValue); i++;
					sqlite3_bind_double(pStmt,i, p.stdev); i++;
					sqlite3_bind_int   (pStmt,i, p.count); i++;

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findAggregateParameterFloatDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findAggregateParameterFloatDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findAggregateParameterFloatDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if (aggParamFloatID!=nullptr) *aggParamFloatID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if (aggParamFloatID!=nullptr) *aggParamFloatID = -1;
					}

					sqlite3_finalize(pStmt);
				}


				void findAggregateParameterBoolDataID(TTSQLite3& dataAccess, TT_AggParamBool p, int *aggParamBoolID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT AggParamBoolID FROM AggregateParameterBool WHERE "
										      "RunID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
							  	  	  	  	  "Window = ? AND "
											  "CallPathID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int   (pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int   (pStmt,i, p.processID); i++;
					sqlite3_bind_int   (pStmt,i, p.window); i++;
					sqlite3_bind_int   (pStmt,i, p.callPathID); i++;
					sqlite3_bind_text  (pStmt,i, p.paramName, -1, SQLITE_TRANSIENT); i++;
					sqlite3_bind_double(pStmt,i, p.minValue); i++;
					sqlite3_bind_double(pStmt,i, p.avgValue); i++;
					sqlite3_bind_double(pStmt,i, p.maxValue); i++;
					sqlite3_bind_double(pStmt,i, p.stdev); i++;
					sqlite3_bind_int   (pStmt,i, p.count); i++;

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findAggregateParameterBoolDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findAggregateParameterBoolDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findAggregateParameterBoolDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(aggParamBoolID!=nullptr) *aggParamBoolID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(aggParamBoolID!=nullptr) *aggParamBoolID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeAggregateParameterIntData(TTSQLite3& dataAccess, TT_AggParamInt p, int *aggParamIntID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggParamIntRecords.push_back(p);
						if(aggParamIntID!=nullptr) *aggParamIntID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findAggregateParameterIntDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(aggParamIntID!=nullptr) *aggParamIntID = tmpID;
							return;
						}
					}

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterInt VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int   (pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int   (pStmt,i, p.processID); i++;
					sqlite3_bind_int   (pStmt,i, p.window); i++;
					sqlite3_bind_int   (pStmt,i, p.callPathID); i++;
					sqlite3_bind_text  (pStmt,i, p.paramName, -1, SQLITE_TRANSIENT); i++;
					sqlite3_bind_int   (pStmt,i, p.minValue); i++;
					sqlite3_bind_double(pStmt,i, p.avgValue); i++;
					sqlite3_bind_int   (pStmt,i, p.maxValue); i++;
					sqlite3_bind_double(pStmt,i, p.stdev); i++;
					sqlite3_bind_int   (pStmt,i, p.count); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeAggregateParameterIntData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeAggregateParameterIntData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeAggregateParameterIntData - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
						if(aggParamIntID!=nullptr) *aggParamIntID = -1;
					}
					else {
						if(aggParamIntID!=nullptr) *aggParamIntID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}


				void writeAggregateParameterFloatData(TTSQLite3& dataAccess, TT_AggParamFloat p, int *aggParamFloatID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggParamFloatRecords.push_back(p);
						if(aggParamFloatID!=nullptr) *aggParamFloatID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findAggregateParameterFloatDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(aggParamFloatID!=nullptr) *aggParamFloatID = tmpID;
							return;
						}
					}

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterFloat VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int   (pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int   (pStmt,i, p.processID); i++;
					sqlite3_bind_int   (pStmt,i, p.window); i++;
					sqlite3_bind_int   (pStmt,i, p.callPathID); i++;
					sqlite3_bind_text  (pStmt,i, p.paramName, -1, SQLITE_TRANSIENT); i++;
					sqlite3_bind_double(pStmt,i, p.minValue); i++;
					sqlite3_bind_double(pStmt,i, p.avgValue); i++;
					sqlite3_bind_double(pStmt,i, p.maxValue); i++;
					sqlite3_bind_double(pStmt,i, p.stdev); i++;
					sqlite3_bind_int   (pStmt,i, p.count); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeAggregateParameterFloatData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeAggregateParameterFloatData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeAggregateParameterFloatData - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
						if(aggParamFloatID!=nullptr) *aggParamFloatID = -1;
					}
					else {
						if(aggParamFloatID!=nullptr) *aggParamFloatID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}


				void writeAggregateParameterBoolData(TTSQLite3& dataAccess, TT_AggParamBool p,  int *aggParamBoolID, bool verifyUnique)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggParamBoolRecords.push_back(p);
						if(aggParamBoolID!=nullptr) *aggParamBoolID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findAggregateParameterBoolDataID(dataAccess, p, &tmpID);
						if (tmpID != -1) {
							if(aggParamBoolID!=nullptr) *aggParamBoolID = tmpID;
							return;
						}
					}

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterBool VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int   (pStmt,i, dataAccess.runID); i++;
					sqlite3_bind_int   (pStmt,i, p.processID); i++;
					sqlite3_bind_int   (pStmt,i, p.window); i++;
					sqlite3_bind_int   (pStmt,i, p.callPathID); i++;
					sqlite3_bind_text  (pStmt,i, p.paramName, -1, SQLITE_TRANSIENT); i++;
					sqlite3_bind_int   (pStmt,i, p.minValue); i++;
					sqlite3_bind_double(pStmt,i, p.avgValue); i++;
					sqlite3_bind_int   (pStmt,i, p.maxValue); i++;
					sqlite3_bind_double(pStmt,i, p.stdev); i++;
					sqlite3_bind_int   (pStmt,i, p.count); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeAggregateParameterBoolData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeAggregateParameterBoolData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeAggregateParameterBoolData - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
						if(aggParamBoolID!=nullptr) *aggParamBoolID = -1;
					}
					else {
						if(aggParamBoolID!=nullptr) *aggParamBoolID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				MPI_Datatype createAggregateParamIntMpiType()
				{
					// Create MPI type for a AggParamInt record:
					int err;
					MPI_Datatype aggParamIntRecord_MPI, tmpType;

					int lengths[10] = {1, 1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1, 1, 1};
					MPI_Aint displacements[10];
					int i=0;
					displacements[i] = offsetof(TT_AggParamInt, rank); i++;
					displacements[i] = offsetof(TT_AggParamInt, processID); i++;
					displacements[i] = offsetof(TT_AggParamInt, window); i++;
					displacements[i] = offsetof(TT_AggParamInt, callPathID); i++;
					displacements[i] = offsetof(TT_AggParamInt, paramName); i++;
					displacements[i] = offsetof(TT_AggParamInt, minValue); i++;
					displacements[i] = offsetof(TT_AggParamInt, maxValue); i++;
					displacements[i] = offsetof(TT_AggParamInt, count); i++;
					displacements[i] = offsetof(TT_AggParamInt, avgValue); i++;
					displacements[i] = offsetof(TT_AggParamInt, stdev); i++;
					MPI_Datatype types[10] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_INT, MPI_INT, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE };
					err = MPI_Type_create_struct(10, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for aggParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for aggParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &aggParamIntRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for aggParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&aggParamIntRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for aggParamIntRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return aggParamIntRecord_MPI;
				}

				MPI_Datatype createAggregateParamFloatMpiType()
				{
					// Create MPI type for a AggParamFloat record:
					int err;
					MPI_Datatype aggParamFloatRecord_MPI, tmpType;

					int lengths[10] = {1, 1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1, 1, 1};
					MPI_Aint displacements[10];
					int i=0;
					displacements[i] = offsetof(TT_AggParamFloat, rank); i++;
					displacements[i] = offsetof(TT_AggParamFloat, processID); i++;
					displacements[i] = offsetof(TT_AggParamFloat, window); i++;
					displacements[i] = offsetof(TT_AggParamFloat, callPathID); i++;
					displacements[i] = offsetof(TT_AggParamFloat, paramName); i++;
					displacements[i] = offsetof(TT_AggParamFloat, minValue); i++;
					displacements[i] = offsetof(TT_AggParamFloat, maxValue); i++;
					displacements[i] = offsetof(TT_AggParamFloat, count); i++;
					displacements[i] = offsetof(TT_AggParamFloat, avgValue); i++;
					displacements[i] = offsetof(TT_AggParamFloat, stdev); i++;
					MPI_Datatype types[10] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_DOUBLE, MPI_DOUBLE, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE };
					err = MPI_Type_create_struct(10, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for aggParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for aggParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &aggParamFloatRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for aggParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&aggParamFloatRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for aggParamFloatRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return aggParamFloatRecord_MPI;
				}

				MPI_Datatype createAggregateParamBoolMpiType()
				{
					// Create MPI type for a AggParamBool record:
					int err;
					MPI_Datatype aggParamBoolRecord_MPI, tmpType;

					int lengths[10] = {1, 1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1, 1, 1};
					MPI_Aint displacements[10];
					int i=0;
					displacements[i] = offsetof(TT_AggParamBool, rank); i++;
					displacements[i] = offsetof(TT_AggParamBool, processID); i++;
					displacements[i] = offsetof(TT_AggParamBool, window); i++;
					displacements[i] = offsetof(TT_AggParamBool, callPathID); i++;
					displacements[i] = offsetof(TT_AggParamBool, paramName); i++;
					displacements[i] = offsetof(TT_AggParamBool, minValue); i++;
					displacements[i] = offsetof(TT_AggParamBool, maxValue); i++;
					displacements[i] = offsetof(TT_AggParamBool, count); i++;
					displacements[i] = offsetof(TT_AggParamBool, avgValue); i++;
					displacements[i] = offsetof(TT_AggParamBool, stdev); i++;
					MPI_Datatype types[10] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_INT, MPI_INT, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE };
					err = MPI_Type_create_struct(10, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for aggParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for aggParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &aggParamBoolRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for aggParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&aggParamBoolRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for aggParamBoolRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return aggParamBoolRecord_MPI;
				}
			}
		}
	}
}
