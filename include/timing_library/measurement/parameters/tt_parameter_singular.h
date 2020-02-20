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

#ifndef TT_PARAMETER_SINGULAR_INCLUDE_H
#define TT_PARAMETER_SINGULAR_INCLUDE_H

#include <string>

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		class ParameterSingular
		{
			public:
				ParameterSingular(std::string name, T value);
				~ParameterSingular();

				std::string name;
				T value;
		};
	}
}

#endif
