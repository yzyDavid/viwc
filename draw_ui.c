//
// Created by yzy on 3/12/16.
//

/*
 * 生成状态栏，标题栏内容并且显示屏幕内容的文件
 * 最主要的redraw_ui函数每次调用时重绘整个屏幕的内容，效率问题明显，但是容易实现，以后应当考虑替换为部分重绘的方式
 * 也定义了关键的表示当前显示部分的位置和当前光标相对于屏幕的位置的函数
 * 也定义了关于光标的数据结构。
 */

#include <stdio.h>
#include <string.h>
#include <graphics.h>

#include "draw_ui.h"
#include "file_str.h"
#include "m_loop.h"
#include "btm_loop.h"
#include "ins_loop.h"

//This globals defines the position of "cursor".
//relative to the line3-23 (actually contains the file) part.
volatile unsigned int cur_line = 1;
volatile unsigned int cur_column = 1;

//This globals defines the position of the screen.
//relative to the file.
volatile unsigned int cur_top = 1;
volatile unsigned int cur_left = 1;

mouse_type mouse;

//The lines and columns of the console window.
unsigned int console_lines = CONSOLE_LINES;
unsigned int console_columns = CONSOLE_COLUMNS;

//The lines and columns of the text area.
unsigned int screen_lines = SCREEN_LINES;
unsigned int screen_columns = SCREEN_COLUMNS;

char title_bar[CONSOLE_COLUMNS + 1];
char menu_bar[CONSOLE_COLUMNS + 1];
char status_bar[CONSOLE_COLUMNS + 1];
char status_bar_template[CONSOLE_COLUMNS + 1];

//The string length should be checked by bottomline mode loop.
char bottomline_text[CONSOLE_COLUMNS + 1];

char log_str[120];

int left_button_hold_down = 0;
int is_moved;

word screen_mask[] =
        {
                /* 16 words of screen mask */
                0x3FFF, /*0011111111111111*/
                0x1FFF, /*0001111111111111*/
                0x0FFF, /*0000111111111111*/
                0x07FF, /*0000011111111111*/
                0x03FF, /*0000001111111111*/
                0x01FF, /*0000000111111111*/
                0x00FF, /*0000000011111111*/
                0x007F, /*0000000001111111*/
                0x01FF, /*0000000111111111*/
                0x10FF, /*0001000011111111*/
                0xB0FF, /*1011000011111111*/
                0xF87F, /*1111100001111111*/
                0xF87F, /*1111100001111111*/
                0xFC3F, /*1111110000111111*/
                0xFC3F, /*1111110000111111*/
                0xFE1F, /*1111111000011111*/
        };

word cursor_mask[] =
        {
                /* 16 words of cursor mask */
                0x0000, /*0000000000000000*/
                0x4000, /*0100000000000000*/
                0x6000, /*0110000000000000*/
                0x7000, /*0111000000000000*/
                0x7800, /*0111100000000000*/
                0x7C00, /*0111110000000000*/
                0x7E00, /*0111111000000000*/
                0x7F00, /*0111111100000000*/
                0x7C00, /*0111110000000000*/
                0x4600, /*0100011000000000*/
                0x0600, /*0000011000000000*/
                0x0300, /*0000001100000000*/
                0x0300, /*0000001100000000*/
                0x0180, /*0000000110000000*/
                0x0180, /*0000000110000000*/
                0x00C0, /*0000000011000000*/
        };


