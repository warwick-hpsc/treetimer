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
																				"Rank INTEGER, "
																				"ProfileNodeID INTEGER, "
																				"ParentNodeID INTEGER, "
																				"FOREIGN KEY(ProfileNodeID) REFERENCES ProfileNodeData(ProfileNodeID), "
																				"FOREIGN KEY(ParentNodeID) REFERENCES CallPathData(CallPathID), "
																				"PRIMARY KEY(CallPathID));";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findCallPathDataID(TTSQLite3& dataAccess, int rank, int profileNodeID, int parentNodeID, int * callPathID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;

					int err = sqlite3_prepare(dataAccess.db,
												"SELECT CallPathID FROM CallPathData WHERE "
												"Rank = ? AND "
												"ProfileNodeID = ? AND "
												"ParentNodeID = ?",
												-1, &pStmt, NULL);

					sqlite3_bind_int(pStmt,1, rank);
					sqlite3_bind_int(pStmt,2, profileNodeID);
					sqlite3_bind_int(pStmt,3, parentNodeID);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findCallPathDataID\n";
						} else if (err = SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findCallPathDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findCallPathDataID - unknown error code " << err << std::endl;
						}
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

				void writeCallPathData(TTSQLite3& dataAccess, int rank, int profileNodeID, int parentNodeID, int * callPathID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID;
					findCallPathDataID(dataAccess, rank, profileNodeID, parentNodeID, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO CallPathData VALUES(NULL, ?, ?, ?)", -1, &pStmt, NULL);

						sqlite3_bind_int(pStmt,1, rank);
						sqlite3_bind_int(pStmt,2, profileNodeID);
						sqlite3_bind_int(pStmt,3, parentNodeID);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeCallPathData\n";
							} else if (err = SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeCallPathData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeCallPathData - unknown error code " << err << std::endl;
							}
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*callPathID = -1;
						}
						else {
							*callPathID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*callPathID = tmpID;
					}
				}

				MPI_Datatype createCallpathNodeMpiType()
				{
					// Create MPI type for a CallpathNode record:
					int err;
					MPI_Datatype callpathNodeRecord_MPI, tmpType;

					int lengths[5] = {1, MAX_STRING_LENGTH, 1, 1, 1};
					MPI_Aint displacements[5];
					displacements[0] = offsetof(TT_CallPathNode, rank);
					displacements[1] = offsetof(TT_CallPathNode, nodeName);
					displacements[2] = offsetof(TT_CallPathNode, blockType);
					displacements[3] = offsetof(TT_CallPathNode, callPathID);
					displacements[4] = offsetof(TT_CallPathNode, parentID);
					MPI_Datatype types[5] = { MPI_INT, MPI_CHAR, MPI_INT, MPI_INT, MPI_INT };
					err = MPI_Type_create_struct(5, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &callpathNodeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&callpathNodeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return callpathNodeRecord_MPI;
				}
			}
		}
	}
}
