#include "../include/stdutils.h"

bool std_is_valid_integer(char* c){
    char *endptr;
    strtol(c, &endptr, 10);
    if (*endptr == '\0') return true;

    return false;
}

bool std_string_to_num(char* c,long* l){
    char* endptr;
    *l = strtol(c,&endptr,10);
    if (*endptr == '\0') return true;
    return false;
}

void std_append_chars(char *dst,size_t* index,char* src,size_t len){
    memcpy(dst + *index,src,len);

    *index = *index + len;
}