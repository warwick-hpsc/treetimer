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

#include "tt_instrumentation_data.h"
#include <iostream>

namespace treetimer
{
	namespace measurement
	{
		InstrumentationData::InstrumentationData()
		{
			this->blockTimes = new treetimer::timers::Timer();
		}

		InstrumentationData::~InstrumentationData()
		{
			// Cleanup timer for this instrumentation
			delete(this->blockTimes);

			// Cleanup any parameter storage used for this instrumentation

			// Cleanup any PAPI storage used for this instrumentation
		}

		namespace drivers
		{
			void startInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callEntryID)
			{
				treetimer::timers::drivers::startTimer(*(node.blockTimes), eATimer, eTTimer, callEntryID);
			}

			void stopInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callExitID)
			{
				treetimer::timers::drivers::stopTimer(*(node.blockTimes), eATimer, eTTimer, callExitID);
			}

			void logParameterValue(InstrumentationData& node, std::string name, int val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit)
			{
				// Find the appropriate parameter object from the map (if it exists) and update it, else create a new object and store
				typename std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>::const_iterator search = node.intParameters.find(name);

				if(search == node.intParameters.end())
				{
					// Not found, create a new parameter
					treetimer::parameters::Parameter<int> * store = new treetimer::parameters::Parameter<int>(name);
					std::pair<std::string, treetimer::parameters::Parameter<int> *> insertPair(name, store);

					node.intParameters.insert(insertPair);
					treetimer::parameters::drivers::addValue(*store, val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
				else
				{
					// Found, update existing
					treetimer::parameters::drivers::addValue(*(search->second), val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
			}

			void logParameterValue(InstrumentationData& node, std::string name, double val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit)
			{
				// Find the appropriate parameter object from the map (if it exists) and update it, else create a new object and store
				typename std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::const_iterator search = node.doubleParameters.find(name);

				if(search == node.doubleParameters.end())
				{
					// Not found, create a new parameter
					treetimer::parameters::Parameter<double> * store = new treetimer::parameters::Parameter<double>(name);
					std::pair<std::string, treetimer::parameters::Parameter<double> *> insertPair(name, store);

					node.doubleParameters.insert(insertPair);
					treetimer::parameters::drivers::addValue(*store, val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
				else
				{
					// Found, update existing
					treetimer::parameters::drivers::addValue(*(search->second), val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
			}

			void logParameterValue(InstrumentationData& node, std::string name, bool val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit)
			{
				// Find the appropriate parameter object from the map (if it exists) and update it, else create a new object and store
				typename std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::const_iterator search = node.boolParameters.find(name);

				if(search == node.boolParameters.end())
				{
					// Not found, create a new parameter
					treetimer::parameters::Parameter<bool> * store = new treetimer::parameters::Parameter<bool>(name);
					std::pair<std::string, treetimer::parameters::Parameter<bool> *> insertPair(name, store);

					node.boolParameters.insert(insertPair);
					treetimer::parameters::drivers::addValue(*store, val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
				else
				{
					// Found, update existing
					treetimer::parameters::drivers::addValue(*(search->second), val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
			}

			void logParameterValue(InstrumentationData& node, std::string name, std::string val, bool eAParam, bool eTParam, long nodeCallEntry, long nodeCallExit)
			{
				// Find the appropriate parameter object from the map (if it exists) and update it, else create a new object and store
				typename std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>::const_iterator search = node.stringParameters.find(name);

				if(search == node.stringParameters.end())
				{
					// Not found, create a new parameter
					treetimer::parameters::Parameter<std::string> * store = new treetimer::parameters::Parameter<std::string>(name);
					std::pair<std::string, treetimer::parameters::Parameter<std::string> *> insertPair(name, store);

					node.stringParameters.insert(insertPair);
					treetimer::parameters::drivers::addValue(*store, val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
				else
				{
					// Found, update existing
					treetimer::parameters::drivers::addValue(*(search->second), val, eAParam, eTParam, nodeCallEntry, nodeCallExit);
				}
			}
		}
	}
}
