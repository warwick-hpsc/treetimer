import sqlite3

def get_table_count(db, table):
	cur = db.cursor()
	query = "SELECT COUNT(*) FROM {0} ;".format(table)
	cur.execute(query)
	count = cur.fetchone()[0]
	return count
