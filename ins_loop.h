//
// Created by yzy on 3/13/16.
//

#ifndef VIC_INSERT_LOOP_H
#define VIC_INSERT_LOOP_H

#define RIGHT_SHIFT 0x01
#define LEFT_SHIFT 0x02

#define CTRL 0x04

#define SHIFT (RIGHT_SHIFT | LEFT_SHIFT)

//relative to file actual lines and columns.
extern volatile unsigned int selected_start_line;
extern volatile unsigned int selected_start_column;
extern volatile unsigned int selected_end_line;
extern volatile unsigned int selected_end_column;

int insert_mode_process(int key_down);

int reverse_selected();

int cancel_select();

int set_start_select_pos();

int set_end_select_pos();

int init_ins_loop_func();

#endif //VIC_INSERT_LOOP_H
