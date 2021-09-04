!
! @file
! @author James Davis
! @version 0.1
!
! @section LICENSE
! Based on PMTM/Ichniea - ToDo: Check License for these
!
! @section DESCRIPTION
!

module treetimer
 	USE, INTRINSIC  :: ISO_C_BINDING
	implicit none

	! Subroutines
	public :: TreeTimerInit,						&
			  TreeTimerFinalize,					&
			  TreeTimerSleep,						&
			  TreeTimerWake,						&
			  TreeTimerEnterMethod,					&
			  TreeTimerEnterLoop,					&
			  TreeTimerEnterCompute,				&
			  TreeTimerEnterBlock,					&
			  TreeTimerEnterMPICommCall,			&
			  TreeTimerEnterMPISyncCall,			&
			  TreeTimerEnterMPICollectiveCall,		&
			  TreeTimerEnterIOCall,					&
			  TreeTimerEnterLibraryCall,			&
			  TreeTimerExit,						&
			  TreeTimerLogParameterInt,				&
			  TreeTimerLogParameterRealSingle,		&
			  TreeTimerLogParameterRealDouble,		&
			  TreeTimerLogParameterLogical,			&
!			  TreeTimerLogGlobalParameterInt,		&
!			  TreeTimerLogGlobalParameterRealSingle,&
!			  TreeTimerLogGlobalParameterRealDouble,&
!			  TreeTimerLogGlobalParameterLogical,	&
!			  TreeTimerLogGlobalParameterString,	&
			  TreeTimerFlushTraceData
			  
	contains
	
	subroutine TreeTimerInit
	
		call c_tree_timer_init()
		
	end subroutine
			  
	subroutine TreeTimerFinalize
	
		call c_tree_timer_finalize()
		
	end subroutine

	subroutine TreeTimerSleep

		call c_tree_timer_sleep

	end subroutine

	subroutine TreeTimerWake

		call c_tree_timer_wake

	end subroutine

	subroutine TreeTimerEnterTraceConductor(blockName, traceCallInterval)
		character(len=*)    :: blockName
		integer :: traceCallInterval

		call c_tree_timer_enter_trace_conductor(blockName, len_trim(blockname), traceCallInterval)
	end subroutine
	
	subroutine TreeTimerEnterMethod(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_method(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterLoop(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_loop(blockName, len_trim(blockName))
	
	end subroutine
	
	subroutine TreeTimerEnterCompute(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_compute(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterBlock(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block(blockName, len_trim(blockName))
		
	end subroutine
			  
	subroutine TreeTimerEnterMPICommCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_mpi_comm_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterMPISyncCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_mpi_sync_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterMPICollectiveCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_mpi_collective_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterIOCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_io_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterLibraryCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_library_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerExit(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_exit(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerLogParameterInt(blockName, value)
		character(len=*)	:: blockName
		integer				:: value
		
		call c_tree_timer_log_parameter_int(blockName, len_trim(blockName), value)
		
	end subroutine
	
	subroutine TreeTimerLogParameterRealSingle(blockName, value)
		character(len=*)	:: blockName
		real(kind=4)		:: value
		
		call c_tree_timer_log_parameter_double(blockName, len_trim(blockName), REAL(value,8))
	
	end subroutine
	
	subroutine TreeTimerLogParameterRealDouble(blockName, value)
		character(len=*)	:: blockName
		real(kind=8)		:: value
		
		call c_tree_timer_log_parameter_double(blockName, len_trim(blockName), value)
	
	end subroutine	
	
	subroutine TreeTimerLogParameterLogical(blockName, value)
		character(len=*)	:: blockName
		logical				:: value
		
		if(value .eqv. .true.) then
			call c_tree_timer_log_parameter_bool(blockName, len_trim(blockName), 1)
		else
			call c_tree_timer_log_parameter_bool(blockName, len_trim(blockName), 0)			
		endif
	
	end subroutine
	
!	Update: What is the purpose of "global" parameters? Codebase only sets them, never uses them.
!	subroutine TreeTimerLogGlobalParameterInt(blockName, value)
!		character(len=*)	:: blockName
!		integer				:: value
!		
!		call c_tree_timer_log_global_parameter_int(blockName, len_trim(blockName), value)
!	end subroutine
!	
!	subroutine TreeTimerLogGlobalParameterRealSingle(blockName, value)
!		character(len=*)	:: blockName
!		real(kind=4)		:: value
!		
!		call c_tree_timer_log_global_parameter_double(blockName, len_trim(blockName), REAL(value,8))
!	end subroutine
!
!	subroutine TreeTimerLogGlobalParameterRealDouble(blockName, value)
!		character(len=*)	:: blockName
!		real(kind=8)		:: value
!		
!		call c_tree_timer_log_global_parameter_double(blockName, len_trim(blockName), value)
!	end subroutine
!		
!	subroutine TreeTimerLogGlobalParameterLogical(blockName, value)
!		character(len=*)	:: blockName
!		logical				:: value
!		
!		if(value .eqv. .true.) then
!			call c_tree_timer_log_global_parameter_bool(blockName, len_trim(blockName), 1)
!		else
!			call c_tree_timer_log_global_parameter_bool(blockName, len_trim(blockName), 0)			
!		endif
!	end subroutine
!	
!	subroutine TreeTimerLogGlobalParameterString(blockName, value)
!		character(len=*)	:: blockName
!		character(len=*)	:: value
!		
!		call c_tree_timer_log_global_parameter_string(blockName, len_trim(blockName), value, len_trim(value))
!	end subroutine
	
	subroutine TreeTimerFlushTraceData()
	
		call c_tree_timer_flush_trace_data()
	
	end subroutine
	
end module
