#!/usr/bin/python

# This file contains functions pertaining to the building of the central database - i.e.:
# (1) Schemas for the database tables
# (2) Functions for copying data from gathered results into the central database in a manner that keeps
#     runs consistently tracked and reduces duplication.

# ToDo: Python interface doesn't have prepared statements?
# ToDo: Use executemany over individual function calls for multiple rows?

import sqlite3

def openConnection(dbPath):
	db = sqlite3.connect(dbPath)

	# Set up connection such that results are returned as rows
	# (so we can access returned data using keys rather than indexes)
	db.row_factory = sqlite3.Row

	# Begin Transaction (in case we need to rollback)

	return db

def closeConnection(db):

	# End Transaction

	# Close Connection
	db.close()

def testDatabase(dbPath):
	createDatabase(dbPath)

def createDatabase(dbPath):
	# Test if file already exists

	# Open Connection
    db = openConnection(dbPath)

    # =====================
	# === Write Schemas ===
    # =====================

    # === Software Configuration Data ===
    # (1) Application Data
    writeSchemaApplicationData(db)

    # (2) Application Configuration Data

    # Parameter storage needed to store application parameter values
    writeSchemaParameterFloat(db)
    writeSchemaParameterInt(db)
    writeSchemaParameterBool(db)
    writeSchemaParameterString(db)

    # Tables that tie various parameters to a configuration ID
    writeSchemaAppConfig(db)

    # (3) Library Configuration Data
    writeSchemaLibraryConfig(db)

    # (4) Run ID Data
    writeSchemaRunConfiguration(db)

    # === Machine Data ===
    # (5) Machine Data
    writeSchemaMachineData(db)

    # (6) CPU Data
    writeSchemaCPUData(db)

    # (7) Compute Node Data
    writeSchemaComputeNodeData(db)

    # (8) Socket Data
    writeSchemaSocketData(db)

    # === Profiling Data ===
    # (9) Process Data
    writeSchemaProcessData(db)

    # (10) Profile Node Type Data
    writeSchemaProfileNodeTypeData(db)

    # (11) Profile Node Data
    writeSchemaProfileNodeData(db)

    # (12) Profile Node CallPath Data
    writeSchemaCallPathData(db)

    # (10) Aggregate Timer Data
    writeSchemaAggregateTimeData(db)

    # (11) Aggregate Parameter Data
    writeSchemaAggParamDataInt(db)
    writeSchemaAggParamDataFloat(db)
    writeSchemaAggParamDataBool(db)

    # (12) Aggregate PAPI Data
    # N/A - ToDo

    # (13) Trace Timer Data
    writeSchemaTraceTimeData(db)

    # (14) Trace Parameter Data
    writeSchemaTraceParameterDataInt(db)
    writeSchemaTraceParameterDataFloat(db)
    writeSchemaTraceParameterDataBool(db)

    # (15) Trace PAPI Data
    # N/A - ToDo

	# === Close Connection ===
    db.close()

def addRunToDatabase(runDataPath, dbPath):

    # Run Data Structure
    # Data from a profiling run is spread out across multiple files in a folder, one per process
    # In addition, there is a selection of data that is common to all processes (i.e. global properties
    # of a run)

    # First, we should store the global properties, since many of these are referenced in profile data

    # === Software Configuration Data ===
    # (1) Application Data

    # (2) Application Configuration Data

    # (3) Library Configuration Data

    # (4) Run ID Data - At this stage, it should be possible to identify a run uniquely

    # === Machine Data ===
    # (5) Machine Data

    # (6) Node Data

    # (7) Socket Data

    # === Profiling Data ===
    # (8) Process Data

    # (9) Aggregate Timer Data

    # (10) Aggregate Parameter Data

    # (11) Aggregate PAPI Data

    # (12) Trace Timer Data

    # (13) Trace Parameter Data

    # (14) Trace PAPI Data

	pass

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaMachineData(db):

    command = "CREATE TABLE IF NOT EXISTS MachineData(MachineID INTEGER, " + \
                                                     "MachineName TEXT, " + \
                                                     "PRIMARY KEY(MachineID));"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findMachineID(db, machineName):

	command = "SELECT MachineID FROM MachineData WHERE MachineName LIKE '" + sqlEscapeChar(machineName) + "';"

	c = db.cursor()
	c.execute(command)

	result = c.fetchone()
	c.close()

	if(result is None):
		return None
	else:
		return result['MachineID']

#########################################################################################################

def findMachineData(db, machineID):

	command = "SELECT * FROM MachineData WHERE MachineID = " + sqlEscapeChar(machineID) + ";"

	c = db.cursor()
	c.execute(command)
	result = c.fetchone()
	c.close()

	if(result is None):
		return None
	else:
		return result

#########################################################################################################

# Returns the Machine ID of a row that contains the exact provided machine details.
# If no such row yet exists, insert a new row into the table and return the newly created machine ID.
# If such a row already exists, return the machine ID of that row rather than creating a duplicate row.

def addMachineData(db, machineName):

	# Search for an existing matching row
	machineID = findMachineID(db, machineName)

	if(machineID is None):

		command = "INSERT INTO MachineData VALUES(" + \
											  "NULL, " + \
											  "'" + sqlEscapeChar(machineName) + "'" + \
											  ");"

		c = db.cursor()
		c.execute(command)
		machineID = c.lastrowid
		c.close()

	return machineID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaCPUData(db):

	command = "CREATE TABLE IF NOT EXISTS CPUData(CpuID INTEGER, " + \
												 "CpuName TEXT, " + \
												 "PRIMARY KEY(CpuID));"

	c = db.cursor()
	c.execute(command)
	c.close()

