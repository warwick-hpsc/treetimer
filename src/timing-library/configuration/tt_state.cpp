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

#include "tt_state.h"
#include "tt_config.h"

namespace treetimer
{
	namespace config
	{
		State::State()
		{
			this->sleeping = false;

			this->config = new Config();
			this->callTree = new treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>();

			// Aggregation window management:
			// -- window stepping:
			this->aggStepInterval = -1;
			this->aggStepIntervalCounter = 0;
			// -- window synchronisation across nodes:
			this->currentAggWindow = 1;
			this->targetNumAggWindows = 1;

			this->traceConductorNodeName = std::string("");
			this->traceCallInterval = 0;
			this->traceCallIntervalCounter = 0;
			this->traceCallCollectionEnabled = false;
		}

		State::~State()
		{
			delete(this->config);
			delete(this->callTree);
		}

		namespace drivers
		{
			void resetState(State& state)
			{
				delete(state.config);
				delete(state.callTree);

				state.config = new Config();
				state.callTree = new treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>();
			}
		}
	}
}
