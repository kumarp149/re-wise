#include "../include/regex.h"

bool is_dirpattern_found(size_t index,char* s,size_t* sn){
    if (index+2 >= *sn){
        return false;
    } else{
        if (s[index] == '*' && s[index+1] == '*' && s[index+2] == '/'){
            return true;
        } else return false;
    }
    return false;
}

size_t get_sequence_closing_index(size_t index,char* s,size_t* sn){
    size_t res = index+1;
    while(res < *sn && s[res] != ']'){
        res++;
    }

    if (res >= *sn) return index;
    return res;
}

bool regex_is_matching_recur(size_t pindex,size_t sindex,char* p,char* s,size_t* pn,size_t* sn,int **dp){
    if (pindex >= *pn && sindex >= *sn) return true;
    if (pindex >= *pn) return false;
    if (sindex >= *sn){
        if (p[pindex] == '*'){
            if (is_dirpattern_found(pindex,p,pn) == true){
                return regex_is_matching_recur(pindex+3,sindex,p,s,pn,sn,dp);
            } else{
                return regex_is_matching_recur(pindex+1,sindex,p,s,pn,sn,dp);
            }
        } else return false;
    }

    if (dp[pindex][sindex] != -1) return dp[pindex][sindex];

    bool ans = false;

    if (p[pindex] == '*'){
        if (is_dirpattern_found(pindex,p,pn) == true){
            for (size_t i=sindex;i<*sn;++i){
                if (s[i] == '/' && regex_is_matching_recur(pindex+3,i+1,p,s,pn,sn,dp)){
                    __REGEX_ANS_(true);
                }
            }
            __REGEX_ANS_(false);
        } else{
            bool found = false;
            for (size_t i=sindex;i<=*sn;++i){
                if (regex_is_matching_recur(pindex+1,i,p,s,pn,sn,dp)){
                    found = true;
                    break;
                }
            }
            __REGEX_ANS_(found);
        }
    } else if (p[pindex] == '?'){
        __REGEX_ANS_(regex_is_matching_recur(pindex+1,sindex+1,p,s,pn,sn,dp));
    } else if (p[pindex] == '['){
        size_t closing_index = get_sequence_closing_index(pindex,p,pn);

        bool found = false;

        for (size_t i=pindex+1;i<closing_index;++i){
            if (p[i] == s[sindex]){
                found = true;
                break;
            }
        }
        __REGEX_ANS_(found && regex_is_matching_recur(closing_index+1,sindex+1,p,s,pn,sn,dp));
    } else{
        if (p[pindex] == s[sindex]) __REGEX_ANS_(regex_is_matching_recur(pindex+1,sindex+1,p,s,pn,sn,dp));
    }

__setans:
    dp[pindex][sindex] = ans;
    return ans;
}

bool regex_is_matching(char* pattern,char* word){
    size_t pn = strlen(pattern);
    size_t wn = strlen(word);

    int** dp = (int **)malloc(pn * sizeof(int*));

    for (size_t i=0;i<pn;++i){
        dp[i] = (int *)malloc(wn * sizeof(int));

        for (size_t j=0;j<wn;++j){
            dp[i][j] = -1;
        }
    }

    bool ans = regex_is_matching_recur(0,0,pattern,word,&pn,&wn,dp);

    for (size_t i=0;i<pn;++i) __RW_MEMFREE__(dp[i]);

    __RW_MEMFREE__(dp);

    return ans;
}

bool regex_is_anypattern_matching(char **patterns,size_t pattern_count,char* word){
    for (size_t i=0;i<pattern_count;++i){
        if (regex_is_matching(patterns[i],word)) return true;
    }
    return false;
}