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
#include "include/history.h" //history命令相关
#include "include/command.h"


//运行单一命令
int run_single_command(char **commands){

    /**
     * 如果是内建命令，直接在当前进程上调用，并直接返回了
     */
    char **argv = splite_command_to_argv(commands[0]);
    int builtin_index = is_builtin(argv);
    if( builtin_index != -1 ){
        (*builtin_funcs[builtin_index])(argv);
        free(argv);
        return 0 ;
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
    }
    return 0;
}

//通过管道运行一组命令
int run_commands(char **commands){
    int count_command = count(commands);
    int count_fd = (count_command - 1) * 2;
    int fd[count_fd]; // 管道的文件描述符
    for (int i = 0; i < count_command; ++i) {
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
        char **argv = splite_command_to_argv(commands[i]);
        int builtin_index = is_builtin(argv);
        if( builtin_index != -1 ){
            (*builtin_funcs[builtin_index])(argv);
            free(argv);
            prompt();
            return 0 ;
        }

        if( i < count_command - 1){
            pipe(&fd[i * 2]);
        }

        if( fork() == 0 ){
            if(i == 0 ){
                dup2(fd[1],STDOUT_FILENO);
                //关闭不需要的描述符
                close(fd[0]);
            }else if (i < count_command - 1){
                dup2(fd[(i - 1) * 2 ],STDIN_FILENO);
                dup2(fd[i * 2 + 1],STDOUT_FILENO);

                for (int j = 0; j < count_fd && j <= i * 2 + 1 ; ++j) {
                    if( j == (i - 1) * 2 || j == i * 2 + 1 ){
                        continue;
                    }
                    close(fd[j]);
                }
            }else if(i == count_command - 1 ){
                dup2(fd[(i - 1) * 2],STDIN_FILENO);
                for (int j = 0; j < count_fd && j <= i * 2 + 1 ; ++j) {
                    if( j == (i - 1) * 2){
                        continue;
                    }
                    close(fd[j]);
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
    for (int k = 0; k < count_command; ++k) {
        wait(NULL);
    }
    return 0;
}

/**
 * 运行程序
 */
int run(char **commands){

    //判断命令是否为空
    int count_command = count(commands);
    switch (count_command){
        case 0: //如果没有输入命令
            break;
        case 1: //如果输入一个命令
            run_single_command(commands);
            break;
        default: //通过管道输入了多个命令
            run_commands(commands);
    }

    prompt();
    return 0;
}

int main(int argc, char *argv[]){
    prompt();
    while (1){
        char *line = read_command_line();
        history_record(line); //记录history
        char **commands =  splite_command_to_pipe(line);
        run(commands);
        free(commands);
    }
}