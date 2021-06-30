#!/usr/bin/python

# Post-processing of treetimer database data

# Select the desired functionality from the main method below by uncommenting the method call
# (todo - add a command line selector!)

# Functionality Needed

# (1) Display Full CallTree (Functions only) alongside exclusive time spent in each function
#	 (Some functions may not be timed individually due to overhead, and thus will show
#	  as time under another function)

# (2) Display Hotspots by Function Exclusive Time (Ordered Version of (1))

# (6) Validate that all function time is captured (summation == walltime)
# (7) Validate that all non function time is captured (i.e. functions are suitable broken down into compute, comms blocks)

# (3) Display Hotspots by Compute Time

# (4) Display Compute/MPI Breakdown for a run

# (5) Display Time by Algorithmic Purpose (domain-specific - e.g. exchange components, combustion, spray etc)

# (8) Generate Wgs for Compute Blocks (time/suitable parameters)

import sys
if sys.version_info[0] < 3:
	raise Exception("Script requires Python 3+")

import os, re, math
from pprint import pprint
from copy import deepcopy
from time import sleep

import sqlite3

import pickle

import imp
imp.load_source("PostProcessDbUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-db-utils.py"))
from PostProcessDbUtils import *
imp.load_source("PostProcessPlotUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-plot-utils.py"))
from PostProcessPlotUtils import *

parallel_process = True
#parallel_process = False

verbose = False
#verbose = True

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

from multiprocessing import Pool, current_process, cpu_count
nprocs = cpu_count() - 1
import tqdm

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-d', '--tt-results-dirpath', help="Dirpath to 'tt_results' folder")
parser.add_argument('-b', '--db-filepath', help="Analyse this specific database file")
parser.add_argument('-t', '--trace-group-focus', help='When grouping traces by loop iterator, can filter by a block within loop')
parser.add_argument('-p', '--parameter', help='Which trace parameter to process')
parser.add_argument('-c', '--charts', choices=["polar", "horizontal", "vertical"], help="Chart the call stack runtime breakdown across ranks")
parser.add_argument('-r', '--chart-ranks', action='store_true', help="If charting, also draw call stack chart for each rank. Expensive!")
parser.add_argument('-l', '--label', action='store_true', help="Add labels to chart elements. Optional because can create clutter")
args = parser.parse_args()

tt_folder_dirpath = args.tt_results_dirpath
tt_db_filepath = args.db_filepath
if (tt_folder_dirpath is None) and (tt_db_filepath is None):
	raise Exception("Must specify either folder or DB file to analyse.")
if (not tt_folder_dirpath is None) and (not tt_db_filepath is None):
	raise Exception("Don't specify folder and DB file. Just one.")


import enum
class PlotType(enum.Enum):
	Polar = 1
	Horizontal = 2
	Vertical = 3
plotTypeToString = {PlotType.Polar:"Polar", PlotType.Horizontal:"Horizontal", PlotType.Vertical:"Vertical"}

# fig_dims = (16,16)
fig_dims = (8,8)


methodTypeToColour = {}
methodTypeToColour["Program"] = "silver"
methodTypeToColour["TraceConductor"] = "silver"
methodTypeToColour["Method"] = "silver"
methodTypeToColour["Block"] = "silver"
methodTypeToColour["Compute"] = "fuchsia"
methodTypeToColour["Loop"] = "silver"
methodTypeToColour["ComputeLoop"] = "fuchsia"
methodTypeToColour["NonMPIMethodCall"] = "fuchsia"
methodTypeToColour["MPICommCall"] = "aqua"
methodTypeToColour["MPINonCommMethodCall"] = "orange"
methodTypeToColour["MPISyncCall"] = "orange"
methodTypeToColour["MPICollectiveCall"] = "red"
methodTypeToColour["LibraryCall"] = "yellowgreen"

