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

import os, re
import pickle
from pprint import pprint
from copy import deepcopy

import sqlite3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--tt-results-dirpath', required=True, help="Dirpath to 'tt_results' folder")
parser.add_argument('-c', '--charts', choices=["polar", "horizontal", "vertical"], help="Chart the call stack runtime breakdown across ranks")
parser.add_argument('-r', '--chart-ranks', action='store_true', help="If charting, also draw call stack chart for each rank. Expensive!")
parser.add_argument('-l', '--label', action='store_true', help="Add labels to chart elements. Optional because can create clutter")
args = parser.parse_args()


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

def main():
	tt_folder_dirpath = args.tt_results_dirpath
	if not os.path.isdir(tt_folder_dirpath):
		raise Exception("Folder does not exist: {0}".format(tt_folder_dirpath))

	df_all_raw = None
	df_all_aggregated = None

	rank_ids = set()

	## Group together rank call traces that have identical topology:
	groupedCallTrees = None

	num_dbs = 0
	for run_root, run_dirnames, run_filenames in os.walk(tt_folder_dirpath):
		for f in run_filenames:
			m = re.match("^results\.([0-9]+)\.db$", f)
			if m:
				num_dbs += 1

	ctr = 0
	for run_root, run_dirnames, run_filenames in os.walk(tt_folder_dirpath):
		for f in run_filenames:
			m = re.match("^results\.([0-9]+)\.db$", f)
			if m:
				ctr += 1
				#if ctr > 1:
				#	break
				db_fp = os.path.join(tt_folder_dirpath, f)
				print("Processing: {0} ({1}/{2})".format(db_fp, ctr, num_dbs))
				rank = int(m.groups()[0])
				rank_ids.add(rank)

				cache_dp = os.path.join(tt_folder_dirpath, "_tt_cache")
				if not os.path.isdir(cache_dp):
					os.mkdir(cache_dp)

				db = sqlite3.connect(db_fp)
				## Loading DB into memory roughly halves 
				## query times. But only load if 
				## access required
				dbm = None

				df_csv = os.path.join(cache_dp, f+".csv")
				if os.path.isfile(df_csv):
					df = pd.read_csv(df_csv)
				else:
					#df = read_db_timings(db_fp)
					if dbm is None:
						dbm = sqlite3.connect(':memory:')
						db.backup(dbm)
					df = loadHotspotsAggregateTime(1, 1, dbm)
					df["rank"] = rank
					df.to_csv(df_csv, index=False)
				if df_all_raw is None:
					df_all_raw = df
				else:
					df_all_raw = df_all_raw.append(df)

				df_agg_fp = os.path.join(cache_dp, f+".typeAgg.csv")
				if os.path.isfile(df_agg_fp):
					df_agg = pd.read_csv(df_agg_fp)
				else:
					if dbm is None:
						dbm = sqlite3.connect(':memory:')
						db.backup(dbm)
					df_agg = aggregateTimesByType(1, 1, dbm)
					df_agg["rank"] = rank
					df_agg.to_csv(df_agg_fp, index=False)
				if df_all_aggregated is None:
					df_all_aggregated = df_agg
				else:
					df_all_aggregated = df_all_aggregated.append(df_agg)

				tree_fp = os.path.join(cache_dp, f+".call-tree.pkl")
				if os.path.isfile(tree_fp):
					with open(tree_fp, 'rb') as input:
						t = pickle.load(input)
				else:
					#print(" - constructing call tree ...")
					if dbm is None:
						dbm = sqlite3.connect(':memory:')
						db.backup(dbm)
					t = buildCallPathTree(1, 1, dbm)
					with open(tree_fp, 'wb') as output:
						pickle.dump(t, output, pickle.HIGHEST_PROTOCOL)

				if rank == 0:
					# print(t)
					particlesLoopTree = None
					particlesLoopTree = buildCallPathNodeTraversal(1, 1, db, particlesLoopTree, 3, 0)
					print(particlesLoopTree)
					quit()
					for l in t.leaves:
						if l.name == "presol":
							print(l)
					quit()

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
						print(" - drawing chart ...")
						if args.charts == "polar":
							plotType = PlotType.Polar
						elif args.charts == "vertical":
							plotType = PlotType.Vertical
						else:
							plotType = PlotType.Horizontal
	
						fig = plt.figure(figsize=fig_dims)
						plotCallPath_root(t, plotType)
	
						fig.suptitle("Call stack times of rank " + str(rank))
						chart_dirpath = os.path.join(tt_folder_dirpath, "charts", plotTypeToString[plotType])
						if not os.path.isdir(chart_dirpath):
							os.makedirs(chart_dirpath)
						chart_filepath = os.path.join(chart_dirpath, "rank-{0}.png".format(rank))
						plt.savefig(chart_filepath)
						plt.close(fig)

	if not args.charts is None:
		## Aggregate together call trees within each group, and create plots:
		print("Drawing group charts")
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

			if args.charts == "polar":
				plotType = PlotType.Polar
			elif args.charts == "vertical":
				plotType = PlotType.Vertical
			else:
				plotType = PlotType.Horizontal

			fig = plt.figure(figsize=fig_dims)
			plotCallPath_root(aggSum, plotType)

			fig.suptitle("Call stack times summed across ranks " + sorted(agg.ranks).__str__())
			chart_dirpath = os.path.join(tt_folder_dirpath, "charts", plotTypeToString[plotType])
			if not os.path.isdir(chart_dirpath):
				os.makedirs(chart_dirpath)
			chart_filepath = os.path.join(chart_dirpath, "rankGroup{0}.png".format(gn))
			plt.savefig(chart_filepath)
			plt.close(fig)


	print("Writing out collated CSVs")
	df_all_raw["num_ranks"] = len(rank_ids)
	df_all_raw.sort_values(["Name", "rank"], inplace=True)
	df_filename = os.path.join(tt_folder_dirpath, "timings_raw.csv")
	df_all_raw.to_csv(df_filename, index=False)
	print("Collated raw data written to '{0}'".format(df_filename))


	df_all_aggregated.sort_values(["Method type", "rank"], inplace=True)
	df_filename = os.path.join(tt_folder_dirpath, "timings_aggregated.csv")
	df_all_aggregated.to_csv(df_filename, index=False)
	print("Collated aggregated data written to '{0}'".format(df_filename))


