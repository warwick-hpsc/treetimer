import os
import sqlite3

import numpy as np
import pandas as pd

import imp
imp.load_source("PostProcessDbUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-db-utils.py"))
from PostProcessDbUtils import *
imp.load_source("PostProcessPdUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-pandas-utils.py"))
from PostProcessPdUtils import *
imp.load_source("PostProcessPlotUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-plot-utils.py"))
from PostProcessPlotUtils import *

def traceTimes_aggregateTraceRange(db, runID, processID, nodeEntryId, nodeExitId):
	cur = db.cursor()

	## Query to get trace entries that occur between nodeEntryId and nodeExitId, summing by CallPathId:
	qGetTraceWalltime = "SELECT TraceTimeID, CallPathID, SUM(WallTime) AS WallTime, ParentNodeID, TypeName FROM TraceTimeData NATURAL JOIN CallPathData NATURAL JOIN ProfileNodeData NATURAL JOIN ProfileNodeType WHERE RunID = {0} AND ProcessID = {1} AND NodeEntryID >= {2} AND NodeExitID <= {3} GROUP BY CallPathID".format(runID, processID, nodeEntryId, nodeExitId)

	## Query to, for each trace entry in range, sum walltimes of its immediate children
	qGetChildrenWalltime = "SELECT ParentNodeID AS PID, SUM(WallTime) AS ChildrenWalltime FROM TraceTimeData AS T2 NATURAL JOIN CallPathData NATURAL JOIN ProfileNodeData NATURAL JOIN ProfileNodeType WHERE RunID = {0} AND ProcessID = {1} AND NodeEntryID >= {2} AND NodeExitID <= {3} GROUP BY ParentNodeID".format(runID, processID, nodeEntryId, nodeExitId)

	## Query to join the above two queries into one, bringing together inclusive waltims of each node and its children. Seems to work.
	query12 = "SELECT TraceTimeID, WallTime, ChildrenWalltime, TypeName FROM ({0}) AS A LEFT OUTER JOIN ({1}) AS B ON A.CallPathID = B.PID".format(qGetTraceWalltime, qGetChildrenWalltime)
	cur.execute(query12)
	rows = np.array(cur.fetchall())
	# Set all to have same TraceTimeID, for easier grouping later
	for i in range(len(rows)):
		rows[i][0] = rows[0][0]
	return rows

