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

#include "tt_timer.h"

#include "tt_systimers_linux.h"
#include "tt_agg_timings.h"
#include "tt_trace_timings.h"

namespace treetimer
{
	namespace timers
	{
		Timer::Timer()
		{
			drivers::resetTimer(*this);
		}

		Timer::~Timer()
		{

		}

		namespace drivers
		{
			void resetTimer(Timer& timer)
			{
				// Clear Temporary Variables
			   	timer.startWallTime = 0.0;
			   	timer.stopWallTime = 0.0;

			   	timer.startCPUTime = 0.0;
			   	timer.stopCPUTime = 0.0;

			   	// Reset Aggregate Data
			    resetTimings(timer.aggTimings);

			   	// Reset Trace Data
			}

			void startTimer(Timer& timer, bool eATimers, bool eTTimers, long callEntryID)
			{
				if(eATimers || eTTimers)
				{
					// Store when timer was started
					set_timers(&(timer.startCPUTime), &(timer.startWallTime));

					// Store the callpath node entry count at timer start
					timer.callEntryID = callEntryID;
				}
			}

			void stopTimer(Timer& timer, bool eATimers,  bool eTTimers, long callExitID)
			{
				if(eATimers || eTTimers)
				{
					// Store when timer was stopped
					set_timers(&(timer.stopCPUTime), &(timer.stopWallTime));

					// Store the callpath node entry count at timer stop
					timer.callExitID = callExitID;

					if(eATimers)
					{
						// Update Aggregate Times
						addTimes(timer.aggTimings, timer.stopWallTime - timer.startWallTime, timer.stopCPUTime - timer.startCPUTime);
					}

					if(eTTimers)
					{
						// Update Trace Times (only tracks walltime for now)
						TraceTimer tmp(timer.stopWallTime - timer.startWallTime, timer.callEntryID, timer.callExitID);
						timer.traceTimers.add(tmp);
					}
				}
			}
		}
	}
}