def preprocess_db(db_fp, ctr, num_dbs, cache_dp):
	if verbose:
		print("Processing: {0} ({1}/{2})".format(db_fp, ctr, num_dbs))
	db = sqlite3.connect(db_fp)
	## Loading DB into memory roughly halves query times. 
	## But only load if access required.
	dbm = None

	f = os.path.basename(db_fp)
	m = re.match("^results\.([0-9]+)\.db$", f)
	rank = int(m.groups()[0])

	df_csv = os.path.join(cache_dp, f+".csv")
	if not os.path.isfile(df_csv):
		if verbose:
			print("- generating: " + df_csv)
		if dbm is None:
			dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
		df = aggregatedTimes_calculateHotspots(1, 1, dbm)
		df["rank"] = rank
		df.to_csv(df_csv, index=False)

	df_agg_fp = os.path.join(cache_dp, f+".typeAgg.csv")
	if not os.path.isfile(df_agg_fp):
		if verbose:
			print("- generating: " + df_agg_fp)
		if dbm is None:
			dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
		df_agg = aggregateTimesByType(1, 1, dbm)
		df_agg["rank"] = rank
		df_agg.to_csv(df_agg_fp, index=False)

	tree_fp = os.path.join(cache_dp, f+".call-tree.pkl")
	t = None
	if not os.path.isfile(tree_fp):
		if verbose:
			print("- generating: " + tree_fp)
		if dbm is None:
			dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
		t = buildCallPathTree(1, 1, dbm)
		with open(tree_fp, 'wb') as output:
			#pickle.dump(t, output, pickle.HIGHEST_PROTOCOL)
			pickle.dump(t, output, 4)

	if not table_empty(db, "TraceTimeData"):
		traceTimes_fp = os.path.join(cache_dp, f+".traceTimes.csv")
		if not os.path.isfile(traceTimes_fp):
			if verbose:
				print("- generating: " + traceTimes_fp)
			if t is None:
				with open(tree_fp, 'rb') as input:
					t = pickle.load(input)

			n = findTreeNodeByType(t, "TraceConductor")
			if n is None:
				n = findSolverNode(t, 1, t.time)
			if n is None:
				raise Exception("Could not deduce top-most callpath node in solver loop, so cannot perform trace analysis")
			else:
				traceGroupNode = n.name
				if dbm is None:
					dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
				traces_df = traceTimes_aggregateByNode(dbm, 1, 1, t, traceGroupNode)
				traces_df["Rank"] = rank
				traces_df.to_csv(traceTimes_fp, index=False)

		if args.trace_group_focus:
			traceTimes_focused_fp = os.path.join(cache_dp, f+".time-traces.focused-on-{0}.csv".format(args.trace_group_focus))
			if not os.path.isfile(traceTimes_focused_fp):
				if verbose:
					print("- generating: " + traceTimes_focused_fp)
				if t is None:
					with open(tree_fp, 'rb') as input:
						t = pickle.load(input)
				n = findTreeNodeByType(t, "TraceConductor")
				if n is None:
					n = findSolverNode(t, 1, t.time)
				if n is None:
					raise Exception("Could not deduce top-most callpath node in solver loop, so cannot perform trace analysis")
				else:
					traceGroupNode = n.name
					if dbm is None:
						dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
					traces_df = traceTimes_aggregateByNode(dbm, 1, 1, t, traceGroupNode, args.trace_group_focus)
					traces_df["Rank"] = rank
					traces_df.to_csv(traceTimes_focused_fp, index=False)

	if (not args.parameter is None):
		traceParameter_fp = os.path.join(cache_dp, f+".traceParameter-{0}.csv".format(args.parameter))
		if not os.path.isfile(traceParameter_fp):
			if verbose:
				print("- generating: " + traceParameter_fp)
			if t is None:
				with open(tree_fp, 'rb') as input:
					t = pickle.load(input)
			n = findTreeNodeByType(t, "TraceConductor")
			if n is None:
				n = findSolverNode(t, 1, t.time)
			if n is None:
				raise Exception("Could not deduce top-most callpath node in solver loop, so cannot perform trace analysis")
			else:
				traceGroupNode = n.name
				if dbm is None:
					dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
				traces_df = traceParameter_aggregateByNode(dbm, 1, 1, t, traceGroupNode, args.parameter)
				traces_df["Rank"] = rank
				traces_df.to_csv(traceParameter_fp, index=False)

	return True

def preprocess_db_job(task):
	db_fp = task[0]
	i = task[1]
	n = task[2]
	cache_dp = task[3]
	preprocess_db(db_fp, i, n, cache_dp)

