//
// Created by yzy on 3/13/16.
//

/*
 * vim命令模式下的消息循环处理函数，现抛弃vim风格后已经废弃，仍保留备用
 * 很多组合快捷键的功能实现最先来自于此
 *
 * 此文件中还包含了控制光标移动的诸多工具函数，所以保留
 */

#include <assert.h>
#include <stdlib.h>

#include "n_loop.h"
#include "file_str.h"
#include "draw_ui.h"
#include "m_loop.h"
#include "file_sl.h"
#include "btm_loop.h"
#include "log_md.h"

int normal_mode_process(int key_down)
{
    char t[100];
    sprintf(t, "%x", key_down);
    print_log(t);

    switch (key_down)
    {
        case '\x1b':  //Esc

            break;

        case '\x48':    //up
            cursor_up();
            break;

        case '\x50':    //down
            cursor_down();
            break;

        case '\x4b':    //left
            cursor_left();
            break;

        case '\x4d':    //right
            cursor_right();
            break;

        case '!':   //quit directly.
            set_cursor_pos(console_columns, console_lines);
            exit(0);
            break;

        case '1':   //New File.
        case 'n':
            if (changed_flag == UNCHANGED || changed_flag == UNSAVED)
            {
                set_cursor_pos(console_columns, console_lines);
            }
            else
            {
                changed_flag = UNSAVED;
            }
            break;

        case 'q':   //quit, is saved should be checked.
            if (changed_flag == UNCHANGED || changed_flag == UNSAVED)
            {
                set_cursor_pos(console_columns, console_lines);
                exit(0);
            }
            else
            {
                changed_flag = UNSAVED;
            }
            break;

            //belows hjkl for cursor moving.
            //aiming to disable moving cursor outside the file part.
        case 'h':
            cursor_left();
            break;

        case 'l':
            cursor_right();
            break;

        case 'j':
            cursor_down();
            break;

        case 'k':
            cursor_up();
            break;

        case 'w':   //word forward.
        {
            int position = cur_left + cur_column - 1;
            int counter = 0;
            char *cur = get_line(cur_file, cur_top + cur_line - 1)->text;
            cur += cur_left + cur_column - 2;   //to be checked.
            if (is_word_start(cur))
            {
                position++;
                counter++;
                cur++;
            }
            for (; position < get_length(get_line(cur_file, cur_line + cur_top - 1)); position++, counter++)
            {
                if (is_word_start(cur++))
                {
                    break;
                }
            }
            for (; counter > 0; counter--)
            {
                cursor_right();
            }
        }
            break;

        case 'b':   //word backward.
        {
            int position = cur_left + cur_column - 1;
            int counter = 0;
            char *cur = get_line(cur_file, cur_top + cur_line - 1)->text;
            cur += cur_left + cur_column - 2;   //to be checked.
            if (is_word_start(cur))
            {
                position--;
                counter++;
                cur--;
            }
            for (; position > 0; position--, counter++)
            {
                if (is_word_start(cur--))
                {
                    break;
                }
            }
            for (; counter > 0; counter--)
            {
                cursor_left();
            }
        }
            break;

        case 'e':   //word-end forward.
        {
            int position = cur_left + cur_column - 1;
            int counter = 0;
            char *cur = get_line(cur_file, cur_top + cur_line - 1)->text;
            cur += cur_left + cur_column - 2;   //to be checked.
            if (is_word_end(cur))
            {
                position++;
                counter++;
                cur++;
            }
            for (; position < get_length(get_line(cur_file, cur_line + cur_top - 1)); position++, counter++)
            {
                if (is_word_end(cur++))
                {
                    break;
                }
            }
            for (; counter > 0; counter--)
            {
                cursor_right();
            }
        }
            break;

        case 'i':   //insert.
            mode_flag = INSERT_MODE;
            break;

        case 'a':   //append.
            cursor_right();
            mode_flag = INSERT_MODE;
            break;

        case 'd':   //delete a word.
        {
            v_line *this_line = get_line(cur_file, cur_top + cur_line - 1);
            unsigned int word_len = 0;
            unsigned int start_index = cur_left + cur_column - 2;
            unsigned int end_index = judge_word(this_line, start_index + 1);
            int i;

            word_len = end_index - start_index + 1;
            for (i = 0; i < end_index - start_index + 1; i++)
            {
                del_char(this_line, cur_left + cur_column - 2);
            }
            if (word_len != 0)
            {
                changed_flag = CHANGED;
            }
        }
            break;

        case 'x':   //delete a single char.
            del_char(get_line(cur_file, cur_top + cur_line - 1), cur_left + cur_column - 2);
            changed_flag = CHANGED;
            break;

        case 'o':   //open new line.
            insert_empty_line(cur_file, cur_top + cur_line - 1);
            cursor_down();
            mode_flag = INSERT_MODE;
            break;

        case '$':
            goto_line_end();
            break;

        case '^':   //goto first char of line.
            goto_line_actual_start();
            break;

        case '0':   //goto fixed first column.
            goto_line_start();
            break;

        case '3':
        case ':':   //bottom line command mode.
            //Now I wanna use this as open file command.
            if (changed_flag == CHANGED)
            {
                changed_flag = UNSAVED;
                break;
            }
            mode_flag = BOTTOMLINE_MODE;
            bottomline_sub_mode = BOTTOM_LINE_FILENAME_OPEN;
            break;

        case '/':    //bottom line and search.
            break;

        case '2':
        case 's':
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
            break;

        default:
            break;
    }
    return 0;
}


