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

#include "tt_sqlite3_db_app_config.h"
#include <vector>
#include "tt_sqlite3_db_parameters.h"
#include <iostream>

namespace treetimer
{
	namespace database
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				// Useless tables!

				// void writeSchemaApplicationConfigData(TTSQLite3& dataAccess)
				// {
				// 	char *zErrMsg = 0;
				// 	std::string stmt;
				// 	int err;

				// 	// === Table for generating an application configuration ID ===
				// 	stmt = "CREATE TABLE IF NOT EXISTS ApplicationConfigData(AppConfigID INTEGER, "
				// 																  "ApplicationID INTEGER, "
				// 																  "FOREIGN KEY(ApplicationID) REFERENCES ApplicationData(ApplicationID), "
				// 																  "PRIMARY KEY(AppConfigID)"
				// 																  ");";

				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	// === Tables for linking application config IDs to different global parameters ===

				// 	stmt = "CREATE TABLE IF NOT EXISTS ApplicationConfigFloatParams(AppConfigID INTEGER, "
				// 														    "ParamFloatID INTEGER, "
				// 															"FOREIGN KEY(AppConfigID) REFERENCES ApplicationConfigData(AppConfigID), "
				// 															"FOREIGN KEY(ParamFloatID) REFERENCES ParameterFloatData(ParamFloatID), "
				// 															"PRIMARY KEY(AppConfigID, ParamFloatID)"
				// 															");";

				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	stmt = "CREATE TABLE IF NOT EXISTS ApplicationConfigIntParams(AppConfigID INTEGER, "
				// 														    "ParamIntID INTEGER, "
				// 															"FOREIGN KEY(AppConfigID) REFERENCES ApplicationConfigData(AppConfigID), "
				// 															"FOREIGN KEY(ParamIntID) REFERENCES ParameterIntData(ParamIntID), "
				// 															"PRIMARY KEY(AppConfigID, ParamIntID)"
				// 															");";

				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	stmt = "CREATE TABLE IF NOT EXISTS ApplicationConfigBoolParams(AppConfigID INTEGER, "
				// 														    "ParamBoolID INTEGER, "
				// 															"FOREIGN KEY(AppConfigID) REFERENCES ApplicationConfigData(AppConfigID), "
				// 															"FOREIGN KEY(ParamBoolID) REFERENCES ParameterBoolData(ParamBoolID), "
				// 															"PRIMARY KEY(AppConfigID, ParamBoolID)"
				// 															");";

				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	stmt = "CREATE TABLE IF NOT EXISTS ApplicationConfigStringParams(AppConfigID INTEGER, "
				// 														    "ParamStringID INTEGER, "
				// 															"FOREIGN KEY(AppConfigID) REFERENCES ApplicationConfigData(AppConfigID), "
				// 															"FOREIGN KEY(ParamStringID) REFERENCES ParameterStringData(ParamStringID), "
				// 															"PRIMARY KEY(AppConfigID, ParamStringID)"
				// 															");";

				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);
				// }


				// // ToDo: Store as pairs in the vector?
				// void findApplicationConfigDataID(TTSQLite3& dataAccess,
				// 		 	 	 	 	 	 	 int applicationID,
				// 								 std::vector<std::string> intParamNames,  std::vector<int> intParamValues,
				// 								 std::vector<std::string> floatingParamNames,  std::vector<double> floatingParamValues,
				// 								 std::vector<std::string> boolParamNames,  std::vector<bool> boolParamValues,
				// 								 std::vector<std::string> stringParamNames,  std::vector<std::string> stringParamValues,
				// 								 int *applicationConfigID)
				// {
				// 	sqlite3_stmt *pStmt = nullptr;
				// 	char *zErrMsg = 0;
				// 	std::string stmt;
				// 	int err;

				// 	// This function is somewhat more complex that the other find functions
				// 	// We need to ensure an equivalent match across multiple tables, and ensure it is an exact match
				// 	// (i.e. not a subset), where all parameters specified are present and no additional parameters.
				// 	// To do this, we need to employ relational division on the database to find a suitable config id.

				// 	// Error Check: Sizes of paired vectors should be same (ToDo: store in vector as pair or pass map through?)

				// 	// Create Temporary Tables for the Parameter Identity Comparisons
				// 	stmt = "CREATE TEMPORARY TABLE IF NOT EXISTS IntParamMatches(ParamIntID INTEGER);";
				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	stmt = "CREATE TEMPORARY TABLE IF NOT EXISTS FloatParamMatches(ParamFloatID INTEGER);";
				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	stmt = "CREATE TEMPORARY TABLE IF NOT EXISTS BoolParamMatches(ParamBoolID INTEGER);";
				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	stmt = "CREATE TEMPORARY TABLE IF NOT EXISTS StringParamMatches(ParamStringID INTEGER);";
				// 	err = sqlite3_exec(dataAccess.db, stmt.c_str(), NULL, 0, &zErrMsg);

				// 	// === Retrieve and copy the relevant IDs into our temporary table ===
				// 	std::vector<int> potentialAppConfigIDs;

				// 	for(int i = 0; i < intParamNames.size(); i++)
				// 	{
				// 		int paramID;
				// 		findParameterIntDataID(dataAccess, intParamNames[i], intParamValues[i], &paramID);

				// 		if(paramID == -1)
				// 		{
				// 			// Parameter not found, therefore there is no appConfigID either.
				// 			*applicationConfigID = -1;
				// 			return;
				// 		}
				// 		else
				// 		{
				// 			// Insert into the temporary table
				// 			sqlite3_prepare(dataAccess.db,"INSERT INTO IntParamMatches VALUES(?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, paramID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in findApplicationConfigDataID  \n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 	}

				// 	for(int i = 0; i < floatingParamNames.size(); i++)
				// 	{
				// 		int paramID;
				// 		findParameterFloatDataID(dataAccess, floatingParamNames[i], floatingParamValues[i], &paramID);

				// 		if(paramID == -1)
				// 		{
				// 			// Parameter not found, therefore there is no appConfigID either.
				// 			*applicationConfigID = -1;
				// 			return;
				// 		}
				// 		else
				// 		{
				// 			// Insert into the temporary table
				// 			sqlite3_prepare(dataAccess.db,"INSERT INTO FloatParamMatches VALUES(?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, paramID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in findApplicationConfigDataID  \n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 	}

				// 	for(int i = 0; i < boolParamNames.size(); i++)
				// 	{
				// 		int paramID;
				// 		findParameterBoolDataID(dataAccess, boolParamNames[i], boolParamValues[i], &paramID);

				// 		if(paramID == -1)
				// 		{
				// 			// Parameter not found, therefore there is no appConfigID either.
				// 			*applicationConfigID = -1;
				// 			return;
				// 		}
				// 		else
				// 		{
				// 			// Insert into the temporary table
				// 			sqlite3_prepare(dataAccess.db,"INSERT INTO BoolParamMatches VALUES(?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, paramID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in findApplicationConfigDataID  \n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 	}

				// 	for(int i = 0; i < stringParamNames.size(); i++)
				// 	{
				// 		int paramID;
				// 		findParameterStringDataID(dataAccess, stringParamNames[i], stringParamValues[i], &paramID);

				// 		if(paramID == -1)
				// 		{
				// 			// Parameter not found, therefore there is no appConfigID either.
				// 			*applicationConfigID = -1;
				// 			return;
				// 		}
				// 		else
				// 		{
				// 			// Insert into the temporary table
				// 			sqlite3_prepare(dataAccess.db,"INSERT INTO StringParamMatches VALUES(?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, paramID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in findApplicationConfigDataID  \n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 	}


				// 	// Relational Division is now neccessary to get the AppConfigs associated with every parameterID
				// 	// those IDs

				// 	// R/S - R is the parameter table, S is the table of parameterIDs found for the parameter list
				// 	// T1 <- Project_AppConfigID(R)
				// 	// T2 <- Project_AppConfig((S x T1) - R)
				// 	// T <- T1 - T2

				// 	// Setup the views used for relational division (makes the commands more readable)
				// 	std::string viewCommands[20] = {
				// 			"CREATE TEMPORARY VIEW IF NOT EXISTS T1_INT AS SELECT DISTINCT AppConfigID FROM ApplicationConfigIntParams",
				// 			"CREATE TEMPORARY VIEW T2_1_INT AS SELECT AppConfigID, ParamIntID FROM T1_INT CROSS JOIN IntParamMatches",
				// 			"CREATE TEMPORARY VIEW T2_2_INT AS SELECT AppConfigID, ParamIntID FROM T2_1_INT EXCEPT SELECT AppConfigID, ParamIntID FROM ApplicationConfigIntParams",
				// 			"CREATE TEMPORARY VIEW T2_INT AS SELECT DISTINCT AppConfigID FROM T2_2_INT",
				// 			"CREATE TEMPORARY VIEW T_INT AS SELECT DISTINCT AppConfigID FROM T1_INT EXCEPT SELECT DISTINCT AppConfigID FROM T2_INT",
				// 			"CREATE TEMPORARY VIEW T1_FLOAT AS SELECT DISTINCT AppConfigID FROM ApplicationConfigFloatParams",
				// 			"CREATE TEMPORARY VIEW T2_1_FLOAT AS SELECT AppConfigID, ParamFloatID FROM T1_FLOAT CROSS JOIN FloatParamMatches",
				// 			"CREATE TEMPORARY VIEW T2_2_FLOAT AS SELECT AppConfigID, ParamFloatID FROM T2_1_FLOAT EXCEPT SELECT AppConfigID, ParamFloatID FROM ApplicationConfigFloatParams",
				// 			"CREATE TEMPORARY VIEW T2_FLOAT AS SELECT DISTINCT AppConfigID FROM T2_2_FLOAT",
				// 			"CREATE TEMPORARY VIEW T_FLOAT AS SELECT DISTINCT AppConfigID FROM T1_FLOAT EXCEPT SELECT DISTINCT AppConfigID FROM T2_FLOAT",
				// 			"CREATE TEMPORARY VIEW T1_BOOL AS SELECT DISTINCT AppConfigID FROM ApplicationConfigBoolParams",
				// 			"CREATE TEMPORARY VIEW T2_1_BOOL AS SELECT AppConfigID, ParamBoolID FROM T1_BOOL CROSS JOIN BoolParamMatches",
				// 			"CREATE TEMPORARY VIEW T2_2_BOOL AS SELECT AppConfigID, ParamBoolID FROM T2_1_BOOL EXCEPT SELECT AppConfigID, ParamBoolID FROM ApplicationConfigBoolParams",
				// 			"CREATE TEMPORARY VIEW T2_BOOL AS SELECT DISTINCT AppConfigID FROM T2_2_BOOL",
				// 			"CREATE TEMPORARY VIEW T_BOOL AS SELECT DISTINCT AppConfigID FROM T1_BOOL EXCEPT SELECT DISTINCT AppConfigID FROM T2_BOOL",
				// 			"CREATE TEMPORARY VIEW T1_STRING AS SELECT DISTINCT AppConfigID FROM ApplicationConfigStringParams",
				// 			"CREATE TEMPORARY VIEW T2_1_STRING AS SELECT AppConfigID, ParamStringID FROM T1_STRING CROSS JOIN StringParamMatches",
				// 			"CREATE TEMPORARY VIEW T2_2_STRING AS SELECT AppConfigID, ParamStringID FROM T2_1_STRING EXCEPT SELECT AppConfigID, ParamStringID FROM ApplicationConfigStringParams",
				// 			"CREATE TEMPORARY VIEW T2_STRING AS SELECT DISTINCT AppConfigID FROM T2_2_STRING",
				// 			"CREATE TEMPORARY VIEW T_STRING AS SELECT DISTINCT AppConfigID FROM T1_STRING EXCEPT SELECT DISTINCT AppConfigID FROM T2_STRING"
				// 	};

				// 	for(int i = 0; i < 20; i++)
				// 	{
				// 		sqlite3_prepare(dataAccess.db, viewCommands[i].c_str(), -1, &pStmt, NULL);
				// 		sqlite3_step(pStmt);
				// 		sqlite3_finalize(pStmt);
				// 	}

				// 	// Next, we need only the appConfigIDs that satisfy the relational division for all four tables
				// 	// Issue: If there are no parameters in a category, then relational division will return no ids
				// 	// This will break intersections of the results from each table.
				// 	// Only intersect them if the provided vector of values is greater than 0 (i.e. there is something
				// 	// to divide by)

				// 	int count;

				// 	if(intParamNames.size() > 0)
				// 	{
				// 		count = count + 1;
				// 	}

				// 	if(floatingParamNames.size() > 0)
				// 	{
				// 		count = count + 1;
				// 	}

				// 	if(boolParamNames.size() > 0)
				// 	{
				// 		count = count + 1;
				// 	}

				// 	if(stringParamNames.size() > 0)
				// 	{
				// 		count = count + 1;
				// 	}

				// 	if(count == 0)
				// 	{
				// 		// There are no parameters specified at all...
				// 		// Just get the full list of AppConfigIDs - we will need to find which (if any), has counts of zero

				// 		sqlite3_prepare(dataAccess.db,"SELECT DISTINCT AppConfigID FROM ApplicationConfigData", -1, &pStmt, NULL);

				// 		while(sqlite3_step(pStmt) == SQLITE_ROW)
				// 		{
				// 			potentialAppConfigIDs.push_back(sqlite3_column_int(pStmt, 0));
				// 		}

				// 		sqlite3_finalize(pStmt);
				// 	}
				// 	else if(count == 1)
				// 	{
				// 		// There is only table to lookup values from
				// 		if(intParamNames.size() > 0)
				// 		{
				// 			sqlite3_prepare(dataAccess.db,"SELECT * FROM T_INT", -1, &pStmt, NULL);

				// 			while(sqlite3_step(pStmt) == SQLITE_ROW)
				// 			{
				// 				potentialAppConfigIDs.push_back(sqlite3_column_int(pStmt, 0));
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 		else if(floatingParamNames.size() > 0)
				// 		{
				// 			sqlite3_prepare(dataAccess.db,"SELECT * FROM T_FLOAT", -1, &pStmt, NULL);

				// 			while(sqlite3_step(pStmt) == SQLITE_ROW)
				// 			{
				// 				potentialAppConfigIDs.push_back(sqlite3_column_int(pStmt, 0));
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 		else if(boolParamNames.size() > 0)
				// 		{
				// 			sqlite3_prepare(dataAccess.db,"SELECT * FROM T_BOOL", -1, &pStmt, NULL);

				// 			while(sqlite3_step(pStmt) == SQLITE_ROW)
				// 			{
				// 				potentialAppConfigIDs.push_back(sqlite3_column_int(pStmt, 0));
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 		else if(stringParamNames.size() > 0)
				// 		{
				// 			sqlite3_prepare(dataAccess.db,"SELECT * FROM T_STRING", -1, &pStmt, NULL);

				// 			while(sqlite3_step(pStmt) == SQLITE_ROW)
				// 			{
				// 				potentialAppConfigIDs.push_back(sqlite3_column_int(pStmt, 0));
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 	}
				// 	else if(count > 1)
				// 	{
				// 		// We need to intersect the results of two or more tables

				// 		int tmpCount = 0;
				// 		std::string preString;

				// 		if(intParamNames.size() > 0)
				// 		{
				// 			if(tmpCount == 0)
				// 			{
				// 				preString = "SELECT * FROM T_INT";
				// 			}
				// 			else
				// 			{
				// 				// Technically this case shouldn't be possible, could remove.
				// 				preString = preString + "INTERSECT SELECT * FROM T_INT";
				// 			}

				// 			tmpCount = tmpCount + 1;

				// 		}
				// 		else if(floatingParamNames.size() > 0)
				// 		{
				// 			if(tmpCount == 0)
				// 			{
				// 				preString = "SELECT * FROM T_FLOAT";
				// 			}
				// 			else
				// 			{
				// 				preString = preString + "INTERSECT SELECT * FROM T_FLOAT";
				// 			}

				// 			tmpCount = tmpCount + 1;
				// 		}
				// 		else if(boolParamNames.size() > 0)
				// 		{
				// 			if(tmpCount == 0)
				// 			{
				// 				preString = "SELECT * FROM T_BOOL";
				// 			}
				// 			else
				// 			{
				// 				preString = preString + "INTERSECT SELECT * FROM T_BOOL";
				// 			}

				// 			tmpCount = tmpCount + 1;
				// 		}
				// 		else if(stringParamNames.size() > 0)
				// 		{
				// 			if(tmpCount == 0)
				// 			{
				// 				preString = "SELECT * FROM T_STRING";
				// 			}
				// 			else
				// 			{
				// 				preString = preString + "INTERSECT SELECT * FROM T_STRING";
				// 			}

				// 			tmpCount = tmpCount + 1;
				// 		}

				// 		sqlite3_prepare(dataAccess.db, preString.c_str(), -1, &pStmt, NULL);

				// 		while(sqlite3_step(pStmt) == SQLITE_ROW)
				// 		{
				// 			potentialAppConfigIDs.push_back(sqlite3_column_int(pStmt, 0));
				// 		}

				// 		sqlite3_finalize(pStmt);
				// 	}

				// 	// Cleanup Temporary Views/Tables, just in case.
				// 	std::string cleanupCommands[24] = {
				// 			"DROP VIEW temp.T1_INT",
				// 			"DROP VIEW temp.T1_FLOAT",
				// 			"DROP VIEW temp.T1_BOOL",
				// 			"DROP VIEW temp.T1_STRING",
				// 			"DROP VIEW temp.T2_1_INT",
				// 			"DROP VIEW temp.T2_1_FLOAT",
				// 			"DROP VIEW temp.T2_1_BOOL",
				// 			"DROP VIEW temp.T2_1_STRING",
				// 			"DROP VIEW temp.T2_2_INT",
				// 			"DROP VIEW temp.T2_2_FLOAT",
				// 			"DROP VIEW temp.T2_2_BOOL",
				// 			"DROP VIEW temp.T2_2_STRING",
				// 			"DROP VIEW temp.T2_INT",
				// 			"DROP VIEW temp.T2_FLOAT",
				// 			"DROP VIEW temp.T2_BOOL",
				// 			"DROP VIEW temp.T2_STRING",
				// 			"DROP VIEW temp.T_INT",
				// 			"DROP VIEW temp.T_FLOAT",
				// 			"DROP VIEW temp.T_BOOL",
				// 			"DROP VIEW temp.T_STRING",
				// 			"DROP TABLE temp.IntParamMatches",
				// 			"DROP TABLE temp.FloatParamMatches",
				// 			"DROP TABLE temp.BoolParamMatches",
				// 			"DROP TABLE temp.StringParamMatches"
				// 	};

				// 	for(int i = 0; i < 24; i++)
				// 	{
				// 		sqlite3_prepare(dataAccess.db, cleanupCommands[i].c_str(), -1, &pStmt, NULL);
				// 		sqlite3_step(pStmt);
				// 		sqlite3_finalize(pStmt);
				// 	}

				// 	// Finally, we want an exact match on parameters (not a subset), so we select through the four tables
				// 	// and check that the number of rows for that appConfigID is equal to the number of parameters
				// 	// we are searching for (we have confirmed by this point that, at a minimum, it has those parameters)
				// 	// If they have all of the parameters (i.e. the subset) and the count is equal then it must be uniquely
				// 	// the correct id.
				// 	// If they are equal across all four tables, then this is the correct appConfigID, else check the next.

				// 	int sqlCount;

				// 	for(int i = 0; i < potentialAppConfigIDs.size(); i++)
				// 	{
				// 		sqlite3_prepare(dataAccess.db, "SELECT COUNT(*) FROM ApplicationConfigIntParams WHERE AppConfigID = ?", -1, &pStmt, NULL);
				// 		sqlite3_bind_int(pStmt,1, potentialAppConfigIDs[i]);
				// 		err = sqlite3_step(pStmt);

				// 		sqlCount = sqlite3_column_int(pStmt, 0);
				// 		sqlite3_finalize(pStmt);

				// 		if(sqlCount != intParamNames.size())
				// 		{
				// 			continue;
				// 		}

				// 		sqlite3_prepare(dataAccess.db, "SELECT COUNT(*) FROM ApplicationConfigFloatParams WHERE AppConfigID = ?", -1, &pStmt, NULL);
				// 		sqlite3_bind_int(pStmt,1, potentialAppConfigIDs[i]);
				// 		err = sqlite3_step(pStmt);

				// 		sqlCount = sqlite3_column_int(pStmt, 0);
				// 		sqlite3_finalize(pStmt);

				// 		if(sqlCount != floatingParamNames.size())
				// 		{
				// 			continue;
				// 		}

				// 		sqlite3_prepare(dataAccess.db, "SELECT COUNT(*) FROM ApplicationConfigBoolParams WHERE AppConfigID = ?", -1, &pStmt, NULL);
				// 		sqlite3_bind_int(pStmt,1, potentialAppConfigIDs[i]);
				// 		err = sqlite3_step(pStmt);

				// 		sqlCount = sqlite3_column_int(pStmt, 0);
				// 		sqlite3_finalize(pStmt);

				// 		if(sqlCount != boolParamNames.size())
				// 		{
				// 			continue;
				// 		}

				// 		sqlite3_prepare(dataAccess.db, "SELECT COUNT(*) FROM ApplicationConfigStringParams WHERE AppConfigID = ?", -1, &pStmt, NULL);
				// 		sqlite3_bind_int(pStmt,1, potentialAppConfigIDs[i]);
				// 		err = sqlite3_step(pStmt);

				// 		sqlCount = sqlite3_column_int(pStmt, 0);
				// 		sqlite3_finalize(pStmt);

				// 		if(sqlCount != stringParamNames.size())
				// 		{
				// 			continue;
				// 		}

				// 		// If this point is reached, sizes matched for all four vectors, AND we have confirmed that the ID in question
				// 		// has all of the parameters. Therefore this is the correct ID.

				// 		*applicationConfigID = potentialAppConfigIDs[i];
				// 		return;
				// 	}

				// 	// If we reach this point, then we never found a suitable ID (e.g could be a subset of existing configuration,
				// 	// but does not exist by itself), therefore there is not one.
				// 	*applicationConfigID = -1;
				// 	return;
				// }

				// void writeApplicationConfigData(TTSQLite3& dataAccess,
				// 		 int applicationID,
				// 		 std::vector<std::string> intParamNames,  std::vector<int> intParamValues,
				// 		 std::vector<std::string> floatingParamNames,  std::vector<double> floatingParamValues,
				// 		 std::vector<std::string> boolParamNames,  std::vector<bool> boolParamValues,
				// 		 std::vector<std::string> stringParamNames,  std::vector<std::string> stringParamValues,
				// 		 int *applicationConfigID)
				// {
				// 	sqlite3_stmt *pStmt = nullptr;
				// 	int err;

				// 	// Error Check: Pair Vectors should be of same size

				// 	// Check for existing entry
				// 	int tmpID;
				// 	findApplicationConfigDataID(dataAccess,
				// 			 applicationID,
				// 			 intParamNames, intParamValues,
				// 			 floatingParamNames, floatingParamValues,
				// 			 boolParamNames, boolParamValues,
				// 			 stringParamNames, stringParamValues,
				// 			 &tmpID);

				// 	if(tmpID == -1)
				// 	{
				// 		// For each parameter:
				// 		// (a) Ensure the parameters are written into the global parameter tables
				// 		// (b) Create an entry in the application config table to generate a new application config id
				// 		// (c) Make an entry in the config->parameterID cross-reference tables to link the two ids together


				// 		// (b) Create the application config entry
				// 		sqlite3_prepare(dataAccess.db,"INSERT INTO ApplicationConfigData VALUES(NULL, ?)", -1, &pStmt, NULL);
				// 		sqlite3_bind_int(pStmt,1, applicationID);
				// 		err = sqlite3_step(pStmt);

				// 		if(err == SQLITE_ERROR)
				// 		{
				// 			std::cout << "SQL Error encountered in writeApplicationConfigData\n";
				// 			char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 			std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 			// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 			sqlite3_free(expandedQuery);
				// 			*applicationConfigID = -1;
				// 		}
				// 		else
				// 		{
				// 			*applicationConfigID = sqlite3_last_insert_rowid(dataAccess.db);
				// 		}

				// 		sqlite3_finalize(pStmt);

				// 		// (c) Ensure that the application config ID is linked to the various global parameters
				// 		for(int i = 0; i < intParamNames.size(); i++)
				// 		{
				// 			int paramIntID;
				// 			writeParameterIntData(dataAccess, intParamNames[i], intParamValues[i], &paramIntID);

				// 			err = sqlite3_prepare(dataAccess.db,"INSERT INTO ApplicationConfigIntParams VALUES(?, ?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, *applicationConfigID);
				// 			sqlite3_bind_int(pStmt,2, paramIntID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in writeApplicationConfigData\n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 				// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}

				// 		for(int i = 0; i < floatingParamNames.size(); i++)
				// 		{
				// 			int paramFloatID;
				// 			writeParameterFloatData(dataAccess, floatingParamNames[i], floatingParamValues[i], &paramFloatID);

				// 			err = sqlite3_prepare(dataAccess.db,"INSERT INTO ApplicationConfigFloatParams VALUES(?, ?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, *applicationConfigID);
				// 			sqlite3_bind_int(pStmt,2, paramFloatID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in writeApplicationConfigData\n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 				// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}

				// 		for(int i = 0; i < boolParamNames.size(); i++)
				// 		{
				// 			int paramBoolID;
				// 			writeParameterBoolData(dataAccess, boolParamNames[i], boolParamValues[i], &paramBoolID);

				// 			err = sqlite3_prepare(dataAccess.db,"INSERT INTO ApplicationConfigBoolParams VALUES(?, ?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, *applicationConfigID);
				// 			sqlite3_bind_int(pStmt,2, paramBoolID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in writeApplicationConfigData\n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 				// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}

				// 		for(int i = 0; i < stringParamNames.size(); i++)
				// 		{
				// 			int paramStringID;
				// 			writeParameterStringData(dataAccess, stringParamNames[i], stringParamValues[i], &paramStringID);

				// 			err = sqlite3_prepare(dataAccess.db,"INSERT INTO ApplicationConfigStringParams VALUES(?, ?)", -1, &pStmt, NULL);
				// 			sqlite3_bind_int(pStmt,1, *applicationConfigID);
				// 			sqlite3_bind_int(pStmt,2, paramStringID);
				// 			err = sqlite3_step(pStmt);

				// 			if(err == SQLITE_ERROR)
				// 			{
				// 				std::cout << "SQL Error encountered in writeApplicationConfigData\n";
				// 				char *expandedQuery = sqlite3_expanded_sql(pStmt);
				// 				std::cout << "Failed query: " << std::string(expandedQuery) << "\n";

				// 				// sqlite3_expanded_sql is not automatically freed by the sqlite3 library on finalize (unlike sqlite3_sql)
				// 				sqlite3_free(expandedQuery);
				// 			}

				// 			sqlite3_finalize(pStmt);
				// 		}
				// 	}
				// 	else
				// 	{
				// 		*applicationConfigID = tmpID;
				// 	}
				// }
			}
		}
	}
}
