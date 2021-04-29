import math

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

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

def sample_n_timesteps(df, n, indexColname):
	## Evenly sample n timepoints
	timestepIndices = df[indexColname].unique()
	timestepIndices.sort()
	if n < len(timestepIndices):
		index_step = len(timestepIndices) / n
		traceTimesIDs_sampled = [timestepIndices[round(i*index_step)] for i in range(0, n)]
		df = df[df[indexColname].isin(traceTimesIDs_sampled)].reset_index(drop=True)
	elif n > len(timestepIndices):
		## For consistent chart X-axis, re-calculate indices to scale 0->100
		index_step = n / len(timestepIndices)
		traceTimesIDs_scaled = [round(index_step*i) for i in range(0, len(timestepIndices))]
		df_ids = pd.DataFrame({indexColname:timestepIndices, indexColname+"new":traceTimesIDs_scaled})
		df = df.merge(df_ids, validate="many_to_one")
		df = df.drop(indexColname, axis=1).rename(columns={indexColname+"new":indexColname})

		return df

def plot_heatmap(df, index_colname, value_colname, fig_filepath):
	df2 = df.pivot_table(index="Rank", columns=index_colname, values=value_colname)
	width = max(10, math.ceil((df2.shape[1])*0.4))
	height = max(10, math.ceil((df2.shape[0])*0.2))
	fs = height*2
	fs2 = round(fs*0.75)
	fig = plt.figure(figsize=(width,height))
	fig.suptitle(value_colname, fontsize=fs)
	ax = fig.add_subplot(1,1,1)
	ax.set_xlabel("Solver timestep progress %", fontsize=fs)
	ax.set_ylabel("Rank", fontsize=fs)
	## Best colormap doc: https://matplotlib.org/stable/tutorials/colors/colormaps.html
	#plt.imshow(df2.values, cmap='Reds')
	plt.imshow(df2.values, cmap='Reds', extent=[0,99, 0,df["Rank"].max()])
	# plt.pcolor(df2.values, cmap='Reds')
	#ax.set_xticks(df2.columns.values)
	#ax.set_xticklabels(df2.columns.values)
	# ax.set_yticks([0,diff_df["Rank"].max()])
	cb = plt.colorbar(aspect=5)
	cb.ax.tick_params(labelsize=fs2)
	ax.tick_params(labelsize=fs2)
	plt.savefig(fig_filepath)
	plt.close(fig)
