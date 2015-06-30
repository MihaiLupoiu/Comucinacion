//
// Created by mihai on 24/06/15.
//

#ifndef COMUCINACION_PARSHER_H
#define COMUCINACION_PARSHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MesageVariablesTag{
    char* action;
    int identification;
    long long startTime;
    int clientPosX;
    int clientPosY;
    int song;
    int songPosX;
    int songPosY;

} MesageVariables;

char **strsplit(const char* str, const char* delim, size_t* numtokens);
MesageVariables processMesage(char* str);



#endif //COMUCINACION_PARSHER_H
