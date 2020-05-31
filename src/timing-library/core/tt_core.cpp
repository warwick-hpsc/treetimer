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

#include "tt_core.h"
#include "tt_state.h"
#include "mpi.h"
#include "tt_code_block_type.h"
#include "tt_global.h"
#include <iostream>
#include "tt_io_data.h"
#include "tt_process_config.h"
#include <iostream>

extern const std::string codeBlockNames[TT_CODE_BLOCK_COUNT] = {std::string("Program"),
																std::string("Method"),
																std::string("Loop"),
																std::string("NonMPIMethodCall"),
																std::string("Unspecified"),
																std::string("MPICommCall"),
																std::string("MPISyncCall"),
																std::string("MPINonCommMethodCall"),
																std::string("MPIIOCall"),
																std::string("ComputeLoop")};

namespace treetimer
{
	namespace core
	{
		treetimer::config::State * instrumState;
		bool libInit = false;

		namespace drivers
		{
			void TreeTimerInit()
			{
				// Check MPI is initialised
			    int mpiInit;
			    MPI_Initialized(&mpiInit);

			    // ToDo: Error Check

			    // Create State Object
			    treetimer::core::instrumState = new treetimer::config::State();

			    // Needed to disable catching MPI Hooks while in library functions
			    treetimer::core::instrumState->config->inLibrary = true;

			    // ToDo: The SQL setup stuff really needs to be in a separate object
			    // so that we can manage the config setup separately (and thus disable
			    // the MPI capture while setting up the other configs - i.e. there
			    // should be a separate data object for managing internal data related
			    // to the library operation only, separate from the runtime configurations
			    // etc.

				int rank;
				MPI_Comm_rank(MPI_COMM_WORLD, &rank);
				 treetimer::core::instrumState->config->sqlIOFilename = "results." + std::to_string(rank) + ".db";

			    // (1) Read in configuration from environment into global state
			    treetimer::config::drivers::setConfigFromEnv(*(treetimer::core::instrumState->config));

			    // (2) Start 'Root' Block and return to program
			    treetimer::core::libInit = true;
			    treetimer::core::instrumState->config->inLibrary = false;
			    TreeTimerEnterBlock("ProgramRoot", TT_NODE_TYPE_PROG);
			}

			void TreeTimerFinalize()
			{
			    // Exit Root Block
			    TreeTimerExitBlock("ProgramRoot");

			    // Inidicate that we are now performing library operations
			    // Important since the effective tree location is now null
			    treetimer::core::instrumState->config->inLibrary = true;

			    // Check MPI hasn't been finalised yet - ToDo: Error Check
			    int mpiInit;
			    MPI_Initialized(&mpiInit);

			    // Output data via whichever means was requested in config
			    treetimer::io::drivers::writeData(*(treetimer::core::instrumState));

			    treetimer::core::libInit = false;

			    // Cleanup State Object
			    delete(treetimer::core::instrumState);
			}

			void TreeTimerEnterBlock(std::string blockName, CodeBlockType blockType)
			{
				// Move position in callpath tree
				instrumState->callTree->moveToChild(blockName);

				// Ensure that code block data is set (would be undefined for new nodes)
				instrumState->callTree->pos->nodeData.blockType = blockType;

				// Start instrumentation on data node
				treetimer::measurement::drivers::startInstrumentation(instrumState->callTree->pos->nodeData,
												  					  instrumState->config->eATimers,
																	  instrumState->config->eTTimers,
																	  instrumState->callTree->nodeEntryCount);
			}

			void TreeTimerExitBlock(std::string blockName)
			{
				// Debug/Error Check: Ensure that the block we are stopping is the same as the one we are in.
				if(blockName != instrumState->callTree->pos->key)
				{
					std::cout << "Warning: Exiting Block " << blockName << "but expected to be in block ";
					std::cout << instrumState->callTree->pos->key << " - recorded data will be invalid\n";
				}

				// Stop instrumentation on current data node
				treetimer::measurement::drivers::stopInstrumentation(instrumState->callTree->pos->nodeData,
																	 instrumState->config->eATimers,
																	 instrumState->config->eTTimers,
																	 instrumState->callTree->nodeExitCount);
				// Move position in tree
				instrumState->callTree->moveToParent();
			}

			void TreeTimerLogParameter(std::string paramName, int val)
			{
				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val,
															 	   instrumState->config->eAParam,
																   instrumState->config->eTParam,
																   instrumState->callTree->nodeEntryCount,
																   instrumState->callTree->nodeExitCount);
			}

			void TreeTimerLogParameter(std::string paramName, double val)
			{
				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val,
															 	   instrumState->config->eAParam,
																   instrumState->config->eTParam,
																   instrumState->callTree->nodeEntryCount,
																   instrumState->callTree->nodeExitCount);
			}

			void TreeTimerLogParameter(std::string paramName, bool val)
			{
				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val,
															 	   instrumState->config->eAParam,
																   instrumState->config->eTParam,
																   instrumState->callTree->nodeEntryCount,
																   instrumState->callTree->nodeExitCount);
			}

			void TreeTimerLogParameter(std::string paramName, std::string val)
			{
				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val,
															 	   instrumState->config->eAParam,
																   instrumState->config->eTParam,
																   instrumState->callTree->nodeEntryCount,
																   instrumState->callTree->nodeExitCount);
			}

			void TreeTimerFlushTraceData()
			{
				treetimer::io::drivers::flushTraceData(*(treetimer::core::instrumState));
			}
		}
	}
}
