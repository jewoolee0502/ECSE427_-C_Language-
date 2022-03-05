#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pcb.h"
#include "shellmemory.h"


struct PCB* head = NULL;
struct PCB* tail = NULL;
struct PCB* currentPCB;

struct PCB* PCBinitialize(int start, int length) {
    struct PCB* p = (struct PCB*) malloc(sizeof(struct PCB));
    p->PC = start;
    p->PID = rand();
    p->start = start;
    p->length = length;
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

struct PCB* getHeadReadyQueueFCFS() {
    struct PCB* output;
    
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

struct PCB* getHeadReadyQueueRR() {
    struct PCB* output;
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    output = head;
    head = head->next;
    output->next = NULL;

    // to continue presence in the queue
    int end_of_file = head->start + head->length;
    if(head->instruction <= end_of_file) {
        tail->next = head;
        tail = tail->next;
    }
    
    return output;
}

struct PCB* getHeadReadyQueueSJF() {
    struct PCB* output;
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    // get output
    output = head;

    // discard the head
    head = head->next;
    output->next = NULL;


    /*  determine new head...
        1/  go through list, find the smallest length process
        2/  save that node as "node"
        3/  remove "node" from the list
        4/  append it to the head
    */

    struct PCB* temp = head;        // setting up params - temp will be used to iterate through linked list
    struct PCB* node = NULL;        // node will be taken from the linked list and appended to the front of the linked list
    int shortest_length = INT_MAX;  // this will be updated throughout the while loop to ensure shortest_length = shortest length


    // 1/ and 2/    standard procedure for linked list iteration
    while(temp) {
        // do something (in this case, update node and shortest length)
        if(temp->length < shortest_length) {
            shortest_length = temp->length;
            node = temp;
        }
        // end do something
        temp = temp->next;
    }


    // 3/   remove "node" from the list

    // node->val = node->next->val
    node->PC = node->next->PC;
    node->PID = node->next->PID;
    node->start = node->next->start;
    node->length = node->next->length;
    node->instruction = node->next->instruction;

    // node->next = node->next->next
    node->next = node->next->next;

    // 4/   append it to the head
    node->next = head;
    head = node;
    
    return output;
}

void addPCBToReadyQueue(struct PCB* p) {
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

void schedulerLogic(char *files[], char *policy) {
    
    // step 1 : for all files, create a PCB and add to ready queue (linked list)
    int i = 0;
    while (*files != "") {  // iterate through all files
        int new_pos = load(*files++, i);
        int length = new_pos-i;
        struct PCB* p = PCBinitialize(i, length);
        addPCBToReadyQueue(p);
        i = new_pos;
    }

    if (strcmp(policy, "SJF") == 0)
    {
        sjf();
    }
    else if (strcmp(policy, "RR") == 0)
    {
        rr();
    }
    else if (strcmp(policy, "AGING") == 0)
    {
        aging();
    }
    else /* default: FCFS */
    {
        fcfs();
    }
}

void fcfs() {
    // for string parsing
    char instr = NULL;
    // while there is a head
    while(getHeadReadyQueueFCFS()) {
        struct PCB* p = getHeadReadyQueueFCFS();    // extract the head
        // set current instruction to start instruction
        current_instruction(p, p->start);
        int end_of_file = p->start + p->length;
        while(p->instruction < end_of_file) {   // fcfs: iterate through entire PCB
            sprintf(instr, "%d", p->instruction);
            parseInput(mem_get_value(instr));
            p->instruction++;
        }
    }
}

void sjf() {
    // for string parsing
    char instr = NULL;
    // while there is a head
    while(getHeadReadyQueueSJF()) {
        struct PCB* p = getHeadReadyQueueSJF();    // extract the head
        // set current instruction to start instruction
        current_instruction(p, p->start);
        int end_of_file = p->start + p->length;

        while(p->instruction < end_of_file) {   // sjf: performs just like fcfs from here on
            sprintf(instr, "%d", p->instruction);
            parseInput(mem_get_value(instr));
            p->instruction++;
        }
    }
}

void rr() {
    // for string parsing
    char instr = NULL;
    // while there is a head
    while(getHeadReadyQueueRR()) {
        struct PCB* p = getHeadReadyQueueRR();    // extract the head
        // set current instruction to start instruction
        current_instruction(p, p->start);
        int end_of_file = p->start + p->length;
        int rr_counter = 0;

        while(p->instruction < end_of_file && rr_counter < 2) {   // rr: iterate twice
            sprintf(instr, "%d", p->instruction);
            parseInput(mem_get_value(instr));
            p->instruction++;
            rr_counter++;
        }
    }
}

void aging() {

}
