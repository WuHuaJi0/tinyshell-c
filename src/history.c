/** 
 * Created by 吴化吉
 **/
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> //malloc

//记录命令输入日志
int history_record(char *commands){
    FILE *file = fopen(".tinyshell_history","ar+");
    if( file == NULL ){
        perror("读取history文件失败");
        return 0;
    }
    //只要不是在文件开头，先换行
    if( ftell(file) != 0 ){
        fputc('\n',file);
    }
    int result = fputs(commands,file);
    fclose(file);
    return 0 ;
}


/**
 * 读取整个history历史
 * @return
 */
int read_history(){
    FILE *file = fopen(".tinyshell_history","r");
    if( file == NULL ){
        perror("读取history文件失败");
        return 0;
    }

    char buffer[1024];
    int history_index = 1;

    while(fgets(buffer,1024,file)!=NULL)
    {
        printf("%d %s",history_index,buffer);
        history_index++;
    }
    printf("\n");
    fclose(file);
    return 0;
}

char *read_history_line(int line){
    char src[10000];
    char *dest = malloc(sizeof(char) * 10000);
    FILE *fp = fopen(".tinyshell_history","r");
    int exist_line = 0;
    while(fgets(src,100,fp)){
        line--;
        if( line==0 ) { //读到第line行数据
            strcpy(dest,src);
            exist_line = 1;
            break;
        }
    }
    fclose(fp);
    if( exist_line ){
        return dest;
    }
    printf("没有此条命令记录\n");
    return NULL;
}