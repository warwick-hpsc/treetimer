# Processes an SQL file output by the TreeTimer Library and generates an indented callpath tree
# alongside the inclusive walltimes, callcounts

# Process of interest
# Exclusive % of total walltime, grouped by type.
#	(a) Useful for breakdown to spot comms/compute impact
#   (b) Useful for validation - all functions should be reducible to some type - if there is an excessive amount of exclusive time
#		in function times, something substantial has not been captured.
# Exclusive % of total walltime, grouped by node name (i.e. code location) - this strips callpath data. Sorted from high to low.
# Exclusive CallPath Summary Data
# Inclusive CallPath Summary Data




import sys
import sqlite3

def main():
	db = None
	indentLevel = 0

	# Open up the SQL Database
	# This will contains run timings and other details for a single process + run
	#openDatabase(sys.argv[1], db)
	db = sqlite3.connect(sys.argv[1])

	# Starting the toplevel node (i.e. the root node), print out the inclusive data alongside that node
	# root should always be nodeID 0

	printNodeDetailsDepthTraversal(0, db, 0)
#	printSummary_ExclusiveTimeByType(db)
#	printSummary_ExclusiveTimeByNodeName(db)

	# Close the Database file
	db.close()

def printSummary_ExclusiveTimeByType(db):

	# Get all node IDs
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	cmd = "SELECT DISTINCT ProfileNodeID FROM ProfileNodes;" 
	cur.execute(cmd)

	result = cur.fetchall()

	# For each node, get the aggreagte details
	nodeProp = []
	for row in result:
		nodeProp.append(getAggregateNodeDetails(row['ProfileNodeID'], 0, 67, db))

	# Sort the list by the type field of each record
	sortedRecords = sorted(nodeProp, key=lambda k: k['Type'])

	# Get Time for Root Node
	rootProp = getAggregateNodeDetails(0, 0, 67, db)


	print "=============================================="
	print "Summary: Exclusive Time in code blocks by type"
	print "Root Walltime: " + str(rootProp['AggTotalTimeI'])
	print "=============================================="
	print ""

	# Sum the exclusive times of each type
	group = sortedRecords[0]['Type']
	groupTime = 0.0

	for record in sortedRecords:
		if(record['Type'] == group):
			groupTime = groupTime + record['AggTotalTimeE']
		else:
			print str(getNodeType(group)) + ":" + str(groupTime)
			group = record['Type']
			groupTime = record['AggTotalTimeE']

	# Print Last Group
	print str(getNodeType(group)) + ":" + str(groupTime)

def printSummary_ExclusiveTimeByNodeName(db):
	# Get all node IDs
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	cmd = "SELECT DISTINCT ProfileNodeID FROM ProfileNodes;" 
	cur.execute(cmd)

	result = cur.fetchall()

	# For each node, get the aggregate details
	nodeProp = []
	for row in result:
		nodeProp.append(getAggregateNodeDetails(row['ProfileNodeID'], 0, 67, db))

	# Sort the list by the NodeName of each record
	sortedRecords = sorted(nodeProp, key=lambda k: k['Name'])

	# Get Time for Root Node
	rootProp = getAggregateNodeDetails(0, 0, 67, db)

	# Sum the exclusive times of each type
	blocks = []

	sum = 0.0
	group = sortedRecords[0]['Name']
	groupTime = 0.0

	for record in sortedRecords:
		if(record['Name'] == group):
			groupTime = groupTime + record['AggTotalTimeE']
		else:
			tmp = {'Name': group, 'TimeE': groupTime}
			sum = sum + groupTime
			blocks.append(tmp)

			group = record['Name']
			groupTime = record['AggTotalTimeE']

	# Store last group
	tmp = {'Name': group, 'TimeE': groupTime}
	blocks.append(tmp)

	# List comprises of nodes grouped by program location/block
	# Sort by time
	sortedRecords = sorted(blocks, key=lambda k: k['TimeE'], reverse=True)

	# Print

	print "==============================================="
	print "Summary: Exclusive Time in code blocks by site"
	print "Root Walltime: " + str(rootProp['AggTotalTimeI'])
	print "Total Exclusive Time Sum:" + str(sum)
	print "==============================================="
	print ""

	for i in sortedRecords:
		print str(i['Name']) + ", " + str(i['TimeE'])


def printNodeDetailsDepthTraversal(nodeID, db, indentLevel):
	# Recursive method to do depth first traversal when printing nodes

	for i in xrange(0, indentLevel):
		sys.stdout.write('  ')

	printAggregateNodeDetails(nodeID, db)
	childNodes = getChildNodeIDs(nodeID, db)

	# Loop over child nodes
	for childID in childNodes:
		# Keep following a path till we reach a node with no children
		printNodeDetailsDepthTraversal(childID, db, indentLevel + 1)

