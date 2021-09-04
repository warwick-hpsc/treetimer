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

#include "tt_io_json.h"

namespace treetimer
{
	namespace io
	{
		namespace json
		{
			TTJson::TTJson(std::string fileName)
			{
				this->fileName = fileName;
			}

			TTJson::~TTJson()
			{

			}

			namespace drivers
			{
				void storeApplicationData(TTJson& data, treetimer::config::Config& config)
				{
					data.root["ApplicationData"]["ApplicationName"] = config.appName;
					data.root["ApplicationData"]["ApplicationVersion"] = config.appVersion;
				}

				// Update: What is the purpose of "global" parameters? Codebase only sets them, never uses them.
				// void storeGlobalIntParameters(TTJson& data, treetimer::config::Config& config)
				// {
				// 	std::unordered_map<std::string, treetimer::parameters::ParameterSingular<int> *>::iterator it;

				// 	for(it = config.intGlobalParams.begin(); it != config.intGlobalParams.end(); it++)
				// 	{
				// 		data.root["GlobalParameters"]["Integers"][it->first] = it->second->value;
				// 	}
				// }

				// void storeGlobalDoubleParameters(TTJson& data, treetimer::config::Config& config)
				// {
				// 	std::unordered_map<std::string, treetimer::parameters::ParameterSingular<double> *>::iterator it;

				// 	for(it = config.doubleGlobalParams.begin(); it != config.doubleGlobalParams.end(); it++)
				// 	{
				// 		data.root["GlobalParameters"]["Doubles"][it->first] = it->second->value;
				// 	}
				// }

				// void storeGlobalBoolParameters(TTJson& data, treetimer::config::Config& config)
				// {
				// 	std::unordered_map<std::string, treetimer::parameters::ParameterSingular<bool> *>::iterator it;

				// 	for(it = config.boolGlobalParams.begin(); it != config.boolGlobalParams.end(); it++)
				// 	{
				// 		data.root["GlobalParameters"]["Bools"][it->first] = it->second->value;
				// 	}
				// }

				// void storeGlobalStringParameters(TTJson& data, treetimer::config::Config& config)
				// {
				// 	std::unordered_map<std::string, treetimer::parameters::ParameterSingular<std::string> *>::iterator it;

				// 	for(it = config.stringGlobalParams.begin(); it != config.stringGlobalParams.end(); it++)
				// 	{
				// 		data.root["GlobalParameters"]["Strings"][it->first] = it->second->value;
				// 	}
				// }

				void storeProcessCount(TTJson& data, treetimer::config::Config& config)
				{

				}

				void storeLibraryConfig(TTJson& data, treetimer::config::Config& config)
				{
					data.root["LibraryConfig"]["VersionMajor"] = config.libVerMajor;
					data.root["LibraryConfig"]["VersionMinor"] = config.libVerMinor;
					data.root["LibraryConfig"]["AggTimers"] = config.eATimers;
					data.root["LibraryConfig"]["TraceTimers"] = config.eTTimers;
					data.root["LibraryConfig"]["AggParam"] = config.eAParam;
					data.root["LibraryConfig"]["TraceParam"] = config.eTParam;
					data.root["LibraryConfig"]["AggPAPI"] = config.eAPAPI;
					data.root["LibraryConfig"]["TracePAPI"] = config.eTPAPI;
					data.root["LibraryConfig"]["MpiHooks"] = config.eMPIHooks;
				}

				void storeProcessMachineData(TTJson& data)
				{

				}

				void writeJSONData(TTJson& data, std::string path)
				{
					// Setup a Writer Builder
					Json::StreamWriterBuilder builder;
					builder["indentation"] = "\t";

					// Build the writer and set the target location
					std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
					std::ofstream outputStream(path + "/" + data.fileName);

					// Write the data out
					writer -> write(data.root, &outputStream);
				}
			}
		}
	}
}
