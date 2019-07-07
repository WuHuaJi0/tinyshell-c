/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>


typedef struct {
    char prompt_character;
    char *user_name;
} user_info;

//获取用户名,判断是否是root
static user_info getusername(){
    user_info user ;
    uid_t userid = getuid();
    struct passwd* pwd = getpwuid(userid);

    user.user_name = pwd->pw_name;
    user.prompt_character = userid ? '$' : '#';
    return user;
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

void prompt(){
    user_info user = getusername();
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    printf("%s@%s:%s%c ",user.user_name,hostname,get_current_directory(),user.prompt_character);
    fflush(stdout);
}
