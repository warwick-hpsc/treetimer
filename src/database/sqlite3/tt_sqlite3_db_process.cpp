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

// TODO: Rename table and files from "process" to "thread"

#include "tt_sqlite3_db_process.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaProcessData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS ProcessData(ProcessID INTEGER, "
																				"CPUCoreID INTEGER, "
																				"MPIRank INTEGER, "
																				"Thread INTEGER, "
																				"FOREIGN KEY(CPUCoreID) REFERENCES CPUCoreData(CPUCoreID), "
																				"PRIMARY KEY(ProcessID)"
																				");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaProcessData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findProcessDataID(TTSQLite3& dataAccess, int cpuCoreID, int mpiRank, int thread, int *processID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
												"SELECT ProcessID FROM ProcessData WHERE "
												"CPUCoreID = ? "
												"AND MPIRank = ? "
												"AND Thread = ? "
												, -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, cpuCoreID);
					sqlite3_bind_int(pStmt,2, mpiRank);
					sqlite3_bind_int(pStmt,3, thread);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findProcessDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findProcessDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findProcessDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(processID!=nullptr) *processID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(processID!=nullptr) *processID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeProcessData(TTSQLite3& dataAccess, int cpuCoreID, int mpiRank, int thread, int *processID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID = -1;
					findProcessDataID(dataAccess, cpuCoreID, mpiRank, thread, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ProcessData VALUES(NULL, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, cpuCoreID);
						sqlite3_bind_int(pStmt,2, mpiRank);
						sqlite3_bind_int(pStmt,3, thread);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeProcessData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeProcessData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeProcessData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(processID!=nullptr) *processID = -1;
						}
						else {
							if(processID!=nullptr) *processID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(processID!=nullptr) *processID = tmpID;
					}
				}
			}
		}
	}
}
