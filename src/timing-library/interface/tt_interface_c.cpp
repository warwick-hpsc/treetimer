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

#include "tt_interface_c.h"
#include "tt_core.h"
#include <string>
#include "tt_code_block_type.h"
#include "tt_global.h"

void TreeTimerInit()
{
	treetimer::core::drivers::TreeTimerInit();
}

void TreeTimerFinalize()
{
	treetimer::core::drivers::TreeTimerFinalize();
}

void TreeTimerEnterBlockMethod(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_METHOD);
}

void TreeTimerEnterBlockLoop(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_LOOP);
}

void TreeTimerEnterBlockComputeLoop(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_COMPUTE_LOOP);
}

void TreeTimerEnterBlockUnspecified(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_UNSPECIFIED);
}

void TreeTimerEnterBlockNonMPICall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_NONMPI_CALL);
}

void TreeTimerEnterBlockMPICommCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_MPI_COMM_CALL);
}

void TreeTimerEnterBlockMPINonCommCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_MPI_NONCOMM_CALL);
}

void TreeTimeEnterBlockIOCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_MPI_IO_CALL);
}

void TreeTimerExitBlock(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerExitBlock(strName);
}

void TreeTimerLogParameterInt(const char * paramName, int value)
{
	std::string strName(paramName);
	treetimer::core::drivers::TreeTimerLogParameter(strName, value);
}

void TreeTimerLogParameterDouble(const char * paramName, double value)
{
	std::string strName(paramName);
	treetimer::core::drivers::TreeTimerLogParameter(strName, value);
}

void TreeTimerLogParameterBool(const char * paramName, int value)
{
	std::string strName(paramName);
	treetimer::core::drivers::TreeTimerLogParameter(strName, (bool) value);
}

void TreeTimerLogGlobalParameterInt(const char * paramName, int value)
{
	std::string strName(paramName);
	treetimer::config::drivers::setGlobalParam(*(treetimer::core::instrumState->config), strName, value);
}

void TreeTimerLogGlobalParameterDouble(const char * paramName, double value)
{
	std::string strName(paramName);
	treetimer::config::drivers::setGlobalParam(*(treetimer::core::instrumState->config), strName, value);
}

void TreeTimerLogGlobalParameterBool(const char * paramName, int value)
{
	std::string strName(paramName);
	treetimer::config::drivers::setGlobalParam(*(treetimer::core::instrumState->config), strName, (bool) value);
}

void TreeTimerLogGlobalParameterString(const char * paramName, const char * value)
{
	std::string strName(paramName);
	treetimer::config::drivers::setGlobalParam(*(treetimer::core::instrumState->config), strName, std::string(value));
}

void TreeTimerFlushTraceData()
{
	treetimer::core::instrumState->config->inLibrary = true;
	treetimer::core::drivers::TreeTimerFlushTraceData();
	treetimer::core::instrumState->config->inLibrary = false;
}
