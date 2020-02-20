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

#ifndef TT_GLOBAL_INCLUDE_H
#define TT_GLOBAL_INCLUDE_H

#include "tt_state.h"

namespace treetimer
{
	namespace core
	{
		// Store as a global reference for the library state.
		// Needed in instances to access global config, current tree state etc
		extern treetimer::config::State * instrumState;
		extern bool libInit;
	}
}

#endif
