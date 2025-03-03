#ifndef _INCLUDE_FUNCTIONS_H
#define _INCLUDE_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

/*Kbit arate*/
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include <math.h>
// for gettimeofday()
#include <sys/time.h>
//#include <alsa/asoundlib.h>

//for true and false
#include <stdbool.h>
#define TRUE   1
#define FALSE  0

//Thread
#include <pthread.h>

long long current_timestamp();
void error(const char *msg);
int kbhit(void);

int getMaxInt(int a[], int size);
float getMaxFloat(float a[], int size);


#endif