def main():
	if (not tt_folder_dirpath is None) and not os.path.isdir(tt_folder_dirpath):
		raise Exception("Folder does not exist: {0}".format(tt_folder_dirpath))

	df_all_raw = None
	df_all_aggregated = None

	traceTimes_all_df = None
	traceTimes_focused_all_df = None
	traceParameter_all_df = None

	rank_ids = set()

	## Group together rank call traces that have identical topology:
	groupedCallTrees = None

	db_fps = []
	if not tt_folder_dirpath is None:
		output_dirpath = tt_folder_dirpath
		cache_dp = os.path.join(tt_folder_dirpath, "_tt_cache")
		for run_root, run_dirnames, run_filenames in os.walk(tt_folder_dirpath):
			for f in run_filenames:
				if re.match("^results\.([0-9]+)\.db$", f):
					db_fps.append(os.path.join(tt_folder_dirpath, f))
	else:
		output_dirpath = os.path.dirname(os.path.realpath(tt_db_filepath))
		cache_dp = os.path.join(output_dirpath, "_tt_cache")
		db_fps = [tt_db_filepath]

	if not os.path.isdir(cache_dp):
		os.mkdir(cache_dp)

	if parallel_process:
		nprocs = cpu_count() - 1
		print("Processing {0} DB files in parallel on {1} CPUs".format(len(db_fps), nprocs))
		dbs_pending = []
		for i in range(0, len(db_fps)):
			db_fp = db_fps[i]
			params = (db_fp, i+1, len(db_fps), cache_dp)
			dbs_pending.append(params)
		p = Pool(nprocs)
		if verbose:
			p.imap_unordered(preprocess_db_job, dbs_pending)
		else:
			for _ in tqdm.tqdm(p.imap_unordered(preprocess_db_job, dbs_pending), total=len(dbs_pending)):
				pass
	else:
		print("Processing {0} DB files".format(len(db_fps)))
		for i in tqdm.tqdm(range(0, len(db_fps))):
			preprocess_db(db_fps[i], i+1, len(db_fps), cache_dp)
	os.sync()
	sleep(1) ## Give time for file writes to complete
	print("All DBs pre-processed, outputs cached")

	print("Collating data")
	ctr = 0
	for db_fp in db_fps:
		f = os.path.basename(db_fp)
		m = re.match("^results\.([0-9]+)\.db$", f)
		if m:
			ctr += 1
			rank = int(m.groups()[0])
			rank_ids.add(rank)

			if verbose:
				print("Collating rank {0} data".format(rank))

			df_csv = os.path.join(cache_dp, f+".csv")
			if not os.path.isfile(df_csv):
				print("Processing: {0} ({1}/{2})".format(db_fp, ctr, len(db_fps)))
				preprocess_db(db_fp, ctr, len(db_fps), cache_dp)
			df = pd.read_csv(df_csv)
			if df_all_raw is None:
				df_all_raw = df
			else:
				df_all_raw = df_all_raw.append(df)

			df_agg_fp = os.path.join(cache_dp, f+".typeAgg.csv")
			df_agg = pd.read_csv(df_agg_fp)
			if df_all_aggregated is None:
				df_all_aggregated = df_agg
			else:
				df_all_aggregated = df_all_aggregated.append(df_agg)

			traceTimes_fp = os.path.join(cache_dp, f+".traceTimes.csv")
			if os.path.isfile(traceTimes_fp):
				df = pd.read_csv(traceTimes_fp)
				if traceTimes_all_df is None:
					traceTimes_all_df = df
				else:
					traceTimes_all_df = traceTimes_all_df.append(df)
				if args.trace_group_focus:
					traceTimes_focused_fp = os.path.join(cache_dp, f+".time-traces.focused-on-{0}.csv".format(args.trace_group_focus))
					df = pd.read_csv(traceTimes_focused_fp)
					if traceTimes_focused_all_df is None:
						traceTimes_focused_all_df = df
					else:
						traceTimes_focused_all_df = traceTimes_focused_all_df.append(df)
			#else:
			#	print("Cannot find: " + traceTimes_fp)

			if not args.parameter is None:
				traceParameter_fp = os.path.join(cache_dp, f+".traceParameter-{0}.csv".format(args.parameter))
				if os.path.isfile(traceParameter_fp):
					df = pd.read_csv(traceParameter_fp)
					if traceParameter_all_df is None:
						traceParameter_all_df = df
					else:
						traceParameter_all_df = traceParameter_all_df.append(df)

			tree_fp = os.path.join(cache_dp, f+".call-tree.pkl")
			with open(tree_fp, 'rb') as input:
				t = pickle.load(input)

			if not tt_db_filepath is None:
				# Just one DB being analysed, so print out call tree:
				print(t)

			if not args.charts is None:
				## Add tree to a group
				if groupedCallTrees is None:
					groupedCallTrees = [ {rank:t} ]
				else:
					found_group = False
					for treeGroup in groupedCallTrees:
						tOther = next(iter(treeGroup.values()))
						if t == tOther:
							treeGroup[rank] = t
							found_group = True
					if not found_group:
						groupedCallTrees.append({rank:t})

				# Plot this call tree
				if args.charts and (args.chart_ranks or rank==1):
					print(" - drawing call-tree chart ...")
					chartCallPath(t, "Call stack times of rank {0}".format(rank), "rank-{0}.png".format(rank))

	if not args.charts is None:
		## Aggregate together call trees within each group, and create plots:
		print("Drawing grouped call-tree charts")
		aggregatedCallTrees = []
		gn = 0
		for g in groupedCallTrees:
			gn += 1
			agg = None
			for rank,t in g.items():
				if agg is None:
					agg = CallTreeNodeAggregated(t, rank)
				else:
					agg.appendNodeElementwise(t, rank)
			aggSum = agg.sumElementwise()
			chartCallPath(aggSum, "Call stack times summed across ranks "+sorted(agg.ranks).__str__(), "rankGroup{0}.png".format(gn))

	print("Drawing trace charts")
	if not traceTimes_all_df is None:
		traceTimes_chartDynamicLoadBalance(traceTimes_all_df)
	if not traceTimes_focused_all_df is None:
		traceTimes_chartDynamicLoadBalance(traceTimes_focused_all_df, "focused-on-"+args.trace_group_focus)
	if not traceParameter_all_df is None:
		traceParameter_chart(traceParameter_all_df, args.parameter)

	print("Writing out collated CSVs")
	df_all_raw["num_ranks"] = len(rank_ids)
	df_all_raw.sort_values(["Name", "rank"], inplace=True)
	df_filename = os.path.join(output_dirpath, "timings_raw.csv")
	df_all_raw.to_csv(df_filename, index=False)
	print("Collated raw data written to '{0}'".format(df_filename))


	df_all_aggregated.sort_values(["Method type", "rank"], inplace=True)
	df_filename = os.path.join(output_dirpath, "timings_aggregated.csv")
	df_all_aggregated.to_csv(df_filename, index=False)
	print("Collated aggregated data written to '{0}'".format(df_filename))


