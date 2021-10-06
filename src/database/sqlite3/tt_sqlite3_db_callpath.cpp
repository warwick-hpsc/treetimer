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
					char *zErrMsg = 0;
					std::string stmt = "CREATE TABLE IF NOT EXISTS CallPathData(CallPathID INTEGER, "
																				"ProcessID INTEGER, "
																				"ProfileNodeID INTEGER, "
																				"ParentNodeID INTEGER, "
																				"FOREIGN KEY(ProfileNodeID) REFERENCES ProfileNodeData(ProfileNodeID), "
																				"FOREIGN KEY(ParentNodeID) REFERENCES CallPathData(CallPathID), "
																				"PRIMARY KEY(CallPathID));";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
					if(err == SQLITE_ERROR) {
						std::cout << "SQL Error encountered in writeSchemaCallPathData\n";
						std::cout << "Failed query: " << std::string(stmt) << "\n";
					}
				}

				void findCallPathDataID(TTSQLite3& dataAccess, TT_CallPathNode c, int *callPathID)
				{
					sqlite3_stmt *pStmt = nullptr;

					int err = sqlite3_prepare(dataAccess.db,
												"SELECT CallPathID FROM CallPathData WHERE "
												"ProcessID = ? AND "
												"ProfileNodeID = ? AND "
												"ParentNodeID = ?",
												-1, &pStmt, NULL);
					int i=1;
					sqlite3_bind_int(pStmt,i, c.processID); i++;
					sqlite3_bind_int(pStmt,i, c.profileNodeID); i++;
					sqlite3_bind_int(pStmt,i, c.parentID); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findCallPathDataID\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findCallPathDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findCallPathDataID - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						if(callPathID!=nullptr) *callPathID = sqlite3_column_int(pStmt, 0);
					}
					else {
						if(callPathID!=nullptr) *callPathID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCallPathData(TTSQLite3& dataAccess, TT_CallPathNode c, int *callPathID, bool verifyUnique)
				{
					sqlite3_stmt *pStmt = nullptr;
					int err;

					if (verifyUnique) {
						// Check for existing entry
						int tmpID = -1;
						findCallPathDataID(dataAccess, c, &tmpID);
						if (tmpID != -1) {
							if(callPathID!=nullptr) *callPathID = tmpID;
							return;
						}
					}

					err = sqlite3_prepare(dataAccess.db,"INSERT INTO CallPathData VALUES(NULL, ?, ?, ?)", -1, &pStmt, NULL);

					int i=1;
					sqlite3_bind_int(pStmt,i, c.processID); i++;
					sqlite3_bind_int(pStmt,i, c.profileNodeID); i++;
					sqlite3_bind_int(pStmt,i, c.parentID); i++;
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in writeCallPathData\n";
						} else if (err == SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in writeCallPathData - misuse\n";
						} else {
							std::cout << "SQL Error encountered in writeCallPathData - unknown error code " << err << std::endl;
						}
						char *expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
						if(callPathID!=nullptr) *callPathID = -1;
					}
					else {
						if(callPathID!=nullptr) *callPathID = sqlite3_last_insert_rowid(dataAccess.db);
					}

					sqlite3_finalize(pStmt);
				}

				MPI_Datatype createCallpathNodeMpiType()
				{
					// Create MPI type for a CallpathNode record:
					int err;
					MPI_Datatype callpathNodeRecord_MPI, tmpType;

					const int n = 7;
					MPI_Aint displacements[n]; MPI_Datatype types[n]; int lengths[n];
					int i=0;
					types[i] = MPI_INT  ; displacements[i] = offsetof(TT_CallPathNode, rank);          lengths[i] = 1 ; i++;
					types[i] = MPI_INT  ; displacements[i] = offsetof(TT_CallPathNode, processID);     lengths[i] = 1 ; i++;
					types[i] = MPI_CHAR ; displacements[i] = offsetof(TT_CallPathNode, nodeName);      lengths[i] = MAX_STRING_LENGTH ; i++;
					types[i] = MPI_INT  ; displacements[i] = offsetof(TT_CallPathNode, blockType);     lengths[i] = 1 ; i++;
					types[i] = MPI_INT  ; displacements[i] = offsetof(TT_CallPathNode, callPathID);    lengths[i] = 1 ; i++;
					types[i] = MPI_INT  ; displacements[i] = offsetof(TT_CallPathNode, parentID);      lengths[i] = 1 ; i++;
					types[i] = MPI_INT  ; displacements[i] = offsetof(TT_CallPathNode, profileNodeID); lengths[i] = 1 ; i++;
					err = MPI_Type_create_struct(n, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &callpathNodeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&callpathNodeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for callpathNodeRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
					}

					return callpathNodeRecord_MPI;
				}
			}
		}
	}
}
