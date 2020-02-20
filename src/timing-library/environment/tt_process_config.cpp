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

#include "tt_process_config.h"
#include "mpi.h"
#include <unistd.h>
#include <string>

// ToDo: Configure needs to handle detection of where HOST_NAME_MAX is defined in system include files
#include <limits.h> // HOST_NAME_MAX
//#ifndef HOST_NAME_MAX
//#define HOST_NAME_MAX 64
//#endif

namespace treetimer
{
	namespace process
	{
		TTProcessConfig::TTProcessConfig()
		{
			this->mpiRank = -1;
			this->nodePhySocket = -1;
			this->socketPhyCore = -1;
			this->hostName = "Unknown Host";
			this->cpuModel = "Unknown CPU";
		};

		TTProcessConfig::~TTProcessConfig()
		{

		}

		namespace drivers
		{
			void setMPIRank(TTProcessConfig& processConfig)
			{
				int rank;
				MPI_Comm_rank(MPI_COMM_WORLD, &processConfig.mpiRank);
			}

			void setNodePhySocket(TTProcessConfig& processConfig)
			{

			}

			void setSocketPhyCore(TTProcessConfig& processConfig)
			{

			}

			void setNodeName(TTProcessConfig& processConfig)
			{
				// ToDo: Suitable handling of truncated host names if this is insufficient
				// (Check return length and see if it is 1024?)
				char hostName[HOST_NAME_MAX + 1];
				size_t hostNameLen = (HOST_NAME_MAX + 1) * sizeof(char);

				gethostname(hostName, hostNameLen);
				processConfig.hostName = std::string(hostName);
			}

			void setCPUModel(TTProcessConfig& processConfig)
			{

			}
		}
	}
}
