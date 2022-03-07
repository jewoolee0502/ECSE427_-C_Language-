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

int getPCBsize();
void addPCBToReadyQueue(struct PCB* p);
// int removePCBFromReadyQueue();
// int executeQueue();

void current_instruction(struct PCB* p, int line_number);
void link_PCB(struct PCB* p, struct PCB* next_PCB);
void rearrangeSJF();
void rearrangeAging();
struct PCB* findPCBHead();
struct PCB* deletePCB(struct PCB* p);
struct PCB* clonePCB(struct PCB *p);

void cleanUp();

struct PCB* getHeadReadyQueueFCFS();
struct PCB* getHeadReadyQueueRR();
struct PCB* getHeadReadyQueueSJF();
struct PCB* getHeadReadyQueueAging();

void schedulerLogic(char *files[], int files_size);

void fcfs();
void sjf();
void rr();
void aging();


#endif