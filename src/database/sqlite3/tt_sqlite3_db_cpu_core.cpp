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

#include "tt_sqlite3_db_cpu_core.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaCPUCoreData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS CPUCoreData(CPUCoreID INTEGER, "
																				"CPUSocketID INTEGER, "
																				"PhysicalCoreNum INTEGER, "
																				"FOREIGN KEY(CPUSocketID) REFERENCES CPUSocketData(CPUSocketID), "
																				"PRIMARY KEY(CPUCoreID)"
																				");";
					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaCPUCoreData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findCPUCoreDataID(TTSQLite3& dataAccess, int cpuSocketID, TT_CpuCore d, int *cpuCoreID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT CPUCoreID FROM CPUCoreData WHERE "
										      "CpuSocketID = ? AND "
											  "PhysicalCoreNum = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, cpuSocketID);
					sqlite3_bind_int(pStmt,2, d.physicalCoreNum);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findCPUCoreDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findCPUCoreDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findCPUCoreDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(cpuCoreID!=nullptr) *cpuCoreID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(cpuCoreID!=nullptr) *cpuCoreID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCPUCoreData(TTSQLite3& dataAccess, int cpuSocketID, TT_CpuCore d, int *cpuCoreID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findCPUCoreDataID(dataAccess, cpuSocketID, d, &tmpID);

					if(tmpID == -1)
					{
						sqlite3_prepare(dataAccess.db,"INSERT INTO CPUCoreData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);
						sqlite3_bind_int(pStmt,1, cpuSocketID);
						sqlite3_bind_int(pStmt,2, d.physicalCoreNum);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeCPUCoreData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeCPUCoreData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeCPUCoreData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(cpuCoreID!=nullptr) *cpuCoreID = -1;
						}
						else {
							if(cpuCoreID!=nullptr) *cpuCoreID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(cpuCoreID!=nullptr) *cpuCoreID = tmpID;
					}
				}
			}
		}
	}
}
