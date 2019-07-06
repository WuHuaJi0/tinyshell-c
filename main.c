/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "include/prompt.h"

char *builtins[] = {
    "cd"
};

int cd( char **argv){
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



//从输入从读取字符串到回车为止
char* readline(){
    int max_length = 1024;
    char *line = malloc(sizeof(char) * max_length);

    if( line == NULL ){
        printf("malloc 分配空间失败\n");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    char c;
    while (1){
        c = getchar();
        if( c == '\n' ){
            line[index] = '\0';
            return line;
        }
        line[index] = c;
        index++;
        

        //如果超出1024个字符，重新分配空间
        if( index >= max_length ){
            max_length += max_length;
            line = realloc(line,max_length);
        }
    }
}


//解析读取到的字符串
char **spite_argv(char *line){
    char delemite[] = " \t\r\n";
    int max_argv = 100;
    char **argv = malloc(sizeof(char *) * max_argv);


    if( argv == NULL ){
        printf("malloc 分配空间失败\n");
        exit(EXIT_FAILURE);
    }
    char *item = strtok(line,delemite);

    int index = 0;
    while (item != NULL){
        argv[index] = item;
        item = strtok(NULL,delemite);
        index++;

        //如果超出1024个字符，重新分配空间
        if( index >= max_argv ){
            max_argv += max_argv;
            argv = realloc(argv,max_argv);
        }
    }
    return argv;
}



int run(char **argv){
    if( argv[0] == NULL ){
        prompt();
        return 0;
    }

    //判断是否是内建命令
    for (int i = 0; i < sizeof(builtins) / sizeof(char *); ++i) {
        if( strcmp(builtins[i],argv[0]) == 0){
            (*builtin_funcs[i])(argv);
            prompt();
            return 0;
        }
    }

    int pid = fork();
    int status;
    if( pid == 0 ){
        int result = execvp(argv[0],argv);
        if( result == -1 ){
            perror("shell");
            exit(EXIT_FAILURE);
        }
    }else{
        wait(&status);
        prompt();
    }

    return 0;
}

void loop(){
    prompt();
    while (1){
        char *line = readline();
        char **argv = spite_argv(line);
        run(argv);

        free(line);
        free(argv);
    }
}

int main(int argc, char *argv[]){
    loop();
    return 0;
}
