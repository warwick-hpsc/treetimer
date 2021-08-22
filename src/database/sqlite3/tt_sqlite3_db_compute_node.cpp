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

#include "tt_sqlite3_db_compute_node.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				// Why does table need to know MachineName?!

				void writeSchemaComputeNodeData(TTSQLite3& dataAccess)
				{
					char * zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS ComputeNodeData(ComputeNodeID INTEGER, "
																				  // "MachineID INTEGER, "
																				  "NodeName TEXT, "
																				  "SocketCount INTEGER, "
																				  "PRIMARY KEY(ComputeNodeID)"
																				  // ", FOREIGN KEY(MachineID) REFERENCES MachineData(MachineID)"
																				  ");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaComputeNodeData\n";
					}
				}

				void findComputeNodeDataID(TTSQLite3& dataAccess,
											// int machineID,
											TT_ComputeNode d,
											int * computeNodeID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					// int err = sqlite3_prepare(dataAccess.db,
					// 						  "SELECT ComputeNodeID FROM ComputeNodeData WHERE "
					// 					      "MachineID = ? AND "
					// 						  "NodeName LIKE ? AND "
					// 						  "SocketCount = ?",
					// 						  -1, &pStmt, NULL);
					// sqlite3_bind_int(pStmt,1, machineID);
					// sqlite3_bind_text(pStmt,2, nodeName.c_str(), -1, SQLITE_TRANSIENT);
					// sqlite3_bind_int(pStmt,3, socketCount);
					int err = sqlite3_prepare(dataAccess.db,
											  "SELECT ComputeNodeID FROM ComputeNodeData WHERE "
											  "NodeName LIKE ? AND "
											  "SocketCount = ?",
											  -1, &pStmt, NULL);
					sqlite3_bind_text(pStmt,1, d.nodeName, -1, SQLITE_TRANSIENT);
					sqlite3_bind_int(pStmt,2, d.socketCount);

					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findComputeNodeDataID\n";
						} else if (err = SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findComputeNodeDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findComputeNodeDataID - unknown error code " << err << std::endl;
						}
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						*computeNodeID = sqlite3_column_int(pStmt, 0);
					}
					else {
						*computeNodeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeComputeNodeData(TTSQLite3& dataAccess,
											// int machineID,
											TT_ComputeNode d,
											int * computeNodeID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// === Check for existing entry ===
					int tmpID = -1;
					// findComputeNodeDataID(dataAccess, machineID, nodeName, socketCount, &tmpID);
					findComputeNodeDataID(dataAccess, d, &tmpID);

					if(tmpID == -1)
					{
						// err = sqlite3_prepare(dataAccess.db,"INSERT INTO ComputeNodeData VALUES(NULL, ?, ?, ?)", -1, &pStmt, NULL);
						// sqlite3_bind_int(pStmt,1, machineID);
						// sqlite3_bind_text(pStmt,2, nodeName.c_str(), -1, SQLITE_TRANSIENT);
						// sqlite3_bind_int(pStmt,3, socketCount);
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO ComputeNodeData VALUES(NULL, ?, ?)", -1, &pStmt, NULL);
						sqlite3_bind_text(pStmt,1, d.nodeName, -1, SQLITE_TRANSIENT);
						sqlite3_bind_int(pStmt,2, d.socketCount);

						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeComputeNodeData\n";
							} else if (err = SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeComputeNodeData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeComputeNodeData - unknown error code " << err << std::endl;
							}
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*computeNodeID = -1;
						}
						else {
							*computeNodeID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						*computeNodeID = tmpID;
					}
				}
			}
		}
	}
}
