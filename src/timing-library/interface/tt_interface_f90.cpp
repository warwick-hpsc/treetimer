/*
 * @file
 * @author James Davis
 * @version 0.1
 *
 * @section LICENSE
 * Based on PMTM/Ichniea - ToDo: Check License for these
 *
 * @section DESCRIPTION
 *
 */

#include "tt_interface_f90.h"
#include "tt_interface_c.h"

int F2C(c_tree_timer_init, C_TREE_TIMER_INIT)()
{
	TreeTimerInit();
	return 0;
}

int F2C(c_tree_timer_finalize, C_TREE_TIMER_FINALIZE)()
{
	TreeTimerFinalize();
	return 0;
}

int F2C(c_tree_timer_log_parameter_int, C_TREE_TIMER_LOG_PARAMETER_INT)(const char * name, int * text_len, int * value)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerLogParameterInt(c_name, *value);

	return 0;
}

int F2C(c_tree_timer_log_parameter_double, C_TREE_TIMER_LOG_PARAMETER_DOUBLE)(const char * name, int * text_len,  double * value)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerLogParameterDouble(c_name, * value);

	return 0;
}

int F2C(c_tree_timer_log_parameter_bool, C_TREE_TIMER_LOG_PARAMETER_BOOL)(const char * name, int * text_len,  int * value)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerLogParameterBool(c_name, *value);

	return 0;
}

int F2C(c_tree_timer_enter_trace_conductor, C_TREE_TIMER_ENTER_TRACE_CONDUCTOR)(const char * name, int * text_len,  int * traceCallInterval)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);
	TreeTimerEnterTraceConductor(c_name, *traceCallInterval);
	return 0;
}

int F2C(c_tree_timer_enter_method, C_TREE_TIMER_ENTER_METHOD)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterMethod(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_loop, C_TREE_TIMER_ENTER_LOOP)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterLoop(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_compute, C_TREE_TIMER_ENTER_COMPUTE)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterCompute(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_block, C_TREE_TIMER_ENTER_BLOCK)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterBlock(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_mpi_comm_call, C_TREE_TIMER_ENTER_MPI_COMM_CALL)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterMPICommCall(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_mpi_sync_call, C_TREE_TIMER_ENTER_MPI_SYNC_CALL)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterMPISyncCall(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_mpi_collective_call, C_TREE_TIMER_ENTER_MPI_COLLECTIVE_CALL)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterMPICollectiveCall(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_io_call, C_TREE_TIMER_ENTER_IO_CALL)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterIOCall(c_name);

	return 0;
}

int F2C(c_tree_timer_enter_library_call, C_TREE_TIMER_ENTER_LIBRARY_CALL)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerEnterLibraryCall(c_name);

	return 0;
}

int F2C(c_tree_timer_exit, C_TREE_TIMER_EXIT)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerExit(c_name);

	return 0;
}

int F2C(c_tree_timer_log_global_parameter_int, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_INT)(const char * name, int * text_len, int * value)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerLogGlobalParameterInt(c_name, *value);

	return 0;
}

int F2C(c_tree_timer_log_global_parameter_double, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_DOUBLE)(const char * name, int * text_len, double * value)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerLogGlobalParameterDouble(c_name, *value);

	return 0;
}

int F2C(c_tree_timer_log_global_parameter_bool, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_BOOL)(const char * name, int * text_len, int * value)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	TreeTimerLogGlobalParameterBool(c_name, *value);

	return 0;
}

int F2C(c_tree_timer_log_global_parameter_string, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_BOOL)(const char * name, int * name_len, const char * value, int * value_len)
{
	char c_name[*name_len + 1];
	F2C_strcpy(c_name, name, *name_len);

	char c_value[*value_len + 1];
	F2C_strcpy(c_value, value, *value_len);

	TreeTimerLogGlobalParameterString(c_name, c_value);

	return 0;
}

int F2C(c_tree_timer_flush_trace_data, C_TREE_TIMER_FLUSH_TRACE_DATA)()
{
	TreeTimerFlushTraceData();

	return 0;
}
