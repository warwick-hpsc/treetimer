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

namespace treetimer
{
	namespace io
	{
		namespace tt_sqlite3
		{
			namespace drivers
			{
				void setupOutput(treetimer::config::Config& config);

				void writeRunConfigData(treetimer::config::Config& config);

				void writeAggData(treetimer::config::Config& config,
								  treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree);

				void writeTraceData(treetimer::config::Config& config,
									treetimer::data_structures::Tree<std::string, treetimer::measurement::InstrumentationData>& callTree);

				void writeTreeNodeAggInstrumentationData(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
											          treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													  int runID, int processID, int parentID, int * callPathID, treetimer::config::Config& config);

				void writeTreeNodeTraceInstrumentationData(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
											          treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													  int runID, int processID, int parentID, int * callPathID, treetimer::config::Config& config);

				void callTreeTraversal(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
									   treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
									   void (*func)(treetimer::database::tt_sqlite3::TTSQLite3& dataAccess,
											   	    treetimer::data_structures::TreeNode<std::string, treetimer::measurement::InstrumentationData>& node,
													int runID, int processID,
													int parentID, int * callPathID,
													treetimer::config::Config& config),
													int runID, int processID, int parentID, treetimer::config::Config& config);
			}
		}
	}
}

#endif
