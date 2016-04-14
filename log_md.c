//
// Created by yzy on 3/15/16.
//

/*
 * 使用条件编译来决定是否输出日志，日志主要用作调试用，故曾经输出日志的代码已大部分移除。
 * 根据洪是否定义来决定是否生成初始化日志，输出日志的代码。
 */

#include <stdio.h>
#include <assert.h>
#include "log_md.h"

char *log_file_name = "vic_log.txt";
FILE *log_file_struct = NULL;

int __vic_log_output(char *content)
{
    return fprintf(log_file_struct, "%s\n", content);
}

int __vic_init_log()
{
    log_file_struct = fopen(log_file_name, "w");
    if (log_file_struct != NULL)
    {
        return 0;
    }
    assert(0);
    return -1;
}
