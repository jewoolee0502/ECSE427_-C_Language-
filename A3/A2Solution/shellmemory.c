#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

struct memory_struct{
	char *var;
	char *value;
};

// struct memory_struct shellmemory[SHELL_MEM_LENGTH];

struct memory_struct frameStore[FRAMESIZE]; 
struct memory_struct variableStore[VARMEMSIZE];
int frameStoreLRU[FRAMESIZE/3];

void printLRUContents() {
	for(int i = 0; i < FRAMESIZE/3; i++) {
		printf("%d, ", frameStoreLRU[i]);
	}
	printf("\n");
}

// Shell memory functions

void frame_store_LRU_init() {
	for(int i = 0; i < FRAMESIZE/3; i++) {
		frameStoreLRU[i] = 0;
	}
	
}

int get_LRU_index() {
	int most = 0;
	int toReturn = 0;
	for(int i = 0; i < FRAMESIZE/3; i++) {
		if(frameStoreLRU[i] >= most) {
			// printf("FrameStoreLRU[i]: %d, Most: %d\n", frameStoreLRU[i], most);
			most = frameStoreLRU[i];
			toReturn = i;
			// printf("After... FrameStoreLRU[i]: %d, Most: %d\n\n", frameStoreLRU[i], most);
		}
	}
	// printf("Most: %d\n", most);
	return toReturn;
}

int get_LRU_index_other_than_index(int index) {
	int most = 0;
	for(int i = 0; i < FRAMESIZE/3; i++) {
		if(frameStoreLRU[i] >= most && i != index) {
			most = i;
		}
	}
	return most;
}

void increment_LRU() {
	for(int i = 0; i < FRAMESIZE/3; i++) {
		if(strcmp(frameStore[i*3].value, "none") != 0) {
			frameStoreLRU[i] += 1;
		}
	}
}

void set_index_LRU(int index, int toSet) {
	frameStoreLRU[index] = toSet;
}

void mem_init_vs(){

	int i;
	for (i = 0; i < VARMEMSIZE; i++){		
		variableStore[i].var = "none";
		variableStore[i].value = "none";
	}
}

void mem_init_fs(){

	int i;
	for(i = 0; i < FRAMESIZE; i++) {
		frameStore[i].var = "none";
		frameStore[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<VARMEMSIZE; i++){
		if (strcmp(variableStore[i].var, var_in) == 0){
			variableStore[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<VARMEMSIZE; i++){
		if (strcmp(variableStore[i].var, "none") == 0){
			variableStore[i].var = strdup(var_in);
			variableStore[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

// Set key value pair
void mem_set_value_fs(int index, char *value_in) {
	frameStore[index].value = strdup(value_in);
}

//get value based on input key - variable store
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<VARMEMSIZE; i++){
		if (strcmp(variableStore[i].var, var_in) == 0){
			return strdup(variableStore[i].value);
		} 
	}
	return "Variable does not exist";

}

char* mem_get_value_by_line(int line){
	return variableStore[line].value;
}

char* mem_get_value_by_line_fs(int line){
	return frameStore[line].value;
}

void printContentsOfFrameStore() {
	for(int i = 0; i < FRAMESIZE; i++) {
		printf("Frame store[%d]: %s\n", i, mem_get_value_by_line_fs(i));
	}
}

void clean_mem(int start, int end){
    for(int i = start; i <= end; i ++){
        variableStore[i].var = "none";
		variableStore[i].value = "none";
    }
}

void clean_mem_fs(int start, int end){
    for(int i = start; i < end; i ++){
        frameStore[i].var = "none";
		frameStore[i].value = "none";
    }
}

void clean_mem_fs_and_print(int start, int end){
	// printContentsOfFrameStore();
	printf("%s\n", "Page fault! Victim page contents:");
    for(int i = start; i < end; i ++){
		if(strcmp(frameStore[i].value, "none") != 0) {
			if(frameStore[i].value[strlen(frameStore[i].value)-1]!='\n') {
				printf("%s\n", frameStore[i].value);
			} else {
				printf("%s", frameStore[i].value);
			}
		}
        frameStore[i].var = "none";
		frameStore[i].value = "none";
    }
	printf("%s\n", "End of victim page contents.");
}

/*
 * Function:  addFileToMem (Deprecated)
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first 100 lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID)
{
    char line[VARMEMSIZE];
    size_t i;
    int error_code = 0;
	bool hasSpaceLeft = false;

    for (i = 100; i < VARMEMSIZE; i++){
        if(strcmp(frameStore[i].var,"none") == 0){
            *pStart = (int)i;
			hasSpaceLeft = true;
            break;
        }
    }

	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}
    
    for (size_t j = i; j < VARMEMSIZE; j++){
        if(feof(fp))
        {
            *pEnd = (int)j-1;
            break;
        }else{
            fgets(line, 999, fp);
			frameStore[j].var = strdup(fileID);
            frameStore[j].value = strdup(line);
        }
    }

	//no space left to load the entire file into shell memory
	if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		for(int j = 1; i <= VARMEMSIZE; i ++){
			frameStore[j].var = "none";
			frameStore[j].value = "none";
    	}
		return error_code;
	}

    return error_code;
}