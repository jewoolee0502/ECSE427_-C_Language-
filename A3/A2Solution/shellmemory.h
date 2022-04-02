#include "stdlib.h"
#include "stdio.h"

void mem_init();
char *mem_get_value(char *var);
char* mem_get_value_by_line(int line);
void mem_set_value(char *var, char *value);
void mem_set_value_fs(char *var_in, char *value_in);
char* mem_get_value_fs(char *var_in);
char* mem_get_value_by_line_fs(int line);
void clean_mem(int start, int end);
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID);
