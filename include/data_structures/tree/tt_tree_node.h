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

#ifndef TT_TREE_NODE_INCLUDE_H
#define TT_TREE_NODE_INCLUDE_H

#include "unordered_map"

namespace treetimer
{
	namespace data_structures
	{
		template <class K, class T>
		class TreeNode
		{
			public:
				TreeNode(K key, int uID);
				~TreeNode();

				K key;				// External identifier for a node lookup (may not be unique in the tree, but is unique within a set of child nodes)
				int uID;			// Internal counter of unique node id in tree
				T nodeData;
				TreeNode * parent;									// Parent Node
				std::unordered_map<K, TreeNode<K,T> *> childNodes;   	// Child Nodes

				void setParent(TreeNode<K,T> * parentNode);
				void addChild(K nodeID, TreeNode<K,T> * childNode);
		};
	}
}

#endif
