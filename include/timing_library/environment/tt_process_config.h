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

#ifndef TT_PROCESS_CONFIG_INCLUDE_H
#define TT_PROCESS_CONFIG_INCLUDE_H

#include <string>

namespace treetimer
{
	namespace process
	{
		class TTProcessConfig
		{
			public:
				// Constructor/Deconstructors
				TTProcessConfig();
				~TTProcessConfig();

				// Variables
				// Rank of MPI Process in MPI_COMM_WORLD
				int mpiRank;

				// Identifier of Physical Socket on node
				int nodePhySocket;

				// Identifier of Physical Core on Socket
				int socketPhyCore;
				std::string hostName;
				std::string cpuModel;	//ToDo: Move CPU into it's own object
		};

		namespace drivers
		{
			void setMPIRank(TTProcessConfig& processConfig);
			void setNodePhySocket(TTProcessConfig& processConfig);
			void setSocketPhyCore(TTProcessConfig& processConfig);
			void setNodeName(TTProcessConfig& processConfig);
			void setCPUModel(TTProcessConfig& processConfig);
		}
	}
}

#endif
