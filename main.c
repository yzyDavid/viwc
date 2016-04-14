//
// Created by yzy on 3/12/16.
// This file contains the entry of the program.
//

/*
 * 入口函数
 * 也包含了关于初始化处理和退出处理的函数
 * 主函数处理命令行参数，初始化并且进入等待键盘的循环
 */

#include <string.h>
#include <stdlib.h>
#include <graphics.h>

#include "intmouse.h"
#include "draw_ui.h"
#include "m_loop.h"
#include "file_str.h"
#include "file_sl.h"
#include "log_md.h"
#include "ins_loop.h"

void *white_buffer;

int main(int argc, char *argv[])
{
    init_log();
    init_graph();
    cur_file = malloc(sizeof(v_file_text));
    memset(cur_file_name, 0, FILE_LINE_LENGTH);
    init_ins_loop_func();

    switch (argc)
    {
        default:
        case 1:
            cur_file_name[0] = 0;
            v_new_file(cur_file);
            filename_assigned_flag = 0;
            break;
        case 2:
            strcpy(cur_file_name, argv[1]);
            if (v_load_file(cur_file_name, cur_file) == -1)
            {
                printf("File not exist!\n");
                exit(-1);
            }
            else
            {
                filename_assigned_flag = 1;
            }
            break;
    }

    determine_file_type(cur_file_name);

    parse_highlighting(cur_file);
    init_mouse();
    redraw_ui();
    main_loop();

    return 0;
}

int _exit_(int status_code)
{
    closegraph();
    close_mouse();
    textcolor(WHITE);
    gotoxy(1, 25);


#ifdef VIC_LOG_ON
    if (log_file_struct != NULL)
    {
        fclose(log_file_struct);
    }
#endif

    exit(status_code);
}

int init_graph()
{
    int driver = 0, mode = VESA_1024x768x8bit;
    dword size = 0;

    initgraph(&driver, &mode, "");
    setcolor(WHITE);
    rectangle(0, 0, 7, 15);
    floodfill(1, 1, WHITE);
    size = imagesize(0, 0, 7, 15);
    white_buffer = malloc(size);
    getimage(0, 0, 7, 15, white_buffer);

    cleardevice();

    return 0;
}