def findCpuID(db, cpuName):

	command = "SELECT CpuID FROM CPUData WHERE CpuName LIKE '" + cpuName + "';"

	c = db.cursor()
	c.execute(command)
	result = c.fetchone()
	c.close()

	if(result is None):
		return None
	else:
		return result['CpuID']


def findCpuData(db, cpuID):

	command = "SELECT * FROM CPUData WHERE CpuID = " + sqlEscapeChar(cpuID) + ";"

	c = db.cursor()
	c.execute(command)
	result = c.fetchone()
	c.close()

	if(result is None):
		return None
	else:
		return result

def addCpuData(db, cpuName):

	cpuID = findCpuID(db, cpuName)

	if(cpuID is None):
		command = "INSERT INTO CPUData VALUES("+ \
										     "NULL, " + \
											 "'" + sqlEscapeChar(cpuName) + "'" + \
											 ");"

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaComputeNodeData(db):

    command = "CREATE TABLE IF NOT EXISTS ComputeNodeData(ComputeNodeID INTEGER, " + \
                                                         "MachineID INTEGER, " + \
                                                         "NodeName TEXT, " + \
														 "SocketCount INTEGER, " + \
                                                         "PRIMARY KEY(ComputeNodeID), " + \
														 "FOREIGN KEY(MachineID) REFERENCES MachineData(MachineID)" + \
                                                         ");"

    c = db.cursor()
    c.execute(command)
    c.close()

def findComputeNodeID(db, machineID, nodeName, socketCount):

    command = "SELECT ComputeNodeID FROM ComputeNodeData WHERE " + \
                                                        "MachineID = " + sqlEscapeChar(machineID) + " AND " + \
                                                        "NodeName LIKE '" + sqlEscapeChar(nodeName) + "'" + " AND " + \
                                                        "SocketCount = " + sqlEscapeChar(socketCount) + \
                                                        ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ComputeNodeID']

def findComputeNodeData(db, computeNodeID):

    command = "SELECT * FROM ComputeNodeData WHERE ComputeNodeID = " + sqlEscapeChar(computeNodeID) + ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result

def addComputeNodeData(db, machineID, nodeName, socketCount):

    computeNodeID = findComputeNodeID(db, machineID, nodeName, socketCount)

    if(computeNodeID is None):
        command = "INSERT INTO ComputeNodeData VALUES(" + \
                                                      "NULL, " + \
                                                      sqlEscapeChar(machineID) + "," + \
                                                      "'" + sqlEscapeChar(nodeName) + "'" + "," + \
                                                      sqlEscapeChar(socketCount) + \
                                                      ");"

        c.execute(command)
        computeNodeID = c.lastrowid

    return computeNodeID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaSocketData(db):

    command = "CREATE TABLE IF NOT EXISTS SocketData(SocketID INTEGER, " + \
                                                    "ComputeNodeID INTEGER, " + \
                                                    "CpuID INTEGER, " + \
                                                    "PhysicalSocketNum INTEGER, " + \
                                                    "PRIMARY KEY(SocketID), " + \
                                                    "UNIQUE(ComputeNodeID, PhysicalSocketNum), " + \
                                                    "FOREIGN KEY(ComputeNodeID) REFERENCES ComputeNodeData(ComputeNodeID), " + \
                                                    "FOREIGN KEY(CpuID) REFERENCES CPUData(CpuID) " + \
                                                    ");"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findSocketID(db, computeNodeID, cpuID, physicalSocketNum):

    command = "SELECT SocketID FROM SocketData WHERE " + \
                                              "ComputeNodeID = " + sqlEscapeChar(computeNodeID) + " AND " + \
                                              "CpuID = " + sqlEscapeChar(cpuID) + " AND " + \
                                              "PhysicalSocketNum = " + sqlEscapeChar(physicalSocketNum) + \
                                              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['SocketID']

#########################################################################################################

def findSocketData(db, socketID):

    command = "SELECT * FROM SocketData WHERE SocketID = " + sqlEscapeChar(socketID) + ";"

    c = db.cursor
    c.execute(command)
    result = c.fetchone()

    if(result is None):
        return None
    else:
        return result

#########################################################################################################

def addSocketData(db, computeNodeID, cpuID, physicalSocketID):

    socketID = findSocketID(db, computeNodeID, cpuID, physicalSocketNum)

    if(socketID is None):
        command = "INSERT INTO SocketData VALUES(" + \
                                          "NULL, " + \
                                          sqlEscapeChar(computeNodeID) + "," + \
                                          sqlEscapeChar(cpuID) + "," + \
                                          sqlEscapeChar(physicalSocketNum) + \
                                          ");"

        c = db.cursor()
        c.execute(command)
        socketID = c.lastrowid
        c.close()

    return socketID

#########################################################################################################
#########################################################################################################
#########################################################################################################


def writeSchemaProfileNodeTypeData(db):

    command = "CREATE TABLE IF NOT EXISTS ProfileNodeType(ProfileNodeTypeID INTEGER, " + \
                                                         "TypeName TEXT, " + \
                                                         "PRIMARY KEY(ProfileNodeTypeID))"

    c = db.cursor()
    c.execute(command)
    c.close()

def findProfileNodeTypeID(db, typeName):

    command = "SELECT ProfileNodeTypeID FROM ProfileNodeType WHERE " + \
                                                            "TypeName LIKE '" + sqlEscapeChar(typeName) + "'" + \
                                                            ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ProfileNodeTypeID']

