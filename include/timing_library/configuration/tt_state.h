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

				Config * config;
				treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData> * callTree;
		};

		namespace drivers
		{
			void resetState(State* state);
		}
	}
}

#endif
