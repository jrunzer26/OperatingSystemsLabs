/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"
#include "hostd.h"

// Put macros or constants here using #define
#define MEMORY 1024
 proc tempProc;
 int avail_mem[MEMORY];
//Index 0 of avail_resources represents printers, 1 = scanner, 2 = modem, and 3 = CD Drives
 
 bool wasSuspended = false;

// Put global environment variables here
 node_t * dispatchList;
 node_t *priority;
 node_t *user1;
 node_t *user2;
 node_t *user3;

 resources res_avail;

// Define functions declared in hostd.h here

 int main(int argc, char *argv[]) {
	// ==================== YOUR CODE HERE ==================== //
 	

 	res_avail.printers = 2;
 	res_avail.scanners = 1;
 	res_avail.modems = 1;
 	res_avail.cds = 2;

	//make the five queues
 	dispatchList = NULL;
 	priority = NULL;
 	user1 = NULL;
 	user2 = NULL;
 	user3 = NULL;

	//allocate memory for the queue
 	dispatchList = malloc(sizeof(node_t));
 	dispatchList->next = NULL;
 	priority = malloc(sizeof(node_t));
 	priority->next = NULL;
 	user1 = malloc(sizeof(node_t));
 	user1->next = NULL;
 	user2 = malloc(sizeof(node_t));
 	user2->next = NULL;
 	user3 = malloc(sizeof(node_t));
 	user3->next = NULL;

 	for (int i = 0; i < MEMORY; i++) {
 		avail_mem[i] = 0;
 	}

	// Load the dispatchlist into the job dispatch queue (from utility.c)
 	load_dispatch("dispatchlist", dispatchList, tempProc);
	

	// Iterate through each item in the job dispatch list, add each process
 	while (dispatchList->next != NULL) {
 		tempProc = *pop(dispatchList, tempProc);

		//figure dispatch the jobs to the other lists
 		if (tempProc.priority == 0) {
 			push(priority, tempProc);
 		} else if (tempProc.priority == 1) {
 			push(user1, tempProc);
 		} else if (tempProc.priority == 2) {
 			push(user2, tempProc);
 		} else
 		push(user3, tempProc);

 	}
 	free(dispatchList);
    printf("\nPriority 0\n\n");
	//Iterate through the priority queue
 	while (priority->next != NULL) {
 		tempProc = *pop(priority, tempProc);

 		//allocate the memory for the process , if it can allocate, then run the process
 		if (allocateMemory(avail_mem, MEMORY, &tempProc)) {
			runProc(tempProc, argv, user1, user2, user3); // run the process!!!!!
		}
	}
	printf("\nUSER 1 POP\n\n");
	while (user1->next != NULL) {
		tempProc = *pop(user1, tempProc);
		
		//Variable to store the state of the process BEFORE memory is allocated
		wasSuspended = tempProc.suspended;
	
		//this is probably dead too
	
		if (allocateMemory(avail_mem, MEMORY, &tempProc)) {

			//Ensure that there are enough resources for the process
			if (allocate_resources(&res_avail, tempProc)) {

				runProc(tempProc, argv, user1, user2, user3);

			} else {
				//If program was not suspended before memory allocation, de-allocate the memory
				if (wasSuspended == false) {
					deallocateMem(avail_mem, tempProc);
				}
				//Push the process to the next queue
				tempProc.priority++;
				push(user2, tempProc);
			}

		} else {
            //push the process to the next queue if there is not enough memory
			tempProc.priority++;
			push(user2, tempProc);
		}
	}
	printf("\nUSER 2 POP\n\n");
	while (user2->next != NULL) {

		tempProc = *pop(user2, tempProc); //get the next process
		

		wasSuspended = tempProc.suspended;

		//try to allocate memory and resources
		if (allocateMemory(avail_mem, MEMORY, &tempProc)) {
			if (allocate_resources(&res_avail, tempProc)) {
                //run the proc if it has resources and memory
				runProc(tempProc, argv, user1, user2, user3);

			} else {
                //free the memory if the process was not suspended, sice the allocate memory funciton returned true, meaning it allocated resources
				if (wasSuspended == false) {
					deallocateMem(avail_mem, tempProc);
				}
                //push that to the next priority
				tempProc.priority++;
				push(user3, tempProc);
			}

		} else {
            //if there was no memory available for the resource, push it to the next list
			tempProc.priority++;
			push(user3, tempProc);
		}
	}
	printf("\nUSER 3 POP\n\n");
	while (user3->next != NULL) {
		tempProc = *pop(user3, tempProc); //get the next proc
		
		wasSuspended = tempProc.suspended;
        //try to allocate memory
		if (allocateMemory(avail_mem, MEMORY, &tempProc)) {
            //try to allocate reources
			if (allocate_resources(&res_avail, tempProc)) {
                //run the proc if it has both memory and resources
				runProc(tempProc, argv, user1, user2, user3);

			} else {//if not deallocate the memory then push it to the list
				if (wasSuspended == false) {
					deallocateMem(avail_mem, tempProc);
				}

				push(user3, tempProc);
			}

		} else {
            //push it back to the list since there was not enough memory
			push(user3, tempProc);
		}
	}
    //print out the contensts of the avail_mem array
    /*
    printf("\nAvail MEM \n");
	for (int i = 0; i < MEMORY; i++) {
        printf("%d ",avail_mem[i]);
	}
    */
    //print out the available resources at the end of all process execution to check
    //printf("\n\nAvail Resources: Scanners: %d, printers: %d, modems: %d, cds: %d\n", res_avail.scanners,res_avail.printers,res_avail.modems,res_avail.cds);
	
    //free up all pointers
    free(dispatchList);
    free(priority);
	free(user1);
	free(user2);
	free(user3);

	return EXIT_SUCCESS;
}

