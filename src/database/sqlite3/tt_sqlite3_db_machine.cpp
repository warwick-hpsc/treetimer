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
				void writeSchemaMachineData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS MachineData(MachineID INTEGER, "
																				  "MachineName TEXT, "
																				  "PRIMARY KEY(MachineID));";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findMachineDataID(TTSQLite3& dataAccess, std::string machineName, int * machineID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;
					int err;

					sqlite3_prepare(dataAccess.db,
											  "SELECT MachineID FROM MachineData WHERE "
										      "MachineName LIKE ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, machineName.c_str(), -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findMachineDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*machineID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*machineID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeMachineData(TTSQLite3& dataAccess, std::string machineName, int * machineID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findMachineDataID(dataAccess, machineName, &tmpID);

					if(tmpID == -1)
					{
						sqlite3_prepare(dataAccess.db,"INSERT INTO MachineData VALUES(NULL, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, machineName.c_str(), -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeMachineData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*machineID = -1;
						}
						else
						{
							*machineID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*machineID = tmpID;
					}
				}
			}
		}
	}
}