# Note - RunID is the SQL Key for the RunID, and processID is the SQL Key ProcessID in the db, not the process rank! (They may not be the same)
def loadHotspotsAggregateTime(runID, processID, db):

	# Get Root Details
	rootID = getRootCallPathID(db)
	rootRecord = getNodeAggregatedStats(rootID, runID, processID, db)

	records = getAllProfileNodesAggregateTimeExclusive(runID, processID, db)

	col_names = ["Name", "TypeName", "CallCount", "InclusiveAggregateTime(s)", "ExclusiveAggregateTime(s)", "ExclusiveAggregateTime(% of Run)"]
	df = None

	df_cols = {c:[] for c in col_names}

	sum = 0.0
	nonfunctionsum = 0.0
	for r in records:
		df_cols["Name"].append(r['Name'])
		df_cols["TypeName"].append(r['TypeName'])
		df_cols["CallCount"].append(r['CallCount'])
		df_cols["InclusiveAggregateTime(s)"].append(r['AggTotalTimeI'])
		df_cols["ExclusiveAggregateTime(s)"].append(r['AggTotalTimeE'])
		df_cols["ExclusiveAggregateTime(% of Run)"].append(r['AggTotalTimeE']/rootRecord['AggTotalTimeI'])

		sum = sum + r['AggTotalTimeE']

		if(r['TypeName'] != 'Method'):
			nonfunctionsum = nonfunctionsum + r['AggTotalTimeE']
	df = pd.DataFrame(data=df_cols, columns=col_names)

	return df

# Note - RunID is the SQL Key for the RunID, and processID is the SQL Key ProcessID in the db, not the process rank! (They may not be the same)
def getAllProfileNodesAggregateTimeExclusive(runID, processID, db):

	# Since aggregate properties operate on call path nodes (since
	# times for different paths to the same profile node are stored
	# separately), we need to merge these values.

	# First, identify the call path nodes associated with a run + process
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	cmd = "SELECT DISTINCT CallPathID FROM AggregateTime " + \
		  "WHERE RunID = " + str(runID) + " AND " + \
		  "ProcessID = " + str(processID)

	cur.execute(cmd)
	result = cur.fetchall()

	# Get Aggregate Data
	records = []

	for callPaths in result:
		data = getNodeAggregatedStats(callPaths['CallPathID'], runID, processID, db)
		data['CallPathID'] = callPaths['CallPathID']
		records.append(data)

	# Sort on profile node name - would ideally like to do this in database,
	# but we need to compute the exclusive time
	sortedRecords = sorted(records, key=lambda k: k['Name'])

	# Group the values together
	groupRecords = []
	group = ''
	count = -1
	for i in sortedRecords:
		if(group == "" or group != i['Name']):
			# Add record to list
			groupRecords.append(i)
			group = i['Name']
			count = count + 1
		else:
			# A record with the same name exists, append the values onto this one
			groupRecords[count]['AggMinTime'] += i['AggMinTime']
			groupRecords[count]['AggAvgTime'] += i['AggAvgTime']
			groupRecords[count]['AggMaxTime'] += i['AggMaxTime']
			groupRecords[count]['CallCount'] += i['CallCount']
			groupRecords[count]['AggTotalTimeI'] += i['AggTotalTimeI']
			groupRecords[count]['AggTotalTimeE'] += i['AggTotalTimeE']

	# We now have a list of nodes with the grouped values
	# Return the list
	return groupRecords

