#include "cpu.h"
#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"
#include "kernel.h"
#include "memorymanager.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Struct:  CPU 
 * --------------------
 * IP: serve as a pointer to shell memory. Ex. IP = 101 means CPU is executing the 101th line in shellmemory
 * IR: stores the line of code that CPU is executing
 * quanta: how many lines of code could the current task run until it finishes or being switched to another task
 */
struct CPU
{
    int IP;
    char IR[1000];
    int quanta;
};
struct CPU aCPU = {.quanta =0};

int cpu_get_ip(){
    return aCPU.IP;
}

void cpu_set_ip(int pIP){
    aCPU.IP = pIP;
}

void cpu_empty(){
    aCPU.quanta = 2;
}

void load_PCB_TO_CPU(int PC){
    cpu_set_ip(PC);
}

/*
 * Function:  cpu_run 
 * 	Added in A2
 * --------------------
 * run "quanta"(first input parameter) lines of code, starting from the cpu.IP
 *
 * quanta: number of lines that the CPU will run before it switches task or the task ends
 * end: the last line of the task in shell memory
 * returns: error code, 2: file reaches an end, 0: no error
 */
int cpu_run(int quanta, int end){
    aCPU.quanta = quanta;
    int error_code;
    while (aCPU.quanta != 0){
        
        strncpy(aCPU.IR, mem_get_value_by_line(aCPU.IP),1000);
        parseInput(aCPU.IR);
        if(end == aCPU.IP){
            error_code = 2;
            return error_code;
        }
        aCPU.IP = aCPU.IP + 1;
        aCPU.quanta -= 1;
    }
    
    error_code = 0;
    return error_code;
}

int cpu_run_2(PCB *aPCB) {
    int quanta = 2;
    int signal = 0;
    // printContentsOfReadyQueue();
    while(quanta != 0) {
        char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
        if(strcmp(line, "none") != 0) {
            increment_LRU();
            set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);
            printf("Here1 printing line %d\n", aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
            parseInput(line);
            // printLRUContents();
            aPCB->index_within_fs+=1;
            if(aPCB->index_within_fs > 2) {
                printf("Here2\n");
                // now need to check if end of program
                signal = 1;
                break;
            }
        } else {
            // still inside but done -> meaning end of program
            return 3;
        }
        quanta -= 1;
    }
    if(signal == 1) {
        // get next page
        aPCB->index_cur_pt += 1;
        printf("index cur pt: %d, num lines: %d\n", aPCB->index_cur_pt, aPCB->num_pages);
        if(aPCB->index_cur_pt == aPCB->num_pages) {
            printf("Here4\n");
            // end of page table -> meaning end of program. index_within_fs doesn't matter anymore
            return 3;
        } else {
            // The current process P is interrupted and placed at the back of the ready queue, even if it may still have code lines left in its “time slice”
            aPCB->index_within_fs = 0;
            if(quanta == 2) {   // if only executed once
                if(aPCB->page_table[aPCB->index_cur_pt] == -1) {
                    int frameStoreIndex = loadPageIntoFrameStore(aPCB->fileName, (aPCB->index_cur_pt)*3);
                    if(frameStoreIndex != -1) {
                        aPCB->page_table[aPCB->index_cur_pt] = frameStoreIndex/3;
                    } else {
                        int victimFrameNumber = evict_LRU();
                        frameStoreIndex = loadPageIntoFrameStore(aPCB->fileName, (aPCB->index_cur_pt)*3);
                        if(victimFrameNumber != frameStoreIndex)  {
                            printf("victim: %d, frstindex: %d\n", victimFrameNumber, frameStoreIndex);
                        }
                        aPCB->page_table[aPCB->index_cur_pt] = victimFrameNumber/3;
                    }
                } else {
                    char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
                    aPCB->index_within_fs+=1;
                    increment_LRU();
                    set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);
                    parseInput(line);
                    // printLRUContents();
                }
            } else {
                aPCB->index_cur_pt+=1;
            }
            return 2;
        }
    } else {
        // if(aPCB->index_cur_pt+1 == aPCB->num_pages) {
        //     aPCB->index_cur_pt+=1;
        //     return 3;
        // }
        // still within frame; do not increment aPCB->index_cur_pt
        return 1;
    }
}

int cpu_run_3(PCB *aPCB) {
    // if the index exists at this point, it means the program must have a line to run.
    if(aPCB->index_within_fs == 0) {
        // case 1
        for(int i = 0; i < 2; i++) {
            char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
            if(strcmp(line, "none") == 0) {
                // case 1.1
                return 2;
            } else {
                parseInput(line);
                increment_LRU();
                set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);
                aPCB->index_within_fs+=1;
            }
        }
        // after lines are run, check if done or not
        aPCB->index_within_fs = 2;
        char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
        if(strcmp(line, "none") == 0) {
            // case 1.2
            return 2;
        } else {
            // case 1.3
            return 1;
        }

    } else if(aPCB->index_within_fs == 1) {
        // case 2; in this case, the num_pages is considered
        for(int i = 0; i < 2; i++) {
            char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
            if(strcmp(line, "none") == 0) {
                // case 2.1
                return 2;
            } else {
                parseInput(line);
                increment_LRU();
                set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);
                aPCB->index_within_fs+=1;
            }
        }
        // the rest of this is checked in the kernel
        aPCB->index_cur_pt += 1;
        aPCB->index_within_fs = 0;
        if(aPCB->index_cur_pt == aPCB->num_pages) {
            // case 2.2
            return 2;
        } else {
            // case 2.3 -> IMPORTANT: Need to check in CPU to load it
            return 1;
        }

    } else if(aPCB->index_within_fs == 2) {
        // case 3; in this case, we have to get the next page.
        char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
        parseInput(line);
        increment_LRU();
        set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);

        aPCB->index_cur_pt += 1;
        aPCB->index_within_fs = 0;

        if(aPCB->index_cur_pt == aPCB->num_pages) {
            // case 3.1
            return 2;
        }

        // cases 3.2 and 3.3
        if(aPCB->page_table[aPCB->index_cur_pt] != -1) {    // if in frame store
            char* line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
            parseInput(line);
            increment_LRU();
            set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);
        } else {
            // load into framestore (by any means. if not loaded, evict). Then, DO NOT execute next command.
            int frame_store_index = loadPageIntoFrameStore(aPCB->fileName, (aPCB->index_cur_pt)*3);
            if(frame_store_index == -1) {   // if not found
                int victim_index = evict_LRU();
                frame_store_index = loadPageIntoFrameStore(aPCB->fileName, (aPCB->index_cur_pt)*3);
                aPCB->page_table[aPCB->index_cur_pt] = frame_store_index/3;
            }
            set_index_LRU(aPCB->page_table[aPCB->index_cur_pt], 0);
            return 1;
        }
        // after this, check if the next line is not "none" -> if it is, return 2, otherwise, return 1
        aPCB->index_within_fs += 1;
        line = mem_get_value_by_line_fs(aPCB->page_table[aPCB->index_cur_pt]*3 + aPCB->index_within_fs);
        if(strcmp(line, "none") == 0) {
            // case 3.2
            return 2;
        } else {
            // case 3.1
            return 1;
        }

    } else {
        printf("ERROR\n");
    }
}