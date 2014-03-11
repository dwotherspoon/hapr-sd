/* queue.h
* Queue functionality for filter chain.
* Written by: David Wotherspoon, Alex Petherick, Mike Jenic
*/
#include "queue.h"
#include "dbg.h"
#include "filters/filter_table.h"
#include <stdlib.h>

Filter * head = NULL; //chain head
Filter * tail = NULL; //chain tail

/* enqueue
* Either set head and create filter from id or add to tail.
* Written by: David Wotherspoon, 
*/
void enqueue(uint16_t id) {
	ASSERT(id < FTABLE_LEN, "Filter id out of range.");
	Filter * in = (ftable_creators[id])();
	if (head == NULL) {
		head = in;
		tail = in;
	}
	else {
		tail->next = in;
		tail = in;
	}
}

/* dequeue
* Remove the first(head) filter from chain.
* Written by:  Alex Petherick
*/
void dequeue(void) {
	ASSERT(head != NULL, "Cannot dequeue, list empty.");
	Filter * n_head = head->next;
	free(head->privateData); //free sub elems
	free(head);
	head = n_head;	
}

/* chain_apply
* Interate over filter queue and produce final value.
* Written by: Mike Jenic
*/
int16_t chain_apply(volatile int16_t * buf, volatile uint16_t ptr) {
	ASSERT(head != NULL, "Filter chain empty!");
	int16_t temp = buf[ptr]; //use current sample as initial value.
	Filter * current = head;	
	while (current != NULL) { //iterate over filter queue
		temp = (current->apply)(current, temp, buf, ptr); //run this filter
		current = current->next;
	}
	return temp;
}

/* dequeuep
* Dequeue from a specified index (position) in the queue.
* Written by: David Wotherspoon, Alex Petherick, Mike Jenic
*/
void dequeuep(uint16_t pos) { 
	Filter * cur = head;
	Filter * nxt = NULL;
	if (pos == 0) {
		dequeue();
	}
	else {
		while (pos != 1) { //run through to one before filter.
			cur = cur->next;
			ASSERT(cur != NULL, "Dequeue exceeded queue length.");
			pos--;
		}

		nxt = cur->next->next;
		ASSERT(cur->next != NULL, "Dequeue exceeded queue length.");

		free(cur->next->privateData); //remove filter from memory
		free(cur->next);

		if (cur->next == tail) { //dequeuing tail?
			tail = cur; //update tail.
		}
		else { //link nxt to cur.
			cur->next = nxt;
		}
	}
}

/* replace
* Replace filter at index with filter of id given.
* Written by: David Wotherspoon
*/
void replace(uint16_t pos, uint16_t id) {
	ASSERT(id < FTABLE_LEN, "Filter id out of range.");
	Filter * cur = head;
	Filter * new = (ftable_creators[id])(); //create the new filter

	if (pos == 0) { //head?
		new->next = head->next;
		head = new;
	}
	else { //not head
		while (pos != 1) { //find one before filter to replace
			cur = cur->next;
			ASSERT(cur != NULL, "Replace exceeded queue length.")
			pos--;
		}
		ASSERT(cur->next != NULL, "Replace exceeded queue length.")

		tail = (tail == cur->next) ? new : tail; //update tail?
		new->next = cur->next->next; //set next field
		free(cur->next->privateData); //remove old filter.
		free(cur->next); 
		cur->next = new; //insert into chain.
	}
}

/* find_filter
* Find the id of a filter from the apply pointer.
* Written by: David Wotherspoon
*/
uint16_t find_filter(void * apply) {
	uint16_t i = 0;
	for (; i < (FTABLE_LEN); i++ ) {
		if (ftable_applys[i] == apply) {
			return i;
		}
	}
	THROW("Unable to find filter from apply function.");
	return 0;
}

/* chain_debug
* Interate over the queue and run filter show functions.
* Written by: Alex Petherick, Mike Jenic
*/
void chain_debug(void) {
	ASSERT(head != NULL, "Filter chain empty!");
	Filter * current = head;
	while (current != NULL) { //iterate over filter queue
		(current->show)(current); //run this filter show
		current = current->next;
	}
}
