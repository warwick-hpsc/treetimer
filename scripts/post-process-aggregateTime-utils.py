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

	aggTimesProcess_tmpTableName = "aggTime_r{0}_p{1}".format(runID, processID)
	if not temp_table_exists(db, aggTimesProcess_tmpTableName):
		query = "CREATE TEMPORARY TABLE {0} AS SELECT * FROM AggregateTime WHERE RunID = {1} AND ProcessID = {2}".format(aggTimesProcess_tmpTableName, runID, processID)
		cur.execute(query)
	callpathProcess_tmpTableName = "callpath_p{0}".format(processID)
	if not temp_table_exists(db, callpathProcess_tmpTableName):
		query = "CREATE TEMPORARY TABLE {0} AS SELECT * FROM CallPathData WHERE ProcessID = {1}".format(callpathProcess_tmpTableName, processID)
		cur.execute(query)

	premergedData_tmpTableName = "aggTimeCallpath_r{0}_p{1}".format(runID, processID)
	if not temp_table_exists(db, premergedData_tmpTableName):
		query = "CREATE TEMPORARY TABLE {0} AS SELECT ".format(premergedData_tmpTableName) + \
				"A.CallPathID AS CallPathID, " + \
				"D.NodeName AS NodeName, " + \
				"T.TypeName AS TypeName, " + \
				"A.Window AS Window, A.MinWallTime AS MinWallTime, A.AvgWallTime AS AvgWallTime, A.MaxWallTime AS MaxWallTime, A.Count AS Count " + \
				"FROM {0} AS A NATURAL JOIN {1} AS C ".format(aggTimesProcess_tmpTableName, callpathProcess_tmpTableName) + \
				"NATURAL JOIN ProfileNodeData AS D NATURAL JOIN ProfileNodeType AS T ;"
		cur.execute(query)

	## Get Node Aggregate Details - Should only ever have one entry
	## If target code enabled windowed aggregation, then for a given CallPathID there may be multiple rows:
	## - exception is nodes above 'AggregationStepper' in tree hierarchy
	## - if window is specified, then expect one row
	## - else then re-calculate statistics across all rows
	if window is None:
		query = "SELECT " + \
				"A.CallPathID AS CallPathID, " + \
				"A.NodeName AS Name, " + \
				"A.TypeName AS TypeName, " + \
				"A.MinWallTime AS AggMinTime, A.AvgWallTime AS AggAvgTime, A.MaxWallTime AS AggMaxTime, A.Count AS CallCount " + \
				"FROM {0} AS A ".format(premergedData_tmpTableName) + \
				"WHERE A.CallPathID = {0} ;".format(callPathID)
		cur.execute(query)
		result = cur.fetchall()
		if result is None or len(result) == 0:
			#print(query)
			#raise Exception("No CallPath node meeting criteria: runID={0}, processID={1}, callPathID={2}".format(runID, processID, callPathID))
			return None
		if len(result) == 1:
			result = result[0]
		else:
			## columns:      0  ,      1    ,      2      ,      3      ,       4     ,      5   ,     6
			##         "CallPathID", "Name", "TypeName", "AggMinTime", "AggAvgTime", "AggMaxTime", "CallCount"]
			sumTime = 0.0
			minTime = result[0][3]
			maxTime = result[0][5]
			callCountSum = 0
			for r in result:
				callCountSum += r[6]
				sumTime += (r[4]*r[6])
				minTime = min(minTime, r[3])
				maxTime = max(maxTime, r[5])
			avgTime = sumTime / float(callCountSum)
			result = {"CallPathID":r[0], "Name":r[1], "TypeName":r[2], "AggMinTime":minTime, "AggAvgTime":avgTime, "AggMaxTime":maxTime, "CallCount":callCountSum}
	else:
		query = "SELECT " + \
				"A.CallPathID AS CallPathID, " + \
				"A.NodeName AS Name, " + \
				"A.TypeName AS TypeName, " + \
				"A.Window AS Window, A.MinWallTime AS AggMinTime, A.AvgWallTime AS AggAvgTime, A.MaxWallTime AS AggMaxTime, A.Count AS CallCount " + \
				"FROM {0} AS A ".format(premergedData_tmpTableName) + \
				"WHERE A.CallPathID = {0} AND A.Window = {1} ;".format(callPathID, window)
		cur.execute(query)
		result = cur.fetchall()
		if result is None or len(result) == 0:
			# raise Exception("No CallPath node meeting criteria: processID={0}, callPathID={1}, window={2}".format(processID, callPathID, window))
			## Update: Note that here a window was specified. Then it is possible that this callpath node 
			##         was not executed in this window.
			return None
		if len(result) > 1:
			raise Exception("Multiple CallPath nodes ({0}) meeting criteria: processID={1}, callPathID={2}, window={3}".format(len(result), processID, callPathID, window))
		result = result[0]
	nodeStats = {k:result[k] for k in result.keys()}
	nodeStats['AggTotalTimeI'] = nodeStats['AggAvgTime'] * nodeStats['CallCount']

	## Get Times for any direct children of this node
	childIDs = getNodeChildrenIDs(db, processID, callPathID)
	if(len(childIDs) == 0):
		## No child nodes means exclusive time = inclusive time
		nodeStats['AggTotalTimeE'] = nodeStats['AggTotalTimeI']

	else:
		## Get sum of inclusive times taken by child nodes
		childIDStr = ','.join([str(x) for x in childIDs])
		if window is None:
			## Note: safe to ignore 'window', because 'SUM()' will sum across windows as well as children.
			query = "SELECT SUM(AvgWallTime * Count) AS ChildrenWalltime FROM {0}".format(aggTimesProcess_tmpTableName) + \
				  " WHERE CallPathID IN ({0}) ;".format(childIDStr)
		else:
			query = "SELECT SUM(AvgWallTime * Count) AS ChildrenWalltime FROM {0}".format(aggTimesProcess_tmpTableName) + \
			  " WHERE Window = {0}".format(window) + \
			  " AND CallPathID IN ({0}) ;".format(childIDStr)
		cur.execute(query);
		childrenWalltime = cur.fetchone()['ChildrenWalltime']

		if nodeStats["CallCount"] == 0:
			nodeStats['AggTotalTimeE'] = 0.0
			nodeStats['AggTotalTimeI'] = childrenWalltime
		else:
			nodeStats['AggTotalTimeE'] = nodeStats['AggTotalTimeI'] - childrenWalltime
			## Due to overhead, potentially possible for this to be slightly negative, so will correct here
			if nodeStats['AggTotalTimeE'] < 0.0:
				print("Note: negative 'AggTotalTimeE' detected for nodeName '{0}', zeroing ({1})".format(nodeStats["Name"], nodeStats['AggTotalTimeE']))
				nodeStats['AggTotalTimeE'] = 0.0

	return nodeStats

