from copy import deepcopy
import os
import sqlite3

import imp
imp.load_source("PostProcessDbUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-db-utils.py"))
from PostProcessDbUtils import *
imp.load_source("PostProcessAggUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-aggregateTime-utils.py"))
from PostProcessAggUtils import *


class CallTreeNodeIterator:
	def __init__(self, callTreeNode, am_root=False):
		self.callTreeNode = callTreeNode
		self.callTreeNodeLeaves = sorted(callTreeNode.leaves)
		self.currentLeafIter = None
		self.index = -1
		self.am_root = am_root

	def __next__(self):
		raise Exception("Entered CallTreeNodeIterator.__next__(), apparently it is used.")
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
	def __init__(self, dbID, name, typeName, time, calls):
		self.dbID = dbID
		self.name = name
		self.typeName = typeName
		self.time = time
		self.calls = calls
		self.leaves = []

	def addLeaf(self, leaf):
		self.leaves.append(leaf)

	def findTreeNodeByType(self, typeName):
		if self.typeName == typeName:
			return self
		else:
			for l in self.leaves:
				r = l.findTreeNodeByType(typeName)
				if not r is None:
					return r
		return None

	def findTreeNodeByName(self, name):
		if self.name == name:
			return self
		else:
			for l in self.leaves:
				r = l.findTreeNodeByName(name)
				if not r is None:
					return r
		return None

	def findSolverNode(self, parentCalls, walltime):
		con1 = self.time > (0.7*walltime)
		#con2 = self.calls > (50*parentCalls)
		#con2 = self.calls > (10*parentCalls)
		con2 = self.calls > (3*parentCalls)
		if con1 and con2:
			return self
		elif len(self.leaves) > 0:
			for l in self.leaves:
				r = l.findSolverNode(self.calls, walltime)
				if not r is None:
					return r
		return None

	def getAllChildrenMembers(self, member):
		validMembers = ["dbID", "name", "typeName", "time", "calls"]
		if not member in validMembers:
			raise Exception("member '{0}' not in valid members {1}".format(member, validMembers))
		if member == "dbID":
			val = self.dbID
		elif member == "name":
			val = self.name
		elif member == "typeName":
			val = self.typeName
		elif member == "time":
			val = self.time
		elif member == "calls":
			val = self.calls
		else:
			raise Exception("member '{0}' not recognised".format(member))

		if len(self.leaves) == 0:
			return [val]
		else:
			vals = [val]
			for l in self.leaves:
				vals += l.getAllChildrenMembers(member)
			return vals

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
		raise Exception("Entered CallTreeNode.__iter__(), apparently it is used.")
		return CallTreeNodeIterator(self, self.name=="ProgramRoot")

class CallTreeNodeAggregated:
	def __init__(self, node, rank):
		if not isinstance(node, CallTreeNode):
			raise Exception("Must initialise CallTreeNodeAggregated with a CallTreeNode")

		self.name = node.name
		self.typeName = node.typeName
		self.dbIDs = [node.dbID]
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

		self.dbIDs.append(node.dbID)
		self.times.append(node.time)
		self.calls.append(node.calls)
		self.ranks.append(rank)
		nodeLeavesSorted = sorted(node.leaves)
		for idx in range(len(self.leaves)):
			self.leaves[idx].appendNodeElementwise(nodeLeavesSorted[idx], rank)

	def sumElementwise(self):
		timesSum = sum(self.times)
		sumNode = CallTreeNode(self.dbIDs[0], self.name, self.typeName, timesSum, None)
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

def buildCallPathTree(db, runID, processID):
	rootID = getNodeCallpathId(db, processID, 'ProgramRoot')
	tree = None
	x = buildCallPathNodeTraversal(db, runID, processID,tree, rootID, 0)
	return x

def getNodeCallStats(callPathID, db):
	## Lean version of getNodeAggregatedTimeStats(), for when AggregateTime table is empty.

	## Function call tree is assumed invariant to runID or processID, i.e. application code
	## does not change significantly between runs

	db.row_factory = sqlite3.Row
	cur = db.cursor()

	# Get Node Details - Should only ever have one entry
	query = "SELECT D.NodeName AS Name, " + \
			"C.CallPathID AS CallPathID, " + \
			"T.TypeName AS TypeName " + \
			"FROM CallPathData AS C " + \
			"NATURAL JOIN ProfileNodeData AS D NATURAL JOIN ProfileNodeType AS T " + \
			"WHERE C.CallPathID = {0} ;".format(callPathID)

	cur.execute(query)
	result = cur.fetchone()
	if result is None:
		print(query)
		raise Exception("No node with CallPathID {0}".format(callPathID))
	nodeStats = {k:result[k] for k in result.keys()}

	return nodeStats

def buildCallPathNodeTraversal(db, runID, processID, treeNode, nodeID, indentLevel):
	# Recursive depth-first traversal through call stack
	if table_empty(db, "AggregateTime"):
		record = getNodeCallStats(nodeID, db)
		leaf = CallTreeNode(record["CallPathID"], record["Name"], record["TypeName"], 0.0, -1)
	else:
		record = getNodeAggregatedTimeStats(db, runID, processID, nodeID)
		if not record is None:
			leaf = CallTreeNode(record["CallPathID"], record["Name"], record["TypeName"], record["AggTotalTimeI"], record["CallCount"])
		else:
			record = getNodeCallStats(nodeID, db)
			leaf = CallTreeNode(record["CallPathID"], record["Name"], record["TypeName"], 0.0, -1)

	am_root = False
	if treeNode is None:
		am_root = True
		treeNode = leaf
	else:
		treeNode.addLeaf(leaf)

	childNodes = getNodeChildrenIDs(db, processID, nodeID)
	for childID in childNodes:
		buildCallPathNodeTraversal(db, runID, processID, leaf, childID, indentLevel + 1)

	if am_root:
		return treeNode