def traceTimes_aggregateTraceSubRanges(db, runID, processID, nodeEntryId, nodeExitId, subnodeEntryIds, subnodeExitIds):
	cur = db.cursor()

	## Query to get trace entries that occur between nodeEntryId and nodeExitId, summing by CallPathId:
	# qGetTraceWalltime = "SELECT TraceTimeID, CallPathID, SUM(WallTime) AS WallTime, ParentNodeID, TypeName FROM TraceTimeData NATURAL JOIN CallPathData NATURAL JOIN ProfileNodeData NATURAL JOIN ProfileNodeType WHERE RunID = {0} AND ProcessID = {1} AND NodeEntryID >= {2} AND NodeExitID <= {3} GROUP BY CallPathID".format(runID, processID, nodeEntryId, nodeExitId)
	qGetTraceWalltime = "SELECT MIN(TraceTimeID) AS TraceTimeID, MIN(NodeEntryID) AS NodeEntryID, CallPathID, SUM(WallTime) AS WallTime, ParentNodeID, TypeName FROM TraceTimeData NATURAL JOIN CallPathData NATURAL JOIN ProfileNodeData NATURAL JOIN ProfileNodeType WHERE RunID = {0} AND ProcessID = {1} AND NodeEntryID >= {2} AND NodeExitID <= {3} GROUP BY CallPathID".format(runID, processID, nodeEntryId, nodeExitId)

	## Query to, for each trace entry in range, sum walltimes of its immediate children
	qGetChildrenWalltime = "SELECT ParentNodeID AS PID, SUM(WallTime) AS ChildrenWalltime FROM TraceTimeData AS T2 NATURAL JOIN CallPathData NATURAL JOIN ProfileNodeData NATURAL JOIN ProfileNodeType WHERE RunID = {0} AND ProcessID = {1} AND NodeEntryID >= {2} AND NodeExitID <= {3} GROUP BY ParentNodeID".format(runID, processID, nodeEntryId, nodeExitId)

	## Query to join the above two queries into one, bringing together inclusive waltimes of each node and its children. Seems to work.
	query12 = "SELECT NodeEntryID, TraceTimeID, WallTime, ChildrenWalltime, TypeName FROM ({0}) AS A LEFT OUTER JOIN ({1}) AS B ON A.CallPathID = B.PID".format(qGetTraceWalltime, qGetChildrenWalltime)
	cur.execute(query12)
	rows = np.array(cur.fetchall())

	## Finally: filter rows to retain those that occur between pairs of subnodeEntryIds and subnodeExitIds.
	## It should only be necessary to filter against the first pair, because 'qGetTraceWalltime' query only retains MIN(NodeEntryID); but to be safe, will check against all pairs.
	rows_filtered = None
	for r in rows:
		nid = r[0]
		for i in range(len(subnodeEntryIds)):
			if nid >= subnodeEntryIds[i] and nid <= subnodeExitIds[i]:
				## Append, dropping the NodeEntryID
				if rows_filtered is None:
					rows_filtered = np.array([r[1:]])
				else:
					rows_filtered = np.append(rows_filtered, [r[1:]], axis=0)
				break

	# Set all to have same TraceTimeID, for easier grouping later
	for i in range(len(rows_filtered)):
		rows_filtered[i][0] = rows_filtered[0][0]

	return rows_filtered

