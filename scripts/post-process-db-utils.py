import sqlite3

def get_table_count(db, table):
	cur = db.cursor()
	query = "SELECT COUNT(*) FROM {0} ;".format(table)
	cur.execute(query)
	count = cur.fetchone()[0]
	return count

def table_empty(db, table):
	cur = db.cursor()
	query = "SELECT COUNT(*) FROM (SELECT * FROM {0} LIMIT 1);".format(table)
	cur.execute(query)
	count = cur.fetchone()[0]
	return count == 0

def table_exists(db, table):
	cur = db.cursor()
	query = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='{0}'".format(table)
	cur.execute(query)
	return (cur.fetchone()[0] > 0)

def temp_table_exists(db, table):
	cur = db.cursor()
	query = "SELECT count(*) FROM sqlite_temp_master WHERE type='table' AND name='{0}'".format(table)
	cur.execute(query)
	return (cur.fetchone()[0] > 0)

def getProcessCallpathIds(db, runID, processID):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	query = "SELECT DISTINCT CallPathID FROM AggregateTime NATURAL JOIN CallPathData WHERE RunID = {0} AND ProcessID = {1};".format(runID, processID)
	cur.execute(query)
	result = cur.fetchall()
	return [row['CallPathID'] for row in result]

def getNodeCallpathId(db, processID, nodeName):
	## Note: runID not needed because assuming call tree is same across repeat runs.
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT CallPathID FROM CallPathData NATURAL JOIN ProfileNodeData WHERE ProcessID = {0} AND NodeName = '{1}'".format(processID, nodeName)
	cur.execute(cmd)
	ids = [r["CallPathID"] for r in cur.fetchall()]
	if len(ids) > 1:
		raise Exception("getNodeCallpathId(processID={0}, nodeName={1}) has returned {2} IDs: {3}".format(processID, nodeName, len(ids), ids))
	return ids[0]

def getNodeChildrenIDs(db, processID, callPathID):
	## Note: runID not needed because assuming call tree is same across repeat runs.
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	callpathProcess_tmpTableName = "callpath_p{0}".format(processID)
	if not temp_table_exists(db, callpathProcess_tmpTableName):
		query = "CREATE TEMPORARY TABLE {0} AS SELECT * FROM CallPathData AS C WHERE C.ProcessID = {1}".format(callpathProcess_tmpTableName, processID)
		cur.execute(query)
	cmd = "SELECT CallPathID FROM {0} WHERE ParentNodeID = {1}".format(callpathProcess_tmpTableName, callPathID)

	cur.execute(cmd)
	return [i['CallPathID'] for i in cur.fetchall()]

def getProcessIds(db):
	if isinstance(db, str):
		db_fp = db
		db = sqlite3.connect(db_fp)
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cur.execute("SELECT ProcessID FROM ProcessData")
	return [r["ProcessID"] for r in cur.fetchall()]

def getMpiRanks(db):
	if isinstance(db, str):
		db_fp = db
		db = sqlite3.connect(db_fp)
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cur.execute("SELECT MPIRank FROM ProcessData")
	return [r["MPIRank"] for r in cur.fetchall()]

def getMpiRank(db, processID):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cur.execute("SELECT MPIRank FROM ProcessData WHERE ProcessID = {0}".format(processID))
	results = cur.fetchall()
	if len(results) > 1:
		raise Exception("getMpiRank(processID={0}) has returned {1} rows".format(processID, len(results)))
	return results[0]["MPIRank"]

def getProcessID(db, MPIRank):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cur.execute("SELECT ProcessID FROM ProcessData WHERE MPIRank = {0}".format(MPIRank))
	results = cur.fetchall()
	if len(results) > 1:
		raise Exception("getMpiRank(MPIRank={0}) has returned {1} rows".format(MPIRank, len(results)))
	return results[0]["ProcessID"]

