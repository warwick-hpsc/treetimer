import sqlite3

def get_table_count(db, table):
	cur = db.cursor()
	query = "SELECT COUNT(*) FROM {0} ;".format(table)
	cur.execute(query)
	count = cur.fetchone()[0]
	return count

def table_empty(db, table):
	# return get_table_count(db, table) == 0
	cur = db.cursor()
	query = "SELECT COUNT(*) FROM (SELECT * FROM {0} LIMIT 1);".format(table)
	cur.execute(query)
	count = cur.fetchone()[0]
	return count == 0

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