def traceTimes_aggregateByNode(db, runID, processID, tree, nodeName, nodeOfInterestName=None):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cid = getNodeCallpathId(db, processID, nodeName)
	query = "SELECT NodeEntryID, NodeExitID FROM TraceTimeData WHERE RunID = {0} AND ProcessID = {1} AND CallPathID = {2};".format(runID, processID, cid)
	cur.execute(query)
	result = cur.fetchall()
	nodeEntryIds = [row['NodeEntryID'] for row in result]
	nodeExitIds  = [row['NodeExitID'] for row in result]

	if len(nodeEntryIds) == 0:
		return None

	if not nodeOfInterestName is None:
		## ONLY aggregate across 'nodeOfInterestName', but still aggregating by 'nodeName'.
		## The scenario is a solver loop 'nodeName', where only part of it is of interest ('nodeOfInterestName')

		## First, confirm that 'nodeOfInterest' is child of 'nodeName':
		t = findTreeNodeByName(tree, nodeName)
		if t is None:
			raise Exception("'{0}' not in call tree".format(nodeName))
		t = findTreeNodeByName(tree, nodeOfInterestName)
		if t is None:
			raise Exception("'{0}' not in call tree".format(nodeOfInterestName))
		t = findTreeNodeByName(findTreeNodeByName(tree, nodeName), nodeOfInterestName)
		if t is None:
			raise Exception("'{0}' not child of '{1}'".format(nodeOfInterestName, nodeName))

		cid = getNodeCallpathId(db, processID, nodeOfInterestName)
		query = "SELECT NodeEntryID, NodeExitID FROM TraceTimeData WHERE RunID = {0} AND ProcessID = {1} AND CallPathID = {2};".format(runID, processID, cid)
		cur.execute(query)
		result = cur.fetchall()
		subnodeEntryIds = [row['NodeEntryID'] for row in result]
		subnodeExitIds  = [row['NodeExitID'] for row in result]

		rows_all = None
		for i in range(len(nodeEntryIds)):
			entryID = nodeEntryIds[i]
			exitID = nodeExitIds[i]
			subnodeEntryIdsInRange = [i for i in subnodeEntryIds if (i >= entryID and i <= exitID)]
			subnodeExitIdsInRange = [i for i in subnodeExitIds if (i >= entryID and i <= exitID)]
			rows = traceTimes_aggregateTraceSubRanges(db, runID, processID, entryID, exitID, subnodeEntryIdsInRange, subnodeExitIdsInRange)
			if rows_all is None:
				rows_all = rows
			else:
				rows_all = np.append(rows_all, rows, axis=0)

	else:
		rows_all = None
		for i in range(len(nodeEntryIds)):
			rows = traceTimes_aggregateTraceRange(db, runID, processID, nodeEntryIds[i], nodeExitIds[i])
			if rows_all is None:
				rows_all = rows
			else:
				rows_all = np.append(rows_all, rows, axis=0)

	# Pack rows into a DataFrame for final analysis
	fields = ["TraceTimeID", "WallTime", "ChildrenWalltime", "TypeName"]
	columns = {}
	columns["TraceTimeID"] = [r[0] for r in rows_all]
	columns["WallTime"] = [r[1] for r in rows_all]
	columns["ChildrenWallTime"] = [r[2] for r in rows_all]
	columns["TypeName"] = [r[3] for r in rows_all]
	df = pd.DataFrame(columns)

	df.loc[df["ChildrenWallTime"].isna(), "ChildrenWallTime"] = 0.0
	df["InclusiveTime"] = df["WallTime"] - df["ChildrenWallTime"]
	if sum(df["InclusiveTime"] < 0.0) > 0:
		print(df)
		raise Exception("Negative inclusive times calculated for trace {0}!".format(tid))
	df = df.drop(["WallTime", "ChildrenWallTime"], axis=1)
	df["Type"] = ""
	df.loc[df["TypeName"].isin(["MPICollectiveCall", "MPICommCall", "MPISyncCall"]), "Type"] = "MPI"
	df.loc[df["TypeName"].isin(["Method", "Loop", "Compute", "Block"]), "Type"] = "Compute"
	df.loc[df["TypeName"].isin(["TraceConductor", "AggregationStepper"]), "Type"] = "Compute"
	if sum(df["Type"]=="") > 0:
		print(df["TypeName"].unique())
		raise Exception("Unhandled TypeName values, investigate")
	df = df.drop(["TypeName"], axis=1)
	df = df.groupby(["TraceTimeID", "Type"]).sum().reset_index()
	df2 = df.groupby("TraceTimeID").sum().reset_index().rename(columns={"InclusiveTime":"TotalTime"})
	df = df.merge(df2)
	df["InclusiveTime %"] = df["InclusiveTime"] / df["TotalTime"] * 100.0
	df = df.drop("TotalTime", axis=1)

	return df

def traceParameter_aggregateTraceRange(db, runID, processID, paramTable, paramName, nodeEntryId, nodeExitId):
	cur = db.cursor()

	traceParamIdColMap = {}
	traceParamIdColMap["TraceParameterBoolData"] = "TraceParamBoolID"
	traceParamIdColMap["TraceParameterIntData"] = "TraceParamIntID"
	traceParamIdColMap["TraceParameterLongData"] = "TraceParamLongID"
	traceParamIdColMap["TraceParameterDoubleData"] = "TraceParamDoubleID"

	## To reduce code development time, check that parameter only has one value in specified nodeID range.
	## If this is not the case, then need to think about what statistics to report (average? max and min? variance?). 
	## Multiple parameter values cannot be handled like runtimes (which can be summed).
	qCountQuery = "SELECT COUNT(*) FROM {0} NATURAL JOIN CallPathData WHERE ProcessID = {1} AND ParamName = \"{2}\" AND NodeEntryID >= {3} AND NodeExitID <= {4} GROUP BY ParentNodeID".format(paramTable, processID, paramName, nodeEntryId, nodeExitId)
	cur.execute(qCountQuery)
	res = cur.fetchone()
	if res is None:
		return None
	count = res[0]
	if count > 1:
		raise Exception("Parameter '{0}' recorded multiple values between a specific nodeID range. This situation has not been coded in TreeTimer, contact developers to request average, variance, or some other aggregating function.")

	query = "SELECT {0} AS TraceParamId, ParamValue FROM {1} NATURAL JOIN CallPathData WHERE ProcessID = {2} AND ParamName = \"{3}\" AND NodeEntryID >= {4} AND NodeExitID <= {5} GROUP BY ParentNodeID".format(traceParamIdColMap[paramTable], paramTable, processID, paramName, nodeEntryId, nodeExitId)
	cur.execute(query)
	res = cur.fetchone()
	row = [res["TraceParamId"], res["ParamValue"]]
	return [row]

