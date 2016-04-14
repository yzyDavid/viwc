//
// Created by yzy on 3/12/16.
//

/*
 * 此文件中是读取和保存文件内容的函数，将文本文件和内存中的链表结构转化的过程在此文件中定义
 * 复制的部分作为另外一个链表在内存中保持，因此此文件中也包含几个操作复制粘贴的函数
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "file_sl.h"
#include "intmouse.h"
#include "ins_loop.h"
#include "draw_ui.h"
#include "n_loop.h"

v_file_text *copied_part = NULL;

//This program convert all the tabs to 4 spaces.

int v_load_file(const char *filename, v_file_text *file_struct)
{
    FILE *fp;
    char ch;
    v_line *cur_line;
    char *cur_char;
    int i_ch = 0;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return -1;
    }

//    file_struct = malloc(sizeof(v_file_text));
    file_struct->head = malloc(sizeof(v_line));
    cur_line = file_struct->head;
    cur_char = cur_line->text;
    cur_line->next = NULL;

    while ((i_ch = fgetc(fp)) != EOF)
    {
        ch = (char) i_ch;
        if (ch == '\r')
        {
            continue;
        }

        if (ch != '\n')
        {
            if (ch == '\t')
            {
                *cur_char = ' ';
                cur_char++;
                *cur_char = ' ';
                cur_char++;
                *cur_char = ' ';
                cur_char++;
                *cur_char = ' ';
                cur_char++;
            }
            else
            {
                *cur_char = ch;
                cur_char++;
            }
        }
        else
        {
            cur_line->next = malloc(sizeof(v_line));
            cur_line = cur_line->next;
            cur_line->next = NULL;
            memset(cur_line->text, 0, FILE_LINE_LENGTH);
            *cur_char = 0;
            cur_char = cur_line->text;
        }
    }

    fclose(fp);
    return 0;
}

int v_save_file(const char *filename, v_file_text *file_struct)
{
    FILE *fp;
    v_line *cur_line;
    char *cur_char;

    if (strlen(filename) == 0)
    {
        return -1;
    }

    if ((fp = fopen(filename, "w")) == NULL)
    {
        return -1;
    }

    cur_line = file_struct->head;
    cur_char = cur_line->text;
    do
    {
        while (*cur_char)
        {
            fputc(*cur_char, fp);
            cur_char++;
        }
        fputc('\n', fp);
        cur_line = cur_line->next;
        cur_char = cur_line->text;
    }
    while (NULL != cur_line);
    fclose(fp);
    return 0;
}

int v_new_file(v_file_text *file_struct)
{
    file_struct->head = malloc(sizeof(v_line));
    memset(file_struct->head->text, 0, FILE_LINE_LENGTH);
    memset(file_struct->head->info, 0, FILE_LINE_LENGTH);
    file_struct->head->next = NULL;

    return 0;
}

//This function do no safety checks!
//position start from 1.
//12340
int add_char(v_file_text *file_struct, unsigned int line, unsigned int position, char char_to_add)
{
    unsigned int length = 0;
    v_line *this_line = get_line(file_struct, line);
    int i;

    length = get_length(this_line);
//    length = (length == 0) ? 1 : length;
    for (i = ((length < LINE_LENGTH) ? length : LINE_LENGTH); i >= position; i--)
    {
        this_line->text[i] = this_line->text[i - 1];
    }
    this_line->text[position - 1] = char_to_add;
    return 0;
}

//delete a single char
//index relative to array index.
int del_char(v_line *line, unsigned int index)
{
    unsigned int length = 0;
    int i;

    length = get_length(line);
    for (i = index; i < length; i++)
    {
        line->text[i] = line->text[i + 1];
    }
    return 0;
}

//this func also freed the file_struct.
int v_free_file(v_file_text *file_struct)
{
    v_line *head = file_struct->head;
    v_line *cur = head;
    v_line *tmp = NULL;

    if (file_struct == NULL)    //Why condition always false?
    {
        return -1;
    }

    while (cur->next != NULL)
    {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    free(cur);
    free(file_struct);
    file_struct = NULL;
    return 0;
}

//this func copy the text between selected mark to copied part.
int copy_selected()
{
    int i;
    v_line *current;
    v_line *to_copy = get_line(cur_file, selected_start_line);

    if (selected_start_line == 0 || selected_start_column == 0 || selected_end_line == 0 || selected_end_column == 0)
    {
        return -1;
    }

    //delete copied text before.
    if (copied_part != NULL)
    {
        v_free_file(copied_part);
    }

    copied_part = malloc(sizeof(v_file_text));
    copied_part->head = malloc(sizeof(v_line));
    memset(copied_part->head->text, 0, FILE_LINE_LENGTH);
    memset(copied_part->head->info, 0, FILE_LINE_LENGTH);
    copied_part->head->next = NULL;
    current = copied_part->head;

    //processing the first line.
    if (selected_start_line == selected_end_line)
    {
        strncpy(current->text, to_copy->text + selected_start_column - 1,
                selected_end_column - selected_start_column + 1);
        return 0;
    }
    else
    {
        strcpy(current->text, to_copy->text + selected_start_column - 1);
        current->next = malloc(sizeof(v_line));
        current = current->next;
        current->next = NULL;
        to_copy = to_copy->next;
    }

    //processing middle part.
    for (i = 1; i < selected_end_line - selected_start_line; i++)
    {
        memset(current->text, 0, FILE_LINE_LENGTH);
        strcpy(current->text, to_copy->text);
        current->next = malloc(sizeof(v_line));
        current = current->next;
        current->next = NULL;
        to_copy = to_copy->next;
    }

    //to process the last line.
    memset(current->text, 0, FILE_LINE_LENGTH);
    strncpy(current->text, to_copy->text, selected_end_column);

    return 0;
}

//this func paste the copied part at the cursor position.
int paste_copied()
{
    unsigned int copied_lines = 0;
    unsigned int actual_column = 0;
    unsigned int length = 0;
    unsigned int i = 0;
    v_line *to_paste_in = NULL;
    v_line *current = NULL;
    v_line *current_copied = NULL;
    v_line *start = NULL;
    v_line *end = NULL;
    v_line *tmp = NULL;
    char *cache = NULL;

    if(copied_part == NULL)
    {
        return 0;
    }
    changed_flag = CHANGED;
    copied_lines = get_total_lines(copied_part);
    to_paste_in = get_line(cur_file, cur_top + cur_line - 1);
    actual_column = cur_left + cur_column - 1;

    if (copied_lines == 0)
    {
        //don't assert 0 here, copied part may be empty.
        return -1;
    }
    else if (copied_lines == 1)
    {
        current = get_line(copied_part, 1);
        length = get_length(current);
        for (i = length; i > 0; i--)
        {
            add_char(cur_file, cur_top + cur_line - 1, actual_column, current->text[i - 1]);
            cursor_right();
        }
        return 0;
    }
    else if (copied_lines > 1)
    {
        //first line.
        cache = malloc(sizeof(char) * LINE_LENGTH);
        memset(cache, 0, LINE_LENGTH);
        length = get_length(to_paste_in);
        strncpy(cache, to_paste_in->text + actual_column - 1, length - actual_column + 1);
        memset(to_paste_in->text + actual_column - 1, 0, LINE_LENGTH - actual_column);
        tmp = to_paste_in->next;
        current = to_paste_in;
        current_copied = get_line(copied_part, 1);
        length = get_length(current_copied);
        strncpy(current->text + actual_column - 1, current_copied->text, length);

        //middle part.
        for (i = 1; i < copied_lines; i++)
        {
            current_copied = current_copied->next;
            current->next = malloc(sizeof(v_line));
            current = current->next;
            assert(current != NULL);
            current->next = NULL;
            memset(current->text, 0, LINE_LENGTH);
            strcpy(current->text, current_copied->text);
            cursor_down();
        }

        //connect last line.
        strcat(current->text, cache);
        current->next = tmp;
        return 0;
    }
    return -1;
}

//useless now.
int delete_copied()
{
    v_free_file(copied_part);

    selected_start_line =
    selected_start_column =
    selected_end_column =
    selected_end_line = 0;

    return 0;
}
