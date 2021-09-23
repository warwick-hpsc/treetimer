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

#include "tt_agg_timings.h"
#include "float.h"
#include "limits.h"

namespace treetimer
{
	namespace timers
	{
		AggTimings::AggTimings()
		{
			this->resetTimings();
		}

		AggTimings::~AggTimings()
		{

		}

		void AggTimings::resetTimings()
		{
			// Variables
			// Walltime - Aggregate Statistics
		   	this->avgWalltime = 0.0;
		   	this->maxWalltime = DBL_MIN;
		   	this->minWalltime = DBL_MAX;
		   	this->totalWalltime = 0.0;
		   	this->varianceWalltime = 0.0;

		   	// CPUtime - Aggregate Statistics
		   	this->avgCPUTime = 0.0;
		   	this->maxCPUTime = DBL_MIN;
		   	this->minCPUTime = DBL_MAX;
		   	this->totalCPUTime = 0.0;
		   	this->varianceCPUTime = 0.0;

		   	// Call Count
		   	this->count = 0;
		}

		void AggTimings::addTimes(double wallTime, double CPUTime)
		{
			this->totalWalltime += wallTime;
			this->totalCPUTime += CPUTime;

			// Mean
			double oldWalltimeMean = this->avgWalltime;
			this->avgWalltime = ((this->avgWalltime * this->count) + wallTime)/ (this->count+1);

			double oldCPUTimeMean = this->avgCPUTime;
			this->avgCPUTime = ((this->avgWalltime * this->count) + CPUTime)/ (this->count+1);

			// Variance
            this->varianceWalltime = ((this->varianceWalltime * this->count) + ((wallTime - oldWalltimeMean) * (wallTime - this->avgWalltime))) / (this->count + 1);
            this->varianceCPUTime = ((this->varianceCPUTime * this->count) + ((CPUTime - oldCPUTimeMean) * (wallTime - this->avgCPUTime))) / (this->count + 1);

			// Count
			this->count++;

			// Min/Max
			if (wallTime < this->minWalltime) this->minWalltime = wallTime;
			if (wallTime > this->maxWalltime) this->maxWalltime = wallTime;
			if (CPUTime < this->minCPUTime) this->minCPUTime = CPUTime;
			if (CPUTime > this->maxCPUTime) this->maxCPUTime = CPUTime;
		}
	}
}
