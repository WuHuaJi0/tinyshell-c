/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "include/prompt.h"  //输出命令行提示符
#include "include/builtins.h" //内建函数
#include "include/history.h" //内建函数

//从输入从读取字符串到回车为止
char *readline(){
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


/**
 * 如果这一行命令，是包含管道，拆分成多个命令组
 * @return
 */
char **splite_pipe(char *line){
    char delimiter[] = "|";
    int max_commond = 10;
    char **commands = malloc(sizeof(char *) * max_commond);

    if( commands == NULL ){
        printf("malloc 分配空间失败\n");
        exit(EXIT_FAILURE);
    }

    char *item = strtok(line,delimiter);
    int index = 0;
    while (item != NULL){
        commands[index] = item;
        item = strtok(NULL,delimiter);
        index++;
        //如果超出max_argv - 1个字符，重新分配空间；为什么需要 - 1 ，因为要给最后的NULL留出空间
        if( index >= max_commond - 1 ){
            max_commond += max_commond;
            commands = realloc(commands,max_commond);
        }
    }
    commands[index] = NULL;

    return commands;
}

//解析读取到的字符串
char **splite_argv(char *line){
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

        //如果超出max_argv - 1个字符，重新分配空间；为什么需要 - 1 ，因为要给最后的NULL留出空间
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
 * @return
 */
int count_command(char **argv){
    int count = 0 ;
    while (argv[count] != NULL){
        count++;
    }
    return count;
}


/**
 * 判断是否内建命令
 * @param commands
 * @return
 */
int is_builtin(char **argv){
    for (int i = 0; i < sizeof(builtins) / sizeof(char *); ++i) {
        if( strcmp(builtins[i],argv[0]) == 0){
            return i;
        }
    }
    return -1;
}


/**
 * 运行程序
 * @param commands
 * @return
 */
int run(char **commands){
    int count = count_command(commands);
    if( count == 0 ){
        prompt();
        return 0;
    }

    int count_fd = (count - 1) * 2;
    int fd[count_fd]; // 管道的文件描述符
    for (int i = 0; i < count; ++i) {

        /**
         * 如果以 !开头，并且后面是数字，那么就是查找history
         */
        if( commands[i][0] == '!' && atoi(commands[i] + 1) > 0){
            char *history_command = read_history_line(atoi(commands[i] + 1));
            if( history_command ){
                printf("%s\n",history_command);
                commands[i] = history_command;
            }
        }


        /**
         * 如果是内建命令，直接在当前进程上调用，并直接返回了
         */
        char **argv = splite_argv(commands[i]);
        int builtin_index = is_builtin(argv);
        if( builtin_index != -1 ){
            (*builtin_funcs[builtin_index])(argv);
            free(argv);
            prompt();
            return 0 ;
        }


        if( count > 1 && i < count - 1){
            pipe(&fd[i * 2]);
        }

        if( fork() == 0 ){
            if( count > 1 ){
                if(i == 0 ){
                    dup2(fd[1],STDOUT_FILENO);
                    //关闭不需要的描述符
                    close(fd[0]);
                }else if (i < count - 1){
                    dup2(fd[(i - 1) * 2 ],STDIN_FILENO);
                    dup2(fd[i * 2 + 1],STDOUT_FILENO);

                    for (int j = 0; j < count_fd && j <= i * 2 + 1 ; ++j) {
                        if( j == (i - 1) * 2 || j == i * 2 + 1 ){
                            continue;
                        }
                        close(fd[j]);
                    }
                }else if(i == count - 1 ){
                    dup2(fd[(i - 1) * 2],STDIN_FILENO);
                    for (int j = 0; j < count_fd && j <= i * 2 + 1 ; ++j) {
                        if( j == (i - 1) * 2){
                            continue;
                        }
                        close(fd[j]);
                    }
                }
            }

            int result = execvp(argv[0],argv);
            free(argv);

            if( result == -1 ){
                perror("shell");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
            break;
        }
    }

    //关闭父进程所有的文件描述符
    for (int j = 0; j < count_fd; ++j) {
        close(fd[j]);
    }

    //等待所有子进程结束
    for (int k = 0; k < count; ++k) {
        wait(NULL);
    }

    prompt();
    return 0;
}

int main(int argc, char *argv[]){
    prompt();
    while (1){
        char *line = readline();
        history_record(line); //记录history
        char **commands =  splite_pipe(line);
        run(commands);
        free(commands);
    }
}