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

#ifndef TT_PARAMETER_INCLUDE_H
#define TT_PARAMETER_INCLUDE_H

#include <string>

namespace treetimer
{
	namespace parameters
	{
		template <class T>
		class AggParameter
		{
			public:
				// Constructors/Deconstructors
				AggParameter();
				~AggParameter();

				std::string name;
				T val;

				double minVal;
				double avgVal;
				double maxVal;
				double totalVal;
				int count;
		};

		namespace drivers
		{
			template <class T>
			void resetParameter(Parameter<T>& param);

			template <class T>
			void setParameter(Parameter<T>& param, std::String name, T val);
		}
	}
}

#endif