def traceParameter_aggregateByNode(db, runID, processID, tree, nodeName, paramName, nodeOfInterestName=None):
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	## First, determine parameter type:
	paramTable = None
	for t in ["TraceParameterBoolData", "TraceParameterIntData", "TraceParameterFloatData"]:
		query = "SELECT COUNT(*) FROM {0} WHERE RunID = {1} and ProcessID = {2} AND ParamName = \"{3}\" ;".format(t, runID, processID, paramName)
		cur.execute(query)
		count = cur.fetchone()[0]
		if count > 0:
			if not paramTable is None:
				raise Exception("ParamName {0} is present in multiple tables: {1} and {2}".format(paramName, paramTable, t))
			paramTable = t
	if paramTable is None:
		# raise Exception("ParamName {0} not found in any TraceParameter* table".format(paramName))
		# print("WARNING: ParamName {0} not found in any TraceParameter* table".format(paramName))
		return None

	query = "SELECT COUNT(*) FROM TraceParameterBoolData WHERE ProcessID = {0} AND ParamName = \"TraceConductorEnabled?\";".format(processID)
	cur.execute(query)
	count = cur.fetchone()[0]
	if count == 0:
		raise Exception("TreeTimer processID {0} has not written parameter 'TraceConductorEnabled?' to table TraceParameterBoolData, which is necessary for grouping trace parameter by ".format(processID))

	cid = getNodeCallpathId(db, processID, nodeName)
	query = "SELECT NodeEntryID, NodeExitID FROM TraceParameterBoolData WHERE RunID = {0} AND ProcessID = {1} AND CallPathID = {2} AND ParamName = \"TraceConductorEnabled?\";".format(runID, processID, cid)
	cur.execute(query)
	result = cur.fetchall()
	nodeEntryIds = [row['NodeEntryID'] for row in result]
	nodeExitIds  = [row['NodeExitID'] for row in result]

	if len(nodeEntryIds) == 0:
		return None

	rows_all = None
	for i in range(len(nodeEntryIds)):
		rows = traceParameter_aggregateTraceRange(db, runID, processID, paramTable, paramName, nodeEntryIds[i], nodeExitIds[i])
		if not rows is None:
			if rows_all is None:
				rows_all = rows
			else:
				rows_all = np.append(rows_all, rows, axis=0)

	# Pack rows into a DataFrame for final analysis
	fields = ["TraceParamID", "ParamName", "Value"]
	columns = {}
	columns["TraceParamID"] = [r[0] for r in rows_all]
	columns["ParamName"] = [paramName]*len(rows_all)
	columns["Value"] = [r[1] for r in rows_all]
	df = pd.DataFrame(columns)
	return df

def add_unit_stride_index_column(df, id_colname):
	## Add a unit-stride index column:
	## Note: different ranks can have different TraceTimeID for same solver timestep. So, 
	##       need to process each rank individually.
	col_ranks = []
	col_ids = []
	col_indices = []
	for r in df["Rank"].unique():
		traceTimesIDs = df.loc[df["Rank"]==r, id_colname].unique()
		traceTimesIDs.sort()
		n = len(traceTimesIDs)
		indices = np.arange(0,n)
		col_ranks += [r]*n
		col_ids += traceTimesIDs.tolist()
		col_indices += indices.tolist()
	df_ids = pd.DataFrame({"Rank":col_ranks, id_colname:col_ids, "TimestepIndex":col_indices})
	df = df.merge(df_ids, validate="many_to_one")

	return df

