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
template class treetimer::parameters::ParameterSingular<int>;
template class treetimer::parameters::ParameterSingular<double>;
template class treetimer::parameters::ParameterSingular<bool>;
template class treetimer::parameters::ParameterSingular<std::string>;
