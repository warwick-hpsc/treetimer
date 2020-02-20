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
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS CPUData(CPUID INTEGER, "
																		  "CPUModel TEXT, "
																		  "PRIMARY KEY(CPUID),"
																		  "UNIQUE(CPUModel)"
																		  ");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findCPUDataID(TTSQLite3& dataAccess, std::string cpuModel, int * cpuID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;
					int err;

					sqlite3_prepare(dataAccess.db,
											  "SELECT CPUID FROM CPUData WHERE "
										      "CPUModel LIKE ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, cpuModel.c_str(), -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findCPUDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*cpuID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*cpuID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCPUData(TTSQLite3& dataAccess, std::string cpuModel, int * cpuID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findCPUDataID(dataAccess, cpuModel, &tmpID);

					if(tmpID == -1)
					{
						sqlite3_prepare(dataAccess.db,"INSERT INTO CPUData VALUES(NULL, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, cpuModel.c_str(), -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeCPUData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*cpuID = -1;
						}
						else
						{
							*cpuID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*cpuID = tmpID;
					}
				}
			}
		}
	}
}
