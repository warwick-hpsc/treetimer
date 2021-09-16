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

#include "tt_sqlite3_db_machine.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				// Need for table unclear, a script can collect this data.
/*
				void writeSchemaMachineData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS MachineData(MachineID INTEGER, "
																				"MachineName TEXT, "
																				"PRIMARY KEY(MachineID));";
					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaCPUSocketData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findMachineDataID(TTSQLite3& dataAccess, std::string machineName, int *machineID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					sqlite3_prepare(dataAccess.db,
											  "SELECT MachineID FROM MachineData WHERE "
										      "MachineName LIKE ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, machineName.c_str(), -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findMachineDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findMachineDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findMachineDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(machineID!=nullptr) *machineID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(machineID!=nullptr) *machineID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeMachineData(TTSQLite3& dataAccess, std::string machineName, int *machineID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findMachineDataID(dataAccess, machineName, &tmpID);

					if(tmpID == -1) {
						sqlite3_prepare(dataAccess.db,"INSERT INTO MachineData VALUES(NULL, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, machineName.c_str(), -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeMachineData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeMachineData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeMachineData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(machineID!=nullptr) *machineID = -1;
						}
						else {
							if(machineID!=nullptr) *machineID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(machineID!=nullptr) *machineID = tmpID;
					}
				}
*/
			}
		}
	}
}
