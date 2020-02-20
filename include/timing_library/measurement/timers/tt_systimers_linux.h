/*
 * File:   timers.h
 * Author: AWE Plc (Tessella plc) (Taken from PMTM/Ichneia)
 *
 * This file specifies the routine needed to access time resources for the
 * given system.
 *
 */

#ifndef TT_SYSTIMERS_LINUX_INCLUDE_H
#define	TT_SYSTIMERS_LINUX_INCLUDE_H

#include <time.h>
#include <cstdlib>

namespace treetimer
{
	namespace timers
	{
		namespace drivers
		{
			inline void set_timers(double * cpu_time, double * elapsed_time)
			{
				struct timespec ts;
				struct timespec cpu_ts;

				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_ts);
				*cpu_time = cpu_ts.tv_sec + cpu_ts.tv_nsec * 1.0E-9;

				clock_gettime(CLOCK_MONOTONIC, &ts);
				*elapsed_time = ts.tv_sec + ts.tv_nsec * 1.0E-9;
			}
		}
	}
}

#endif
