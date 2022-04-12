#include "stdlib.h"
#include "stdio.h"

void printLRUContents();
void frame_store_LRU_init();
void increment_LRU();
int get_LRU_index();
int get_LRU_index_other_than_index(int index);
void set_index_LRU(int index, int toSet);
void mem_init_vs();
void mem_init_fs();
void printContentsOfFrameStore();
char *mem_get_value(char *var);
char* mem_get_value_by_line(int line);
void mem_set_value(char *var, char *value);
void mem_set_value_fs(int index, char *value_in);
char* mem_get_value_fs(char *var_in);
char* mem_get_value_by_line_fs(int line);
void clean_mem(int start, int end);
void clean_mem_fs(int start, int end);
void clean_mem_fs_and_print(int start, int end);
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID);
