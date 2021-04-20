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

void TreeTimerEnterTraceConductor(const char * name, int traceCallInterval)
{
	treetimer::core::drivers::TreeTimerEnterTraceConductor(name, traceCallInterval);
}
void TreeTimerExitTraceConductor(const char * name)
{
	treetimer::core::drivers::TreeTimerExitTraceConductor(name);
}

void TreeTimerEnterMethod(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_METHOD);
}

void TreeTimerEnterLoop(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_LOOP);
}

void TreeTimerEnterCompute(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_COMPUTE);
}

void TreeTimerEnterBlock(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_BLOCK);
}

void TreeTimerEnterMPICommCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_MPI_COMM_CALL);
}

void TreeTimerEnterMPISyncCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_MPI_SYNC_CALL);
}

void TreeTimerEnterMPICollectiveCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_MPI_COLLECTIVE_CALL);
}

void TreeTimerEnterIOCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_IO_CALL);
}

void TreeTimerEnterLibraryCall(const char * blockName)
{
	std::string strName(blockName);
	treetimer::core::drivers::TreeTimerEnterBlock(strName, TT_NODE_TYPE_LIB_CALL);
}

void TreeTimerExit(const char * blockName)
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
