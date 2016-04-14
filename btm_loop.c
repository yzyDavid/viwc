//
// Created by yzy on 3/18/16.
//

/*
 * 底行模式循环的代码，负责处理底行模式的键盘输入并且维护此行的内容
 * 底行仅用于书写文件名，包括打开文件和关闭文件的文件名，这样可以避免对主体部分显示文件内容的影响
 */

#include <string.h>

#include "btm_loop.h"
#include "draw_ui.h"
#include "file_str.h"
#include "m_loop.h"
#include "highlit.h"
#include "file_sl.h"
#include "ins_loop.h"

int bottomline_sub_mode = BOTTOM_LINE_EXCEPT;

int bottomline_mode_process(int key_down)
{
    unsigned int text_length = 0;
    int exist_flag = 0;
    char *i;
    text_length = strlen((const char *) bottomline_text);
    switch (key_down)
    {
        case '\x00':    //special keys.
            break;

        case '\x0d':    //enter in dos.
            switch (bottomline_sub_mode)
            {
                case BOTTOM_LINE_FILENAME_SAVE:
                    strcpy(cur_file_name, (const char *) bottomline_text);
                    mode_flag = INSERT_MODE;
                    memset(bottomline_text, 0, CONSOLE_COLUMNS + 1);
                    break;

                case BOTTOM_LINE_FILENAME_OPEN:
                    strcpy(cur_file_name, (const char *) bottomline_text);
                    mode_flag = INSERT_MODE;
                    v_load_file(cur_file_name, cur_file);
                    determine_file_type(cur_file_name);
                    memset(bottomline_text, 0, CONSOLE_COLUMNS + 1);
                    break;

                default:
                    mode_flag = INSERT_MODE;
                    memset(bottomline_text, 0, CONSOLE_COLUMNS + 1);
                    break;
            }
            break;

        case '\x1b':    //Esc.
            mode_flag = INSERT_MODE;
            memset(bottomline_text, 0, CONSOLE_COLUMNS + 1);
            break;

        case '\x08':    //backspace in windows.
            bottomline_text[text_length - 1] = 0;
            break;

        default:
            if (bioskey(2) & CTRL)
            {
                break;
            }

            for (i = word_char_list; *i != 0; i++)
            {
                if (key_down == *i)
                {
                    exist_flag = 1;
                }
            }
            if (key_down == '.' || key_down == '-')
            {
                exist_flag = 1;
            }

            if (!exist_flag)
            {
                return -1;
            }

            if (text_length < (8 + 1 + 3))
            {
                bottomline_text[text_length] = (char) key_down;
            }
            else
            {
                return -1;
            }
            break;
    }
    return 0;
}

