#ifndef __STRVEC_H__
#define __STRVEC_H__

#include <stdlib.h>
#include <stdbool.h>
#include "../error.h"
#include <stdio.h>
#include <string.h>
#include "../global.h"
#include <pthread.h>
#include <stdarg.h>

typedef struct strvec{
    char** v;
    size_t len;
    size_t alloc;
    pthread_mutex_t* lock;
} strvec;

#define STRVEC_DEFAULT_INIT 30

#define STRVEC_DEFAULT_INCREMENT 10


struct strvec* strvec_init();

void strvec_push(struct strvec* vec,const char* s);

void strvec_pop(struct strvec* vec);

void strvec_free(struct strvec** vec);

void strvec_pushf(struct strvec* vec,const char* format,...);

#endif