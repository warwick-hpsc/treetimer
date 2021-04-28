/*
 * @file
 * @author James Davis
 * @version 0.1
 *
 * @section LICENSE
 *
 * @section DESCRIPTION
 *
 *
 */

#ifndef TT_TRACE_PARAMETER_INCLUDE_H
#define TT_TRACE_PARAMETER_INCLUDE_H

#include <string>

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		class TraceParameter
		{
			public:
				// Constructors/Deconstructors
				TraceParameter();
				TraceParameter(T val, long nodeEntryID, long nodeExitID);
				~TraceParameter();

				T val;
				long nodeEntryID;	// Track which callpath node entry/exit this value is tied to
				long nodeExitID;
		};
	}
}

#endif
