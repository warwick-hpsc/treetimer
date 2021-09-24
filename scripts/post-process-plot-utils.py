import math

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

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
	if not args.tt_results_dirpath is None:
		chart_dirpath = os.path.join(args.tt_results_dirpath, "charts", plotTypeToString[plotType])
	else:
		chart_dirpath = os.path.join("charts", plotTypeToString[plotType])
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

def plot_heatmap(df, index_colname, value_colname, fig_filepath):
	df_pvt = df.pivot_table(index="Rank", columns=index_colname, values=value_colname)

	fig = plt.figure(dpi=75)
	fig.suptitle(value_colname)
	ax = fig.add_subplot(1,1,1)
	ax.set_xlabel("Solver timestep progress %")
	ax.set_ylabel("Rank")

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
	plt.imshow(df_pvt.values, cmap=cm, norm=nm, extent=[0,99, 0,df["Rank"].max()], interpolation='none')
	cb = plt.colorbar()

	plt.savefig(fig_filepath)
	plt.close(fig)

	print("Heatmap written to: " + fig_filepath)