def findProfileNodeTypeDetails(db, profileNodeTypeID):

    command = "SELECT * FROM ProfileNodeType WHERE ProfileNodeTypeID = " + strEscapeChar(profileNodeTypeID) + ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result

def addProfileNodeTypeData(db, typeName):

    profileNodeTypeID = findProfileNodeTypeID(db, typeName)

    if(profileNodeTypeID is None):
        command = "INSERT INTO ProfileNodeType VALUES(" + \
                                                     "NULL, " + \
                                                     "'" + sqlEscapeChar(typeName) + "'" + \
                                                     ");"

        c = db.cursor()
        c.execute(command)
        profileNodeTypeID = c.lastrowid
        c.close()

    return profileNodeTypeID


#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaApplicationData(db):

    command = "CREATE TABLE IF NOT EXISTS ApplicationData(ApplicationID INTEGER, " + \
                                                         "ApplicationName TEXT, " + \
                                                         "ApplicationVersion TEXT, " + \
                                                         "PRIMARY KEY(ApplicationID));"

    c = db.cursor()
    c.execute(command)
    c.close()

def findApplicationID(db, appName, appVersion):

    command = "SELECT ApplicationID FROM ApplicationData WHERE " + \
                                                        "ApplicationName LIKE '" + sqlEscapeChar(appName) + "'" + " AND " + \
                                                        "ApplicationVersion LIKE '" + sqlEscapeChar(appVersion) + "'" + \
                                                        ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ApplicationID']

def findApplicationData(db, applicationID):

    command = "SELECT * FROM ApplicationData WHERE ApplicationID = " + sqlEscapeChar(applicationID) + ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result

def addApplicationData(db, appName, appVersion):

    applicationID = findApplicationID(db, appName, appVersion)

    if(applicationID is None):
        command = "INSERT INTO ApplicationData VALUES(" + \
                                               "NULL," + \
                                               "'" + sqlEscapeChar(appName) + "'" + "," + \
                                               "'" + sqlEscapeChar(appVersion) + "'" + \
                                               ");"

        c = db.cursor()
        c.execute(command)
        applicationID = c.lastrowid
        c.close()

    return applicationID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaProcessData(db):
    command = "CREATE TABLE IF NOT EXISTS ProcessData(ProcessID INTEGER, " + \
                                                     "MPIRank INTEGER, " + \
                                                     "SocketID INTEGER, " + \
                                                     "PhysicalCoreID INTEGER, " + \
                                                     "PRIMARY KEY(ProcessID));"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findProcessID(db, mpiRank, socketID, physicalCoreID):

    command = "SELECT ProcessID FROM ProcessData WHERE " + \
                                                 "MPIRank = " + sqlEscapeChar(mpiRank) + " AND " + \
                                                 "SocketID = " + sqlEscapeChar(socketID) + " AND " + \
                                                 "PhysicalCoreID = " + sqlEscapeChar(physicalCoreID) + \
                                                 ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ProcessID']

#########################################################################################################

def findProcessData(db, processID):

    command = "SELECT * FROM ProcessData WHERE ProcessID = " + sqlEscapeChar(processID) + ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result

#########################################################################################################

def addProcessData(db, mpiRank, socketID, physicalCoreID):

    processID = findProcessID(db, mpiRank, socketID, physicalCoreID)

    if(processID is None):
        command = "INSERT INTO ProcessData VALUES(" + \
                                           sqlEscapeChar(mpiRank) + "," + \
                                           sqlEscapeChar(socketID) + "," + \
                                           sqlEscapeChar(physicalCoreID) + \
                                           ");"

        c = db.cursor()
        c.execute(command)
        processID = c.lastrowid
        c.close()

    return processID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaRunConfiguration(db):

    # A run has the following qualities:
    # (1) The Application
    # (2) The Application Configuration (Parameters)
    # (3) The Process Count

    # Other qualitites (nodes, machine name etc) are defined by the profile nodes (e.g. it could be
    # possible to operate across one or more machines in a cloud environment), and are tied to a runID
    # in those tables

    # Note (ApplicationID, ApplicationConfigID) is not unique - can have multiple runs with same config
    command = "CREATE TABLE IF NOT EXISTS ProfileRunData(RunID INTEGER, " + \
                                                        "AppConfigID INTEGER, " + \
                                                        "LibraryConfigID, " + \
                                                        "ProcessCount INTEGER, " + \
                                                        "PRIMARY KEY(RunID));"
    c = db.cursor()
    c.execute(command)
    c.close()

def addRunID(db, appConfigID, processCount):

    # RunIDs are always treated as new, since we can have multiple runs of the same configuration

    command = "INSERT INTO ProfileRunData VALUES(" + \
              "NULL, " + \
              sqlEscapeChar(appConfigID) + "," + \
              sqlEscapeChar(libConfigID) + "," + \
              sqlEscapeChar(processCount) + "," + \
              ");"

    c = db.cursor()
    c.execute(command)
    runID = c.lastrowid
    c.close()

    return runID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaAppConfig(db):

    # Problem Config is unique in this table only, to manage the creation of unique ids.
    # However, it can be paired with multiple configurations of the same application id -
    # the parameters are spread out across multiple tables due to the variability in their
    # amount and type.

    c = db.cursor()
    command = "CREATE TABLE IF NOT EXISTS AppConfig(" + \
              "AppConfigID INTEGER, " + \
              "ApplicationID INTEGER" + \
              ");"

    c.execute(command)

    # Table for linking floating-point application parameters to a configuration
    command = "CREATE TABLE IF NOT EXISTS AppConfigFloat(AppConfigID INTEGER, " + \
                                                        "ParamFloatID INTEGER)"
    c.execute(command)

    # Table for linking integer application parameters to a configuration
    command = "CREATE TABLE IF NOT EXISTS AppConfigInteger(AppConfigID INTEGER, " + \
                                                          "ParamIntID INTEGER)"
    c.execute(command)

    # Table for linking boolean application parameters to a configuration
    # N.B. This still uses an integer table underneath, but the seperation of tables
    # allows us to distinguish between booleans and integers at a higher level
    command = "CREATE TABLE IF NOT EXISTS AppConfigBoolean(AppConfigID INTEGER, " + \
                                                          "ParamBoolID INTEGER)"
    c.execute(command)

    # Table for linking string application parameters to a configuration
    command = "CREATE TABLE IF NOT EXISTS AppConfigString(AppConfigID INTEGER, " + \
                                                         "ParamStringID INTEGER)"
    c.execute(command)
    c.close()

