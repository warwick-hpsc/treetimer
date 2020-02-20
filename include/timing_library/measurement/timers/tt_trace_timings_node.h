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

#ifndef TT_TRACE_TIMINGS_NODE_INCLUDE_H
#define TT_TRACE_TIMINGS_NODE_INCLUDE_H

namespace treetimer
{
	namespace timers
	{
		class TraceTimingsNode
		{
			public:
				TraceTimingsNode();
				TraceTimingsNode(double walltime, long callEntryID, long callExitID);
				~TraceTimingsNode();

				double wallTime;	// The measured walltime
				long callEntryID;	// Track the number of call path node entries when this time was measured
				long callExitID;	// Track the number of call path node exits when this time was measured
				TraceTimingsNode * next;
		};

		namespace drivers
		{
			void resetTraceTimingsNode(TraceTimingsNode& node);
			void setTraceTimingsNode(TraceTimingsNode& node, double walltime, long callEntryID, long callExitID);
		}
	}
}

#endif
