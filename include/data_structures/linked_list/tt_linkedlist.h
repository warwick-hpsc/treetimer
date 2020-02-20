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

#ifndef TT_LINKED_LIST_INCLUDE_H
#define TT_LINKED_LIST_INCLUDE_H

#include "tt_linkedlist_node.h"

namespace treetimer
{
	namespace data_structures
	{
		template <class T>
		class LinkedList
		{
			public:
				LinkedList();
				~LinkedList();

				LinkedListNode<T> * head;
				LinkedListNode<T> * tail;

				// ToDo: Should add an iterator to loop over contents
				void add(T data);
				void reset();
		};
	}
}

#endif
