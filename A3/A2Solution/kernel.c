#include "pcb.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "memorymanager.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define QUEUE_LENGTH 10
#define MAX_INT 2147483646
PCB* readyQueue[QUEUE_LENGTH];

PCB* get_ready_queue_at(int index) {
    return readyQueue[index];
}

void printContentsOfReadyQueue() {
    for(int i = 0; i < QUEUE_LENGTH; i++) {
        PCB* cur = get_ready_queue_at(i);
        // printf("file: %s\t", cur->fileName);
        printf("num_lines: %d\t", cur->num_pages);
    }
    printf("\n");
}

void ready_queue_initialize()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        (*readyQueue[i]).PC = -1;
        (*readyQueue[i]).start = -1;
        (*readyQueue[i]).end = -1;
        (*readyQueue[i]).pid = NULL;
        for(int j = 0; j < 100; j++) {
            (*readyQueue[i]).page_table[j] = -1;
        }
        (*readyQueue[i]).index_init_pt = 0;
        (*readyQueue[i]).index_within_fs = 0;
        (*readyQueue[i]).index_cur_pt = 0;
        (*readyQueue[i]).num_pages = 0;
        (*readyQueue[i]).job_length_score = -1;
    }
}

void ready_queue_Empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        (*readyQueue[i]).PC = -1;
        (*readyQueue[i]).start = -1;
        (*readyQueue[i]).end = -1;
        (*readyQueue[i]).pid = NULL;
        for(int j = 0; j < 100; j++) {
            (*readyQueue[i]).page_table[j] = -1;
        }
        (*readyQueue[i]).index_init_pt = 0;
        (*readyQueue[i]).index_within_fs = 0;
        (*readyQueue[i]).index_cur_pt = 0;
        (*readyQueue[i]).num_pages = 0;
        (*readyQueue[i]).job_length_score = -1;
    }
}

void ready_queue_destory()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        free(readyQueue[i]);
    }
}

PCB ready_queue_pop(int index, bool inPlace)
{
    PCB head = (*readyQueue[index]);
    if(inPlace){
        for (size_t i = index+1; i < QUEUE_LENGTH; i++){
            (*readyQueue[i-1]).PC = (*readyQueue[i]).PC;
            (*readyQueue[i-1]).start = (*readyQueue[i]).start;
            (*readyQueue[i-1]).end = (*readyQueue[i]).end;
            (*readyQueue[i-1]).pid = (*readyQueue[i]).pid;
            memcpy((*readyQueue[i-1]).page_table, (*readyQueue[i]).page_table, sizeof (*readyQueue[i]).page_table);
            (*readyQueue[i-1]).index_within_fs = (*readyQueue[i]).index_within_fs;
            (*readyQueue[i-1]).index_init_pt = (*readyQueue[i]).index_init_pt;
            (*readyQueue[i-1]).index_cur_pt = (*readyQueue[i]).index_cur_pt;
            (*readyQueue[i-1]).fileName = (*readyQueue[i]).fileName;
            (*readyQueue[i-1]).num_pages = (*readyQueue[i]).num_pages;
            (*readyQueue[i-1]).job_length_score = (*readyQueue[i]).job_length_score;
        }
        (*readyQueue[QUEUE_LENGTH-1]).PC = -1;
        (*readyQueue[QUEUE_LENGTH-1]).start = -1;
        (*readyQueue[QUEUE_LENGTH-1]).end = -1;
        for(int j = 0; j < 100; j++) {
            (*readyQueue[QUEUE_LENGTH-1]).page_table[j] = -1;
        }
        (*readyQueue[QUEUE_LENGTH-1]).index_init_pt = 0;
        (*readyQueue[QUEUE_LENGTH-1]).index_within_fs = 0;
        (*readyQueue[QUEUE_LENGTH-1]).index_cur_pt = 0;
        (*readyQueue[QUEUE_LENGTH-1]).fileName = NULL;
        (*readyQueue[QUEUE_LENGTH-1]).num_pages = 0;
        (*readyQueue[QUEUE_LENGTH-1]).pid = NULL;
        (*readyQueue[QUEUE_LENGTH-1]).job_length_score = -1;
    }
    return head;
}

