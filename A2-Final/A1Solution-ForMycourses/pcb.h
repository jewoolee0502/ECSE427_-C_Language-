#ifndef PCB_H
#define PCB_H

struct PCB {
    int PCB;
    int PC;
    int PID;
    int start;
    int length;
    int instruction;
    int job_length_score;
    struct PCB* next;
};

struct PCB* PCBinitialize(int start, int end);

void link_instructions(struct PCB* p, int line_number);
void link_PCB(struct PCB* p, struct PCB *next_PCB);

struct PCB* findPCBHead();
struct PCB* getHeadReadyQueue();
void addReadyQueuetoPCB(struct PCB* p);

void cleanUp();

void schedulerLogic();

#endif