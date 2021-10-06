import pandas as pd

def sample_n_timesteps(df, n, indexColname):
	## Evenly sample 'n' timepoints from 'df'

	timestepIndices = df[indexColname].unique()
	timestepIndices.sort()
	if n < len(timestepIndices):
		## More data than requested, so can evenly sample
		index_step = len(timestepIndices) / n
		IDs_sampled = [timestepIndices[round(i*index_step)] for i in range(0, n)]
		df = df[df[indexColname].isin(IDs_sampled)].reset_index(drop=True)
	elif n > len(timestepIndices):
		## Less data than requested, so spread out all data to range 1->n
		index_step = n / len(timestepIndices)
		IDs_scaled = [round(index_step*i) for i in range(0, len(timestepIndices))]
		df_ids = pd.DataFrame({indexColname:timestepIndices, indexColname+"new":IDs_scaled})
		df = df.merge(df_ids, validate="many_to_one")
		df = df.drop(indexColname, axis=1).rename(columns={indexColname+"new":indexColname})

	return df
