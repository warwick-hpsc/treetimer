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

#include "tt_sqlite3_db_app.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				// Useless table!

				// void writeSchemaApplicationData(TTSQLite3& dataAccess)
				// {
				// 	char *zErrMsg = 0;
				// 	std::string stmt = "CREATE TABLE IF NOT EXISTS ApplicationData(ApplicationID INTEGER, "
				// 																  "ApplicationName TEXT, "
				// 																  "ApplicationVersion TEXT, "
				// 																  "PRIMARY KEY(ApplicationID));";

				// 	int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				// }

				// void findApplicationDataID(TTSQLite3& dataAccess, std::string appName, std::string appVer, int *applicationID)
				// {
				// 	sqlite3_stmt *pStmt = nullptr;
				// 	char *zErrMsg = 0;

				// 	int err = sqlite3_prepare(dataAccess.db,
				// 							  "SELECT ApplicationID FROM ApplicationData WHERE "
				// 						      "ApplicationName LIKE ? AND "
				// 							  "ApplicationVersion LIKE ?",
				// 							  -1, &pStmt, NULL);

				// 	sqlite3_bind_text(pStmt,1, appName.c_str(), -1, SQLITE_TRANSIENT);
				// 	sqlite3_bind_text(pStmt,2, appVer.c_str(), -1, SQLITE_TRANSIENT);
				// 	err = sqlite3_step(pStmt);

				// 	if(err == SQLITE_ERROR)
				// 	{
				// 		std::cout << "SQL Error encountered in findApplicationDataID\n";
				// 		char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 		std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 		// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 		sqlite3_free(expandedQuery);
				// 	}
				// 	else if(err == SQLITE_ROW)
				// 	{
				// 		*applicationID = sqlite3_column_int(pStmt, 0);
				// 	}
				// 	else
				// 	{
				// 		*applicationID = -1;
				// 	}

				// 	sqlite3_finalize(pStmt);
				// }

				// void writeApplicationData(TTSQLite3& dataAccess, std::string appName, std::string appVer, int *applicationID)
				// {
				// 	sqlite3_stmt *pStmt = nullptr;
				// 	int err;

				// 	// Check for existing entry
				// 	int tmpID;
				// 	findApplicationDataID(dataAccess, appName, appVer, &tmpID);

				// 	if(tmpID == -1)
				// 	{
				// 		sqlite3_prepare(dataAccess.db,"INSERT INTO ApplicationData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);

				// 		sqlite3_bind_text(pStmt,1, appName.c_str(), -1, SQLITE_TRANSIENT);
				// 		sqlite3_bind_text(pStmt,2, appVer.c_str(), -1, SQLITE_TRANSIENT);
				// 		err = sqlite3_step(pStmt);

				// 		if(err == SQLITE_ERROR)
				// 		{
				// 			std::cout << "SQL Error encountered in writeApplicationData\n";
				// 			char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 			std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 			// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 			sqlite3_free(expandedQuery);
				// 			*applicationID = -1;
				// 		}
				// 		else
				// 		{
				// 			*applicationID = sqlite3_last_insert_rowid(dataAccess.db);
				// 		}

				// 		sqlite3_finalize(pStmt);
				// 	}
				// 	else
				// 	{
				// 		*applicationID = tmpID;
				// 	}
				// }
			}
		}
	}
}
