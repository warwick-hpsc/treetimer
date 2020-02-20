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
 * Has to also be suitable for inclusion in C source files
 */

#ifndef TT_CODE_BLOCK_TYPE_INCLUDE_H
#define TT_CODE_BLOCK_TYPE_INCLUDE_H

#include <string>

//ToDo: Fix namespace

#define TT_CODE_BLOCK_COUNT 9

enum CodeBlockType {
	TT_NODE_TYPE_PROG = 0,
	TT_NODE_TYPE_METHOD = 1,
	TT_NODE_TYPE_LOOP = 2,
	TT_NODE_TYPE_NONMPI_CALL = 3,
	TT_NODE_TYPE_UNSPECIFIED = 4,
	TT_NODE_TYPE_MPI_COMM_CALL = 5,
	TT_NODE_TYPE_MPI_NONCOMM_CALL = 6,
	TT_NODE_TYPE_MPI_IO_CALL = 7,
	TT_NODE_TYPE_COMPUTE_LOOP = 8
};

extern const std::string codeBlockNames[TT_CODE_BLOCK_COUNT];

#endif
