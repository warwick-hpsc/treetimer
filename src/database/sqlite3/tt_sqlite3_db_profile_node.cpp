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

#include "tt_sqlite3_db_profile_node.h"

#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaProfileNodeData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;

					std::string stmt = "CREATE TABLE IF NOT EXISTS ProfileNodeData(ProfileNodeID INTEGER, "
																				  "NodeName TEXT, "
																				  "ProfileNodeTypeID INTEGER, "
																				  "PRIMARY KEY(ProfileNodeID),"
																				  "FOREIGN KEY(ProfileNodeTypeID) REFERENCES ProfileNodeType(ProfileNodeTypeID)"
																				  ");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaProfileNodeData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findProfileNodeID(TTSQLite3& dataAccess, std::string profileNodeName, int profileNodeTypeID, int *profileNodeID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ProfileNodeID FROM ProfileNodeData WHERE NodeName LIKE ? AND ProfileNodeTypeID = ?",
											  -1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, profileNodeName.c_str(), -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,2, profileNodeTypeID);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findProfileNodeID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findProfileNodeID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findProfileNodeID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(profileNodeID!=nullptr) *profileNodeID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(profileNodeID!=nullptr) *profileNodeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeTreeNodeProfileNodeData(TTSQLite3& dataAccess,
												  std::string profileNodeName,
												  int profileNodeTypeID,
												  int *profileNodeID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// Check for existing entry
					int tmpID;
					findProfileNodeID(dataAccess, profileNodeName, profileNodeTypeID, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ProfileNodeData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_text(pStmt,1, profileNodeName.c_str(), -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,2, profileNodeTypeID);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeTreeNodeProfileNodeData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeTreeNodeProfileNodeData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeTreeNodeProfileNodeData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(profileNodeID!=nullptr) *profileNodeID = -1;
						}
						else {
							if(profileNodeID!=nullptr) *profileNodeID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(profileNodeID!=nullptr) *profileNodeID = tmpID;
					}
				}
			}
		}
	}
}
