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

#include "tt_io_data.h"
#include "tt_io_json.h"
#include "mpi.h"
#include <unistd.h>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "tt_io_sqlite3.h"

namespace tt_sql = treetimer::database::tt_sqlite3;

namespace treetimer
{
	namespace io
	{
		namespace drivers
		{
			// Write all gathered data to file(s)
			void writeData(treetimer::config::State& state)
			{
				// Run Setup
				tt_sql::TTSQLite3* dataAccess = setupOutput(*(state.config));

				// Write RunConfig Data
				writeRunConfigData(*(state.config), dataAccess);

				// Write Aggregate Data
				writeAggData(*(state.config), *(state.callTree), dataAccess);

				// Write Trace Data
				writeTraceData(*(state.config), *(state.callTree), dataAccess);

				tt_sql::drivers::closeConnection(*dataAccess);
				delete(dataAccess);
			}

			// Writes current trace data (and any other required data for this to occur)
			// and then clears existing stored trace data in memory.
			void flushTraceData(treetimer::config::State& state)
			{
				// Flushing Trace Data requires us to:
				// (a) Store the run configuration
				// (b) Store any current information we have about profile nodes
				// (c) Store any current trace node data we have (but not aggregate data,
				//     since if it is not the run end then it will not be accurate)
				// (d) Clear any trace data stored in memory, and reset the linked lists
				//	   to free memory for new trace data.

				// Note: This function will be v. expensive due to the significant I/O.
				// Normally it is desirable to reserve writing such data to the end to avoid
				// skewing run timers in the program itself. Since we do not want to measure the cost
				// of this, pause any currently active timers, and resume them once it is complete.

				// Run Setup
				tt_sql::TTSQLite3* dataAccess = setupOutput(*(state.config));

				// Write RunConfig Data
				writeRunConfigData(*(state.config), dataAccess);

				// Write Trace Data
				writeTraceData(*(state.config), *(state.callTree), dataAccess);
				
				tt_sql::drivers::closeConnection(*dataAccess);
				delete(dataAccess);

				// Reset Trace Data Status
				resetTraceDataTree(*(state.callTree->root));
			}

			void resetTraceDataTree(treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node)
			{
				// Reset the trace data linked lists to free any linked list nodes
				node.nodeData.blockTimer->traceTimers.reset();

				// Reset the trace parameter data linked lists to free any linked list nodes
				std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>::iterator it_int;

				// Loop over integer parameters
				for(it_int = node.nodeData.intParameters.begin(); it_int != node.nodeData.intParameters.end(); it_int++)
				{
					// Reset Linked List for this parameter
					it_int->second->traceParams.reset();
				}

				// Reset the trace parameter data linked lists to free any linked list nodes
				std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::iterator it_double;

				// Loop over integer parameters
				for(it_double = node.nodeData.doubleParameters.begin(); it_double != node.nodeData.doubleParameters.end(); it_double++)
				{
					// Reset Linked List for this parameter
					it_double->second->traceParams.reset();
				}

				// Reset the trace parameter data linked lists to free any linked list nodes
				std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::iterator it_bool;

				// Loop over integer parameters
				for(it_bool = node.nodeData.boolParameters.begin(); it_bool != node.nodeData.boolParameters.end(); it_bool++)
				{
					// Reset Linked List for this parameter
					it_bool->second->traceParams.reset();
				}


				// Reset the trace parameter data linked lists to free any linked list nodes
				std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>::iterator it_string;

				// Loop over integer parameters
				for(it_string = node.nodeData.stringParameters.begin(); it_string != node.nodeData.stringParameters.end(); it_string++)
				{
					// Reset Linked List for this parameter
					it_string->second->traceParams.reset();
				}

				std::unordered_map<std::string, treetimer::data_structures::TreeNode<std::string,treetimer::measurement::InstrumentationData> *>::iterator it;

				// Loop over child nodes
				for(it = node.childNodes.begin(); it != node.childNodes.end(); it ++)
				{
					resetTraceDataTree(*(it->second));
				}
			}

			void createResultsDirectory(treetimer::config::Config& config)
			{
				// Make the directory
				mkdir(config.outputFolder.c_str(), 0700);
			}

			tt_sql::TTSQLite3* setupOutput(treetimer::config::Config& config)
			{
				// === Results Directory needs to be created ===
				int rank;
				MPI_Comm_rank(MPI_COMM_WORLD, &rank);

				if(rank == 0)
				{
					createResultsDirectory(config);
				}

				int err = MPI_Barrier(MPI_COMM_WORLD);

				return treetimer::io::tt_sqlite3::drivers::setupOutput(config);
			}

			void writeRunConfigData(treetimer::config::Config& config, tt_sql::TTSQLite3* access)
			{
				treetimer::io::tt_sqlite3::drivers::writeRunConfigData(config, access);
			}

			void writeAggData(treetimer::config::Config& config,
							  treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
							  tt_sql::TTSQLite3* access)
			{
				treetimer::io::tt_sqlite3::drivers::writeAggData(config, callTree, access);
			}

			void writeTraceData(treetimer::config::Config& config,
								treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
								tt_sql::TTSQLite3* access)
			{
				treetimer::io::tt_sqlite3::drivers::writeTraceData(config, callTree, access);
			}
		}
	}
}
