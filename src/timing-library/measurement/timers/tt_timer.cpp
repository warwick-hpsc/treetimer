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

namespace treetimer
{
	namespace timers
	{
		Timer::Timer()
		{
			this->resetTimer();
		}

		Timer::~Timer()
		{

		}

		void Timer::resetTimer()
		{
			// Clear Temporary Variables
		   	this->startWallTime = 0.0;
		   	this->stopWallTime = 0.0;

		   	this->startCPUTime = 0.0;
		   	this->stopCPUTime = 0.0;

		   	// Reset Aggregate Data
		    this->aggTimings.reset();

		   	// Reset Trace Data
		}

		void Timer::startTimer(bool eATimers, bool eTTimers, long callEntryID)
		{
			if(eATimers || eTTimers)
			{
				// Store when timer was started
				treetimer::timers::drivers::set_timers(&(this->startCPUTime), &(this->startWallTime));

				// Store the callpath node entry count at timer start
				this->callEntryID = callEntryID;
			}
		}

		void Timer::stopTimer(bool eATimers,  bool eTTimers, long callExitID)
		{
			if(eATimers || eTTimers)
			{
				// Store when timer was stopped
				treetimer::timers::drivers::set_timers(&(this->stopCPUTime), &(this->stopWallTime));

				// Store the callpath node entry count at timer stop
				this->callExitID = callExitID;

				if(eATimers)
				{
					// Update Aggregate Times
					if (this->aggTimings.size == 0) {
						this->aggTimings.add(AggTimings());
					}
					this->aggTimings.tail->data.addTimes(this->stopWallTime - this->startWallTime, this->stopCPUTime - this->startCPUTime);
				}

				if(eTTimers)
				{
					// Update Trace Times (only tracks walltime for now)
					TraceTimer tmp(this->stopWallTime - this->startWallTime, this->callEntryID, this->callExitID);
					this->traceTimers.add(tmp);
				}
			}
		}

		void Timer::setNumAggWindows(int n)
		{
			if (n > this->aggTimings.size) {
				while (n != this->aggTimings.size) {
					this->aggTimings.add(AggTimings());
				}
			}
		}
	}
}
