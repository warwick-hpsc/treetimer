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

#include "tt_sqlite3_db_profile_run_config.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				// Most of table refers to other now-redundant tables, as a script can collect their data.

				void writeSchemaProfileRunConfigData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					// std::string stmt = "CREATE TABLE IF NOT EXISTS ProfileRunConfigData(RunID INTEGER, "
					// 															  "AppConfigID INTEGER, "
					// 															  "LibraryConfigID INTEGER, "
					// 		  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessCount INTEGER, "
					// 															  "FOREIGN KEY(AppConfigID) REFERENCES ApplicationConfigData(AppConfigID), "
					// 															  "FOREIGN KEY(LibraryConfigID) REFERENCES InstrumLibConfig(LibraryConfigID), "
					// 															  "PRIMARY KEY(RunID)"
					// 															  ");";
					std::string stmt = "CREATE TABLE IF NOT EXISTS ProfileRunConfigData(RunID INTEGER, "
																				  "LibraryConfigID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  	  "ProcessCount INTEGER, "
																				  "PRIMARY KEY(RunID)"
																				  ");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaProfileRunConfigData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void writeProfileRunConfigData(TTSQLite3& dataAccess, 
												// int appConfigID, 
												int libConfigID, int processCount, int *runID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// We can have duplicate runs with the same configuration
					// Therefore we will always write a new run when this function is called.

					// err = sqlite3_prepare(dataAccess.db,"INSERT INTO ProfileRunConfigData VALUES(NULL, ?, ?, ?)", -1, &pStmt, NULL);
					// sqlite3_bind_int(pStmt,1, appConfigID);
					// sqlite3_bind_int(pStmt,2, libConfigID);
					// sqlite3_bind_int(pStmt,3, processCount);
					err = sqlite3_prepare(dataAccess.db,"INSERT INTO ProfileRunConfigData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);
					sqlite3_bind_int(pStmt,1, libConfigID);
					sqlite3_bind_int(pStmt,2, processCount);

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeProfileRunConfigData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeProfileRunConfigData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeProfileRunConfigData - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
						if(runID!=nullptr) *runID = -1;
					}
					else {
						if(runID!=nullptr) *runID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}
			}
		}
	}
}
