/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>

//获取用户名
static char *getusername(){
    uid_t userid = getuid();
    struct passwd* pwd = getpwuid(userid);
    return pwd->pw_name;
}

/**
 * 读取当前工作目录
 */
static char *get_current_directory(){
    int max_directory_path = 1024;
    char *s = malloc(max_directory_path);
    char *result = getcwd(s, max_directory_path);
    if( result == NULL ){
        printf("读取当前工作目录失败\n");
    }
    return s;
}

/**
 * 判断是否root用户
 * @return
 */
static char is_root(){
    uid_t user_id = getuid();
    return user_id ? '$' : '#'; //对于root用户，uid为0
}


void prompt(){
    char *username = getusername();
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    printf("%s@%s:%s%c ",username,hostname,get_current_directory(),is_root());
    fflush(stdout);

}
