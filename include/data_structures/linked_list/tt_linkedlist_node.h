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

#ifndef TT_LINKEDLIST_NODE_INCLUDE_H
#define TT_LINKEDLIST_NODE_INCLUDE_H

namespace treetimer
{
	namespace data_structures
	{
		template <class T>
		class LinkedListNode
		{
			public:
				LinkedListNode();
				LinkedListNode(T data);
				~LinkedListNode();

				T data;
				LinkedListNode * next;
		};
	}
}

#endif
