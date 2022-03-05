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
    p->job_length_score = length;

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

// return the head, make new head = head->next
struct PCB* getHeadReadyQueueFCFS() {
    struct PCB* output;
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    output = head;
    head = head->next;
    output->next = NULL;

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
    // TODO: not sure about <= operator
    if(head->instruction <= end_of_file) {
        tail->next = output;
        tail = tail->next;
    }
    
    return output;
}

// in SJF, a process will have completed 100% and thus we can remove the PCB from the ready queue
struct PCB* getHeadReadyQueueSJF() {
    struct PCB* output;
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    // get output (old head)
    output = head;
    output->next = NULL;    //  deallocate pointers

    // get new head
    head = head->next;

    if(head) {
        struct PCB* temp = head;        // temp = new head (temporary)
        struct PCB* node = head;        // node to be taken from the queue
        int shortest_length = INT_MAX;  // will be updated throughout loop to ensure shortest_length is shortest length (temporary)

        // find node
        while(temp) {
            if(temp->length < shortest_length) {
                shortest_length = temp->length;
                node = temp;
            }
            temp = temp->next;
        }

        // remove node from queue
        if(node->next) {
            node = deletePCB(node);

            // make node new head
            node->next = head;
            head = node;
        } else {    // remove old tail (a clone of the new head)
            node->next = head;  // make new head = node
            head = node;
            temp = head;
            // now, remove the tail (which is now the new head) and reallocate it's pointer
            while(temp->next && temp->next->next) {
                temp = temp->next;
            }
            temp->next = NULL;
            tail = temp;
        }

    }
    
    // return output (old head)
    return output;
}

// creates a copy, deletes the node, returns the copy (head maintained)
struct PCB* deletePCB(struct PCB* p) {
    // clone node (TODO: Syntax)
    struct PCB* deletedNode;
    deletedNode->PC = p->PC;
    deletedNode->PID = p->PID;
    deletedNode->start = p->start;
    deletedNode->length = p->length;
    deletedNode->job_length_score = p->job_length_score;
    deletedNode->instruction = p->instruction;

    if(p->next == tail) {   // reallocate tail
        tail = p;
    }
    // delete node (if head, p->next becomes head)
    // node->val = node->next->val
    p->PC = p->next->PC;
    p->PID = p->next->PID;
    p->start = p->next->start;
    p->length = p->next->length;
    p->job_length_score = p->next->job_length_score;
    p->instruction = p->next->instruction;
    p->next = p->next->next;

    return deletedNode;
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
        int length = new_pos-i+1;
        struct PCB* p = PCBinitialize(i, length);
        addPCBToReadyQueue(p);
        i = new_pos+1;
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

struct PCB* getHeadReadyQueueAging() {
    struct PCB* output;
    
    if(!head == NULL) {
        tail = NULL;
        return NULL;
    }

    struct PCB* temp = head->next;
    struct PCB* node = head;
    int shortest_length = head->job_length_score-1;

    while(temp) {
        temp->job_length_score -= 1;
        // if equal in length, take the first occurrence (a strict "<" accounts for this)
        if(temp->job_length_score < shortest_length) {
            shortest_length = temp->length;
            node = temp;
        }
        // end do something
        temp = temp->next;
    }
    node = deletePCB(node);

    node->next = head;
    head = node;
    
    return head;
}

void aging() {
    // reassess for every time slice of 1 instruction
    // get the head of the queue
    // perform one instruction

    // 
}