# Note - ProcessID is just a DB key, NOT the MPI rank.
def aggregatedTimes_calculateHotspots(runID, processID, db):
	if table_empty(db, "AggregateTime"):
		raise Exception("DB table 'AggregateTime' is empty")

	rootID = getNodeCallpathId(db, 'ProgramRoot')
	rootRecord = getNodeAggregatedStats(rootID, runID, processID, db)

	df = getAllProfileNodesAggregateTimeExclusive(runID, processID, db)

	df['ExclusiveAggregateTime(% of Run)'] = df['AggTotalTimeE']/rootRecord['AggTotalTimeI']
	df = df.rename(columns={'AggTotalTimeI':'InclusiveAggregateTime(s)', 'AggTotalTimeE':'ExclusiveAggregateTime(s)'})
	df = df.drop(['AggAvgTime', 'AggMaxTime', 'AggMinTime', 'CallPathID'], axis=1)
	df = df.sort_values('ExclusiveAggregateTime(% of Run)', ascending=False)

	return df

# Note - ProcessID is just a DB key, NOT the MPI rank.
def getAllProfileNodesAggregateTimeExclusive(runID, processID, db):

	# Since aggregate properties operate on call path nodes (since
	# times for different paths to the same profile node are stored
	# separately), we need to merge these values.

	# First, identify the callpath nodes associated with a run & process
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT DISTINCT CallPathID FROM AggregateTime WHERE RunID = {0} AND ProcessID = {1}".format(runID, processID)
	cur.execute(cmd)
	result = cur.fetchall()

	callpathIDs = [row['CallPathID'] for row in result]

	col_names = ["Name", "TypeName", "CallPathID", "CallCount", "AggTotalTimeI", "AggTotalTimeE", "AggAvgTime", "AggMaxTime", "AggMinTime"]
	df = None
	df_cols = {c:[] for c in col_names}
	for callpathID in callpathIDs:
		data = getNodeAggregatedStats(callpathID, runID, processID, db)
		for k in data.keys():
			df_cols[k].append(data[k])
		df_cols['CallPathID'].append(callpathID)
	df = pd.DataFrame(data=df_cols, columns=col_names)
	df_grp = df.groupby(['Name', 'TypeName', 'CallPathID']).sum().reset_index()

	return df_grp

def getNodeCallpathId(db, nodeName):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT CallPathID FROM CallPathData NATURAL JOIN ProfileNodeData WHERE NodeName = '{0}'".format(nodeName)
	cur.execute(cmd)
	result = cur.fetchone()
	return result['CallPathID']

def getNodeChildrenIDs(db, callPathID):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT CallPathID FROM CallPathData WHERE ParentNodeID = {0}".format(callPathID)
	cur.execute(cmd)
	result = cur.fetchall()
	return [i['CallPathID'] for i in result]

class CallTreeNodeIterator:
	def __init__(self, callTreeNode, am_root=False):
		self.callTreeNode = callTreeNode
		self.callTreeNodeLeaves = sorted(callTreeNode.leaves)
		self.currentLeafIter = None
		self.index = -1
		self.am_root = am_root

	def __next__(self):
		n = None
		if self.index == -1:
			n = self.callTreeNode.time
			# n = "{0} : {1}".format(self.callTreeNode.name, self.callTreeNode.time)
			self.index += 1
		else:
			if len(self.callTreeNodeLeaves) > 0:
				while self.index < len(self.callTreeNodeLeaves):
					if self.currentLeafIter is None:
						self.currentLeafIter = self.callTreeNodeLeaves[self.index].__iter__()
					n = self.currentLeafIter.__next__()
					if not n is None:
						break
					self.index += 1
					self.currentLeafIter = None

		if n is None and self.am_root:
			raise StopIteration
		return n