void runProc(proc tempProc, char *argv[], node_t *user1, node_t *user2,
	node_t *user3) {

	int status;
	pid_t pid;

	if (!tempProc.suspended) { //check if it was suspended or not

		pid = fork(); //fork the child
		if (pid == 0) { //CHILD

			execv("./process", argv);
			exit(0);
		} else if (pid < 0) { //FORK FAILED
			status = -1;
		} else { //PARENT

			tempProc.pid = pid;

			printProc(&tempProc);

			//if its a priority get it done!
			if (tempProc.priority == 0) {
				sleep(tempProc.runtime); //sleep for the runtime
				kill(pid, SIGINT);
				waitpid(tempProc.pid, &status, 0);
				deallocateMem(avail_mem, tempProc); //deallocate the memory
				//return_resources(&res_avail, tempProc);

			} else {
				sleep(1);
				tempProc.runtime--; //decrease the runtime
				tempProc.suspended = true;
				kill(pid, SIGTSTP); //suspend the

				//add the process to the designated priority
				if (tempProc.priority >= 3) {
					push(user3, tempProc);
				} else {

					if (tempProc.priority == 1) {
						tempProc.priority++;
						push(user2, tempProc); //if the process is not finished, add it to the end of the queue.
					} else if (tempProc.priority == 2) {
						tempProc.priority++;
						push(user3, tempProc);
					}
				}

			}

		}
	} else {

		printProc(&tempProc);

		kill(tempProc.pid, SIGCONT); //signal the process to continue
		sleep(1);
		kill(tempProc.pid, SIGTSTP); //tell the process to suspend

		if (tempProc.runtime <= 1) { //kill the process if it finished
			kill(tempProc.pid, SIGINT);
			waitpid(tempProc.pid, &status, 0);
			deallocateMem(avail_mem, tempProc); //deallocate the used memory

			
			return_resources(&res_avail, tempProc);
		} else {

			tempProc.runtime--;
            //add to the next list
			if (tempProc.priority >= 3) {
				push(user3, tempProc);
			} else {

				if (tempProc.priority == 1) {
					tempProc.priority++;
					push(user2, tempProc); //if the process is not finished, add it to the end of the queue.
				} else if (tempProc.priority == 2) {
					tempProc.priority++;
					push(user3, tempProc);
				}
			}
		}

	}
   
}

