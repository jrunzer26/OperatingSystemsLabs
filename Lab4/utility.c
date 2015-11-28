/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>

#include "utility.h"
#include "queue.h"
#define MAX_PRINTERS 2
#define MAX_MODEMS 1
#define MAX_SCANNERS 1
#define MAX_CD 2



//allocate resoureces for the process, returns true if the process was suspended, and true if it was able to allocate the resources
 bool allocate_resources(resources *resources, proc tempProc) {
	
 	//if it was already suspended, dont allocate resources
 	if (tempProc.suspended) {
 		return true;
 	}
 	//Check if the resources needs to be allocated, and if there is enough resources to allocate.
 	if (tempProc.r.printers != 0 ||tempProc.r.scanners != 0
 		|| tempProc.r.modems != 0 || tempProc.r.cds != 0) {
 		if (tempProc.r.printers <= resources->printers
 			&& tempProc.r.scanners <= resources->scanners
 			&& tempProc.r.modems <= resources->modems
 			&& tempProc.r.cds <= resources->cds) {
 			resources->printers = resources->printers - tempProc.r.printers;
 		resources->scanners = resources->scanners - tempProc.r.scanners;
 		resources->modems = resources->modems - tempProc.r.modems;
 		resources->cds = resources->cds - tempProc.r.cds;
 		
 	} else {
 		return false;
 	}
 	
 }
 
	
 return true;
}
//returns the resources that the process has used
bool return_resources(resources *resources, proc tempProc) {

	//Ensure that re-allocating resources will not exceed original amount. If not, re-allocate
	if (resources->printers + tempProc.r.printers <= MAX_PRINTERS
		&& resources->scanners + tempProc.r.scanners <= MAX_SCANNERS
		&& resources->modems + tempProc.r.modems <= MAX_MODEMS
		&& resources->cds + tempProc.r.cds <= MAX_CD) {

		resources->printers = resources->printers + tempProc.r.printers;
	resources->scanners = resources->scanners + tempProc.r.scanners;
	resources->modems = resources->modems + tempProc.r.modems;
	resources->cds = resources->cds + tempProc.r.cds;

} else {
	return false;
	
}

return true;
}

//deallocated the memory that is specified for the current process
void deallocateMem(int *memory, proc tempProc) {

	for (int i = 0; i < tempProc.memory; i++) {
		memory[i + tempProc.address] = 0;
	}
}

//allocates memory for the current process. returns true if successful, false otherwise
bool allocateMemory(int *memory, int size, proc *tempProc) {
	//already has allocated memory
	if (tempProc->suspended)
		return true;
	sleep(1); // used to delay the output so the prints for each process when running looks normal
	int allocated = 0; //the number of allocated spots possible
	int start = 0; //the starting position of a valid address for a proc
	
	 int a = 0;

	 if (tempProc->priority > 0) {
	 	a = 64;
	 	start = 64;
	 }
	//go through the size of the array and check each number for unallocated spaces
	 for (int i = a; i < size; i++) {

	 	if (memory[i] == 1) {
	 		allocated = 0;
			start = i + 1; //reset the start address

		} else {
			allocated++;

		}
		//check if the total space is free for the proc
		if (allocated == tempProc->memory) {
			tempProc->address = start; //set the address of the proc

			//allocate the memory for the proc
			for (int k = 0; k < tempProc->memory; k++) {
				memory[k + tempProc->address] = 1;

			}
			
			 return true;
			}

		}
	
		return false;
	}

//asdasda
//loads the dispatch list into the given queue
//tempProc is used to make sure the numbers for the process are not deleted
	void load_dispatch(char *dispatch_file, node_t *queue, proc tempProc) {
		size_t len = 0;
		ssize_t read;
		char *line = NULL;

	//open the file to get the processes
		FILE *fp = fopen(dispatch_file, "r");

		if (!fp) {
		}

	//go though the text file and add each process to the list
		while ((read = getline(&line, &len, fp)) != -1) {
		//tokenize the line with , to get each entity of the process
		char * tokens = NULL;
		tokens = strtok(line, ",\n");
		tempProc.arrivalTime = atoi(tokens);
		tokens = strtok(NULL, ",\n");
		tempProc.priority = atoi(tokens); //atoi casts a char * to an integer
		tokens = strtok(NULL, ",\n");
		tempProc.runtime = atoi(tokens);
		tokens = strtok(NULL, ",\n");
		tempProc.memory = atoi(tokens);
		tokens = strtok(NULL, ",\n");
		tempProc.r.printers = atoi(tokens);
		tokens = strtok(NULL, ",\n");
		tempProc.r.scanners = atoi(tokens);
		tokens = strtok(NULL, ",\n");
		tempProc.r.modems = atoi(tokens);
		tokens = strtok(NULL, ",\n");
		tempProc.r.cds = atoi(tokens);
		tempProc.pid = 0;
		tempProc.address = 0;
		tempProc.suspended = false;

		push(queue, tempProc); //push the process to the list

	}
	//free everything
	fclose(fp);
	free(line);

}

//prints the process that is given
void printProc(proc *pro) {

	if (pro != NULL) {
		printf("Arrival Time: %d\nProcess ID: %d\nPriority: %d\nProcessor Time Remaining: %d\nMemory Location: %d\nBlock Size: %d\nResources Requested : Printers: %d Scanners: %d Modems: %d CDs: %d\n\n",
			pro->arrivalTime, pro->pid, pro->priority, pro->runtime,
			pro->address, pro->memory, pro->r.printers, pro->r.scanners,
			pro->r.modems, pro->r.cds);
	}
}
