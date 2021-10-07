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

#include "tt_config.h"
#include "tt_version.h"
#include <cstdlib>
#include <string.h>
#include <vector>
#include <iostream>
#include "mpi.h"

namespace treetimer
{
	namespace config
	{

		Config::Config()
		{
			// By default, capture nothing but aggregate times.
			this->appName = "Unknown";
			this->appVersion = "Unknown";
			this->machineName = "Unknown";
			this->outputFolder = "./tt_results";
			this->libVerMajor = 0;
			this->libVerMinor = 1;
			this->processCount = 0;
			this->eATimers = false;
			this->eTTimers = false;
			this->eAPAPI = false;
			this->eTPAPI = false;
			this->eAParam = false;
			this->eTParam = false;
			this->eMPIHooks = false;
			this->eMPIHooksTParam = false;
			this->inLibrary = false;

			// ToDo: move this into a separate SQLConfig

			this->sqlIOProcessID = -1;

			this->sqlIOSetup = false;
			this->sqlIORunConfig = false;
			this->sqlIOAggData = false;
			this->sqlIOTraceData = false;
		}

		Config::~Config()
		{

		}

		void Config::setFromEnv() {
			char *envVar = nullptr;

			envVar = getenv("TT_CONFIG_OUTPUT_DIR");
			if(envVar != nullptr) this->outputFolder = std::string(envVar);

			// Update: TT: does not need to know these GLOBAL, STATIC values:
			/*
			*envVar = getenv("TT_CONFIG_APPNAME");
			if(envVar != nullptr) this->appName = std::string(envVar);
			envVar = getenv("TT_CONFIG_APPVER");
			if(envVar != nullptr) this->appVersion = std::string(envVar);
			envVar = getenv("TT_CONFIG_MACHINENAME");
			if(envVar != nullptr) this->machineName = std::string(envVar);
			*/

			envVar = getenv("TT_CONFIG_ATIMERS");
			this->eATimers = (envVar != nullptr) && (strcmp(envVar, "1") == 0);
			envVar = getenv("TT_CONFIG_TTIMERS");
			this->eTTimers = (envVar != nullptr) && (strcmp(envVar, "1") == 0);

			envVar = getenv("TT_CONFIG_APAPI");
			this->eAPAPI = (envVar != nullptr) && (strcmp(envVar, "1") == 0);
			envVar = getenv("TT_CONFIG_TPAPI");
			this->eAPAPI = (envVar != nullptr) && (strcmp(envVar, "1") == 0);

			envVar = getenv("TT_CONFIG_APARAM");
			this->eAParam = (envVar != nullptr) && (strcmp(envVar, "1") == 0);
			envVar = getenv("TT_CONFIG_TPARAM");
			this->eTParam = (envVar != nullptr) && (strcmp(envVar, "1") == 0);

			envVar = getenv("TT_CONFIG_AGG_STEP_NODE");
			if(envVar != nullptr) this->aggregationStepperNodeName = std::string(envVar);
			envVar = getenv("TT_CONFIG_AGG_STEP_INTERVAL");
			if(envVar != nullptr) this->aggregationStepInterval = std::stoi(std::string(envVar));
			envVar = getenv("TT_CONFIG_TRACE_CONDUCTOR_NODE");
			if(envVar != nullptr) this->traceConductorNodeName = std::string(envVar);
			envVar = getenv("TT_CONFIG_TRACE_INTERVAL");
			if(envVar != nullptr) this->traceConductorInterval = std::stoi(std::string(envVar));

			envVar = getenv("TT_CONFIG_MPI_HOOKS");
			this->eMPIHooks = (envVar != nullptr) && (strcmp(envVar, "1") == 0);
			envVar = getenv("TT_CONFIG_MPI_HOOKS_TPARAM");
			this->eMPIHooksTParam = (envVar != nullptr) && (strcmp(envVar, "1") == 0);

			// setGlobalParamFromEnv(this);

			this->print();
		}

