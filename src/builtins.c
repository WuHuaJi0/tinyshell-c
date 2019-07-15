/** 
 * Created by 吴化吉
 **/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../include/history.h"

char *builtins[] = {
    "cd",
    "history"
};

static int cd( char **argv){
    if (argv[1] == NULL) {
        printf("cd命令需要参数\n");
    } else {
        if (chdir(argv[1]) != 0) {
            perror("cd");
        }
    }
    return 1;
}


int (*builtin_funcs[])(char **) = {
    &cd,
    &read_history
};


/**
 * 判断是否内建命令
 */
int is_builtin(char **argv){
    for (int i = 0; i < sizeof(builtins) / sizeof(char *); ++i) {
        if( strcmp(builtins[i],argv[0]) == 0){
            return i;
        }
    }
    return -1;
}
