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

#ifndef TT_CORE_INCLUDE_H
#define TT_CORE_INCLUDE_H

#include "tt_state.h"
#include "tt_code_block_type.h"

namespace treetimer
{
	namespace core
	{
		namespace drivers
		{
			void TreeTimerInit();
			void TreeTimerFinalize();

			void TreeTimerEnterBlock(std::string blockName, CodeBlockType blockType);
			void TreeTimerExitBlock(std::string blockName);

			void TreeTimerLogParameter(std::string paramName, int val);
			void TreeTimerLogParameter(std::string paramName, double val);
			void TreeTimerLogParameter(std::string paramName, bool val);
			void TreeTimerLogParameter(std::string paramName, std::string val);

			void TreeTimerFlushTraceData();
		}
	}
}

#endif
