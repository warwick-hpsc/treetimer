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

#include "tt_io_sqlite3.h"

#include "tt_code_block_type.h"
#include "tt_global.h"

#include "tt_sqlite3_db_access.h"
#include "tt_sqlite3_db_aggregate_parameters.h"
#include "tt_sqlite3_db_aggregate_timers.h"
#include "tt_sqlite3_db_app_config.h"
#include "tt_sqlite3_db_app.h"
#include "tt_sqlite3_db_callpath.h"
#include "tt_sqlite3_db_compute_node.h"
#include "tt_sqlite3_db_cpu_core.h"
#include "tt_sqlite3_db_cpu_socket.h"
#include "tt_sqlite3_db_cpu.h"
#include "tt_sqlite3_db_lib_config.h"
#include "tt_sqlite3_db_machine.h"
#include "tt_sqlite3_db_parameters.h"
#include "tt_sqlite3_db_process.h"
#include "tt_sqlite3_db_profile_node.h"
#include "tt_sqlite3_db_profile_run_config.h"
#include "tt_sqlite3_db_profile_type.h"
#include "tt_sqlite3_db_trace_parameters.h"
#include "tt_sqlite3_db_trace_timers.h"

#include "mpi.h"

#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace tt_sql = treetimer::database::tt_sqlite3;

// Note! When performing intra-node gather of database records,
// MPI tags will be used to distinguish between different DB tables.
#define TAG_AGG 0
#define TAG_TRC 1

namespace treetimer
{
	namespace io
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				tt_sql::TTSQLite3* setupOutput(treetimer::config::Config& config)
				{
					// Create SQL Access Object
					tt_sql::TTSQLite3* dataAccess = new tt_sql::TTSQLite3(config.outputFolder + "/" + config.sqlIOFilename);
					tt_sql::drivers::openConnection(*dataAccess);

					if(config.sqlIOSetup == false)
					{
						// Setup the Database Schemas
						// (a) Block Types
						tt_sql::drivers::writeSchemaProfileNodeTypeData(*dataAccess);

						// (b) Library Configuration
						tt_sql::drivers::writeSchemaLibraryConfigData(*dataAccess);

						// (c) Singular Parameters (used for Global Parameter Storage)
						tt_sql::drivers::writeSchemaParameterData(*dataAccess);

						// (d) Application Data
						tt_sql::drivers::writeSchemaApplicationData(*dataAccess);

						// (e) Application Configuration Data (Links Combinations of Global Parameters)
						tt_sql::drivers::writeSchemaApplicationConfigData(*dataAccess);

						// (f) Profile Nodes
						tt_sql::drivers::writeSchemaProfileNodeData(*dataAccess);

						// (g) CPU Data
						tt_sql::drivers::writeSchemaCPUData(*dataAccess);

						// (h) Machine Data
						tt_sql::drivers::writeSchemaMachineData(*dataAccess);

						// (i) Compute Node Data
						tt_sql::drivers::writeSchemaComputeNodeData(*dataAccess);

						// (j) Compute Node Socket Data
						tt_sql::drivers::writeSchemaCPUSocketData(*dataAccess);

						// (k) Compute Core Data
						tt_sql::drivers::writeSchemaCPUCoreData(*dataAccess);

						// (l) Call Path Data
						tt_sql::drivers::writeSchemaCallPathData(*dataAccess);

						// (m) MPI Process Data
						tt_sql::drivers::writeSchemaProcessData(*dataAccess);

						// (n) Run Data (Configurations, Process Count etc)
						tt_sql::drivers::writeSchemaProfileRunConfigData(*dataAccess);

						// (o) Aggregate Parameter Data (Run Specific)
						tt_sql::drivers::writeSchemaAggregateParameterData(*dataAccess);

						// (p) Aggregate Timer Data (Run Specific)
						tt_sql::drivers::writeSchemaAggregateTimeData(*dataAccess);

						// (q) Trace Parameter Data (Run Specific)
						tt_sql::drivers::writeSchemaTraceParameterData(*dataAccess);

						// (r) Trace Timer Data (Run Specific)
						tt_sql::drivers::writeSchemaTraceTimeData(*dataAccess);

						config.sqlIOSetup = true;
					}

					return dataAccess;
				}

