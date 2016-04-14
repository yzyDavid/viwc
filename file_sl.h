//
// Created by yzy on 3/12/16.
//

#ifndef VIC_FILE_SL_H
#define VIC_FILE_SL_H

#include "file_str.h"

extern v_file_text *copied_part;

int v_load_file(const char *filename, v_file_text *);

int v_save_file(const char *filename, v_file_text *);

int v_new_file(v_file_text *file_struct);

int add_char(v_file_text *file_struct, unsigned int line, unsigned int position, char char_to_add);

int del_char(v_line *line, unsigned int index);

int v_free_file(v_file_text *file_struct);

int copy_selected();

int paste_copied();

int delete_copied();

#endif //VIC_FILE_SL_H
