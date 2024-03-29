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

#include <chrono>
#include <ctime>

namespace tt_sql = treetimer::database::tt_sqlite3;

// Note! When performing intra-node gather of database records,
// MPI tags necessary to distinguish between different DB tables.
#define TAG_CALLPATH 0
#define TAG_HWINFO 1
#define TAG_AGG 11
#define TAG_AGG_PARAM_INT 12
#define TAG_AGG_PARAM_FLOAT 13
#define TAG_AGG_PARAM_BOOL 14
#define TAG_TRACE 21
#define TAG_TRACE_PARAM_INT 22
#define TAG_TRACE_PARAM_FLOAT 23
#define TAG_TRACE_PARAM_BOOL 24
#define TAG_TRACE_PARAM_STRING 25

// Because TreeTimer intercepts MPI calls, then profiling TreeTimer with another profiler is tricky.
// Enable 'DBG_GATHER_PERFORMANCE' to enable reporting of gather-at-root performance. Best done with
// just one node.
// #define DBG_GATHER_PERFORMANCE 1

namespace treetimer
{
	namespace io
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				// strlcpy is a safer version of strncpy, but non-standard so not guaranteed to be present. 
				// To avoid name conflicts, will implement as tt_strlcpy
				void tt_strlcpy(char *dst, const char *src, size_t size)
				{
					strncpy(dst, src, size);
					// Ensure string is NUL-terminated:
					size_t n = strlen(src);
					if (n >= size) dst[n-1] = '\0';
				}

				std::string getTimestamp()
				{
					std::chrono::time_point<std::chrono::system_clock> clk = std::chrono::system_clock::now();
					std::time_t clk_time_t = std::chrono::system_clock::to_time_t(clk);
					std::string s = std::ctime(&clk_time_t);
					// Remove newline:
					s.resize(s.size()-1);
					return s;
				}

