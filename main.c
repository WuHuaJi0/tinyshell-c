/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



void prompt(){
    /**
     * print todo: user@host
     */
    printf("> ");
    fflush(stdout);
}


//从输入从读取字符串到回车为止
char* readline(){
    char buffer[1024];
    char *line = malloc(sizeof(char) * 1024);
    int index = 0;
    char c;
    while (1){
        c = getchar();
        if( c == EOF || c == '\n' ){
            return line;
        }
        line[index] = c;
        index++;
    }
}


//解析读取到的字符串
char **spite_argv(char *line){
    char delemite[] = " \t\r\n";
    char **argv = malloc(sizeof(char *) * 1024);
    char *item = strtok(line,delemite);

    int index = 0;
    while (item != NULL){
        argv[index] = item;
        item = strtok(NULL,delemite);
        index++;
    }
    return argv;
}

void run(char **argv){
    int pid = fork();
    int status;
    if( pid == 0 ){
        int result = execvp(argv[0],argv);
        if( result == -1 ){
            perror("shell:");
        }
    }else{
        wait(&status);
        prompt();
    }
}

void loop(){
    prompt();
    while (1){
        char *line = readline();
        char **argv = spite_argv(line);
        run(argv);
    }
}

int main(int argc, char *argv[]){
    loop();
    return 0;
}
