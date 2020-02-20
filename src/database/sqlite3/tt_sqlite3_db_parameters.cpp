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
					char * zErrMsg = 0;
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

					stmt = "CREATE TABLE IF NOT EXISTS ParameterIntData(ParamIntID INTEGER, "
																		"ParamName TEXT, "
																		"ParamValue INTEGER, "
																		"PRIMARY KEY(ParamIntID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					// SQLite3 doesn't have a bool type - use integer with a constraint if needed
					// Mostly exists so we can use the table to distinguish between types rather than
					// reuse the integer table
					stmt = "CREATE TABLE IF NOT EXISTS ParameterBoolData(ParamBoolID INTEGER, "
																		"ParamName TEXT, "
																		"ParamValue INTEGER, "
																		"PRIMARY KEY(ParamBoolID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

					stmt = "CREATE TABLE IF NOT EXISTS ParameterStringData(ParamStringID INTEGER, "
																		"ParamName TEXT, "
																		"ParamValue TEXT, "
																		"PRIMARY KEY(ParamStringID));";

					err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				// ============================================================================================================

				// ToDo: Is there a nice way of templating these (that won't require use of typeid?)
				// Lot of duplicated boilerplate code otherwise.

				void findParameterFloatDataID(TTSQLite3& dataAccess, std::string paramName, double value, int * paramFloatID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamFloatID FROM ParameterFloatData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_double(pStmt,2, value);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findParameterFloatDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*paramFloatID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*paramFloatID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				// ============================================================================================================

				void findParameterIntDataID(TTSQLite3& dataAccess, std::string paramName, int value, int * paramIntID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamIntID FROM ParameterIntData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,2, value);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findParameterIntDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*paramIntID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*paramIntID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				// ============================================================================================================

				void findParameterBoolDataID(TTSQLite3& dataAccess, std::string paramName, bool value, int * paramBoolID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamBoolID FROM ParameterBoolData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,2, (int) value);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findParameterBoolDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*paramBoolID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*paramBoolID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				// ============================================================================================================

				void findParameterStringDataID(TTSQLite3& dataAccess, std::string paramName, std::string value, int * paramStringID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ParamStringID FROM ParameterStringData WHERE "
										      "ParamName LIKE ? AND "
											  "ParamValue LIKE ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, paramName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_text(pStmt,2, value.c_str(), -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findParameterStringDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*paramStringID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*paramStringID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				// ============================================================================================================

				void writeParameterFloatData(TTSQLite3& dataAccess, std::string paramName, double value, int * paramFloatID)
				{
					sqlite3_stmt * pStmt;
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

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeParameterFloatData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*paramFloatID = -1;
						}
						else
						{
							*paramFloatID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);

					}
					else
					{
						*paramFloatID = tmpID;
					}
				}

				// ============================================================================================================

				void writeParameterIntData(TTSQLite3& dataAccess, std::string paramName, int value, int * paramIntID)
				{
					sqlite3_stmt * pStmt;
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

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeParameterIntData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*paramIntID = -1;
						}
						else
						{
							*paramIntID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*paramIntID = tmpID;
					}
				}

				// ============================================================================================================

				void writeParameterBoolData(TTSQLite3& dataAccess, std::string paramName, bool value, int * paramBoolID)
				{
					sqlite3_stmt * pStmt;
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

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeParameterBoolData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*paramBoolID = -1;
						}
						else
						{
							*paramBoolID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*paramBoolID = tmpID;
					}
				}

				// ============================================================================================================

				void writeParameterStringData(TTSQLite3& dataAccess, std::string paramName, std::string value, int * paramStringID)
				{
					sqlite3_stmt * pStmt;
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

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeParameterStringData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*paramStringID = -1;
						}
						else
						{
							*paramStringID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*paramStringID = tmpID;
					}
				}

				// ============================================================================================================
			}
		}
	}
}
