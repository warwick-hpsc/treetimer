from copy import deepcopy
import math
import os

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from mpl_toolkits import axes_grid1

import imp
imp.load_source("PostProcessCallTreeUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-callTree-utils.py"))
from PostProcessCallTreeUtils import *
imp.load_source("PostProcessPdUtils", os.path.join(os.path.dirname(os.path.realpath(__file__)), "post-process-pandas-utils.py"))
from PostProcessPdUtils import *

import enum
class PlotType(enum.Enum):
	Polar = 1
	Horizontal = 2
	Vertical = 3
plotTypeToString = {PlotType.Polar:"Polar", PlotType.Horizontal:"Horizontal", PlotType.Vertical:"Vertical"}

fig_dims = (8,8)

methodTypeToColour = {}
methodTypeToColour["Program"] = "silver"
methodTypeToColour["AggregationStepper"] = "silver"
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

def chartCallPath(tree, plotType, title, output_dir, filename, addLabels):
	fig = plt.figure(figsize=fig_dims)
	plotCallPath_root(tree, plotType, addLabels)
	fig.suptitle(title)
	if not output_dir is None:
		chart_dirpath = os.path.join(output_dir, "charts", plotTypeToString[plotType])
	else:
		chart_dirpath = os.path.join("charts", plotTypeToString[plotType])
	if not os.path.isdir(chart_dirpath):
		os.makedirs(chart_dirpath)
	chart_filepath = os.path.join(chart_dirpath, filename)
	plt.savefig(chart_filepath)
	plt.close(fig)

def plotCallPath_root(tree, plotType, addLabels):
	if plotType == PlotType.Polar:
		root_total = np.pi * 2
		ax = plt.subplot(111, projection='polar')
	else:
		root_total = tree.time
		ax = plt.subplot(111)

	plotCallPath(tree, root_total, root_total, 0, 0, ax, plotType, addLabels)

def plotCallPath(tree, root_total, node_total, offset, level, ax, plotType, addLabels):
	are_any_nodes_labelled = False
	if level == 0:
		if not isinstance(tree, CallTreeNode):
			raise Exception("'tree' parameter at root must be a CallTreeNode")
		label = tree.name
		methodType = tree.typeName
		value = tree.time
		subnodes = tree.leaves

		are_any_subnodes_labelled = plotCallPath(subnodes, root_total, value, 0, level+1, ax, plotType, addLabels)
		do_label_node = addLabels and not are_any_subnodes_labelled

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

			are_any_subnodes_labelled = plotCallPath(subnodes, root_total, node_total, subnode_offset, level+1, ax, plotType, addLabels)
			subnode_offset += value

			## Logic to ensure only outermost and non-tiny pie elements are labelled.
			do_label_node = addLabels and not are_any_subnodes_labelled
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


