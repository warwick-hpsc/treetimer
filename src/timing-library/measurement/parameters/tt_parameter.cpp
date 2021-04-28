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

		namespace drivers
		{
			template <class T>
			void addValue(Parameter<T>& param, T value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID)
			{
				// Update the aggregate parameter statistics
				if(eAparam)
				{
					updateValue(param.aggParam, value);
				}

				// Add a trace
				if(eTParam)
				{
					// We're going to make a copy of this object, so create it on the stack
					TraceParameter<T> tmp(value, nodeEntryID, nodeExitID);
					param.traceParams.add(tmp);
				}
			}

			template <>
			void addValue(Parameter<std::string>& param, std::string value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID)
			{
				// String Parameters do not have aggregate values, only traces

				// Add a trace
				if(eTParam)
				{
					// We're going to make a copy of this object, so create it on the stack
					TraceParameter<std::string> tmp(value, nodeEntryID, nodeExitID);
					param.traceParams.add(tmp);
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
// Drivers
template void treetimer::parameters::drivers::addValue<int>(Parameter<int>& param, int value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID);
template void treetimer::parameters::drivers::addValue<long>(Parameter<long>& param, long value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID);
template void treetimer::parameters::drivers::addValue<double>(Parameter<double>& param, double value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID);
template void treetimer::parameters::drivers::addValue<bool>(Parameter<bool>& param, bool value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID);
template void treetimer::parameters::drivers::addValue<>(Parameter<std::string>& param, std::string value, bool eAparam, bool eTParam, long nodeEntryID, long nodeExitID);

