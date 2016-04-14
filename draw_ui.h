//
// Created by yzy on 3/12/16.
//

/*
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
 */

#ifndef VIC_DRAW_UI_H
#define VIC_DRAW_UI_H

#include "main.h"
#include "intmouse.h"

//constants below are default lines or columns of the console.
//#define CONSOLE_LINES 25

//windows console and standard dos screen is 25
//linux console is 24
//while the d**n dosbox screen is remain to see.
//#define CONSOLE_COLUMNS 80

//belows are data for 1024x768 graphics mode under DOS.
#define CONSOLE_LINES 48

#define CONSOLE_COLUMNS 128

extern mouse_type mouse;

extern int left_button_hold_down;
extern int is_moved;


extern word screen_mask[];
extern word cursor_mask[];

extern unsigned int screen_lines;
extern unsigned int screen_columns;

extern unsigned int console_lines;
extern unsigned int console_columns;

extern char title_bar[CONSOLE_COLUMNS + 1];
extern char menu_bar[CONSOLE_COLUMNS + 1];
extern char status_bar[CONSOLE_COLUMNS + 1];
extern char bottomline_text[CONSOLE_COLUMNS + 1];

//defines default lines and columns for text area.
#define SCREEN_LINES (CONSOLE_LINES - 3)
#define SCREEN_COLUMNS CONSOLE_COLUMNS

#define NORMAL "NORMAL"
#define INSERT "INSERT"
#define SAVE_WARN "!SAVE!"
#define REWRIT "REWRIT"

int gen_status_bar(char *status_bar);

int roll_rightward(int repeat);

int roll_downward(int repeat);


int set_cursor_pos(unsigned int x, unsigned int y);

int print_cursor();

extern volatile unsigned int cur_line;
extern volatile unsigned int cur_column;

extern volatile unsigned int cur_top;
extern volatile unsigned int cur_left;

int redraw_ui();

#endif //VIC_DRAW_UI_H

/*
#pragma clang diagnostic pop
 */