def aggTimes_chartDynamicLoadBalance(agg_df, output_dirpath=None, filename_suffix=None):
	if not "Window" in agg_df.columns.values:
		raise Exception("aggTimes_chartDynamicLoadBalance() requires windowed aggregated times (must have 'Window' column")

	# Restrict to MPI time %:
	mpi_times = agg_df[agg_df["Type"] == "MPI"]
	if mpi_times.shape[0] == 0:
		print("No MPI functions were timed")
		return
	mpi_times = mpi_times.drop(["Type", "Exclusive time"], axis=1)
	mpi_times = mpi_times.rename(columns={"Exclusive time %":"MPI %"})

	# For a intuitive plot, scale % from 0->1 to 0->100
	mpi_times["MPI %"] *= 100.0

	r_root = mpi_times["Rank"].min()
	num_windows_root = mpi_times[mpi_times["Rank"]==r_root].shape[0]
	for r in mpi_times["Rank"].unique():
		num_windows = mpi_times[mpi_times["Rank"]==r].shape[0]
		if num_windows != num_windows_root:
			raise Exception("Ranks {0} and {1} performed different number of aggregation windows - {2} vs {3}".format(r_root, r, num_windows_root, num_windows))

	## Evenly sample 100 timepoints, so that final chart is legible:
	mpi_times = sample_n_timesteps(mpi_times, 100, "Window")
	timestepIndices = mpi_times["Window"].unique()
	timestepIndices.sort()

	## Ensure table is sorted for calculation
	mpi_times = mpi_times.sort_values(["Window", "Rank"])

	## Construct heatmap, of MPI % during simulation:
	if not filename_suffix is None:
		fig_filename = "aggTimesWindowed-mpiPct-heatmap.{0}.png".format(filename_suffix)
	else:
		fig_filename = "aggTimesWindowed-mpiPct-heatmap.png"
	if not output_dirpath is None:
		fig_filepath = os.path.join(output_dirpath, fig_filename)
	else:
		fig_filepath = fig_filename

	plot_heatmap(mpi_times, "Window", "MPI %", fig_filepath)

	## Finally! Calculate difference in MPI % over time:
	col_ranks = None
	col_index = None
	col_diffs = None
	step = mpi_times[mpi_times["Rank"]==mpi_times["Rank"].min()].shape[0]//2
	## Calculate difference against last:
	tsLast = mpi_times[mpi_times["Window"]==timestepIndices[-1]]
	tsFirst = mpi_times[mpi_times["Window"]==timestepIndices[0]]
	for ts in range(0, len(timestepIndices)):
		ts1 = mpi_times[mpi_times["Window"]==timestepIndices[ts]]
		## Calculate diff, etc
		col_ranks = ts1["Rank"].values if (col_ranks is None) else np.append(col_ranks, ts1["Rank"].values)
		col_index = ts1["Window"].values if (col_index is None) else np.append(col_index, ts1["Window"].values)
		diff = ts1["MPI %"].values - tsLast["MPI %"].values
		# diff = ts1["MPI %"].values - tsFirst["MPI %"].values
		col_diffs = diff if (col_diffs is None) else np.append(col_diffs, diff)
	diff_df = pd.DataFrame({"Window":col_index, "Rank":col_ranks, "(MPI %) diff":col_diffs})

	if not filename_suffix is None:
		fig_filename = "aggTimesWindowed-mpiPctChange-heatmap.{0}.png".format(filename_suffix)
	else:
		fig_filename = "aggTimesWindowed-mpiPctChange-heatmap.png"
	if not output_dirpath is None:
		fig_filepath = os.path.join(output_dirpath, fig_filename)
	else:
		fig_filepath = fig_filename
	plot_heatmap(diff_df, "Window", "(MPI %) diff", fig_filepath)

	## Sum stdev across ranks
	diffSum_df = diff_df.drop("Rank", axis=1).groupby("Window").sum().reset_index()
	diffSum_df = diffSum_df.rename(columns={"(MPI %) diff":"Sum MPI % diff"})
	diffSum_stdev = diffSum_df["Sum MPI % diff"].std()
	diffSum_mean = diffSum_df["Sum MPI % diff"].mean()
	diffSum_stdev_pct = 0.0 if diffSum_mean == 0.0 else diffSum_stdev/diffSum_mean*100.0
	# print("diffSum_stdev % mean = {0:.1f}".format(diffSum_stdev_pct))


