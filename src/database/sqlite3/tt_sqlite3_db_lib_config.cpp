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

#include "tt_sqlite3_db_lib_config.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaLibraryConfigData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS InstrumLibConfig("
										"LibraryConfigID INTEGER, "
										"VersionMajor INTEGER, "
										"VersionMinor INTEGER, "
										"AggTimers INTEGER, "
										"TraceTimers INTEGER, "
										"AggParam INTEGER, "
										"TraceParam INTEGER, "
										"AggPAPI INTEGER, "
										"TracePAPI INTEGER, "
										"MpiHooks INTEGER, "
										"PRIMARY KEY(LibraryConfigID)"
										");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaLibraryConfigData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findLibraryConfigID(TTSQLite3& dataAccess,
										int verMajor, int verMinor,
										bool aggTimers, bool traceTimers,
										bool aggParam, bool traceParam,
										bool aggPAPI, bool tracePAPI,
										bool mpiHooks,
										int *libConfigID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					sqlite3_prepare(dataAccess.db,
									"SELECT LibraryConfigID FROM InstrumLibConfig WHERE "
									"VersionMajor = ? AND "
									"VersionMinor = ? AND "
									"AggTimers = ? AND "
									"TraceTimers = ? AND "
									"AggParam = ? AND "
									"TraceParam = ? AND "
									"AggPAPI = ? AND "
									"TracePAPI = ? AND "
									"MpiHooks = ?",
									-1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, verMajor);
					sqlite3_bind_int(pStmt,2, verMinor);
					sqlite3_bind_int(pStmt,3, (int) aggTimers);
					sqlite3_bind_int(pStmt,4, (int) traceTimers);
					sqlite3_bind_int(pStmt,5, (int) aggParam);
					sqlite3_bind_int(pStmt,6, (int) traceParam);
					sqlite3_bind_int(pStmt,7, (int) aggPAPI);
					sqlite3_bind_int(pStmt,8, (int) tracePAPI);
					sqlite3_bind_int(pStmt,9, (int) mpiHooks);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findLibraryConfigID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findLibraryConfigID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findLibraryConfigID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(libConfigID!=nullptr) *libConfigID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(libConfigID!=nullptr) *libConfigID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeLibraryConfigID(TTSQLite3& dataAccess,
										 int verMajor, int verMinor,
										 bool aggTimers, bool traceTimers,
										 bool aggParam, bool traceParam,
										 bool aggPAPI, bool tracePAPI,
										 bool mpiHooks,
										 int *libConfigID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findLibraryConfigID(dataAccess, verMajor, verMinor, aggTimers, traceTimers,
										aggParam, traceParam, aggPAPI, tracePAPI,
										mpiHooks, &tmpID);

					if(tmpID == -1)
					{
						sqlite3_prepare(dataAccess.db,
										"INSERT INTO InstrumLibConfig VALUES(NULL, ?, ?, ?, ?, ? ,? ,? ,? ,?)",
										-1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, verMajor);
						sqlite3_bind_int(pStmt,2, verMinor);
						sqlite3_bind_int(pStmt,3, (int) aggTimers);
						sqlite3_bind_int(pStmt,4, (int) traceTimers);
						sqlite3_bind_int(pStmt,5, (int) aggParam);
						sqlite3_bind_int(pStmt,6, (int) traceParam);
						sqlite3_bind_int(pStmt,7, (int) aggPAPI);
						sqlite3_bind_int(pStmt,8, (int) tracePAPI);
						sqlite3_bind_int(pStmt,9, (int) mpiHooks);

						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeLibraryConfigID\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeLibraryConfigID - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeLibraryConfigID - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(libConfigID!=nullptr) *libConfigID = -1;
						}
						else {
							if(libConfigID!=nullptr) *libConfigID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(libConfigID!=nullptr) *libConfigID = tmpID;
					}
				}
			}
		}
	}
}
