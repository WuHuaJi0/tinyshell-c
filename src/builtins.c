/** 
 * Created by 吴化吉
 **/
#include <stdio.h>
#include <unistd.h>

char *builtins[] = {
    "cd"
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
    &cd
};