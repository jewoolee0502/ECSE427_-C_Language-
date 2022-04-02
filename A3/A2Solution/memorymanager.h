#ifndef MEM_MAN_H

#define MEM_MAN_H

#include <stdio.h>

int codeLoading(FILE* file);
char* generateFileName(int index);
int resetIndex();
int loadProgramPages(FILE* file, int pageNum, int frameNum);
int findFreeFrame();

#endif