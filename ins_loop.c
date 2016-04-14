//
// Created by yzy on 3/13/16.
//

/*
 * 插入模式，也就是正常的输入文本的模式交互的循环。因为分支过多故接受建议拆分为函数指针数组方式进行跳转。
 * 也定义了表示选取范围的全局变量，和部分与选取相关的工具函数。
 */

#include "main.h"

#include <stdio.h>

#include "ins_loop.h"
#include "m_loop.h"
#include "file_sl.h"
#include "draw_ui.h"
#include "n_loop.h"
#include "btm_loop.h"

volatile unsigned int selected_start_line;
volatile unsigned int selected_start_column;
volatile unsigned int selected_end_line;
volatile unsigned int selected_end_column;

int (*ins_loop_func_list[0xFF])();

int (*function_key_sublist[0xFF])();

int insert_up()
{
    if (bioskey(2) & SHIFT)
    {
        if (selected_start_column == 0 && selected_start_line == 0)
        {
            set_start_select_pos();
            cursor_up();
            set_end_select_pos();
        }
        else
        {
            if (selected_end_line == cur_top + cur_line - 1 &&
                selected_end_column == cur_left + cur_column - 1)
            {
                cursor_up();
                set_end_select_pos();
            }
            else
            {
                cursor_up();
                set_start_select_pos();
            }
        }
        reverse_selected();
    }
    else
    {
        cancel_select();
        cursor_up();
    }
    return 0;
}

int insert_down()
{
    if (bioskey(2) & SHIFT)
    {
        if (selected_start_column == 0 && selected_start_line == 0)
        {
            set_start_select_pos();
            cursor_down();
            set_end_select_pos();
        }
        else
        {
            if (selected_end_line == cur_top + cur_line - 1 &&
                selected_end_column == cur_left + cur_column - 1)
            {
                cursor_down();
                set_end_select_pos();
            }
            else
            {
                cursor_down();
                set_start_select_pos();
            }
        }
        reverse_selected();
    }
    else
    {
        cancel_select();
        cursor_down();
    }
    return 0;
}

int insert_left()
{
    if (bioskey(2) & SHIFT)
    {
        if (selected_start_column == 0 && selected_start_line == 0)
        {
            set_start_select_pos();
            cursor_left();
            set_end_select_pos();
        }
        else
        {
            if (selected_end_line == cur_top + cur_line - 1 &&
                selected_end_column == cur_left + cur_column - 1)
            {
                cursor_left();
                set_end_select_pos();
            }
            else
            {
                cursor_left();
                set_start_select_pos();
            }
        }
        reverse_selected();
    }
    else
    {
        cancel_select();
        cursor_left();
    }
    return 0;
}

int insert_right()
{
    if (bioskey(2) & SHIFT)
    {
        if (selected_start_column == 0 && selected_start_line == 0)
        {
            set_start_select_pos();
            cursor_right();
            set_end_select_pos();
        }
        else
        {
            if (selected_end_line == cur_top + cur_line - 1 &&
                selected_end_column == cur_left + cur_column - 1)
            {
                cursor_right();
                set_end_select_pos();
            }
            else
            {
                cursor_right();
                set_start_select_pos();
            }
        }
        reverse_selected();
    }
    else
    {
        cancel_select();
        cursor_right();
    }
    return 0;
}

int insert_home()
{
    cancel_select();
    goto_line_actual_start();
    return 0;
}

int insert_end()
{
    cancel_select();
    goto_line_end();
    return 0;
}

int insert_case0()  //for function keys.
{
    int key = getch();
    return function_key_sublist[key] == NULL ? 0 : function_key_sublist[key]();
}

int insert_tab()
{
    int i = 0;

    cancel_select();
    for (; i < 4; i++)
    {
        add_char(cur_file, cur_line + cur_top - 1, cur_column + cur_left - 1, ' ');
        cursor_right();
    }
    changed_flag = CHANGED;
    changed_flag = (changed_flag == UNSAVED) ? CHANGED : changed_flag;
    return 0;
}