class CallTreeNode:
	def __init__(self, name, typeName, time, calls):
		self.name = name
		self.typeName = typeName
		self.time = time
		self.calls = calls
		self.leaves = []

	def addLeaf(self, leaf):
		self.leaves.append(leaf)

	## Comparison operators for tree structure, ignoring time:
	def __lt__(self, other):
		if self.name != other.name:
			return self.name < other.name
		if self.typeName != other.typeName:
			return self.typeName < other.typeName
		return self.leaves < other.leaves

	def __eq__(self, other):
		if self.name != other.name:
			return False
		if self.typeName != other.typeName:
			return False
		return self.leaves == other.leaves

	def toString(self, maxdepth=99):
		return self.__str__(maxdepth=maxdepth)

	def __str__(self, maxdepth=99, depth=0):
		nodeStr = ""
		if depth > 0:
			#nodeStr += "  :"*(depth-1) + "  |" + "-"
			nodeStr += " :"*(depth-1) + " |" + "-"
		else:
			nodeStr += " "
		#nodeStr += "{0} [{1}] - {2:.2f} seconds\n".format(self.name, self.typeName, self.time)
		detailsStr = self.name.ljust(60-depth*2)
		detailsStr += " - {0:.2f}s".format(self.time)
		#detailsStr += " {0} calls".format(self.calls)
		if self.calls == 1:
			detailsStr += ", x{0} call".format(self.calls)
		else:
			detailsStr += ", x{0} calls".format(self.calls)
		detailsStr += " [{0}]".format(self.typeName).ljust(12)
		nodeStr += detailsStr + "\n"

		if maxdepth > 0 and depth < maxdepth-1:
			for c in self.leaves:
				cStr = c.__str__(maxdepth, depth+1)
				nodeStr += cStr
		return nodeStr

	def __add__(self, other):
		if not self == other:
			raise Exception("Tree structures not identical")
		summed = CallTreeNode(self.name, self.typeName, self.time, self.calls)
		summed.time += other.time
		for idx in range(len(self.leaves)):
			summed.addLeaf(self.leaves[idx] + other.leaves[idx])
		return summed

	def __iter__(self, am_root=False):
		return CallTreeNodeIterator(self, self.name=="ProgramRoot")

class CallTreeNodeAggregated:
	def __init__(self, node, rank):
		if not isinstance(node, CallTreeNode):
			raise Exception("Must initialise CallTreeNodeAggregated with a CallTreeNode")

		self.name = node.name
		self.typeName = node.typeName
		self.times = [node.time]
		self.calls = [node.calls]
		self.leaves = []
		self.init_node_copy = deepcopy(node)
		self.ranks = [rank]
		for l in sorted(node.leaves):
			self.leaves.append(CallTreeNodeAggregated(l, rank))

	def appendNodeElementwise(self, node, rank):
		if not isinstance(node, CallTreeNode):
			raise Exception("Addition must be a CallTreeNode")

		if self.init_node_copy != node:
			raise Exception("Attempting to add node to CallTreeNodeAggregated with different topology")

		self.times.append(node.time)
		self.calls.append(node.calls)
		self.ranks.append(rank)
		nodeLeavesSorted = sorted(node.leaves)
		for idx in range(len(self.leaves)):
			self.leaves[idx].appendNodeElementwise(nodeLeavesSorted[idx], rank)

	def sumElementwise(self):
		timesSum = sum(self.times)
		sumNode = CallTreeNode(self.name, self.typeName, timesSum, None)
		for l in self.leaves:
			sumNode.addLeaf(l.sumElementwise())
		return(sumNode)

	def __str__(self, indent=0):
		timesStr = "[{0:.2f}".format(self.times[0])
		for i in range(1,len(self.times)):
			timesStr += ", {0:.2f}".format(self.times[i])
		timesStr += "]"
		# nodeStr = " "*indent + "{0} [{1}] - {2}\n".format(self.name, self.typeName, timesStr)
		nodeStr = " "*indent + "{0} [{1}]:\n".format(self.name, self.typeName)
		nodeStr+= " "*indent + "  times: {0}\n".format(timesStr)
		nodeStr+= " "*indent + "  ranks: {0}\n".format(self.ranks.__str__())
		for c in self.leaves:
			cStr = c.__str__(indent+1)
			nodeStr += cStr
		return nodeStr

def buildCallPathTree(runID, processID, db):
	rootID = getNodeCallpathId(db, 'ProgramRoot')
	tree = None
	x = buildCallPathNodeTraversal(runID, processID, db, tree, rootID, 0)
	return x

def buildCallPathNodeTraversal(runID, processID, db, treeNode, nodeID, indentLevel):
	# Recursive depth-first traversal through call stack
	record = getNodeAggregatedStats(nodeID, runID, processID, db)

	leaf = CallTreeNode(record["Name"], record["TypeName"], record["AggTotalTimeI"], record["CallCount"])
	am_root = False
	if treeNode is None:
		am_root = True
		treeNode = leaf
	else:
		treeNode.addLeaf(leaf)

	childNodes = getNodeChildrenIDs(db, nodeID)
	for childID in childNodes:
		buildCallPathNodeTraversal(runID, processID, db, leaf, childID, indentLevel + 1)

	if am_root:
		return treeNode

