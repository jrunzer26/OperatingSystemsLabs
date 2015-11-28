/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include "utility.h"
#include <signal.h>

// Your linked list structure for your queue
typedef struct node {
	proc process;
	struct node *next;
} node_t;


extern void push(node_t *queue, proc process);

extern proc *pop(node_t *queue, proc tempProc);

extern void printList(node_t *queue, proc tempProc);

#endif /* QUEUE_H_ */
