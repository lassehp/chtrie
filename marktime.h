#ifndef MARKTIME_H
#define MARKTIME_H

void resettime();
void marktime();
void logtime(char*s);
void print_timemarks();

#ifdef MARKTIME_IMPL
#include <time.h>
#include <stdio.h>

#define MAX_SAVE_MARKS 100
clock_t timemark[MAX_SAVE_MARKS];
size_t n_timemark = 0;

void resettime() {
    n_timemark = 1;
}

void marktime() {
    if(n_timemark < MAX_SAVE_MARKS) timemark[n_timemark++] = clock();
    else fprintf(stderr,"cannot save more time marks\n");
}

void logtime(char*s) {
    if(n_timemark < MAX_SAVE_MARKS) timemark[n_timemark++] = clock();
    else fprintf(stderr,"cannot save more time marks\n");
    int i = n_timemark-1;
    fprintf(stderr, "[%d] = %12.6f µs %12.6f µs -- %s\n", i, 1000000.0*((float)timemark[i])/CLOCKS_PER_SEC,
        1000000.0*((float)(timemark[i]-timemark[i?i-1:0]))/CLOCKS_PER_SEC, s);
}


void print_timemarks() {
    printf("Mark   Abs time    ∆time\n");
    for(int i=0; i < n_timemark; i++) {
        printf("[%d] = %12.6f µs %12.6f µs\n", i, 1000000.0*((float)timemark[i])/CLOCKS_PER_SEC,
            1000000.0*((float)(timemark[i]-timemark[i?i-1:0]))/CLOCKS_PER_SEC);
    }
}


#endif /*MARKTIME_IMPL*/

#endif /*MARKTIME_H*/