		void Config::print() {
			printf("--------\n");
			printf("CONFIG:\n");
			printf("--------\n");
			printf("appName = %s\n", this->appName.c_str());
			printf("appVersion = %s\n", this->appVersion.c_str());
			printf("machineName = %s\n", this->machineName.c_str());
			printf("outputFolder = %s\n", this->outputFolder.c_str());

			printf("libVerMajor = %d\n", this->libVerMajor);
			printf("libVerMinor = %d\n", this->libVerMinor);

			printf("eATimers = %d\n", this->eATimers);
			printf("eTTimers = %d\n", this->eTTimers);
			printf("eAPAPI = %d\n", this->eAPAPI);
			printf("eTPAPI = %d\n", this->eTPAPI);
			printf("eAParam = %d\n", this->eAParam);
			printf("eTParam = %d\n", this->eTParam);
			printf("eMPIHooks = %d\n", this->eMPIHooks);
			printf("eMPIHooksTParam = %d\n", this->eMPIHooksTParam);

			printf("aggregationStepperNodeName = %s\n", this->aggregationStepperNodeName.c_str());
			printf("aggregationStepInterval = %d\n", this->aggregationStepInterval);
			printf("traceConductorNodeName = %s\n", this->traceConductorNodeName.c_str());
			printf("traceConductorInterval = %d\n", this->traceConductorInterval);
			printf("--------\n");
		}

		namespace drivers
		{

/*
			void setFormatFromEnv(Config& config)
			{
				// ====== Output Format ========
				envVar = getenv("TT_CONFIG_OUTFORMAT");
				// Copy if not null and below string length limit
				if(envVar != nullptr) {
					if(strcmp(envVar, IT_OUT_SQLITE3_STR) == 0) {
						config->format = IT_OUT_SQLITE3;
					}
					else if(strcmp(envVar, IT_OUT_CSV_STR) == 0) {
						config->format = IT_OUT_CSV;
					}
					else if(strcmp(envVar, IT_OUT_JSON_STR) == 0) {
						config->format = IT_OUT_JSON;
					}
				}
			}
*/

			void strSplit(std::string input, std::vector<std::string>& output, char delimiter)
			{
				std::vector<int> delimPos;

				for(size_t i = 0; i < input.length(); i++) {
					if(input[i] == delimiter) {
						delimPos.push_back(i);
					}
				}

				int ptr = 0;

				// Push one past the end of the string onto the end
				// i.e. count as as the last delimiter since we are
				// adding strings before delimiters.
				delimPos.push_back(input.length());

				if(delimPos.size() == 0) {
					output.push_back(input);
					return;
				} else {
					for(size_t i = 0; i < delimPos.size(); i++) {
						// Retrieve the next delimiter position
						int pos = delimPos[i];

						// Compute the length
						int len = pos - ptr;

						// If length is greater than 0, add the substr,
						// else add an empty string (e.g. delimiter is first
						// or last char of string, or two delimiters are next
						// to each other)

						if(len > 0) {
							output.push_back(input.substr(ptr,len));
						} else {
							output.push_back("");
						}

						ptr += len + 1;
					}
				}

			}

