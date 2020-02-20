
program main
	use treetimer
	use mpi

	implicit none

	integer :: rank, commSize, comm
	integer :: err
	integer :: i, j
	integer :: rankMax
	integer :: allGatherInput
	integer, allocatable :: allGatherOutput(:)

	call MPI_Init(err)

	if(err .ne. MPI_SUCCESS) then
		write(6,*) 'Error Initializing MPI'
	endif

	call TreeTimerInit()

    call MPI_Comm_rank(MPI_COMM_WORLD, rank, err)
	call MPI_Comm_size(MPI_COMM_WORLD, commSize, err)

	call TreeTimerEnterBlockUnspecified("params1");

	call TreeTimerLogGlobalParameterInt("GlobalIntTest", 19);
	call TreeTimerLogGlobalParameterString("GlobalStringTest", "Hello");
	call TreeTimerLogGlobalParameterRealDouble("GlobalDoubleTest", DBLE(12.0));	
	call TreeTimerLogGlobalParameterLogical("GlobalLogicalTestT", .true.);
	call TreeTimerLogGlobalParameterLogical("GlobalLogicalTestF", .false.);

	call TreeTimerExitBlock("params1");

	call TreeTimerEnterBlockUnspecified("allgather");

	allGatherInput = rank
	allocate(allGatherOutput(commSize))
    call MPI_Allgather(allGatherInput, 1, MPI_INTEGER, &
					   allGatherOutput, 1, MPI_INTEGER, &
					   MPI_COMM_WORLD, err)

    deallocate(allGatherOutput)

	call TreeTimerExitBlock("allgather");

	do i = 1, 4
		call TreeTimerEnterBlockUnspecified("Test");

		call TreeTimerLogParameterInt("TestVal", 2);
		call TreeTimerLogParameterInt("TestVal", 4);
		call TreeTimerLogParameterInt("TestVal", 6);
		call TreeTimerLogParameterInt("TestVal", 8);
		call TreeTimerLogParameterInt("i", i);
		call TreeTimerLogParameterRealDouble("i_double", DBLE(i));
		call TreeTimerLogParameterLogical("TestValLogT", .true.);
		call TreeTimerLogParameterLogical("TestValLogF", .false.);
		
		do j = 1, 4
			call TreeTimerEnterBlockLoop("Test2");
			call sleep(j);

			call TreeTimerEnterBlockLoop("Test3");
			call sleep(1);
			call TreeTimerExitBlock("Test3");

			call TreeTimerExitBlock("Test2");
		end do

		call sleep(i);
		call TreeTimerExitBlock("Test");

		do j = 1, 3
			call TreeTimerEnterBlockLoop("Test2");
			call sleep(j);

			call TreeTimerEnterBlockLoop("Test3");
			call sleep(1);
			call TreeTimerExitBlock("Test3");

			call TreeTimerExitBlock("Test2");
		end do
		
	end do

	call TreeTimerFinalize()
	call MPI_Finalize(err)

end program
