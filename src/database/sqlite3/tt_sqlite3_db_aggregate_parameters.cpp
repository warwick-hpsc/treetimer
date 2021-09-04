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
					if(err == SQLITE_ERROR) {
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
					if(err == SQLITE_ERROR) {
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
											  "CallPathID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int   (pStmt,1, dataAccess.runID);
					sqlite3_bind_int   (pStmt,2, p.callPathID);
					sqlite3_bind_int   (pStmt,3, p.processID);
					sqlite3_bind_text  (pStmt,4, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_int   (pStmt,5, p.minValue);
					sqlite3_bind_double(pStmt,6, p.avgValue);
					sqlite3_bind_int   (pStmt,7, p.maxValue);
					sqlite3_bind_double(pStmt,8, p.stdev);
					sqlite3_bind_int   (pStmt,9, p.count);

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
											  "CallPathID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int   (pStmt,1, dataAccess.runID);
					sqlite3_bind_int   (pStmt,2, p.callPathID);
					sqlite3_bind_int   (pStmt,3, p.processID);
					sqlite3_bind_text  (pStmt,4, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,5, p.minValue);
					sqlite3_bind_double(pStmt,6, p.avgValue);
					sqlite3_bind_double(pStmt,7, p.maxValue);
					sqlite3_bind_double(pStmt,8, p.stdev);
					sqlite3_bind_int   (pStmt,9, p.count);

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
											  "CallPathID = ? AND "
							  	  	  	  	  "ProcessID = ? AND "
			  	  	  	  	  	  	  	  	  "ParamName LIKE ? AND "
			  	  	  	  	  	  	  	  	  "MinValue = ? AND "
											  "AvgValue = ? AND "
											  "MaxValue = ? AND "
											  "StdDev = ? AND "
											  "Count = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int   (pStmt,1, dataAccess.runID);
					sqlite3_bind_int   (pStmt,2, p.callPathID);
					sqlite3_bind_int   (pStmt,3, p.processID);
					sqlite3_bind_text  (pStmt,4, p.paramName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,5, p.minValue);
					sqlite3_bind_double(pStmt,6, p.avgValue);
					sqlite3_bind_double(pStmt,7, p.maxValue);
					sqlite3_bind_double(pStmt,8, p.stdev);
					sqlite3_bind_int   (pStmt,9, p.count);

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

				void writeAggregateParameterIntData(TTSQLite3& dataAccess, TT_AggParamInt p, int *aggParamIntID)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggParamIntRecords.push_back(p);
						if(aggParamIntID!=nullptr) *aggParamIntID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findAggregateParameterIntDataID(dataAccess, p, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterInt VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int   (pStmt,1, dataAccess.runID);
						sqlite3_bind_int   (pStmt,2, p.callPathID);
						sqlite3_bind_int   (pStmt,3, p.processID);
						sqlite3_bind_text  (pStmt,4, p.paramName, -1, SQLITE_TRANSIENT);
						sqlite3_bind_int   (pStmt,5, p.minValue);
						sqlite3_bind_double(pStmt,6, p.avgValue);
						sqlite3_bind_int   (pStmt,7, p.maxValue);
						sqlite3_bind_double(pStmt,8, p.stdev);
						sqlite3_bind_int   (pStmt,9, p.count);
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
					else {
						if(aggParamIntID!=nullptr) *aggParamIntID = tmpID;
					}
				}


				void writeAggregateParameterFloatData(TTSQLite3& dataAccess, TT_AggParamFloat p, int *aggParamFloatID)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggParamFloatRecords.push_back(p);
						if(aggParamFloatID!=nullptr) *aggParamFloatID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findAggregateParameterFloatDataID(dataAccess, p, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterFloat VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int   (pStmt,1, dataAccess.runID);
						sqlite3_bind_int   (pStmt,2, p.callPathID);
						sqlite3_bind_int   (pStmt,3, p.processID);
						sqlite3_bind_text  (pStmt,4, p.paramName, -1, SQLITE_TRANSIENT);
						sqlite3_bind_double(pStmt,5, p.minValue);
						sqlite3_bind_double(pStmt,6, p.avgValue);
						sqlite3_bind_double(pStmt,7, p.maxValue);
						sqlite3_bind_double(pStmt,8, p.stdev);
						sqlite3_bind_int   (pStmt,9, p.count);
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
					else {
						if(aggParamFloatID!=nullptr) *aggParamFloatID = tmpID;
					}
				}


				void writeAggregateParameterBoolData(TTSQLite3& dataAccess, TT_AggParamBool p,  int *aggParamBoolID)
				{
					if (dataAccess.gatherIntraNode && dataAccess.rankLocal != 0) {
						dataAccess.aggParamBoolRecords.push_back(p);
						if(aggParamBoolID!=nullptr) *aggParamBoolID = -1;
						return;
					}

					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findAggregateParameterBoolDataID(dataAccess, p, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO AggregateParameterBool VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int   (pStmt,1, dataAccess.runID);
						sqlite3_bind_int   (pStmt,2, p.callPathID);
						sqlite3_bind_int   (pStmt,3, p.processID);
						sqlite3_bind_text  (pStmt,4, p.paramName, -1, SQLITE_TRANSIENT);
						sqlite3_bind_int   (pStmt,5, p.minValue);
						sqlite3_bind_double(pStmt,6, p.avgValue);
						sqlite3_bind_int   (pStmt,7, p.maxValue);
						sqlite3_bind_double(pStmt,8, p.stdev);
						sqlite3_bind_int   (pStmt,9, p.count);
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
					else {
						if(aggParamBoolID!=nullptr) *aggParamBoolID = tmpID;
					}
				}

				MPI_Datatype createAggregateParamIntMpiType()
				{
					// Create MPI type for a AggParamInt record:
					int err;
					MPI_Datatype aggParamIntRecord_MPI, tmpType;

					int lengths[9] = {1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					displacements[0] = offsetof(TT_AggParamInt, rank);
					displacements[1] = offsetof(TT_AggParamInt, callPathID);
					displacements[2] = offsetof(TT_AggParamInt, processID);
					displacements[3] = offsetof(TT_AggParamInt, paramName);
					displacements[4] = offsetof(TT_AggParamInt, minValue);
					displacements[5] = offsetof(TT_AggParamInt, maxValue);
					displacements[6] = offsetof(TT_AggParamInt, count);
					displacements[7] = offsetof(TT_AggParamInt, avgValue);
					displacements[8] = offsetof(TT_AggParamInt, stdev);
					MPI_Datatype types[9] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_INT, MPI_INT, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE };
					err = MPI_Type_create_struct(9, lengths, displacements, types, &tmpType);

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

					int lengths[9] = {1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					displacements[0] = offsetof(TT_AggParamFloat, rank);
					displacements[1] = offsetof(TT_AggParamFloat, callPathID);
					displacements[2] = offsetof(TT_AggParamFloat, processID);
					displacements[3] = offsetof(TT_AggParamFloat, paramName);
					displacements[4] = offsetof(TT_AggParamFloat, minValue);
					displacements[5] = offsetof(TT_AggParamFloat, maxValue);
					displacements[6] = offsetof(TT_AggParamFloat, count);
					displacements[7] = offsetof(TT_AggParamFloat, avgValue);
					displacements[8] = offsetof(TT_AggParamFloat, stdev);
					MPI_Datatype types[9] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_DOUBLE, MPI_DOUBLE, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE };
					err = MPI_Type_create_struct(9, lengths, displacements, types, &tmpType);

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

					int lengths[9] = {1, 1, 1, MAX_STRING_LENGTH, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					displacements[0] = offsetof(TT_AggParamBool, rank);
					displacements[1] = offsetof(TT_AggParamBool, callPathID);
					displacements[2] = offsetof(TT_AggParamBool, processID);
					displacements[3] = offsetof(TT_AggParamBool, paramName);
					displacements[4] = offsetof(TT_AggParamBool, minValue);
					displacements[5] = offsetof(TT_AggParamBool, maxValue);
					displacements[6] = offsetof(TT_AggParamBool, count);
					displacements[7] = offsetof(TT_AggParamBool, avgValue);
					displacements[8] = offsetof(TT_AggParamBool, stdev);
					MPI_Datatype types[9] = { MPI_INT, MPI_INT, MPI_INT, 
												MPI_CHAR, 
												MPI_INT, MPI_INT, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE };
					err = MPI_Type_create_struct(9, lengths, displacements, types, &tmpType);

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
