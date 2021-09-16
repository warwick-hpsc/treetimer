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

#include "tt_sqlite3_db_cpu_socket.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaCPUSocketData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS CPUSocketData(CPUSocketID INTEGER, "
																				"ComputeNodeID INTEGER, "
																				"CPUID INTEGER, "
																				"PhysicalSocketNum INTEGER, "
																				"UNIQUE(ComputeNodeID, PhysicalSocketNum), "
																				"FOREIGN KEY(ComputeNodeID) REFERENCES ComputeNodeData(ComputeNodeID), "
																				"FOREIGN KEY(CPUID) REFERENCES CPUData(CPUID), "
																				"PRIMARY KEY(CPUSocketID));";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaCPUSocketData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findCPUSocketDataID(TTSQLite3& dataAccess, int computeNodeID, int cpuID, TT_Socket d, int *cpuSocketID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
												"SELECT CPUSocketID FROM CPUSocketData WHERE "
												"ComputeNodeID = ? AND "
												"CPUID = ? AND "
												"PhysicalSocketNum = ?",
												-1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, computeNodeID);
					sqlite3_bind_int(pStmt,2, cpuID);
					sqlite3_bind_int(pStmt,3, d.phySocketNum);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findCPUSocketDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findCPUSocketDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findCPUSocketDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(cpuSocketID!=nullptr) *cpuSocketID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(cpuSocketID!=nullptr) *cpuSocketID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCPUSocketData(TTSQLite3& dataAccess, int computeNodeID, int cpuID, TT_Socket d, int *cpuSocketID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findCPUSocketDataID(dataAccess, computeNodeID, cpuID, d, &tmpID);

					if(tmpID == -1)
					{
						sqlite3_prepare(dataAccess.db,"INSERT INTO CPUSocketData VALUES(NULL, ?, ?, ?)", -1, &pStmt, NULL);
						sqlite3_bind_int(pStmt,1, computeNodeID);
						sqlite3_bind_int(pStmt,2, cpuID);
						sqlite3_bind_int(pStmt,3, d.phySocketNum);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeCPUSocketData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeCPUSocketData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeCPUSocketData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(cpuSocketID!=nullptr) *cpuSocketID = -1;
						}
						else {
							if(cpuSocketID!=nullptr) *cpuSocketID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						if(cpuSocketID!=nullptr) *cpuSocketID = tmpID;
					}
				}
			}
		}
	}
}
