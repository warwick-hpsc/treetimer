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

#include "tt_sqlite3_db_cpu.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaCPUData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS CPUData(CPUID INTEGER, "
																			"CPUModel TEXT, "
																			"PRIMARY KEY(CPUID)"
																			",UNIQUE(CPUModel)"
																			");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaCPUData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findCPUDataID(TTSQLite3& dataAccess, TT_Cpu d, int *cpuID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					sqlite3_prepare(dataAccess.db,
									"SELECT CPUID FROM CPUData WHERE "
									"CPUModel LIKE ?",
									-1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, d.cpuModel, -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findCPUDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findCPUDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findCPUDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(cpuID!=nullptr) *cpuID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(cpuID!=nullptr) *cpuID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCPUData(TTSQLite3& dataAccess, TT_Cpu d, int *cpuID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID = -1;
					findCPUDataID(dataAccess, d, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO CPUData VALUES(NULL, ?)", -1, &pStmt, NULL);
						if (err != SQLITE_OK) {
							std::cout << "SQL Error encountered in writeCPUData - stmt prepare failed\n";
							if(cpuID!=nullptr) *cpuID = -1;
							return;
						}

						sqlite3_bind_text(pStmt,1, d.cpuModel, -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeCPUData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeCPUData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeCPUData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(cpuID!=nullptr) *cpuID = -1;
						}
						else {
							if(cpuID!=nullptr) *cpuID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						if(cpuID!=nullptr) *cpuID = tmpID;
					}
				}
			}
		}
	}
}