void ready_queue_add_to_end(PCB *pPCB)
{
    for(int i = 0; i < QUEUE_LENGTH; i++){
        if ( (*readyQueue[i]).start == -1 ){
            (*readyQueue[i]).PC = (*pPCB).PC;
            (*readyQueue[i]).start = (*pPCB).start;
            (*readyQueue[i]).end = (*pPCB).end;
            (*readyQueue[i]).pid = (*pPCB).pid;
            memcpy((*readyQueue[i]).page_table, (*pPCB).page_table, sizeof (*pPCB).page_table);
            (*readyQueue[i]).index_init_pt = (*pPCB).index_init_pt;
            (*readyQueue[i]).index_within_fs = (*pPCB).index_within_fs;
            (*readyQueue[i]).index_cur_pt = (*pPCB).index_cur_pt;
            (*readyQueue[i]).fileName = (*pPCB).fileName;
            (*readyQueue[i]).num_pages = (*pPCB).num_pages;
            (*readyQueue[i]).job_length_score = (*pPCB).job_length_score;
            break;
        }
    }
}

void ready_queue_add_to_front(PCB *pPCB){
    for (size_t i = QUEUE_LENGTH-1; i > 0; i--){
        (*readyQueue[i]).PC = (*readyQueue[i-1]).PC;
        (*readyQueue[i]).start = (*readyQueue[i-1]).start;
        (*readyQueue[i]).end = (*readyQueue[i-1]).end;
        memcpy((*readyQueue[i]).page_table, (*readyQueue[i-1]).page_table, sizeof (*readyQueue[i-1]).page_table);
        (*readyQueue[i]).index_init_pt = (*readyQueue[i-1]).index_init_pt;
        (*readyQueue[i]).index_within_fs = (*readyQueue[i-1]).index_within_fs;
        (*readyQueue[i]).index_cur_pt = (*readyQueue[i-1]).index_cur_pt;
        (*readyQueue[i]).fileName = (*readyQueue[i-1]).fileName;
        (*readyQueue[i]).num_pages = (*readyQueue[i-1]).num_pages;
        (*readyQueue[i]).pid = (*readyQueue[i-1]).pid;
        (*readyQueue[i]).job_length_score = (*readyQueue[i-1]).job_length_score;
    }
    // readyQueue[0] = pPCB;
    (*readyQueue[0]).PC = (*pPCB).PC;
    (*readyQueue[0]).start = (*pPCB).start;
    (*readyQueue[0]).end = (*pPCB).end;
    memcpy((*readyQueue[0]).page_table, (*pPCB).page_table, sizeof (*pPCB).page_table);
    (*readyQueue[0]).index_init_pt = (*pPCB).index_init_pt;
    (*readyQueue[0]).index_within_fs = (*pPCB).index_within_fs;
    (*readyQueue[0]).index_cur_pt = (*pPCB).index_cur_pt;
    (*readyQueue[0]).num_pages = (*pPCB).num_pages;
    (*readyQueue[0]).pid = (*pPCB).pid;
    (*readyQueue[0]).job_length_score = (*pPCB).job_length_score;
}

bool is_ready_empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        if((*readyQueue[i]).start != -1){
            return false;
        }  
    }
    return true;
}

void terminate_task_in_queue_by_index(int i){
    (*readyQueue[i]).start = -1; 
    (*readyQueue[i]).end = -1; 
    (*readyQueue[i]).PC = -1; 
    (*readyQueue[i]).pid = NULL;
    for(int j = 0; j < 100; j++) {
        (*readyQueue[i]).page_table[j] = -1;
    }
    (*readyQueue[i]).index_init_pt = 0;
    (*readyQueue[i]).index_within_fs = 0;
    (*readyQueue[i]).index_cur_pt = 0;
    (*readyQueue[i]).num_pages = 0;
    (*readyQueue[i]).job_length_score = -1;
}