def aggregateTimesByType(db, runID):
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
			if not r is None:
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

	df_sum_all["Type"] = ""
	df_sum_all.loc[df_sum_all["Method type"].isin(["MPICollectiveCall", "MPICommCall", "MPISyncCall"]), "Type"] = "MPI"
	df_sum_all.loc[df_sum_all["Method type"].isin(["Program", "Method", "Loop", "Compute", "Block"]), "Type"] = "Compute"
	df_sum_all.loc[df_sum_all["Method type"].isin(["TraceConductor", "AggregationStepper"]), "Type"] = "Compute"
	if sum(df_sum_all["Type"]=="") > 0:
		print(df_sum_all["Method type"].unique())
		raise Exception("Unhandled Method type values, investigate")
	df_sum_all = df_sum_all.drop(["Method type"], axis=1)		
	df_sum_all = df_sum_all.groupby(["Rank", "Type"]).sum().reset_index()

	return df_sum_all

def collateWindowedAggregateTimes(db, runID, processID, tree):
	aggStepperNode = tree.findTreeNodeByType("AggregationStepper")
	aggStepperNodeName = aggStepperNode.name
	aggStepperCallPathID = getNodeCallpathId(db, processID, aggStepperNodeName)
	windows = getNodeAggregationWindows(db, runID, processID, aggStepperCallPathID)
	allChildrenCallPathIDs = aggStepperNode.getAllChildrenMembers("dbID")

	rank = getMpiRank(db, processID)

	df_sum_all = None
	for w in windows:
		col_names = ["Window", "Name", "TypeName", "CallCount", "AggTotalTimeI", "AggTotalTimeE", "AggAvgTime", "AggMaxTime", "AggMinTime"]
		df = None
		df_cols = {c:[] for c in col_names}
		for callpathID in allChildrenCallPathIDs:
			r = getNodeAggregatedTimeStats(db, runID, processID, callpathID, window=w)
			if not r is None:
				for k in col_names:
					df_cols[k].append(r[k])
		df = pd.DataFrame(data=df_cols, columns=col_names)

		walltime = df[df['TypeName']=="AggregationStepper"].loc[0,'AggTotalTimeI']

		# Sum times across method types
		df_sum = df[['TypeName', 'AggTotalTimeE']].groupby('TypeName').sum().reset_index()
		df_sum = df_sum.rename(columns={'TypeName':'Method type', 'AggTotalTimeE':'Exclusive time'})
		df_sum['Exclusive time %'] = df_sum['Exclusive time'] / walltime
		df_sum = df_sum.sort_values("Exclusive time", ascending=False)

		df_sum["Window"] = w
		if df_sum_all is None:
			df_sum_all = df_sum
		else:
			df_sum_all = df_sum_all.append(df_sum)

	df_sum_all["Rank"] = rank

	df_sum_all["Type"] = ""
	df_sum_all.loc[df_sum_all["Method type"].isin(["MPICollectiveCall", "MPICommCall", "MPISyncCall"]), "Type"] = "MPI"
	df_sum_all.loc[df_sum_all["Method type"].isin(["Program", "Method", "Loop", "Compute", "Block"]), "Type"] = "Compute"
	df_sum_all.loc[df_sum_all["Method type"].isin(["TraceConductor", "AggregationStepper"]), "Type"] = "Compute"
	if sum(df_sum_all["Type"]=="") > 0:
		print(df_sum_all["Method type"].unique())
		raise Exception("Unhandled Method type values, investigate")
	df_sum_all = df_sum_all.drop(["Method type"], axis=1)		
	df_sum_all = df_sum_all.groupby(["Rank", "Window", "Type"]).sum().reset_index()

	return df_sum_all

