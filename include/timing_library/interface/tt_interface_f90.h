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

int F2C(c_tree_timer_enter_block_method, C_TREE_TIMER_ENTER_BLOCK_METHOD)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_loop, C_TREE_TIMER_ENTER_BLOCK_LOOP)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_compute_loop, C_TREE_TIMER_ENTER_BLOCK_COMPUTE_LOOP)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_unspecified, C_TREE_TIMER_ENTER_BLOCK_UNSPECIFIED)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_non_mpi_call, C_TREE_TIMER_ENTER_BLOCK_NON_MPI_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_mpi_comm_call, C_TREE_TIMER_ENTER_BLOCK_MPI_COMM_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_mpi_non_comm_call, C_TREE_TIMER_ENTER_BLOCK_MPI_COMM_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_enter_block_io_call, C_TREE_TIMER_ENTER_BLOCK_IO_CALL)(const char * name, int * text_len);
int F2C(c_tree_timer_exit_block, C_TREE_TIMER_EXIT_BLOCK)(const char * name, int * text_len);

int F2C(c_tree_timer_log_global_parameter_int, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_INT)(const char * name, int * text_len, int * value);
int F2C(c_tree_timer_log_global_parameter_double, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_DOUBLE)(const char * name, int * text_len, double * value);
int F2C(c_tree_timer_log_global_parameter_bool, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_BOOL)(const char * name, int * text_len, int * value);
int F2C(c_tree_timer_log_global_parameter_string, C_TREE_TIMER_LOG_GLOBAL_PARAMETER_BOOL)(const char * name, int * name_len, const char * value, int * value_len);

int F2C(c_tree_timer_flush_trace_data, C_TREE_TIMER_FLUSH_TRACE_DATA)();

#ifdef  __cplusplus
}
#endif


#endif
