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

#ifndef TT_IO_JSON_INCLUDE_H
#define TT_IO_JSON_INCLUDE_H

#include "json.h"
#include "string.h"
#include "tt_config.h"
#include <fstream>
#include <memory>

namespace treetimer
{
	namespace io
	{
		namespace json
		{
			class TTJson
			{
				public:
					TTJson(std::string fileName);
					~TTJson();

					std::string fileName;
					Json::Value root;
			};

			namespace drivers
			{
				// ========= Global Data, Shared amongst all processes =========
				// Store Application Name, Version - Global
				void storeApplicationData(TTJson& data, treetimer::config::Config& config);

				// Store Integer Global Parameters
				void storeGlobalIntParameters(TTJson& data, treetimer::config::Config& config);

				// Store Double Global Parameters
				void storeGlobalDoubleParameters(TTJson& data, treetimer::config::Config& config);

				// Store Boolean Global Parameters
				void storeGlobalBoolParameters(TTJson& data, treetimer::config::Config& config);

				// Store String Global Parameters
				void storeGlobalStringParameters(TTJson& data, treetimer::config::Config& config);

				// Store Global Process Count
				void storeProcessCount(TTJson& data, treetimer::config::Config& config);

				// Store the runtime configuration of this library
				void storeLibraryConfig(TTJson& data, treetimer::config::Config& config);

				// ======= Process Specific Data ==========

				// Stores various pieces of data that are specific/semi-specific
				// to a process, namely:
				// Machine Name
				// Node Data (Name, Number of Sockets, etc)
				// Socket Position
				// CPU Position
				// CPU Properties (Name, Max Speed etc etc)
				void storeProcessMachineData(TTJson& data);

				//void storeMachineData(TTJson& data, Config& config);
				//void storeCPUData(TTJson& data, Config& config);


				// ========== I/O ==============
				void writeJSONData(TTJson& data, std::string path);
			}
		}
	}
}

#endif
