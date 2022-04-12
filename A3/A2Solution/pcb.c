#include "pcb.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//In this implementation, Pid is the same as file ID 
PCB* makePCB(int start, int end, char* pid){
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pid = pid;
    newPCB->PC = start;
    newPCB->start  = start;
    newPCB->end = end;
    newPCB->job_length_score = 1+end-start;
    // NEW VARS
    // newPCB->fileName = "";
    // probably not necessary
    newPCB->index_init_pt = 0;
    newPCB->index_within_fs = 0;
    newPCB->index_cur_pt = 0;
    for(int i = 0; i < 100; i++) {
        newPCB->page_table[i] = -1;
    }
    newPCB->num_pages = 0;
    return newPCB;
}