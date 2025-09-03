#include "../../include/structs/strvec.h"

struct strvec* strvec_init(){

    struct strvec* vec = (struct strvec *) malloc(sizeof(struct strvec));

    vec->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if (!vec || !vec->lock){
        fprintf(stderr,__ERROR_MEMALLOC__);
        exit(1);
    }
    pthread_mutex_init(vec->lock, NULL);

    pthread_mutex_lock(vec->lock);

    vec->v = (char **)malloc(sizeof(char *)*STRVEC_DEFAULT_INIT);

    if (!vec->v){
        fprintf(stderr,__ERROR_MEMALLOC__);
        exit(1);
    }
    
    vec->alloc = STRVEC_DEFAULT_INIT;
    vec->len = 0;

    pthread_mutex_unlock(vec->lock);

    return vec;
}

void strvec_push(struct strvec* vec,const char* s){
    if (!vec) return;

    pthread_mutex_lock(vec->lock);

    if (vec->len < vec->alloc){
        vec->v[vec->len] = strdup(s);
        vec->len++;

        pthread_mutex_unlock(vec->lock);

        return;
    }

    char** dang = vec->v;

    size_t sz = vec->alloc;

    vec->v = (char **) malloc(sizeof(char *) * (sz + STRVEC_DEFAULT_INCREMENT));

    size_t i=0;

    for (i = 0;i<sz;++i){
        vec->v[i] = strdup(dang[i]);

        __RW_MEMFREE__(dang[i]);
    }

    vec->v[i] = strdup(s);

    vec->alloc = sz + STRVEC_DEFAULT_INCREMENT;
    vec->len = sz + 1;

    pthread_mutex_unlock(vec->lock);
}

void strvec_pop(struct strvec* vec){
    if (!vec) return;

    pthread_mutex_lock(vec->lock);

    if (vec->len == 0){
        pthread_mutex_unlock(vec->lock);

        return;
    }

    __RW_MEMFREE__(vec->v[vec->len-1]);

    vec->len--;

    pthread_mutex_unlock(vec->lock);
}

void strvec_free(struct strvec** vec){
    if (!*vec) return;
    
    for (size_t i=0;i<(*vec)->len;++i){
        __RW_MEMFREE__((*vec)->v[i]);
    }

    pthread_mutex_destroy((*vec)->lock);

    __RW_MEMFREE__((*vec));

    *vec = NULL;

    return;
}

void strvec_pushf(struct strvec* vec,const char* format,...){
    if (!vec) return;

    pthread_mutex_lock(vec->lock);

    if (vec->len < vec->alloc){
        va_list args;
        va_start(args, format);
        
        int size = vsnprintf(NULL, 0, format, args);
        va_end(args);

        vec->v[vec->len] = (char *)malloc((size_t) size+1);

        va_start(args, format);
        vsnprintf(vec->v[vec->len], (size_t) size+1, format, args);
        va_end(args);
        vec->len++;

        pthread_mutex_unlock(vec->lock);
        return;
    }

    char** dang = vec->v;
    size_t sz = vec->alloc;

    vec->v = (char **) malloc(sizeof(char *) * (sz + STRVEC_DEFAULT_INCREMENT));

    size_t i=0;

    for (i = 0;i<sz;++i){
        vec->v[i] = strdup(dang[i]);

        __RW_MEMFREE__(dang[i]);
    }

    va_list args;
    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    vec->v[i] = (char *)malloc(sizeof(char)* ((size_t)size+1));
    va_start(args, format);
    vsnprintf(vec->v[vec->len], (size_t)size+1, format, args);
    va_end(args);


    vec->alloc = sz + STRVEC_DEFAULT_INCREMENT;
    vec->len = sz + 1;

    pthread_mutex_unlock(vec->lock);

    return;
}