int insert_backspace()
{
    unsigned int length = 0;
    unsigned int actual_column = 0;
    unsigned int i;

    length = get_length(get_line(cur_file, cur_line + cur_top - 1));
    actual_column = cur_column + cur_left - 1;

    cancel_select();
    if (actual_column != 1)
    {
        del_char(get_line(cur_file, cur_top + cur_line - 1), cur_left + cur_column - 3);
        cursor_left();
    }
    else
    {
        if (length == 0)
        {
            if (cur_line + cur_top - 1 == 1)
            {
                return 0;
            }
            delete_line(cur_file, cur_top + cur_line - 1);
            cursor_up();
            goto_line_end();
        }
        else
        {
            connect_line(cur_file, cur_top + cur_line - 1);
            cursor_up();
            goto_line_end();
            for (i = 0; i < length; i++)
            {
                cursor_left();
            }
        }
    }
    changed_flag = CHANGED;
    return 0;
}

int insert_C_C()
{
    copy_selected();
    cancel_select();
    changed_flag = (changed_flag == UNSAVED) ? CHANGED : changed_flag;
    return 0;
}

int insert_C_V()
{
    paste_copied();
    changed_flag = (changed_flag == UNSAVED) ? CHANGED : changed_flag;
    return 0;
}

int insert_esc()
{
    cancel_select();
    changed_flag = (changed_flag == UNSAVED) ? CHANGED : changed_flag;
    return 0;
}

int insert_C_N()
{
    if (changed_flag == UNCHANGED || changed_flag == UNSAVED)
    {
        set_cursor_pos(console_columns, console_lines);
    }
    else
    {
        changed_flag = UNSAVED;
    }
    return 0;
}

int insert_C_O()
{
    if (changed_flag == CHANGED)
    {
        changed_flag = UNSAVED;
        return 0;
    }
    mode_flag = BOTTOMLINE_MODE;
    bottomline_sub_mode = BOTTOM_LINE_FILENAME_OPEN;
    return 0;
}

int insert_C_Q()
{
    if (changed_flag == CHANGED)
    {
        changed_flag = UNSAVED;
    }
    else if (changed_flag == UNSAVED)
    {
        _exit_(1);
    }
    else
    {
        _exit_(0);
    }
    return 0;
}

int insert_C_S()
{
    if (strlen(cur_file_name) == 0)
    {
        mode_flag = BOTTOMLINE_MODE;
        bottomline_sub_mode = BOTTOM_LINE_FILENAME_SAVE;
    }
    else
    {
        v_save_file(cur_file_name, cur_file);
        changed_flag = UNCHANGED;
    }
    return 0;
}

int insert_enter()
{
    cancel_select();
    split_line(cur_file, cur_top + cur_line - 1, cur_left + cur_column - 2);
    cursor_down();
    goto_line_start();
    changed_flag = CHANGED;
    changed_flag = (changed_flag == UNSAVED) ? CHANGED : changed_flag;
    return 0;
}

int insert_default(int key_down)
{
    unsigned int length = 0;

    if (bioskey(2) & CTRL)
    {
        return 0;
    }
    length = get_length(get_line(cur_file, cur_top + cur_line - 1));

    cancel_select();
    if (mode_flag == REWRITE_MODE)
    {
        if (cur_left + cur_column - 1 <= length)
        {
            del_char(get_line(cur_file, cur_top + cur_line - 1), cur_left + cur_column - 2);
        }
    }
    add_char(cur_file, cur_line + cur_top - 1, cur_column + cur_left - 1, (char) key_down);
    cursor_right();
    changed_flag = CHANGED;
    changed_flag = (changed_flag == UNSAVED) ? CHANGED : changed_flag;
    return 0;
}

int insert_page_up()
{
    if (bioskey(2) & SHIFT)
    {
        cur_top = 1;
        cur_line = 1;
        cur_left = 1;
        cur_column = 1;
    }
    else
    {
        if (cur_top <= SCREEN_LINES)
        {
            cur_top = 1;
        }
        else
        {
            cur_top -= SCREEN_LINES;
        }
        cur_left = 1;
        cur_column = 1;
    }
    return 0;
}

