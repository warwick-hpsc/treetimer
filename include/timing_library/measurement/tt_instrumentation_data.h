/*
 * @file
 * @author James Davis
 * @version 0.1
 *
 * @section LICENSE
 * Based on PMTM/Ichniea - ToDo: Check License for these
 *
 * @section DESCRIPTION
 *
 */

#ifndef TT_INSTRUMENTATION_DATA_INCLUDE_H
#define TT_INSTRUMENTATION_DATA_INCLUDE_H

#include "tt_timer.h"
#include "tt_code_block_type.h"
#include "tt_parameter.h"
#include <string>
#include <unordered_map>

namespace treetimer
{
	namespace measurement
	{
		class InstrumentationData
		{
			public:
				InstrumentationData();
				~InstrumentationData();

				// Code Block Type
				CodeBlockType blockType;

				// ========= Instrumentation Data =================
				treetimer::timers::Timer * blockTimer;			// Timers

				// Parameters
				// Multiple parameters of the same type are permitted per node
				// Accessible by the parameter name
				std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>			intParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<double> *> 		doubleParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>   		boolParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>   	stringParameters;

				// PAPI
		};

		namespace drivers
		{
			void startInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callEntryID);
			void stopInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callExitID);

			void logParameterValue(InstrumentationData& node, std::string name, int val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit);
			void logParameterValue(InstrumentationData& node, std::string name, double val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit);
			void logParameterValue(InstrumentationData& node, std::string name, bool val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit);
			void logParameterValue(InstrumentationData& node, std::string name, std::string val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit);
		}
	}
}

#endif
