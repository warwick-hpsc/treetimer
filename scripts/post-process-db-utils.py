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
