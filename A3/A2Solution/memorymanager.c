#include "memorymanager.h"
#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

int index_;

char* codeLoading(char* file) {
    //create a file in the Backing_Store
    char* generatedName = generateFileName(index_);

    FILE *fptr1, *fptr2;
    char c;
    fptr1 = fopen(file, "r");
    if (fptr1 == NULL)
    {
        printf("Cannot open file %s \n", file);
        return "11";
    }
  
    // Open another file for writing
    fptr2 = fopen(generatedName, "w");
    if (fptr2 == NULL)
    {
        printf("Cannot open file %s \n", generatedName);
        return "11";
    }
  
    // Read contents from file
    c = fgetc(fptr1);
    while (c != EOF)
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }
    fclose(fptr1);
    fclose(fptr2);

    index_ += 1;

    return generatedName;
}

char* generateFileName(int indice) {
    char* name = malloc(sizeof(char) * 50);
    strcpy(name, "Backing_Store/file");   //copying "file" into string name

    char number[100];
    sprintf(number, "%d", indice);   //converting int to char
    
    strcat(name, number);   //concatenating two char
    return name;
}

int resetIndex() {
    index_ = 0;  //set index = 0
    return 0;
}

int loadFilesIntoFrameStore(char* fileArr[]) {
    int numFiles = 0;
    for(int i = 0; i < 3; i++) {
        if(fileArr[i] != NULL) {
            numFiles++;
        }
    }
    int counters[numFiles];
    int lengths[numFiles];
    char* fileNames[3];
    for(int i = 0; i < 3; i++) {
        if(fileArr[i] != NULL) {
            // get number of lines in file

            FILE *fileptr;
            char chr;
            int count_lines = 0;
            fileptr = fopen(fileArr[i], "rt");
            //extract character from file and store in chr
            chr = getc(fileptr);
            while (chr != EOF)
            {
                //Count whenever new line is encountered
                if (chr == '\n') {
                    count_lines++;
                }
                //take next character from file.
                chr = getc(fileptr);
            }
            fclose(fileptr); //close file.
            // end this process

            fileNames[numFiles-i-1] = (char*)malloc(sizeof(char) * (strlen(fileArr[i]) + 1 ) );
            strcpy(fileNames[numFiles-i-1], fileArr[i]);   // reversed order
            lengths[numFiles-i-1] = count_lines+1;
        }
    }
    for(int i = 0; i < numFiles; i++) { // probably not necessary
        counters[i] = 0;
        for(int j = 0; j < 10; j++) {
            PCB* curPCB = get_ready_queue_at(j);
            if(curPCB->pid == NULL) {
                continue;
            }
            if(curPCB->fileName != NULL && strcmp(curPCB->fileName, fileNames[i]) == 0) {
                // printf("Index init pt: %d | Frame store index: %d \n", curPCB->index_init_pt, frameStoreIndex/3);
                int val = (lengths[i] / 3) + ((lengths[i] % 3) != 0);
                curPCB->num_pages = val;
            }
        }
        // also, set in PCB
    }
    int frameStoreIndex = 0;
    int notOverCount = 0;
    int totalLoops = 2;
    for(int count = 0; count < totalLoops; count++) {
        notOverCount = 0;
        for(int i = 0; i < numFiles; i++) {
            if(counters[i] < lengths[i]) {
                int frameStoreIndex = loadPageIntoFrameStore(fileNames[i], counters[i]);
                // printf("frameStoreIndex: %d\n", frameStoreIndex);
                if(frameStoreIndex == -1) {
                    return 1;
                }
                counters[i] += 3;
                // get PCB, add to pagetable
                for(int j = 0; j < 10; j++) {
                    PCB* curPCB = get_ready_queue_at(j);
                    if(curPCB->pid == NULL) {
                        continue;
                    }
                    if(curPCB->fileName != NULL && strcmp(curPCB->fileName, fileNames[i]) == 0) {
                        // printf("Index init pt: %d | Frame store index: %d \n", curPCB->index_init_pt, frameStoreIndex/3);
                        curPCB->page_table[curPCB->index_init_pt] = frameStoreIndex/3;
                        curPCB->index_init_pt = curPCB->index_init_pt+1;
                    }
                }
            } else {
                notOverCount += 1;
            }
        }
        if(notOverCount == numFiles) {
            break;
        }
    }
    // printContentsOfPageTable();
    return 0;

}

void printContentsOfPageTable() {
    for(int j = 0; j < 10; j++) {
        PCB* curPCB = get_ready_queue_at(j);
        if(curPCB->pid == NULL) {
                    continue;
        }
        printf("PID: %s \n", curPCB->pid);
        printf("FILE NAME: %s \n", curPCB->fileName);
        printf("PAGE TABLE[0]: %d \n", curPCB->page_table[0]);
        printf("PAGE TABLE[1]: %d \n", curPCB->page_table[1]);
        printf("PAGE TABLE[2]: %d \n", curPCB->page_table[2]);
        printf("PAGE TABLE[3]: %d \n", curPCB->page_table[3]);
    }
    return;
}

int findFreeFrame() {
    int i = 0;
    while(i < FRAMESIZE) {    // as framesize is the length of the array
        if(strcmp(mem_get_value_by_line_fs(i), "none") == 0) {
            return i;
        }
        i = i+3;
    }
    return -1;
}

int loadPageIntoFrameStore(char* filename, int pageNum) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return 1;
    }
    char line[1000];
    char buffer[1000];
    int i = 0;

    // go through all the lines in the program
    while(i < pageNum) {
        fgets(buffer, 999, file);
        i++;
    }
    int j = 0;
    int cur_index = findFreeFrame();
    int toReturn = cur_index;
    // printf("%d", cur_index);
    if(cur_index != -1) {
        while(fgets(line, 999, file) && j < 3 && cur_index < FRAMESIZE) {
            mem_set_value_fs(cur_index, strdup(line));
            cur_index++;
            j++;
        }
    }
    fclose(file);
    // return -1 if no free frame found (doesn't load in page either)
    return toReturn;

}

// sourced from: https://stackoverflow.com/questions/2509679/how-to-generate-a-random-integer-number-from-within-a-range
unsigned int random_number(unsigned int min, unsigned int max)
{
    int r;
    const unsigned int range_of_nums = 1 + max - min;
    const unsigned int num_buckets = RAND_MAX / range_of_nums;
    const unsigned int limit = num_buckets * range_of_nums;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / num_buckets);
}

int evict_random() {
    int victimFrameNumber = random_number(0, ((const unsigned int)floor((double) FRAMESIZE / 3)-1))*3;    
    clean_mem_fs_and_print(victimFrameNumber, victimFrameNumber + 3);
    return victimFrameNumber;

}

int evict_LRU() {
    int victimFrameNumber = get_LRU_index()*3;
    // printf("VFN: %d\n", victimFrameNumber);
    clean_mem_fs_and_print(victimFrameNumber, victimFrameNumber + 3);
    return victimFrameNumber;

}