def findAppConfig(db, applicationID, intParamDict, floatParamDict, boolParamDict, stringParamDict):

    intIDList = []
    floatIDList = []
    boolIDList = []
    stringIDList = []

    # For each parameter of each type:
    # (1) Find if a parameterID exists
    # (1a) If it does, add it to a list
    # (1b) If it does not, then this problem configuration does not exist, and we can return none

    # Integers
    for paramKey in intParamDict.keys():
        val = intParamDict[paramKey]
        paramID = findParameterIntID(db, paramKey, val)

        if(paramID is None):
            return None
        else:
            intIDList.append(paramID)

    # Floats
    for paramKey in floatParamDict.keys():
        val = floatParamDict[paramKey]
        paramID = findParameterFloatID(db, paramKey, val)

        if(paramID is None):
            return None
        else:
            floatIDList.append(paramID)

    # Booleans
    for paramKey in boolParamDict.keys():
        val = boolParamDict[paramKey]
        paramID = findParameterBoolID(db, paramKey, val)

        if(paramID is None):
            return None
        else:
            boolIDList.append(paramID)

    # Strings
    for paramKey in stringParamDict.keys():
        val = stringParamDict[paramKey]
        paramID = findParameterStringID(db, paramKey, val)

        if(paramID is None):
            return None
        else:
            stringIDList.append(paramID)

    # If we got this far, then all of the parameters <name,val> pairs already existed. Now we must check whether there is a single
    # AppConfigID that has a match for all of the IDs across the tables

    # Convert integer list into string
    intIDString = ""
    for i in intIDList:
        intIDString = intIDString + sqlEscapeChar(i) + ","

    if(intIDString != ""):
        # Remove last comma
        intIDString = intIDString[:-1]

    if(intIDString != ""):
        # Relation division gets us all AppConfigIDs that have all of the ids as part of them (but not exclusively)
        # https://stackoverflow.com/questions/43306078/relational-division-in-sql-with-sqlite
        # ToDo: Replace this with views + set operations instead - main blocking point is cross-joining list with table
        command = "SELECT DISTINCT AppConfigID FROM AppConfigInteger AS X " + \
                  "WHERE NOT EXISTS(" + \
                    "SELECT * FROM AppConfigInteger AS Y " + \
                    "WHERE Y.ParamIntID IN (" + sqlEscapeChar(intIDString) + ") " + \
                      "AND NOT EXISTS(" + \
                        "SELECT * FROM ParamIntID AS Z " + \
                        "WHERE Z.AppConfigID = X.AppConfigID AND Z.ParamIntID = Y.ParamIntID" + \
                      ")" + \
                  ")"

        c = db.cursor()
        c.execute(command)
        intResult = c.fetchall()
        c.close()

    # Convert float list into string
    floatIDString = ""
    for i in floatIDList:
        floatIDString = floatIDString + sqlEscapeChar(i) + ","

    if(floatIDString != ""):
        # Remove last comma
        floatIDString = floatIDString[:-1]

    if(floatIDString != ""):
        # Relation division gets us all AppConfigIDs that have all of the ids as part of them (but not exclusively)
        # https://stackoverflow.com/questions/43306078/relational-division-in-sql-with-sqlite
        # ToDo: Replace this with views + set operations instead - main blocking point is cross-joining list with table
        command = "SELECT DISTINCT AppConfigID FROM AppConfigFloat AS X " + \
                  "WHERE NOT EXISTS(" + \
                    "SELECT * FROM AppConfigFloat AS Y " + \
                    "WHERE Y.ParamFloatID IN (" + sqlEscapeChar(floatIDString) + ") " + \
                      "AND NOT EXISTS(" + \
                        "SELECT * FROM ParamFloatID AS Z " + \
                        "WHERE Z.AppConfigID = X.AppConfigID AND Z.ParamFloatID = Y.ParamFloatID" + \
                      ")" + \
                  ")"

        c = db.cursor()
        c.execute(command)
        floatResult = c.fetchall()
        c.close()

    # Convert boolean list into string
    boolIDString = ""
    for i in boolIDList:
        boolIDString = boolIDString + sqlEscapeChar(i) + ","

    if(boolIDString != ""):
        # Remove last comma
        boolIDString = boolIDString[:-1]

    if(boolIDString != ""):
        # Relation division gets us all AppConfigIDs that have all of the ids as part of them (but not exclusively)
        # https://stackoverflow.com/questions/43306078/relational-division-in-sql-with-sqlite
        # ToDo: Replace this with views + set operations instead - main blocking point is cross-joining list with table
        command = "SELECT DISTINCT AppConfigID FROM AppConfigBoolean AS X " + \
                  "WHERE NOT EXISTS(" + \
                    "SELECT * FROM AppConfigBoolean AS Y " + \
                    "WHERE Y.ParamBoolID IN (" + sqlEscapeChar(boolIDString) + ") " + \
                      "AND NOT EXISTS(" + \
                        "SELECT * FROM ParamBoolID AS Z " + \
                        "WHERE Z.AppConfigID = X.AppConfigID AND Z.ParamBoolID = Y.ParamBoolID" + \
                      ")" + \
                  ")"

        c = db.cursor()
        c.execute(command)
        boolResult = c.fetchall()
        c.close()

    # Convert string ID list into string
    stringIDString = ""
    for i in stringIDList:
        stringIDString = stringIDString + sqlEscapeChar(i) + ","

    if(stringIDString != ""):
        # Remove last comma
        stringIDString = stringIDString[:-1]

    if(stringIDString != ""):
        # Relation division gets us all AppConfigIDs that have all of the ids as part of them (but not exclusively)
        # https://stackoverflow.com/questions/43306078/relational-division-in-sql-with-sqlite
        # ToDo: Replace this with views + set operations instead - main blocking point is cross-joining list with table
        command = "SELECT DISTINCT AppConfigID FROM AppConfigString AS X " + \
                  "WHERE NOT EXISTS(" + \
                    "SELECT * FROM AppConfigString AS Y " + \
                    "WHERE Y.ParamStringID IN (" + sqlEscapeChar(stringIDString) + ") " + \
                      "AND NOT EXISTS(" + \
                        "SELECT * FROM ParamStringID AS Z " + \
                        "WHERE Z.AppConfigID = X.AppConfigID AND Z.ParamStringID = Y.ParamStringID" + \
                      ")" + \
                  ")"

        c = db.cursor()
        c.execute(command)
        stringResult = c.fetchall()
        c.close()

    # Now we have 4 lists of potential IDs
    # (1) Merge them so that we have a list of only ids common to all 4
    # (2) Find the ID that only have the same matching count for each param list as we have parameters
    #     (since we know that they have the parameter ids we are looking for, a match in count means they have
    #      those ids and only those ids)
    mergeList = list(set(intResult) & set(floatResult))
    mergeList = list(set(mergeList) & set(boolResult))
    mergeList = list(set(mergeList) & set(stringResult))

    if(len(mergeList) == 0):
        return None

    # If there exist one or more AppConfigID's that match in mergeList, we need ensure that it matches the list exclusively,
    # i.e. there are no additional parameters (this would be a different specific configuration)

    intCount = len(intIDList)
    floatCount = len(floatIDList)
    boolCount = len(boolIDList)
    stringCount = len(stringIDList)

    for id in mergeList:

        command = "SELECT COUNT(*) FROM AppConfigInteger WHERE AppConfigID = " + sqlEscapeChar(id) + ";"
        c = db.cursor()
        c.execute(command)
        result = c.fetchone()
        c.close()

        # If the counts do not match, this is not a valid ID
        if(result != intCount):
            continue

        command = "SELECT COUNT(*) FROM AppConfigFloat WHERE AppConfigID = " + sqlEscapeChar(id) + ";"
        c = db.cursor()
        c.execute(command)
        result = c.fetchone()
        c.close()

        # If the counts do not match, this is not a valid ID
        if(result != floatCount):
            continue

        command = "SELECT COUNT(*) FROM AppConfigBoolean WHERE AppConfigID = " + sqlEscapeChar(id) + ";"
        c = db.cursor()
        c.execute(command)
        result = c.fetchone()
        c.close()

        # If the counts do not match, this is not a valid ID
        if(result != boolCount):
            continue

        command = "SELECT COUNT(*) FROM AppConfigSting WHERE AppConfigID = " + sqlEscapeChar(id) + ";"
        c = db.cursor()
        c.execute(command)
        result = c.fetchone()
        c.close()

        # If the counts do not match, this is not a valid ID
        if(result != stringCount):
            continue

        # If we made it this far, then we have a valid ID!
        return id

    # If we make it to this point, then we had no valid ids
    return None