				void writeRunConfigData(treetimer::config::Config& config, tt_sql::TTSQLite3* dataAccess)
				{
					// Write data pertaining to the run configuration

					// Note: This can only be done once. Any global parameters set after this step will
					// not be stored. It must be done prior to writing any instrumentation data,
					// so should be performed prior to a trace/full data output if not yet performed.

					// At completion, set a suitable runID for reuse during any data writes.

					if(config.sqlIORunConfig == false)
					{
						// Profile Node Type Data

						for(int i = 0; i < TT_CODE_BLOCK_COUNT; i++)
						{
							int dbID;
							tt_sql::drivers::writeProfileNodeTypeData(*dataAccess, codeBlockNames[i], &dbID);
						}

						// Library Config Data
						int libConfigID;
						tt_sql::drivers::writeLibraryConfigID(*dataAccess, config.libVerMajor, config.libVerMinor,
																config.eATimers, config.eTTimers,
																config.eAParam, config.eTParam,
																config.eAPAPI, config.eTPAPI,
																config.eMPIHooks, &libConfigID);
						// Application Data
						int appID;
						tt_sql::drivers::writeApplicationData(*dataAccess, config.appName, config.appVersion, &appID);

						// Machine Data
						int machineID;
						tt_sql::drivers::writeMachineData(*dataAccess, config.machineName, &machineID);

						// CPU Data
						int cpuID;
						std::string cpuModel = "Unknown";
						tt_sql::drivers::writeCPUData(*dataAccess, cpuModel, &cpuID);

						// Compute Node Data
						int computeNodeID;
						std::string nodeName = "Unknown";
						int socketCount = -1;
						tt_sql::drivers::writeComputeNodeData(*dataAccess, machineID, nodeName,
																socketCount, &computeNodeID);

						// CPU Socket Data
						int cpuSocketID;
						int phySocketNum = -1;
						tt_sql::drivers::writeCPUSocketData(*dataAccess, computeNodeID, cpuID, phySocketNum, &cpuSocketID);

						// CPU Core Data
						int cpuCoreID;
						int physicalCoreNum = -1;
						tt_sql::drivers::writeCPUCoreData(*dataAccess, cpuSocketID, physicalCoreNum, &cpuCoreID);

						// ProcessID
						int processID;
						int rank;
						MPI_Comm_rank(MPI_COMM_WORLD, &rank);
						tt_sql::drivers::writeProcessData(*dataAccess, cpuCoreID,  rank, &processID);

						// App Config Data
						// Build Vectors of the global parameters
						std::vector<std::string> intParamNames;
						std::vector<int> intParamValues;

						std::vector<std::string> doubleParamNames;
						std::vector<double> doubleParamValues;

						std::vector<std::string> boolParamNames;
						std::vector<bool> boolParamValues;

						std::vector<std::string> stringParamNames;
						std::vector<std::string> stringParamValues;

						std::unordered_map<std::string, treetimer::parameters::ParameterSingular<int> *>::iterator it1;
						for(it1 = config.intGlobalParams.begin(); it1 != config.intGlobalParams.end(); it1++)
						{
							intParamNames.push_back(it1->first);
							intParamValues.push_back(it1->second->value);
						}

						std::unordered_map<std::string, treetimer::parameters::ParameterSingular<double> *>::iterator it2;
						for(it2 = config.doubleGlobalParams.begin(); it2 != config.doubleGlobalParams.end(); it2++)
						{
							doubleParamNames.push_back(it2->first);
							doubleParamValues.push_back(it2->second->value);
						}

						std::unordered_map<std::string, treetimer::parameters::ParameterSingular<bool> *>::iterator it3;
						for(it3 = config.boolGlobalParams.begin(); it3 != config.boolGlobalParams.end(); it3++)
						{
							boolParamNames.push_back(it3->first);
							boolParamValues.push_back(it3->second->value);
						}

						std::unordered_map<std::string, treetimer::parameters::ParameterSingular<std::string> *>::iterator it4;
						for(it4 = config.stringGlobalParams.begin(); it4 != config.stringGlobalParams.end(); it4++)
						{
							stringParamNames.push_back(it4->first);
							stringParamValues.push_back(it4->second->value);
						}

						// Application Config Data
						int appConfigID;
						tt_sql::drivers::writeApplicationConfigData(*dataAccess,
																	appID,
																	intParamNames, intParamValues,
																	doubleParamNames, doubleParamValues,
																	boolParamNames, boolParamValues,
																	stringParamNames, stringParamValues,
																	&appConfigID);

						// Run Config Data (Runs are always recorded as new)
						int processCount;
						int runID;
						MPI_Comm_size(MPI_COMM_WORLD, &processCount);
						tt_sql::drivers::writeProfileRunConfigData(*dataAccess,
																	appConfigID,
																	libConfigID,
																	processCount,
																	&runID);

						config.sqlIORunID = runID;
						config.sqlIOProcessID = processID;

						config.sqlIORunConfig = true;
					}
				}

