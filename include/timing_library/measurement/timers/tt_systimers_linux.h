/*
Copyright (c) British Crown Owned Copyright 2013/AWE
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of AWE Plc nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
			inline void set_timers(double *cpu_time, double *elapsed_time)
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