				tt_sql::TTSQLite3* setupOutput(treetimer::config::Config& config)
				{
					tt_sql::TTSQLite3 *dataAccess = new tt_sql::TTSQLite3(config.outputFolder + "/" + config.sqlIOFilename);

					if(!config.sqlIOSetup) {
						// Setup the Database Schemas

						if (!dataAccess->gatherIntraNode || dataAccess->rankLocal==0) {
							// Create SQL Access Object
							tt_sql::drivers::openConnection(*dataAccess);

							// (a) Block Types
							tt_sql::drivers::writeSchemaProfileNodeTypeData(*dataAccess);

							// (b) Library Configuration
							tt_sql::drivers::writeSchemaLibraryConfigData(*dataAccess);

							// (c) Singular Parameters (used for Global Parameter Storage)
							tt_sql::drivers::writeSchemaParameterData(*dataAccess);

							// Disabled table: script can collect data.
							// // (d) Application Data
							// tt_sql::drivers::writeSchemaApplicationData(*dataAccess);

							// Disabled table: script can collect data.
							// // (e) Application Configuration Data (Links Combinations of Global Parameters)
							// tt_sql::drivers::writeSchemaApplicationConfigData(*dataAccess);

							// (n) Run Data (Configurations, Process Count etc)
							tt_sql::drivers::writeSchemaProfileRunConfigData(*dataAccess);

							// (f) Profile Nodes
							tt_sql::drivers::writeSchemaProfileNodeData(*dataAccess);

							// Disabled table: script can collect data.
							// // (h) Machine Data
							// tt_sql::drivers::writeSchemaMachineData(*dataAccess);

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
					}
					return dataAccess;
				}

				void writeRunConfigData(treetimer::config::Config& config, tt_sql::TTSQLite3 *dataAccess)
				{
					// Write data pertaining to the run configuration

					// Note: This can only be done once. Any global parameters set after this step will
					// not be stored. It must be done prior to writing any instrumentation data,
					// so should be performed prior to a trace/full data output if not yet performed.

					// At completion, set a suitable runID for reuse during any data writes.

					if(!config.sqlIORunConfig) {

						if (!dataAccess->gatherIntraNode || dataAccess->rankLocal==0) {
							// Profile Node Type Data - identical across ranks so no comms needed
							for(int i = 0; i < TT_CODE_BLOCK_COUNT; i++) {
								tt_sql::drivers::writeProfileNodeTypeData(*dataAccess, codeBlockNames[i], nullptr);
							}

							// Library Config Data - assume identical across ranks, why would user 
							// configure different ranks differently? So assume no comms needed.
							int libConfigID = -1;
							tt_sql::drivers::writeLibraryConfigID(*dataAccess, config.libVerMajor, config.libVerMinor,
																	config.eATimers, config.eTTimers,
																	config.eAParam, config.eTParam,
																	config.eAPAPI, config.eTPAPI,
																	config.eMPIHooks, &libConfigID);

							// Run Config Data (Runs are always recorded as new)
							int processCount, runID;
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

							// Need for table unclear, a script can collect this data.
							// // Application Data
							// int appID;
							// tt_sql::drivers::writeApplicationData(*dataAccess, config.appName, config.appVersion, &appID);

							// Machine Data - machineName is global and constant.
							// int machineID;
							// tt_sql::drivers::writeMachineData(*dataAccess, config.machineName, &machineID);
						}

						// Now handle process execution data:
						int processID = -1;
						if (dataAccess->gatherIntraNode) {
							if (dataAccess->rankLocal==0) {
								writeProcessHwData(config, dataAccess, getProcessData(dataAccess), &processID);
								config.sqlIOProcessID = processID;
							}
							gatherWriteProcessData(config, dataAccess);
						} else {
							// All ranks write
							writeProcessHwData(config, dataAccess, getProcessData(dataAccess), &processID);
							config.sqlIOProcessID = processID;
						}

						// Update: Why do global parameters exist? They're set but never used.
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

						// Need for table unclear, a script can collect this data.
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

				tt_sql::TT_HwInfo getProcessData(tt_sql::TTSQLite3 *dataAccess)
				{
					std::string cpuModel = "Unknown";
					char nodeName[MAX_STRING_LENGTH]; gethostname(nodeName, MAX_STRING_LENGTH);
					int socketCount = 1;
					int phySocketNum = 0;
					int physicalCoreNum = sched_getcpu();
					int thread = 0;

					tt_sql::TT_HwInfo hw;
					hw.rankGlobal = dataAccess->rankGlobal;
					tt_strlcpy(hw.cpuModel, cpuModel.c_str(), MAX_STRING_LENGTH);
					tt_strlcpy(hw.nodeName, nodeName, MAX_STRING_LENGTH);
					hw.socketCount = socketCount;
					hw.phySocketNum = phySocketNum;
					hw.physicalCoreNum = physicalCoreNum;
					hw.thread = thread;
					return hw;
				}

				void writeProcessHwData(treetimer::config::Config& config, tt_sql::TTSQLite3 *dataAccess, tt_sql::TT_HwInfo hw, int *processID)
				{
					int pID = -1;

					// CPU Data
					int cpuID;
					tt_sql::TT_Cpu cpu;
					tt_strlcpy(cpu.cpuModel, hw.cpuModel, MAX_STRING_LENGTH);
					tt_sql::drivers::writeCPUData(*dataAccess, cpu, &cpuID);

					// Compute Node Data
					int computeNodeID;
					tt_sql::TT_ComputeNode cnode;
					tt_strlcpy(cnode.nodeName, hw.nodeName, MAX_STRING_LENGTH);
					cnode.socketCount = hw.socketCount;
					tt_sql::drivers::writeComputeNodeData(*dataAccess, 
															// machineID, 
															cnode,
															&computeNodeID);
					// CPU Socket Data
					int cpuSocketID;
					tt_sql::TT_Socket skt;
					skt.phySocketNum = hw.phySocketNum;
					tt_sql::drivers::writeCPUSocketData(*dataAccess, computeNodeID, cpuID, skt, &cpuSocketID);

					// CPU Core Data
					int cpuCoreID;
					tt_sql::TT_CpuCore core;
					core.physicalCoreNum = hw.physicalCoreNum;
					tt_sql::drivers::writeCPUCoreData(*dataAccess, cpuSocketID, core, &cpuCoreID);

					// ProcessID
					tt_sql::drivers::writeProcessData(*dataAccess, cpuCoreID, hw.rankGlobal, hw.thread, &pID);

					if (processID != nullptr) {
						*processID = pID;
					}
				}

				void gatherWriteProcessData(treetimer::config::Config& config, tt_sql::TTSQLite3 *dataAccess)
				{
					if (!dataAccess->gatherIntraNode) return;

					// Perform intra-node gather-at-root:

					tt_sql::TT_HwInfo d = getProcessData(dataAccess);

					MPI_Datatype hwInfoRecord_MPI = tt_sql::drivers::createHwInfoMpiType();
					int err;
					if (dataAccess->nRanksLocal > 1) {
						if (dataAccess->rankLocal == 0) {
							int n = dataAccess->nRanksLocal;
							tt_sql::TT_HwInfo *records = nullptr; int nRecords = 0;
							int nReceives = 0; int srcRank;
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

									int processID = -1;
									writeProcessHwData(config, dataAccess, r, &processID);

									dataAccess->rankLocalToProcessID[srcRank] = processID;
								}
								free(records); records=nullptr; nRecords=0;
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

				void gatherAndWriteCallPathData(treetimer::config::Config& config, 
												tt_sql::TTSQLite3 *dataAccess)
				{
					if (!dataAccess->gatherIntraNode) return;
					if (dataAccess->callpathNodeIdRemap_created) return;

					// Gather and write callpath data, and adjust ID's for different ranks 
					// potentially having different callpath trees.

					int err;

					MPI_Datatype callpathNodeRecord_MPI = tt_sql::drivers::createCallpathNodeMpiType();
					if (dataAccess->nRanksLocal > 1) {
						if (dataAccess->rankLocal == 0) {
							int n = dataAccess->nRanksLocal;
							tt_sql::TT_CallPathNode *records = nullptr; int nRecords = 0;
							int nReceives = 0; int srcRank = -1;
							while (nReceives != (n-1)) {
								srcRank = -1;
								err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																sizeof(tt_sql::TT_CallPathNode), callpathNodeRecord_MPI, TAG_CALLPATH);
								if (err != 0) {
									fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								if (srcRank == -1) {
									fprintf(stderr, "Root failed on fetchNextGatheredRecord() - srcRank = %d\n", srcRank);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								if (dataAccess->callpathNodeIdRemap[srcRank].size() > 0) {
									fprintf(stderr, "Root has already received data from srcRank %d, why receiving %d more?\n", srcRank, nRecords);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								nReceives++;
								#ifdef DBG_GATHER_PERFORMANCE
									double t1 = MPI_Wtime();
								#endif
								for (int i=0; i<nRecords; i++) {
									tt_sql::TT_CallPathNode r = records[i];

									// Apply remap:
									if (r.parentID != -1) {
										// parentID value should be in map
										if (dataAccess->callpathNodeIdRemap[srcRank].find(r.parentID) == dataAccess->callpathNodeIdRemap[srcRank].end()) {
											// Oh dear ...
											fprintf(stderr, "TreeTimer error: encountered a CallPath node before encountering its parent! rank=%d, nodeID=%d, parentID=%d\n", srcRank, r.callPathID, r.parentID);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										r.parentID = dataAccess->callpathNodeIdRemap[srcRank][r.parentID];
									}
									
									int blockTypeID;
									tt_sql::drivers::findProfileNodeTypeID(*dataAccess, codeBlockNames[r.blockType], &blockTypeID);
									int profileNodeID;
									tt_sql::drivers::writeTreeNodeProfileNodeData(*dataAccess, r.nodeName, blockTypeID, &profileNodeID);
									int callPathID;
									// Get processID, which only root will know from earlier:
									int processID = dataAccess->rankLocalToProcessID[srcRank];

									tt_sql::TT_CallPathNode c;
									c.processID = processID;
									c.profileNodeID = profileNodeID;
									c.parentID = r.parentID;
									tt_sql::drivers::writeCallPathData(*dataAccess, c, &callPathID, true);

									if (callPathID == 0) {
										fprintf(stderr, "TreeTimer error: writeCallPathData() has returned callPathID=%d\n", callPathID);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}

									// Store the remap:
									dataAccess->callpathNodeIdRemap[srcRank][r.callPathID] = callPathID;
								}
								#ifdef DBG_GATHER_PERFORMANCE
									double t2 = MPI_Wtime();
									if (nRecords > 0) {
										std::cout << getTimestamp() << " | " << "Root wrote " << "to CallPath DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
									}
								#endif
								free(records); records=nullptr; nRecords=0;
							}
							if (nReceives != (n-1)) {
								fprintf(stderr, "Rank %d expected to gather CallPath records from %d ranks but received from %d\n", dataAccess->rankLocal, n-1, nReceives);
								MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
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

					dataAccess->callpathNodeIdRemap_created = true;
				}

				void prepareAndWriteAggData(treetimer::config::Config& config,
											treetimer::data_structures::Tree<std::string, 
											treetimer::measurement::InstrumentationData>& callTree,
											tt_sql::TTSQLite3 *dataAccess)
				{
					// ToDo: Error Check - ensure database has been setup

					if (config.sqlIOAggData) return;

					if (callTree.root == nullptr) {
						// Nothing to do
						return;
					}

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeAggInstrumentationData, 
										config.sqlIOProcessID, -1, config);

					gatherAndWriteCallPathData(config, dataAccess);

					if (dataAccess->gatherIntraNode) {
						int err;

						// Gather AggregateTime data (intra-node gather-at-root):
						MPI_Datatype aggTimeRecord_MPI = tt_sql::drivers::createAggregateTimeMpiType();
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;
								tt_sql::TT_AggTiming *records = nullptr; int nRecords = 0;
								int nReceives = 0; int srcRank;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_AggTiming), aggTimeRecord_MPI, TAG_AGG);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									nReceives++;
									#ifdef DBG_GATHER_PERFORMANCE
										double t1 = MPI_Wtime();
									#endif
									for (int i=0; i<nRecords; i++) {
										// Add in processID, which only root will know from earlier:
										records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
										// Remap callpath IDs:
										records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
										tt_sql::drivers::writeAggregateTimeData(*dataAccess, records[i], nullptr, false);
									}
									#ifdef DBG_GATHER_PERFORMANCE
										double t2 = MPI_Wtime();
										if (nRecords > 0) {
											std::cout << getTimestamp() << " | " << "Root wrote " << "to AggregateTime DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
										}
									#endif
									free(records); records=nullptr; nRecords=0;
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
							// Gather AggregateParameter data (intra-node gather-at-root):

							// AggregateParamterInt:
							MPI_Datatype aggParamIntRecord_MPI = tt_sql::drivers::createAggregateParamIntMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									tt_sql::TT_AggParamInt *records = nullptr; int nRecords = 0;
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
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeAggregateParameterIntData(*dataAccess, records[i], nullptr, false);
										}
										free(records); records=nullptr; nRecords=0;
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
									tt_sql::TT_AggParamFloat *records = nullptr; int nRecords = 0;
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
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeAggregateParameterFloatData(*dataAccess, records[i], nullptr, false);
										}
										free(records); records=nullptr; nRecords=0;
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
									tt_sql::TT_AggParamBool *records = nullptr; int nRecords = 0;
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
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeAggregateParameterBoolData(*dataAccess, records[i], nullptr, false);
										}
										free(records); records=nullptr; nRecords=0;
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
											tt_sql::TTSQLite3 *dataAccess)
				{
					// ToDo: Error Check - ensure database has been setup

					if (callTree.root == nullptr) {
						// Nothing to do
						return;
					}

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeTraceInstrumentationData, 
										config.sqlIOProcessID, -1, config);

					gatherAndWriteCallPathData(config, dataAccess);

					if (dataAccess->gatherIntraNode) {
						int err;

						// Gather TraceTime data (intra-node gather-at-root):
						MPI_Datatype traceTimeRecord_MPI = tt_sql::drivers::createTraceTimeMpiType();
						if (dataAccess->nRanksLocal > 1) {
							if (dataAccess->rankLocal == 0) {
								int n = dataAccess->nRanksLocal;
								tt_sql::TT_TraceTiming *records = nullptr; int nRecords = 0;
								int nReceives = 0; int srcRank;
								while (nReceives != (n-1)) {
									err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																	sizeof(tt_sql::TT_TraceTiming), traceTimeRecord_MPI, TAG_TRACE);
									if (err != 0) {
										fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									nReceives++;
									#ifdef DBG_GATHER_PERFORMANCE
										double t1 = MPI_Wtime();
									#endif
									for (int i=0; i<nRecords; i++) {
										// Add in processID, which only root will know from earlier:
										records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
										// Remap callpath IDs:
										records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
										tt_sql::drivers::writeTraceTimeData(*dataAccess, records[i], nullptr, false);
									}
									#ifdef DBG_GATHER_PERFORMANCE
										double t2 = MPI_Wtime();
										if (nRecords > 0) {
											std::cout << getTimestamp() << " | " << "Root wrote " << "to TraceTime DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
										}
									#endif
									free(records); records=nullptr; nRecords=0;
								}
							}
							else {
								// Send to local root
								err = sendRecordsToRoot(*dataAccess, dataAccess->traceTimeRecords.data(), dataAccess->traceTimeRecords.size(), 
														sizeof(tt_sql::TT_TraceTiming), traceTimeRecord_MPI, TAG_TRACE);
								if (err != 0) {
									fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
									MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
								}
								dataAccess->traceTimeRecords.clear();
							}
						}

						if(config.eTParam) {
							// Gather TraceParameter data (intra-node gather-at-root):

							// TraceParamterInt:
							MPI_Datatype traceParamIntRecord_MPI = tt_sql::drivers::createTraceParamIntMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									tt_sql::TT_TraceParamInt *records = nullptr; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_TraceParamInt), traceParamIntRecord_MPI, TAG_TRACE_PARAM_INT);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										#ifdef DBG_GATHER_PERFORMANCE
											double t1 = MPI_Wtime();
										#endif
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeTraceParameterIntData(*dataAccess, records[i], nullptr, false);
										}
										#ifdef DBG_GATHER_PERFORMANCE
											double t2 = MPI_Wtime();
											if (nRecords > 0) {
												std::cout << getTimestamp() << " | " << "Root wrote " << "to TraceParamInt DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
											}
										#endif
										free(records); records=nullptr; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->traceParamIntRecords.data(), dataAccess->traceParamIntRecords.size(), 
															sizeof(tt_sql::TT_TraceParamInt), traceParamIntRecord_MPI, TAG_TRACE_PARAM_INT);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->traceParamIntRecords.clear();
								}
							}
							
							// TraceParamterFloat:
							MPI_Datatype traceParamFloatRecord_MPI = tt_sql::drivers::createTraceParamFloatMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									tt_sql::TT_TraceParamFloat *records = nullptr; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_TraceParamFloat), traceParamFloatRecord_MPI, TAG_TRACE_PARAM_FLOAT);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										#ifdef DBG_GATHER_PERFORMANCE
											double t1 = MPI_Wtime();
										#endif
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeTraceParameterFloatData(*dataAccess, records[i], nullptr, false);
										}
										#ifdef DBG_GATHER_PERFORMANCE
											double t2 = MPI_Wtime();
											if (nRecords > 0) {
												std::cout << getTimestamp() << " | " << "Root wrote " << "to TraceParamFlt DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
											}
										#endif
										free(records); records=nullptr; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->traceParamFloatRecords.data(), dataAccess->traceParamFloatRecords.size(), 
															sizeof(tt_sql::TT_TraceParamFloat), traceParamFloatRecord_MPI, TAG_TRACE_PARAM_FLOAT);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->traceParamFloatRecords.clear();
								}
							}
							
							// TraceParamterBool:
							MPI_Datatype traceParamBoolRecord_MPI = tt_sql::drivers::createTraceParamBoolMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									tt_sql::TT_TraceParamBool *records = nullptr; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_TraceParamBool), traceParamBoolRecord_MPI, TAG_TRACE_PARAM_BOOL);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										#ifdef DBG_GATHER_PERFORMANCE
											double t1 = MPI_Wtime();
										#endif
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeTraceParameterBoolData(*dataAccess, records[i], nullptr, false);
										}
										#ifdef DBG_GATHER_PERFORMANCE
											double t2 = MPI_Wtime();
											if (nRecords > 0) {
												std::cout << getTimestamp() << " | " << "Root wrote " << "to TraceParamBool DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
											}
										#endif
										free(records); records=nullptr; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->traceParamBoolRecords.data(), dataAccess->traceParamBoolRecords.size(), 
															sizeof(tt_sql::TT_TraceParamBool), traceParamBoolRecord_MPI, TAG_TRACE_PARAM_BOOL);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->traceParamBoolRecords.clear();
								}
							}
							
							// TraceParamterString:
							MPI_Datatype traceParamStringRecord_MPI = tt_sql::drivers::createTraceParamStringMpiType();
							if (dataAccess->nRanksLocal > 1) {
								if (dataAccess->rankLocal == 0) {
									int n = dataAccess->nRanksLocal;
									tt_sql::TT_TraceParamString *records = nullptr; int nRecords = 0;
									int nReceives = 0; int srcRank;
									while (nReceives != (n-1)) {
										err = fetchNextGatheredRecord(*dataAccess, (void**)&records, &nRecords, &srcRank, 
																		sizeof(tt_sql::TT_TraceParamString), traceParamStringRecord_MPI, TAG_TRACE_PARAM_STRING);
										if (err != 0) {
											fprintf(stderr, "Root failed on fetchNextGatheredRecord()\n");
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										nReceives++;
										#ifdef DBG_GATHER_PERFORMANCE
											double t1 = MPI_Wtime();
										#endif
										for (int i=0; i<nRecords; i++) {
											// Add in processID, which only root will know from earlier:
											records[i].processID = dataAccess->rankLocalToProcessID[srcRank];
											// Remap callpath IDs:
											records[i].callPathID = dataAccess->callpathNodeIdRemap[srcRank][records[i].callPathID];
											tt_sql::drivers::writeTraceParameterStringData(*dataAccess, records[i], nullptr, false);
										}
										#ifdef DBG_GATHER_PERFORMANCE
											double t2 = MPI_Wtime();
											if (nRecords > 0) {
												std::cout << getTimestamp() << " | " << "Root wrote " << "to TraceParamStr DB at " << int(double(nRecords)/(t2-t1)/1e3) << "K records/sec" << std::endl;
											}
										#endif
										free(records); records=nullptr; nRecords=0;
									}
								}
								else {
									// Send to local root
									err = sendRecordsToRoot(*dataAccess, dataAccess->traceParamStringRecords.data(), dataAccess->traceParamStringRecords.size(), 
															sizeof(tt_sql::TT_TraceParamString), traceParamStringRecord_MPI, TAG_TRACE_PARAM_STRING);
									if (err != 0) {
										fprintf(stderr, "Rank %d failed on sendRecordsToRoot()\n", dataAccess->rankGlobal);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}
									dataAccess->traceParamStringRecords.clear();
								}
							}

						}
					}

					// We may wish to write trace data multiple times, this is just an indicator that we've done it at least once
					config.sqlIOTraceData = true;
				}

				int fetchNextGatheredRecord(tt_sql::TTSQLite3& dataAccess, void* *records, int *nRecords, int *srcRank, int elemBytes, MPI_Datatype elemType, int mpiTag)
				{
					if (!dataAccess.gatherIntraNode) return 0;

					int n = dataAccess.nRanksLocal;

					if (dataAccess.nRanksLocal > 1) {
						if (dataAccess.rankLocal == 0) {
							int nReceives = 0;
							bool workDone = false;
							int ready = 0;
							int err;
							MPI_Status stat;
							while (!workDone) {
								workDone = false;
								
								for (int r=1; r<n; r++) {
									err = MPI_Iprobe(r, mpiTag, dataAccess.nodeComm, &ready, &stat);
									if (err != MPI_SUCCESS) {
										fprintf(stderr, "Root rank failed to probe msg from %d\n", r);
										MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
									}

									if (ready == 1) {
										workDone = true;
										nReceives++;
										*srcRank = r;

										err = MPI_Get_count(&stat, elemType, nRecords);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Root failed on MPI_Get_count(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}

										// printf("Rank %d received %d records from rank %d\n", dataAccess.rankGlobal, *nRecords, r); fflush(stdout);

										if (*nRecords == 0) {
											*records = malloc(1 * elemBytes);
										} else {
											*records = malloc(*nRecords * elemBytes);
										}

										#ifdef DBG_GATHER_PERFORMANCE
											double t1 = MPI_Wtime();
										#endif
										err = MPI_Recv(*records, *nRecords, elemType, r, mpiTag, dataAccess.nodeComm, &stat);
										if (err != MPI_SUCCESS) {
											fprintf(stderr, "Root failed on MPI_Recv(rank=%d)\n", r);
											MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
										}
										#ifdef DBG_GATHER_PERFORMANCE
											double mb = (double(*nRecords)*elemBytes/1e6);
											double t2 = MPI_Wtime();
											if ((*nRecords) > 0) {
												std::cout << getTimestamp() << " | " << "Root received at " << int(mb/(t2-t1)) << " MB/sec" << std::endl;
											}
										#endif

										return 0;
									}
								}

								if (!workDone) {
									sleep(1);
								}
							}
						}
					}

					return 0;
				}

				int sendRecordsToRoot(tt_sql::TTSQLite3& dataAccess, const void *data, int nElems, int elemBytes, MPI_Datatype elemType, int mpiTag)
				{
					if (!dataAccess.gatherIntraNode) {
						return 0;
					}

					if (nElems < 0) {
						fprintf(stderr, "Rank %d: Isend error: negative nElems=%d\n", dataAccess.rankGlobal, nElems);
						MPI_Abort(MPI_COMM_WORLD, 1); exit(EXIT_FAILURE);
					}

					if (dataAccess.nRanksLocal > 1) {
						if (dataAccess.rankLocal > 0) {
							// Send to local root

							// printf("Rank %d sending %d records (tag=%d) to root (elemBytes = %d, total MB = %d)\n", 
							// 		dataAccess.rankGlobal, nElems, mpiTag, elemBytes, (nElems*elemBytes)/1000000); fflush(stdout);

							// Write speed to root is invariant to blocking vs nonblocking send.

							// MPI_Request req;
							// MPI_Status stat;
							// int err = MPI_Isend(data, nElems, elemType, 0, mpiTag, dataAccess.nodeComm, &req);
							// if (err != MPI_SUCCESS) {
							// 	fprintf(stderr, "Rank %d failed Isend (nElems=%d, tag=%d)\n", dataAccess.rankGlobal, nElems, mpiTag);
							// 	MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
							// }
							// // printf("Rank %d sent %d records (tag=%d) to root (elemBytes = %d, total MB = %d)\n", dataAccess.rankGlobal, nElems, mpiTag, elemBytes, nElems*elemBytes/1e6); fflush(stdout);
							// MPI_Wait(&req, &stat);

							int err = MPI_Send(data, nElems, elemType, 0, mpiTag, dataAccess.nodeComm);
							if (err != MPI_SUCCESS) {
								fprintf(stderr, "Rank %d failed Send (nElems=%d, tag=%d)\n", dataAccess.rankGlobal, nElems, mpiTag);
								MPI_Abort(MPI_COMM_WORLD, err); exit(EXIT_FAILURE);
							}
						}
					}

					return 0;
				}

				void writeTreeNodeAggInstrumentationData(tt_sql::TTSQLite3& dataAccess,
														treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
														int processID, int parentID, int *callPathID, treetimer::config::Config& config)
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

						tt_sql::TT_CallPathNode c;
						c.processID = processID;
						c.profileNodeID = profileNodeID;
						// ToDo: Is there a better way of handling no parent (i.e. root node?) - NULL would be equivalent.
						// Could create an 'unknown' profile node to point to, that has a profile node entry but no callpath node entry
						if(node.parent != nullptr) {
							c.parentID = parentID;
						} else {
							c.parentID = -1;
						}
						tt_sql::drivers::writeCallPathData(dataAccess, c, callPathID, true);
					}
					else {
						// Write to local cache, to be sent to local root rank for writing:
						tt_sql::TT_CallPathNode r;
						r.rank = dataAccess.rankGlobal;
						tt_strlcpy(r.nodeName, node.key.c_str(), MAX_STRING_LENGTH);
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
						int w=1;
						treetimer::data_structures::LinkedListNode<treetimer::timers::AggTimings> *ptr = node.nodeData.blockTimer->aggTimings.head;
						while(ptr != nullptr) {
							d.window = w;
							d.minWallTime = ptr->data.minWalltime;
							d.avgWallTime = ptr->data.avgWalltime;
							d.maxWallTime = ptr->data.maxWalltime;
							d.stdev = sqrt(ptr->data.varianceWalltime);
							d.count = ptr->data.count;
							tt_sql::drivers::writeAggregateTimeData(dataAccess, d, &aggTimeID, false);

							w++;
							ptr = ptr->next;
						}
					}

					// (d) Aggregate Parameter Data

					if(config.eAParam)
					{
						// Loop over and write each aggregate parameter

						std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>::iterator it_int;

						// Int Parameters;
						for(it_int = node.nodeData.intParameters.begin(); it_int != node.nodeData.intParameters.end(); it_int++)
						{
							tt_sql::TT_AggParamInt p;
							p.rank = dataAccess.rankGlobal;
							p.callPathID = *callPathID;
							p.processID = processID;
							tt_strlcpy(p.paramName, it_int->first.c_str(), MAX_STRING_LENGTH);
							int w=1;
							treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<int>> *ptr = it_int->second->aggValues.head;
							while (ptr != nullptr) {
								p.window = w;
								p.minValue = ptr->data.minVal;
								p.maxValue = ptr->data.maxVal;
								p.avgValue = ptr->data.avgVal;
								p.stdev = sqrt(ptr->data.variance);
								p.count = ptr->data.count;
								tt_sql::drivers::writeAggregateParameterIntData(dataAccess, p, nullptr, false);

								w++;
								ptr = ptr->next;
							}
						}

						std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::iterator it_float;

						// Float Parameters;
						for(it_float = node.nodeData.doubleParameters.begin(); it_float != node.nodeData.doubleParameters.end(); it_float++)
						{
							tt_sql::TT_AggParamFloat p;
							p.rank = dataAccess.rankGlobal;
							p.callPathID = *callPathID;
							p.processID = processID;
							tt_strlcpy(p.paramName, it_float->first.c_str(), MAX_STRING_LENGTH);
							int w=1;
							treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<double>> *ptr = it_float->second->aggValues.head;
							while (ptr != nullptr) {
								p.window = w;
								p.minValue = ptr->data.minVal;
								p.maxValue = ptr->data.maxVal;
								p.avgValue = ptr->data.avgVal;
								p.stdev = sqrt(ptr->data.variance);
								p.count = ptr->data.count;
								tt_sql::drivers::writeAggregateParameterFloatData(dataAccess, p, nullptr, false);

								w++;
								ptr = ptr->next;
							}
						}

						std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::iterator it_bool;

						// Bool Parameters;
						for(it_bool = node.nodeData.boolParameters.begin(); it_bool != node.nodeData.boolParameters.end(); it_bool++)
						{
							tt_sql::TT_AggParamBool p;
							p.rank = dataAccess.rankGlobal;
							p.callPathID = *callPathID;
							p.processID = processID;
							tt_strlcpy(p.paramName, it_bool->first.c_str(), MAX_STRING_LENGTH);
							int w=1;
							treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<bool>> *ptr = it_bool->second->aggValues.head;
							while (ptr != nullptr) {
								p.window = w;
								p.minValue = ptr->data.minVal;
								p.maxValue = ptr->data.maxVal;
								p.avgValue = ptr->data.avgVal;
								p.stdev = sqrt(ptr->data.variance);
								p.count = ptr->data.count;
								tt_sql::drivers::writeAggregateParameterBoolData(dataAccess, p, nullptr, false);

								w++;
								ptr = ptr->next;
							}
						}
					}


				}

				void writeTreeNodeTraceInstrumentationData(tt_sql::TTSQLite3& dataAccess,
													  treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													  int processID, int parentID, int *callPathID, treetimer::config::Config& config)
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

						tt_sql::TT_CallPathNode c;
						c.processID = processID;
						c.profileNodeID = profileNodeID;
						// ToDo: Is there a better way of handling no parent (i.e. root node?) - NULL would be equivalent.
						// Could create an 'unknown' profile node to point to, that has a profile node entry but no callpath node entry
						if(node.parent != nullptr) {
							c.parentID = parentID;
						} else {
							c.parentID = -1;
						}
						tt_sql::drivers::writeCallPathData(dataAccess, c, callPathID, true);
					}
					else {
						// Write to local cache, to be sent to local root rank for writing:
						tt_sql::TT_CallPathNode r;
						tt_strlcpy(r.nodeName, node.key.c_str(), MAX_STRING_LENGTH);
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
						treetimer::data_structures::LinkedListNode<treetimer::timers::TraceTimer> *ptr = node.nodeData.blockTimer->traceTimers.head;

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
							tt_sql::drivers::writeTraceTimeData(dataAccess, d, &traceTimeID, false);

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
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<int>> *ptr = it_int->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::TT_TraceParamInt p;
								p.rank = dataAccess.rankGlobal;
								p.processID = processID;
								p.callPathID = *callPathID;
								tt_strlcpy(p.paramName, it_int->first.c_str(), MAX_STRING_LENGTH);
								p.paramValue = ptr->data.val;
								p.nodeEntryID = ptr->data.nodeEntryID;
								p.nodeExitID = ptr->data.nodeExitID;
								tt_sql::drivers::writeTraceParameterIntData(dataAccess, p, &traceParamID, false);

								ptr = ptr->next;
							}
						}


						// Floats/Doubles
						std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::iterator it_double;

						for(it_double = node.nodeData.doubleParameters.begin(); it_double != node.nodeData.doubleParameters.end(); it_double++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<double>> *ptr = it_double->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::TT_TraceParamFloat p;
								p.rank = dataAccess.rankGlobal;
								p.processID = processID;
								p.callPathID = *callPathID;
								tt_strlcpy(p.paramName, it_double->first.c_str(), MAX_STRING_LENGTH);
								p.paramValue = ptr->data.val;
								p.nodeEntryID = ptr->data.nodeEntryID;
								p.nodeExitID = ptr->data.nodeExitID;
								tt_sql::drivers::writeTraceParameterFloatData(dataAccess, p, &traceParamID, false);

								ptr = ptr->next;
							}
						}

						// Bool Parameters;
						std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::iterator it_bool;

						for(it_bool = node.nodeData.boolParameters.begin(); it_bool != node.nodeData.boolParameters.end(); it_bool++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<bool>> *ptr = it_bool->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::TT_TraceParamBool p;
								p.rank = dataAccess.rankGlobal;
								p.processID = processID;
								p.callPathID = *callPathID;
								tt_strlcpy(p.paramName, it_bool->first.c_str(), MAX_STRING_LENGTH);
								p.paramValue = ptr->data.val;
								p.nodeEntryID = ptr->data.nodeEntryID;
								p.nodeExitID = ptr->data.nodeExitID;
								tt_sql::drivers::writeTraceParameterBoolData(dataAccess, p, &traceParamID, false);

								ptr = ptr->next;
							}
						}

						// String Parameters
						std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>::iterator it_string;

						for(it_string = node.nodeData.stringParameters.begin(); it_string != node.nodeData.stringParameters.end(); it_string++)
						{
							// Loop over linked list
							treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<std::string>> *ptr = it_string->second->traceParams.head;

							while(ptr != nullptr)
							{
								int traceParamID;
								tt_sql::TT_TraceParamString p;
								p.rank = dataAccess.rankGlobal;
								p.processID = processID;
								p.callPathID = *callPathID;
								tt_strlcpy(p.paramName, it_string->first.c_str(), MAX_STRING_LENGTH);
								tt_strlcpy(p.paramValue, ptr->data.val.c_str(), MAX_STRING_LENGTH);
								p.nodeEntryID = ptr->data.nodeEntryID;
								p.nodeExitID = ptr->data.nodeExitID;
								tt_sql::drivers::writeTraceParameterStringData(dataAccess, p, &traceParamID, false);

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
													int parentID, int *callPathID,
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
