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
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <map>

namespace tt_sql = treetimer::database::tt_sqlite3;

// Note! When performing intra-node gather of database records,
// MPI tags necessary to distinguish between different DB tables.
#define TAG_CALLPATH 0
#define TAG_HWINFO 1
#define TAG_AGG 11
#define TAG_AGG_PARAM_INT 12
#define TAG_AGG_PARAM_FLOAT 13
#define TAG_AGG_PARAM_BOOL 14
#define TAG_TRC 21

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

						if (!dataAccess->gatherIntraNode || dataAccess->rankLocal==0) {
							// (a) Block Types
							tt_sql::drivers::writeSchemaProfileNodeTypeData(*dataAccess);

							// (b) Library Configuration
							tt_sql::drivers::writeSchemaLibraryConfigData(*dataAccess);
						}

						// (c) Singular Parameters (used for Global Parameter Storage)
						tt_sql::drivers::writeSchemaParameterData(*dataAccess);

						// Disabled tables:
						// // (d) Application Data
						// tt_sql::drivers::writeSchemaApplicationData(*dataAccess);

						// // (e) Application Configuration Data (Links Combinations of Global Parameters)
						// tt_sql::drivers::writeSchemaApplicationConfigData(*dataAccess);

						// (n) Run Data (Configurations, Process Count etc)
						tt_sql::drivers::writeSchemaProfileRunConfigData(*dataAccess);

						// (f) Profile Nodes
						tt_sql::drivers::writeSchemaProfileNodeData(*dataAccess);

						// (h) Machine Data
						tt_sql::drivers::writeSchemaMachineData(*dataAccess);

						// (g) CPU Data
						tt_sql::drivers::writeSchemaCPUData(*dataAccess);
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
						// Profile Node Type Data - identical across ranks so no comms needed
						if (!dataAccess->gatherIntraNode || dataAccess->rankLocal==0) {
							for(int i = 0; i < TT_CODE_BLOCK_COUNT; i++)
							{
								int dbID;
								tt_sql::drivers::writeProfileNodeTypeData(*dataAccess, codeBlockNames[i], &dbID);
							}
						}

						// Library Config Data - assume identical across ranks, why would user 
						// configure different ranks differently? So assume no comms needed.
						if (!dataAccess->gatherIntraNode || dataAccess->rankLocal==0) {
							int libConfigID = -1;
							tt_sql::drivers::writeLibraryConfigID(*dataAccess, config.libVerMajor, config.libVerMinor,
																	config.eATimers, config.eTTimers,
																	config.eAParam, config.eTParam,
																	config.eAPAPI, config.eTPAPI,
																	config.eMPIHooks, &libConfigID);

							// Run Config Data (Runs are always recorded as new)
							int processCount;
							int runID;
							// Trimmed-down table should be identical across ranks, because 
							// it only contains rank count. So no comms needed. 
							// If 'appConfigID' is restored then re-evaluate decision.
							MPI_Comm_size(MPI_COMM_WORLD, &processCount);
							tt_sql::drivers::writeProfileRunConfigData(*dataAccess,
																		// appConfigID,
																		libConfigID,
																		processCount,
																		&runID);
							dataAccess->runID = runID;
						}

						// Useless table!
						// // Application Data
						// int appID;
						// tt_sql::drivers::writeApplicationData(*dataAccess, config.appName, config.appVersion, &appID);

						// Machine Data - machineName is global and constant.
						if (!dataAccess->gatherIntraNode || dataAccess->rankLocal==0) {
							int machineID;
							tt_sql::drivers::writeMachineData(*dataAccess, config.machineName, &machineID);
						}

						prepareAndWriteExecutionData(dataAccess);

						// // App Config Data
						// // Build Vectors of the global parameters
						// std::vector<std::string> intParamNames;
						// std::vector<int> intParamValues;

						// std::vector<std::string> doubleParamNames;
						// std::vector<double> doubleParamValues;

						// std::vector<std::string> boolParamNames;
						// std::vector<bool> boolParamValues;

						// std::vector<std::string> stringParamNames;
						// std::vector<std::string> stringParamValues;

						// std::unordered_map<std::string, treetimer::parameters::ParameterSingular<int> *>::iterator it1;
						// for(it1 = config.intGlobalParams.begin(); it1 != config.intGlobalParams.end(); it1++)
						// {
						// 	intParamNames.push_back(it1->first);
						// 	intParamValues.push_back(it1->second->value);
						// }

						// std::unordered_map<std::string, treetimer::parameters::ParameterSingular<double> *>::iterator it2;
						// for(it2 = config.doubleGlobalParams.begin(); it2 != config.doubleGlobalParams.end(); it2++)
						// {
						// 	doubleParamNames.push_back(it2->first);
						// 	doubleParamValues.push_back(it2->second->value);
						// }

						// std::unordered_map<std::string, treetimer::parameters::ParameterSingular<bool> *>::iterator it3;
						// for(it3 = config.boolGlobalParams.begin(); it3 != config.boolGlobalParams.end(); it3++)
						// {
						// 	boolParamNames.push_back(it3->first);
						// 	boolParamValues.push_back(it3->second->value);
						// }

						// std::unordered_map<std::string, treetimer::parameters::ParameterSingular<std::string> *>::iterator it4;
						// for(it4 = config.stringGlobalParams.begin(); it4 != config.stringGlobalParams.end(); it4++)
						// {
						// 	stringParamNames.push_back(it4->first);
						// 	stringParamValues.push_back(it4->second->value);
						// }

						// Useless table!
						// // Application Config Data
						// int appConfigID;
						// tt_sql::drivers::writeApplicationConfigData(*dataAccess,
						// 											appID,
						// 											intParamNames, intParamValues,
						// 											doubleParamNames, doubleParamValues,
						// 											boolParamNames, boolParamValues,
						// 											stringParamNames, stringParamValues,
						// 											&appConfigID);

						// config.sqlIORunID = runID;
						// config.sqlIOProcessID = processID;

						config.sqlIORunConfig = true;
					}
				}

				// int 
				void 
				prepareAndWriteExecutionData(tt_sql::TTSQLite3* dataAccess)
				{
					int processID = -1;

					std::string cpuModel = "Unknown";
					// std::string nodeName = "Unknown";
					char nodeNameStr[MAX_STRING_LENGTH];
					gethostname(nodeNameStr, MAX_STRING_LENGTH);
					std::string nodeName = nodeNameStr;
					// int socketCount = -1;
					// int phySocketNum = -1;
					// int physicalCoreNum = -1;
					int socketCount = 1;
					int phySocketNum = 0;
					// int physicalCoreNum = 2;
					int physicalCoreNum = sched_getcpu();
					int thread = 0;

					if (!dataAccess->gatherIntraNode || dataAccess->rankLocal == 0) {
						// CPU Data
						int cpuID;
						tt_sql::TT_Cpu d;
						// d.rank = dataAccess->rankGlobal;
						strncpy(d.cpuModel, cpuModel.c_str(), MAX_STRING_LENGTH);
						tt_sql::drivers::writeCPUData(*dataAccess, d, &cpuID);

						// Compute Node Data
						int computeNodeID;
						tt_sql::TT_ComputeNode d_cn;
						strncpy(d_cn.nodeName, nodeName.c_str(), MAX_STRING_LENGTH);
						d_cn.socketCount = socketCount;
						tt_sql::drivers::writeComputeNodeData(*dataAccess, 
																// machineID, 
																d_cn,
																&computeNodeID);
						// CPU Socket Data
						int cpuSocketID;
						tt_sql::TT_Socket d_skt;
						d_skt.phySocketNum = phySocketNum;
						tt_sql::drivers::writeCPUSocketData(*dataAccess, computeNodeID, cpuID, d_skt, &cpuSocketID);

						// CPU Core Data
						int cpuCoreID;
						tt_sql::TT_CpuCore d_cc;
						d_cc.physicalCoreNum = physicalCoreNum;
						tt_sql::drivers::writeCPUCoreData(*dataAccess, cpuSocketID, d_cc, &cpuCoreID);

						// ProcessID
						int rank;
						MPI_Comm_rank(MPI_COMM_WORLD, &rank);
						tt_sql::drivers::writeProcessData(*dataAccess, cpuCoreID, rank, thread, &processID);

						dataAccess->processID = processID;
						dataAccess->rankLocalToProcessID[0] = processID;
					}

					if (dataAccess->gatherIntraNode) {
						tt_sql::TT_HwInfo d;
						d.rankGlobal = dataAccess->rankGlobal;
						strncpy(d.cpuModel, cpuModel.c_str(), MAX_STRING_LENGTH);
						strncpy(d.nodeName, nodeName.c_str(), MAX_STRING_LENGTH);
						d.socketCount = socketCount;
						d.phySocketNum = phySocketNum;
						d.physicalCoreNum = physicalCoreNum;
						d.thread = thread;

						MPI_Datatype hwInfoRecord_MPI = tt_sql::drivers::createHwInfoMpiType();

						// Perform intra-node gather-at-root:
						int err;
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;

								int nReceives = 0;
								int hwInfoId;
								tt_sql::TT_HwInfo* records = NULL;
								int nRecords = 0;
								int srcRank;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_HwInfo), hwInfoRecord_MPI, TAG_HWINFO);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE); fflush(stdout);
									}
									nReceives++;
									for (int i=0; i<nRecords; i++) {
										tt_sql::TT_HwInfo r = records[i];
										dataAccess->rankLocalToRankGlobal[srcRank] = r.rankGlobal;

										int cpuID;
										tt_sql::TT_Cpu d;
										strcpy(d.cpuModel, r.cpuModel);
										tt_sql::drivers::writeCPUData(*dataAccess, d, &cpuID);

										// Compute Node Data
										int computeNodeID;
										tt_sql::TT_ComputeNode d_cn;
										strcpy(d_cn.nodeName, r.nodeName);
										d_cn.socketCount = r.socketCount;
										tt_sql::drivers::writeComputeNodeData(*dataAccess, 
																				d_cn,
																				&computeNodeID);

										// CPU Socket Data
										int cpuSocketID;
										tt_sql::TT_Socket d_skt;
										d_skt.phySocketNum = r.phySocketNum;
										tt_sql::drivers::writeCPUSocketData(*dataAccess, computeNodeID, cpuID, d_skt, &cpuSocketID);

										// CPU Core Data
										int cpuCoreID;
										tt_sql::TT_CpuCore d_cc;
										d_cc.physicalCoreNum = r.physicalCoreNum;
										tt_sql::drivers::writeCPUCoreData(*dataAccess, cpuSocketID, d_cc, &cpuCoreID);

										// ProcessID
										int rank;
										MPI_Comm_rank(MPI_COMM_WORLD, &rank);
										tt_sql::drivers::writeProcessData(*dataAccess, cpuCoreID, r.rankGlobal, thread, &processID);
										dataAccess->rankLocalToProcessID[srcRank] = processID;

									}
									free(records); records=NULL; nRecords=0;
								}
							}
							else {
								// Send to local root
								err = sendRecordsToRoot(*dataAccess, &d, 1, 
														sizeof(tt_sql::TT_HwInfo), hwInfoRecord_MPI, TAG_HWINFO);
								if (err != 0) {
									fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
							}
						}
					}
				}

				void prepareAndWriteAggData(treetimer::config::Config& config,
											treetimer::data_structures::Tree<std::string, 
											treetimer::measurement::InstrumentationData>& callTree,
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
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeAggInstrumentationData, 
										dataAccess->processID, 
										-1, config);

					if (dataAccess->gatherIntraNode) {
						int err;

						// Perform intra-node gather-at-root of SQL table data:

						// Before gather/writing any performance data, must first 
						// gather callpath data and adjust ID's for different ranks 
						// potentially having different callpath trees.
						std::map<int, int> callpathNodeIdRemap[dataAccess->nRanksLocal];
						MPI_Datatype callpathNodeRecord_MPI = tt_sql::drivers::createCallpathNodeMpiType();
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;

								int nReceives = 0;
								int callpathNodeID;
								tt_sql::TT_CallPathNode* records = NULL;
								int nRecords = 0;
								int srcRank;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_CallPathNode), callpathNodeRecord_MPI, TAG_CALLPATH);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									nReceives++;
									for (int i=0; i<nRecords; i++) {
										tt_sql::TT_CallPathNode r = records[i];

										// Apply remap:
										if (r.parentID != -1 && callpathNodeIdRemap[srcRank].find(r.parentID) != callpathNodeIdRemap[srcRank].end()) {
											r.parentID = callpathNodeIdRemap[srcRank][r.parentID];
										}
										
										int blockTypeID;
										tt_sql::drivers::findProfileNodeTypeID(*dataAccess, codeBlockNames[r.blockType], &blockTypeID);
										int profileNodeID;
										tt_sql::drivers::writeTreeNodeProfileNodeData(*dataAccess, r.nodeName, blockTypeID, &profileNodeID);
										int callPathID;
										tt_sql::drivers::writeCallPathData(*dataAccess, r.rank, profileNodeID, r.parentID, &callPathID);

										// Store the remap:
										callpathNodeIdRemap[srcRank][r.callPathID] = callPathID;
									}
									free(records); records=NULL; nRecords=0;
								}
							}
							else {
								// Send to local root
								err = sendRecordsToRoot(*dataAccess, dataAccess->callPathNodeRecords.data(), dataAccess->callPathNodeRecords.size(), 
														sizeof(tt_sql::TT_CallPathNode), callpathNodeRecord_MPI, TAG_CALLPATH);
								if (err != 0) {
									fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								dataAccess->callPathNodeRecords.clear();
							}
						}

						// Gather AggregateTime data (intra-node gather-at-root):
						MPI_Datatype aggTimeRecord_MPI = tt_sql::drivers::createAggregateTimeMpiType();
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;
								tt_sql::TT_AggTiming* records = NULL; int nRecords = 0;
								int nReceives = 0; int srcRank;
								int aggTimeID;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_AggTiming), aggTimeRecord_MPI, TAG_AGG);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									nReceives++;
									for (int i=0; i<nRecords; i++) {
										// Add in processID, which only root will know from earlier:
										records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
										// Remap callpath IDs:
										records[i].callPathID = callpathNodeIdRemap[srcRank][records[i].callPathID];
										tt_sql::drivers::writeAggregateTimeData(*dataAccess, records[i], &aggTimeID);
									}
									free(records); records=NULL; nRecords=0;
								}
							}
							else {
								// Send to local root
								err = sendRecordsToRoot(*dataAccess, dataAccess->aggTimeRecords.data(), dataAccess->aggTimeRecords.size(), 
														sizeof(tt_sql::TT_AggTiming), aggTimeRecord_MPI, TAG_AGG);
								if (err != 0) {
									fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								dataAccess->aggTimeRecords.clear();
							}
						}

						if(config.eAParam) {
							// Gather AggregateParameter* data (intra-node gather-at-root):

							// AggregateParamterInt:
							MPI_Datatype aggParamIntRecord_MPI = tt_sql::drivers::createAggregateParamIntMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									int aggParamIntID;
									tt_sql::TT_AggParamInt* records = NULL; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_AggParamInt), aggParamIntRecord_MPI, TAG_AGG_PARAM_INT);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeAggregateParameterIntData(*dataAccess, records[i], &aggParamIntID);
										}
										free(records); records=NULL; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->aggParamIntRecords.data(), dataAccess->aggParamIntRecords.size(), 
															sizeof(tt_sql::TT_AggParamInt), aggParamIntRecord_MPI, TAG_AGG_PARAM_INT);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->aggParamIntRecords.clear();
								}
							}
							
							// AggregateParamterFloat:
							MPI_Datatype aggParamFloatRecord_MPI = tt_sql::drivers::createAggregateParamFloatMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									int aggParamFloatID;
									tt_sql::TT_AggParamFloat* records = NULL; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_AggParamFloat), aggParamFloatRecord_MPI, TAG_AGG_PARAM_FLOAT);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeAggregateParameterFloatData(*dataAccess, records[i], &aggParamFloatID);
										}
										free(records); records=NULL; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->aggParamFloatRecords.data(), dataAccess->aggParamFloatRecords.size(), 
															sizeof(tt_sql::TT_AggParamFloat), aggParamFloatRecord_MPI, TAG_AGG_PARAM_FLOAT);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->aggParamFloatRecords.clear();
								}
							}
							
							// AggregateParamterBool:
							MPI_Datatype aggParamBoolRecord_MPI = tt_sql::drivers::createAggregateParamBoolMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									int aggParamBoolID;
									tt_sql::TT_AggParamBool* records = NULL; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_AggParamBool), aggParamBoolRecord_MPI, TAG_AGG_PARAM_BOOL);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeAggregateParameterBoolData(*dataAccess, records[i], &aggParamBoolID);
										}
										free(records); records=NULL; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->aggParamBoolRecords.data(), dataAccess->aggParamBoolRecords.size(), 
															sizeof(tt_sql::TT_AggParamBool), aggParamBoolRecord_MPI, TAG_AGG_PARAM_BOOL);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->aggParamBoolRecords.clear();
								}
							}

						}
					}


					config.sqlIOAggData = true;
				}

				void prepareAndWriteTraceData(treetimer::config::Config& config,
											treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
											tt_sql::TTSQLite3* dataAccess)
				{
					// ToDo: Error Check - ensure database has been setup

					if (callTree.root == nullptr) {
						// Nothing to do
						return;
					}

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeTraceInstrumentationData, 
										config.sqlIOProcessID, -1, config);

					if (dataAccess->gatherIntraNode) {
						int err;

						// Before gather/writing any performance data, must first 
						// gather callpath data and adjust ID's for different ranks 
						// potentially having different callpath trees.
						std::map<int, int> callpathNodeIdRemap[dataAccess->nRanksLocal];
						MPI_Datatype callpathNodeRecord_MPI = tt_sql::drivers::createCallpathNodeMpiType();
						// Perform intra-node gather-at-root:
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;

								int nReceives = 0;
								int callpathNodeID;
								tt_sql::TT_CallPathNode* records = NULL;
								int nRecords = 0;
								int srcRank;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_CallPathNode), callpathNodeRecord_MPI, TAG_CALLPATH);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									nReceives++;
									for (int i=0; i<nRecords; i++) {
										tt_sql::TT_CallPathNode r = records[i];

										// Apply remap:
										if (r.parentID != -1 && callpathNodeIdRemap[srcRank].find(r.parentID) != callpathNodeIdRemap[srcRank].end()) {
											r.parentID = callpathNodeIdRemap[srcRank][r.parentID];
										}
										
										int blockTypeID;
										tt_sql::drivers::findProfileNodeTypeID(*dataAccess, codeBlockNames[r.blockType], &blockTypeID);
										int profileNodeID;
										tt_sql::drivers::writeTreeNodeProfileNodeData(*dataAccess, r.nodeName, blockTypeID, &profileNodeID);
										int callPathID;
										tt_sql::drivers::writeCallPathData(*dataAccess, r.rank, profileNodeID, r.parentID, &callPathID);

										// Store the remap:
										callpathNodeIdRemap[srcRank][r.callPathID] = callPathID;
									}
									free(records); records=NULL; nRecords=0;
								}
							}
							else {
								// Send to local root
								err = sendRecordsToRoot(*dataAccess, dataAccess->callPathNodeRecords.data(), dataAccess->callPathNodeRecords.size(), 
														sizeof(tt_sql::TT_CallPathNode), callpathNodeRecord_MPI, TAG_CALLPATH);
								if (err != 0) {
									fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								dataAccess->callPathNodeRecords.clear();
							}
						}

						// Create MPI type for a TraceTime record:
						MPI_Datatype traceTimeRecord_MPI = tt_sql::drivers::createTraceTimeMpiType();

						// Perform intra-node gather-at-root:
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;

								int nReceives = 0;
								int traceTimeID;
								tt_sql::TT_TraceTiming* records = NULL;
								int nRecords = 0;
								int srcRank;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_TraceTiming), traceTimeRecord_MPI, TAG_TRC);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									nReceives++;
									for (int i=0; i<nRecords; i++) {
										// Add in processID, which only root will know from earlier:
										records[i].processID = dataAccess->rankLocalToProcessID[srcRank];

										// Todo: remap callpath IDs:
										records[i].callPathID = callpathNodeIdRemap[srcRank][records[i].callPathID];
										
										tt_sql::drivers::writeTraceTimeData(*dataAccess, records[i], &traceTimeID);
									}
									free(records); records=NULL; nRecords=0;
								}
							}
							else {
								// Send to local root
								err = sendRecordsToRoot(*dataAccess, dataAccess->traceTimeRecords.data(), dataAccess->traceTimeRecords.size(), 
														sizeof(tt_sql::TT_TraceTiming), traceTimeRecord_MPI, TAG_TRC);
								if (err != 0) {
									fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								dataAccess->traceTimeRecords.clear();
							}
						}
					}

					// We may wish to write trace data multiple times, this is just an indicator that we've done it at least once
					config.sqlIOTraceData = true;
				}

				int fetchNextGatheredRecord(tt_sql::TTSQLite3& dataAccess, void** records, int* nRecords, int* srcRank, int elemBytes, MPI_Datatype elemType, int mpiTag)
				{
					int n = dataAccess.nRanksLocal;

					if (dataAccess.nRanksLocal > 1) {
						if (dataAccess.rankLocal == 0) {
							int nReceives = 0;
							bool workDone = false;
							int ready = 0;
							int err;
							MPI_Status stat;
							MPI_Request req;
							int aggTimeID;
							while (!workDone) {
								workDone = false;
								
								for (int r=1; r<n; r++) {
									// printf("Root is probing %d ...\n", r); fflush(stdout);
									err = MPI_Iprobe(r, mpiTag, dataAccess.nodeComm, &ready, &stat);
									if (err != MPI_SUCCESS) {
										fprintf(stderr, "Root rank failed to probe msg from %d\n", r);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									// printf("Root probe complete\n"); fflush(stdout);

									if (ready == 1) {
										workDone = true;
										nReceives++;
										*srcRank = r;

										// printf("Root is getting count ...\n"); fflush(stdout);
										err = MPI_Get_count(&stat, elemType, nRecords);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Root failed on MPI_Get_count(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										// printf("Root is receiving %d records from %d\n", *nRecords, r); fflush(stdout);

										if (*nRecords == 0) {
											*records = malloc(1 * elemBytes);
										} else {
											*records = malloc(*nRecords * elemBytes);
										}
										err = MPI_Recv(*records, *nRecords, elemType, r, mpiTag, dataAccess.nodeComm, &stat);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Root failed on MPI_Recv(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										return 0;
									}
								}

								if (!workDone) {
									// printf("Root got nothing to do, sleeping\n"); fflush(stdout);
									sleep(1);
								}
							}
						}
					}

					return 0;
				}

				int sendRecordsToRoot(tt_sql::TTSQLite3& dataAccess, const void* data, int nElems, int elemBytes, MPI_Datatype elemType, int mpiTag)
				{
					int err;
					if (dataAccess.nRanksLocal > 1) {
						if (dataAccess.rankLocal > 0) {
							// Send to local root
							MPI_Request req;
							MPI_Status stat;
							err = MPI_Isend(data, nElems, elemType, 0, mpiTag, dataAccess.nodeComm, &req);
							if (err != MPI_SUCCESS) {
								fprintf(stderr, "Rank %d failed Isend\n", dataAccess.rankGlobal);
								MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
							}
							// printf("Rank %d sent %d records to root (elemBytes = %d)\n", dataAccess.rankGlobal, nElems, elemBytes); fflush(stdout);
							MPI_Wait(&req, &stat);
						}
					}

					return 0;
				}

				void writeTreeNodeAggInstrumentationData(tt_sql::TTSQLite3& dataAccess,
														treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
														int processID, int parentID, int * callPathID, treetimer::config::Config& config)
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

					int blockTypeID = -1;
					int profileNodeID = -1;

					if (!dataAccess.gatherIntraNode || dataAccess.rankLocal==0) {
						// Retrieve this node's block type ID
						tt_sql::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.nodeData.blockType], &blockTypeID);

						// (a) Profile Nodes
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

							tt_sql::drivers::writeCallPathData(dataAccess, dataAccess.rankGlobal, profileNodeID, parentID, callPathID);
						}
						else
						{
							tt_sql::drivers::writeCallPathData(dataAccess, dataAccess.rankGlobal, profileNodeID, -1, callPathID);
						}
					}
					else {
						// Write to local cache, to be sent to local root rank for writing:
						tt_sql::TT_CallPathNode r;
						r.rank = dataAccess.rankGlobal;
						strncpy(r.nodeName, node.key.c_str(), MAX_STRING_LENGTH);
						r.blockType = node.nodeData.blockType;
						if(node.parent != nullptr) {
							r.parentID = parentID;
						} else {
							r.parentID = -1;
						}

						*callPathID = dataAccess.callPathNodeRecords.size()+1;
						r.callPathID = *callPathID;
						dataAccess.callPathNodeRecords.push_back(r);
					}

					// (c) Aggregate Time Data
					// Only write data if aggregate timers are enabled

					if(config.eATimers)
					{
						int aggTimeID;
						tt_sql::TT_AggTiming d;
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
							tt_sql::TT_AggParamInt p;
							p.rank = dataAccess.rankGlobal;
							p.callPathID = *callPathID;
							p.processID = processID;
							strncpy(p.paramName, it_int->first.c_str(), MAX_STRING_LENGTH);
							p.minValue = it_int->second->aggParam.minVal;
							p.maxValue = it_int->second->aggParam.maxVal;
							p.avgValue = it_int->second->aggParam.avgVal;
							p.stdev    = sqrt(it_int->second->aggParam.variance);
							p.count    = it_int->second->aggParam.count;
							tt_sql::drivers::writeAggregateParameterIntData(dataAccess, p, &aggParamID);
						}

						std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::iterator it_float;

						// Float Parameters;
						for(it_float = node.nodeData.doubleParameters.begin(); it_float != node.nodeData.doubleParameters.end(); it_float++)
						{
							int aggParamID;
							tt_sql::TT_AggParamFloat p;
							p.rank = dataAccess.rankGlobal;
							p.callPathID = *callPathID;
							p.processID = processID;
							strncpy(p.paramName, it_float->first.c_str(), MAX_STRING_LENGTH);
							p.minValue = it_float->second->aggParam.minVal;
							p.maxValue = it_float->second->aggParam.maxVal;
							p.avgValue = it_float->second->aggParam.avgVal;
							p.stdev    = sqrt(it_float->second->aggParam.variance);
							p.count    = it_float->second->aggParam.count;
							tt_sql::drivers::writeAggregateParameterFloatData(dataAccess, p, &aggParamID);
						}

						std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::iterator it_bool;

						// Bool Parameters;
						for(it_bool = node.nodeData.boolParameters.begin(); it_bool != node.nodeData.boolParameters.end(); it_bool++)
						{
							int aggParamID;
							tt_sql::TT_AggParamBool p;
							p.rank = dataAccess.rankGlobal;
							p.callPathID = *callPathID;
							p.processID = processID;
							strncpy(p.paramName, it_bool->first.c_str(), MAX_STRING_LENGTH);
							p.minValue = it_bool->second->aggParam.minVal;
							p.maxValue = it_bool->second->aggParam.maxVal;
							p.avgValue = it_bool->second->aggParam.avgVal;
							p.stdev    = sqrt(it_bool->second->aggParam.variance);
							p.count    = it_bool->second->aggParam.count;
							tt_sql::drivers::writeAggregateParameterBoolData(dataAccess, p, &aggParamID);
						}
					}


				}

				void writeTreeNodeTraceInstrumentationData(tt_sql::TTSQLite3& dataAccess,
													  treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													  int processID, int parentID, int * callPathID, treetimer::config::Config& config)
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

					int blockTypeID = -1;
					int profileNodeID = -1;

					if (!dataAccess.gatherIntraNode || dataAccess.rankLocal==0) {
						// Retrieve this node's block type ID
						tt_sql::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.nodeData.blockType], &blockTypeID);

						// (a) Profile Nodes
						tt_sql::drivers::writeTreeNodeProfileNodeData(dataAccess, node.key, blockTypeID, &profileNodeID);

						// (b) Call Path Data
						// CallPath Data should have been written for the parent as part of the tree traversal, and identified by parentID.

						// ToDo: Is there a better way of handling no parent (i.e. root node?) - NULL would be equivalent.
						// Could create an 'unknown' profile node to point to, that has a profile node entry but no callpath node entry
						if(node.parent != nullptr)
						{
							tt_sql::drivers::writeCallPathData(dataAccess, dataAccess.rankGlobal, profileNodeID, parentID, callPathID);
						}
						else
						{
							tt_sql::drivers::writeCallPathData(dataAccess, dataAccess.rankGlobal, profileNodeID, -1, callPathID);
						}
					}
					else {
						// Write to local cache, to be sent to local root rank for writing:
						tt_sql::TT_CallPathNode r;
						strncpy(r.nodeName, node.key.c_str(), MAX_STRING_LENGTH);
						r.rank = dataAccess.rankGlobal;
						r.blockType = node.nodeData.blockType;
						if(node.parent != nullptr) {
							r.parentID = parentID;
						} else {
							r.parentID = -1;
						}

						*callPathID = dataAccess.callPathNodeRecords.size()+1;
						r.callPathID = *callPathID;
						dataAccess.callPathNodeRecords.push_back(r);
					}

					// (c) Trace Times

					if(config.eTTimers)
					{
						// Loop over trace entries for the timer
						treetimer::data_structures::LinkedListNode<treetimer::timers::TraceTimer> * ptr = node.nodeData.blockTimer->traceTimers.head;

						while(ptr != nullptr)
						{
							int traceTimeID;
							tt_sql::TT_TraceTiming d;
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
										processID, *callPathID,
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
										processID, *callPathID,
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
										processID, *callPathID,
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
										processID, *callPathID,
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
													int processID,
													int parentID, int * callPathID,
													treetimer::config::Config& config),
													int processID, int parentID, treetimer::config::Config& config)
				{
					// Run function on current node
					// The retrived callPathID will be reused as the parentID for the child nodes.
					int callPathID;
					(*func)(dataAccess, node, 
						processID, parentID, &callPathID, config);

					std::unordered_map<std::string, treetimer::data_structures::TreeNode<std::string,treetimer::measurement::InstrumentationData> *>::iterator it;

					// Loop over child nodes
					for(it = node.childNodes.begin(); it != node.childNodes.end(); it ++)
					{
						callTreeTraversal(dataAccess, *(it->second), func, 
							processID, callPathID, config);
					}
				}
			}
		}
	}
}
