#ifndef LOG_MALLOC_H
#define LOG_MALLOC_H
#include <stddef.h>

#define malloc log_malloc
#define calloc log_calloc
#define free log_free

void *log_malloc(size_t n);
void *log_calloc(size_t nmemb, size_t n);
void log_free(void *ptr);
void log_malloc_report();

#ifdef LOG_MALLOC_IMPL
#undef malloc
#undef calloc
#undef free
#include <stdio.h>
#include <stdlib.h>

static size_t log_allocation = 0;
static size_t log_allocation_calls = 0;
static size_t log_freedmem = 0;
static size_t log_free_calls = 0;
static size_t log_big_chunk = 0;

void *log_malloc(size_t n) {
    log_allocation += n;
    if(n > log_big_chunk) log_big_chunk = n;
    log_allocation_calls++;
    void *res = malloc(n+sizeof(size_t));
    if(!res) fprintf(stderr, "malloc of %d bytes failed\n",n);
    (((size_t*)res)[0]) = n;
    return &(((size_t*)res)[1]);
}

void *log_calloc(size_t nmemb, size_t n) {
    log_allocation += nmemb*n;
    log_allocation_calls++;
    void *res = calloc(nmemb*n+sizeof(size_t),1); 
    if(!res) fprintf(stderr, "calloc of %d bytes failed\n",nmemb*n);
    (((size_t*)res)[0]) = nmemb*n;
    return &(((size_t*)res)[1]);
}

void log_free(void *ptr) {
    log_free_calls++;
    size_t*p = &(((size_t*)ptr)[-1]);
    log_freedmem += *p;
    free(p);
}

void log_malloc_report() {
    fprintf(stderr, "allocation calls: %d\n", log_allocation_calls);
    fprintf(stderr, "allocated: %d\n", log_allocation);
    fprintf(stderr, "free calls: %d\n", log_free_calls);
    fprintf(stderr, "freed: %d\n", log_freedmem);
    fprintf(stderr, "biggest chunk allocated: %d\n", log_big_chunk);
}

#endif /*LOG_MALLOC_IMPL*/
#endif /*LOG_MALLOC_H*/