//functions below is the implements of moving the cursor.
//only should be called when its under normal or insert mode.
//return code:
//0: move failed.
//1: move success.
//-1: exceptions.
int cursor_left()
{
    unsigned int lines;
    lines = get_total_lines(cur_file);
    if (cur_column == 1 && cur_left == 1)
    {
        return 0;
    }
    cur_column--;
    if (!is_position_in_file())
    {
        if (cur_line + cur_top - 1 <= lines)
        {
            return 1;
        }
        cur_column++;
        return 0;
    }
    else if (cur_column > 1)
    {
        return 1;
    }
    else if (cur_left > 1)
    {
        cur_column++;
        roll_rightward(-1);
        return 1;
    }
    return -1;
}

int cursor_right()
{
    assert(cur_column <= screen_columns);
    ++cur_column;
    if (!is_position_in_file())
    {
        cur_column--;
        return 0;
    }
    else if (cur_column >= screen_columns)
    {
        --cur_column;
        roll_rightward(1);
        return 1;
    }
    else
    {
        return 1;
    }
}

int cursor_up()
{
    unsigned int length = 0;
    if (cur_line == 1 && cur_top == 1)
    {
        return 0;
    }
    else if (cur_line > 1)
    {
        cur_line--;
        length = (int) strlen((const char *) get_line(cur_file, cur_line + cur_top - 1));
//        length = (length == 0) ? 1 : length;
        if (length < cur_column)
        {
            cur_column = length + 1;
            cur_column = (cur_column == 0) ? 1 : cur_column;
        }
        return 1;
    }
    else if (cur_top > 1)
    {
        roll_downward(-1);
        return 1;
    }
    return -1;
}

//Have bugs in calc position.
// test data:
// cur_left     21
// cur_column   80
// length       0
//
//another group crash data:
//cur_left      1
//cur_column    1
//length        0

//return code:
//0: move failed.
//1: move success.
//-1: exceptions.
int cursor_down()
{
    unsigned int length = 0;
    unsigned int lines = 0;
//    unsigned int actual_column = 0;

    lines = get_total_lines(cur_file);
    cur_line++;

    if (cur_top + cur_line - 1 > lines)
    {
        cur_line--;
        return 0;
    }

    length = (unsigned int) strlen((const char *) get_line(cur_file, cur_line + cur_top - 1));
//    actual_column = cur_left + cur_column - 1;

    if (is_position_in_file())
    {
        if (cur_line > screen_lines)
        {
            cur_line--;
            roll_downward(1);
            return 1;
        }
        else
        {
            return 1;
        }
    }
    else    //is_position_in_file = false
    {
        if (cur_line + cur_top - 1 <= lines)   //not exceed the lines EOF
        {
            if (cur_line > screen_lines)
            {
                cur_line--;
                roll_downward(1);
            }

            // decline if the end of line outside the screen.
            if (length == 0)
            {
                cur_left = 1;
                cur_column = 1;
                return 1;
            }
            if (cur_left <= length)
            {
                cur_column = length - cur_left + 1 + 1;     //just modified, and extra 1.
                cur_column = (cur_column == 0) ? 1 : cur_column;
                return 1;
            }
            else
            {
                cur_column = screen_columns;
                cur_left = length - cur_column + 1;
                return 1;
            }
        }
        else    //exceed EOF lines
        {
            cur_line--;
            return 0;
        }
    }
}

int goto_line_end()
{
    unsigned int actual_column = 0;
    unsigned int length = 0;
    int i;

    length = (unsigned int) strlen((const char *) get_line(cur_file, cur_line + cur_top - 1));
    actual_column = cur_left + cur_column - 1;

    if (length > actual_column)
    {
        for (i = 0; i < length - actual_column + 1; i++)
        {
            cursor_right();
        }
    }
    else
    {
        for (i = 0; i < actual_column - length; i++)
        {
            cursor_left();
        }
    }
    return 0;
}

int goto_line_start()
{
    unsigned int actual_column = 0;
    int i;

    actual_column = cur_left + cur_column - 1;
    for (i = 0; i < actual_column - 1; i++)
    {
        cursor_left();
    }

    return 0;
}

int goto_line_actual_start()
{
    unsigned int actual_column = 0;
    unsigned int repeat;
    int i;
    char *pos;

    actual_column = cur_left + cur_column - 1;
    pos = get_line(cur_file, cur_top + cur_line - 1)->text;
    repeat = actual_column - 1;

    while (repeat > 0)
    {
        if (*pos++ == ' ')
        {
            repeat--;
        }
        else
        {
            break;
        }
    }
    for (i = 0; i < repeat; i++)
    {
        cursor_left();
    }
    return 0;
}