int insert_page_down()
{
    unsigned int lines = get_total_lines(cur_file);

    if (bioskey(2) & SHIFT)
    {
        if (lines > SCREEN_LINES)
        {
            cur_line = SCREEN_LINES;
            cur_top = lines - SCREEN_LINES + 1;
        }
        else
        {
            cur_top = 1;
            cur_line = lines;
        }

        cur_left = 1;
        cur_column = 1;
    }
    else    //no shift press down.
    {
        if (cur_top - 1 + SCREEN_LINES > lines)
        {
            cur_top = lines - SCREEN_LINES + 1;
        }
        else
        {
            cur_top += SCREEN_LINES;
        }

        cur_left = 1;
        cur_column = 1;
    }
    return 0;
}

int insert_insert()
{
    mode_flag = (mode_flag == INSERT_MODE ? REWRITE_MODE : INSERT_MODE);
    cancel_select();
    return 0;
}

int insert_delete()
{
    unsigned int length = 0;
    unsigned int actual_column = 0;
    unsigned int lines = 0;

    cancel_select();

    length = get_length(get_line(cur_file, cur_top + cur_line - 1));
    actual_column = cur_left + cur_column - 1;
    lines = get_total_lines(cur_file);

    if (actual_column <= length)
    {
        del_char(get_line(cur_file, cur_top + cur_line - 1), cur_left + cur_column - 2);
        changed_flag = CHANGED;
    }
    else if (actual_column > length)
    {
        if (cur_top + cur_line - 1 < lines)
        {
            connect_line(cur_file, cur_top + cur_line);
            changed_flag = CHANGED;
        }
        else
        {

        }
    }
    return 0;
}

int insert_mode_process(int key_down)
{
    if (key_down < 0)
    {
        return -1;
    }

    return (ins_loop_func_list[key_down] == NULL) ? insert_default(key_down) : ins_loop_func_list[key_down]();
}

//check and fix if the start position and end position is reversed.
int reverse_selected()
{
    unsigned int tmp;

    if (selected_start_line < selected_end_line)
    {
        return 0;
    }
    if (selected_start_line == selected_end_line && selected_start_column < selected_end_column)
    {
        return 0;
    }

    tmp = selected_end_line;
    selected_end_line = selected_start_line;
    selected_start_line = tmp;
    tmp = selected_start_column;
    selected_start_column = selected_end_column;
    selected_end_column = tmp;
    return 1;
}

int cancel_select()
{
    selected_start_line = selected_start_column = selected_end_line = selected_end_column = 0;
    return 0;
}

int set_start_select_pos()
{
    selected_start_column = cur_left + cur_column - 1;
    selected_start_line = cur_top + cur_line - 1;
    return 0;
}

int set_end_select_pos()
{
    selected_end_column = cur_left + cur_column - 1;
    selected_end_line = cur_top + cur_line - 1;
    return 0;
}

int init_ins_loop_func()
{
    ins_loop_func_list[0x00] = insert_case0;
    ins_loop_func_list[0x03] = insert_C_C;
    ins_loop_func_list[0x08] = insert_backspace;
    ins_loop_func_list[0x0d] = insert_enter;
    ins_loop_func_list[0x0e] = insert_C_N;
    ins_loop_func_list[0x0f] = insert_C_O;
    ins_loop_func_list[0x11] = insert_C_Q;
    ins_loop_func_list[0x13] = insert_C_S;
    ins_loop_func_list[0x16] = insert_C_V;
    ins_loop_func_list[0x1b] = insert_esc;
    ins_loop_func_list['\t'] = insert_tab;

    function_key_sublist[0x47] = insert_home;
    function_key_sublist[0x48] = insert_up;
    function_key_sublist[0x4b] = insert_left;
    function_key_sublist[0x4d] = insert_right;
    function_key_sublist[0x4f] = insert_end;
    function_key_sublist[0x50] = insert_down;
    function_key_sublist[0x49] = insert_page_up;
    function_key_sublist[0x51] = insert_page_down;
    function_key_sublist[0x52] = insert_insert;
    function_key_sublist[0x53] = insert_delete;

    return 0;
}
