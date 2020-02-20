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
			drivers::resetTimings(*this);
		}

		AggTimings::~AggTimings()
		{

		}

		namespace drivers
		{
			void resetTimings(AggTimings& timings)
			{
				// Variables
				// Walltime - Aggregate Statistics
			   	timings.avgWalltime = 0.0;
			   	timings.maxWalltime = DBL_MIN;
			   	timings.minWalltime = DBL_MAX;
			   	timings.totalWalltime = 0.0;
			   	timings.varianceWalltime = 0.0;

			   	// CPUtime - Aggregate Statistics
			   	timings.avgCPUTime = 0.0;
			   	timings.maxCPUTime = DBL_MIN;
			   	timings.minCPUTime = DBL_MAX;
			   	timings.totalCPUTime = 0.0;
			   	timings.varianceCPUTime = 0.0;

			   	// Call Count
			   	timings.count = 0;
			}

			void addTimes(AggTimings& timings, double wallTime, double CPUTime)
			{
				timings.totalWalltime = timings.totalWalltime + wallTime;
				timings.totalCPUTime = timings.totalCPUTime + CPUTime;

				// Mean
				double oldWalltimeMean = timings.avgWalltime;
				timings.avgWalltime = ((timings.avgWalltime * timings.count) + wallTime)/ ((timings.count) + 1);

				double oldCPUTimeMean = timings.avgCPUTime;
				timings.avgCPUTime = ((timings.avgWalltime * timings.count) + CPUTime)/ ((timings.count) + 1);

                // Variance
                timings.varianceWalltime = ((timings.varianceWalltime * timings.count) + ((wallTime - oldWalltimeMean) * (wallTime - timings.avgWalltime))) / (timings.count + 1);
                timings.varianceCPUTime = ((timings.varianceCPUTime * timings.count) + ((CPUTime - oldCPUTimeMean) * (wallTime - timings.avgCPUTime))) / (timings.count + 1);

				// Count
				timings.count = (timings.count) + 1;

				// Min/Max
				if(wallTime < timings.minWalltime)
				{
					timings.minWalltime = wallTime;
				}

				if(CPUTime < timings.minCPUTime)
				{
					timings.minCPUTime = CPUTime;
				}

				if(wallTime > timings.maxWalltime)
				{
					timings.maxWalltime = wallTime;
				}

				if(CPUTime > timings.maxCPUTime)
				{
					timings.maxCPUTime = CPUTime;
				}
			}
		}
	}
}
