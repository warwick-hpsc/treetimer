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

namespace treetimer
{
	namespace io
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void setupOutput(treetimer::config::Config& config)
				{
					if(config.sqlIOSetup == false)
					{
						// Create SQL Access Object
						treetimer::database::tt_sqlite3::TTSQLite3 * dataAccess = new treetimer::database::tt_sqlite3::TTSQLite3(config.outputFolder + "/" + config.sqlIOFilename);

						// Open Connection
						treetimer::database::tt_sqlite3::drivers::openConnection(*dataAccess);

						// Setup the Database Schemas
						// (a) Block Types
						treetimer::database::tt_sqlite3::drivers::writeSchemaProfileNodeTypeData(*dataAccess);

						// (b) Library Configuration
						treetimer::database::tt_sqlite3::drivers::writeSchemaLibraryConfigData(*dataAccess);

						// (c) Singular Parameters (used for Global Parameter Storage)
						treetimer::database::tt_sqlite3::drivers::writeSchemaParameterData(*dataAccess);

						// (d) Application Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaApplicationData(*dataAccess);

						// (e) Application Configuration Data (Links Combinations of Global Parameters)
						treetimer::database::tt_sqlite3::drivers::writeSchemaApplicationConfigData(*dataAccess);

						// (f) Profile Nodes
						treetimer::database::tt_sqlite3::drivers::writeSchemaProfileNodeData(*dataAccess);

						// (g) CPU Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaCPUData(*dataAccess);

						// (h) Machine Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaMachineData(*dataAccess);

						// (i) Compute Node Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaComputeNodeData(*dataAccess);

						// (j) Compute Node Socket Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaCPUSocketData(*dataAccess);

						// (k) Compute Core Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaCPUCoreData(*dataAccess);

						// (l) Call Path Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaCallPathData(*dataAccess);

						// (m) MPI Process Data
						treetimer::database::tt_sqlite3::drivers::writeSchemaProcessData(*dataAccess);

						// (n) Run Data (Configurations, Process Count etc)
						treetimer::database::tt_sqlite3::drivers::writeSchemaProfileRunConfigData(*dataAccess);

						// (o) Aggregate Parameter Data (Run Specific)
						treetimer::database::tt_sqlite3::drivers::writeSchemaAggregateParameterData(*dataAccess);

						// (p) Aggregate Timer Data (Run Specific)
						treetimer::database::tt_sqlite3::drivers::writeSchemaAggregateTimeData(*dataAccess);

						// (q) Trace Parameter Data (Run Specific)
						treetimer::database::tt_sqlite3::drivers::writeSchemaTraceParameterData(*dataAccess);

						// (r) Trace Timer Data (Run Specific)
						treetimer::database::tt_sqlite3::drivers::writeSchemaTraceTimeData(*dataAccess);

						// Close Connection
						treetimer::database::tt_sqlite3::drivers::closeConnection(*dataAccess);

						// Cleanup
						delete(dataAccess);

						config.sqlIOSetup = true;
					}
				}

