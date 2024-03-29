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

#ifndef TT_PARAMETER_INCLUDE_H
#define TT_PARAMETER_INCLUDE_H

#include <string>
#include "tt_linkedlist.h"
#include "tt_trace_parameter.h"
#include "tt_agg_parameter.h"

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		class Parameter
		{
			public:
				// Constructors/Deconstructors
				Parameter();
				Parameter(std::string name);
				~Parameter();

				void addValue(T value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID);

				void setNumAggWindows(int n);

				std::string name;

				// Aggregate Parameter Values
				treetimer::data_structures::LinkedList<AggParameter<T>> aggValues;

				// Trace Parameter Values
				treetimer::data_structures::LinkedList<TraceParameter<T>> traceParams;
		};
	}
}

#endif
