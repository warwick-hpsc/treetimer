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

#ifndef TT_AGG_TIMER_INCLUDE
#define TT_AGG_TIMER_INCLUDE

namespace treetimer
{
	namespace timers
	{
		class AggTimings
		{
			public:
				// Constructor/Deconstructors
				AggTimings();
				~AggTimings();

				// Variables
				// Walltime - Aggregate Statistics
			   	double avgWalltime;
			   	double maxWalltime;
			   	double minWalltime;
			   	double totalWalltime;
			   	//double stDevWalltime;
				double varianceWalltime;

			   	// CPUtime - Aggregate Statistics
			   	double avgCPUTime;
			   	double maxCPUTime;
			   	double minCPUTime;
			   	double totalCPUTime;
			   	//double stDevCPUTime;
				double varianceCPUTime;

			   	// Call Count
			   	int count;
		};

		namespace drivers
		{
			void resetTimings(AggTimings& timings);
			void addTimes(AggTimings& timings, double wallTime, double CPUTime);
		}
	}
}

#endif
