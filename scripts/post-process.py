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
from time import sleep

import sqlite3

import pickle

import imp
imp.load_source("PostProcessDbUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-db-utils.py"))
from PostProcessDbUtils import *
imp.load_source("PostProcessCallTreeUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-callTree-utils.py"))
from PostProcessCallTreeUtils import *
imp.load_source("PostProcessAggUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-aggregateTime-utils.py"))
from PostProcessAggUtils import *
imp.load_source("PostProcessTraceUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-traceTime-utils.py"))
from PostProcessTraceUtils import *
imp.load_source("PostProcessPlotUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-plot-utils.py"))
from PostProcessPlotUtils import *

verbose = False
#verbose = True

import pandas as pd
import numpy as np

import tqdm

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-d', '--tt-results-dirpath', help="Dirpath to 'tt_results' folder")
parser.add_argument('-m', '--parallel', action='store_true', help="Process multiple DB files in parallel")
parser.add_argument('-b', '--db-filepath', help="Analyse this specific database file")
parser.add_argument('-t', '--trace-group-focus', help='When grouping traces by loop iterator, can filter by a block within loop')
parser.add_argument('-p', '--parameter', help='Which trace parameter to process')
parser.add_argument('-c', '--charts', choices=["polar", "horizontal", "vertical"], help="Chart the call stack runtime breakdown across ranks")
parser.add_argument('-r', '--chart-ranks', action='store_true', help="If charting, also draw call stack chart for each rank. Expensive!")
parser.add_argument('-l', '--label', action='store_true', help="Add labels to chart elements. Optional because can create clutter")
args = parser.parse_args()

if args.parallel:
	from multiprocessing import Pool, current_process, cpu_count

tt_folder_dirpath = args.tt_results_dirpath
tt_db_filepath = args.db_filepath
if (tt_folder_dirpath is None) and (tt_db_filepath is None):
	raise Exception("Must specify either folder or DB file to analyse.")
if (not tt_folder_dirpath is None) and (not tt_db_filepath is None):
	raise Exception("Don't specify folder and DB file. Just one.")

if args.charts == "polar":
	plotType = PlotType.Polar
elif args.charts == "vertical":
	plotType = PlotType.Vertical
else:
	plotType = PlotType.Horizontal



