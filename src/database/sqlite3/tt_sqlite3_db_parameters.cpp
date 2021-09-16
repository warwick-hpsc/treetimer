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

#include "tt_sqlite3_db_parameters.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaParameterData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					int err;
					std::string stmt;

					// Create separate tables for each parameter type
					// ToDo: It would likely be far easier to just store these all as TEXT, but would
					// this affect later functionality?
					stmt = "CREATE TABLE IF NOT EXISTS ParameterFloatData(ParamFloatID INTEGER, "
																				  "ParamName TEXT, "
																				  "ParamValue REAL, "
																				  "PRIMARY KEY(ParamFloatID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					stmt = "CREATE TABLE IF NOT EXISTS ParameterIntData(ParamIntID INTEGER, "
																		"ParamName TEXT, "
																		"ParamValue INTEGER, "
																		"PRIMARY KEY(ParamIntID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					// SQLite3 doesn't have a bool type - use integer with a constraint if needed
					// Mostly exists so we can use the table to distinguish between types rather than
					// reuse the integer table
					stmt = "CREATE TABLE IF NOT EXISTS ParameterBoolData(ParamBoolID INTEGER, "
																		"ParamName TEXT, "
																		"ParamValue INTEGER, "
																		"PRIMARY KEY(ParamBoolID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}

					stmt = "CREATE TABLE IF NOT EXISTS ParameterStringData(ParamStringID INTEGER, "
																		"ParamName TEXT, "
																		"ParamValue TEXT, "
																		"PRIMARY KEY(ParamStringID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaParameterData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				// ToDo: Is there a nice way of templating these (that won't require use of typeid?)
				// Lot of duplicated boilerplate code otherwise.

				void findParameterFloatDataID(TTSQLite3& dataAccess, std::string paramName, double value, int *paramFloatID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamFloatID FROM ParameterFloatData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,2, value);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findParameterFloatDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findParameterFloatDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findParameterFloatDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(paramFloatID!=nullptr) *paramFloatID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(paramFloatID!=nullptr) *paramFloatID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void findParameterIntDataID(TTSQLite3& dataAccess, std::string paramName, int value, int *paramIntID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamIntID FROM ParameterIntData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,2, value);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findParameterIntDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findParameterIntDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findParameterIntDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(paramIntID!=nullptr) *paramIntID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(paramIntID!=nullptr) *paramIntID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void findParameterBoolDataID(TTSQLite3& dataAccess, std::string paramName, bool value, int *paramBoolID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamBoolID FROM ParameterBoolData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,2, (int) value);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findParameterBoolDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findParameterBoolDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findParameterBoolDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(paramBoolID!=nullptr) *paramBoolID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(paramBoolID!=nullptr) *paramBoolID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void findParameterStringDataID(TTSQLite3& dataAccess, std::string paramName, std::string value, int *paramStringID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamStringID FROM ParameterStringData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue LIKE ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,2, value.c_str(), -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findParameterStringDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findParameterStringDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findParameterStringDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(paramStringID!=nullptr) *paramStringID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(paramStringID!=nullptr) *paramStringID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeParameterFloatData(TTSQLite3& dataAccess, std::string paramName, double value, int *paramFloatID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findParameterFloatDataID(dataAccess, paramName, value, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ParameterFloatData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_double(pStmt,2, value);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeParameterFloatData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeParameterFloatData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeParameterFloatData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(paramFloatID!=nullptr) *paramFloatID = -1;
						}
						else {
							if(paramFloatID!=nullptr) *paramFloatID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);

					}
					else {
						if(paramFloatID!=nullptr) *paramFloatID = tmpID;
					}
				}

				void writeParameterIntData(TTSQLite3& dataAccess, std::string paramName, int value, int *paramIntID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findParameterIntDataID(dataAccess, paramName, value, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ParameterIntData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,2, value);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeParameterIntData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeParameterIntData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeParameterIntData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(paramIntID!=nullptr) *paramIntID = -1;
						}
						else {
							if(paramIntID!=nullptr) *paramIntID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(paramIntID!=nullptr) *paramIntID = tmpID;
					}
				}

				void writeParameterBoolData(TTSQLite3& dataAccess, std::string paramName, bool value, int *paramBoolID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findParameterBoolDataID(dataAccess, paramName, value, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ParameterBoolData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,2, (int) value);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeParameterBoolData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeParameterBoolData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeParameterBoolData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(paramBoolID!=nullptr) *paramBoolID = -1;
						}
						else {
							if(paramBoolID!=nullptr) *paramBoolID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(paramBoolID!=nullptr) *paramBoolID = tmpID;
					}
				}

				void writeParameterStringData(TTSQLite3& dataAccess, std::string paramName, std::string value, int *paramStringID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findParameterStringDataID(dataAccess, paramName, value, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ParameterStringData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_text(pStmt,2, value.c_str(), -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeParameterStringData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeParameterStringData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeParameterStringData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(paramStringID!=nullptr) *paramStringID = -1;
						}
						else {
							if(paramStringID!=nullptr) *paramStringID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(paramStringID!=nullptr) *paramStringID = tmpID;
					}
				}
			}
		}
	}
}
