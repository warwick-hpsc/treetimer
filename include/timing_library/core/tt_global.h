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

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#ifndef DEBUGGABLE_ERROR
	#ifdef DEBUG
		// #define DEBUGGABLE_ERROR fprintf(stderr, "%s:%d\n\n", __FILE__, __LINE__); fflush(stderr); fflush(stdout);
 		#define DEBUGGABLE_ERROR {std::cerr << __FILE__ << ":" << STRINGIZE(__LINE__) << std::endl; }
	#else
		#define DEBUGGABLE_ERROR 
	#endif
#endif

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
