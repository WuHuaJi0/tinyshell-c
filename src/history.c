/** 
 * Created by 吴化吉
 **/
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

static int need_newline(fd){
    //如果读出不为空，则换行之后再写入
    char buffer_read[10];
    int read_count = read(fd,buffer_read,10);
    if( read_count > 0 ){
        if( write(fd,"\n",1) == -1 ){
            perror("write error"); return -1;
        }
    }
    return 0 ;
}

//记录命令输入日志
int history_record(char *commands){
    int fd = open(".tinyshell_history",O_RDWR | O_APPEND | O_CREAT, 0755);
    if( fd == -1 ){ //如果文件不存在
        perror("tinyshell_history");
        return -1;
    }

    //判断是否需要换行再写入
    need_newline(fd);

    int commonds_length = strlen(commands);
    int write_result = write(fd,commands, strlen(commands));
    if( write_result == -1 ){
        perror("write error");
    }

    close(fd);
    return 0;
}
