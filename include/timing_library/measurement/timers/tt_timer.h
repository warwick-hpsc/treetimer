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

#ifndef TT_TIMER_INCLUDE
#define TT_TIMER_INCLUDE

#include "tt_agg_timings.h"
#include "tt_linkedlist.h"
#include "tt_trace_timer.h"

namespace treetimer
{
	namespace timers
	{
		class Timer
		{
			public:
				// Constructor/Deconstructors
				Timer();
				~Timer();

			   	// 'Hot' variables - used for in-progress timings
				// Walltime
			   	double startWallTime;
			   	double stopWallTime;

			   	// CPUTime
			   	double startCPUTime;
			   	double stopCPUTime;

			   	// Node Callpath Entry/Exit Counts
			   	long callEntryID;
			   	long callExitID;

				// Aggregate Timer Statistics
				AggTimings aggTimings;

				// Trace Timer Linked List
				treetimer::data_structures::LinkedList<TraceTimer> traceTimers;

		};

		namespace drivers
		{
			void resetTimer(Timer& timer);
			void startTimer(Timer& timer, bool eATimers, bool eTTimers, long callEntryID);
			void stopTimer(Timer& timer, bool eATimers,  bool eTTimers, long callExitID);
		}
	}
}

#endif