def addProblemConfiguration(db, applicationID, intParamDict, floatParamDict, boolParamDict, stringParamDict):
    # A Problem Configuration is new if a matching id cannot be found that has both
    # (a) all of the provided parameters, and
    # (b) no additional parameters

    # If it is new, create a new id and update all relevant tables, else return the old id

    pass

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaParameterFloat(db):

    # Note that RunID is not unique here, the same run can have multiple parameters
    command = "CREATE TABLE IF NOT EXISTS ParameterFloat(ParamFloatID INTEGER, " + \
                                                      "ParamName TEXT, " + \
                                                      "ParamValue REAL, " + \
                                                      "PRIMARY KEY(ParamFloatID)" + \
                                                      ")"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findParameterFloatID(db, paramName, paramValue):

    command = "SELECT ParamFloatID FROM ParameterFloat WHERE " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + " AND " + \
              "ParamValue = " + sqlEscapeChar(paramValue) + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ParamFloatID']

#########################################################################################################

def addParameterFloatData(db, paramName, paramValue):

    appParamID = findParameterFloatID(db, paramName, paramValue)

    if(appParamID is None):
        command = "INSERT INTO ParameterFloat VALUES(" + \
                                                "NULL, " + \
												sqlEscapeChar(paramName) + "," + \
												sqlEscapeChar(paramValue) + \
												")"

        c = db.cursor()
        c.execute(command)
        appParamID = c.lastrowid
        c.close()

    return appParamID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaParameterInt(db):

    # Note that RunID is not unique here, the same run can have multiple parameters
    command = "CREATE TABLE IF NOT EXISTS ParameterInt(ParamIntID INTEGER, " + \
                                                      "ParamName TEXT, " + \
                                                      "ParamValue INTEGER, " + \
                                                      "PRIMARY KEY(ParamIntID)" + \
                                                      ")"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findParameterIntID(db, paramName, paramValue):

    command = "SELECT ParamIntID FROM ParameterInt WHERE " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + " AND " + \
              "ParamValue = " + sqlEscapeChar(paramValue) + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ParamIntID']

