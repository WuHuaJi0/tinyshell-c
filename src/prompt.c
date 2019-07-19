/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char prompt_character;
    char *user_name;
    char *home;
} user_info;

//获取用户名,判断是否是root
static user_info getusername(){
    user_info user ;
    uid_t userid = getuid();
    struct passwd *pwd = getpwuid(userid);

    user.user_name = pwd->pw_name;
    user.prompt_character = userid ? '$' : '#';
    user.home = pwd->pw_dir;
    return user;
}

/**
 * 把工作目录中 home 的部分替换为 ~
 * 如：/Users/wuhuaji/code/fundament/tinyshell 替换为：~/code/fundament/tinyshell
 * @return
 */
static char *replace_home(char *current_directory,char *home){

    //判断当前目录起始是否包含完整的home目录：
    char *c = strstr(current_directory,home);
    if( c != NULL && c == current_directory ){
        int length_home = strlen(home);
        int length_current_directory = strlen(current_directory);

        char *result = malloc(length_current_directory - length_home + 2);
        result[0] = '~';
        result[1] = '\0';
        char *cpresult = strcat(result,current_directory + length_home);
        return result;
    }

    return current_directory;
}

/**
 * 读取当前工作目录
 */
static char *get_current_directory(char *home){
    int max_directory_path = 1024;
    char *s = malloc(max_directory_path);
    char *result = getcwd(s, max_directory_path);
    if( result == NULL ){
        printf("读取当前工作目录失败\n");
    }

    //获取user的home目录，
    char *current = replace_home(s,home);
    return current;
}

void prompt(){
    user_info user = getusername();
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    printf("\033[0;32;34m%s@\033[32m%s\033[m:%s%c ",user.user_name,hostname,get_current_directory(user.home),user.prompt_character);
    fflush(stdout);
}
