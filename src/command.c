/** 
 * Created by 吴化吉
 * command.h : 与命令相关的函数
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/history.h"

/**
 * 读取用户输入的命令，到回车为止
 */
char *read_command_line(){
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

        //如果超出 max_length 个字符，重新分配空间
        if( index >= max_length ){
            max_length += max_length;
            line = realloc(line,max_length);
        }
    }
}


/**
 * 如果这一行命令包含管道符 | ，拆分成多个命令组
 */
char **splite_command_to_pipe(char *line){
    char delimiter[] = "|";
    int max_command = 10;
    char **commands = malloc(sizeof(char *) * max_command);

    if( commands == NULL ){
        printf("malloc 分配空间失败\n");
        exit(EXIT_FAILURE);
    }

    char *item = strtok(line,delimiter);
    int index = 0;
    while (item != NULL){

        /**
         * 如果以 !开头，并且后面是数字，那么就是查找history
         */
        int exist_history = 0;
        if( item[0] == '!' && atoi(item + 1) > 0){
            char *history_command = read_history_line(atoi(item + 1));
            if( history_command ){
                printf("%s\n",history_command);
                commands[index] = history_command;
                exist_history = 1;
            }
        }

        if( exist_history == 0 ){
            char *item_cpy = malloc(sizeof(item));
            strcpy(item_cpy,item);
            commands[index] = item_cpy;
        }

        item = strtok(NULL,delimiter);
        index++;
        //如果超出max_argv - 1个字符，重新分配空间；为什么需要 - 1 ，因为要给最后的NULL留出空间
        if( index >= max_command - 1 ){
            max_command += max_command;
            commands = realloc(commands,max_command);
        }
    }
    commands[index] = NULL;
    free(line);
    return commands;
}

/**
 * 把命令拆分成多个参数部分：如"ls -al",分为："ls","-al"两部分，以便调用execvp函数
 */
char **splite_command_to_argv(char *line){
    char delimiter[] = " \t\r\n";
    int max_argv = 100;
    char **argv = malloc(sizeof(char *) * max_argv);
    if( argv == NULL ){
        printf("malloc 分配空间失败\n");
        exit(EXIT_FAILURE);
    }
    char *item = strtok(line,delimiter);

    int index = 0;
    while (item != NULL){
        argv[index] = item;
        item = strtok(NULL,delimiter);
        index++;

        //如果超出 max_argv - 1 个字符，重新分配空间；为什么需要 - 1 ，因为要给最后的NULL留出空间
        if( index >= max_argv - 1 ){
            max_argv += max_argv;
            argv = realloc(argv,max_argv);
        }
    }
    argv[index] = NULL;

    return argv;
}

/**
 * 计算写入了多少个命令
 */
int count(char **argv){
    int count = 0 ;
    while (argv[count] != NULL){
        count++;
    }
    return count;
}
