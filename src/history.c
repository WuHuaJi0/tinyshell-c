/** 
 * Created by 吴化吉
 **/
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

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
    printf("写入结果: %d\n",result);
    fclose(file);
    return 0 ;
}


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