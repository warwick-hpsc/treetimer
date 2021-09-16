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

#include "tt_sqlite3_db_profile_type.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaProfileNodeTypeData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS ProfileNodeType(ProfileNodeTypeID INTEGER, "
																				  "TypeName TEXT, "
																				  "PRIMARY KEY(ProfileNodeTypeID));";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaProfileNodeTypeData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findProfileNodeTypeID(TTSQLite3& dataAccess, std::string profileTypeName, int *profileNodeTypeID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					sqlite3_prepare(dataAccess.db,"SELECT ProfileNodeTypeID FROM ProfileNodeType WHERE TypeName LIKE ?", -1, &pStmt, NULL);
					sqlite3_bind_text(pStmt,1, profileTypeName.c_str(), -1, SQLITE_TRANSIENT);
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
						if(profileNodeTypeID!=nullptr) *profileNodeTypeID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(profileNodeTypeID!=nullptr) *profileNodeTypeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeProfileNodeTypeData(TTSQLite3& dataAccess, std::string profileTypeName, int *profileNodeTypeID)
				{
					sqlite3_stmt *pStmt = nullptr;

					// Check for existing entry
					int tmpID;
					findProfileNodeTypeID(dataAccess, profileTypeName, &tmpID);

					if(tmpID == -1)
					{
						int err = sqlite3_prepare(dataAccess.db,"INSERT INTO ProfileNodeType VALUES(NULL, ?)", -1, &pStmt, NULL);

						err = sqlite3_bind_text(pStmt,1, profileTypeName.c_str(), -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);
						
						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeProfileNodeTypeData\n";
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeProfileNodeTypeData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeProfileNodeTypeData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(profileNodeTypeID!=nullptr) *profileNodeTypeID = -1;
						}
						else {
							if(profileNodeTypeID!=nullptr) *profileNodeTypeID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(profileNodeTypeID!=nullptr) *profileNodeTypeID = tmpID;
					}
				}
			}
		}
	}
}