#########################################################################################################

def addParameterIntData(db, paramName, paramValue):

    paramIntID = findParameterIntID(db, paramName, paramValue)

    if(paramIntID is None):
        command = "INSERT INTO ParameterInt VALUES(" + \
                                                "NULL, " + \
												sqlEscapeChar(paramName) + "," + \
												sqlEscapeChar(paramValue) + \
												")"

        c = db.cursor()
        c.execute(command)
        paramIntID = c.lastrowid
        c.close()

    return paramIntID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaParameterBool(db):

    command = "CREATE TABLE IF NOT EXISTS ParameterBool(ParamBoolID INTEGER, " + \
                                                      "ParamName TEXT, " + \
                                                      "ParamValue INTEGER, " + \
                                                      "PRIMARY KEY(ParamBoolID), " + \
                                                      "CHECK (ParamValue IN (0, 1))" + \
                                                      ")"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findParameterBoolID(db, paramName, paramValue):

    command = "SELECT ParamBoolID FROM ParameterBool WHERE " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + " AND " + \
              "ParamValue = " + sqlEscapeChar(paramValue) + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ParamBoolID']

#########################################################################################################

def addParameterBoolData(db, paramName, paramValue):

    paramBoolID = findParameterBoolID(db, paramName, paramValue)

    if(paramBoolID is None):
        command = "INSERT INTO ParameterBool VALUES(" + \
                                                "NULL, " + \
												sqlEscapeChar(paramName) + "," + \
												sqlEscapeChar(paramValue) + \
												")"

        c = db.cursor()
        c.execute(command)
        paramBoolID = c.lastrowid
        c.close()

    return paramBoolID


#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaParameterString(db):

    # Note that RunID is not unique here, the same run can have multiple parameters
    command = "CREATE TABLE IF NOT EXISTS ParameterString(ParamStringID INTEGER, " + \
                                                      "ParamName TEXT, " + \
                                                      "ParamValue TEXT, " + \
                                                      "PRIMARY KEY(ParamStringID)" + \
                                                      ")"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################

def findParameterStringID(db, paramName, paramValue):

    command = "SELECT ParamStringID FROM ParameterString WHERE " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + " AND " + \
              "ParamValue = " + sqlEscapeChar(paramValue) + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['ParamStringID']

#########################################################################################################

def addParameterStringData(db, paramName, paramValue):

    paramStringID = findParameterStringID(db, paramName, paramValue)

    if(paramStringID is None):
        command = "INSERT INTO ParameterString VALUES(" + \
                                                "NULL, " + \
												sqlEscapeChar(paramName) + "," + \
												sqlEscapeChar(paramValue) + \
												")"

        c = db.cursor()
        c.execute(command)
        paramStringID = c.lastrowid
        c.close()

    return paramStringID


#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaProfileNodeData(db):

    command = "CREATE TABLE IF NOT EXISTS ProfileNodeData(ProfileNodeID INTEGER, " + \
                                                         "NodeName TEXT, " + \
                                                         "NodeType INTEGER, " + \
                                                         "PRIMARY KEY(ProfileNodeID));"

    c = db.cursor()
    c.execute(command)
    c.close()

def findProfileNodeID(nodeName, nodeType):
    pass

def addProfileNodeData(db, nodeName, nodeType):
    pass

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaCallPathData(db):

    command = "CREATE TABLE IF NOT EXISTS ProfileNodeCallPathData(CallPathID INTEGER, " + \
                                                                 "ProfileNodeID INTEGER, " + \
                                                                 "ParentNodeID INTEGER, " + \
                                                                 "PRIMARY KEY(CallPathID));"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaAggregateTimeData(db):

    command = "CREATE TABLE IF NOT EXISTS AggregateTime(AggTimeID INTEGER, " + \
                                                       "RunID INTEGER, " + \
                                                       "CallPathID INTEGER, " + \
                                                       "ProcessID INTEGER, " + \
                                                       "MinWallTime REAL, " + \
                                                       "AvgWallTime REAL, " + \
                                                       "MaxWallTime REAL, " + \
                                                       "StdDev REAL, " + \
                                                       "Count INTEGER, " + \
                                                       "PRIMARY KEY(AggTimeID), " + \
                                                       "UNIQUE(RunID, CallPathID, ProcessID));"

    c = db.cursor()
    c.execute(command)
    c.close()

def findAggregateTimeID(db, runID, callPathID, processID):
    pass


def addAggregateTimeID(db, runID, callPathID, processID, minWallTime, avgWallTime, maxWallTime, stdDev, count):
    pass

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaTraceTimeData(db):

    command = "CREATE TABLE IF NOT EXISTS TraceTime(TraceTimeID INTEGER, " + \
                                                   "RunID INTEGER, " + \
                                                   "CallPathID INTEGER, " + \
                                                   "ProcessID INTEGER, " + \
                                                   "NodeEntryCount INTEGER, " + \
                                                   "NodeExitCount INTEGER, " + \
                                                   "WallTime REAL, " + \
                                                   "PRIMARY KEY(TraceTimeID), " + \
                                                   "UNIQUE(RunID, CallPathID, ProcessID, NodeEntryCount, NodeExitCount));"

