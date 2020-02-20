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
			}

			TTSQLite3::~TTSQLite3()
			{

			}

			namespace drivers
			{
				void openConnection(TTSQLite3& dataAccess)
				{
					// Open a Database Connection
					int err = sqlite3_open(dataAccess.filePath.c_str(), &(dataAccess.db));
					sqlite3_exec(dataAccess.db,"BEGIN TRANSACTION",NULL,NULL,NULL);
				}

				void closeConnection(TTSQLite3& dataAccess)
				{
					sqlite3_exec(dataAccess.db,"END TRANSACTION",NULL,NULL,NULL);
					int err = sqlite3_close(dataAccess.db);
				}
			}
		}
	}
}
