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
#include "tt_agg_timings.h"
#include "tt_agg_parameter.h"
#include "tt_trace_timer.h"
#include "tt_trace_parameter.h"

namespace treetimer
{
	namespace data_structures
	{
		template <class T>
		LinkedListNode<T>::LinkedListNode()
		{
			this->next = nullptr;
		}

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
template class treetimer::data_structures::LinkedListNode<treetimer::timers::AggTimings>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<int>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<long>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<double>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<bool>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::AggParameter<std::string>>;

template class treetimer::data_structures::LinkedListNode<treetimer::timers::TraceTimer>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<int>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<long>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<double>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<bool>>;
template class treetimer::data_structures::LinkedListNode<treetimer::parameters::TraceParameter<std::string>>;
