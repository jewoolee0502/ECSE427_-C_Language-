#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

char* codeLoading(char* file);
char* generateFileName(int indice);
int evict_LRU();
int resetIndex();
int loadFilesIntoFrameStore(char* fileArr[]);
int findFreeFrame();
void printContentsOfPageTable();
int loadPageIntoFrameStore(char* filename, int pageNum);
unsigned int random_number(unsigned int min, unsigned int max);
int evict_random();