# Get the CallPathID of the root walltime node
# Note: This is the only special case, other Call Path Nodes may appear
# multiple times for the same profile node, since they could have different parent
# nodes for a different run
# Since root has no parent nodes, this should only ever appear once and is likely
# ID 1, but don't wish to assume
# This is also the only profile/callpath node that is guaranteed to exist
def getRootCallPathID(db):
	db.row_factory = sqlite3.Row
	cur = db.cursor()

	cmd = "SELECT CallPathID FROM CallPathData NATURAL JOIN ProfileNodeData " + \
		  "WHERE NodeName LIKE 'ProgramRoot'"

	cur.execute(cmd)
	result = cur.fetchone()

	return result['CallPathID']



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

	def __str__(self, indent=0):
		nodeStr = ""
		if indent > 0:
			#nodeStr += "  :"*(indent-1) + "  |" + "-"
			nodeStr += " :"*(indent-1) + " |" + "-"
		else:
			nodeStr += " "
		#nodeStr += "{0} [{1}] - {2:.2f} seconds\n".format(self.name, self.typeName, self.time)
		detailsStr = self.name.ljust(60-indent*2)
		detailsStr += " - {0:.2f}s".format(self.time)
		#detailsStr += " {0} calls".format(self.calls)
		if self.calls == 1:
			detailsStr += ", x{0} call".format(self.calls)
		else:
			detailsStr += ", x{0} calls".format(self.calls)
		detailsStr += " [{0}]".format(self.typeName).ljust(12)
		nodeStr += detailsStr + "\n"
		for c in self.leaves:
			cStr = c.__str__(indent+1)
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
	rootID = getRootCallPathID(db)
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

	childNodes = getNodeChildrenIDs(nodeID, db)
	for childID in childNodes:
		buildCallPathNodeTraversal(runID, processID, db, leaf, childID, indentLevel + 1)

	if am_root:
		return treeNode

def getNodeChildrenIDs(callPathID, db):
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT CallPathID FROM CallPathData WHERE ParentNodeID = {0}".format(callPathID)
	cur.execute(cmd)
	result = cur.fetchall()
	return [i['CallPathID'] for i in result]

def plotCallPath_root(tree, plotType):
	if plotType == PlotType.Polar:
		root_total = np.pi * 2
		ax = plt.subplot(111, projection='polar')
	else:
		root_total = tree.time
		ax = plt.subplot(111)

	plotCallPath(tree, root_total, root_total, 0, 0, ax, plotType)

def plotCallPath(tree, root_total, node_total, offset, level, ax, plotType):
	if level == 0:
		if not isinstance(tree, CallTreeNode):
			raise Exception("'tree' parameter at root must be a CallTreeNode")
		label = tree.name
		methodType = tree.typeName
		value = tree.time
		subnodes = tree.leaves

		if plotType == PlotType.Polar:
			ax.bar(x=[0], height=[0.5], width=[node_total], color=[methodTypeToColour[methodType]])
			if args.label:
				ax.text(0, 0, label, ha='center', va='center')
		else:
			if plotType == PlotType.Vertical:
				ax.bar(x=[node_total/2], height=[1.0], width=[node_total], color=[methodTypeToColour[methodType]])
				if args.label:
					ax.text(root_total/2, 0.5, label, ha='center', va='center')
			else:
				ax.bar(x=[0.5], height=[node_total], width=[1.0], color=[methodTypeToColour[methodType]])
				if args.label:
					ax.text(0.5, root_total/2, label, ha='center', va='center')

		plotCallPath(subnodes, root_total, value, 0, level+1, ax, plotType)
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

			if (value / node_total) < 0.01:
				## No label text for tiny segments
				labels.append("")
			else:
				labels.append(label)
			sizes.append(value * d)
			colours.append(methodTypeToColour[methodType])
			plotCallPath(subnodes, root_total, node_total, subnode_offset, level+1, ax, plotType)
			subnode_offset += value
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
					if args.label:
						ax.text(x, y, label, rotation=rotation, ha='center', va='center')
				else:
					if args.label:
						ax.text(x, y, label, ha='center', va='center')
			else:
				if args.label:
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


