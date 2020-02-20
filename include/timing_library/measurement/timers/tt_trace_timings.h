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

#ifndef TT_TRACE_TIMINGS_INCLUDE_H
#define TT_TRACE_TIMINGS_INCLUDE_H

#include "tt_trace_timings_node.h"

namespace treetimer
{
	namespace timers
	{
		class TraceTimings
		{
			public:
				TraceTimings();
				~TraceTimings();

				TraceTimingsNode * head;
				TraceTimingsNode * tail;
		};

		namespace drivers
		{
			void resetTraceTimings(TraceTimings& timings);
			void addTime(TraceTimings& timings, double walltime, long callEntryID, long callExitID);
		}
	}
}

#endif
