#ifndef __STDUTILS__
#define __STDUTILS__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool std_is_valid_integer(char* c);

bool std_string_to_num(char* c,long* l);

void std_append_chars(char *dst,size_t* index,char* src,size_t len);


#endif