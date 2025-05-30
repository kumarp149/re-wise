// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "../include/utils.h"
//#include "../include/structures.h"
#include "../include/track.h"
#include "../include/status.h"
#include "../include/print.h"
#include "../include/regex.h"
#define BUFFER_SIZE 8192



int main(int argc,char** argv) {

    log_message("first arg is: %s",argv[0]);
    if (strcmp(argv[1],"track") == 0){
        log_message("running track");
        process_track(argc,argv);
    } else if (strcmp(argv[1],"status") == 0){
        log_message("running status");
        process_status(argc,argv);
    } else if (strcmp(argv[1],"commit") == 0){
        //log_message(argc,argv);
        log_message("running commit");
        process_commit(argc,argv);
    } else{
        printf("%d\n",regex_is_matching("abc/pqr","abc/pqr")); //1
        printf("%d\n",regex_is_matching("a*","ab")); //1
        printf("%d\n",regex_is_matching("a?","ab")); //1
        printf("%d\n",regex_is_matching("a?","abc")); //0
        printf("%d\n",regex_is_matching("?a?*","paq")); //1
        printf("%d\n",regex_is_matching("*a?*?*","abccccca")); //1
        printf("%d\n",regex_is_matching("*a?[abcd]","pqraxd")); //1
        printf("%d\n",regex_is_matching("*a?[abcd]","pqraxe")); //0
        printf("%d\n",regex_is_matching("*a?[abcd]**/pqr","pqraxd/xyz/klm/pqr")); //1
        printf("%d\n",regex_is_matching("*a?[abcd]**/pqr","pqraxd/xyz/klm/pqr")); //1
        printf("%d\n",regex_is_matching("*a?[abcd]**/pqr","pqraxd/xyz/")); //0
        printf("%d\n",regex_is_matching("*a?[abcd]**/pqr/**/xyz","pqraxd/xyz/klm/pqr/klm/klm/klm/klm/klm/klm/klm/klm/klm/klm/klm/klm/xyz")); //1
        printf("%d\n",regex_is_matching("*a*?","ab")); //1
        printf("%d\n",regex_is_matching("*a*[abcd]?","aaaaa")); //1
        printf("%d\n",regex_is_matching("*a*[abcd]?","apq")); //0
        printf("%d\n",regex_is_matching("*a*[abcd]**/**/**/?","pqraa/klm/klm/klm")); //0
        printf("%d\n",regex_is_matching("*a*[abcd]**/**/**/?","pqraa/klm/klm/klma")); //0
        printf("%d\n",regex_is_matching("*a*[abcd]**/**/**/?","pqraa/klm/klm/klma/p")); //1
    }
}
