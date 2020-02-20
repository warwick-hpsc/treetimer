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

#ifndef TT_CONFIG_INCLUDE_H
#define TT_CONFIG_INCLUDE_H

#include <string>
#include <unordered_map>
#include "tt_parameter_singular.h"

namespace treetimer
{
	namespace config
	{
		class Config
		{
			public:

				// Constructors/Deconstructors
				Config();
				~Config();

				// Variables
				std::string appName;		// Store the registered name of the application
				std::string appVersion;		// Store the registered version of the application
				std::string machineName;	// Store the registered name of the machine
				std::string outputFolder;	// Store the name of the output folder where results are stored

				int libVerMajor;
				int libVerMinor;

				//it_outformat_t format;	// Output Format

				bool eATimers;		// Global enable or disable aggregate timers
				bool eTTimers;		// Global enable or disable trace timers (will also activate aggregate timers when on)
				bool eAPAPI;		// Global enable or disable aggregate PAPI counters
				bool eTPAPI;		// Global enable or disable trace PAPI counters (will also activate aggregate PAPI Counters)
				bool eAParam;		// Global enable or disable aggregate PAPI counters
				bool eTParam;		// Global enable or disable trace PAPI counters (will also activate aggregate PAPI Counters)

				bool eMPIHooks;		// Global enable or disable instrumentation hooks for MPI (will still be affected by timing and parameter capture options)
									// N.B. Developers: due to the nature of the hooks, internal calls will still be made, but if disabled then no nodes will be
									// generated for MPI calls

				// Storage for global parameters that are expected to influence program configuration
				// Can only hold one value per parameter name

				// Number of MPI Processes
				int processCount;

				// We don't want to hook MPI calls while we are performing library related operations
				// Set this to false to disable MPI Hooks, True to enable Hooks if eMPIHooks is true
				bool inLibrary;

				// ToDo - The name/key is used in the map already, and the only other item of interest is the value
				// We could probably eliminate the ParameterSingularObject entirely and just map directly to the value
				std::unordered_map<std::string, treetimer::parameters::ParameterSingular<int> *> intGlobalParams;
				std::unordered_map<std::string, treetimer::parameters::ParameterSingular<double> *> doubleGlobalParams;
				std::unordered_map<std::string, treetimer::parameters::ParameterSingular<bool> *> boolGlobalParams;
				std::unordered_map<std::string, treetimer::parameters::ParameterSingular<std::string> *> stringGlobalParams;

				// SQL I/O Config Parameters
				std::string sqlIOFilename;
				int sqlIORunID;
				int sqlIOProcessID;

				// Tracke the status of various SQL Data write stages
				bool sqlIOSetup;		// Setup stage (Schemas, file creation)
				bool sqlIORunConfig;	// Run Configuration Data - global parameters cannot be changed after this.
				bool sqlIOAggData;		// Aggregate Data
				bool sqlIOTraceData; 	// Trace Data
		};

		namespace drivers
		{
			void setConfigFromEnv(Config& config);

			void setAppNameFromEnv(Config& config);
			void setAppVerFromEnv(Config& config);
			void setMachineNameFromEnv(Config& config);
			void setOutputFolderFromEnv(Config& config);
			void setAggTimerFromEnv(Config& config);
			void setTraceTimerFromEnv(Config& config);
			void setAggPAPIFromEnv(Config& config);
			void setTracePAPIFromEnv(Config& config);
			void setAggParamFromEnv(Config& config);
			void setTraceParamFromEnv(Config& config);
			void setMPIHooksFromEnv(Config& config);

			void setGlobalParamFromEnv(Config& config);

			// ToDo: Template these
			void setGlobalParam(Config& config, std::string name, int val);
			void setGlobalParam(Config& config, std::string name, double val);
			void setGlobalParam(Config& config, std::string name, bool val);
			void setGlobalParam(Config& config, std::string name, std::string val);

			void setConfig(Config& config, std::string appName, std::string appVersion, std::string machineName, std::string outputFolder,
						   int processCount,
						   bool eATimers, bool eTTimers, bool eAPAPI, bool eTPAPI, bool eAParam, bool eTParam, bool eMPIHooks, bool inLibrary);
		}
	}
}

#endif
