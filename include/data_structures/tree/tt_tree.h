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

#ifndef TT_TREE_INCLUDE_H
#define TT_TREE_INCLUDE_H

#include "tt_tree_node.h"

namespace treetimer
{
	namespace data_structures
	{
		template <class K, class T>
		class Tree
		{
			public:
				Tree();
				~Tree();
				TreeNode<K,T> * root;	// Root node of the tree
				TreeNode<K,T> * pos;		// Pointer to current position within the tree

				int uniqueNodeCount;	// Track the number of nodes in the tree
				long nodeEntryCount;	// Track the number of times we have entered a node globally (used for trace information)
				long nodeExitCount;		// Track the number of times we have exited a node globally (used for trace information)

				void moveToChild(K key);
				void moveToParent();
		};
	}
}

#endif
