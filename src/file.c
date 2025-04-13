// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "../include/utils.h"
//#include "../include/structures.h"
#include "../include/track.h"
#define BUFFER_SIZE 8192



int main(int argc,char** argv) {
    int err;

    if (strcmp(argv[1],"track") == 0){
        //printf("argv[1] is track\n");
        process_track(argc,argv);
    }
}
