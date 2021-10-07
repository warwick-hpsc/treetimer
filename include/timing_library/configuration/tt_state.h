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

#ifndef TT_STATE_INCLUDE_H
#define TT_STATE_INCLUDE_H

#include "tt_config.h"
#include <string>
#include "tt_instrumentation_data.h"
#include "tt_tree.h"

namespace treetimer
{
	namespace config
	{
		class State
		{
			public:
				State();
				~State();

				bool sleeping; // If true, don't store any measured data. Call-tree traversal unaffected.

				Config *config;
				treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData> *callTree;

				// Aggregation window management:
				// -- window stepping:
				int aggStepInterval;
				int aggStepIntervalCounter;
				// -- window synchronisation across nodes:
				int currentAggWindow;
				int targetNumAggWindows;

				std::string traceConductorNodeName;
				int traceCallInterval;
				int traceCallIntervalCounter;
				bool traceCallCollectionEnabled;
		};

		namespace drivers
		{
			void resetState(State *state);
		}
	}
}

#endif