			// Update: What is the purpose of "global" parameters? Codebase only sets them, never uses them.
/*
			//ToDo: Do we want to store the variables as separate types?
			// We have to convert from a string anyway, so we could just store all of them as strings
			void setGlobalParamFromEnv(Config& config)
			{
				// Read in global parameters from environment variables.
				// This is handled via 4 separate environment variables, with each containing a comma separated list
				// of name=value pairs.

				//ToDo: Error checking that the strings are valid - we assume they are for now...

				char *envVar;

				// === Integers ===
				envVar = getenv("TT_CONFIG_GLOBAL_PARAM_INT");

				if(envVar != NULL)
				{
					std::vector<std::string> commaSplit;
					strSplit(envVar, commaSplit, ',');

					for(size_t i = 0; i < commaSplit.size(); i++)
					{
						// Now split each substring by = to get name, value pair separately
						std::vector<std::string> substrSplit;
						strSplit(commaSplit[i], substrSplit, '=');

						for(size_t j = 0; j < substrSplit.size(); j+=2)
						{
							setGlobalParam(config, substrSplit[j], stoi(substrSplit[j+1]));
						}
					}
				}

				// === Doubles ===
				envVar = getenv("TT_CONFIG_GLOBAL_PARAM_FLOAT");

				if(envVar != NULL)
				{
					std::vector<std::string> commaSplit;
					strSplit(envVar, commaSplit, ',');

					for(size_t i = 0; i < commaSplit.size(); i++)
					{
						// Now split each substring by = to get name, value pair separately
						std::vector<std::string> substrSplit;
						strSplit(commaSplit[i], substrSplit, '=');

						for(size_t j = 0; j < substrSplit.size(); j+=2)
						{
							setGlobalParam(config, substrSplit[j], stod(substrSplit[j+1]));
						}
					}
				}

				// === Bool ===
				envVar = getenv("TT_CONFIG_GLOBAL_PARAM_BOOL");

				if(envVar != NULL)
				{
					std::vector<std::string> commaSplit;
					strSplit(envVar, commaSplit, ',');

					for(size_t i = 0; i < commaSplit.size(); i++)
					{
						// Now split each substring by = to get name, value pair separately
						std::vector<std::string> substrSplit;
						strSplit(commaSplit[i], substrSplit, '=');

						for(size_t j = 0; j < substrSplit.size(); j+=2)
						{
							setGlobalParam(config, substrSplit[j], (bool) (stoi(substrSplit[j+1])));
						}
					}
				}

				// === String ===
				envVar = getenv("TT_CONFIG_GLOBAL_PARAM_STRING");

				if(envVar != NULL)
				{
					std::vector<std::string> commaSplit;
					strSplit(envVar, commaSplit, ',');

					for(size_t i = 0; i < commaSplit.size(); i++)
					{
						// Now split each substring by = to get name, value pair separately
						std::vector<std::string> substrSplit;
						strSplit(commaSplit[i], substrSplit, '=');

						for(size_t j = 0; j < substrSplit.size(); j+=2)
						{
							setGlobalParam(config, substrSplit[j], substrSplit[j+1]);
						}
					}
				}
			}

			void setGlobalParam(Config& config, std::string name, int val)
			{
				// Search for existing parameter of type int with the same name
				// If found, overwrite, else create a new parameter
				typename std::unordered_map<std::string, treetimer::parameters::ParameterSingular<int> *>::const_iterator search = config.intGlobalParams.find(name);

				if(search == config.intGlobalParams.end())
				{
					treetimer::parameters::ParameterSingular<int> *store = new treetimer::parameters::ParameterSingular<int>(name, val);
					std::pair<std::string, treetimer::parameters::ParameterSingular<int> *> insertPair(name, store);
					config.intGlobalParams.insert(insertPair);
				}
				else
				{
					search->second->value = val;
				}
			}

			void setGlobalParam(Config& config, std::string name, double val)
			{
				// Search for existing parameter of type int with the same name
				// If found, overwrite, else create a new parameter
				typename std::unordered_map<std::string, treetimer::parameters::ParameterSingular<double> *>::const_iterator search = config.doubleGlobalParams.find(name);

				if(search == config.doubleGlobalParams.end())
				{
					treetimer::parameters::ParameterSingular<double> *store = new treetimer::parameters::ParameterSingular<double>(name, val);
					std::pair<std::string, treetimer::parameters::ParameterSingular<double> *> insertPair(name, store);
					config.doubleGlobalParams.insert(insertPair);
				}
				else
				{
					search->second->value = val;
				}
			}

			void setGlobalParam(Config& config, std::string name, bool val)
			{
				// Search for existing parameter of type int with the same name
				// If found, overwrite, else create a new parameter
				typename std::unordered_map<std::string, treetimer::parameters::ParameterSingular<bool> *>::const_iterator search = config.boolGlobalParams.find(name);

				if(search == config.boolGlobalParams.end())
				{
					treetimer::parameters::ParameterSingular<bool> *store = new treetimer::parameters::ParameterSingular<bool>(name, val);
					std::pair<std::string, treetimer::parameters::ParameterSingular<bool> *> insertPair(name, store);
					config.boolGlobalParams.insert(insertPair);
				}
				else
				{
					search->second->value = val;
				}
			}

			void setGlobalParam(Config& config, std::string name, std::string val)
			{
				// Search for existing parameter of type int with the same name
				// If found, overwrite, else create a new parameter
				typename std::unordered_map<std::string, treetimer::parameters::ParameterSingular<std::string> *>::const_iterator search = config.stringGlobalParams.find(name);

				if(search == config.stringGlobalParams.end())
				{
					treetimer::parameters::ParameterSingular<std::string> *store = new treetimer::parameters::ParameterSingular<std::string>(name, val);
					std::pair<std::string, treetimer::parameters::ParameterSingular<std::string> *> insertPair(name, store);
					config.stringGlobalParams.insert(insertPair);
				}
				else
				{
					search->second->value = val;
				}
			}
*/
		}
	}
}
