/** 
 * Created by 吴化吉
 **/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../include/command.h"

char *redirect_symbol[9] = {
    ">",
    "1>",
    ">>",
    "1>>",
    "2>",
    "2>>",
    "2>&1",
    "2>>&1",
    "<",
};

//判断参数是否是重定向符号
int is_redirect_symbol(char *s){
    for (int i = 0; i < sizeof(redirect_symbol) / sizeof(char *); ++i) {
        if( strcmp(redirect_symbol[i],s) == 0){
            return 1;
        }
    }
    return 0 ;
}

int openfile(char *path, int is_append){
    int flag ;
    if( is_append ){
        flag = O_RDWR | O_APPEND | O_CREAT;
    }else{
        flag = O_RDWR | O_TRUNC | O_CREAT;
    }
    int fd = open(path,flag, 0755);
    if( fd == -1 ){ //如果文件不存在
        perror("无法打开或创建文件");
        return -1;
    }
    return fd;
}

/**
 * 把命令中与重定向相关的参数去除掉
 */
char **remove_redirect_symbol(char **old_argv){
    int old_argv_count = count(old_argv) + 1;
    char **new_argv = malloc(sizeof(char *) * old_argv_count);
    int index = 0; //index循环遍历旧命令
    int index_new_argv = 0 ;//index_new_argv,记录新命令的下标
    while (old_argv[index] != NULL){
        int is_result =  is_redirect_symbol(old_argv[index]);
        if( is_result ){
            index += 2;
            continue;
        }
        char *s = malloc(sizeof(old_argv[index]));
        strcpy(s,old_argv[index]);
        new_argv[index_new_argv] = s;
        index_new_argv++;
        index++;
    }
    new_argv[index_new_argv] = NULL;
    free(old_argv);
    return new_argv;
}

char **redirect(char **argv){
    int index = 0;
    while (argv[index] != NULL && argv[index + 1] != NULL){
        if( strcmp(argv[index],">") == 0  || strcmp(argv[index],"1>") ==  0){
            int fd = openfile(argv[index + 1],0);
            if( fd != -1 ){
                dup2(fd,STDOUT_FILENO);
                index += 2;
                continue;
            }
        }
        if( strcmp(argv[index],">>") == 0  || strcmp(argv[index],"1>>") ==  0){
            int fd = openfile(argv[index + 1],1);
            if( fd != -1 ){
                dup2(fd,STDOUT_FILENO);
                index += 2;
                continue;
            }
        }
        if( strcmp(argv[index],"2>") == 0){
            int fd = openfile(argv[index + 1],0);
            if( fd != -1 ){
                dup2(fd,STDERR_FILENO);
                index += 2;
                continue;
            }
        }
        if( strcmp(argv[index],"2>>") == 0){
            int fd = openfile(argv[index + 1],1);
            if( fd != -1 ){
                dup2(fd,STDERR_FILENO);
                index += 2;
                continue;
            }
        }
        if( strcmp(argv[index],"2>&1") == 0){
            int fd = openfile(argv[index + 1],0);
            if( fd != -1 ){
                dup2(fd,STDERR_FILENO);
                dup2(fd,STDOUT_FILENO);
                index += 2;
                continue;
            }
        }
        if( strcmp(argv[index],"2>>&1") == 0){
            int fd = openfile(argv[index + 1],1);
            if( fd != -1 ){
                dup2(fd,STDERR_FILENO);
                dup2(fd,STDOUT_FILENO);
                index += 2;
                continue;
            }
        }
        if( strcmp(argv[index],"<") == 0){
            int fd = open(argv[index + 1],O_RDWR);
            if( fd != -1 ){
                dup2(fd,STDIN_FILENO);
                index += 2;
                continue;
            }
        }
        index++;
    }
    return remove_redirect_symbol(argv);
}