def findTraceTimeID(db):
    pass


def addTraceTimeID(db):
    pass

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaAggParamDataInt(db):

    command = "CREATE TABLE IF NOT EXISTS AggParamInt(" + \
              "AggParamIntID INTEGER, " + \
              "RunID INTEGER, " + \
              "CallPathID INTEGER, " + \
              "ProcessID INTEGER, " + \
              "ParamName TEXT, " + \
              "MinValue INTEGER, " + \
              "AvgValue REAL, " + \
              "MaxValue INTEGER, " + \
              "StdDev REAL, " + \
              "Count INTEGER, " + \
              "PRIMARY KEY(AggParamIntID), " + \
              "UNIQUE(RunID, CallPathID, ProcessID, ParamName));"

    c = db.cursor()
    c.execute(command)
    c.close()

def findAggParamIntID(db, runID, callPathID, processID, paramName):

    command = "SELECT AggParamIntID FROM AggParamInt WHERE " + \
              "RunID = " + sqlEscapeChar(runID) + " AND " + \
              "CallPathID = " + sqlEscapeChar(callPathID) + " AND " + \
              "ProcessID = " + sqlEscapeChar(processID) + " AND " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['AggParamIntID']

def addAggParamDataIntID(db, runID, callPathID, processID, paramName, minValue, avgValue, maxValue,
                         stdDev, count):

    aggParamIntID = findAggParamIntID(db, runID, callPathID, processID, paramName)

    if(aggParamIntID is None):
        command = "INSERT INTO AggParamInt VALUES(" + \
                  "NULL, " + \
                  sqlEscapeChar(runID) + "," + \
                  sqlEscapeChar(callPathID) + "," + \
                  sqlEscapeChar(processID) + "," + \
                  "'" + sqlEscapeChar(paramName) + "'" + \
                  ");"

        c = db.cursor()
        c.execute(command)
        aggParamIntID = c.lastrowid
        c.close()

    return aggParamIntID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaAggParamDataFloat(db):

    command = "CREATE TABLE IF NOT EXISTS AggParamFloat(" + \
              "AggParamFloatID INTEGER, " + \
              "RunID INTEGER, " + \
              "CallPathID INTEGER, " + \
              "ProcessID INTEGER, " + \
              "ParamName TEXT, " + \
              "MinValue REAL, " + \
              "AvgValue REAL, " + \
              "MaxValue REAL, " + \
              "StdDev REAL, " + \
              "Count INTEGER, " + \
              "PRIMARY KEY(AggParamFloatID), " + \
              "UNIQUE(RunID, CallPathID, ProcessID, ParamName));"

    c = db.cursor()
    c.execute(command)
    c.close()

def findAggParamFloatID(db, runID, callPathID, processID, paramName):

    command = "SELECT AggParamFloatID FROM AggParamFloat WHERE " + \
              "RunID = " + sqlEscapeChar(runID) + " AND " + \
              "CallPathID = " + sqlEscapeChar(callPathID) + " AND " + \
              "ProcessID = " + sqlEscapeChar(processID) + " AND " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['AggParamFloatID']

def addAggParamDataFloatID(db, runID, callPathID, processID, paramName, minValue, avgValue, maxValue,
                         stdDev, count):

    aggParamFloatID = findAggParamFloatID(db, runID, callPathID, processID, paramName)

    if(aggParamFloatID is None):
        command = "INSERT INTO AggParamFloat VALUES(" + \
                  "NULL, " + \
                  sqlEscapeChar(runID) + "," + \
                  sqlEscapeChar(callPathID) + "," + \
                  sqlEscapeChar(processID) + "," + \
                  "'" + sqlEscapeChar(paramName) + "'" + \
                  ");"

        c = db.cursor()
        c.execute(command)
        aggParamFloatID = c.lastrowid
        c.close()

    return aggParamFloatID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaAggParamDataBool(db):

    command = "CREATE TABLE IF NOT EXISTS AggParamBool(" + \
              "AggParamBoolID INTEGER, " + \
              "RunID INTEGER, " + \
              "CallPathID INTEGER, " + \
              "ProcessID INTEGER, " + \
              "ParamName TEXT, " + \
              "MinValue INTEGER, " + \
              "AvgValue REAL, " + \
              "MaxValue INTEGER, " + \
              "StdDev REAL, " + \
              "Count INTEGER, " + \
              "PRIMARY KEY(AggParamBoolID), " + \
              "UNIQUE(RunID, CallPathID, ProcessID, ParamName));"

    c = db.cursor()
    c.execute(command)
    c.close()

def findAggParamBoolID(db, runID, callPathID, processID, paramName):

    command = "SELECT AggParamBoolID FROM AggParamBool WHERE " + \
              "RunID = " + sqlEscapeChar(runID) + " AND " + \
              "CallPathID = " + sqlEscapeChar(callPathID) + " AND " + \
              "ProcessID = " + sqlEscapeChar(processID) + " AND " + \
              "ParamName LIKE '" + sqlEscapeChar(paramName) + "'" + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['AggParamBoolID']

