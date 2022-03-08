#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "pcb.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "shell.h"


struct PCB* head = NULL;
struct PCB* tail = NULL;
struct PCB* currentPCB;
int pcb_size = 0;

struct PCB* PCBinitialize(int start, int length) {
    struct PCB* p = (struct PCB*) malloc(sizeof(struct PCB));
    p->PC = start;
    p->PID = rand();
    p->start = start;
    p->length = length;
    p->instruction = start;
    p->next = NULL;
    p->job_length_score = length;

    return p;
}

void iterateThroughQueue() {
    struct PCB* current = head;
    while(current) {
        printf("(%d, %d)", current->PID, current->job_length_score);
        current = current->next;
    }
}

int getPCBSize() {
    return pcb_size;
}

// appends to end of list
void addPCBToReadyQueue(struct PCB* p) {
    if(head == NULL) {
        head = p;
        tail = p;
    }
    else {
        tail->next = p;
        tail = p;
    }
    pcb_size++;
}

// sets p->instruction
void current_instruction(struct PCB* p, int line_number) {
    p->instruction = line_number;
}

void link_PCB(struct PCB* p, struct PCB* next_PCB) {
    p->next = next_PCB;
}

struct PCB* findPCBHead() {
    return head;
}

// creates a copy, deletes the node, returns the copy (head maintained)
struct PCB* deletePCB(struct PCB* p) {

    // clone node (TODO: Syntax)
    struct PCB* deletedNode = clonePCB(p);

    if(p->next && p->next == tail) {   // reallocate tail
        tail = p;
    }
    // delete node (if head, p->next becomes head)
    // node->val = node->next->val
    if(p->next) {
        p->PC = p->next->PC;
        p->PID = p->next->PID;
        p->start = p->next->start;
        p->length = p->next->length;
        p->job_length_score = p->next->job_length_score;
        p->instruction = p->next->instruction;
        p->next = p->next->next;
    } else {    // reassign the tail, make node null
        struct PCB* temp = head;
        while(temp->next && temp->next->next) {
            temp = temp->next;
        }
        tail = temp;
        p = NULL;
    }
    pcb_size--;

    return deletedNode;
}

void cleanUp() {
    mem_init();
}

// return the head, make new head = head->next
struct PCB* getHeadReadyQueueFCFS() {
    struct PCB* output;
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    // extract output
    output = head;
    head = head->next;    // new list
    output->next = NULL;

    return output;
}

struct PCB* getHeadReadyQueueRR() {
    struct PCB* output;
    
    if(head == NULL) {
        tail = NULL;
        return NULL;
    }
    
    // extract output
    output = head;
    head = head->next;    // new list
    output->next = NULL;

    int end_of_file = output->start + output->length;
    if(output->instruction+2 < end_of_file) {
        addPCBToReadyQueue(output);
    }
    // return output;
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
    head = head->next;      // get new head
    output->next = NULL;    //  deallocate pointers

    rearrangeSJF();
    
    // return output (old head)
    return output;
}

void rearrangeSJF() {
    if(head) {
        struct PCB* temp = head;        // temp = new head (temporary)
        struct PCB* node = head;        // node to be taken from the queue
        int shortest_length = INT_MAX;  // will be updated throughout loop to ensure shortest_length is shortest length (temporary)

        // find node
        while(temp) {
            shortest_length = INT_MAX;
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
            struct PCB* newNode = clonePCB(node);
            newNode->next = head;
            head = newNode;
            temp = head;

            // now, remove the tail (which is now the new head) and reallocate it's pointer
            while(temp->next != NULL && temp->next->next != NULL) {
                temp = temp->next;
            }
            temp->next = NULL;
            tail = temp;
        }

    }
}

struct PCB* clonePCB(struct PCB* p) {
    struct PCB* newNode = PCBinitialize(p->start, p->length);
    newNode->PC = p->PC;
    newNode->PID = p->PID;
    newNode->job_length_score = p->job_length_score;
    newNode->instruction = p->instruction;
    return newNode;
}

// in SJF, a process will have completed 100% and thus we can remove the PCB from the ready queue. This is not the case with Aging.
struct PCB* getHeadReadyQueueAging() {
    struct PCB* output;

    if(head == NULL) {
        tail = NULL;
        return NULL;
    }

    rearrangeAging();
    // get output (old head)
    output = head;
    head = head->next;      // get new head
    output->next = NULL;    //  deallocate pointers

    return output;
}

void rearrangeAging() {
    if(head != NULL) {
        struct PCB* temp = head;        // temp = new head (temporary)
        struct PCB* node = head;        // node to be taken from the queue
        int shortest_length;  // will be updated throughout loop to ensure shortest_length is shortest length (temporary)

        // find node
        while(temp != NULL) {
            shortest_length = head->job_length_score;
            if(temp->job_length_score < shortest_length) {
                shortest_length = temp->job_length_score;
                node = temp;
            }
            temp = temp->next;
        }
        // iterateThroughQueue();

        // remove node from queue
        if(node->next) {
            node = deletePCB(node);

            // make node new head
            node->next = head;
            head = node;
        } else {    // remove old tail (a clone of the new head)
            struct PCB* newNode = clonePCB(node);
            newNode->next = head;
            head = newNode;
            temp = head;

            // now, remove the tail (which is now the new head) and reallocate it's pointer
            while(temp->next != NULL && temp->next->next != NULL) {
                temp = temp->next;
            }
            temp->next = NULL;
            tail = temp;
        }

    }
}

