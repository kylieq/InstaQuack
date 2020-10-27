#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_ops.h"

struct FileLines *LoadAFile(char *filename) {
    FILE *file = fopen(filename,"r");
    struct FileLines *lines = NULL;
    char buffer[300];
    int counter = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        counter += 1;   
    }

    fseek(file, 0, SEEK_SET);

    lines = (struct FileLines *)malloc(sizeof(struct FileLines));
    lines->FileName = strdup(filename);
    lines->LineCount = counter;
    lines->Lines = (char **)malloc((lines->LineCount+1) * sizeof(char *));
    lines->Lines[lines->LineCount] = NULL;

    for (int i=0; i<lines->LineCount; i++) {
        fgets(buffer, sizeof(buffer), file);
        int len = strlen(buffer);
        for (int j=0; j<len; j++) {
            char c = buffer[j];
            if (c == '\n' || c == '\r') {
                buffer[j] = 0;
            }
        }
        lines->Lines[i] = strdup(buffer);
    }

    fclose(file);
    return lines;
}

void FreeFile(struct FileLines *file) {
    free(file->FileName);             
    for (int i=0; i < file->LineCount; i++) {
        if ((file->Lines[i]) != NULL) {
            free(file->Lines[i]);
        }
    }    
    free(file->Lines); 
    free(file);         
}

