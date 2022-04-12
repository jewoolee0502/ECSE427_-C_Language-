#ifndef PCB_H
#define PCB_H

/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: program counter, stores the line that the task is executing
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
 */
typedef struct
{
    char* pid;
    int PC;
    int start;
    int end;
    int job_length_score;
    char* fileName;
    int page_table[100];
    int index_init_pt;
    int index_within_fs;
    int index_cur_pt;
    int num_pages;
}PCB;

PCB * makePCB(int start, int end, char* pid);
#endif
