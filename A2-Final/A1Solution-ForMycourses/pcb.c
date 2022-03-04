#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"


struct PCB* head = NULL;
struct PCB* tail = NULL;
struct PCB* currentPCB;

struct PCB* PCBinitialize(int start, int end) {
    struct PCB* p = (struct PCB*) malloc(sizeof(struct PCB));
    p->PC = start;
    p->PID = rand();
    p->start = start;
    p->end = end;
    p->instruction = 0;
    p->next = NULL;

    return p;
}

void current_instruction(struct PCB* p, int line_number) {
    p->instruction = line_number;
}

void link_PCB(struct PCB* p, struct PCB* next_PCB) {
    p->next = next_PCB;
}

struct PCB* findPCBHead() {
    return head;
}

struct PCB* getHeadReadyQueue() {
    struct PCB* output
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    output = head;
    head = head->next;
    output->next = NULL;

    // if(head == NULL) {   //just check if this needs to be here
    //     tail = NULL;
    // }

    return output;
}

void addReadyQueuetoPCB(struct PCB* p) {
    if(head == NULL) {
        head = p;
        tail = p;
    }
    else {
        tail->next = p;
        tail = p;
    }
}

void cleanUp() {
    mem_init();
}

void schedulerLogic() {
    while(head != NULL) {
        currentPCB = getHeadReadyQueue();
        
    }
}