				void writeRunConfigData(treetimer::config::Config& config)
				{
					// Write data pertaining to the run configuration

					// Note: This can only be done once. Any global parameters set after this step will
					// not be stored. It must be done prior to writing any instrumentation data,
					// so should be performed prior to a trace/full data output if not yet performed.

					// At completion, set a suitable runID for reuse during any data writes.

					if(config.sqlIORunConfig == false)
					{
						treetimer::database::tt_sqlite3::TTSQLite3 * dataAccess = new treetimer::database::tt_sqlite3::TTSQLite3(config.outputFolder + "/" + config.sqlIOFilename);

						// Profile Node Type Data
						treetimer::database::tt_sqlite3::drivers::openConnection(*dataAccess);

						for(int i = 0; i < TT_CODE_BLOCK_COUNT; i++)
						{
							int dbID;
							treetimer::database::tt_sqlite3::drivers::writeProfileNodeTypeData(*dataAccess, codeBlockNames[i], &dbID);
						}

						// Library Config Data
						int libConfigID;
						treetimer::database::tt_sqlite3::drivers::writeLibraryConfigID(*dataAccess, config.libVerMajor, config.libVerMinor,
																						 config.eATimers, config.eTTimers,
																						 config.eAParam, config.eTParam,
																						 config.eAPAPI, config.eTPAPI,
																						 config.eMPIHooks, &libConfigID);
						// Application Data
						int appID;
						treetimer::database::tt_sqlite3::drivers::writeApplicationData(*dataAccess, config.appName, config.appVersion, &appID);

						// Machine Data
						int machineID;
						treetimer::database::tt_sqlite3::drivers::writeMachineData(*dataAccess, config.machineName, &machineID);

						// CPU Data
						int cpuID;
						std::string cpuModel = "Unknown";
						treetimer::database::tt_sqlite3::drivers::writeCPUData(*dataAccess, cpuModel, &cpuID);

						// Compute Node Data
						int computeNodeID;
						std::string nodeName = "Unknown";
						int socketCount = -1;
						treetimer::database::tt_sqlite3::drivers::writeComputeNodeData(*dataAccess, machineID, nodeName,
																					   socketCount, &computeNodeID);

						// CPU Socket Data
						int cpuSocketID;
						int phySocketNum = -1;
						treetimer::database::tt_sqlite3::drivers::writeCPUSocketData(*dataAccess, computeNodeID, cpuID, phySocketNum, &cpuSocketID);

						// CPU Core Data
						int cpuCoreID;
						int physicalCoreNum = -1;
						treetimer::database::tt_sqlite3::drivers::writeCPUCoreData(*dataAccess, cpuSocketID, physicalCoreNum, &cpuCoreID);

						// ProcessID
						int processID;
						int rank;
						MPI_Comm_rank(MPI_COMM_WORLD, &rank);
						treetimer::database::tt_sqlite3::drivers::writeProcessData(*dataAccess, cpuCoreID,  rank, &processID);

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
						treetimer::database::tt_sqlite3::drivers::writeApplicationConfigData(*dataAccess,
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
						treetimer::database::tt_sqlite3::drivers::writeProfileRunConfigData(*dataAccess,
																							appConfigID,
																							libConfigID,
																							processCount,
																							&runID);

						config.sqlIORunID = runID;
						config.sqlIOProcessID = processID;

						// Close Connection
						treetimer::database::tt_sqlite3::drivers::closeConnection(*dataAccess);

						// Cleanup
						delete(dataAccess);

						config.sqlIORunConfig = true;
					}
				}

				void writeAggData(treetimer::config::Config& config,
								  treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree)
				{
					// ToDo: Error Check - ensure database has been setup

					if(config.sqlIOAggData == true)
					{
						return;
					}

					// Open an access object
					treetimer::database::tt_sqlite3::TTSQLite3 * dataAccess = new treetimer::database::tt_sqlite3::TTSQLite3(config.outputFolder + "/" + config.sqlIOFilename);
					treetimer::database::tt_sqlite3::drivers::openConnection(*dataAccess);

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeAggInstrumentationData, config.sqlIORunID, config.sqlIOProcessID, -1, config);

					treetimer::database::tt_sqlite3::drivers::closeConnection(*dataAccess);

					delete(dataAccess);

					config.sqlIOAggData = true;
				}

				void writeTraceData(treetimer::config::Config& config,
									treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree)
				{
					// ToDo: Error Check - ensure database has been setup

					// Open an access object
					treetimer::database::tt_sqlite3::TTSQLite3 * dataAccess = new treetimer::database::tt_sqlite3::TTSQLite3(config.outputFolder + "/" + config.sqlIOFilename);
					treetimer::database::tt_sqlite3::drivers::openConnection(*dataAccess);

					// Start at the root of the tree - there is no valid parentID so pass as -1
					callTreeTraversal(*dataAccess, *(callTree.root), writeTreeNodeTraceInstrumentationData, config.sqlIORunID, config.sqlIOProcessID, -1, config);

					treetimer::database::tt_sqlite3::drivers::closeConnection(*dataAccess);

					delete(dataAccess);

					// We may wish to write trace data multiple times, this is just an indicator that we've done it at least once
					config.sqlIOTraceData = true;
				}