def findSolverNode(tree, parentCalls, walltime):
	con1 = tree.time > (0.7*walltime)
	#con2 = tree.calls > (50*parentCalls)
	#con2 = tree.calls > (10*parentCalls)
	con2 = tree.calls > (3*parentCalls)
	if con1 and con2:
		return tree
	elif len(tree.leaves) > 0:
		for l in tree.leaves:
			r = findSolverNode(l, tree.calls, walltime)
			if not r is None:
				return r
	return None

def findTreeNodeByType(tree, typeName):
	if tree.typeName == typeName:
		return tree
	else:
		for l in tree.leaves:
			r = findTreeNodeByType(l, typeName)
			if not r is None:
				return r
	return None

def findTreeNodeByName(tree, name):
	if tree.name == name:
		return tree
	else:
		for l in tree.leaves:
			r = findTreeNodeByName(l, name)
			if not r is None:
				return r
	return None

def chartCallPath(tree, title, filename):
	if args.charts == "polar":
		plotType = PlotType.Polar
	elif args.charts == "vertical":
		plotType = PlotType.Vertical
	else:
		plotType = PlotType.Horizontal
	fig = plt.figure(figsize=fig_dims)
	plotCallPath_root(tree, plotType)
	fig.suptitle(title)
	chart_dirpath = os.path.join(args.tt_results_dirpath, "charts", plotTypeToString[plotType])
	if not os.path.isdir(chart_dirpath):
		os.makedirs(chart_dirpath)
	chart_filepath = os.path.join(chart_dirpath, filename)
	plt.savefig(chart_filepath)
	plt.close(fig)

def plotCallPath_root(tree, plotType):
	if plotType == PlotType.Polar:
		root_total = np.pi * 2
		ax = plt.subplot(111, projection='polar')
	else:
		root_total = tree.time
		ax = plt.subplot(111)

	plotCallPath(tree, root_total, root_total, 0, 0, ax, plotType)

def plotCallPath(tree, root_total, node_total, offset, level, ax, plotType):
	are_any_nodes_labelled = False
	if level == 0:
		if not isinstance(tree, CallTreeNode):
			raise Exception("'tree' parameter at root must be a CallTreeNode")
		label = tree.name
		methodType = tree.typeName
		value = tree.time
		subnodes = tree.leaves

		are_any_subnodes_labelled = plotCallPath(subnodes, root_total, value, 0, level+1, ax, plotType)
		do_label_node = args.label and not are_any_subnodes_labelled

		are_any_nodes_labelled = are_any_nodes_labelled or do_label_node

		if plotType == PlotType.Polar:
			ax.bar(x=[0], height=[0.5], width=[node_total], color=[methodTypeToColour[methodType]])
			if do_label_node:
				ax.text(0, 0, label, ha='center', va='center')
		else:
			if plotType == PlotType.Vertical:
				ax.bar(x=[node_total/2], height=[1.0], width=[node_total], color=[methodTypeToColour[methodType]])
				if do_label_node:
					ax.text(root_total/2, 0.5, label, ha='center', va='center')
			else:
				ax.bar(x=[0.5], height=[node_total], width=[1.0], color=[methodTypeToColour[methodType]])
				if do_label_node:
					ax.text(0.5, root_total/2, label, ha='center', va='center')

	elif tree:
		if not isinstance(tree, list):
			raise Exception("non-root 'tree' must be a list of CallTreeNode")
		for n in tree:
			if not isinstance(n, CallTreeNode):
				raise Exception("non-root 'tree' must be a list of CallTreeNode")

		if plotType == PlotType.Polar:
			d = root_total / node_total
		else:
			d = root_total / node_total
		sizes = []
		labels = []
		colours = []
		subnode_offset = offset
		for t in tree:
			label = t.name
			methodType = t.typeName
			value = t.time
			subnodes = t.leaves

			are_any_subnodes_labelled = plotCallPath(subnodes, root_total, node_total, subnode_offset, level+1, ax, plotType)
			subnode_offset += value

			## Logic to ensure only outermost and non-tiny pie elements are labelled.
			do_label_node = args.label and not are_any_subnodes_labelled
			## No label text for tiny segments:
			if (value / node_total) < 0.01:
				do_label_node = False
			are_any_nodes_labelled = are_any_nodes_labelled or do_label_node or are_any_subnodes_labelled

			if do_label_node:
				labels.append(label)
			else:
				labels.append("")
			sizes.append(value * d)
			colours.append(methodTypeToColour[methodType])
		if plotType == PlotType.Polar:
			widths = sizes
			heights = [1] * len(sizes)
			lefts = np.cumsum([offset * d] + widths[:-1])
			bottoms = np.zeros(len(sizes)) + level - 0.5
		else:
			if plotType == PlotType.Vertical:
				heights = [1] * len(sizes)
				widths = sizes
				bottoms = np.zeros(len(sizes)) + level
				lefts = np.cumsum([offset * d] + widths[:-1])
			else:
				widths = [1] * len(sizes)
				heights = sizes
				lefts = np.zeros(len(sizes)) + level
				bottoms = np.cumsum([offset * d] + heights[:-1])
		rects = ax.bar(x=lefts, height=heights, width=widths, bottom=bottoms, linewidth=1,
					   color=colours, edgecolor='white', align='edge')
		for rect, label in zip(rects, labels):
			x = rect.get_x() + rect.get_width() / 2
			y = rect.get_y() + rect.get_height() / 2
			if plotType == PlotType.Polar:
				do_rotate_text = True
				if do_rotate_text:
					if level < 2:
						rotation = ((360 - np.degrees(x) % 180)) % 360
					else:
						rotation = (90 + (360 - np.degrees(x) % 180)) % 360
					if label != "" :
						ax.text(x, y, label, rotation=rotation, ha='center', va='center')
				else:
					if label != "" :
						ax.text(x, y, label, ha='center', va='center')
			else:
				if label != "" :
					ax.text(x, y, label, ha='center', va='center')

	if level == 0:
		if plotType == PlotType.Polar:
			ax.set_theta_direction(-1)
			ax.set_theta_zero_location('N')
			ax.set_axis_off()
		else:
			if plotType == PlotType.Vertical:
				ax.get_yaxis().set_visible(False)
			else:
				ax.get_xaxis().set_visible(False)

		## Build custom colour legend:
		legend_dict = { "MPI sync" : methodTypeToColour["MPISyncCall"], 
		                "MPI comm" : methodTypeToColour["MPICommCall"], 
		                "MPI collectives" : methodTypeToColour["MPICollectiveCall"], 
		                "compute" : methodTypeToColour["Compute"],
		                "library call" : methodTypeToColour["LibraryCall"] }
		patchList = []
		for k in legend_dict:
			patchList.append(mpatches.Patch(color=legend_dict[k], label=k))
		ax.legend(handles=patchList)

	return are_any_nodes_labelled	