def aggParam_chartDynamicLoadBalance(aggParam_df, paramName, output_dirpath=None):
	r_root = aggParam_df["Rank"].min()
	num_ts_root = aggParam_df[aggParam_df["Rank"]==r_root].shape[0]
	for r in aggParam_df["Rank"].unique():
		num_ts = aggParam_df[aggParam_df["Rank"]==r].shape[0]
		if num_ts != num_ts_root:
			raise Exception("Ranks {0} and {1} performed different number of solver timesteps - {2} vs {3}".format(r_root, r, num_ts_root, num_ts))

	# # ## Add a unit-stride index column:
	# aggParam_df = add_unit_stride_index_column(aggParam_df, "Window")

	## Evenly sample 100 timepoints, so that final chart is legible:
	aggParam_df = sample_n_timesteps(aggParam_df, 100, "Window")
	timestepIndices = aggParam_df["Window"].unique()
	timestepIndices.sort()

	## Ensure table is sorted for calculation
	aggParam_df = aggParam_df.sort_values(["Window", "Rank"])

	## Construct heatmap, of MPI % during simulation:
	fig_filename = "aggParameter-{0}-heatmap.png".format(paramName)
	if not output_dirpath is None:
		fig_filepath = os.path.join(output_dirpath, fig_filename)
	else:
		fig_filepath = fig_filename
	plot_heatmap(aggParam_df, "Window", "Value", fig_filepath)


def plot_heatmap(df, index_colname, value_colname, fig_filepath):
	df_pvt = df.pivot_table(index="Rank", columns=index_colname, values=value_colname)

	# use_default_vertical_draw = True
	# if (df_pvt.shape[0] > 50) and (df_pvt.shape[0] > df_pvt.shape[1]*4):
	# 	## Drawing with many ranks as Y-axis and few readings will produce a tall narrow plot.
	# 	## Better to draw this horizontal with ranks as X-axis.
	# 	use_default_vertical_draw = False
	## Actually, for consistency, always draw with ranks horizontal
	use_default_vertical_draw = False

	if not use_default_vertical_draw:
		df_pvt = df.pivot_table(index=index_colname, columns="Rank", values=value_colname)

	## Ensure Y data is drawn in correct direction:
	if use_default_vertical_draw:
		df_pvt = df_pvt.sort_values("Rank", ascending=False)
	else:
		df_pvt = df_pvt.sort_values(index_colname, ascending=False)

	# fig = plt.figure(dpi=75)
	fig_width = max(10, (df_pvt.shape[1]/40))
	fig = plt.figure(dpi=75, figsize=(fig_width,10))

	fig.suptitle(value_colname)
	ax = fig.add_subplot(1,1,1)
	if use_default_vertical_draw:
		ax.set_xlabel("Solver timestep progress %")
		ax.set_ylabel("Rank")
	else:
		ax.set_xlabel("Rank")
		ax.set_ylabel("Solver timestep progress %")

	## Best colormap doc: https://matplotlib.org/stable/tutorials/colors/colormaps.html
	## Normalize() used to ensure 0.0 is white.
	if df[value_colname].min() < 0.0 and df[value_colname].max() > 0.0:
		cm = 'bwr'
		a = df[value_colname].abs().max()
		nm = plt.Normalize(-a, a)
	else:
		if df[value_colname].max() > 0.0:
			cm = 'Reds'
			nm = plt.Normalize(0.0, df[value_colname].max())
		else:
			cm = 'Blues'
			nm = plt.Normalize(df[value_colname].min(), 0.0)

	if use_default_vertical_draw:
		ranges = [0,99, 0,df["Rank"].max()]
	else:
		ranges = [0,df["Rank"].max(), 0,99]
	im = plt.imshow(df_pvt.values, cmap=cm, norm=nm, extent=ranges, interpolation='none')

	## Getting colorbar height to match plot is remarkably fiddly!
	## Even harder to stop width expanding with plot.
	## This seems to work: https://stackoverflow.com/a/33505522
	divider = axes_grid1.make_axes_locatable(im.axes)
	aspect = 20
	width = axes_grid1.axes_size.AxesY(im.axes, aspect=1./aspect)
	pad_fraction = 0.5
	pad = axes_grid1.axes_size.Fraction(pad_fraction, width)
	current_ax = plt.gca()
	cax = divider.append_axes("right", size=width, pad=pad)
	plt.sca(current_ax)
	plt.colorbar(cax=cax)

	plt.savefig(fig_filepath)
	plt.close(fig)

	print("Heatmap written to: " + fig_filepath)

