// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "../include/utils.h"
//#include "../include/structures.h"
#include "../include/track.h"
#include "../include/status.h"
#include "../include/print.h"
#include "../include/regex.h"
#include "../include/restore.h"
#include "../include/commit.h"
#include "../include/tag.h"
#include "../include/log.h"
#include "../include/fingerprint.h"
#include "../include/diff.h"

#define BUFFER_SIZE 8192

void show_help(){
    show_message("usage: rw <command> <zip-path> <arg> <val> ....\n");
    show_message("Following are the commands supported");
    show_message("  track: start tracking the archive");
    show_message("  status: check the paths modified/deleted/created post the latest commit");
    show_message("  commit: commit the changes to track them");
    show_message("  restore: restore the paths from a particular commit");
    show_message("  fingerprint: get the hash of the file from a commit/tag or worktree");
    show_message("  log: display the commits");
    show_message("  tag: tag a particular commit with a name");
    show_message("  diff: get the diff between two commits\n");

    show_message("use \"rw <command> --help\" to get command specific help");
}



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
    } else if (strcmp(argv[1],"restore") == 0){
        process_restore(argc,argv);
    } else if (strcmp(argv[1],"tag") == 0){
        process_tag(argc,argv);
    } else if (strcmp(argv[1],"log") == 0){
        process_log(argc,argv);
    } else if (strcmp(argv[1],"fingerprint") == 0){
        process_fingerprint(argc,argv);
    } else if (strcmp(argv[1],"diff") == 0){
        process_diff(argc,argv);
    } 
    else if (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0){
        show_help();
    } else if (strcmp(argv[1],"--version") == 0 || strcmp(argv[1],"-v") == 0){
        show_message(__CONSTANTS_RW_VERSION__);
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
        printf("%d\n",regex_is_matching("*","xyz/xyz.txty")); //1
    }
}