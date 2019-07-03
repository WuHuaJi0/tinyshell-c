/** 
 * Created by 吴化吉
 **/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

//获取用户名
static char* getusername(){
    uid_t userid = getuid();
    struct passwd* pwd = getpwuid(userid);
    return pwd->pw_name;
}

void prompt(){
    char *username = getusername();
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    printf("%s@%s > ",username,hostname);
    fflush(stdout);

}
