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

#ifndef TT_IO_DATA_INCLUDE_H
#define TT_IO_DATA_INCLUDE_H

#include "tt_config.h"
#include "tt_state.h"
#include "tt_instrumentation_data.h"
#include "tt_io_sqlite3.h"
#include "tt_tree.h"
#include <string>

namespace tt_sql = treetimer::database::tt_sqlite3;

namespace treetimer
{
	namespace io
	{
		namespace drivers
		{
			// Write all gathered data to file(s)
			void writeData(treetimer::config::State& state);

			void flushTraceData(treetimer::config::State& state);

			void resetTraceDataTree(treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node);

			// Create the results directory
			void createResultsDirectory(treetimer::config::Config& config);

			tt_sql::TTSQLite3* setupOutput(treetimer::config::Config& config);

			void writeRunConfigData(treetimer::config::Config& config, tt_sql::TTSQLite3* access);

			void writeAggData(treetimer::config::Config& config,
							  treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
							  tt_sql::TTSQLite3* access);

			void writeTraceData(treetimer::config::Config& config,
								treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree,
								tt_sql::TTSQLite3* access);
		}
	}
}


#endif
