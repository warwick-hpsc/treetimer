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
			this->nodeCallEntry = 0;
			this->nodeCallExit = 0;
		}

		template <class T>
		TraceParameter<T>::TraceParameter(T val, long nodeCallEntry, long nodeCallExit)
		{
			this->val = val;
			this->nodeCallEntry = nodeCallEntry;
			this->nodeCallExit = nodeCallExit;
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
using namespace treetimer::parameters;

// Class
template class TraceParameter<int>;
template class TraceParameter<double>;
template class TraceParameter<bool>;
template class TraceParameter<std::string>;
