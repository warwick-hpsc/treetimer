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

#include "tt_tree.h"
#include "tt_instrumentation_data.h"
#include <unordered_map>
#include "tt_tree_node.h"

namespace treetimer
{
	namespace data_structures
	{
		template<class K, class T>
		Tree<K,T>::Tree()
		{
			this->root = nullptr;
			this->pos = nullptr;
			this->uniqueNodeCount = 0;
			this->nodeEntryCount = 0;
			this->nodeExitCount = 0;
		}

		template<class K, class T>
		Tree<K,T>::~Tree()
		{
			//ToDo: Traverse Tree and ensure that all nodes are destroyed
		}

		template<class K, class T>
		void Tree<K,T>::moveToChild(K key)
		{
			// Move position to the child node with Key k.
			// If it does not exist, create it, add to the current position and then move to it

			// Update node entry counter
			this->nodeEntryCount = this->nodeEntryCount + 1;


			// If root does not exist
			if(this->root == nullptr)
			{
				this->uniqueNodeCount = this->uniqueNodeCount + 1;
				this->root = new TreeNode<K,T>(key, this->uniqueNodeCount);
				this->pos = this->root;
				return;
			}

			// If root does already exist
			typename std::unordered_map<K, TreeNode<K,T> *>::const_iterator search = this->pos->childNodes.find(key);

			if(search == this->pos->childNodes.end())
			{
				// Node does not exist, create a new one
				this->uniqueNodeCount = this->uniqueNodeCount + 1;
				TreeNode<K,T> * node = new TreeNode<K,T>(key, this->uniqueNodeCount);

				// Add node as child of current node
				pos->addChild(key, node);

				// Set Parent of Node to current position
				node->setParent(pos);

				// Update position in tree
				this->pos = node;
			}
			else
			{
				this->pos = search->second;
			}
		}

		template<class K, class T>
		void Tree<K,T>::moveToParent()
		{
			// Move position to the parent of the current node.
			// If one does not exist (i.e. root), then remain where we are

			// Update Exit Counter (we will do this even if we don't move due to being at root,
			// since this effectively signifies the end of the graph)
			this->nodeExitCount = this->nodeExitCount + 1;

			if(this->pos->parent == nullptr)
			{
				return;
			}

			this->pos = this->pos->parent;
		}
	}
}

// Explicit Instantiation
using namespace treetimer::data_structures;

// Classes
template class Tree<std::string, treetimer::measurement::InstrumentationData>;
