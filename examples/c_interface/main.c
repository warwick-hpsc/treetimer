
#include "unistd.h"
#include "mpi.h"
#include "tt_interface_c.h"
#include "stdio.h"

void main(int argc, char ** argv)
{
	MPI_Init(&argc, &argv);
	TreeTimerInit();

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	TreeTimerLogGlobalParameterInt("GlobalIntTest", 19);

	for(int i = 1; i < 5; i++)
	{
		TreeTimerEnterBlock("Test");
		TreeTimerLogParameterInt("TestVal", 2);
		TreeTimerLogParameterInt("TestVal", 4);
		TreeTimerLogParameterInt("TestVal", 6);
		TreeTimerLogParameterInt("TestVal", 8);
		TreeTimerLogParameterInt("i", i);
		TreeTimerLogParameterDouble("i_double", (double) i);

		for(int j = 1; j < 5; j++)
		{
			TreeTimerEnterLoop("Test2");
			sleep(j);
			TreeTimerExit("Test2");
		}

		sleep(i);
		TreeTimerExit("Test");

		for(int j = 1; j < 4; j++)
		{
			TreeTimerEnterLoop("Test2");
			sleep(j);
			TreeTimerExit("Test2");
		}

	}

	printf("Finalizing\n");
	TreeTimerFinalize();
	MPI_Finalize();
}