				void writeAggData(treetimer::config::Config& config,
								  treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
								  tt_sql::TTSQLite3* dataAccess)
				{
					// ToDo: Error Check - ensure database has been setup

					if(config.sqlIOAggData == true)
					{
						return;
					}

					if (callTree.root == nullptr) {
						// Nothing to do
						return;
					}

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeAggInstrumentationData, config.sqlIORunID, config.sqlIOProcessID, -1, config);

					// Create MPI type for a AggregateTime record:
					int err;
					MPI_Datatype aggTimeRecord_MPI;
					int lengths[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					displacements[0] = offsetof(tt_sql::TTAggTiming, runID);
					displacements[1] = offsetof(tt_sql::TTAggTiming, rank);
					displacements[2] = offsetof(tt_sql::TTAggTiming, callPathID);
					displacements[3] = offsetof(tt_sql::TTAggTiming, processID);
					displacements[4] = offsetof(tt_sql::TTAggTiming, minWallTime);
					displacements[5] = offsetof(tt_sql::TTAggTiming, avgWallTime);
					displacements[6] = offsetof(tt_sql::TTAggTiming, maxWallTime);
					displacements[7] = offsetof(tt_sql::TTAggTiming, stdev);
					displacements[8] = offsetof(tt_sql::TTAggTiming, count);
					MPI_Datatype types[9] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, 
												MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, 
												MPI_INT };
					err = MPI_Type_create_struct(9, lengths, displacements, types, &aggTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Rank %d failed to create custom type for aggTimeRecord\n", dataAccess->rankGlobal);
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&aggTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Rank %d failed to commit custom type for aggTimeRecord\n", dataAccess->rankGlobal);
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					// Perform intra-node gather-at-root:
					if (dataAccess->nRanksLocal > 1) {
						if (dataAccess->rankLocal == 0) {
							int n = dataAccess->nRanksLocal;

							// New gather logic: switch probe to non-blocking. Above use of blocking probe
							// was prevening any async comms from occurring.
							int nReceives = 0;
							bool workDone = false;
							int ready = 0;
							MPI_Status stat;
							MPI_Request req;
							int bufferSize = 0;
							tt_sql::TTAggTiming* buffer = NULL;
							int aggTimeID;
							while (nReceives != (n-1)) {
								for (int r=1; r<n; r++) {
									err = MPI_Iprobe(r, TAG_AGG, dataAccess->nodeComm, &ready, &stat);
									if (err != MPI_SUCCESS) {
										fprintf(stderr, "Root rank failed to probe msg from %d\n", r);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}

									if (ready == 1) {
										workDone = true;
										nReceives++;

										err = MPI_Get_count(&stat, aggTimeRecord_MPI, &bufferSize);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Root failed on MPI_Get_count(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										if (bufferSize == 0) {
											buffer = (tt_sql::TTAggTiming*)malloc(1 * sizeof(tt_sql::TTAggTiming));
										} else {
											buffer = (tt_sql::TTAggTiming*)malloc(bufferSize * sizeof(tt_sql::TTAggTiming));
										}
										err = MPI_Recv(buffer, bufferSize, aggTimeRecord_MPI, r, TAG_AGG, dataAccess->nodeComm, &stat);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Root failed on MPI_Recv(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										for (int i=0; i<bufferSize; i++) {
											tt_sql::drivers::writeAggregateTimeData(*dataAccess, buffer[i], &aggTimeID);
										}

										free(buffer); buffer = NULL; bufferSize = 0;
									}
								}
								if (!workDone) sleep(1);
							}
						}
						else {
							// Send to local root
							MPI_Request req;
							MPI_Status stat;
							err = MPI_Isend(dataAccess->aggTimeRecords.data(), dataAccess->aggTimeRecords.size(), aggTimeRecord_MPI, 0, TAG_AGG, dataAccess->nodeComm, &req);
							if (err != MPI_SUCCESS) {
								fprintf(stderr, "Rank %d failed Isend\n", dataAccess->rankGlobal);
								MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
							}
							MPI_Wait(&req, &stat);
							dataAccess->aggTimeRecords.clear();
						}
					}


					config.sqlIOAggData = true;
				}

				void writeTraceData(treetimer::config::Config& config,
									treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
									tt_sql::TTSQLite3* dataAccess)
				{
					// ToDo: Error Check - ensure database has been setup

					if (callTree.root == nullptr) {
						// Nothing to do
						return;
					}

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeTraceInstrumentationData, config.sqlIORunID, config.sqlIOProcessID, -1, config);

					// Create MPI type for a TraceTime record:
					int err;
					MPI_Datatype traceTimeRecord_MPI;
					int lengths[7] = {1, 1, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					displacements[0] = offsetof(tt_sql::TTTraceTiming, runID);
					displacements[1] = offsetof(tt_sql::TTTraceTiming, rank);
					displacements[2] = offsetof(tt_sql::TTTraceTiming, callPathID);
					displacements[3] = offsetof(tt_sql::TTTraceTiming, processID);
					displacements[4] = offsetof(tt_sql::TTTraceTiming, nodeEntryID);
					displacements[5] = offsetof(tt_sql::TTTraceTiming, nodeExitID);
					displacements[6] = offsetof(tt_sql::TTTraceTiming, walltime);
					MPI_Datatype types[7] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, 
												MPI_INT, MPI_INT, MPI_DOUBLE };
					err = MPI_Type_create_struct(7, lengths, displacements, types, &traceTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Rank %d failed to create custom type for traceTimeRecord\n", dataAccess->rankGlobal);
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&traceTimeRecord_MPI);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Rank %d failed to commit custom type for traceTimeRecord\n", dataAccess->rankGlobal);
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}

					// Perform intra-node gather-at-root:
					if (dataAccess->nRanksLocal > 1) {
						if (dataAccess->rankLocal == 0) {
							int n = dataAccess->nRanksLocal;

							// New gather logic: switch probe to non-blocking. Above use of blocking probe
							// was prevening any async comms from occurring.
							int nReceives = 0;
							bool workDone = false;
							int ready = 0;
							MPI_Status stat;
							MPI_Request req;
							int bufferSize = 0;
							tt_sql::TTTraceTiming* buffer = NULL;
							int traceTimeID;
							while (nReceives != (n-1)) {
								for (int r=1; r<n; r++) {
									err = MPI_Iprobe(r, TAG_TRC, dataAccess->nodeComm, &ready, &stat);
									if (err != MPI_SUCCESS) {
										fprintf(stderr, "Gather rank failed to probe msg from %d\n", r);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}

									if (ready == 1) {
										workDone = true;
										nReceives++;

										err = MPI_Get_count(&stat, traceTimeRecord_MPI, &bufferSize);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Gather rank failed on MPI_Get_count(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										if (bufferSize == 0) {
											buffer = (tt_sql::TTTraceTiming*)malloc(1 * sizeof(tt_sql::TTTraceTiming));
										} else {
											buffer = (tt_sql::TTTraceTiming*)malloc(bufferSize * sizeof(tt_sql::TTTraceTiming));
										}
										err = MPI_Recv(buffer, bufferSize, traceTimeRecord_MPI, r, TAG_TRC, dataAccess->nodeComm, &stat);
										if (err != MPI_SUCCESS) {
											fflush(stdout); sleep(1);
											fprintf(stderr, "Root failed on MPI_Recv(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										for (int i=0; i<bufferSize; i++) {
											tt_sql::drivers::writeTraceTimeData(*dataAccess, buffer[i], &traceTimeID);
										}

										free(buffer); buffer = NULL; bufferSize = 0;
									}
								}
								if (!workDone) sleep(1);
							}
						}
						else {
							// Send to local root
							MPI_Request r;
							MPI_Status s;
							err = MPI_Isend(dataAccess->traceTimeRecords.data(), dataAccess->traceTimeRecords.size(), traceTimeRecord_MPI, 0, TAG_TRC, dataAccess->nodeComm, &r);
							if (err != MPI_SUCCESS) {
								fprintf(stderr, "Rankd %d failed Isend\n", r);
								MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
							}
							MPI_Wait(&r, &s);
							dataAccess->traceTimeRecords.clear();
						}
					}

					// We may wish to write trace data multiple times, this is just an indicator that we've done it at least once
					config.sqlIOTraceData = true;
				}

				void writeTreeNodeAggInstrumentationData(tt_sql::TTSQLite3& dataAccess,
														treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
														int runID, int processID, int parentID, int * callPathID, treetimer::config::Config& config)
				{

					// Instrumentation Data can include:
					// (a) A new profile node entry
					// (b) A new call path entry
					// (c) New aggregate time data
					// (d) New trace time data
					// (e) New aggregate parameter data
					// (f) New trace parameter data

					// We could make separate traversal functions for each piece of data, but more efficient to process a node entirely
					// so we don't have to incur as many ID lookups for previously entered data (such as Profile Node IDs).
					// This function operates as the main recursive function, and enters the instrumentation data that is applicable for
					// a node into the database.

					// Each of the pieces of data have their own insertion function for their relevant piece of data, but they may have
					// differing requirements for existing data (i.e. they may need to be provided foreign key values)

					// Retrieve this node's block type ID
					int blockTypeID;
					tt_sql::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.nodeData.blockType], &blockTypeID);

					// (a) Profile Nodes
					int profileNodeID;
					tt_sql::drivers::writeTreeNodeProfileNodeData(dataAccess, node.key, blockTypeID, &profileNodeID);

					// (b) Call Path Data
					// CallPath Data should have been written for the parent as part of the tree traversal, and identified by parentID.

					// ToDo: Is there a better way of handling no parent (i.e. root node?) - NULL would be equivalent.
					// Could create an 'unknown' profile node to point to, that has a profile node entry but no callpath node entry
					if(node.parent != nullptr)
					{
						// Get details of parent
						//int parentNodeID;
						//int parentBlockType;

						//std::string parentName = node.parent->key;
						//tt_sql::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.parent->nodeData.blockType], &parentBlockType);
						//tt_sql::drivers::findProfileNodeID(dataAccess, parentName, parentBlockType, &parentNodeID);

						tt_sql::drivers::writeCallPathData(dataAccess, profileNodeID, parentID, callPathID);
					}
					else
					{
						tt_sql::drivers::writeCallPathData(dataAccess, profileNodeID, -1, callPathID);
					}

					// (c) Aggregate Time Data
					// Only write data if aggregate timers are enabled

					if(config.eATimers)
					{
						int aggTimeID;
						tt_sql::TTAggTiming d;
						d.runID = runID;
						d.rank = dataAccess.rankGlobal;
						d.callPathID = *callPathID;
						d.processID = processID;
						d.minWallTime = node.nodeData.blockTimer->aggTimings.minWalltime;
						d.avgWallTime = node.nodeData.blockTimer->aggTimings.avgWalltime;
						d.maxWallTime = node.nodeData.blockTimer->aggTimings.maxWalltime;
						d.stdev = sqrt(node.nodeData.blockTimer->aggTimings.varianceWalltime);
						d.count = node.nodeData.blockTimer->aggTimings.count;
						tt_sql::drivers::writeAggregateTimeData(dataAccess, d, &aggTimeID);
					}

					// (d) Aggregate Parameter Data

					if(config.eAParam)
					{
						// Loop over and write each aggregate parameter

						std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>::iterator it_int;

						// Int Parameters;
						for(it_int = node.nodeData.intParameters.begin(); it_int != node.nodeData.intParameters.end(); it_int++)
						{
							int aggParamID;
							tt_sql::drivers::writeAggregateParameterIntData(
													   dataAccess, runID, *callPathID, processID, it_int->first,
													   it_int->second->aggParam.minVal,
													   it_int->second->aggParam.avgVal,
													   it_int->second->aggParam.maxVal,
													   sqrt(it_int->second->aggParam.variance),
													   it_int->second->aggParam.count,
													   &aggParamID);
						}

						std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::iterator it_float;

						// Float Parameters;
						for(it_float = node.nodeData.doubleParameters.begin(); it_float != node.nodeData.doubleParameters.end(); it_float++)
						{
							int aggParamID;
							tt_sql::drivers::writeAggregateParameterFloatData(
													   dataAccess, runID, *callPathID, processID, it_float->first,
													   it_float->second->aggParam.minVal,
													   it_float->second->aggParam.avgVal,
													   it_float->second->aggParam.maxVal,
													   sqrt(it_float->second->aggParam.variance),
													   it_float->second->aggParam.count,
													   &aggParamID);
						}

						std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::iterator it_bool;

						// Bool Parameters;
						for(it_bool = node.nodeData.boolParameters.begin(); it_bool != node.nodeData.boolParameters.end(); it_bool++)
						{
							int aggParamID;
							tt_sql::drivers::writeAggregateParameterBoolData(
													   dataAccess, runID, *callPathID, processID, it_bool->first,
													   it_bool->second->aggParam.minVal,
													   it_bool->second->aggParam.avgVal,
													   it_bool->second->aggParam.maxVal,
													   sqrt(it_bool->second->aggParam.variance),
													   it_bool->second->aggParam.count,
													   &aggParamID);
						}
					}


				}

