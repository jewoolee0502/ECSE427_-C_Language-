#ifndef kernel_H
#define kernel_H

#include "pcb.h"

void ready_queue_initialize();

int get_scheduling_policy_number(char* policy);
PCB* get_ready_queue_at(int index);
void printContentsOfReadyQueue();

char* myinit(const char *filename);
int scheduler();

#endif
