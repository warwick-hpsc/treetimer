
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

	call TreeTimerEnterBlock("params1");

	call TreeTimerLogGlobalParameterInt("GlobalIntTest", 19);
	call TreeTimerLogGlobalParameterString("GlobalStringTest", "Hello");
	call TreeTimerLogGlobalParameterRealDouble("GlobalDoubleTest", DBLE(12.0));	
	call TreeTimerLogGlobalParameterLogical("GlobalLogicalTestT", .true.);
	call TreeTimerLogGlobalParameterLogical("GlobalLogicalTestF", .false.);

	call TreeTimerExit("params1");

	call TreeTimerEnterBlock("allgather");

	allGatherInput = rank
	allocate(allGatherOutput(commSize))
    call MPI_Allgather(allGatherInput, 1, MPI_INTEGER, &
					   allGatherOutput, 1, MPI_INTEGER, &
					   MPI_COMM_WORLD, err)

    deallocate(allGatherOutput)

	call TreeTimerExit("allgather");

	do i = 1, 4
		call TreeTimerEnterBlock("Test");

		call TreeTimerLogParameterInt("TestVal", 2);
		call TreeTimerLogParameterInt("TestVal", 4);
		call TreeTimerLogParameterInt("TestVal", 6);
		call TreeTimerLogParameterInt("TestVal", 8);
		call TreeTimerLogParameterInt("i", i);
		call TreeTimerLogParameterRealDouble("i_double", DBLE(i));
		call TreeTimerLogParameterLogical("TestValLogT", .true.);
		call TreeTimerLogParameterLogical("TestValLogF", .false.);
		
		do j = 1, 4
			call TreeTimerEnterLoop("Test2");
			call sleep(j);

			call TreeTimerEnterLoop("Test3");
			call sleep(1);
			call TreeTimerExit("Test3");

			call TreeTimerExit("Test2");
		end do

		call sleep(i);
		call TreeTimerExit("Test");

		do j = 1, 3
			call TreeTimerEnterLoop("Test2");
			call sleep(j);

			call TreeTimerEnterLoop("Test3");
			call sleep(1);
			call TreeTimerExit("Test3");

			call TreeTimerExit("Test2");
		end do
		
	end do

	call TreeTimerFinalize()
	call MPI_Finalize(err)

end program
