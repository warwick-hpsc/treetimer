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

// Carefully ensure codeBlockNames[] matches with 'enum CodeBlockType' in 'tt_code_block_type.h'.
// Logic would dictate that variables should be declared in same file, but 
// I lack the CPP knowledge to achieve this.
extern const std::string codeBlockNames[TT_CODE_BLOCK_COUNT] = {std::string("Program"),
																std::string("Method"),
																std::string("Block"),
																std::string("Loop"),
																std::string("Compute"),
																std::string("MPICommCall"),
																std::string("MPISyncCall"),
																std::string("MPICollectiveCall"),
																std::string("MPIIOCall"),
																std::string("LibraryCall"),
																std::string("TraceConductor"),
																std::string("AggregationStepper")};
																// std::string("MPINonCommMethodCall"),
																// std::string("NonMPIMethodCall"),
																// std::string("Unspecified"),

namespace treetimer
{
	namespace core
	{
		treetimer::config::State *instrumState = nullptr;
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
			    treetimer::core::instrumState->config->setFromEnv();

			    // Configure 'Trace Conductor'
			    //treetimer::core::instrumState->traceCallCollectionEnabled = treetimer::core::instrumState->config->eTTimers;
			    treetimer::core::instrumState->traceCallCollectionEnabled = treetimer::core::instrumState->config->eTTimers || treetimer::core::instrumState->config->eTParam;

			    // (2) Start 'Root' Block and return to program
			    treetimer::core::libInit = true;
			    treetimer::core::instrumState->config->inLibrary = false;
			    TreeTimerEnterBlock("ProgramRoot", TT_NODE_TYPE_PROG);
			}

			void TreeTimerFinalize()
			{
			    // Exit Root Block
			    TreeTimerExit("ProgramRoot");

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

			void TreeTimerSleep()
			{
				treetimer::core::instrumState->sleeping = true;
			}

			void TreeTimerWake()
			{
				treetimer::core::instrumState->sleeping = false;
			}

			void TreeTimerEnterBlock(std::string blockName, CodeBlockType blockType)
			{
				if (!treetimer::core::instrumState->sleeping) {
					if (blockName == instrumState->config->aggregationStepperNodeName) {
						if (instrumState->config->eATimers || instrumState->config->eAParam) {
							int stepInterval = instrumState->config->aggregationStepInterval;
							if (stepInterval <= 0) stepInterval = 1;
							if (instrumState->aggStepInterval <= 0) {
								// Initialise
								instrumState->aggStepInterval = stepInterval;
								instrumState->aggStepIntervalCounter = stepInterval;
								instrumState->targetNumAggWindows = 1;
								// Negate the subsequent decrement:
								instrumState->aggStepIntervalCounter++;
							}

							// Decide whether to step to next aggregation window:
							instrumState->aggStepIntervalCounter--;
							if (instrumState->aggStepIntervalCounter <= 0) {
								// Trigger a step to a next window
								instrumState->targetNumAggWindows++;
								instrumState->aggStepIntervalCounter = instrumState->aggStepInterval;
							}
						}
					}

					if (blockName == instrumState->config->traceConductorNodeName) {
						if (instrumState->config->eTTimers || instrumState->config->eTParam) {
							int traceCallInterval = instrumState->config->traceConductorInterval;
							if (traceCallInterval <= 0) traceCallInterval = 1;
							if (instrumState->traceConductorNodeName == "") {
								// Initialise
								instrumState->traceConductorNodeName = blockName;
								instrumState->traceCallInterval = traceCallInterval;
								instrumState->traceCallIntervalCounter = 0;
							}

							// Decide whether to enable/block trace collection during this call:
							instrumState->traceCallIntervalCounter--;
							if (instrumState->traceCallIntervalCounter <= 0) {
								// Enable trace collection
								instrumState->traceCallCollectionEnabled = true;
								instrumState->traceCallIntervalCounter = instrumState->traceCallInterval;
							}
							else {
								// Disable trace collection
								instrumState->traceCallCollectionEnabled = false;
							}
						}
					}
				}

				// Move position in callpath tree
				instrumState->callTree->moveToChild(blockName);

				// Ensure that code block data is set (would be undefined for new nodes)
				instrumState->callTree->pos->nodeData.blockType = blockType;

				instrumState->callTree->pos->nodeData.currentNodeEntryID = instrumState->callTree->nodeEntryCounter;

				instrumState->callTree->pos->nodeData.instrumentThisVisit = ! treetimer::core::instrumState->sleeping;
				if (instrumState->callTree->pos->parent == nullptr) {
					// Always instrument root node, need to know walltime
					instrumState->callTree->pos->nodeData.instrumentThisVisit = true;
				}

				// Ensure each node is on same aggregation window
				instrumState->callTree->pos->nodeData.targetNumAggWindows = instrumState->targetNumAggWindows;

				if (instrumState->callTree->pos->nodeData.instrumentThisVisit) {
					// Start instrumentation on data node
					treetimer::measurement::drivers::startInstrumentation(
						instrumState->callTree->pos->nodeData,
						instrumState->config->eATimers,
						// instrumState->config->eTTimers,
						instrumState->config->eTTimers && instrumState->traceCallCollectionEnabled,
						// (instrumState->config->eTTimers || instrumState->config->eTParam) && instrumState->traceCallCollectionEnabled,
						instrumState->callTree->nodeEntryCounter);
				}
			}

			void TreeTimerExit(std::string blockName)
			{
				// Debug/Error Check: Ensure that the block we are stopping is the same as the one we are in.
				if(blockName != instrumState->callTree->pos->key)
				{
					std::cout << "Warning: Exiting Block " << blockName << " but expected to be in block ";
					std::cout << instrumState->callTree->pos->key << " - recorded data will be invalid\n";
				}

				// nodeVisitCounter not updated until moveToParent() called, so need to 
				// pre-emptively increment
				int nodeExitID = instrumState->callTree->nodeVisitCounter+1;

				if (instrumState->callTree->pos->nodeData.instrumentThisVisit) {
					// Stop instrumentation on current data node
					treetimer::measurement::drivers::stopInstrumentation(
						instrumState->callTree->pos->nodeData,
						instrumState->config->eATimers,
						instrumState->config->eTTimers && instrumState->traceCallCollectionEnabled,
						// (instrumState->config->eTTimers || instrumState->config->eTParam) && instrumState->traceCallCollectionEnabled,
						nodeExitID);

					treetimer::measurement::drivers::commitParameters(
						instrumState->callTree->pos->nodeData, 
						instrumState->config->eAParam,
						instrumState->config->eTParam && instrumState->traceCallCollectionEnabled,
						instrumState->callTree->pos->nodeData.currentNodeEntryID, 
						nodeExitID);
				}

				// Move position in tree
				instrumState->callTree->moveToParent();
			}

			void TreeTimerLogParameter(std::string paramName, int val)
			{
				if (! instrumState->callTree->pos->nodeData.instrumentThisVisit) return;

				bool inMPIBlock =  (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COMM_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_SYNC_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COLLECTIVE_CALL);
				if (instrumState->config->eTParam && inMPIBlock && !instrumState->config->eMPIHooksTParam) {
					return;
				}

				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val);
			}

