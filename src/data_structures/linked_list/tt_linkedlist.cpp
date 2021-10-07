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

#include "tt_linkedlist.h"
#include "tt_linkedlist_node.h"
#include "tt_agg_timings.h"
#include "tt_agg_parameter.h"
#include "tt_trace_timer.h"
#include "tt_trace_parameter.h"
#include <iostream>

namespace treetimer
{
	namespace data_structures
	{
		template <class T>
		LinkedList<T>::LinkedList()
		{
			size = 0;
			head = nullptr;
			tail = nullptr;
		}

		template <class T>
		LinkedList<T>::~LinkedList()
		{
			reset();
		}

		template <class T>
		void LinkedList<T>::reset()
		{
			if(this->head != nullptr)
			{
				LinkedListNode<T> *ptr = this->head;

				while(ptr != nullptr)
				{
					LinkedListNode<T> *current = ptr;
					ptr = ptr->next;
					delete(current);
				}
			}

			this->head = nullptr;
			this->tail = nullptr;
		}

		template <class T>
		void LinkedList<T>::add(T data)
		{
			if(head == nullptr)
			{
				head = new LinkedListNode<T>(data);
				tail = head;
			}
			else
			{
				tail->next = new LinkedListNode<T>(data);
				tail = tail->next;
			}
			this->size++;
		}
	}
}

// Explicit Instantiation
template class treetimer::data_structures::LinkedList<treetimer::timers::AggTimings>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::AggParameter<int>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::AggParameter<long>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::AggParameter<double>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::AggParameter<bool>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::AggParameter<std::string>>;

template class treetimer::data_structures::LinkedList<treetimer::timers::TraceTimer>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::TraceParameter<int>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::TraceParameter<long>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::TraceParameter<double>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::TraceParameter<bool>>;
template class treetimer::data_structures::LinkedList<treetimer::parameters::TraceParameter<std::string>>;
