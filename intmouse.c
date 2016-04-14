//rewrite this file to check bug.

/*
 * 关于绘制鼠标的内容，以及鼠标中断的回调函数。
 * 目前由于库函数实现不标准的原因，调用其他工具函数不能工作，导致功能未实现
 * 仍保留了显示光标指针的功能，可以用于指示，但是未实现光标选取的功能。
 */

#include <graphics.h>
#include "draw_ui.h"
#include "ins_loop.h"

static char log_str[120];

void (far
*old_mouse_event_handler)(
int event,
int x,
int y
);

void __loadds
        far mouse_event_handler(int event, int x, int y)
{
#pragma aux mouse_event_handler parm [EAX] [ECX] [EDX];

    extern dword _x386_stacklow;
    static byte stack[0x400];
    static word old_stack[3];
    static word new_stack[3];

    static int last_state = 0;

    __asm
    {

    }

    if (event & MOUSE_MOVE)
    {
        if (left_button_hold_down)
        {
            recover_mouse_background();
            save_mouse_background(x, y);
            is_moved = 1;
        }
        move_mouse(x, y);
    }
    else if (event & MOUSE_LEFT_BUTTON_DOWN)
    {
        is_moved = 0;
        if (last_state == LAST_LEFT_UP)
        {
            on_left_down(event, x, y);
        }
        last_state = LAST_LEFT_DOWN;
    }
    else if (event & MOUSE_LEFT_BUTTON_UP)
    {
        if (last_state == LAST_LEFT_DOWN)
        {
            on_left_up(event, x, y);
            /*
            memset(log_str, 0, sizeof(log_str));
            sprintf(log_str, "%d\n%d\n\n%d\n%d\n\n", selected_start_line, selected_start_column, selected_end_line,
                    selected_end_column);
            print_log(log_str);
             */
        }
        last_state = LAST_LEFT_UP;
    }

    __asm
    {

    }
}

//use mask array to build a image with given color?
void expand_mask_to_image(word m[16], byte *p, byte color, int width, int height)
{
    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (m[i] & 1 << (width - 1 - j))
                p[i * width + j] = color;
            else
                p[i * width + j] = 0x00;
        }
    }
}

void build_mouse_image(void)
{
    mouse.old_img = malloc(imagesize(0, 0, MOUSE_SIZE - 1, MOUSE_SIZE - 1));
    mouse.mask = malloc(imagesize(0, 0, MOUSE_SIZE - 1, MOUSE_SIZE - 1));
    mouse.img = malloc(imagesize(0, 0, MOUSE_SIZE - 1, MOUSE_SIZE - 1));
    mouse.mask->picwidth = mouse.mask->picheight = MOUSE_SIZE;
    mouse.img->picwidth = mouse.img->picheight = MOUSE_SIZE;
    expand_mask_to_image(screen_mask, (byte *) &mouse.mask->buffer, 0xFF, MOUSE_SIZE, MOUSE_SIZE);
    expand_mask_to_image(cursor_mask, (byte *) &mouse.img->buffer, WHITE, MOUSE_SIZE, MOUSE_SIZE);
}

void save_mouse_background(int x, int y)
{
    getimage(x, y, x + MOUSE_SIZE - 1, y + MOUSE_SIZE - 1, mouse.old_img);
    mouse.x = x;
    mouse.y = y;
}

void draw_mouse(int x, int y)
{
    putimage(x, y, mouse.mask, AND_PUT);
    putimage(x, y, mouse.img, OR_PUT);
}

void recover_mouse_background(void)
{
    putimage(mouse.x, mouse.y, mouse.old_img, COPY_PUT);
}

void move_mouse(int x, int y)
{
    recover_mouse_background();
    save_mouse_background(x, y);
    draw_mouse(x, y);
}

int init_mouse(void)
{
    int x = _width / 2, y = _height / 2;
    union REGS r;
    struct SREGS sr;

    r.w.ax = 0;      /* check mouse driver */
    int86(0x33, &r, &r);
    if (r.w.ax != 0xFFFF)
        return 0;

    r.w.ax = 0x0007; /* set horizontal range */
    r.w.cx = 0;
    r.w.dx = _width - 1;
    int86(0x33, &r, &r);
    r.w.ax = 0x0008; /* set vertical range */
    r.w.cx = 0;
    r.w.dx = _height - 1;
    int86(0x33, &r, &r);

    r.w.ax = 0x0004; /* position cursor */
    r.w.cx = x;
    r.w.dx = y;
    int86(0x33, &r, &r);

    r.w.ax = 2;      /* hide cursor */
    int86(0x33, &r, &r);

    r.w.ax = 0x0014; /* install mouse event handler */
    r.w.cx = MOUSE_MOVE | MOUSE_LEFT_BUTTON_DOWN | MOUSE_LEFT_BUTTON_UP |
             MOUSE_RIGHT_BUTTON_DOWN | MOUSE_RIGHT_BUTTON_UP; /* monitor these events */
    r.x.edx = FP_OFF(mouse_event_handler);
    memset(&sr, 0, sizeof(sr));
    sr.es = FP_SEG(mouse_event_handler);
    int86x(0x33, &r, &r, &sr);
    old_mouse_event_handler = MK_FP(sr.es, r.x.edx);

    build_mouse_image();
    save_mouse_background(x, y);
    move_mouse(x, y);

    return 1;
}

void close_mouse(void)
{
    union REGS r;
    struct SREGS sr;
    r.w.ax = 0x0014; /* install mouse event handler */
    r.w.cx = 0;      // DO NOT monitor any events!!!
    r.x.edx = FP_OFF(old_mouse_event_handler);
    memset(&sr, 0, sizeof(sr));
    sr.es = FP_SEG(old_mouse_event_handler);
    int86x(0x33, &r, &r, &sr);
}

//This function calc the pixel position to char position.
//return 0 for normal exit.
//out position is relative to screen, not the text area.
int px2charpos(int x, int y, int *x_out, int *y_out)
{
    *x_out = (x / 8) + 1;
    *y_out = (y / 16) + 1;
    return 0;
}

int on_left_down(int event, int x, int y)
{
    int x_out = 0;
    int y_out = 0;

    px2charpos(x, y, &x_out, &y_out);
    selected_start_column = cur_left + x_out - 1;
    selected_start_line = cur_top + y_out - 2 - 1;

    selected_end_column = 0;
    selected_end_line = 0;
    return 0;
}


int on_left_up(int event, int x, int y)
{
    int x_out = 0;
    int y_out = 0;

    px2charpos(x, y, &x_out, &y_out);
    selected_end_column = cur_left + x_out - 1;
    selected_end_line = cur_top + y_out - 2 - 1;

    if (!is_moved)
    {
        selected_start_line =
        selected_start_column =
        selected_end_column =
        selected_end_line = 0;
    }

    left_button_hold_down = 0;

    reverse_selected();

    //feels like this part of code would be executed many times while mouse down.

    //why this part of code result in crash??!!!

//    redraw_ui();

    return 0;
}
