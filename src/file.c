// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "../include/utils.h"
//#include "../include/structures.h"
#include "../include/track.h"
#include "../include/status.h"
#include "../include/print.h"
#define BUFFER_SIZE 8192



int main(int argc,char** argv) {
    int err;

    log_message("first arg is: %s",argv[0]);
    if (strcmp(argv[1],"track") == 0){
        log_message("running track");
        process_track(argc,argv);
    } else if (strcmp(argv[1],"status") == 0){
        log_message("running status");
        process_status(argc,argv);
    }
}
