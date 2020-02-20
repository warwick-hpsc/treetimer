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
			//this->stdDev = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <>
		AggParameter<double>::AggParameter()
		{
			this->minVal = DBL_MAX;
			this->maxVal = DBL_MIN;
			this->avgVal = 0.0;
			//this->stdDev = 0.0;
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
			//this->stdDev = 0.0;
			this->variance = 0.0;
			this->count = 0;
		}

		template <>
		AggParameter<std::string>::AggParameter()
		{
			this->minVal = "";
			this->maxVal = "";
			this->avgVal = 0.0;
			//this->stdDev = 0.0;
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

				// Minval
				if(value < param.minVal)
				{
					param.minVal = value;
				}

				// Maxval
				if(value > param.maxVal)
				{
					param.maxVal = value;
				}

				// Welford's Online Algorithm

				// Avg(Mean) Val
				// Need old mean for variance calculation
				double oldMean = param.avgVal;
				param.avgVal = ((param.avgVal * param.count) + value) / (param.count + 1);

				// Variance
				param.variance = ((param.variance * param.count) + ((value - oldMean) * (value - param.avgVal))) / (param.count + 1);

				// Count
				param.count = param.count + 1;

			}

			template <>
			void updateValue(AggParameter<bool>& param, bool value)
			{
				int tmp;

				if(value)
				{
					tmp = 1;
				}
				else
				{
					tmp = 0;
				}

				// Minval
				if(tmp < param.minVal)
				{
					param.minVal = tmp;
				}

				// Maxval
				if(tmp > param.maxVal)
				{
					param.maxVal = tmp;
				}

				// Avg(Mean) Val
				// Welford's Online Algorithm
				double oldMean = param.avgVal;
				param.avgVal = ((param.avgVal * param.count) + tmp) / (param.count + 1);

				// Variance
				param.variance = ((param.variance * param.count) + ((tmp - oldMean) * (tmp - param.avgVal))) / (param.count + 1);

				// Count
				param.count = param.count + 1;
			}

			template <>
			void updateValue(AggParameter<std::string>& param, std::string value)
			{
				// Most of this is blank for now - we can't really have aggregate
				// trackers for a string parameter. This exists mostly to override
				// the template, since the log function should not call this function.

				// Count
				param.count = param.count + 1;
			}
		}
	}
}

// Explicit Instantiation
using namespace treetimer::parameters;

// Classes
template class AggParameter<int>;
template class AggParameter<double>;
template class AggParameter<bool>;
template class AggParameter<std::string>;

template void drivers::updateValue<int>(AggParameter<int>& param, int value);
template void drivers::updateValue<double>(AggParameter<double>& param, double value);
template void drivers::updateValue<>(AggParameter<bool>& param, bool value);
template void drivers::updateValue<>(AggParameter<std::string>& param, std::string value);
