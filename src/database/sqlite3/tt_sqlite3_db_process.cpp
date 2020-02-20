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
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS ProcessData(ProcessID INTEGER, "
																			  "CPUCoreID INTEGER, "
							  	  	  	  	  	  	  	  	  	  	  	  	  "MPIRank INTEGER, "
																			  "FOREIGN KEY(CPUCoreID) REFERENCES CPUCoreData(CPUCoreID), "
																			  "PRIMARY KEY(ProcessID)"
																			  ");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findProcessDataID(TTSQLite3& dataAccess, int cpuCoreID,  int mpiRank, int * processID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ProcessID FROM ProcessData WHERE "
										      "CPUCoreID = ? AND "
											  "MPIRank = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, cpuCoreID);
					sqlite3_bind_int(pStmt,2, mpiRank);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findProcessDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*processID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*processID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeProcessData(TTSQLite3& dataAccess, int cpuCoreID,  int mpiRank, int * processID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findProcessDataID(dataAccess, cpuCoreID, mpiRank, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ProcessData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, cpuCoreID);
						sqlite3_bind_int(pStmt,2, mpiRank);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeProcessData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*processID = -1;
						}
						else
						{
							*processID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*processID = tmpID;
					}
				}
			}
		}
	}
}
