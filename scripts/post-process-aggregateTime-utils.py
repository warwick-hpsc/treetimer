import os
import sqlite3

import pandas as pd

import imp
imp.load_source("PostProcessDbUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-db-utils.py"))
from PostProcessDbUtils import *

def getNodeAggregatedTimeStats(db, runID, processID, callPathID, window=None):
	## 1 - handle windowed aggregation
	## 2 - calculate exclusive time of node (inclusive - children)

	db.row_factory = sqlite3.Row
	cur = db.cursor()

	## Get Node Aggregate Details - Should only ever have one entry
	## If target code enabled windowed aggregation, then for a given CallPathID there may be multiple rows:
	## - exception is nodes above 'AggregationStepper' in tree hierarchy
	## - if window is specified, then expect one row
	## - else then re-calculate statistics across all rows
	if window is None:
		query = "SELECT D.NodeName AS Name, " + \
				"T.TypeName AS TypeName, " + \
				"A.MinWallTime AS AggMinTime, A.AvgWallTime AS AggAvgTime, A.MaxWallTime AS AggMaxTime, A.Count AS CallCount " + \
				"FROM AggregateTime AS A " + \
				"NATURAL JOIN CallPathData AS C NATURAL JOIN ProfileNodeData AS D NATURAL JOIN ProfileNodeType AS T " + \
				"WHERE A.RunID = {0} AND A.ProcessID = {1} AND A.CallPathID = {2} ;".format(runID, processID, callPathID)
		cur.execute(query)
		result = cur.fetchall()
		if result is None or len(result) == 0:
			raise Exception("No CallPath node meeting criteria: runID={0}, processID={1}, callPathID={2}".format(runID, processID, callPathID))
		if len(result) == 1:
			result = result[0]
		else:
			## columns:   0  ,      1    ,      2      ,      3      ,       4     ,      5
			##         "Name", "TypeName", "AggMinTime", "AggAvgTime", "AggMaxTime", "CallCount"]
			sumTime = 0.0
			minTime = result[0][2]
			maxTime = result[0][4]
			callCountSum = 0
			for r in result:
				callCountSum += r[5]
				sumTime += (r[3]*r[5])
				minTime = min(minTime, r[2])
				maxTime = max(maxTime, r[4])
			avgTime = sumTime / float(callCountSum)
			result = {"Name":r[0], "TypeName":r[1], "AggMinTime":minTime, "AggAvgTime":avgTime, "AggMaxTime":maxTime, "CallCount":callCountSum}
	else:
		query = "SELECT D.NodeName AS Name, " + \
				"T.TypeName AS TypeName, " + \
				"A.Window AS Window, A.MinWallTime AS AggMinTime, A.AvgWallTime AS AggAvgTime, A.MaxWallTime AS AggMaxTime, A.Count AS CallCount " + \
				"FROM AggregateTime AS A " + \
				"NATURAL JOIN CallPathData AS C NATURAL JOIN ProfileNodeData AS D NATURAL JOIN ProfileNodeType AS T " + \
				"WHERE A.RunID = {0} AND A.ProcessID = {1} AND A.CallPathID = {2} AND A.Window = {3} ;".format(runID, processID, callPathID, window)
		cur.execute(query)
		result = cur.fetchall()
		if result is None:
			raise Exception("No CallPath node meeting criteria: processID={0}, callPathID={1}, window={2}".format(processID, callPathID, window))
		if len(result) > 1:
			raise Exception("Multiple CallPath nodes ({0}) meeting criteria: processID={1}, callPathID={2}, window={3}".format(len(result), processID, callPathID, window))
		result = result[0]
	nodeStats = {k:result[k] for k in result.keys()}
	nodeStats['AggTotalTimeI'] = nodeStats['AggAvgTime'] * nodeStats['CallCount']

	## Get Times for any direct children of this node
	childIDs = getNodeChildrenIDs(db, callPathID)
	if(len(childIDs) == 0):
		## No child nodes means exclusive time = inclusive time
		nodeStats['AggTotalTimeE'] = nodeStats['AggTotalTimeI']

	else:
		## Get sum of inclusive times taken by child nodes
		childIDStr = ','.join([str(x) for x in childIDs])
		query = "SELECT SUM(AvgWallTime * Count) AS ChildrenWalltime FROM AggregateTime" + \
			  " WHERE RunID = {0} AND ProcessID = {1} ".format(runID, processID) + \
			  " AND CallPathID IN ({0}) ;".format(childIDStr)
		cur.execute(query);
		childrenWalltime = cur.fetchone()['ChildrenWalltime']

		if nodeStats["CallCount"] == 0:
			nodeStats['AggTotalTimeE'] = 0.0
			nodeStats['AggTotalTimeI'] = childrenWalltime
		else:
			nodeStats['AggTotalTimeE'] = nodeStats['AggTotalTimeI'] - childrenWalltime
			#s# Due to overhead, potentially possible for this to be slightly negative, so will correct here
			if nodeStats['AggTotalTimeE'] < 0.0:
				print("Note: negative 'AggTotalTimeE' detected for nodeName '{0}', zeroing ({1})".format(nodeStats["Name"], nodeStats['AggTotalTimeE']))
				nodeStats['AggTotalTimeE'] = 0.0

	return nodeStats

