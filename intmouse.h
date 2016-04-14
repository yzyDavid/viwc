#ifndef VIC_INTMOUSE_H
#define VIC_INTMOUSE_H

#include "graphics.h"

#define MOUSE_SIZE               16   // 16*16点
#define MOUSE_MOVE               0x01
#define MOUSE_LEFT_BUTTON_DOWN   0x02
#define MOUSE_LEFT_BUTTON_UP     0x04
#define MOUSE_RIGHT_BUTTON_DOWN  0x08
#define MOUSE_RIGHT_BUTTON_UP    0x10
#define MOUSE_MIDDLE_BUTTON_DOWN 0x20
#define MOUSE_MIDDLE_BUTTON_UP   0x40

#define LAST_LEFT_DOWN 1
#define LAST_LEFT_UP 2

typedef struct __mouse_type
{
    int x, y;
    struct picture *old_img;
    struct picture *mask;
    struct picture *img;
} mouse_type;

extern void (far
*old_mouse_event_handler)(
int event,
int x,
int y
);

//functions and variables defined below may be useless outside the intmouse.c

extern word screen_mask[];
extern word cursor_mask[];

int init_mouse(void);

void draw_mouse(int x, int y);

void recover_mouse_background(void);

void save_mouse_background(int x, int y);

void move_mouse(int x, int y);

void close_mouse(void);

int px2charpos(int x, int y, int *x_out, int *y_out);

int on_left_up(int event, int x, int y);

int on_left_down(int event, int x, int y);

void expand_mask_to_image(word m[16], byte *p, byte color, int width, int height);

void build_mouse_image(void);

void
        __loadds
        far mouse_event_handler(int event, int x, int y);


#endif