int redraw_ui()
{
    int i, j;
    v_line *drawing_line = NULL;
    unsigned int lines = get_total_lines(cur_file);
    unsigned int length = 0;

    gotoxy(1, 1);

    gen_status_bar(status_bar);

    parse_highlighting(cur_file);

    //drawing title line.
    // line 1:
    printf("%-127s\n", title_bar);

    //menu bar.
    // line 2:
    printf("%-127s\n", menu_bar);

    //main text part.
    //line 3 to line 47.
    for (i = 0; i < SCREEN_LINES; i++)
    {
        drawing_line = get_line(cur_file, cur_top + i);
        if (drawing_line == NULL)
        {
            length = 0;
        }
        else
        {
            length = get_length(drawing_line);
        }
        for (j = 0; j < SCREEN_COLUMNS; j++)
        {
            if (cur_left + j > length)
            {
                printf(" ");
            }
            else
            {
                printf("%c", drawing_line->text[cur_left + j - 1]);
            }
        }
    }

    //status bar.
    // line 48:
    printf("%-127s\n", status_bar);

    //paint the selected part here.
    if (selected_end_column != 0 && selected_end_line != 0 && selected_start_column != 0 && selected_start_line != 0)
    {
        for (i = 0; i < SCREEN_LINES; i++)
        {
            drawing_line = get_line(cur_file, cur_top + i);
            if (drawing_line == NULL)
            {
                break;
            }
            length = get_length(drawing_line);
            for (j = 0; j < SCREEN_COLUMNS; j++)
            {
                if (cur_left + j - 1 > length)
                {
                    //there isn't char, continue.
                    continue;
                }
                else
                {
                    if (cur_top + i > selected_start_line && cur_top + i < selected_end_line)
                    {
                        putimage(j * 8, (i + 2) * 16, white_buffer, XOR_PUT);
                        continue;
                    }
                    if (cur_top + i == selected_start_line && cur_left + j >= selected_start_column)
                    {
                        if (selected_start_line != selected_end_line)
                        {
                            putimage(j * 8, (i + 2) * 16, white_buffer, XOR_PUT);
                        }
                        else if (cur_left + j <= selected_end_column)
                        {
                            putimage(j * 8, (i + 2) * 16, white_buffer, XOR_PUT);
                        }
                        continue;
                    }
                    if (cur_top + i == selected_end_line && cur_left + j <= selected_end_column)
                    {
                        if (selected_start_line != selected_end_line)
                        {
                            putimage(j * 8, (i + 2) * 16, white_buffer, XOR_PUT);
                        }
                        else if (cur_left + j >= selected_start_column)
                        {
                            putimage(j * 8, (i + 2) * 16, white_buffer, XOR_PUT);
                        }
                        continue;
                    }
                }
            }
        }
    }

    print_cursor();

    return -1;
}

//Also generate title bar and menu bar here.
int gen_status_bar(char *status_bar)
{
    unsigned int a_line = 0;
    unsigned int a_column = 0;

    a_line = cur_top + cur_line - 1;
    a_column = cur_left + cur_column - 1;

    memset(title_bar, 0, CONSOLE_COLUMNS + 1);
    memset(status_bar, 0, CONSOLE_COLUMNS + 1);
    strcpy(title_bar, "Dos Text - ");
    if (cur_file_name[0] != 0)
    {
        strcat(title_bar, cur_file_name);
    }
    else
    {
        strcat(title_bar, "New File");
    }
    strcpy(menu_bar, "New [C-N]   Open [C-O]   Save [C-S]         Exit [C-Q]");

    strcpy(status_bar_template,
           "status: Line: %4u Column: %4u                        -- %s --");

    if (changed_flag == UNSAVED)
    {
        sprintf(status_bar, status_bar_template, a_line, a_column, SAVE_WARN);
        return 0;
    }

    switch (mode_flag)
    {
        case NORMAL_MODE:
            //This usage is dangerous!!
            sprintf(status_bar, status_bar_template, a_line, a_column, NORMAL);
            break;

        case INSERT_MODE:
            sprintf(status_bar, status_bar_template, a_line, a_column, INSERT);
            break;

        case REWRITE_MODE:
            sprintf(status_bar, status_bar_template, a_line, a_column, REWRIT);
            break;


        case BOTTOMLINE_MODE:
            switch (bottomline_sub_mode)
            {
                case BOTTOM_LINE_FILENAME_SAVE:
                    sprintf(status_bar, ":%s", bottomline_text);
                    break;

                case BOTTOM_LINE_FILENAME_OPEN:
                    sprintf(status_bar, ":%s", bottomline_text);
                    break;

                case BOTTOM_LINE_SEARCH:
                    sprintf(status_bar, "/%s", bottomline_text);
                    break;

                default:
                    sprintf(status_bar, "%s", bottomline_text);
                    break;
            }
        default:
            break;
    }
    return 0;
}

//repeat can be assigned to minus number to perform rolling left.
//return -1 for error.
//return number of units actually rolled.
int roll_rightward(int repeat)
{
    cur_left += repeat;
    return -1;
}

int roll_downward(int repeat)
{
    cur_top += repeat;
    return -1;
}

//dummy
int set_cursor_pos(unsigned int x, unsigned int y)
{
    return -1;
}

int print_cursor()
{
    int x = (cur_column - 1) * 8;
    int y = (cur_line + 2) * 16;
    line(x, y, x + 8, y);
    line(x, y - 1, x + 8, y - 1);
    return -1;
}
