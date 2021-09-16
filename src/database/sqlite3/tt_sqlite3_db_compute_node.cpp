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
				// Why did table need to know MachineName?

				void writeSchemaComputeNodeData(TTSQLite3& dataAccess)
				{
					char *zErrMsg = 0;
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

				void findComputeNodeDataID(TTSQLite3& dataAccess, TT_ComputeNode d, int *computeNodeID)
				{
					sqlite3_stmt *pStmt = nullptr;

					// int err = sqlite3_prepare(dataAccess.db,
					// 							"SELECT ComputeNodeID FROM ComputeNodeData WHERE "
					// 					    	"MachineID = ? AND "
					// 							"NodeName LIKE ? AND "
					// 							"SocketCount = ?",
					// 							-1, &pStmt, NULL);
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
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findComputeNodeDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findComputeNodeDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(computeNodeID!=nullptr) *computeNodeID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(computeNodeID!=nullptr) *computeNodeID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeComputeNodeData(TTSQLite3& dataAccess,
											// int machineID,
											TT_ComputeNode d, int *computeNodeID)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					// === Check for existing entry ===
					int tmpID = -1;
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
							} else if (err == SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeComputeNodeData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeComputeNodeData - unknown error code " << err << std::endl;
							}
							char *expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							if(computeNodeID!=nullptr) *computeNodeID = -1;
						}
						else {
							if(computeNodeID!=nullptr) *computeNodeID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else {
						if(computeNodeID!=nullptr) *computeNodeID = tmpID;
					}
				}

				MPI_Datatype createHwInfoMpiType()
				{
					// Create MPI type for a AggregateTime record:
					int err;
					MPI_Datatype hwInfoRecord_MPI, tmpType;

					int lengths[7] = {1, 1, MAX_STRING_LENGTH, MAX_STRING_LENGTH, 1, 1, 1};
					MPI_Aint displacements[7];
					displacements[0] = offsetof(TT_HwInfo, rankGlobal);
					displacements[1] = offsetof(TT_HwInfo, thread);
					displacements[2] = offsetof(TT_HwInfo, nodeName);
					displacements[3] = offsetof(TT_HwInfo, cpuModel);
					displacements[4] = offsetof(TT_HwInfo, socketCount);
					displacements[5] = offsetof(TT_HwInfo, phySocketNum);
					displacements[6] = offsetof(TT_HwInfo, physicalCoreNum);
					MPI_Datatype types[7] = { MPI_INT, MPI_INT, MPI_CHAR, MPI_CHAR, MPI_INT, MPI_INT, MPI_INT };
					err = MPI_Type_create_struct(7, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for hwInfoRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for hwInfoRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &hwInfoRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for hwInfoRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&hwInfoRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for hwInfoRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return hwInfoRecord_MPI;
				}
			}
		}
	}
}
