// uses: PCB, ready queue
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"

ready queue initialized


void schedule(char *files[], char *policy) {

    while (*files != "") {  // iterate through all files (as strings)
        // open the file
        // line by line,
        // get the instruction at that line, place into memory
        // e.g. mem_set_value(1, instruction);
        // when done with the file, set new start to i
        printf ("%s\n",  *files++);

    }

    // schedule is FCFS
    // load the file into memory
    // put the file into a PCB
    // put the PCB into a ready queue
    // with respect to the policy, schedule accordingly
/*
    input: list of opened files (file_list). Takes up to three files
    input: policy

    switch based on the policy, default is FCFS
        switch based on length of file_list
        if two ... FCFS
        if three ... check policy
        if four ... check policy
    
    opens the file
    loads the file line by line into memory with line_no : instruction as k-v pairs
    creates a pcb...
        start: address of memory allocated, length: size of memory allocated
    creates a ready queue
    puts the pcb into a ready queue 
*/
}