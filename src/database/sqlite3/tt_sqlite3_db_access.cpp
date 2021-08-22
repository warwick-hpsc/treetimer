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

#include <sys/stat.h>

#include "tt_sqlite3_db_access.h"

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			TTSQLite3::TTSQLite3(std::string filePath)
			{
				this->filePath = filePath;

				// To avoid high-rank runs smashing filesystem, perform intra-node gather and write.
				// Need to decide if will perform gather before traversing Tree.
				int rankGlobal, nRanksGlobal, err;
				MPI_Comm_rank(MPI_COMM_WORLD, &rankGlobal);
				MPI_Comm_size(MPI_COMM_WORLD, &nRanksGlobal);
				// Todo: make 'gatherIntraNode' conditional on number of ranks
				bool gatherIntraNode = true;
				MPI_Comm nodeComm;
				int rankLocal, nRanksLocal;
				MPI_Info info;
				err = MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rankGlobal, info, &nodeComm);
				if (err != MPI_SUCCESS) {
					fprintf(stderr, "Rank %d failed to create intra-node MPI communicator\n", rankGlobal);
					MPI_Abort(MPI_COMM_WORLD, err);
					exit(EXIT_FAILURE);
				}
				MPI_Comm_rank(nodeComm, &rankLocal);
				MPI_Comm_size(nodeComm, &nRanksLocal);
				if (nRanksLocal == 1) {
					gatherIntraNode = false;
				}
				this->rankGlobal = rankGlobal;
				this->rankLocal = rankLocal;
				this->nRanksLocal = nRanksLocal;
				this->gatherIntraNode = gatherIntraNode;
				this->nodeComm = nodeComm;

				// This fixes push_back() seg-fault:
				this->aggTimeRecords.resize(0);
				this->traceTimeRecords.resize(0);
				this->callPathNodeRecords.resize(0);

				this->rankLocalToProcessID.resize(nRanksLocal);
				this->rankLocalToRankGlobal.resize(nRanksLocal);
				for (int i=0; i<nRanksLocal; i++) {
					this->rankLocalToProcessID[i]  = -1;
					this->rankLocalToRankGlobal[i] = -1;
				}
			}

			TTSQLite3::~TTSQLite3()
			{

			}

			namespace drivers
			{
				void openConnection(TTSQLite3& dataAccess)
				{
					// Create in-memory DB. If DB file exists, copy that into memory

					int err;
					sqlite3* db_file = NULL;
					sqlite3_backup* db_backup = NULL;

					err = sqlite3_open(":memory:", &(dataAccess.db));
					if (err != SQLITE_OK) {
						printf("TreeTimer error: sqlite3_open(\":memory:\") failed\n"); fflush(stdout);
						exit(EXIT_FAILURE);
					}

					bool db_file_exists = false;
					struct stat buffer;
					db_file_exists = (stat (dataAccess.filePath.c_str(), &buffer) == 0); 
					if (db_file_exists) {
						// Load and backup into in-memory DB:
						err = sqlite3_open(dataAccess.filePath.c_str(), &db_file);
						if (err != SQLITE_OK) {
							printf("TreeTimer error: sqlite3_open(%s) failed", dataAccess.filePath.c_str()); fflush(stdout);
							exit(EXIT_FAILURE);
						}

						db_backup = sqlite3_backup_init(dataAccess.db, "main", db_file, "main");
						if (db_backup) {
							sqlite3_backup_step(db_backup, -1);
							sqlite3_backup_finish(db_backup);
						}

						err = sqlite3_close(db_file);
						if (err != SQLITE_OK) {
							printf("TreeTimer error: sqlite3_close() of file DB failed\n"); fflush(stdout);
							exit(EXIT_FAILURE);
						}
					}

					sqlite3_exec(dataAccess.db,"BEGIN TRANSACTION",NULL,NULL,NULL);
				}

				void closeConnection(TTSQLite3& dataAccess)
				{
					// Write in-memory DB to file

					int err;
					sqlite3* db_file = NULL;
					sqlite3_backup* db_backup = NULL;

					sqlite3_exec(dataAccess.db,"END TRANSACTION",NULL,NULL,NULL);

					err = sqlite3_open(dataAccess.filePath.c_str(), &db_file);
					if (err != SQLITE_OK) {
						printf("TreeTimer error: sqlite3_open(%s) failed", dataAccess.filePath.c_str()); fflush(stdout);
						exit(EXIT_FAILURE);
					}

					db_backup = sqlite3_backup_init(db_file, "main", dataAccess.db, "main");
					if (db_backup != NULL) {
						//err = sqlite3_backup_step(db_backup, -1);
						//if ((err != SQLITE_OK) && (err != SQLITE_DONE)) {
						//	printf("TreeTimer error: sqlite3_backup_step() of mem->file failed\n"); fflush(stdout);
						//	exit(EXIT_FAILURE);
						//}
						err = SQLITE_OK;
						while (err == SQLITE_OK) {
							err = sqlite3_backup_step(db_backup, -1);
							if ((err != SQLITE_OK) && (err != SQLITE_DONE)) {
								printf("TreeTimer error: sqlite3_backup_step() of mem->file failed (code=%d)\n", err); fflush(stdout);
								exit(EXIT_FAILURE);
							}
						}

						err = sqlite3_backup_finish(db_backup);
						if (err != SQLITE_OK) {
							printf("TreeTimer error: sqlite3_backup_finish() of mem->file failed\n"); fflush(stdout);
							exit(EXIT_FAILURE);
						}
					}

					err = sqlite3_close(db_file);
					if (err != SQLITE_OK) {
						printf("TreeTimer error: sqlite3_close() of file DB failed (code=%d)\n", err); fflush(stdout);
						exit(EXIT_FAILURE);
					}

					err = sqlite3_close(dataAccess.db);
					if (err != SQLITE_OK) {
						printf("TreeTimer error: sqlite3_close() of in-memory failed (code=%d)\n", err); fflush(stdout);

						sqlite3_stmt* pStmt = sqlite3_next_stmt(dataAccess.db, NULL);
						if (pStmt != NULL) {
							char* expandedQuery = sqlite3_expanded_sql(pStmt);
							printf("TreeTimer error: Unfinalised query: %s\n", expandedQuery);
						}

						exit(EXIT_FAILURE);
					}
					dataAccess.db = NULL;
				}
			}
		}
	}
}
