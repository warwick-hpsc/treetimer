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

#ifndef TT_F2C_WRAPPER_INCLUDE_H
#define TT_F2C_WRAPPER_INCLUDE_H

#include "f2c.h"

#ifdef  __cplusplus
extern "C" {
#endif

int F2C(c_tree_timer_init, C_TREE_TIMER_INIT)();
int F2C(c_tree_timer_finalize, C_TREE_TIMER_FINALIZE)();

int F2C(c_tree_timer_log_parameter_int, C_TREE_TIMER_LOG_PARAMETER_INT)(const char * name, int * text_len, int * value);
int F2C(c_tree_timer_log_parameter_double, C_TREE_TIMER_LOG_PARAMETER_DOUBLE)(const char * name, int * text_len,  double * value);
int F2C(c_tree_timer_log_parameter_bool, C_TREE_TIMER_LOG_PARAMETER_BOOL)(const char * name, int * text_len,  int * value);

int F2C(c_tree_timer_enter_trace_conductor, TreeTimerEnterTraceConductor)(const char * name, int * text_len, int * trace_call_interval);
int F2C(c_tree_timer_enter_method, C_TREE_TIMER_ENTER_METHOD)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_loop, C_TREE_TIMER_ENTER_LOOP)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_compute, C_TREE_TIMER_ENTER_COMPUTE)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block, C_TREE_TIMER_ENTER_BLOCK)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_mpi_comm_call, C_TREE_TIMER_ENTER_MPI_COMM_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_mpi_sync_call, C_TREE_TIMER_ENTER_MPI_SYNC_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_mpi_collective_call, C_TREE_TIMER_ENTER_MPI_COLLECTIVE_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_io_call, C_TREE_TIMER_ENTER_IO_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_library_call, C_TREE_TIMER_ENTER_LIBRARY_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_exit, C_TREE_TIMER_EXIT)(const char * name, int * text_len);

int F2C(c_tree_timer_log_global_parameter_int, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_INT)(const char * name, int * text_len, int * value);
int F2C(c_tree_timer_log_global_parameter_double, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_DOUBLE)(const char * name, int * text_len, double * value);
int F2C(c_tree_timer_log_global_parameter_bool, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_BOOL)(const char * name, int * text_len, int * value);
int F2C(c_tree_timer_log_global_parameter_string, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_BOOL)(const char * name, int * name_len, const char * value, int * value_len);

int F2C(c_tree_timer_flush_trace_data, C_TREE_TIMER_FLUSH_TRACE_DATA)();

#ifdef  __cplusplus
}
#endif


#endif