				void writeTreeNodeAggInstrumentationData(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
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
					treetimer::database::tt_sqlite3::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.nodeData.blockType], &blockTypeID);

					// (a) Profile Nodes
					int profileNodeID;
					treetimer::database::tt_sqlite3::drivers::writeTreeNodeProfileNodeData(dataAccess, node.key, blockTypeID, &profileNodeID);

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
						//treetimer::database::tt_sqlite3::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.parent->nodeData.blockType], &parentBlockType);
						//treetimer::database::tt_sqlite3::drivers::findProfileNodeID(dataAccess, parentName, parentBlockType, &parentNodeID);

						treetimer::database::tt_sqlite3::drivers::writeCallPathData(dataAccess, profileNodeID, parentID, callPathID);
					}
					else
					{
						treetimer::database::tt_sqlite3::drivers::writeCallPathData(dataAccess, profileNodeID, -1, callPathID);
					}

					// (c) Aggregate Time Data
					// Only write data if aggregate timers are enabled

					if(config.eATimers)
					{
						int aggTimeID;
						treetimer::database::tt_sqlite3::drivers::writeAggregateTimeData(dataAccess, runID, *callPathID, processID,
										node.nodeData.blockTimes->aggTimings.minWalltime,
										node.nodeData.blockTimes->aggTimings.avgWalltime,
										node.nodeData.blockTimes->aggTimings.maxWalltime,
										sqrt(node.nodeData.blockTimes->aggTimings.varianceWalltime),
										node.nodeData.blockTimes->aggTimings.count,
										&aggTimeID);
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
							treetimer::database::tt_sqlite3::drivers::writeAggregateParameterIntData(
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
							treetimer::database::tt_sqlite3::drivers::writeAggregateParameterFloatData(
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
							treetimer::database::tt_sqlite3::drivers::writeAggregateParameterBoolData(
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

				void writeTreeNodeTraceInstrumentationData(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
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
					treetimer::database::tt_sqlite3::drivers::findProfileNodeTypeID(dataAccess, codeBlockNames[node.nodeData.blockType], &blockTypeID);

					// (a) Profile Nodes
					int profileNodeID;
					treetimer::database::tt_sqlite3::drivers::writeTreeNodeProfileNodeData(dataAccess, node.key, blockTypeID, &profileNodeID);

					// (b) Call Path Data
					// CallPath Data should have been written for the parent as part of the tree traversal, and identified by parentID.

					// ToDo: Is there a better way of handling no parent (i.e. root node?) - NULL would be equivalent.
					// Could create an 'unknown' profile node to point to, that has a profile node entry but no callpath node entry
					if(node.parent != nullptr)
					{
						treetimer::database::tt_sqlite3::drivers::writeCallPathData(dataAccess, profileNodeID, parentID, callPathID);
					}
					else
					{
						treetimer::database::tt_sqlite3::drivers::writeCallPathData(dataAccess, profileNodeID, -1, callPathID);
					}

					// (c) Trace Times

					if(config.eTTimers)
					{
						// Loop over trace entries for the timer
						treetimer::data_structures::LinkedListNode<treetimer::timers::TraceTimer> * ptr = node.nodeData.blockTimes->traceTimers.head;

						while(ptr != nullptr)
						{
							int traceTimeID;
							treetimer::database::tt_sqlite3::drivers::writeTraceTimeData(dataAccess,
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
								treetimer::database::tt_sqlite3::drivers::writeTraceParameterIntData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeCallEntry,
										 ptr->data.nodeCallExit,
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
								treetimer::database::tt_sqlite3::drivers::writeTraceParameterFloatData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeCallEntry,
										 ptr->data.nodeCallExit,
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
								treetimer::database::tt_sqlite3::drivers::writeTraceParameterBoolData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeCallEntry,
										 ptr->data.nodeCallExit,
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
								treetimer::database::tt_sqlite3::drivers::writeTraceParameterStringData(dataAccess,
										 runID, processID, *callPathID,
										 ptr->data.nodeCallEntry,
										 ptr->data.nodeCallExit,
										 it_string->first,
										 ptr->data.val,
										 &traceParamID);

								ptr = ptr->next;
							}
						}
					}

				}

				void callTreeTraversal(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
									   treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
									   void (*func)(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
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
