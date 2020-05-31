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

#ifndef TT_INTERFACE_C_INCLUDE_H
#define TT_INTERFACE_C_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

void TreeTimerInit();
void TreeTimerFinalize();

void TreeTimerLogParameterInt(const char * paramName, int value);
void TreeTimerLogParameterDouble(const char * paramName, double value);
void TreeTimerLogParameterBool(const char * paramName, int value);

void TreeTimerEnterBlockMethod(const char * blockName);
void TreeTimerEnterBlockLoop(const char * blockName);
void TreeTimerEnterBlockComputeLoop(const char * blockName);
void TreeTimerEnterBlockUnspecified(const char * blockName);
void TreeTimerEnterBlockNonMPICall(const char * blockName);
void TreeTimerEnterBlockMPICommCall(const char * blockName);
void TreeTimerEnterBlockMPISyncCall(const char * blockName);
void TreeTimerEnterBlockMPINonCommCall(const char * blockName);
void TreeTimeEnterBlockIOCall(const char * blockName);

void TreeTimerExitBlock(const char * blockName);

void TreeTimerLogGlobalParameterInt(const char * paramName, int value);
void TreeTimerLogGlobalParameterDouble(const char * paramName, double value);
void TreeTimerLogGlobalParameterBool(const char * paramName, int value);
void TreeTimerLogGlobalParameterString(const char * paramName, const char * value);

void TreeTimerFlushTraceData();

#ifdef  __cplusplus
}
#endif

#endif