def getNodeAggregatedStats(callPathID, runID, processID, db):
	# This method retrieves the following aggregate details about a node and returns them as a dictionary
	# (1) Node Name: Key 'Name'
	# (2) Node Type Name: Key 'TypeName'
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
	#		  Ensure only depth-first traversal that can reuse data from prior loads

	db.row_factory = sqlite3.Row
	cur = db.cursor()

	# Get Node Aggregate Details - Should only ever have one entry
	cmd = "SELECT D.NodeName AS Name, " + \
			"T.TypeName AS TypeName, " + \
			"A.MinWallTime AS AggMinTime, A.AvgWallTime AS AggAvgTime, A.MaxWallTime AS AggMaxTime, A.Count AS CallCount " + \
			"FROM AggregateTime AS A " + \
			"NATURAL JOIN CallPathData AS C NATURAL JOIN ProfileNodeData AS D NATURAL JOIN ProfileNodeType AS T " + \
			"WHERE A.CallPathID = {0} AND A.RunID = {1} AND A.ProcessID = {2} ;".format(callPathID, runID, processID)

	cur.execute(cmd)
	result = cur.fetchone()

	rDict = {k:result[k] for k in result.keys()}
	rDict['AggTotalTimeI'] = rDict['AggAvgTime'] * rDict['CallCount']

	# === Get Times for any direct children of this node ===
	childIDs = getNodeChildrenIDs(callPathID, db)
	if(len(childIDs) == 0):
		# No child nodes means exclusive time = inclusive time
		rDict['AggTotalTimeE'] = rDict['AggTotalTimeI']
	else:
		# Get sum of inclusive times taken by child nodes
		childIDStr = ','.join([str(x) for x in childIDs])
		cmd = "SELECT SUM(AvgWallTime * Count) AS ChildTimeSum FROM AggregateTime " + \
			  "WHERE RunID = {0} ".format(processID) + \
			  "AND CallPathID IN ({0}) ;".format(childIDStr)
		cur.execute(cmd);
		result = cur.fetchone()

		rDict['AggTotalTimeE'] = rDict['AggTotalTimeI'] - result['ChildTimeSum']
		# Due to overhead, potentially possible for this to be slightly negative, so will correct here
		if(rDict['AggTotalTimeE'] < 0.0):
			print("Note: negative 'AggTotalTimeE' detected, zeroing")
			rDict['AggTotalTimeE'] = 0.0

	return rDict

def aggregateTimesByType(runID, processID, db):
	# Get all callpath node IDs
	db.row_factory = sqlite3.Row
	cur = db.cursor()
	cmd = "SELECT DISTINCT CallPathID FROM AggregateTime NATURAL JOIN CallPathData WHERE RunID = {0};".format(runID)
	cur.execute(cmd)
	result = cur.fetchall()

	# For each callpath node, get the aggregate details
	records = [getNodeAggregatedStats(row['CallPathID'], runID, processID, db) for row in result]

	# Get the time for the root node
	rootID = getRootCallPathID(db)
	rootProp = getNodeAggregatedStats(rootID, runID, processID, db)
	walltime = rootProp['AggTotalTimeI']

	# Sum times across method types
	typeExclusiveTimes = {}
	for r in records:
		t = r['TypeName']
		if not t in typeExclusiveTimes.keys():
			typeExclusiveTimes[t] = r['AggTotalTimeE']
		else:
			typeExclusiveTimes[t] += r['AggTotalTimeE']

	# Pack into a Pandas dataframe
	methodTypes = list(typeExclusiveTimes.keys())
	df_init = {'Method type':methodTypes, 
			   'Exclusive time':[typeExclusiveTimes[t] for t in methodTypes],
			   'Exclusive time %':[typeExclusiveTimes[t]/walltime for t in methodTypes]}
	df = pd.DataFrame(data=df_init)
	df.sort_values("Exclusive time", ascending=False, inplace=True)
	return(df)

if __name__ == "__main__":
	main()

