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
			this->blockTimer = new treetimer::timers::Timer();

			this->blockType = TT_NODE_TYPE_UNSPECIFIED;
		}

		InstrumentationData::~InstrumentationData()
		{
			// Cleanup timer for this instrumentation
			delete(this->blockTimer);

			// Cleanup any parameter storage used for this instrumentation

			// Cleanup any PAPI storage used for this instrumentation
		}

		namespace drivers
		{
			void startInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callEntryID)
			{
				node.blockTimer->setNumAggWindows(node.targetNumAggWindows);

				node.blockTimer->startTimer(eATimer, eTTimer, callEntryID);

				node.intParametersCached.clear();
				node.longParametersCached.clear();
				node.doubleParametersCached.clear();
				node.boolParametersCached.clear();
				node.stringParametersCached.clear();
			}

			void stopInstrumentation(InstrumentationData& node, bool eATimer, bool eTTimer, long callExitID)
			{
				node.blockTimer->stopTimer(eATimer, eTTimer, callExitID);
			}

			void logParameterValue(InstrumentationData& node, std::string name, int val)
			{
				// Cache parameter value, but don't commit until current block is exited
				node.intParametersCached.push_back(std::make_pair(name, val));
			}

			void logParameterValue(InstrumentationData& node, std::string name, long val)
			{
				// Cache parameter value, but don't commit until current block is exited
				node.longParametersCached.push_back(std::make_pair(name, val));
			}

			void logParameterValue(InstrumentationData& node, std::string name, double val)
			{
				// Cache parameter value, but don't commit until current block is exited
				node.doubleParametersCached.push_back(std::make_pair(name, val));
			}

			void logParameterValue(InstrumentationData& node, std::string name, bool val)
			{
				// Cache parameter value, but don't commit until current block is exited
				node.boolParametersCached.push_back(std::make_pair(name, val));
			}

			void logParameterValue(InstrumentationData& node, std::string name, std::string val)
			{
				// Cache parameter value, but don't commit until current block is exited
				node.stringParametersCached.push_back(std::make_pair(name, val));
			}

			void commitParameters(InstrumentationData& node, bool eAParam, bool eTParam, long nodeEntryID, long nodeExitID)
			{
				// Integer parameters
				for (size_t i=0; i<node.intParametersCached.size(); i++) {
					std::string name = std::get<0>(node.intParametersCached[i]);
					int val = std::get<1>(node.intParametersCached[i]);

					// Find the appropriate parameter object from the map (if it exists) and update it, else create a new object and store
					typename std::unordered_map<std::string, treetimer::parameters::Parameter<int> *>::const_iterator search = node.intParameters.find(name);

					if(search == node.intParameters.end()) {
						// Not found, create a new parameter
						treetimer::parameters::Parameter<int> *store = new treetimer::parameters::Parameter<int>(name);
						std::pair<std::string, treetimer::parameters::Parameter<int> *> insertPair(name, store);

						node.intParameters.insert(insertPair);
						store->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					} else {
						// Found, update existing
						search->second->setNumAggWindows(node.targetNumAggWindows);
						search->second->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					}
				}
				node.intParametersCached.clear();

				// Long parameters
				for (size_t i=0; i<node.longParametersCached.size(); i++) {
					std::string name = std::get<0>(node.longParametersCached[i]);
					long val = std::get<1>(node.longParametersCached[i]);

					typename std::unordered_map<std::string, treetimer::parameters::Parameter<long> *>::const_iterator search = node.longParameters.find(name);
					if(search == node.longParameters.end()) {
						treetimer::parameters::Parameter<long> *store = new treetimer::parameters::Parameter<long>(name);
						std::pair<std::string, treetimer::parameters::Parameter<long> *> insertPair(name, store);
						node.longParameters.insert(insertPair);
						store->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					} else {
						search->second->setNumAggWindows(node.targetNumAggWindows);
						search->second->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					}
				}
				node.longParametersCached.clear();

				// Double parameters
				for (size_t i=0; i<node.doubleParametersCached.size(); i++) {
					std::string name = std::get<0>(node.doubleParametersCached[i]);
					double val = std::get<1>(node.doubleParametersCached[i]);

					typename std::unordered_map<std::string, treetimer::parameters::Parameter<double> *>::const_iterator search = node.doubleParameters.find(name);
					if(search == node.doubleParameters.end()) {
						treetimer::parameters::Parameter<double> *store = new treetimer::parameters::Parameter<double>(name);
						std::pair<std::string, treetimer::parameters::Parameter<double> *> insertPair(name, store);
						node.doubleParameters.insert(insertPair);
						store->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					} else {
						search->second->setNumAggWindows(node.targetNumAggWindows);
						search->second->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					}
				}
				node.doubleParametersCached.clear();

				// Bool parameters
				for (size_t i=0; i<node.boolParametersCached.size(); i++) {
					std::string name = std::get<0>(node.boolParametersCached[i]);
					bool val = std::get<1>(node.boolParametersCached[i]);

					typename std::unordered_map<std::string, treetimer::parameters::Parameter<bool> *>::const_iterator search = node.boolParameters.find(name);
					if(search == node.boolParameters.end()) {
						treetimer::parameters::Parameter<bool> *store = new treetimer::parameters::Parameter<bool>(name);
						std::pair<std::string, treetimer::parameters::Parameter<bool> *> insertPair(name, store);
						node.boolParameters.insert(insertPair);
						store->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					} else {
						search->second->setNumAggWindows(node.targetNumAggWindows);
						search->second->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					}
				}
				node.boolParametersCached.clear();

				// String parameters
				for (size_t i=0; i<node.stringParametersCached.size(); i++) {
					std::string name = std::get<0>(node.stringParametersCached[i]);
					std::string val = std::get<1>(node.stringParametersCached[i]);

					typename std::unordered_map<std::string, treetimer::parameters::Parameter<std::string> *>::const_iterator search = node.stringParameters.find(name);
					if(search == node.stringParameters.end()) {
						treetimer::parameters::Parameter<std::string> *store = new treetimer::parameters::Parameter<std::string>(name);
						std::pair<std::string, treetimer::parameters::Parameter<std::string> *> insertPair(name, store);
						node.stringParameters.insert(insertPair);
						store->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					} else {
						search->second->setNumAggWindows(node.targetNumAggWindows);
						search->second->addValue(val, eAParam, eTParam, nodeEntryID, nodeExitID);
					}
				}
				node.stringParametersCached.clear();
			}
		}
	}
}
