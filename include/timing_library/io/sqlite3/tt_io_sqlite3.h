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

#ifndef TT_IO_SQLITE3_INCLUDE_H
#define TT_IO_SQLITE3_INCLUDE_H

#include "tt_instrumentation_data.h"
#include "tt_tree_node.h"

#include "tt_sqlite3_db_access.h"
#include "tt_tree.h"
#include "tt_config.h"
#include "tt_instrumentation_data.h"

namespace tt_sql = treetimer::database::tt_sqlite3;
namespace tt_ds = treetimer::data_structures;
namespace tt_m = treetimer::measurement;

namespace treetimer
{
	namespace io
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				tt_sql::TTSQLite3* setupOutput(treetimer::config::Config& config);

				void writeRunConfigData(treetimer::config::Config& config, tt_sql::TTSQLite3* access);

				void writeAggData(treetimer::config::Config& config,
								  tt_ds::Tree<std::string, tt_m::InstrumentationData>& callTree,
								  tt_sql::TTSQLite3* access);

				void writeTraceData(treetimer::config::Config& config,
									tt_ds::Tree<std::string, tt_m::InstrumentationData>& callTree,
									tt_sql::TTSQLite3* access);

				void writeTreeNodeAggInstrumentationData(tt_sql::TTSQLite3& dataAccess,
											          tt_ds::TreeNode<std::string, tt_m::InstrumentationData>& node,
													  int runID, int processID, int parentID, int * callPathID, treetimer::config::Config& config);

				void writeTreeNodeTraceInstrumentationData(tt_sql::TTSQLite3& dataAccess,
											          tt_ds::TreeNode<std::string, tt_m::InstrumentationData>& node,
													  int runID, int processID, int parentID, int * callPathID, treetimer::config::Config& config);

				void callTreeTraversal(tt_sql::TTSQLite3& dataAccess,
									   tt_ds::TreeNode<std::string, tt_m::InstrumentationData>& node,
									   void (*func)(tt_sql::TTSQLite3& dataAccess,
											   	    tt_ds::TreeNode<std::string, tt_m::InstrumentationData>& node,
													int runID, int processID,
													int parentID, int * callPathID,
													treetimer::config::Config& config),
													int runID, int processID, int parentID, treetimer::config::Config& config);
			}
		}
	}
}

#endif