def preprocess_db(db_fp, ctr, num_dbs, cache_dp):
	if verbose: print("Processing: {0} ({1}/{2})".format(db_fp, ctr, num_dbs))

	## For now just support single-run databases:
	runID = 1

	db = sqlite3.connect(db_fp)
	## Loading DB into memory roughly halves query times. But only lazy-load.
	dbm = None

	f = os.path.basename(db_fp)

	trees_fp = os.path.join(cache_dp, f+".call-trees.pkl")
	trees = None
	if not os.path.isfile(trees_fp):
		if verbose: print("- generating: " + trees_fp)
		if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
		trees = {}
		for processID in getProcessIds(dbm):
			rank = getMpiRank(dbm, processID)
			trees[rank] = buildCallPathTree(dbm, runID, processID)
		with open(trees_fp, 'wb') as output:
			#pickle.dump(t, output, pickle.HIGHEST_PROTOCOL)
			pickle.dump(trees, output, 4)

	# df_agg_fp = os.path.join(cache_dp, f+".csv")
	df_agg_fp = os.path.join(cache_dp, f+".aggTimes.csv")
	if not os.path.isfile(df_agg_fp):
		if verbose: print("- generating: " + df_agg_fp)
		if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
		df = getAllAggregatedTimeStats(dbm, runID)
		if not df is None:
			df.to_csv(df_agg_fp, index=False)

	df_agg_grouped_fp = os.path.join(cache_dp, f+".aggTimes.groupByType.csv")
	if not os.path.isfile(df_agg_grouped_fp):
		if verbose: print("- generating: " + df_agg_grouped_fp)
		if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
		df_agg_grouped = aggregateTimesByType(dbm, runID)
		if not df_agg_grouped is None:
			df_agg_grouped.to_csv(df_agg_grouped_fp, index=False)

	if not table_empty(db, "AggregateTime"):
		windowedAggTimes_fp = os.path.join(cache_dp, f+".aggTimes.windowed.groupByType.csv")
		if not os.path.isfile(windowedAggTimes_fp):
			if trees is None:
				with open(trees_fp, 'rb') as input:
					trees = pickle.load(input)
			didAllRanksPerformWindowedAggregation = True
			for rank, t in trees.items():
				n = t.findTreeNodeByType("AggregationStepper")
				if n is None:
					didAllRanksPerformWindowedAggregation = False
					break
			if didAllRanksPerformWindowedAggregation:
				if verbose: print("- generating: " + windowedAggTimes_fp)
				if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
				df_all = None
				## This loop is expensive. Can I cache every N iterations?
				#for rank, t in trees.items():
				ranks = [r for r in trees.keys()]
				print("Collating windowed aggregated times ...")
				for i in tqdm.tqdm(range(0, len(ranks))):
					rank = ranks[i]
					t = trees[rank]
					processID = getProcessID(dbm, rank)
					df = collateWindowedAggregateTimes(dbm, runID, processID, t)
					if df_all is None:
						df_all = df
					else:
						df_all = df_all.append(df)
				df_all.sort_values(["Window", "Type", "Rank"], inplace=True)
				df_all.to_csv(windowedAggTimes_fp, index=False)

	if not table_empty(db, "TraceTimeData"):
		traceTimes_fp = os.path.join(cache_dp, f+".traceTimes.csv")
		if not os.path.isfile(traceTimes_fp):
			if verbose: print("- generating: " + traceTimes_fp)
			if trees is None:
				with open(trees_fp, 'rb') as input:
					trees = pickle.load(input)

			traces_df_all = None
			for rank, t in trees.items():
				n = t.findTreeNodeByType("TraceConductor")
				if n is None:
					n = findSolverNode(t, 1, t.time)
				if n is None:
					raise Exception("Could not deduce top-most callpath node in solver loop, so cannot perform trace analysis")
				if not n is None:
					traceGroupNode = n.name
					if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
					processID = getProcessID(dbm, rank)
					traces_df = traceTimes_aggregateByNode(dbm, runID, processID, t, traceGroupNode)
					traces_df["Rank"] = rank
					if traces_df_all is None:
						traces_df_all = traces_df
					else:
						traces_df_all = traces_df_all.append(traces_df)
			traces_df_all.to_csv(traceTimes_fp, index=False)


		if args.trace_group_focus:
			traceTimes_focused_fp = os.path.join(cache_dp, f+".time-traces.focused-on-{0}.csv".format(args.trace_group_focus))
			if not os.path.isfile(traceTimes_focused_fp):
				if verbose: print("- generating: " + traceTimes_focused_fp)
				if trees is None:
					with open(trees_fp, 'rb') as input:
						trees = pickle.load(input)

				traces_df_all = None
				for rank, t in trees.items():
					n = t.findTreeNodeByType("TraceConductor")
					if n is None:
						n = findSolverNode(t, 1, t.time)
					if n is None:
						raise Exception("Could not deduce top-most callpath node in solver loop, so cannot perform trace analysis")
					else:
						traceGroupNode = n.name
						if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
						processID = getProcessID(dbm, rank)
						traces_df = traceTimes_aggregateByNode(dbm, runID, processID, t, traceGroupNode, args.trace_group_focus)
						traces_df["Rank"] = rank
						if traces_df_all is None:
							traces_df_all = traces_df
						else:
							traces_df_all = traces_df_all.append(traces_df)
				traces_df_all.to_csv(traceTimes_focused_fp, index=False)

	if not args.parameter is None:
		windowedAggParam_fp = os.path.join(cache_dp, f+".aggParameter-{0}.windowed.csv".format(args.parameter))
		if not os.path.isfile(windowedAggParam_fp):
			if trees is None:
				with open(trees_fp, 'rb') as input:
					trees = pickle.load(input)
			aggParam_df_all = None
			didAllRanksPerformWindowedAggregation = True
			for rank, t in trees.items():
				n = t.findTreeNodeByType("AggregationStepper")
				if n is None:
					didAllRanksPerformWindowedAggregation = False
					break
			if didAllRanksPerformWindowedAggregation:
				if verbose: print("- generating: " + windowedAggParam_fp)
				if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
				df_all = None
				## This loop is expensive. Can I cache every N iterations?
				for rank, t in trees.items():
					processID = getProcessID(dbm, rank)
					df = collateWindowedAggregateParameter(dbm, runID, processID, t, args.parameter)
					if df_all is None:
						df_all = df
					else:
						df_all = df_all.append(df)
				df_all.sort_values(["Window", "Rank"], inplace=True)
				df_all.to_csv(windowedAggParam_fp, index=False)

		traceParameter_fp = os.path.join(cache_dp, f+".traceParameter-{0}.csv".format(args.parameter))
		if not os.path.isfile(traceParameter_fp):
			if verbose: print("- generating: " + traceParameter_fp)
			if trees is None:
				with open(trees_fp, 'rb') as input:
					trees = pickle.load(input)

			traces_df_all = None
			for rank, t in trees.items():
				n = t.findTreeNodeByType("TraceConductor")
				if n is None:
					n = findSolverNode(t, 1, t.time)
				if n is None:
					raise Exception("Could not deduce top-most callpath node in solver loop, so cannot perform trace analysis")
				else:
					traceGroupNode = n.name
					if dbm is None: dbm = sqlite3.connect(':memory:') ; db.backup(dbm)
					processID = getProcessID(dbm, rank)
					traces_df = traceParameter_aggregateByNode(dbm, runID, processID, t, traceGroupNode, args.parameter)
					if not traces_df is None:
						traces_df["Rank"] = rank
						if traces_df_all is None:
							traces_df_all = traces_df
						else:
							traces_df_all = traces_df_all.append(traces_df)
			if not traces_df_all is None:
				traces_df_all.to_csv(traceParameter_fp, index=False)

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

	aggTimes_raw_df = None
	aggTimes_grouped_df = None
	aggTimesWindowed_grouped_df = None
	aggParamWindowed_all_df = None

	traceTimes_all_df = None
	traceTimes_focused_all_df = None
	traceParameter_all_df = None

	ranks = set()

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

	if args.parallel:
		nprocs = min(cpu_count(), len(db_fps))
		print("Processing {0} DB files in parallel on {1} CPUs".format(len(db_fps), nprocs))
		dbs_pending = []
		for i in range(0, len(db_fps)):
			db_fp = db_fps[i]
			params = (db_fp, i+1, len(db_fps), cache_dp)
			dbs_pending.append(params)
		p = Pool(nprocs)
		if verbose: p.imap_unordered(preprocess_db_job, dbs_pending)
		else:
			for _ in tqdm.tqdm(p.imap_unordered(preprocess_db_job, dbs_pending), total=len(dbs_pending)):
				pass
	else:
		if len(db_fps) > 1:
			print("Hint: speedup post-processing with parallelism: --parallel")
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
			db_ranks = getMpiRanks(db_fp)
			for rank in db_ranks: ranks.add(rank)
			if verbose: print("Collating data from DB {0}".format(f))

			df_csv = os.path.join(cache_dp, f+".aggTimes.csv")
			if not os.path.isfile(df_csv):
				print("Processing: {0} ({1}/{2})".format(db_fp, ctr, len(db_fps)))
				preprocess_db(db_fp, ctr, len(db_fps), cache_dp)
			if os.path.isfile(df_csv):
				df = pd.read_csv(df_csv)
				if aggTimes_raw_df is None:
					aggTimes_raw_df = df
				else:
					aggTimes_raw_df = aggTimes_raw_df.append(df)

			df_agg_fp = os.path.join(cache_dp, f+".aggTImes.groupByType.csv")
			if os.path.isfile(df_agg_fp):
				df_agg = pd.read_csv(df_agg_fp)
				if aggTimes_grouped_df is None:
					aggTimes_grouped_df = df_agg
				else:
					aggTimes_grouped_df = aggTimes_grouped_df.append(df_agg)

			windowedAggTimes_fp = os.path.join(cache_dp, f+".aggTimes.windowed.groupByType.csv")
			if os.path.isfile(windowedAggTimes_fp):
				df = pd.read_csv(windowedAggTimes_fp)
				if aggTimesWindowed_grouped_df is None:
					aggTimesWindowed_grouped_df = df
				else:
					aggTimesWindowed_grouped_df = aggTimesWindowed_grouped_df.append(df)

			windowedAggParam_fp = os.path.join(cache_dp, f+".aggParameter-{0}.windowed.csv".format(args.parameter))
			if os.path.isfile(windowedAggParam_fp):
				df = pd.read_csv(windowedAggParam_fp)
				if aggParamWindowed_all_df is None:
					aggParamWindowed_all_df = df
				else:
					aggParamWindowed_all_df = aggParamWindowed_all_df.append(df)

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

			if not args.parameter is None:
				traceParameter_fp = os.path.join(cache_dp, f+".traceParameter-{0}.csv".format(args.parameter))
				if os.path.isfile(traceParameter_fp):
					df = pd.read_csv(traceParameter_fp)
					if traceParameter_all_df is None:
						traceParameter_all_df = df
					else:
						traceParameter_all_df = traceParameter_all_df.append(df)

			trees_fp = os.path.join(cache_dp, f+".call-trees.pkl")
			with open(trees_fp, 'rb') as input:
				trees = pickle.load(input)

			for rank, t in trees.items():
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
						chartCallPath(t, plotType, "Call stack times of rank {0}".format(rank), args.tt_results_dirpath, "rank-{0}.png".format(rank), args.label)

	if (not args.charts is None) and (not groupedCallTrees is None):
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
			chartCallPath(aggSum, plotType, "Call stack times summed across ranks "+sorted(agg.ranks).__str__(), args.tt_results_dirpath, "rankGroup{0}.png".format(gn), args.label)

	if len(ranks) > 1:
		if not aggTimesWindowed_grouped_df is None:
			print("Drawing windowed aggregated times charts")
			aggTimes_chartDynamicLoadBalance(aggTimesWindowed_grouped_df, output_dirpath=args.tt_results_dirpath)

		if not aggParamWindowed_all_df is None:
			print("Drawing windowed aggregated parameter '{0}' chart".format(args.parameter))
			aggParam_chartDynamicLoadBalance(aggParamWindowed_all_df, args.parameter, output_dirpath=args.tt_results_dirpath)

		print("Drawing trace charts")
		if not traceTimes_all_df is None:
			traceTimes_chartDynamicLoadBalance(traceTimes_all_df, output_dirpath=args.tt_results_dirpath)
		if not traceTimes_focused_all_df is None:
			traceTimes_chartDynamicLoadBalance(traceTimes_focused_all_df, output_dirpath=args.tt_results_dirpath, filename_suffix="focused-on-"+args.trace_group_focus)
		if not traceParameter_all_df is None:
			traceParameter_chart(traceParameter_all_df, args.parameter, output_dirpath=args.tt_results_dirpath)

	print("Writing out collated CSVs")
	aggTimes_raw_df["num_ranks"] = len(ranks)
	aggTimes_raw_df.sort_values(["Name", "Rank"], inplace=True)
	df_filename = os.path.join(output_dirpath, "timings_raw.csv")
	aggTimes_raw_df.to_csv(df_filename, index=False)
	print("Collated raw data written to '{0}'".format(df_filename))

	if not aggTimes_grouped_df is None:
		aggTimes_grouped_df.sort_values(["Method type", "Rank"], inplace=True)
		df_filename = os.path.join(output_dirpath, "timings_aggregated.csv")
		aggTimes_grouped_df.to_csv(df_filename, index=False)
		print("Collated aggregated data written to '{0}'".format(df_filename))


if __name__ == "__main__":
	main()

