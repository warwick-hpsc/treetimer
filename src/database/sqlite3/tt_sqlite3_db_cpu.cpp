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
																			"PRIMARY KEY(CPUID)"
																			",UNIQUE(CPUModel)"
																			");";

					int err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				}

				void findCPUDataID(TTSQLite3& dataAccess, TT_Cpu d, int* cpuID)
				{
					sqlite3_stmt * pStmt;
					char * zErrMsg = 0;
					int err;

					sqlite3_prepare(dataAccess.db,
									"SELECT CPUID FROM CPUData WHERE "
									"CPUModel LIKE ?",
									-1, &pStmt, NULL);

					sqlite3_bind_text(pStmt,1, d.cpuModel, -1, SQLITE_TRANSIENT);
					err = sqlite3_step(pStmt);

					if (err != SQLITE_OK && err != SQLITE_DONE && err != SQLITE_ROW) {
						if (err == SQLITE_ERROR) {
							std::cout << "SQL Error encountered in findCPUDataID\n";
						} else if (err = SQLITE_MISUSE) {
							std::cout << "SQL Error encountered in findCPUDataID - misuse\n";
						} else {
							std::cout << "SQL Error encountered in findCPUDataID - unknown error code " << err << std::endl;
						}
						char * expandedQuery = sqlite3_expanded_sql(pStmt);
						std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

						// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
						sqlite3_free(expandedQuery);
					}
					else if(err == SQLITE_ROW) {
						*cpuID = sqlite3_column_int(pStmt, 0);
					}
					else {
						*cpuID = -1;
					}

					sqlite3_finalize(pStmt);
				}

				void writeCPUData(TTSQLite3& dataAccess, TT_Cpu d, int* cpuID)
				{
					sqlite3_stmt * pStmt;
					int err;

					// Check for existing entry
					int tmpID = -1;
					findCPUDataID(dataAccess, d, &tmpID);

					if(tmpID == -1)
					{
						err = sqlite3_prepare(dataAccess.db,"INSERT INTO CPUData VALUES(NULL, ?)", -1, &pStmt, NULL);
						if (err != SQLITE_OK) {
							std::cout << "SQL Error encountered in writeCPUData - stmt prepare failed\n";
							*cpuID = -1;
							return;
						}

						sqlite3_bind_text(pStmt,1, d.cpuModel, -1, SQLITE_TRANSIENT);
						err = sqlite3_step(pStmt);

						if (err != SQLITE_OK && err != SQLITE_DONE) {
							if (err == SQLITE_ERROR) {
								std::cout << "SQL Error encountered in writeCPUData\n";
							} else if (err = SQLITE_MISUSE) {
								std::cout << "SQL Error encountered in writeCPUData - misuse\n";
							} else {
								std::cout << "SQL Error encountered in writeCPUData - unknown error code " << err << std::endl;
							}
							char * expandedQuery = sqlite3_expanded_sql(pStmt);
							std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

							// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
							sqlite3_free(expandedQuery);
							*cpuID = -1;
						}
						else {
							*cpuID = sqlite3_last_insert_rowid(dataAccess.db);
						}

						sqlite3_finalize(pStmt);
					}
					else
					{
						*cpuID = tmpID;
					}
				}

				MPI_Datatype createCpuMpiType()
				{
					// Create MPI type for a AggregateTime record:
					int err;
					MPI_Datatype cpuRecord_MPI, tmpType;

					// int lengths[2] = {1, MAX_STRING_LENGTH};
					// MPI_Aint displacements[2];
					// displacements[0] = offsetof(TT_Cpu, rank);
					// displacements[1] = offsetof(TT_Cpu, cpuModel);
					// MPI_Datatype types[2] = { MPI_INT, MPI_CHAR };
					// err = MPI_Type_create_struct(2, lengths, displacements, types, &tmpType);
					int lengths[1] = {MAX_STRING_LENGTH};
					MPI_Aint displacements[1];
					displacements[0] = offsetof(TT_Cpu, cpuModel);
					MPI_Datatype types[1] = { MPI_CHAR };
					err = MPI_Type_create_struct(1, lengths, displacements, types, &tmpType);

					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to create custom type for cpuRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Aint lb, extent;
					err = MPI_Type_get_extent(tmpType, &lb, &extent);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to get extent of custom type for cpuRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_create_resized(tmpType, lb, extent, &cpuRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to resize custom type for cpuRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&cpuRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Failed to commit custom type for cpuRecord\n");
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					return cpuRecord_MPI;
				}
			}
		}
	}
}
