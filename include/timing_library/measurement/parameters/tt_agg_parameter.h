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

#ifndef TT_AGG_PARAMETER_INCLUDE_H
#define TT_AGG_PARAMETER_INCLUDE_H

#include <string>

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		class AggParameter
		{
			public:
				AggParameter();
				~AggParameter();

				void updateValue(T value);

				T minVal;
				T maxVal;

				double avgVal;
				double variance;
				int count;
		};
	}
}


#endif
