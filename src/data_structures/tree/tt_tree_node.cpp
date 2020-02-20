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

#include "tt_tree_node.h"
#include "tt_instrumentation_data.h"

namespace treetimer
{
	namespace data_structures
	{
		template <class K, class T>
		TreeNode<K,T>::TreeNode(K key, int uID)
		{
			this->key = key;
			this->uID = uID;
			this->parent = nullptr;
		}

		template <class K, class T>
		TreeNode<K,T>::~TreeNode()
		{

		}

		template <class K, class T>
		void TreeNode<K,T>::setParent(TreeNode<K,T> * parentNode)
		{
			this->parent = parentNode;
		}

		template <class K, class T>
		void TreeNode<K,T>::addChild(K nodeID, TreeNode<K,T> * childNode)
		{
			std::pair<K, TreeNode<K,T> *> insertPair(nodeID, childNode);
			this->childNodes.insert(insertPair);
		}
	}
}

// Explicit Instantiation
using namespace treetimer::data_structures;

// Classes
template class TreeNode<std::string, treetimer::measurement::InstrumentationData>;
