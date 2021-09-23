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

		template <class T>
		void AggParameter<T>::updateValue(T value)
		{
			if(value < this->minVal) this->minVal = value;
			if(value > this->maxVal) this->maxVal = value;

			// Welford's Online Algorithm

			// Avg(Mean) Val
			// Need old mean for variance calculation
			double oldMean = this->avgVal;
			this->avgVal = ((this->avgVal * this->count) + value) / (this->count + 1);

			// Variance
			this->variance = ((this->variance * this->count) + ((value - oldMean) * (value - this->avgVal))) / (this->count + 1);

			// Count
			this->count++;
		}

		template <>
		void AggParameter<bool>::updateValue(bool value)
		{
			int tmp = value ? 1 : 0;

			if(tmp < this->minVal) this->minVal = tmp;
			if(tmp > this->maxVal) this->maxVal = tmp;

			// Avg(Mean) Val
			// Welford's Online Algorithm
			double oldMean = this->avgVal;
			this->avgVal = ((this->avgVal * this->count) + tmp) / (this->count + 1);

			// Variance
			this->variance = ((this->variance * this->count) + ((tmp - oldMean) * (tmp - this->avgVal))) / (this->count + 1);

			// Count
			this->count++;
		}

		template <>
		void AggParameter<std::string>::updateValue(std::string value)
		{
			// Most of this is blank for now - we can't really have aggregate
			// trackers for a string parameter. This exists mostly to override
			// the template, since the log function should not call this function.

			// Count
			this->count++;
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

