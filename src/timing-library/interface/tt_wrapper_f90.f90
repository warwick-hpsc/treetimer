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
			  TreeTimerEnterBlockMethod,			&
			  TreeTimerEnterBlockLoop,				&
			  TreeTimerEnterBlockComputeLoop,		&
			  TreeTimerEnterBlockUnspecified,		&
			  TreeTimerEnterBlockNonMPICall,		&
			  TreeTimerEnterBlockMPICommCall,		&
			  TreeTimerEnterBlockMPINonCommCall,	&
			  TreeTimeEnterBlockIOCall,				&
			  TreeTimerExitBlock,					&
			  TreeTimerLogParameterInt,				&
			  TreeTimerLogParameterRealSingle,		&
			  TreeTimerLogParameterRealDouble,		&
			  TreeTimerLogParameterLogical,			&
			  TreeTimerLogGlobalParameterInt,		&
			  TreeTimerLogGlobalParameterRealSingle,&
			  TreeTimerLogGlobalParameterRealDouble,&
			  TreeTimerLogGlobalParameterLogical,	&
			  TreeTimerLogGlobalParameterString,	&
			  TreeTimerFlushTraceData
			  
	contains
	
	subroutine TreeTimerInit
	
		call c_tree_timer_init()
		
	end subroutine
			  
	subroutine TreeTimerFinalize
	
		call c_tree_timer_finalize()
		
	end subroutine
	
	subroutine TreeTimerEnterBlockMethod(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_method(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterBlockLoop(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_loop(blockName, len_trim(blockName))
	
	end subroutine
	
	subroutine TreeTimerEnterBlockComputeLoop(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_compute_loop(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterBlockUnspecified(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_unspecified(blockName, len_trim(blockName))
		
	end subroutine
			  
	subroutine TreeTimerEnterBlockNonMPICall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_non_mpi_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterBlockMPICommCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_mpi_comm_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerEnterBlockMPINonCommCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_mpi_non_comm_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimeEnterBlockIOCall(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_enter_block_io_call(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerExitBlock(blockName)
		character(len=*)	:: blockName
		
		call c_tree_timer_exit_block(blockName, len_trim(blockName))
		
	end subroutine
	
	subroutine TreeTimerLogParameterInt(blockName, value)
		character(len=*)	:: blockName
		integer				:: value
		
		call c_tree_timer_log_parameter_int(blockName, len_trim(blockName), value)
		
	end subroutine
	
	subroutine TreeTimerLogParameterRealSingle(blockName, value)
		character(len=*)	:: blockName
		real(kind=4)		:: value
		
		call c_tree_timer_log_parameter_double(blockName, len_trim(blockName), value)
	
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
	
	subroutine TreeTimerLogGlobalParameterInt(blockName, value)
		character(len=*)	:: blockName
		integer				:: value
		
		call c_tree_timer_log_global_parameter_int(blockName, len_trim(blockName), value)
	end subroutine
	
	subroutine TreeTimerLogGlobalParameterRealSingle(blockName, value)
		character(len=*)	:: blockName
		real(kind=4)		:: value
		
		call c_tree_timer_log_global_parameter_double(blockName, len_trim(blockName), value)
	end subroutine

	subroutine TreeTimerLogGlobalParameterRealDouble(blockName, value)
		character(len=*)	:: blockName
		real(kind=8)		:: value
		
		call c_tree_timer_log_global_parameter_double(blockName, len_trim(blockName), value)
	end subroutine
		
	subroutine TreeTimerLogGlobalParameterLogical(blockName, value)
		character(len=*)	:: blockName
		logical				:: value
		
		if(value .eqv. .true.) then
			call c_tree_timer_log_global_parameter_bool(blockName, len_trim(blockName), 1)
		else
			call c_tree_timer_log_global_parameter_bool(blockName, len_trim(blockName), 0)			
		endif
	end subroutine
	
	subroutine TreeTimerLogGlobalParameterString(blockName, value)
		character(len=*)	:: blockName
		character(len=*)	:: value
		
		call c_tree_timer_log_global_parameter_string(blockName, len_trim(blockName), value, len_trim(value))
	end subroutine
	
	subroutine TreeTimerFlushTraceData()
	
		call c_tree_timer_flush_trace_data()
	
	end subroutine
	
end module