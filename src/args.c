#include "../include/args.h"

int get_arg_type(char* arg,struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size,struct args_flag** cur_flag,struct args_valarg** cur_valarg){
    for (size_t i=0;i<flags_size;++i){
        if (strcmp(arg,(flags+i)->longId) == 0 || strcmp(arg,(flags+i)->shortId) == 0){
            *cur_flag = (flags+i);
            return 1;
        }
    }
    for (size_t i=0;i<valargs_size;++i){
        if (strcmp(arg,(valargs+i)->longId) == 0 || strcmp(arg,(valargs+i)->shortId) == 0){
            *cur_valarg = (valargs+i);
            return 2;
        }
    }
    return 0;
}

bool is_probable_option(char* arg){
    if (strlen(arg) < 2) return false;
    if (strlen(arg) == 2){
        if (*(arg) == '-') return true;
    } else{
        if (*(arg) == '-' && *(arg+1) == '-') return true;
        return false;
    }
    return false;
}

void processArgs(int argc, char** argv, struct args_flag* flags, size_t flags_size, struct args_valarg* valargs, size_t valargs_size, zip_t** archive, int* flag,char ***option_values, int *option_counts, int* err, char** error_message, ErrorCode* errorCode){
    int archive_open_error = 0;

    *archive = zip_open(argv[2],ZIP_CHECKCONS,&archive_open_error);

    *flag = 0;

    for (int i=3;i<argc;++i){
        char* arg = argv[i];

        struct args_flag* cur_flag;
        struct args_valarg* cur_valarg;

        int arg_type = get_arg_type(arg,flags,flags_size,valargs,valargs_size,&cur_flag,&cur_valarg);

        if (arg_type == 1){
            (*flag) = (*flag) ^ (1 << (cur_flag->flagId));
        } else if (arg_type == 2){
            int index = *((cur_valarg->shortId) + 1)-'a';
            int start = i+1;
            int countArgs = 0;

            if (option_counts[index] > 0){
                countArgs = option_counts[index];
            } else{
                //printf("option values init\n");
                option_values[index] = malloc(((size_t)(argc - i)) * sizeof(char *));
            }

            while(start < argc && is_probable_option(argv[start]) == 0){
                option_values[index][option_counts[index]] = strdup(argv[start]);
                option_counts[index]++;
                start++;
                countArgs++;
            }

            start--;

            i = start;

            if (countArgs > cur_valarg->maxCount){
                *err = 1;
                sprintf(*error_message,"error: more than necessary arguments provided for the option <%s>",cur_valarg->short_description);
                return;
            } else if (cur_valarg->mandatory == true && countArgs <= 0){
                *err = 1;
                if (cur_valarg->maxCount == 1){
                    sprintf(*error_message,"error: option <%s> expects an argument",cur_valarg->short_description);
                } else{
                    sprintf(*error_message,"error: option <%s> expects atleast one argument",cur_valarg->short_description);
                }
                
                return;
            }
        } else{
            *err = 1;
            sprintf(*error_message, "error: unknown argument <%s> provided", arg);
            return;
        }

        cur_flag = NULL;
        cur_valarg = NULL;
    }

    for (size_t i=0;i<valargs_size;++i){
        char *c = ((valargs+i)->shortId) + 1;
        if ((valargs+i)->mandatory == true && *(option_counts + (c[0] - 'a')) == 0){
            *err = 1;
            sprintf(*error_message,"error: the argument <%s> is mandatory",(valargs+i)->short_description);
            return;
        }
    }

    if (archive_open_error != 0){
        if (archive_open_error == ZIP_ER_NOENT){
            *errorCode = ERR_ZIP_NOFILE;
            *error_message = argv[2];
        } else if (archive_open_error == ZIP_ER_NOZIP){
            *errorCode = ERR_ZIP_NOT_VALID;
            *error_message = argv[2];
        } else if (archive_open_error == ZIP_ER_OPEN){
            *errorCode = ERR_UNKNOWN;
        }
    }
}