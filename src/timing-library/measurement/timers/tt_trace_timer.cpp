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

#include "tt_trace_timer.h"

namespace treetimer
{
	namespace timers
	{
		TraceTimer::TraceTimer()
		{
			this->wallTime = 0.0;
			this->callEntryID = 0;
			this->callExitID = 0;
		}

		TraceTimer::TraceTimer(double wallTime, long callEntryID, long callExitID)
		{
			this->wallTime = wallTime;
			this->callEntryID = callEntryID;
			this->callExitID = callExitID;
		}

		TraceTimer::~TraceTimer()
		{

		}
	}
}