char* myinit(const char *filename){
    FILE* fp;
    char* error_code = "";
    int* start = (int*)malloc(sizeof(int));
    int* end = (int*)malloc(sizeof(int));
    
    // loads file into backing store
    char* new_file_name = (char*) codeLoading((char*)filename);

    if(fp == NULL){
        error_code = "11"; // 11 is the error code for file does not exist
        return error_code;
    }

    //generate a random ID as file ID
    char* fileID = (char*)malloc(32);
    sprintf(fileID, "%d", rand());

    // error_code = add_file_to_mem(fp, start, end, fileID);
    // if(error_code != 0){
    //     fclose(fp);
    //     return error_code;
    // }
    PCB* newPCB = makePCB(*start,*end,fileID);
    newPCB->job_length_score = 1 + *end - *start;
    newPCB->fileName = strdup(new_file_name);

    ready_queue_add_to_end(newPCB);

    return new_file_name;

}

int get_scheduling_policy_number(char* policy){
    if(strcmp("FCFS",policy)==0){
        return 0;
    }else if(strcmp("SJF",policy)==0){
        return 1;
    }else if(strcmp("RR",policy)==0){
        return 2;
    }else if(strcmp("AGING",policy)==0){
        return 3;
    }else{
        //error code 15
        return 15;
    }
}


/*
 * Variable:  schedulingPolicy 
 * --------------------
 * 0: FCFS
 * 1: SJF
 * 2: RR
 * 3: AGING
 */
