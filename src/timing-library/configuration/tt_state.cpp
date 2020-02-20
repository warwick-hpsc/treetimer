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
			this->config = new Config();
			this->callTree = new treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>();
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
