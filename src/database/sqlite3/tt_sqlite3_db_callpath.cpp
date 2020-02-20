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

#include "tt_sqlite3_db_callpath.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaCallPathData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS CallPathData(CallPathID INTEGER, "
																		       "ProfileNodeID INTEGER, "
																		       "ParentNodeID INTEGER, "
																		       "FOREIGN KEY(ProfileNodeID) REFERENCES ProfileNodeData(ProfileNodeID), "
							   	   	   	   	   	   	   	   	   	   	      	   "FOREIGN KEY(ParentNodeID) REFERENCES CallPathData(CallPathID), "
																			   "PRIMARY KEY(CallPathID));";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findCallPathDataID(TTSQLite3& dataAccess, int profileNodeID, int parentNodeID, int * callPathID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT CallPathID FROM CallPathData WHERE "
										      "ProfileNodeID = ? AND "
											  "ParentNodeID = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, profileNodeID);
					sqlite3_bind_int(pStmt,2, parentNodeID);
					err = sqlite3_step(pStmt);

					if(err == SQLITE_ERROR)
					{
						std::cout << "SQL Error encountered in findCallPathDataID\n";
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW)
					{
						*callPathID = sqlite3_column_int(pStmt, 0);
					}
					else
					{
						*callPathID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCallPathData(TTSQLite3& dataAccess, int profileNodeID, int parentNodeID, int * callPathID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findCallPathDataID(dataAccess, profileNodeID, parentNodeID, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO CallPathData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, profileNodeID);
						sqlite3_bind_int(pStmt,2, parentNodeID);
						err = sqlite3_step(pStmt);

						if(err == SQLITE_ERROR)
						{
							std::cout << "SQL Error encountered in writeCallPathData\n";
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*callPathID = -1;
						}
						else
						{
							*callPathID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*callPathID = tmpID;
					}
				}
			}
		}
	}
}