def getNodeAggregatedStats(callPathID, runID, processID, db):
	# ToDo: If looping over nodes, this will lead to the same data being loaded from the database multiple
	# times, which is likely expensive.
	# Options: Load full data from database into tree in memory (might be v. large)
	#		  Ensure only depth-first traversal that can reuse data from prior loads

	db.row_factory = sqlite3.Row
	cur = db.cursor()

	# Get Node Aggregate Details - Should only ever have one entry
	query = "SELECT D.NodeName AS Name, " + \
			"T.TypeName AS TypeName, " + \
			"A.MinWallTime AS AggMinTime, A.AvgWallTime AS AggAvgTime, A.MaxWallTime AS AggMaxTime, A.Count AS CallCount " + \
			"FROM AggregateTime AS A " + \
			"NATURAL JOIN CallPathData AS C NATURAL JOIN ProfileNodeData AS D NATURAL JOIN ProfileNodeType AS T " + \
			"WHERE A.CallPathID = {0} AND A.RunID = {1} AND A.ProcessID = {2} ;".format(callPathID, runID, processID)

	cur.execute(query)
	result = cur.fetchone()
	if result is None:
		print(query)
		raise Exception("No node with CallPathID {0}".format(callPathID))
	nodeStats = {k:result[k] for k in result.keys()}
	nodeStats['AggTotalTimeI'] = nodeStats['AggAvgTime'] * nodeStats['CallCount']

	# === Get Times for any direct children of this node ===
	childIDs = getNodeChildrenIDs(db, callPathID)
	if(len(childIDs) == 0):
		# No child nodes means exclusive time = inclusive time
		nodeStats['AggTotalTimeE'] = nodeStats['AggTotalTimeI']

	else:
		# Get sum of inclusive times taken by child nodes
		childIDStr = ','.join([str(x) for x in childIDs])
		query = "SELECT SUM(AvgWallTime * Count) AS ChildrenWalltime FROM AggregateTime " + \
			  "WHERE RunID = {0} ".format(processID) + \
			  "AND CallPathID IN ({0}) ;".format(childIDStr)
		cur.execute(query);
		childrenWalltime = cur.fetchone()['ChildrenWalltime']

		if nodeStats["CallCount"] == 0:
			nodeStats['AggTotalTimeE'] = 0.0
			nodeStats['AggTotalTimeI'] = childrenWalltime
		else:
			nodeStats['AggTotalTimeE'] = nodeStats['AggTotalTimeI'] - childrenWalltime
			# Due to overhead, potentially possible for this to be slightly negative, so will correct here
			if nodeStats['AggTotalTimeE'] < 0.0:
				print("Note: negative 'AggTotalTimeE' detected for nodeName '{0}', zeroing ({1})".format(nodeStats["Name"], nodeStats['AggTotalTimeE']))
				nodeStats['AggTotalTimeE'] = 0.0

	return nodeStats

