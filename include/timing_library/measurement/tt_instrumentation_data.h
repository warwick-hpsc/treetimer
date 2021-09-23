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
#include <vector>

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
				treetimer::timers::Timer *blockTimer;			// Timers

				// Parameters
				// Multiple parameters of the same type are permitted per node
				// Accessible by the parameter name
				std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>			intParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<long> *>			longParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<double> *> 		doubleParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>   		boolParameters;
				std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>   	stringParameters;

				// Temporary parameter cache, until current block is exited (so that nodeExitID can be recorded)
				std::vector<std::pair<std::string, int>>          intParametersCached;
				std::vector<std::pair<std::string, long>>         longParametersCached;
				std::vector<std::pair<std::string, double>>       doubleParametersCached;
				std::vector<std::pair<std::string, bool>>         boolParametersCached;
				std::vector<std::pair<std::string, std::string>>  stringParametersCached;

				int targetNumAggWindows = 1;

				// Trace collection management
				bool instrumentThisVisit = true;

				int currentNodeEntryID = 0;

				// PAPI
		};

		namespace drivers
		{
			void startInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callEntryID);
			void stopInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callExitID);

			void logParameterValue(InstrumentationData& node, std::string name, int val);
			void logParameterValue(InstrumentationData& node, std::string name, long val);
			void logParameterValue(InstrumentationData& node, std::string name, double val);
			void logParameterValue(InstrumentationData& node, std::string name, bool val);
			void logParameterValue(InstrumentationData& node, std::string name, std::string val);
			void commitParameters(InstrumentationData& node, bool eAParam, bool eTParam, long nodeEntryID, long nodeExitID);

		}
	}
}

#endif
