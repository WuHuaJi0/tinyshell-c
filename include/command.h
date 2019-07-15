/**
 * Created by 吴化吉 on 2019-07-15.
 * command.h : 与命令相关的函数
 */

char *read_command_line(); //读取用户输入的一行命令
char **splite_command_to_pipe(char *line); //如果这一行命令包含管道符 | ，拆分成多个命令组
char **splite_command_to_argv(char *line);
int count_command(char **argv); //计算输入的命令数量