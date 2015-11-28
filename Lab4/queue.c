/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file

//adds a process to the end of the list
void push(node_t * queue, proc process) {
	node_t *current = queue; //set the current node to the first one in the queue

	//traverse through the linked list until it finds the next node = to NULL
	while (current->next != NULL) {
		current = current->next;
	}
	//allocate and and the process to the list
	current->next = (node_t *) malloc(sizeof(node_t));
	current->next->process = process;
	current->next->next = NULL; //set the next node to NULL to specify the end of the list
}

	//pops the first node in the list and returns a pointer to the process that was deleted
proc * pop(node_t * queue, proc tempProc) {
	node_t * next_node = NULL;
	if (queue->next == NULL) {
		return NULL;
	}

	next_node = queue->next->next; // get the next node after the sentinal
	node_t * currentNode = queue->next;
	tempProc = currentNode->process;

	free(currentNode);
	queue->next = next_node;

	return &tempProc;

}

void printList(node_t *list, proc tempProc) {
	node_t * current = list;
	//get rid of the sentinal
	current = current->next;

	while (current != NULL) {

		tempProc = current->process;
		printProc(&tempProc); //print the current process
		current = current->next; //advance the node
	}

}

