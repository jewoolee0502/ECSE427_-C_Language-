#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <shellmemory.h>

int index = 0;

char* codeLoading(FILE* file) {
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
        return "";
    }

    //copying info into the new file
    char info[1000];
    while(fgets(info, 999, file)) {
        fputs(info, newFile);
    }

    fclose(file);

    index++;

    return directory;
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

int loadFilesIntoFrameStore(char* fileArr) {
    int numInputs = strlen(fileArr);
    int numFiles = 0;
    for(int i = 0; i < numInputs; i++) {
        if(fileArr[i]!=NULL) {
            numFiles = numFiles + 1;
        }
    }
    int counters[numFiles];
    int lengths[numFiles];
    char* fileNames[numFiles];
    for(int i = 0; i < numInputs; i++) {
        if(fileArr[i]!=NULL) {
            FILE *fp = fopen(fileArr[i], "rt");
            char* newName = codeLoading(fp);
            FILE *fp = fopen(newName, "rt");
            fseek(fp, 0L, SEEK_END);
            int size = ftell(fp);
            fseek(fp, 0L, SEEK_SET);    // probably not necessary
            fileNames[numFiles-i] = newName;   // reversed order
            lengths[numFiles-i] = size;
        }
    }
    for(int i = 0; i < numFiles; i++) { // probably not necessary
        counters[i] = 0;
    }
    int notOver = 0;
    while(notOver == 0) {
        int notOverCount = 0;
        for(int i = 0; i < numFiles; i++) {
            char* pageTable[counters[i]/3][500];
            if(counters[i] < lengths[i]) {
                int frameStoreIndex = findFreeFrame();
                FILE *fp = fopen(fileNames[i], "rt");
                loadPageIntoFrameStore(fp, counters[i]/3);

                counters[i]+=3;
            } else {
                notOverCount += 1;
            }
        }
        if(notOverCount == numFiles) {
            break;
        }
    }
    return 0;

}

int loadPageIntoFrameStore(FILE* file, int pageNum) {
    char line[1000];
    char buffer[1000];
    int i = 0;
    
    // go through all the lines in the program
    while(i < pageNum*3) {
        fgets(buffer, 999, file);
        i++;
    }

    int j = 0;
    int index = findFreeFrame();
    if(index!=-1) {
        while(fgets(line, 999, file) && j < 3 && index < 498) {
            mem_set_value_fs(index, strdup(line));
            index++;
            j++;
        }
        return 0;

    }
    return 1;
}

int findFreeFrame() {

    int i = 0;
    while(i < 500) {    // as 500 is the length of the array
        // proceed until end of frame store is reached
        // if frame (i) is empty, found is true, return i
        // else i*3
        if(strcmp(mem_get_value_by_line_fs(i), "none") == 0) {
            return i;
        }
        i = i+3;
    }

    return -1;
}
