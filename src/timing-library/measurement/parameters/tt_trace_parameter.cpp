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

#include "tt_trace_parameter.h"


namespace treetimer
{
	namespace parameters
	{

		template <class T>
		TraceParameter<T>::TraceParameter()
		{
			// Val is undefined - can't set default without knowing type
			// Make specific type constructors?
			this->nodeEntryID = 0;
			this->nodeExitID = 0;
		}

		template <class T>
		TraceParameter<T>::TraceParameter(T val, long nodeEntryID, long nodeExitID)
		{
			this->val = val;
			this->nodeEntryID = nodeEntryID;
			this->nodeExitID  = nodeExitID;
		}

		template <class T>
		TraceParameter<T>::~TraceParameter()
		{

		}

		namespace drivers
		{

		}

	}
}

// Explicit Instantiation
template class treetimer::parameters::TraceParameter<int>;
template class treetimer::parameters::TraceParameter<long>;
template class treetimer::parameters::TraceParameter<double>;
template class treetimer::parameters::TraceParameter<bool>;
template class treetimer::parameters::TraceParameter<std::string>;
