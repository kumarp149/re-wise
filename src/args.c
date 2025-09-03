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

void processArgs(int argc,char** argv, struct args_flag* flags, size_t flags_size, struct args_valarg* valargs, size_t valargs_size, zip_t** archive, int* flag,char ***option_values, int *option_counts,void (*show_usage)(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size),int* proceed_further,int help_bit,char* integer_args,size_t integer_args_size){
    *proceed_further = 0;
    if (argc == 2){
        __ARGS_SHOW_USAGE_;
        return;
    } else if (argc >= 3 && (strcmp(argv[2],"--help") == 0 || strcmp(argv[2],"-h") == 0)){
        __ARGS_SHOW_USAGE_;
        return;
    }

    struct strvec* arg_errors = strvec_init();

    int archive_open_error = 0;

    *archive = zip_open(argv[2],ZIP_CHECKCONS,&archive_open_error);

    if (archive_open_error != 0){
        show_message("error opening the archive. please check that it is valid and you have enough permissions to access it");
        goto __RET;
    }

    *flag = 0;

    for (int i=3;i<argc;++i){
        char* arg = argv[i];

        struct args_flag* cur_flag;
        struct args_valarg* cur_valarg;

        int arg_type = get_arg_type(arg,flags,flags_size,valargs,valargs_size,&cur_flag,&cur_valarg);

        if (arg_type == 1) (*flag) = (*flag) ^ (1 << (cur_flag->flagId));
        else if (arg_type == 2){
            int index = *((cur_valarg->shortId) + 1)-'a';
            int start = i+1;
            int countArgs = 0;

            if (option_counts[index] > 0){
                countArgs = option_counts[index];
            } else{
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
                strvec_pushf(arg_errors,__ARGS_ERROR_ARG_MORE_PROVIDED__,cur_valarg->short_description);
            } else if (cur_valarg->mandatory == true && countArgs <= 0){
                if (cur_valarg->maxCount == 1){
                    strvec_pushf(arg_errors,__ARGS_ERROR_ARG_MANDATORY_NOTPROVIDED__,cur_valarg->short_description);
                } else{
                    strvec_pushf(arg_errors,__ARGS_ERROR_ARG_ATLEAST_ONE__,cur_valarg->short_description);
                }
            } else if (countArgs == 0){
                strvec_pushf(arg_errors,__ARGS_ERROR_ARG_NONMANDATORY_NOTPROVIDED__,cur_valarg->short_description);
            }
        } else{
            strvec_pushf(arg_errors,__ARGS_ERROR_ARG_UNKNOWNARG__,arg);
        }
    }

    for (size_t i=0;i<valargs_size;++i){
        char *c = ((valargs+i)->shortId) + 1;
        if ((valargs+i)->mandatory == true && *(option_counts + (c[0] - 'a')) == 0){
            strvec_pushf(arg_errors,__ARGS_ERROR_ARG_MANDATORY_NOTGIVEN__,(valargs+i)->short_description);
        }
    }
    for (size_t i=0;i<integer_args_size;++i){
        char* c = integer_args + i;
        if (*(option_counts + (c[0] - 'a')) > 0 && (!(std_is_valid_integer(option_values[c[0]-'a'][0])) || (atoi(option_values[c[0]-'a'][0])) < 0)){
            strvec_pushf(arg_errors,__ARGS_ERROR_ARG_NUMERIC_ARG__,(valargs+i)->short_description);
        }
    }
    if (((*flag) & (1 << help_bit)) != 0){
        __ARGS_SHOW_USAGE_;
        goto __RET;
    } else if (arg_errors->len > 0){
        show_message(arg_errors->v[0]);
        __ARGS_SHOW_USAGE_;
        goto __RET;
    }
    *proceed_further = 1;
__RET:
    strvec_free(&arg_errors);
    return;
}