/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_

// The amount of available memory
#define MEMORY 1024
#include <stdbool.h>

// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
	int printers;
	int scanners;
	int modems;
	int cds;
} resources;

//process structure
typedef struct {
	int arrivalTime;
	int priority;
	int runtime;
	int pid;
	int address;
	int memory;
	resources r;
	bool suspended;
	bool hasResources;
	bool deallocated_mem;
} proc;

extern void printProc(proc *pro);
extern void deallocateMem(int *memory, proc tempProc);
extern bool allocateMemory(int *memory, int size, proc *tempProc);
extern bool return_resources(resources *resources, proc tempProc);
extern bool allocate_resources(resources *resources, proc tempProc);



#endif /* UTILITY_H_ */
