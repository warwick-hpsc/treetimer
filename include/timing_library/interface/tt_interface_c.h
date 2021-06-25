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

void TreeTimerSleep();
void TreeTimerWake();

void TreeTimerLogParameterInt(const char * paramName, int value);
void TreeTimerLogParameterLong(const char * paramName, long value);
void TreeTimerLogParameterDouble(const char * paramName, double value);
void TreeTimerLogParameterBool(const char * paramName, int value);

void TreeTimerEnterTraceConductor(const char * name, int traceCallInterval);
void TreeTimerEnterMethod(const char * blockName);
void TreeTimerEnterLoop(const char * blockName);
void TreeTimerEnterCompute(const char * blockName);
// void TreeTimerEnterUnspecified(const char * blockName);
void TreeTimerEnterBlock(const char * blockName);
// void TreeTimerEnterNonMPICall(const char * blockName);
void TreeTimerEnterMPICommCall(const char * blockName);
void TreeTimerEnterMPISyncCall(const char * blockName);
void TreeTimerEnterMPICollectiveCall(const char * blockName);
// void TreeTimerEnterMPINonCommCall(const char * blockName);
void TreeTimerEnterIOCall(const char * blockName);
void TreeTimerEnterLibraryCall(const char * blockName);

void TreeTimerExit(const char * blockName);

void TreeTimerLogGlobalParameterInt(const char * paramName, int value);
void TreeTimerLogGlobalParameterDouble(const char * paramName, double value);
void TreeTimerLogGlobalParameterBool(const char * paramName, int value);
void TreeTimerLogGlobalParameterString(const char * paramName, const char * value);

void TreeTimerFlushTraceData();

#ifdef  __cplusplus
}
#endif

#endif