def addAggParamDataBoolID(db, runID, callPathID, processID, paramName, minValue, avgValue, maxValue,
                         stdDev, count):

    aggParamBoolID = findAggParamBoolID(db, runID, callPathID, processID, paramName)

    if(aggParamBoolID is None):
        command = "INSERT INTO AggParamBool VALUES(" + \
                  "NULL, " + \
                  sqlEscapeChar(runID) + "," + \
                  sqlEscapeChar(callPathID) + "," + \
                  sqlEscapeChar(processID) + "," + \
                  "'" + sqlEscapeChar(paramName) + "'" + \
                  ");"

        c = db.cursor()
        c.execute(command)
        aggParamBoolID = c.lastrowid
        c.close()

    return aggParamBoolID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaTraceParameterDataInt(db):

    command = "CREATE TABLE IF NOT EXISTS TraceParamInt(" + \
              "TraceParamIntID INTEGER, " + \
              "RunID INTEGER, " + \
              "ProcessID INTEGER, " + \
              "CallPathID INTEGER, " + \
              "NodeEntryCount INTEGER, " + \
              "NodeExitCount INTEGER, " + \
              "ParamIntID INTEGER, " + \
              "PRIMARY KEY(TraceParamIntID), " + \
              "UNIQUE(RunID, ProcessID, CallPathID, NodeEntryCount, NodeExitCount), " + \
              "FOREIGN KEY(ParamIntID) REFERENCES ParameterInt(ParamIntID)" + \
              ");"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaTraceParameterDataFloat(db):

    command = "CREATE TABLE IF NOT EXISTS TraceParamFloat(" + \
              "TraceParamFloatID INTEGER, " + \
              "RunID INTEGER, " + \
              "ProcessID INTEGER, " + \
              "CallPathID INTEGER, " + \
              "NodeEntryCount INTEGER, " + \
              "NodeExitCount INTEGER, " + \
              "ParamFloatID INTEGER, " + \
              "PRIMARY KEY(TraceParamFloatID), " + \
              "UNIQUE(RunID, ProcessID, CallPathID, NodeEntryCount, NodeExitCount), " + \
              "FOREIGN KEY(ParamFloatID) REFERENCES ParameterFloat(ParamFloatID)" + \
              ");"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaTraceParameterDataBool(db):

    command = "CREATE TABLE IF NOT EXISTS TraceParamBool(" + \
              "TraceParamBoolID INTEGER, " + \
              "RunID INTEGER, " + \
              "ProcessID INTEGER, " + \
              "CallPathID INTEGER, " + \
              "NodeEntryCount INTEGER, " + \
              "NodeExitCount INTEGER, " + \
              "ParamBoolID INTEGER, " + \
              "PRIMARY KEY(TraceParamBoolID), " + \
              "UNIQUE(RunID, ProcessID, CallPathID, NodeEntryCount, NodeExitCount), " + \
              "FOREIGN KEY(ParamBoolID) REFERENCES ParameterBool(ParamBoolID)" + \
              ");"

    c = db.cursor()
    c.execute(command)
    c.close()

#########################################################################################################
#########################################################################################################
#########################################################################################################

def writeSchemaLibraryConfig(db):

    command = "CREATE TABLE IF NOT EXISTS InstrumLibConfig(" + \
              "LibraryConfigID INTEGER, " + \
              "VersionMajor INTEGER, " + \
              "VersionMinor INTEGER, " + \
              "AggTimers INTEGER, " + \
              "TraceTimers INTEGER, " + \
              "AggParam INTEGER, " + \
              "TraceParam INTEGER, " + \
              "AggPAPI INTEGER, " + \
              "TracePAPI INTEGER, " + \
              "MpiHooks INTEGER, " + \
              "PRIMARY KEY(LibraryConfigID)" + \
              ");"

    c = db.cursor()
    c.execute(command)
    c.close()

def findLibraryConfigID(db, versionMajor, versionMinor, aggTimers, traceTimers, aggParam, traceParam,
                        aggPAPI, tracePAPI, mpiHooks):

    command = "SELECT LibraryConfigID FROM InstrumLibConfig WHERE " + \
              "VersionMajor = " + sqlEscapeChar(versionMajor) + " AND " + \
              "VersionMinor = " + sqlEscapeChar(versionMinor) + " AND " + \
              "AggTimers = " + sqlEscapeChar(aggTimers) + " AND " + \
              "TraceTimers = " + sqlEscapeChar(traceTimers) + " AND " + \
              "AggParam = " + sqlEscapeChar(aggParam) + " AND " + \
              "TraceParam = " + sqlEscapeChar(traceParam) + " AND " + \
              "AggPAPI = " + sqlEscapeChar(aggPAPI) + " AND " + \
              "TracePAPI = " + sqlEscapeChar(tracePAPI) + " AND " + \
              "MpiHooks = " + sqlEscapeChar(mpiHooks) + \
              ";"

    c = db.cursor()
    c.execute(command)
    result = c.fetchone()
    c.close()

    if(result is None):
        return None
    else:
        return result['LibraryConfigID']

def addLibraryConfigID(db, versionMajor, versionMinor, aggTimers, traceTimers, aggParam, traceParam,
                       aggPAPI, tracePAPI, mpiHooks):

    libConfigID = findLibraryConfigID(db, versionMajor, versionMinor, aggTimers, traceTimers, aggParam,
                                      traceParam, aggPAPI, tracePAPI, mpiHooks)

    if(libConfigID is None):
        command = ""

        c = db.cursor()
        c.execute(command)
        libConfigID = c.lastrowid
        c.close()

    return libConfigID

#########################################################################################################
#########################################################################################################
#########################################################################################################

def sqlEscapeChar(var):
	return str(var)

#########################################################################################################
#########################################################################################################
#########################################################################################################


if __name__ == "__main__":
	testDatabase("./test.db")