def aggregateTimesByType(runID, processID, db):
	# # Get all callpath node IDs
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	query = "SELECT DISTINCT CallPathID FROM AggregateTime NATURAL JOIN CallPathData WHERE RunID = {0};".format(runID)
	cur.execute(query)
	result = cur.fetchall()
	callpathIDs = [row['CallPathID'] for row in result]

	# For each callpath node, get the aggregate details
	col_names = ["Name", "TypeName", "CallCount", "AggTotalTimeI", "AggTotalTimeE", "AggAvgTime", "AggMaxTime", "AggMinTime"]
	df = None
	df_cols = {c:[] for c in col_names}
	for callpathID in callpathIDs:
		r = getNodeAggregatedStats(callpathID, runID, processID, db)
		for k in col_names:
			df_cols[k].append(r[k])
	df = pd.DataFrame(data=df_cols, columns=col_names)

	walltime = df[df['Name']=='ProgramRoot'].loc[0,'AggTotalTimeI']

	# Sum times across method types
	df_sum = df[['TypeName', 'AggTotalTimeE']].groupby('TypeName').sum().reset_index()
	df_sum = df_sum.rename(columns={'TypeName':'Method type', 'AggTotalTimeE':'Exclusive time'})
	df_sum['Exclusive time %'] = df_sum['Exclusive time'] / walltime
	df_sum = df_sum.sort_values("Exclusive time", ascending=False)

	return(df_sum)

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

	## Query to join the above two queries into one, bringing together inclusive waltims of each node and its children. Seems to work.
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
	cid = getNodeCallpathId(db, nodeName)
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

		cid = getNodeCallpathId(db, nodeOfInterestName)
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
	df.loc[df["TypeName"].isin(["Method", "Loop", "Compute", "Block", "TraceConductor"]), "Type"] = "Compute"
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
	qCountQuery = "SELECT COUNT(*) FROM {0} NATURAL JOIN CallPathData WHERE ParamName = \"{1}\" AND NodeEntryID >= {2} AND NodeExitID <= {3} GROUP BY ParentNodeID".format(paramTable, paramName, nodeEntryId, nodeExitId)
	cur.execute(qCountQuery)
	res = cur.fetchone()
	if res is None:
		return None
	count = res[0]
	if count > 1:
		raise Exception("Parameter '{0}' recorded multiple values between a specific nodeID range. This situation has not been coded in TreeTimer, contact developers to request average, variance, or some other aggregating function.")

	query = "SELECT {0} AS TraceParamId, ParamValue FROM {1} NATURAL JOIN CallPathData WHERE ParamName = \"{2}\" AND NodeEntryID >= {3} AND NodeExitID <= {4} GROUP BY ParentNodeID".format(traceParamIdColMap[paramTable], paramTable, paramName, nodeEntryId, nodeExitId)
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
		raise Exception("ParamName {0} not found in any TraceParameter* table".format(paramName))

	query = "SELECT COUNT(*) FROM TraceParameterBoolData WHERE ParamName = \"TraceConductorEnabled?\";"
	cur.execute(query)
	count = cur.fetchone()[0]
	if count == 0:
		raise Exception("TreeTimer has not written parameter 'TraceConductorEnabled?' to table TraceParameterBoolData, which is necessary for grouping trace parameter by ")

	cid = getNodeCallpathId(db, nodeName)
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

def traceTimes_chartDynamicLoadBalance(traces_df, filename_suffix=None):
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
	fig_filepath = os.path.join(args.tt_results_dirpath, fig_filename)
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
	fig_filepath = os.path.join(args.tt_results_dirpath, fig_filename)
	plot_heatmap(diff_df, "TimestepIndex", "(MPI %) diff", fig_filepath)

	## Sum stdev across ranks
	diffSum_df = diff_df.drop("Rank", axis=1).groupby("TimestepIndex").sum().reset_index()
	diffSum_df = diffSum_df.rename(columns={"(MPI %) diff":"Sum MPI % diff"})
	diffSum_stdev = diffSum_df["Sum MPI % diff"].std()
	diffSum_mean = diffSum_df["Sum MPI % diff"].mean()
	diffSum_stdev_pct = 0.0 if diffSum_mean == 0.0 else diffSum_stdev/diffSum_mean*100.0
	print("diffSum_stdev % mean = {0:.1f}".format(diffSum_stdev_pct))

def traceParameter_chart(traceParam_df, paramName):
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
	fig_filepath = os.path.join(args.tt_results_dirpath, fig_filename)
	plot_heatmap(traceParam_df, "TimestepIndex", "Value", fig_filepath)


if __name__ == "__main__":
	main()

