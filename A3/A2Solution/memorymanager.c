#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <shellmemory.h>

int index = 0;

int codeLoading(FILE* file) {
    //errCode = 0 success, errCode = 1 failure
    int errCode = 0;

    //create a file in the Backing_Store
    char* fileName = generateFileName(index);
    char* directory = "Backing_Store/";
    strcat(directory, fileName);

    printf("New file, %s, is created in %s \n", fileName, directory);

    //open the new file
    FILE* newFile = fopen(directory, "w");  //w for opening an empty file

    //when the file is null
    if(!newFile) {
        printf("error: The file %s cannot be opened in the directory: %s", fileName, directory);
        errCode = 1;
        return errCode;
    }

    //copying info into the new file
    char info[1000];
    while(fgets(info, 999, file)) {
        fputs(info, newFile);
    }

    fclose(file);

    index++;

    return errCode;
}

char* generateFileName(int index) {
    char* name = malloc(sizeof(char) * 50);
    strcpy(name, "file");   //copying "file" into string name

    char number[100];
    sprintf(number, "%d", index);   //converting int to char
    
    strcat(name, number);   //concatenating two char
    return name;
}

int resetIndex() {
    index = 0;  //set index = 0
    return 0;
}

int loadProgramPages(FILE* file, int pageNum, int frameNum) {
    char line[1000];
    char buffer[1000];
    char frame[1000];
    int i = 0;
    int j = 0;
    int index = frameNum*3;
    
    while(i < pageNum*3) {
        fgets(buffer, 999, file);
        i++;
    }
    
    while(fgets(line, 999, file) && j < 3) {
        frame[index] = strdup(line);
        index++;
        j++;
    }
}

int findFreeFrame() {
    //errCode = 0 success, errCode = 1 failure
    int errCode = 0;

    int i = 0;
    while(i < 500) {

    }

    return errCode;
}