def collateWindowedAggregateParameter(db, runID, processID, tree, paramName):
	aggStepperNode = tree.findTreeNodeByType("AggregationStepper")
	aggStepperNodeName = aggStepperNode.name
	aggStepperCallPathID = getNodeCallpathId(db, processID, aggStepperNodeName)
	windows = getNodeAggregationWindows(db, runID, processID, aggStepperCallPathID)
	allChildrenCallPathIDs = aggStepperNode.getAllChildrenMembers("dbID")

	db.row_factory = sqlite3.Row
	cur = db.cursor()

	## First, determine parameter type:
	paramTable = None
	for t in ["AggregateParameterBool", "AggregateParameterInt", "AggregateParameterFloat"]:
		query = "SELECT COUNT(*) FROM {0} WHERE RunID = {1} and ProcessID = {2} AND ParamName = \"{3}\" ;".format(t, runID, processID, paramName)
		cur.execute(query)
		count = cur.fetchone()[0]
		if count > 0:
			if not paramTable is None:
				raise Exception("ParamName {0} is present in multiple tables: {1} and {2}".format(paramName, paramTable, t))
			paramTable = t
	if paramTable is None:
		raise Exception("ParamName {0} not found in any AggregateParameter* table".format(paramName))
		# print("WARNING: ParamName {0} not found in any TraceParameter* table".format(paramName))
		# return None

	df_sum_all = None
	for w in windows:
		query = "SELECT A.CallPathID, A.ParamName, A.AvgValue, A.Count " + \
				"FROM {0} AS A ".format(paramTable) + \
				"WHERE A.RunID={0} AND A.ProcessID={1} AND A.Window={2} AND A.ParamName='{3}' ".format(runID, processID, w, paramName)
		## Columns:      0      ,     1     ,    2     ,   3
		##           CallPathID , ParamName , AvgValue , Count
		cur.execute(query)
		result = cur.fetchall()
		if (result is None) or (len(result) == 0):
			continue
		if len(result) > 1:
			raise Exception("Multiple {0} nodes ({1}) meeting criteria: processID={2}, callPathID={3}, window={4}, paramName={5}".format(paramTable, len(result), processID, callPathID, window, paramName))
		r = result[0]
		#result = {"CallPathID":r[0], "ParamName":r[1], "SumValue":r[2]*r[3]}
		result = {"SumValue":r[2]*r[3]}
		result["Window"] = w
		df = pd.DataFrame(data={k:[v] for k,v in result.items()}, columns=result.keys())
		if df_sum_all is None:
			df_sum_all = df
		else:
			df_sum_all = df_sum_all.append(df)

	## TODO: Is summing aggregate parameters always the right choice?
	df_sum_all = df_sum_all.rename(columns={"SumValue":"Value"})

	df_sum_all["Rank"] = getMpiRank(db, processID)
	return df_sum_all


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
		if not data is None:
			for k in data.keys():
				df_cols[k].append(data[k])
	df = pd.DataFrame(data=df_cols, columns=col_names)
	df_grp = df.groupby(['Name', 'TypeName', 'CallPathID']).sum().reset_index()

	return df_grp

def getAllAggregatedTimeStats(db, runID):
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

def getNodeAggregationWindows(db, runID, processID, callPathID):
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	query = "SELECT Window FROM AggregateTime WHERE RunID = {0} AND ProcessID = {1} AND CallPathID = {2}".format(runID, processID, callPathID)	
	cur.execute(query)
	return [r["Window"] for r in cur.fetchall()]

