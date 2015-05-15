#ifndef PROCESSLIB_H
#define PROCESSLIB_H

#include "spatiallib.h"
//#include "superwavlib.h"

typedef struct SuperWAVTag{
    int leido1;
    int leido2;
    unsigned char **filewav;

} SuperWAV;

typedef struct PlayerTAG{

    long long timeToStart;
    int flag;
    int finishPlaying;
    int IDPlaying;

    pthread_mutex_t lock;

    void **bufferToPlay;

} Player;

double **WFS(double posX, double posY);

char * handleWAVFiles();
SuperWAV loadFile();

#endif