def getChildNodeIDs(nodeID, db):
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	cmd = "SELECT ProfileNodeID FROM ProfileNodes " + \
		  "WHERE ParentNodeID = " + str(nodeID)
	cur.execute(cmd)

	result = cur.fetchall()

	list = []
	for i in result:
		list.append(i['ProfileNodeID'])

	return list

def getAggregateNodeDetails(nodeID, runID, processID, db):
	# This method retrieves the following aggregate details about a node and returns them as a dictionary
	# (1) Node Name: Key 'Name'
	# (2) Node Type: Key 'Type'
	# (3) Min Time: Key 'AggMinTime'
	# (4) Avg Time: Key 'AggAvgTime'
	# (5) Max Time: Key 'AggMaxTime'
	# (6) Count: Key 'CallCount'
	# (7) Total Time Inclusive (including child node times): Key 'AggTotalTimeI'
	# (8) Total Time Exclusive (time in this node only, excluding child node times): Key 'AggTotalTimeE'
	# (9)

	# ToDo: If looping over nodes, this will lead to the same data being loaded from the database multiple
	# times, which is likely expensive.
	# Options: Load full data from database into tree in memory (might be v. large)
	#		   Ensure only depth-first traversal that can reuse data from prior loads

	db.row_factory = sqlite3.Row
	cur = db.cursor()

	# Should only ever have one entry
	cmd = "SELECT B.NodeName AS NodeName, B.NodeType AS NodeType, A.RunID AS RunID, A.ProcessID AS ProcessID, A.MinTime AS MinTime, A.AvgTime AS AvgTime, " + \
		  "A.MaxTime AS MaxTime, A.Count AS Count FROM AggregateTimes AS A " + \
		  "NATURAL JOIN ProfileNodes AS B " + \
		  "WHERE ProfileNodeID = " + str(nodeID) + " " + \
		  "AND RunID = " + str(runID) + " " + \
		  "AND ProcessID = " + str(processID) + \
		  ";"

	cur.execute(cmd)
	result = cur.fetchone()

	rDict = {}
	rDict['Name'] = result['NodeName']
	rDict['Type'] = result['NodeType']
	rDict['AggMinTime'] = result['MinTime']
	rDict['AggAvgTime'] = result['AvgTime']
	rDict['AggMaxTime'] = result['MaxTime']
	rDict['CallCount'] = result['Count']
	rDict['AggTotalTimeI'] = rDict['AggAvgTime'] * rDict['CallCount']

	# === Get times for direct children of this node ===
	# Get their IDs
	childIDs = getChildNodeIDs(nodeID, db)

	# Build a string for SQL
	if(len(childIDs) == 0):
		rDict['AggTotalTimeE'] = rDict['AggTotalTimeI']
	else:
		childIDStr = str(childIDs[0])

		if(len(childIDs) > 1):
			for id in childIDs:
				childIDStr = childIDStr + "," + str(id)

		# Get sum of their times
		cmd = "SELECT SUM(AvgTime*Count) AS ChildTimeSum FROM AggregateTimes " + \
    	      "WHERE RunID = " + str(runID) + " " + \
        	  "AND ProcessID = " + str(processID) + " " + \
			  "AND ProfileNodeID IN (" + str(childIDStr) + ")" + \
			  ";"

		cur.execute(cmd)
		result = cur.fetchone()

		rDict['AggTotalTimeE'] = rDict['AggTotalTimeI'] - result['ChildTimeSum']
		# Due to overhead, potentially possible for this to be negative, so will correct
		if(rDict['AggTotalTimeE'] < 0.0):
			rDict['AggTotalTimeE'] = 0.0

	return rDict

def printAggregateNodeDetails(nodeID, db):
	row = getAggregateNodeDetails(nodeID, 0, 67, db)

	print str(row['Name']) + ", " + \
		  str(getNodeType(row['Type'])) + ", " + \
		  str(row['AggMinTime']) + ", " + \
		  str(row['AggAvgTime']) + ", " + \
		  str(row['AggMaxTime']) + ", " + \
		  str(row['CallCount']) + ", " + \
		  str(row['AggTotalTimeI']) + ", " + \
		  str(row['AggTotalTimeE'])

def getNodeType(nodeType):

	if(nodeType == 0):
		return "Program_Root"
	elif(nodeType == 1):
		return "Method"
	elif(nodeType == 2):
		return "Loop"
	elif(nodeType == 3):
		return "MPI Function Call"
	elif(nodeType == 4):
		return "NonMPI Function Call"
	elif(nodeType == 5):
		return "Unspecified"
	elif(nodeType == 6):
		return ""
	elif(nodeType == 7):
		return ""
	elif(nodeType == 8):
		return "MPI_Comm"
	elif(nodeType == 9):
		return "MPI_NonComm"
	elif(nodeType == 10):
		return "MPI_IO"
	else:
		return ""

if __name__ == "__main__":
    main()
