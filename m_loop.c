//
// Created by yzy on 3/12/16.
//

/*
 * main loop 为主键盘输入转发函数，其中的normal process为vim风格的产物，现已废弃。
 */


#include <conio.h>

#include "m_loop.h"
#include "file_str.h"
#include "draw_ui.h"
#include "n_loop.h"
#include "ins_loop.h"
#include "btm_loop.h"

int mode_flag = INSERT_MODE;

int main_loop()
{
    //This function assume the display back is disabled.
    int key_down = 0;
    while (1)
    {
        key_down = getch();

        switch (mode_flag)
        {
            case NORMAL_MODE:
                normal_mode_process(key_down);
                break;

            case REWRITE_MODE:
            case INSERT_MODE:
                insert_mode_process(key_down);
                break;

            case BOTTOMLINE_MODE:
                bottomline_mode_process(key_down);
                break;

            default:
                break;
        }
        parse_highlighting(cur_file);
        redraw_ui();
    }
}

//This function judge if the position of cursor is contained in the file.
//***!!! position available: including one pos at each end of line  !!!***
//return code:
//1 for True
//0 for False
int is_position_in_file()
{
    unsigned int cur_file_line;
    unsigned int cur_file_column;
    cur_file_line = cur_top + cur_line - 1;
    cur_file_column = cur_left + cur_column - 1;

    if (cur_file_line > get_total_lines(cur_file))
    {
        return 0;
    }
    if (cur_file_column > 1 + get_length(get_line(cur_file, cur_file_line)))
    {
        return 0;
    }

    return 1;
}

