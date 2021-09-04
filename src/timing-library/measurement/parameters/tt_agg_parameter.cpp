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

#include "tt_agg_parameter.h"
#include <climits>
#include <cfloat>
#include <iostream>

namespace treetimer
{
	namespace parameters
	{
		template <>
		AggParameter<int>::AggParameter()
		{
			this->minVal = INT_MAX;
			this->maxVal = INT_MIN;
			this->avgVal = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <>
		AggParameter<long>::AggParameter()
		{
			this->minVal = LONG_MAX;
			this->maxVal = LONG_MIN;
			this->avgVal = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <>
		AggParameter<double>::AggParameter()
		{
			this->minVal = DBL_MAX;
			this->maxVal = DBL_MIN;
			this->avgVal = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <>
		AggParameter<bool>::AggParameter()
		{
			// '1' is the largest possible value of min, so should be overwritten or equalled on first update
			this->minVal = 1;
			this->maxVal = 0;
			this->avgVal = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <>
		AggParameter<std::string>::AggParameter()
		{
			this->minVal = "";
			this->maxVal = "";
			this->avgVal = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <class T>
		AggParameter<T>::~AggParameter()
		{

		}

		namespace drivers
		{
			template <class T>
			void updateValue(AggParameter<T>& param, T value)
			{
				if(value < param.minVal) param.minVal = value;
				if(value > param.maxVal) param.maxVal = value;

				// Welford's Online Algorithm

				// Avg(Mean) Val
				// Need old mean for variance calculation
				double oldMean = param.avgVal;
				param.avgVal = ((param.avgVal * param.count) + value) / (param.count + 1);

				// Variance
				param.variance = ((param.variance * param.count) + ((value - oldMean) * (value - param.avgVal))) / (param.count + 1);

				// Count
				param.count++;

			}

			template <>
			void updateValue(AggParameter<bool>& param, bool value)
			{
				int tmp = value ? 1 : 0;

				if(tmp < param.minVal) param.minVal = tmp;
				if(tmp > param.maxVal) param.maxVal = tmp;

				// Avg(Mean) Val
				// Welford's Online Algorithm
				double oldMean = param.avgVal;
				param.avgVal = ((param.avgVal * param.count) + tmp) / (param.count + 1);

				// Variance
				param.variance = ((param.variance * param.count) + ((tmp - oldMean) * (tmp - param.avgVal))) / (param.count + 1);

				// Count
				param.count++;
			}

			template <>
			void updateValue(AggParameter<std::string>& param, std::string value)
			{
				// Most of this is blank for now - we can't really have aggregate
				// trackers for a string parameter. This exists mostly to override
				// the template, since the log function should not call this function.

				// Count
				param.count++;
			}
		}
	}
}

// Explicit Instantiation
// Classes
template class treetimer::parameters::AggParameter<int>;
template class treetimer::parameters::AggParameter<long>;
template class treetimer::parameters::AggParameter<double>;
template class treetimer::parameters::AggParameter<bool>;
template class treetimer::parameters::AggParameter<std::string>;

template void treetimer::parameters::drivers::updateValue<int>(AggParameter<int>& param, int value);
template void treetimer::parameters::drivers::updateValue<long>(AggParameter<long>& param, long value);
template void treetimer::parameters::drivers::updateValue<double>(AggParameter<double>& param, double value);
template void treetimer::parameters::drivers::updateValue<>(AggParameter<bool>& param, bool value);
template void treetimer::parameters::drivers::updateValue<>(AggParameter<std::string>& param, std::string value);