void schedulerLogic(char *files[], int files_size) {
    // step 1 : for all files, create a PCB and add to ready queue (linked list)
    int i = 0;
    // works up to here

    if (strcmp(files[files_size-1], "SJF") == 0)
    {
        for(int j = 1; j < files_size-1; j++) {

            int new_pos = load(files[j], i);
            int length = new_pos-i+1;
            struct PCB* p = PCBinitialize(i, length);
            addPCBToReadyQueue(p);
            i = new_pos+1;
        }
        sjf();
    }
    else if (strcmp(files[files_size-1], "RR") == 0)
    {
        for(int j = 1; j < files_size-1; j++) {

            int new_pos = load(files[j], i);
            int length = new_pos-i+1;
            struct PCB* p = PCBinitialize(i, length);
            addPCBToReadyQueue(p);
            i = new_pos+1;
        }
        rr();
    }
    else if (strcmp(files[files_size-1], "AGING") == 0)
    {
        for(int j = 1; j < files_size-1; j++) {

            int new_pos = load(files[j], i);
            int length = new_pos-i+1;
            struct PCB* p = PCBinitialize(i, length);
            addPCBToReadyQueue(p);
            i = new_pos+1;
        }
        aging();
    }
    else if (strcmp(files[files_size-1], "FCFS") == 0)
    {
        for(int j = 1; j < files_size-1; j++) {

            int new_pos = load(files[j], i);
            int length = new_pos-i+1;
            struct PCB* p = PCBinitialize(i, length);
            addPCBToReadyQueue(p);
            i = new_pos+1;
        }
        fcfs();
    }
    else
    {
        badcommand();
        cleanUp();  //should i call this first?
    }
}

void fcfs() {
    // for string parsing
    char instr[1000];
    // while there is a head
    while(1) {
        struct PCB* p = getHeadReadyQueueFCFS();    // extract the head
        if(!p) {
            break;
        }
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
    //rearrangeSJF();
    // for string parsing
    char instr[1000];
    // while there is a head

    while(1) {
        struct PCB* p = getHeadReadyQueueSJF();    // extract the head
        if(!p) {
            break;
        }
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
    char instr[1000];
    // while there is a head
    while(1) {
        struct PCB* p = getHeadReadyQueueRR();    // extract the head
        if(!p) {
            break;
        }
        // set current instruction to start instruction
        // current_instruction(p, p->start);
        int end_of_file = p->start + p->length;
        int rr_counter = 0;

        while(rr_counter < 2 && p->instruction < end_of_file) {   // rr: iterate twice
            sprintf(instr, "%d", p->instruction);
            parseInput(mem_get_value(instr));
            p->instruction++;
            rr_counter++;
        }
    }
}

void decrementScore() {
    if (head == NULL) {
        return;
    }

    struct PCB *temp = head;
//    printf("PID: %d \tSCORE: %d\n", head->PID, head->job_length_score);

    while (temp != NULL) {
        if (temp->job_length_score > 0 && temp->PID != head->PID) {
            temp->job_length_score--;
//            printf("PID: %d \tSCORE: %d\n", temp->PID, temp->job_length_score);
        }

        temp = temp->next; 
    }
}

struct PCB* dequeue() {
    if (head == NULL) {
        return NULL;
    }

    struct PCB *temp = head;
    head = head->next;
    pcb_size--;

    return temp;
}



void aging() {

    // // for string parsing
    // char* instr = NULL;
    // // while there is a head
    // while(1) {
    //     struct PCB* p = getHeadReadyQueueAging();    // extract the head
    //     if(!p) {
    //         break;
    //     }
    //     // set current instruction to start instruction
    //     current_instruction(p, p->start);
    //     int end_of_file = p->start + p->length;
    //     int count = 0;

    //     while(p->instruction < end_of_file && count < 1) {   // aging: performs just like fcfs from here on with count
    //         sprintf(instr, "%d", p->instruction);
    //         parseInput(mem_get_value(instr));
    //         p->instruction++;
    //         count++;
    //     }
    // }

    rearrangeAging();
    // for string parsing
    char instr[1000];
    // while there is a head
    while(1) {
        // rearrangeSJF();
        struct PCB* p = getHeadReadyQueueAging();
        if(p == NULL) {
            break;
        }

        // set current instruction to start instruction
        // current_instruction(p, p->start);
        int end_of_file = p->start + p->length;
        int count = 0;

        while(count < 1 && p->instruction < end_of_file) {   // aging: performs just like fcfs from here on with count
            sprintf(instr, "%d", p->instruction);
            parseInput(mem_get_value(instr));
            // if(p->instruction == end_of_file) {
            //     deletePCB(p);
            // }

            // if (head->instruction > (head->start + head->length)) {
            //     dequeue();
            // }
            // decrementScore();
            rearrangeAging();

            p->instruction++;
            count++;
        }
        struct PCB* temp = head;

        // if (instruction valid) then add to front of ready queue
        if(p->instruction < end_of_file) {
            p->next = temp;
            head = p;
            
            while(temp) {
                temp->job_length_score -= 1;
                temp = temp->next;
            }
        }


    }

    // char instr[1000];
    // //rearrangeSJF();
    // struct PCB* p = getHeadReadyQueueSJF();

    // current_instruction(p, p->start);
    // int end_of_file = p->start + p->length;

    // while(getPCBSize() > 0) {
    //     //struct PCB* p = getHeadReadyQueueSJF();
    //     sprintf(instr, "%d", p->instruction);
    //     parseInput(mem_get_value(instr));
    //     if(head->instruction > (head->start + head->length)) {
    //         dequeue();
    //     }
    //     decrementScore();
    //     rearrangeSJF();
    // }
    

}
