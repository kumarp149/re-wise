#ifndef __REGEX__
#define __REGEX__

#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


/*
 * * Matches any number of characters (including /)
 * ? Matches a single character
 * [abc] Matches one character in the set
 * **\/	Matches directories recursively
 * !pattern	Negates a pattern
 * 
*/
bool regex_is_matching(char* pattern,char* word);


#endif