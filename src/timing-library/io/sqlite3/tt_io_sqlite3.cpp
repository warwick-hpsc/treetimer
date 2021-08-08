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
#include <iostream>
#include "mpi.h"

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

#include "tt_code_block_type.h"

#include <vector>

#include <cmath>

namespace tt_sql = treetimer::database::tt_sqlite3;

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

					// To avoid high-rank runs smashing filesystem, perform intra-node gather and write.
					int rankGlobal, nRanksGlobal, err;
					MPI_Comm_rank(MPI_COMM_WORLD, &rankGlobal);
					MPI_Comm_size(MPI_COMM_WORLD, &nRanksGlobal);
					// Todo: make 'gatherIntraNode' conditional on number of ranks
					bool gatherIntraNode = true;
					MPI_Comm nodeComm;
					int rankLocal, nRanksLocal;
					err = MPI_Comm_split(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rankGlobal, &nodeComm);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Rank %d failed to create intra-node MPI communicator\n", rankGlobal);
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					MPI_Comm_rank(nodeComm, &rankLocal);
					MPI_Comm_size(nodeComm, &nRanksLocal);
					if (nRanksLocal == 1) {
						gatherIntraNode = false;
					}
					dataAccess->rankGlobal = rankGlobal;
					dataAccess->rankLocal = rankLocal;
					dataAccess->nRanksLocal = nRanksLocal;
					dataAccess->gatherIntraNode = gatherIntraNode;

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeAggInstrumentationData, config.sqlIORunID, config.sqlIOProcessID, -1, config);


					// // Test that local collation of records works, by writing out here:
					// dataAccess->gatherIntraNode = false;
					// int aggTimeID;
					// for (int i=0; i<dataAccess->aggTimeRecords.size(); i++) {
					// 	tt_sql::drivers::writeAggregateTimeData_v2(*dataAccess, dataAccess->aggTimeRecords[i], &aggTimeID);
					// }
					// Above works. Now implement MPI
					tt_sql::aggTimeData atr;
					MPI_Datatype aggTimeRecord_MPI;
					int lengths[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
					MPI_Aint displacements[9];
					displacements[0] = offsetof(tt_sql::aggTimeData, runID);
					displacements[1] = offsetof(tt_sql::aggTimeData, rank);
					displacements[2] = offsetof(tt_sql::aggTimeData, callPathID);
					displacements[3] = offsetof(tt_sql::aggTimeData, processID);
					displacements[4] = offsetof(tt_sql::aggTimeData, minWallTime);
					displacements[5] = offsetof(tt_sql::aggTimeData, avgWallTime);
					displacements[6] = offsetof(tt_sql::aggTimeData, maxWallTime);
					displacements[7] = offsetof(tt_sql::aggTimeData, stdev);
					displacements[8] = offsetof(tt_sql::aggTimeData, count);
					if (dataAccess->rankLocal==0) {
						printf("Displacements: "); for (int i=0; i<9; i++) printf("%d, ", displacements[i]); printf("\n");
					}
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

					// Adjust type to account for struct padding
					MPI_Datatype aggTimeRecord_MPI_rs;
					err = MPI_Type_create_resized(aggTimeRecord_MPI, 0, sizeof(tt_sql::aggTimeData), &aggTimeRecord_MPI_rs);
					if (err != MPI_SUCCESS) {
						fprintf(stderr, "Rank %d failed to resize custom type for aggTimeRecord\n", dataAccess->rankGlobal);
						MPI_Abort(MPI_COMM_WORLD, err);
						exit(EXIT_FAILURE);
					}
					err = MPI_Type_commit(&aggTimeRecord_MPI_rs);

					// int bytes;
					// // MPI_Type_size(aggTimeRecord_MPI, &bytes);
					// MPI_Type_size(aggTimeRecord_MPI_rs, &bytes);
					// if (bytes != sizeof(tt_sql::aggTimeData)) {
					// 	if (dataAccess->rankGlobal == 0) {
					// 		fprintf(stderr, "aggTimeRecord_MPI is %d bytes, but aggTimeData is %d bytes\n", bytes, sizeof(tt_sql::aggTimeData));
					// 	}
					// 	MPI_Barrier(MPI_COMM_WORLD);
					// 	MPI_Abort(MPI_COMM_WORLD, 0);
					// 	exit(EXIT_FAILURE);
					// }
					// if (dataAccess->rankLocal==0) {
					// 	printf("Type size = %d bytes\n", bytes);
					// 	printf("Struct size = %d\n", sizeof(tt_sql::aggTimeData));
					// }

					if (dataAccess->nRanksLocal > 1) {
						if (dataAccess->rankLocal == 0) {
							// Gather and write:
							int n = dataAccess->nRanksLocal;
							int  bufferSizes[n-1];
							MPI_Status stats[n-1];
							MPI_Request reqs[n-1];
							bool    receives[n-1];
							int nReceives = 0;
							for (int r=1; r<n; r++) {
								receives[r-1] = false;
								err = MPI_Probe(r, 0, nodeComm, &stats[r-1]);
								if (err != MPI_SUCCESS) {
									fprintf(stderr, "Gather rank failed to probe msg from %d\n", r);
									MPI_Abort(MPI_COMM_WORLD, err);
									exit(EXIT_FAILURE);
								}
							}

							tt_sql::aggTimeData* buffers[n-1];

							printf("Root probing for sizes of %d ranks\n", n);
							for (int r=1; r<n; r++) {
								printf("Root probing for # from rank %d (n=%d)\n", r, n);
								err = MPI_Get_count(&stats[r-1], aggTimeRecord_MPI, &bufferSizes[r-1]);
								if (err != MPI_SUCCESS) {
									fprintf(stderr, "Gather rank failed on MPI_Get_count(rank=%d)\n", r);
									MPI_Abort(MPI_COMM_WORLD, err);
									exit(EXIT_FAILURE);
								}

								printf("Root expecting %d records from rank %d\n", bufferSizes[r-1], r);

								buffers[r-1] = (tt_sql::aggTimeData*)malloc(bufferSizes[r-1] * sizeof(tt_sql::aggTimeData));

								// err = MPI_Irecv(buffers[r-1], bufferSizes[r-1], aggTimeRecord_MPI, r, 0, nodeComm, &reqs[r-1]);
								// err = MPI_Recv(buffers[r-1], bufferSizes[r-1], aggTimeRecord_MPI, r, 0, nodeComm, &stats[r-1]);
								err = MPI_Recv(buffers[r-1], bufferSizes[r-1], aggTimeRecord_MPI_rs, r, 0, nodeComm, &stats[r-1]);
								if (err != MPI_SUCCESS) {
									fprintf(stderr, "Gather rank failed on MPI_Irecv(rank=%d)\n", r);
									MPI_Abort(MPI_COMM_WORLD, err);
									exit(EXIT_FAILURE);
								}

								printf("Root rank: buffers[r=%d][0].rank = %d\n", r, buffers[r-1][0].rank);
							}

							printf("Root waiting for data ...\n");
							// while (nReceives != (n - 1)) {
							// 	int indx;
							// 	MPI_Status s;
							// 	printf("Root checking for data\n");
							// 	err = MPI_Waitany(n-1, reqs, &indx, &s);
							// 	if (err != MPI_SUCCESS) {
							// 		fprintf(stderr, "Gather rank failed on MPI_Waitany\n");
							// 		MPI_Abort(MPI_COMM_WORLD, err);
							// 		exit(EXIT_FAILURE);
							// 	}
							// 	nReceives++;
							// 	int r = indx+1;
							// 	printf("Apparently, received data from rank %d\n", r);
							// }

						}
						else {
							// Send to local root
							MPI_Request r;
							MPI_Status s;
							printf("Rank %d sending %d records\n", dataAccess->rankLocal, dataAccess->aggTimeRecords.size());
							// err = MPI_Isend(dataAccess->aggTimeRecords.data(), dataAccess->aggTimeRecords.size(), aggTimeRecord_MPI, 0, 0, nodeComm, &r);
							// err = MPI_Send(dataAccess->aggTimeRecords.data(), dataAccess->aggTimeRecords.size(), aggTimeRecord_MPI, 0, 0, nodeComm);
							err = MPI_Send(dataAccess->aggTimeRecords.data(), dataAccess->aggTimeRecords.size(), aggTimeRecord_MPI_rs, 0, 0, nodeComm);
							if (err != MPI_SUCCESS) {
								fprintf(stderr, "Rankd %d failed Isend\n", r);
								MPI_Abort(MPI_COMM_WORLD, err);
								exit(EXIT_FAILURE);
							}
							// MPI_Wait(&r, &s);
							printf("Rank %d has completed transfer?\n", dataAccess->rankLocal);
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
						// tt_sql::drivers::writeAggregateTimeData(dataAccess, runID, rankGlobal, *callPathID, processID,
						// 				node.nodeData.blockTimer->aggTimings.minWalltime,
						// 				node.nodeData.blockTimer->aggTimings.avgWalltime,
						// 				node.nodeData.blockTimer->aggTimings.maxWalltime,
						// 				sqrt(node.nodeData.blockTimer->aggTimings.varianceWalltime),
						// 				node.nodeData.blockTimer->aggTimings.count,
						// 				&aggTimeID);
						tt_sql::aggTimeData d;
						d.runID = runID;
						d.rank = dataAccess.rankGlobal;
						d.callPathID = *callPathID;
						d.processID = processID;
						d.minWallTime = node.nodeData.blockTimer->aggTimings.minWalltime;
						d.avgWallTime = node.nodeData.blockTimer->aggTimings.avgWalltime;
						d.maxWallTime = node.nodeData.blockTimer->aggTimings.maxWalltime;
						d.stdev = sqrt(node.nodeData.blockTimer->aggTimings.varianceWalltime);
						d.count = node.nodeData.blockTimer->aggTimings.count;
						tt_sql::drivers::writeAggregateTimeData_v2(dataAccess, d, &aggTimeID);
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
							tt_sql::drivers::writeTraceTimeData(dataAccess,
																	 runID, *callPathID, processID,
																	 ptr->data.callEntryID, ptr->data.callExitID,
																	 ptr->data.wallTime,
																	 &traceTimeID);

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