def traceTimes_chartDynamicLoadBalance(traces_df, output_dirpath=None, filename_suffix=None):
	# Restrict to MPI time %:
	mpi_traces = traces_df[traces_df["Type"]=="MPI"]
	if mpi_traces.shape[0] == 0:
		print("No MPI functions were timed")
		return
	mpi_traces = mpi_traces.drop(["Type", "InclusiveTime"], axis=1)
	mpi_traces = mpi_traces.rename(columns={"InclusiveTime %":"MPI %"})

	r_root = mpi_traces["Rank"].min()
	num_ts_root = mpi_traces[mpi_traces["Rank"]==r_root].shape[0]
	for r in mpi_traces["Rank"].unique():
		num_ts = mpi_traces[mpi_traces["Rank"]==r].shape[0]
		if num_ts != num_ts_root:
			raise Exception("Ranks {0} and {1} performed different number of solver timesteps - {2} vs {3}".format(r_root, r, num_ts_root, num_ts))

	## Add a unit-stride index column:
	mpi_traces = add_unit_stride_index_column(mpi_traces, "TraceTimeID")
	## Can drop 'TraceTimeID', a SQL relic:
	mpi_traces = mpi_traces.drop("TraceTimeID", axis=1)

	# # Discard first N timesteps as warming-up:
	# N = 2
	# if N < mpi_traces["TimestepIndex"].max():
	# 	mpi_traces = mpi_traces[mpi_traces["TimestepIndex"]>N].reset_index(drop=True)
	# 	mpi_traces["TimestepIndex"] -= N

	## Evenly sample 100 timepoints, so that final chart is legible:
	mpi_traces = sample_n_timesteps(mpi_traces, 100, "TimestepIndex")
	timestepIndices = mpi_traces["TimestepIndex"].unique()
	timestepIndices.sort()

	## Ensure table is sorted for calculation
	mpi_traces = mpi_traces.sort_values(["TimestepIndex", "Rank"])

	## Construct heatmap, of MPI % during simulation:
	if not filename_suffix is None:
		fig_filename = "mpi-pct-heatmap.{0}.png".format(filename_suffix)
	else:
		fig_filename = "mpi-pct-heatmap.png"
	if not output_dirpath is None:
		fig_filepath = os.path.join(output_dirpath, fig_filename)
	else:
		fig_filepath = fig_filename
	plot_heatmap(mpi_traces, "TimestepIndex", "MPI %", fig_filepath)

	## Finally! Calculate difference in MPI % over time:
	col_ranks = None
	col_index = None
	col_diffs = None
	# step = 1
	# step = 10
	step = mpi_traces[mpi_traces["Rank"]==mpi_traces["Rank"].min()].shape[0]//2
	# step = 50
    # # Zero up until step kicks in
	# for ts in range(0, step):
	# 	ts0 = mpi_traces[mpi_traces["TimestepIndex"]==timestepIndices[ts]]
	# 	col_ranks = ts0["Rank"].values if (col_ranks is None) else np.append(col_ranks, ts0["Rank"].values)
	# 	col_index = ts0["TimestepIndex"].values if (col_index is None) else np.append(col_index, ts0["TimestepIndex"].values)
	# 	diff = [0.0]*ts0["Rank"].shape[0]
	# 	col_diffs = diff if (col_diffs is None) else np.append(col_diffs, diff)
	# ts0 = mpi_traces[mpi_traces["TimestepIndex"]==timestepIndices[0]]
	# for ts in range(step, len(timestepIndices)):
	# 	ts1 = mpi_traces[mpi_traces["TimestepIndex"]==timestepIndices[ts]]
	# 	## Calculate diff, etc
	# 	col_ranks = ts1["Rank"].values if (col_ranks is None) else np.append(col_ranks, ts1["Rank"].values)
	# 	col_index = ts1["TimestepIndex"].values if (col_index is None) else np.append(col_index, ts1["TimestepIndex"].values)
	# 	diff = np.absolute(ts1["MPI %"].values - ts0["MPI %"].values)
	# 	col_diffs = diff if (col_diffs is None) else np.append(col_diffs, diff)
	# 	ts0 = mpi_traces[mpi_traces["TimestepIndex"]==timestepIndices[ts-step]]
	## Calculate difference against last:
	tsLast = mpi_traces[mpi_traces["TimestepIndex"]==timestepIndices[-1]]
	for ts in range(0, len(timestepIndices)):
		ts1 = mpi_traces[mpi_traces["TimestepIndex"]==timestepIndices[ts]]
		## Calculate diff, etc
		col_ranks = ts1["Rank"].values if (col_ranks is None) else np.append(col_ranks, ts1["Rank"].values)
		col_index = ts1["TimestepIndex"].values if (col_index is None) else np.append(col_index, ts1["TimestepIndex"].values)
		#diff = np.absolute(tsLast["MPI %"].values - ts1["MPI %"].values)
		diff = ts1["MPI %"].values - tsLast["MPI %"].values
		col_diffs = diff if (col_diffs is None) else np.append(col_diffs, diff)
	diff_df = pd.DataFrame({"TimestepIndex":col_index, "Rank":col_ranks, "(MPI %) diff":col_diffs})

	if not filename_suffix is None:
		fig_filename = "mpi-pct-change-heatmap.{0}.png".format(filename_suffix)
	else:
		fig_filename = "mpi-pct-change-heatmap.png"
	if not output_dirpath is None:
		fig_filepath = os.path.join(output_dirpath, fig_filename)
	else:
		fig_filepath = fig_filename
	plot_heatmap(diff_df, "TimestepIndex", "(MPI %) diff", fig_filepath)

	## Sum stdev across ranks
	diffSum_df = diff_df.drop("Rank", axis=1).groupby("TimestepIndex").sum().reset_index()
	diffSum_df = diffSum_df.rename(columns={"(MPI %) diff":"Sum MPI % diff"})
	diffSum_stdev = diffSum_df["Sum MPI % diff"].std()
	diffSum_mean = diffSum_df["Sum MPI % diff"].mean()
	diffSum_stdev_pct = 0.0 if diffSum_mean == 0.0 else diffSum_stdev/diffSum_mean*100.0
	print("diffSum_stdev % mean = {0:.1f}".format(diffSum_stdev_pct))

