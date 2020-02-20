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

#include "tt_parameter_singular.h"

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		ParameterSingular<T>::ParameterSingular(std::string name, T value)
		{
			this->name = name;
			this->value = value;
		}

		template <class T>
		ParameterSingular<T>::~ParameterSingular()
		{

		}
	}
}

// Explicit Instantiation
using namespace treetimer::parameters;

// Classes
template class ParameterSingular<int>;
template class ParameterSingular<double>;
template class ParameterSingular<bool>;
template class ParameterSingular<std::string>;
