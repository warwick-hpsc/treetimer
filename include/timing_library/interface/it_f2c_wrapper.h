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

#ifndef IT_F2C_WRAPPER_INCLUDE_H
#define IT_F2C_WRAPPER_INCLUDE_H

#include "it_instrum.h"
#include "f2c.h"

// Init and Finalize Functions
int F2C(c_it_init, C_IT_INIT)();
int F2C(c_it_finalize, C_IT_FINALIZE)();

// Node Entry Functions
int F2C(c_it_enter, C_IT_ENTER)(const char * name, int * text_len, int * nodeType, int * dummy, int * dummy2, int * dummy3);
int F2C(c_it_exit, C_IT_EXIT)(const char * name, int * text_len);

// Parameter Storage Functions
int F2C(c_it_parameter_update_d, C_IT_PARAMETER_UPDATE_D)(const char * name, int * text_len, double * val);
int F2C(c_it_parameter_update_i, C_IT_PARAMETER_UPDATE_I)(const char * name, int * text_len, int * val);
int F2C(c_it_parameter_update_b, C_IT_PARAMETER_UPDATE_B)(const char * name, int * text_len, int * val);

// Other Functionality can be managed via environment variables

// =================================================================================
// ==========================  Function Implementations ============================
// =================================================================================

int F2C(c_it_init, C_IT_INIT)()
{
	it_init();
	return 0;
}

int F2C(c_it_finalize, C_IT_FINALIZE)()
{
	it_finalize();
	return 0;
}

// Node Entry Functions
int F2C(c_it_enter, C_IT_ENTER)(const char * name, int * text_len, int * nodeType, int * dummy, int * dummy2, int * dummy3)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	it_enter(c_name, *nodeType, *dummy, *dummy2, *dummy3);
}

int F2C(c_it_exit, C_IT_EXIT)(const char * name, int * text_len)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	it_exit(c_name);
}

// Parameter Storage Functions
int F2C(c_it_parameter_update_d, C_IT_PARAMETER_UPDATE_D)(const char * name, int * text_len, double * val)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	it_parameter_update_d(c_name, *val);
}

int F2C(c_it_parameter_update_i, C_IT_PARAMETER_UPDATE_I)(const char * name, int * text_len, int * val)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	it_parameter_update_i(c_name, *val);
}

int F2C(c_it_parameter_update_b, C_IT_PARAMETER_UPDATE_B)(const char * name, int * text_len, int * val)
{
	char c_name[*text_len + 1];
	F2C_strcpy(c_name, name, *text_len);

	it_parameter_update_b(c_name, *val);
}

#endif
