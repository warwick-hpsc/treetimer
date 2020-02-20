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

#include "tt_linkedlist_node.h"
#include "tt_trace_timer.h"
#include "tt_trace_parameter.h"

namespace treetimer
{
	namespace data_structures
	{
		template <class T>
		LinkedListNode<T>::LinkedListNode(T data)
		{
			this->data = data;
			this->next = nullptr;
		}

		template <class T>
		LinkedListNode<T>::~LinkedListNode()
		{

		}
	}
}

// Explicit Instantiation
using namespace treetimer::data_structures;

// Classes
template class LinkedListNode<treetimer::timers::TraceTimer>;
template class LinkedListNode<treetimer::parameters::TraceParameter<int>>;
template class LinkedListNode<treetimer::parameters::TraceParameter<double>>;
template class LinkedListNode<treetimer::parameters::TraceParameter<bool>>;
template class LinkedListNode<treetimer::parameters::TraceParameter<std::string>>;