def aggregateTimesByType(runID, db):
	if table_empty(db, "AggregateTime"):
		return None

	processIDs = getProcessIds(db)
	df_sum_all = None
	for processID in processIDs:
		callpathIDs = getProcessCallpathIds(db, runID, processID)

		# For each callpath node, get the aggregate details
		col_names = ["Name", "TypeName", "CallCount", "AggTotalTimeI", "AggTotalTimeE", "AggAvgTime", "AggMaxTime", "AggMinTime"]
		df = None
		df_cols = {c:[] for c in col_names}
		for callpathID in callpathIDs:
			r = getNodeAggregatedTimeStats(db, runID, processID, callpathID)
			for k in col_names:
				df_cols[k].append(r[k])
		df = pd.DataFrame(data=df_cols, columns=col_names)

		walltime = df[df['Name']=='ProgramRoot'].loc[0,'AggTotalTimeI']

		# Sum times across method types
		df_sum = df[['TypeName', 'AggTotalTimeE']].groupby('TypeName').sum().reset_index()
		df_sum = df_sum.rename(columns={'TypeName':'Method type', 'AggTotalTimeE':'Exclusive time'})
		df_sum['Exclusive time %'] = df_sum['Exclusive time'] / walltime
		df_sum = df_sum.sort_values("Exclusive time", ascending=False)

		df_sum["Rank"] = getMpiRank(db, processID)
		if df_sum_all is None:
			df_sum_all = df_sum
		else:
			df_sum_all = df_sum_all.append(df_sum)

	return(df_sum_all)

def getAllProfileNodesAggregateTimeExclusive(runID, processID, db):
	# Since aggregate properties operate on call path nodes (since
	# times for different paths to the same profile node are stored
	# separately), we need to merge these values.

	# First, identify the callpath nodes associated with a run & process
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT DISTINCT CallPathID FROM AggregateTime WHERE RunID = {0} AND ProcessID = {1}".format(runID, processID)
	cur.execute(cmd)
	callpathIDs = [row['CallPathID'] for row in cur.fetchall()]

	col_names = ["Name", "TypeName", "CallPathID", "CallCount", "AggTotalTimeI", "AggTotalTimeE", "AggAvgTime", "AggMaxTime", "AggMinTime"]
	df = None
	df_cols = {c:[] for c in col_names}
	for callpathID in callpathIDs:
		data = getNodeAggregatedTimeStats(db, runID, processID, callpathID)
		for k in data.keys():
			df_cols[k].append(data[k])
		df_cols['CallPathID'].append(callpathID)
	df = pd.DataFrame(data=df_cols, columns=col_names)
	df_grp = df.groupby(['Name', 'TypeName', 'CallPathID']).sum().reset_index()

	return df_grp

def getAllAggregatedTimeStats(runID, db):
	if table_empty(db, "AggregateTime"):
		return None

	processIDs = getProcessIds(db)
	df_all = None
	for processID in processIDs:
		rootID = getNodeCallpathId(db, processID, 'ProgramRoot')
		rootRecord = getNodeAggregatedTimeStats(db, runID, processID, rootID)

		df = getAllProfileNodesAggregateTimeExclusive(runID, processID, db)

		df['ExclusiveAggregateTime(% of Run)'] = df['AggTotalTimeE']/rootRecord['AggTotalTimeI']
		df = df.rename(columns={'AggTotalTimeI':'InclusiveAggregateTime(s)', 'AggTotalTimeE':'ExclusiveAggregateTime(s)'})
		df = df.drop(['AggAvgTime', 'AggMaxTime', 'AggMinTime', 'CallPathID'], axis=1)
		df = df.sort_values('ExclusiveAggregateTime(% of Run)', ascending=False)

		df["Rank"] = getMpiRank(db, processID)
		if df_all is None:
			df_all = df
		else:
			df_all = df_all.append(df)

	return df_all