def traceParameter_chart(traceParam_df, paramName, output_dirpath=None):
	r_root = traceParam_df["Rank"].min()
	num_ts_root = traceParam_df[traceParam_df["Rank"]==r_root].shape[0]
	for r in traceParam_df["Rank"].unique():
		num_ts = traceParam_df[traceParam_df["Rank"]==r].shape[0]
		if num_ts != num_ts_root:
			raise Exception("Ranks {0} and {1} performed different number of solver timesteps - {2} vs {3}".format(r_root, r, num_ts_root, num_ts))

	# ## Add a unit-stride index column:
	traceParam_df = add_unit_stride_index_column(traceParam_df, "TraceParamID")
	## Can drop 'TraceParamID', a SQL relic:
	traceParam_df = traceParam_df.drop("TraceParamID", axis=1)

	## Evenly sample 100 timepoints, so that final chart is legible:
	traceParam_df = sample_n_timesteps(traceParam_df, 100, "TimestepIndex")
	timestepIndices = traceParam_df["TimestepIndex"].unique()
	timestepIndices.sort()

	## Ensure table is sorted for calculation
	traceParam_df = traceParam_df.sort_values(["TimestepIndex", "Rank"])

	## Construct heatmap, of MPI % during simulation:
	fig_filename = "traceParameter-{0}-heatmap.png".format(paramName)
	if not output_dirpath is None:
		fig_filepath = os.path.join(output_dirpath, fig_filename)
	else:
		fig_filepath = fig_filename
	plot_heatmap(traceParam_df, "TimestepIndex", "Value", fig_filepath)
