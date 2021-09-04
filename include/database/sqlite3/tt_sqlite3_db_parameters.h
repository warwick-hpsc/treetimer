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

#ifndef TT_DATABASE_SQLITE3_PARAMETERS_INCLUDE_H
#define TT_DATABASE_SQLITE3_PARAMETERS_INCLUDE_H

#include "tt_sqlite3_db_access.h"
#include "sqlite3.h"
#include <string>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void writeSchemaParameterData(TTSQLite3& dataAccess);

				// ============================================================================================================

				void findParameterFloatDataID(TTSQLite3& dataAccess, std::string paramName, double value, int *paramFloatID);

				// ============================================================================================================

				void findParameterIntDataID(TTSQLite3& dataAccess, std::string paramName, int value, int *paramIntID);

				// ============================================================================================================

				void findParameterBoolDataID(TTSQLite3& dataAccess, std::string paramName, bool value, int *paramBoolID);

				// ============================================================================================================

				void findParameterStringDataID(TTSQLite3& dataAccess, std::string paramName, std::string value, int *paramStringID);

				// ============================================================================================================

				void writeParameterFloatData(TTSQLite3& dataAccess, std::string paramName, double value, int *paramFloatID);

				// ============================================================================================================

				void writeParameterIntData(TTSQLite3& dataAccess, std::string paramName, int value, int *paramIntID);

				// ============================================================================================================

				void writeParameterBoolData(TTSQLite3& dataAccess, std::string paramName, bool value, int *paramBoolID);

				// ============================================================================================================

				void writeParameterStringData(TTSQLite3& dataAccess, std::string paramName, std::string value, int *paramStringID);

				// ============================================================================================================
			}
		}
	}
}

#endif