				void writeTreeNodeTraceInstrumentationData(tt_sql::TTSQLite3& dataAccess,
													  treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													  int runID, int processID, int parentID, int * callPathID, treetimer::config::Config& config)
				{

					// Instrumentation Data can include:
					// (a) A new profile node entry
					// (b) A new call path entry
					// (c) New aggregate time data
					// (d) New trace time data
					// (e) New aggregate parameter data
					// (f) New trace parameter data

					// We could make separate traversal functions for each piece of data, but more efficient to process a node entirely
					// so we don't have to incur as many ID lookups for previously entered data (such as Profile Node IDs).
					// This function operates as the main recursive function, and enters the instrumentation data that is applicable for
					// a node into the database.

					// Each of the pieces of data have their own insertion function for their relevant piece of data, but they may have
					// differing requirements for existing data (i.e. they may need to be provided foreign key values)

					// Retrieve this node's block type ID
					int blockTypeID;
					tt_sql::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.nodeData.blockType], &blockTypeID);

					// (a) Profile Nodes
					int profileNodeID;
					tt_sql::drivers::writeTreeNodeProfileNodeData(dataAccess, node.key, blockTypeID, &profileNodeID);

					// (b) Call Path Data
					// CallPath Data should have been written for the parent as part of the tree traversal, and identified by parentID.

					// ToDo: Is there a better way of handling no parent (i.e. root node?) - NULL would be equivalent.
					// Could create an 'unknown' profile node to point to, that has a profile node entry but no callpath node entry
					if(node.parent != nullptr)
					{
						tt_sql::drivers::writeCallPathData(dataAccess, profileNodeID, parentID, callPathID);
					}
					else
					{
						tt_sql::drivers::writeCallPathData(dataAccess, profileNodeID, -1, callPathID);
					}

					// (c) Trace Times

					if(config.eTTimers)
					{
						// Loop over trace entries for the timer
						treetimer::data_structures::LinkedListNode<treetimer::timers::TraceTimer> * ptr = node.nodeData.blockTimer->traceTimers.head;

						while(ptr != nullptr)
						{
							int traceTimeID;
							tt_sql::TTTraceTiming d;
							d.runID = runID;
							d.rank = dataAccess.rankGlobal;
							d.callPathID = *callPathID;
							d.processID = processID;
							d.nodeEntryID = ptr->data.callEntryID;
							d.nodeExitID  = ptr->data.callExitID;
							d.walltime    = ptr->data.wallTime;
							tt_sql::drivers::writeTraceTimeData(dataAccess, d, &traceTimeID);

							ptr = ptr->next;
						}
					}

					// (d) Trace Parameters
					if(config.eTParam)
					{

						std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>::iterator it_int;

						// Int Parameters;
						for(it_int = node.nodeData.intParameters.begin(); it_int != node.nodeData.intParameters.end(); it_int++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<int>> * ptr = it_int->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::drivers::writeTraceParameterIntData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeEntryID, ptr->data.nodeExitID,
										 it_int->first,
										 ptr->data.val,
										 &traceParamID);

								ptr = ptr->next;
							}
						}


						// Floats/Doubles
						std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::iterator it_double;

						for(it_double = node.nodeData.doubleParameters.begin(); it_double != node.nodeData.doubleParameters.end(); it_double++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<double>> * ptr = it_double->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::drivers::writeTraceParameterFloatData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeEntryID, ptr->data.nodeExitID,
										 it_double->first,
										 ptr->data.val,
										 &traceParamID);

								ptr = ptr->next;
							}
						}

						// Bool Parameters;
						std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::iterator it_bool;

						for(it_bool = node.nodeData.boolParameters.begin(); it_bool != node.nodeData.boolParameters.end(); it_bool++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<bool>> * ptr = it_bool->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::drivers::writeTraceParameterBoolData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeEntryID, ptr->data.nodeExitID,
										 it_bool->first,
										 ptr->data.val,
										 &traceParamID);

								ptr = ptr->next;
							}
						}

						// String Parameters
						std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>::iterator it_string;

						for(it_string = node.nodeData.stringParameters.begin(); it_string != node.nodeData.stringParameters.end(); it_string++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<std::string>> * ptr = it_string->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::drivers::writeTraceParameterStringData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeEntryID, ptr->data.nodeExitID,
										 it_string->first,
										 ptr->data.val,
										 &traceParamID);

								ptr = ptr->next;
							}
						}
					}

				}

				void callTreeTraversal(tt_sql::TTSQLite3& dataAccess,
									   treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
									   void (*func)(tt_sql::TTSQLite3& dataAccess,
											   		treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													int runID, int processID,
													int parentID, int * callPathID,
													treetimer::config::Config& config),
													int runID, int processID, int parentID, treetimer::config::Config& config)
				{
					// Run function on current node
					// The retrived callPathID will be reused as the parentID for the child nodes.
					int callPathID;
					(*func)(dataAccess, node, runID, processID, parentID, &callPathID, config);

					std::unordered_map<std::string, treetimer::data_structures::TreeNode<std::string,treetimer::measurement::InstrumentationData> *>::iterator it;

					// Loop over child nodes
					for(it = node.childNodes.begin(); it != node.childNodes.end(); it ++)
					{
						callTreeTraversal(dataAccess, *(it->second), func, runID, processID, callPathID, config);
					}
				}
			}
		}
	}
}
