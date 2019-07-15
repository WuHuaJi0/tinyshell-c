//
// Created by 吴化吉 on 2019-07-10.
//
int (*builtin_funcs[2])(char **);

char *builtins[2];

int is_builtin(char **argv); //判断命令是否builtin