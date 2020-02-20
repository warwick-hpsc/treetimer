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

#ifndef TT_DATABASE_SQLITE3_ACCESS_INCLUDE_H
#define TT_DATABASE_SQLITE3_ACCESS_INCLUDE_H

#include "sqlite3.h"
#include <string>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			class TTSQLite3
			{
				public:
					TTSQLite3(std::string filePath);
					~TTSQLite3();

					std::string filePath;
					sqlite3 * db;
			};

			namespace drivers
			{
				void openConnection(TTSQLite3& dataAccess);
				void closeConnection(TTSQLite3& dataAccess);
			}
		}
	}
}

#endif