int scheduler(int policyNumber){
    int error_code = 0;

    int cpu_quanta_per_program = 2;

    //FCFS and SJF: running program will stop when it finishes
    if( policyNumber == 0 || policyNumber == 1 ){
        cpu_quanta_per_program = MAX_INT;
    }else if(policyNumber == 3){
        cpu_quanta_per_program = 1;
    }

    //scheduling logic for 0: FCFS and 2: RR
    // printContentsOfFrameStore();
    // printContentsOfReadyQueue();
    if(policyNumber == 2){
        //keep running programs while ready queue is not empty
        while(ready_queue_pop(0,false).PC != -1)
        {
            PCB aPCB = ready_queue_pop(0,false);

            // case 2.3
            if(aPCB.page_table[aPCB.index_cur_pt] == -1 && aPCB.index_cur_pt < aPCB.num_pages) {
                int frameStoreIndex = loadPageIntoFrameStore(aPCB.fileName, (aPCB.index_cur_pt)*3);
                if(frameStoreIndex != -1) {
                    aPCB.page_table[aPCB.index_cur_pt] = frameStoreIndex/3;
                } else {
                    int victim_index = evict_LRU();
                    // load into frame store
                    int frame_store_index = loadPageIntoFrameStore(aPCB.fileName, (aPCB.index_cur_pt)*3);
                    aPCB.page_table[aPCB.index_cur_pt] = frame_store_index/3;
                    set_index_LRU(aPCB.page_table[aPCB.index_cur_pt], 0);
                }
                // place at back of ready queue
                ready_queue_pop(0, true);
                ready_queue_add_to_end(&aPCB);
            } else {
            
                int error_code_load_PCB_TO_CPU = cpu_run_3(&aPCB);

                // if good to continue, pop and place at end, don't clear frame store
                ready_queue_pop(0, true);
                if(error_code_load_PCB_TO_CPU == 1) {
                    ready_queue_add_to_end(&aPCB);
                }
                // printContentsOfFrameStore();
            }
            // printContentsOfReadyQueue();
            // printContentsOfFrameStore();
        }
        // printContentsOfReadyQueue();
        mem_init_fs();
        frame_store_LRU_init();
        // printContentsOfFrameStore();
    }

    if(policyNumber == 0){
        //keep running programs while ready queue is not empty
        while(ready_queue_pop(0,false).PC != -1)
        {
            PCB firstPCB = ready_queue_pop(0,false);
            load_PCB_TO_CPU(firstPCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, firstPCB.end);
            
            if(error_code_load_PCB_TO_CPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem(firstPCB.start, firstPCB.end);
                ready_queue_pop(0,true);
            }
            if(error_code_load_PCB_TO_CPU == 0){
                //the head PCB program has finished its quanta, it need to be put to the end of ready queue
                firstPCB.PC = cpu_get_ip();
                ready_queue_pop(0,true);
                ready_queue_add_to_end(&firstPCB);
            }
        }
    }

    //scheduling policy for 1: SJF
    if(policyNumber == 1){
        while(!is_ready_empty())
        {
            //task with the lowest lines of codes runs first
            int task_index_with_the_least_lines;
            int task_lines = MAX_INT;
            //get the lowest job length 
            for(int i = 0; i < QUEUE_LENGTH; i++){
                if((*readyQueue[i]).start != -1 && 1 + (*readyQueue[i]).end - (*readyQueue[i]).start < task_lines){
                    task_lines = 1 + (*readyQueue[i]).end - (*readyQueue[i]).start;
                    task_index_with_the_least_lines = i;
                }
            }

            PCB current_task_PCB = (*readyQueue[task_index_with_the_least_lines]);
            load_PCB_TO_CPU(current_task_PCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, current_task_PCB.end);
            
            //the head PCB program has been done, time to reclaim the shell mem
            clean_mem(current_task_PCB.start, current_task_PCB.end);
            //put the current PCB into invalid mode
            terminate_task_in_queue_by_index(task_index_with_the_least_lines);
        }
    }

    //scheduling policy for 3: Aging
    if(policyNumber == 3){
        int task_index_least_job_length_score;
        int task_job_length_score = MAX_INT;

        //find job with the lowest job score
        for(int i = 0; i < QUEUE_LENGTH; i++){
            //get the lowest job length score
            if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score < task_job_length_score){
                task_job_length_score = (*readyQueue[i]).job_length_score;
                task_index_least_job_length_score = i;
            }
        }
        //move the task with the lowest job score to the front of the queue
        PCB job_with_lowest_job_score = ready_queue_pop(task_index_least_job_length_score,true);
        ready_queue_add_to_front(&job_with_lowest_job_score);
        
        while(!is_ready_empty())
        {
            //task with the lowest job length score runs first
            //in this case, the task with the lowest job length score is the first task in queue
            task_job_length_score = (*readyQueue[0]).job_length_score;
            task_index_least_job_length_score = 0;

            PCB current_task_PCB = (*readyQueue[task_index_least_job_length_score]);
            load_PCB_TO_CPU(current_task_PCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, current_task_PCB.end);

            if(error_code_load_PCB_TO_CPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem((*readyQueue[task_index_least_job_length_score]).start, (*readyQueue[task_index_least_job_length_score]).end);
                ready_queue_pop(task_index_least_job_length_score, true);
                task_job_length_score = MAX_INT;
            }

            if(error_code_load_PCB_TO_CPU == 0){
                //the head PCB program has finished its quanta
                (*readyQueue[task_index_least_job_length_score]).PC = cpu_get_ip(); // update the PC for the PCB
                //Age all the tasks (other than the current executing task) in queue by 1
                for(int i = 0; i < QUEUE_LENGTH; i++){
                    //get the lowest job length score
                    if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score > 0 && i != task_index_least_job_length_score){
                        (*readyQueue[i]).job_length_score -= 1;
                    }
                }
            }
            
            //if the first task job score is not the lowest, 
            //then move the first task to the end 
            //and the lowest job score task to the front
            for(int i = 0; i < QUEUE_LENGTH; i++){
                //get the lowest job length score
                if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score < task_job_length_score){
                    task_job_length_score = (*readyQueue[i]).job_length_score;
                    task_index_least_job_length_score = i;
                }
            }
            if(task_index_least_job_length_score != 0){
                //pop the task with the lowest job score 
                PCB lowest_job_score_task = ready_queue_pop(task_index_least_job_length_score, true);
                //move the frst task to the end
                PCB first_pcb = ready_queue_pop(0, true);
                ready_queue_add_to_end(&first_pcb);
                //move the lowest job score task to the front
                ready_queue_add_to_front(&lowest_job_score_task);
            }
        
        }
    }

    //clean up
    ready_queue_Empty();
    cpu_empty();

    return error_code;
}