			void TreeTimerLogParameter(std::string paramName, long val)
			{
				if (! instrumState->callTree->pos->nodeData.instrumentThisVisit) return;

				bool inMPIBlock =  (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COMM_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_SYNC_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COLLECTIVE_CALL);
				if (instrumState->config->eTParam && inMPIBlock && !instrumState->config->eMPIHooksTParam) {
					return;
				}

				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val);
			}

			void TreeTimerLogParameter(std::string paramName, double val)
			{
				if (! instrumState->callTree->pos->nodeData.instrumentThisVisit) return;

				bool inMPIBlock =  (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COMM_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_SYNC_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COLLECTIVE_CALL);
				if (instrumState->config->eTParam && inMPIBlock && !instrumState->config->eMPIHooksTParam) {
					return;
				}

				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val);
			}

			void TreeTimerLogParameter(std::string paramName, bool val)
			{
				if (! instrumState->callTree->pos->nodeData.instrumentThisVisit) return;

				bool inMPIBlock =  (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COMM_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_SYNC_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COLLECTIVE_CALL);
				if (instrumState->config->eTParam && inMPIBlock && !instrumState->config->eMPIHooksTParam) {
					return;
				}

				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val);
			}

			void TreeTimerLogParameter(std::string paramName, std::string val)
			{
				if (! instrumState->callTree->pos->nodeData.instrumentThisVisit) return;

				bool inMPIBlock =  (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COMM_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_SYNC_CALL) 
								|| (instrumState->callTree->pos->nodeData.blockType == TT_NODE_TYPE_MPI_COLLECTIVE_CALL);
				if (instrumState->config->eTParam && inMPIBlock && !instrumState->config->eMPIHooksTParam) {
					return;
				}

				// Store/Update the parameter value under the current active node
				treetimer::measurement::drivers::logParameterValue(instrumState->callTree->pos->nodeData, paramName, val);
			}

			void TreeTimerFlushTraceData()
			{
				treetimer::io::drivers::flushTraceData(*(treetimer::core::instrumState));
			}
		}
	}
}
