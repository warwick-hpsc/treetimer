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

