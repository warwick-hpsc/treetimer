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

#include "tt_parameter.h"
#include <string>
#include <iostream>

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		Parameter<T>::Parameter()
		{

		}

		template <class T>
		Parameter<T>::Parameter(std::string name)
		{
			this->name = name;
		}

		template <class T>
		Parameter<T>::~Parameter()
		{
			// Nothing to do currently
		}

		template <class T>
		void Parameter<T>::addValue(T value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID)
		{
			// Update the aggregate parameter statistics
			if(eAparam)
			{
				if (this->aggValues.size == 0) {
					this->aggValues.add(AggParameter<T>());
				}
				this->aggValues.tail->data.updateValue(value);
			}

			// Add a trace
			if(eTParam)
			{
				// We're going to make a copy of this object, so create it on the stack
				TraceParameter<T> tmp(value, nodeEntryID, nodeExitID);
				this->traceParams.add(tmp);
			}
		}

		template <>
		void Parameter<std::string>::addValue(std::string value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID)
		{
			// String Parameters do not have aggregate values, only traces

			// Add a trace
			if(eTParam)
			{
				// We're going to make a copy of this object, so create it on the stack
				TraceParameter<std::string> tmp(value, nodeEntryID, nodeExitID);
				this->traceParams.add(tmp);
			}
		}

		template<class T>
		void Parameter<T>::setNumAggWindows(int n)
		{
			if (n > this->aggValues.size) {
				while (n != this->aggValues.size) {
					this->aggValues.add(AggParameter<T>());
				}
			}
		}
	}
}

// Explicit Instantiation
// Class
template class treetimer::parameters::Parameter<int>;
template class treetimer::parameters::Parameter<long>;
template class treetimer::parameters::Parameter<double>;
template class treetimer::parameters::Parameter<bool>;
template class treetimer::parameters::Parameter<std::string>;

